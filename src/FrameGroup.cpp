#include "FrameGroup.h"

#include "FrameObject.h"
#include <acore/utils/MapUtils.h>

#include <aproto/pframe/frame.pb.h>

using namespace framegroup;

FrameGroup::FrameGroup()
:id_(0),
time_controller_(std::make_unique<FrameTimeController>()),
client_(6),
OnUpdateId(nullptr),
OnLogin(nullptr),
OnEffect(nullptr),
save_frame_file_task_pool_(1),
save_frame_file_(NULL)
{
    
    acore::Server::MSG_FUNC recv_cb = std::bind(&FrameGroup::RecvCB, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    client_.RegisterReadCallBack(std::make_shared<acore::Server::MSG_FUNC>(recv_cb));
    acore::Server::EVENT_FUNC event_cb = std::bind(&FrameGroup::EventCB, this, std::placeholders::_1, std::placeholders::_2);
    client_.RegisterEventCallBack(std::make_shared<acore::Server::EVENT_FUNC>(event_cb));
}

FrameGroup::~FrameGroup()
{
    if(save_frame_file_){
        fflush(save_frame_file_);
        fclose(save_frame_file_);
    }
}

void FrameGroup::Connect(std::string ip, int port){
    acore::SocketAddress addr(std::move(ip), port);
    client_.Connect(addr);
    client_loop_ = std::thread(&acore::Client::Loop, &client_);
    client_loop_.detach();
}

void FrameGroup::Login(){
    pframe::Login login;
    login.set_proto_type(pframe::ProtoType::LOGIN);
    client_.Send(client_.client_bev_, login.SerializeAsString());
}
void FrameGroup::EnterRoom(uint64_t room_id){
    assert(id_);
    pframe::EnterRoom enter_room;
    enter_room.set_proto_type(pframe::ProtoType::ENTER_ROOM);
    enter_room.set_group_id(id_);
    enter_room.set_room_id(room_id);
    
    client_.Send(client_.client_bev_, enter_room.SerializeAsString());
}
void FrameGroup::ExitRoom(uint64_t room_id){
    assert(id_);
    pframe::ExitRoom exit_room;
    exit_room.set_proto_type(pframe::ProtoType::EXIT_ROOM);
    exit_room.set_group_id(id_);
    exit_room.set_room_id(room_id);
    
    client_.Send(client_.client_bev_, exit_room.SerializeAsString());
}

void FrameGroup::SetSaveFrameFilePath(std::string file_path){
    save_frame_file_ = fopen(file_path.c_str(), "wb+");
}

void FrameGroup::AddCaptureredObjects(int num_of_objects){
    assert(id_);
    if(!id_ || !num_of_objects){
        return;
    }

    pframe::RegisterObjects register_objects;
    register_objects.set_group_id(id_);
    register_objects.set_proto_type(pframe::ProtoType::REGISTER_OBJECTS);
    register_objects.set_num_of_objects(num_of_objects);

    client_.Send(client_.client_bev_, register_objects.SerializeAsString());
}

void FrameGroup::AddCapturer(uint64_t remote_id, FrameCapturer* capturer){
    capturer->AddSink(frame_objects_[remote_id].get());
    frame_capturers_[remote_id] = capturer;
    capturer->AttachTimeController(time_controller_.get());
}

void FrameGroup::AddRender(uint64_t remote_id, FrameRender* render){
    frame_objects_[remote_id]->AddSink(render);
}

void FrameGroup::InitCapturedFrameObjects(){
    //TODO fill frame_objects_ & captured_objects_id_ & uncaptured_objects_id_
    //Maybe from server data
    for(CORE_MAP<uint64_t, std::unique_ptr<FrameObject>>::value_type& i : frame_objects_){
        i.second->SendPacket = std::bind(&FrameGroup::SendPacket, this, i.first, std::placeholders::_1);
    }
}

void FrameGroup::SendPacket(uint64_t object_id, std::shared_ptr<PacketItf> packet){
    assert(id_);

    if(packet->data_.empty()){
        return;
    }

    pframe::Frame frame;
    frame.set_proto_type(pframe::ProtoType::FRAME);
    frame.set_group_id(id_);
    frame.set_object_id(object_id);

    packet->ToProto(frame);
    client_.Send(client_.client_bev_, frame.SerializeAsString());
    EffectCaculate(object_id);
}

void FrameGroup::SaveFrame(std::shared_ptr<FrameItf> frame){
    if(!save_frame_file_) return;

    std::shared_ptr<acore::Task> task = std::make_shared<acore::Task>();
    task->run_ = [=](){
        // encode frame to packet then save with a lenth head
        pframe::FrameData pframe_data;
        frame->ToProto(pframe_data);
        std::string data = pframe_data.SerializeAsString();
        uint16_t size = data.size();
        fwrite(&size, sizeof(uint16_t), 1, save_frame_file_);
        fwrite(data.data(), size, 1, save_frame_file_);
    };
    save_frame_file_task_pool_.PostTask(task);
}

void FrameGroup::RecvCB(acore::Server::Client* client, struct evbuffer* evb, u_int32_t packet_len){

    const char* msg = (const char*)evbuffer_pullup(evb, packet_len);
    const char* data = msg + PACKET_HEAD_SIZE;
    uint16_t* len = (uint16_t*)msg;

    pframe::Relay relay;
    relay.ParseFromArray(data, *len);

    if(relay.proto_type() == pframe::ProtoType::FRAME){
        pframe::Frame frame;
        frame.ParseFromArray(data, *len);

        // Only receive other groups' frame
        assert(frame.group_id() != id_);

        uint64_t object_id = frame.object_id();
        CORE_MAP<uint64_t, std::unique_ptr<FrameObject>>::iterator it = frame_objects_.find(object_id);
        if (it != frame_objects_.end()){
            std::shared_ptr<PacketItf> packet = std::make_shared<PacketItf>();
            // to PacketItf
            packet->ParseFrom(frame);
            it->second->OnPacket(packet);
        }
        EffectCaculate(object_id);
    }
    else if(relay.proto_type() == pframe::ProtoType::EVENT){
        pframe::Event event;
        event.ParseFromArray(data, *len);
        
        if(event.code() == pframe::EventCode::LOGIN_SUCCEED){
            id_ = event.id();
            OnLogin(1, id_);
        }
        else if(event.code() == pframe::EventCode::LOGIN_FAILED){
            id_ = 0;
            OnLogin(0, id_);
        }
        else if(event.code() == pframe::EventCode::REGISTERED_OBJECTS){
            pframe::RegisterObjects registered_objects;
            registered_objects.ParseFromString(event.data());

            for(const uint64_t& remote_id : registered_objects.ids()){
                std::lock_guard<std::mutex> lock(objects_id_mutex_);
                frame_objects_[remote_id] = std::make_unique<FrameObject>();
                frame_objects_[remote_id]->id_ = remote_id;
                captured_objects_id_.insert(remote_id);//atfer FrameObject being created!
                if(OnUpdateId){
                    OnUpdateId(1, remote_id);
                }
            }
            InitCapturedFrameObjects();
        }
        else if(event.code() == pframe::EventCode::UNREGISTERED_OBJECTS){
            pframe::RegisterObjects registered_objects;
            registered_objects.ParseFromString(std::move(event.data()));

            for(const uint64_t& uncaptured_id : registered_objects.ids()){
                std::lock_guard<std::mutex> lock(objects_id_mutex_);
                frame_objects_[uncaptured_id] = std::make_unique<FrameObject>();
                frame_objects_[uncaptured_id]->id_ = uncaptured_id;
                uncaptured_objects_id_.insert(uncaptured_id);//atfer FrameObject being created!
                if(OnUpdateId){
                    OnUpdateId(0, uncaptured_id);
                }
            }
        }
        else if(event.code() == pframe::EventCode::FRAME_IDX_SYNC){
            uint64_t client_idx = event.id();
            uint64_t server_idx = event.target_id();
            std::cout << "FRAME_IDX_SYNC:" << server_idx << " " <<client_idx << std::endl;
            if(time_controller_->IsStarted()){
                time_controller_->Tune(client_idx - server_idx);
            }else{
                assert(client_idx == 0);
                time_controller_->Start(server_idx);
            }
        }
    }
}
void FrameGroup::EventCB(acore::Server::Client* client, const short event){

}


void FrameGroup::EffectCaculate(uint64_t object_id){
    std::lock_guard<std::mutex> lock(objects_id_mutex_);
    std::shared_lock<std::shared_mutex> lock_object_id(frame_objects_[object_id]->frames_mutex_);

    if(captured_objects_id_.contains(object_id)){
        std::shared_ptr<FrameItf>& decider_frame = frame_objects_[object_id]->local_frames_.back();
        if(decider_frame->processes_.empty()){
            return;
        }
        // local frame added
        for(const CORE_SET<uint64_t>::value_type& id : uncaptured_objects_id_){
            std::shared_lock<std::shared_mutex> lock_i(frame_objects_[id]->frames_mutex_);
            //ensure other has frames
            if(frame_objects_[id]->remote_frames_.size() > 0){
                ReviseEffect(frame_objects_[object_id].get(), decider_frame.get(), frame_objects_[id].get(), frame_objects_[id]->remote_frames_.back().get());
            }
        }
    }else if(uncaptured_objects_id_.contains(object_id)){
        // remote frame received
        // If the network is fine, there is no influence. The main situation is a bad network between CS 
        std::shared_ptr<FrameItf>& last_frame = frame_objects_[object_id]->remote_frames_.back();
        for(const uint64_t& id : captured_objects_id_){
            std::shared_lock<std::shared_mutex> lock_i(frame_objects_[id]->frames_mutex_);
            std::deque<std::shared_ptr<FrameItf>>& captured_local_frames = frame_objects_[id]->local_frames_;
            for (std::deque<std::shared_ptr<FrameItf>>::reverse_iterator it = captured_local_frames.rbegin();
                it != captured_local_frames.rend();
                it++){
                if((*it)->idx_ == last_frame->idx_ && !(*it)->processes_.empty()){
                    ReviseEffect(frame_objects_[id].get(), it->get(),
                        frame_objects_[object_id].get(), last_frame.get());
                }
                else if((*it)->idx_ < last_frame->idx_){
                    break;
                }
                
            } 
        }
    }else{
        //FIXME: error? maybe just ignore it
        assert(0);
    }
}


void FrameGroup::ReviseEffect(FrameObject* decider, FrameItf* decider_frame, FrameObject* other, FrameItf* other_frame){
    {
        std::lock_guard<std::mutex> lg_decider(decider->effected_mutex_);
        std::lock_guard<std::mutex> lg_other(other->effected_mutex_);
        
        CORE_SET<uint64_t>& effected_ids = decider->effected_map_[decider_frame->idx_];
        if(effected_ids.contains(other->id_)){
            return;
        }

        //effect now!
        //Attach effect in the nearest Frame
        if(OnEffect){
            for(Process& i : decider_frame->processes_){
                for(CORE_MAP<std::string, std::vector<std::string>>::value_type& j : other_frame->states_){
                    int ret = OnEffect(decider->id_, i.type_, i.args_, other->id_, j.first, j.second);
                    if(ret){
                        effected_ids.insert(other->id_);
                    }
                }
            }
        }
    }
}

void FrameGroup::SetCallBackOnUpdateId(std::function<OnUpdateId_FUNC> cb){
    OnUpdateId = cb;
}
void FrameGroup::SetCallBackOnLogin(std::function<OnLogin_FUNC> cb){
    OnLogin = cb;
}
void FrameGroup::SetCallBackOnEffect(std::function<OnEffect_FUNC> cb){
    OnEffect = cb;
}