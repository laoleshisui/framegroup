#include "FrameGroup.h"

#include <AProto/pframe/frame.pb.h>

using namespace aoles;

FrameGroup::FrameGroup()
:id_(0)
{
    Core::SocketAddress addr("192.168.0.105", 10001);
    Core::Server::MSG_FUNC recv_cb = std::bind(&FrameGroup::RecvCB, this, std::placeholders::_1, std::placeholders::_2);
    client_.RegisterReadCallBack(std::make_shared<Core::Server::MSG_FUNC>(recv_cb));
    Core::Server::EVENT_FUNC event_cb = std::bind(&FrameGroup::EventCB, this, std::placeholders::_1, std::placeholders::_2);
    client_.RegisterEventCallBack(std::make_shared<Core::Server::EVENT_FUNC>(event_cb));
    client_.Connect(addr);
    
    client_loop_ = std::thread(&Core::Client::Loop, &client_);
    client_loop_.detach();
}

FrameGroup::~FrameGroup()
{
}

void FrameGroup::Login(){
    pframe::Login login;
    login.set_proto_type(pframe::ProtoType::LOGIN);
    client_.SendToServer(login.SerializeAsString());
}

void FrameGroup::AddCapturer(uint64_t local_id, FrameCapturer* capturer){
    //local id will be replaced by remote id soon.
    captured_objects_id_.insert(local_id);
    frame_objects_.emplace(local_id, FrameObject());
    capturer->AddSink(&(frame_objects_[local_id]));
}

void FrameGroup::RegisterCaptureredOnServer(){
    if(!id_){
        return;
    }

    pframe::RegisterObjects register_objects;
    register_objects.set_group_id(id_);
    register_objects.set_proto_type(pframe::ProtoType::REGISTER_OBJECTS);
    for (const uint64_t& local_id : captured_objects_id_){
        register_objects.add_ids(local_id);
    }
    
    client_.SendToServer(register_objects.SerializeAsString());
}

void FrameGroup::AddRender(uint64_t remote_id, FrameRender* render){
    frame_objects_[remote_id].AddSink(render);
}

void FrameGroup::InitFrameObjects(){
    //TODO fill frame_objects_ & captured_objects_id_ & uncaptured_objects_id_
    //Maybe from server data
    for(CORE_MAP<uint64_t, FrameObject>::value_type& i : frame_objects_){
        i.second.id_ = i.first;
        i.second.SendPacket = std::bind(&FrameGroup::SendPacket, this, i.first, std::placeholders::_1);
    }
}

void FrameGroup::SendPacket(uint64_t id, std::shared_ptr<PacketItf> packet){
    if(packet->data_.empty()){
        return;
    }

    pframe::Frame frame;
    frame.set_proto_type(pframe::ProtoType::FRAME);
    frame.set_id(id);
    frame.set_data(std::move(packet->data_));

    uint16_t proto_size = frame.ByteSizeLong();
    if(proto_size <= MAX_SEND_BUF_LEN){
        //no extra copy
        memcpy(client_.send_buf_.HeadPtr(), &proto_size, PACKET_HEAD_SIZE);
        frame.SerializeToArray(client_.send_buf_.DataPtr(), proto_size);
        client_.SendToServer(client_.send_buf_.HeadPtr() , proto_size + PACKET_HEAD_SIZE);
    }
    else{
        client_.SendToServer(frame.SerializeAsString());
    }
}

void FrameGroup::RecvCB(Core::Server::Client* client, const char* msg){
    const char* data = msg + PACKET_HEAD_SIZE;
    uint16_t* len = (uint16_t*)msg;

    pframe::Relay relay;
    relay.ParseFromArray(data, *len);

    if(relay.proto_type() == pframe::ProtoType::FRAME){
        pframe::Frame frame;
        frame.ParseFromArray(data, *len);

        uint64_t object_id = frame.id();
        CORE_MAP<uint64_t, FrameObject>::iterator it = frame_objects_.find(object_id);
        if (it != frame_objects_.end()){
            std::shared_ptr<PacketItf> packet = std::make_shared<PacketItf>();
            packet->data_ = std::move(frame.data());
            it->second.OnPacket(packet);
        }
        EffectCaculate(object_id);
    }
    else if(relay.proto_type() == pframe::ProtoType::EVENT){
        pframe::Event event;
        event.ParseFromArray(data, *len);
        
        if(event.code() == pframe::EventCode::LOGIN_SUCCEED){
            id_ = event.id();
        }
        else if(event.code() == pframe::EventCode::LOGIN_FAILED){
            id_ = 0;
        }
        else if(event.code() == pframe::EventCode::REGISTERED_OBJECT){
            pframe::RegisterObjects registered_objects;
            registered_objects.ParseFromString(std::move(event.data()));
            
            CORE_SET<uint64_t> registered_objects_id;
            int i = 0;
            for (const uint64_t& local_id : captured_objects_id_){
                uint64_t remote_id = registered_objects.ids(i++);
                CORE_MAP<uint64_t, FrameObject>::node_type pair = frame_objects_.extract(local_id);
                pair.key() = remote_id;
                frame_objects_.insert(std::move(pair));

                registered_objects_id.insert(remote_id);
                if(OnUpdateCapturedLocalId){
                    OnUpdateCapturedLocalId(local_id, remote_id);
                }
            }
            captured_objects_id_.swap(registered_objects_id);
        }
        else if(event.code() == pframe::EventCode::UNREGISTERED_OBJECTS){
            pframe::RegisterObjects registered_objects;
            registered_objects.ParseFromString(std::move(event.data()));

            for(const uint64_t& uncaptured_id : registered_objects.ids()){
                uncaptured_objects_id_.insert(uncaptured_id);
                frame_objects_.emplace(uncaptured_id, FrameObject());
                if(OnUpdateUncapturedRemoteId){
                    OnUpdateUncapturedRemoteId(uncaptured_id);
                }
            }
        }
    }
}
void FrameGroup::EventCB(Core::Server::Client* client, const Core::NET_EVENT event){

}


void FrameGroup::EffectCaculate(uint64_t object_id){
    if(captured_objects_id_.find(object_id) != captured_objects_id_.end()){
        // local frame added
        for(const CORE_SET<uint64_t>::value_type& id : uncaptured_objects_id_){
            ReviseEffect(&(frame_objects_[object_id]), frame_objects_[object_id].local_frames_.end()->get(), &(frame_objects_[id]), frame_objects_[id].remote_frames_.end()->get());
        }
    }else{
        // remote frame received
        // If the network is fine, there is no influence. The main situation is a bad network between CS 
        for(const CORE_SET<uint64_t>::value_type& id : captured_objects_id_){
            for (std::vector<std::shared_ptr<FrameItf>>::reverse_iterator it = frame_objects_[id].local_frames_.rbegin();
                it != frame_objects_[id].local_frames_.rend();
                it++){
                if((*it)->idx_ == (*(frame_objects_[object_id].remote_frames_.end()))->idx_){
                    ReviseEffect(&(frame_objects_[id]), it->get(), &(frame_objects_[object_id]), frame_objects_[object_id].remote_frames_.end()->get());
                }
                else if((*it)->idx_ < (*(frame_objects_[object_id].remote_frames_.end()))->idx_){
                    break;
                }
                
            } 
        }
    }
}


void FrameGroup::ReviseEffect(FrameObject* decider, FrameItf* decider_frame, FrameObject* other, FrameItf* other_frame){
    // TODO must judge whether the local frame at this idx has effected this object. If have done, break.
    CORE_MAP<uint64_t, CORE_SET<uint64_t>>::iterator effected_pair = decider->effected_map_.find(decider_frame->idx_);
    if(effected_pair != decider->effected_map_.end()){
        if(effected_pair->second.find(other->id_) != effected_pair->second.end()){
            effected_pair->second.insert(other->id_);
        }else{
            return;
        }
    }else{
        CORE_SET<uint64_t> set = {other->id_};
        decider->effected_map_.emplace(decider_frame->idx_, std::move(set));
    }

    //TODO effect now!
    //Attach effect in the nearest Frame
}