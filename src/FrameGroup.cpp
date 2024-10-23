#include "FrameGroup.h"

#include "FrameObject.h"
#include <acore/utils/MapUtils.h>

#include <aproto/pframe/frame.pb.h>

#include <acore/log/Log.h>

using namespace framegroup;

FrameGroup::FrameGroup()
:id_(0),
room_id_(0),
time_controller_(std::make_unique<FrameTimeController>()),
objects_id_mutex_(),
client_(6),
save_frame_file_task_pool_(1),
save_frame_file_(nullptr),
enable_effect_caculate_(false),
observers_()
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
    if(id_) return;
    acore::SocketAddress addr(std::move(ip), port);
    client_.Connect(addr);
    client_loop_ = std::thread(&acore::Client::Loop, &client_);
    client_loop_.detach();
}

void FrameGroup::DisConnect(){
    id_ = 0;
    client_.DisConnect();
}

void FrameGroup::Login(){
    if(id_) return;
    pframe::Login login;
    login.set_proto_type(pframe::ProtoType::LOGIN);
    client_.Send(client_.client_bev_, login.SerializeAsString());
}
void FrameGroup::Logout(){
    if(!id_) return;
    pframe::Type logout;
    logout.set_proto_type(pframe::ProtoType::LOGOUT);
    client_.Send(client_.client_bev_, logout.SerializeAsString());
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
    std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
    save_frame_file_ = fopen(file_path.c_str(), "wb+");
}

void FrameGroup::EnableEffectCaculate(bool enabled){
    std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
    enable_effect_caculate_ = enabled;
}

void FrameGroup::AddCaptureredObjects(std::string object_type, int num_of_objects, bool commit){
    std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
    assert(id_);
    if(!id_ || !num_of_objects){
        return;
    }

    pending_captured_objects_nums_[object_type] += num_of_objects;

    if(!commit){
        return;
    }

    pframe::RegisterObjects register_objects;
    register_objects.set_group_id(id_);
    register_objects.set_proto_type(pframe::ProtoType::REGISTER_OBJECTS);

    for(CORE_MAP<std::string, int>::value_type& i : pending_captured_objects_nums_){
        register_objects.add_object_types(i.first);
        register_objects.add_nums_of_objects(i.second);
    }

    client_.Send(client_.client_bev_, register_objects.SerializeAsString());

    pending_captured_objects_nums_.clear();
}

void FrameGroup::AddCapturer(uint64_t remote_id, FrameCapturer* capturer){
    std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
    if(frame_capturers_.contains(remote_id)){
        CORE_LOG(ERROR) << "frame_capturers_ has already contianed id:" << remote_id;
        assert(0);
    }
    capturer->AddSink(frame_objects_[remote_id].get());
    frame_capturers_[remote_id] = capturer;
    capturer->AttachTimeController(time_controller_.get());
}

void FrameGroup::RemoveCapturer(uint64_t remote_id){
    std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
    if(!frame_capturers_.contains(remote_id)){
        CORE_LOG(WARNING) << "frame_capturers_ does not contian id:" << remote_id;
        return;
    }
    FrameCapturer* capturer = frame_capturers_[remote_id];
    capturer->DetachTimeController();
    capturer->RemoveSink(frame_objects_[remote_id].get());
    frame_capturers_.erase(remote_id);
}

void FrameGroup::AddRender(uint64_t remote_id, FrameRender* render){
    std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
    if(frame_renders_.contains(remote_id)){
        CORE_LOG(ERROR) << "frame_renders_ has already contianed id:" << remote_id;
        assert(0);
    }else{
        frame_renders_[remote_id] = render;
        frame_objects_.at(remote_id)->AddSink(render);
    }
}

void FrameGroup::RemoveRender(uint64_t remote_id){
    std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
    if(!frame_renders_.contains(remote_id)){
        CORE_LOG(WARNING) << "frame_renders_ does not contianed id:" << remote_id;
        return;
    }
    FrameRender* render = frame_renders_[remote_id];
    frame_objects_[remote_id]->RemoveSink(render);
    frame_renders_.erase(remote_id);
}
bool FrameGroup::RenderExisted(uint64_t remote_id){
    std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
    return frame_renders_.contains(remote_id);
}

void FrameGroup::SyncIFrames(uint64_t remote_id){
    std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
    if(!frame_renders_.contains(remote_id)){
        CORE_LOG(WARNING) << "frame_renders_ does not contianed id:" << remote_id;
        CORE_LOG(WARNING) << "RequireIFrame return";
        return;
    }
    pframe::SyncIFrames sync_iframes;
    sync_iframes.set_proto_type(pframe::ProtoType::SYNCIFRAMES);
    sync_iframes.set_object_id(remote_id);//0 is all members's iframe in the room

    client_.Send(client_.client_bev_, sync_iframes.SerializeAsString());
}

void FrameGroup::ConsumeItem(uint64_t remote_id, uint64_t item_id, int count){
    pframe::ConsumeItem consume_item;
    consume_item.set_proto_type(pframe::ProtoType::CONSUME_ITEM);
    consume_item.mutable_item()->set_object_id(remote_id);
    consume_item.mutable_item()->set_item_id(item_id);
    consume_item.mutable_item()->set_count(count);
    {
        std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
        client_.Send(client_.client_bev_, consume_item.SerializeAsString());
    }
}
void FrameGroup::ObtainItem(uint64_t remote_id, uint64_t item_id, int count){
    pframe::ObtainItem obtain_item;
    obtain_item.set_proto_type(pframe::ProtoType::OBTAIN_ITEM);
    obtain_item.mutable_item()->set_object_id(remote_id);
    obtain_item.mutable_item()->set_item_id(item_id);
    obtain_item.mutable_item()->set_count(count);
    {
        std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
        client_.Send(client_.client_bev_, obtain_item.SerializeAsString());
    }
}
void FrameGroup::IterateItems(uint64_t remote_id, uint64_t item_id){
    pframe::IterateItems iterate_items;
    iterate_items.set_proto_type(pframe::ProtoType::ITERATE_ITEMS);
    iterate_items.set_object_id(remote_id);
    iterate_items.set_item_id(item_id);
    {
        std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
        client_.Send(client_.client_bev_, iterate_items.SerializeAsString());
    }
}

void FrameGroup::RemoveAllIDs(){
    std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);

    //remove all capturers id
    for(const uint64_t& id : captured_objects_id_){
        RemoveCapturer(id);
    }
    //remove all renders id
    for(const uint64_t& id : uncaptured_objects_id_){
        RemoveRender(id);
    }
    //remove all objects
    frame_objects_.clear();
    captured_objects_id_.clear();
    uncaptured_objects_id_.clear();

    time_controller_->Reset();
}

void FrameGroup::AddObserver(FrameGroupObserver* observer){
    std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
    observers_.push_back(observer);
}

void FrameGroup::InitCapturedFrameObjects(){
    //TODO fill frame_objects_ & captured_objects_id_ & uncaptured_objects_id_
    //Maybe from server data
   std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_); 
    for(CORE_MAP<uint64_t, std::unique_ptr<FrameObject>>::value_type& i : frame_objects_){
        i.second->SendPacket = std::bind(&FrameGroup::SendPacket, this, i.first, std::placeholders::_1);
    }
}

void FrameGroup::SendPacket(uint64_t object_id, std::shared_ptr<PacketItf> packet){
    if(!id_){
        return;
    }

    if(packet->data_.empty()){
        return;
    }

    pframe::Frame frame;
    frame.set_proto_type(pframe::ProtoType::FRAME);
    frame.set_group_id(id_);
    frame.set_object_id(object_id);

    packet->ToProto(frame);
    client_.Send(client_.client_bev_, frame.SerializeAsString());
    SaveFrame(object_id, packet);
    EffectCaculate(object_id);
}

void FrameGroup::SaveFrame(uint64_t object_id, std::shared_ptr<PacketItf> packet){
    std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
    if(!save_frame_file_) return;

    std::shared_ptr<acore::Task> task = std::make_shared<acore::Task>();
    task->run_ = [=](){
        // encode frame to packet then save with a lenth head
        std::string& data = packet->data_;
        uint64_t id = object_id;
        uint16_t data_size = sizeof(uint64_t) + data.size();
        fwrite(&data_size, sizeof(data_size), 1, save_frame_file_);
        fwrite(&id, sizeof(id), 1, save_frame_file_);
        fwrite(data.data(), data.size(), 1, save_frame_file_);
    };
    save_frame_file_task_pool_.PostTask(task);
}

void FrameGroup::RecvCB(acore::Server::Client* client, struct evbuffer* evb, u_int32_t packet_len){
    const char* msg = (const char*)evbuffer_pullup(evb, packet_len);
    const char* data = msg + PACKET_HEAD_SIZE;
    uint16_t* len = (uint16_t*)msg;

    pframe::Relay relay;
    relay.ParseFromArray(data, *len);

    // CORE_LOG(INFO) << relay.proto_type();
    if(relay.proto_type() == pframe::ProtoType::FRAME){
        pframe::Frame frame;
        frame.ParseFromArray(data, *len);

        // Only receive other groups' frame
        assert(frame.group_id() != id_);
        // CORE_LOG(INFO) << id_ << " recv from " << frame.group_id();

        CORE_MAP<uint64_t, std::unique_ptr<FrameObject>>::iterator it;
        uint64_t object_id = frame.object_id();
        {
            std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
            it = frame_objects_.find(object_id);
            if (it == frame_objects_.end()){
                CORE_LOG(WARNING) << "Cannot find frame_object: " << object_id;
                return;
            } 
        }

        std::shared_ptr<PacketItf> packet = std::make_shared<PacketItf>();
        // to PacketItf
        packet->ParseFrom(frame);
        it->second->OnPacket(packet);
        SaveFrame(object_id, packet);
        EffectCaculate(object_id);
    }
    else if(relay.proto_type() == pframe::ProtoType::EVENT){
        pframe::Event event;
        event.ParseFromArray(data, *len);

        CORE_LOG(INFO) << "event code:"<< event.code()<< " id:"  << event.id();
        
        if(event.code() == pframe::EventCode::LOGIN_SUCCEED){
            std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
            assert(event.id());
            id_ = event.id();
            OnLogin(1, id_);
        }
        else if(event.code() == pframe::EventCode::LOGIN_FAILED || event.code() == pframe::EventCode::LOGOUT_SUCCEED){
            std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
            id_ = 0;
            OnLogin(0, id_);
        }
        else if(event.code() == pframe::EventCode::REGISTERED_OBJECTS){
            pframe::RegisterObjects registered_objects;
            registered_objects.ParseFromString(event.data());

            std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
            OnCaptured();

            int ids_idx = 0;
            for(int i = 0; i < registered_objects.nums_of_objects_size(); ++i){
                int num = registered_objects.nums_of_objects(i);
                std::string type = registered_objects.object_types(i);
                for(int j = 0; j < num; ++j){
                    uint64_t remote_id = registered_objects.ids(ids_idx + j);

                    if(frame_objects_.contains(remote_id)){
                        CORE_LOG(WARNING) << "frame_objects_ has contained the capturer:" << remote_id;
                    }else{
                        frame_objects_[remote_id] = std::make_unique<FrameObject>();
                        frame_objects_[remote_id]->id_ = remote_id;
                        captured_objects_id_.insert(remote_id);//atfer FrameObject being created!

                        OnUpdateId(1, type, remote_id);
                    }
                }
                ids_idx += num;
            }
            
            InitCapturedFrameObjects();
        }
        else if(event.code() == pframe::EventCode::UNREGISTERED_OBJECTS){
            pframe::RegisterObjects registered_objects;
            registered_objects.ParseFromString(std::move(event.data()));

            std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
            int ids_idx = 0;
            for(int i = 0; i < registered_objects.object_types_size(); ++i){
                int num = registered_objects.nums_of_objects(i);
                std::string type = registered_objects.object_types(i);
                
                for(int j = 0; j < num; ++j){
                    uint64_t remote_id = registered_objects.ids(ids_idx + j);

                    if(frame_objects_.contains(remote_id)){
                        CORE_LOG(WARNING) << "frame_objects_ has contained the render:" << remote_id;
                    }else{
                        frame_objects_[remote_id] = std::make_unique<FrameObject>();
                        frame_objects_[remote_id]->id_ = remote_id;
                        uncaptured_objects_id_.insert(remote_id);//atfer FrameObject being created!

                        OnUpdateId(0, type, remote_id);
                    }
                }
                ids_idx += num;
            }
        }
        else if(event.code() == pframe::EventCode::FRAME_IDX_SYNC){
            uint64_t client_idx = event.id();
            uint64_t server_idx = event.target_id();
            CORE_LOG(INFO) << "FRAME_IDX_SYNC:" << server_idx << " " << client_idx << " " << time_controller_->IsStarted();
            {
                std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
                if(time_controller_->IsStarted()){
                    time_controller_->Tune(client_idx - server_idx);
                }else{
                    assert(client_idx == 0);
                    time_controller_->Start(server_idx);
                }
            }
        }
        else if(event.code() == pframe::EventCode::ENTER_ROOM_SUCCEED){
            room_id_ = event.id();
            OnEnterRoom(1, room_id_, event.target_id());
        }
        else if(event.code() == pframe::EventCode::ENTER_ROOM_FAILED){
            room_id_ = event.id();
            OnEnterRoom(0, room_id_, 0);
        }
        else if(event.code() == pframe::EventCode::EXIT_ROOM_SUCCEED){
            if(event.target_id() == id_){
                OnExitRoom(room_id_, event.target_id(), "", 0);
                room_id_ = 0;
                return;
            }

            pframe::RegisterObjects registered_objects;
            registered_objects.ParseFromString(event.data());

            int ids_idx = 0;
            for(int i = 0; i < registered_objects.nums_of_objects_size(); ++i){
                int num = registered_objects.nums_of_objects(i);
                std::string type = registered_objects.object_types(i);
                for(int j = 0; j < num; ++j){
                    uint64_t remote_id = registered_objects.ids(ids_idx + j);
                    OnExitRoom(room_id_, event.target_id(), type, remote_id);
                }
                ids_idx += num;
            }
        }
        else if(event.code() == pframe::EventCode::SYNCIFRAMES_SUCCEED){
            OnSyncIFrame(captured_objects_id_.contains(event.id()), 1, event.id());
        }
        else if(event.code() == pframe::EventCode::SYNCIFRAMES_FAILED){
            OnSyncIFrame(captured_objects_id_.contains(event.id()), 0, event.id());
        }
        else if(event.code() == pframe::EventCode::ROOM_END){
            OnRoomEnd(event.id());
        }
        else if(event.code() == pframe::EventCode::CONSUME_ITEM_SUCCEED){
            pframe::ConsumeItem consume_item;
            consume_item.ParseFromString(event.data());
            OnConsumeItem(1, consume_item.item().object_id(), consume_item.item().item_id(), consume_item.item().count());
        }
        else if(event.code() == pframe::EventCode::CONSUME_ITEM_FAILED){
            pframe::ConsumeItem consume_item;
            consume_item.ParseFromString(event.data());
            OnConsumeItem(0, consume_item.item().object_id(), consume_item.item().item_id(), consume_item.item().count());
        }
        else if(event.code() == pframe::EventCode::OBTAIN_ITEM_SUCCEED){
            pframe::ObtainItem obtain_item;
            obtain_item.ParseFromString(event.data());
            OnObtainItem(1, obtain_item.item().object_id(), obtain_item.item().item_id(), obtain_item.item().count());
        }
        else if(event.code() == pframe::EventCode::OBTAIN_ITEM_FAILED){
            pframe::ObtainItem obtain_item;
            obtain_item.ParseFromString(event.data());
            OnObtainItem(0, obtain_item.item().object_id(), obtain_item.item().item_id(), obtain_item.item().count());
        }
        else if(event.code() == pframe::EventCode::ITERATE_ITEMS_SUCCEED){
            pframe::IterateItemsResp iterate_item_resp;
            iterate_item_resp.ParseFromString(event.data());
            for(const pframe::Item& item : iterate_item_resp.item()){
                OnIterateItem(item.object_id(), item.item_id(), item.count());
            }
        }
    }
}
void FrameGroup::EventCB(acore::Server::Client* client, const short event){
    OnConn(int(event & BEV_EVENT_CONNECTED));
}

void FrameGroup::EffectCaculate(uint64_t object_id){
    if(!enable_effect_caculate_){
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(objects_id_mutex_);
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
        CORE_LOG(ERROR) << "unknown object id";
        assert(0);
    }
}


void FrameGroup::ReviseEffect(FrameObject* decider, FrameItf* decider_frame, FrameObject* other, FrameItf* other_frame){
    {
        std::lock_guard<std::mutex> lg_decider(decider->effected_mutex_);
        std::lock_guard<std::mutex> lg_other(other->effected_mutex_);
        if(decider_frame->type_ == pframe::Frametype::I || other_frame->type_ == pframe::Frametype::I){
            return;
        }
        
        CORE_SET<uint64_t>& effected_ids = decider->effected_map_[decider_frame->idx_];
        if(effected_ids.contains(other->id_)){
            return;
        }

        //effect now!
        //Attach effect in the nearest Frame

        //FIXME: unuse this code temp because of processes_ is replaced with multimap.
        // for(Process& i : decider_frame->processes_){
        //     for(CORE_MAP<std::string, std::vector<std::string>>::value_type& j : other_frame->states_){
        //         int ret = OnEffect(decider->id_, i.type_, i.args_, other->id_, j.first, j.second);
        //         if(ret){
        //             effected_ids.insert(other->id_);
        //         }
        //     }
        // }
    }
}

void FrameGroup::OnConn(int succeed){
    for(FrameGroupObserver* observer : observers_){
        observer->OnConn(succeed);
    }
}
void FrameGroup::OnLogin(int code, int id){
    for(FrameGroupObserver* observer : observers_){
        observer->OnLogin(code, id);
    }
}
void FrameGroup::OnEnterRoom(int succeed, uint64_t room_id, int64_t remaining_ms){
    for(FrameGroupObserver* observer : observers_){
        observer->OnEnterRoom(succeed, room_id, remaining_ms);
    }
}
void FrameGroup::OnExitRoom(uint64_t room_id, uint64_t group_id, std::string type, uint64_t remote_id){
    for(FrameGroupObserver* observer : observers_){
        observer->OnExitRoom(room_id, group_id, type, remote_id);
    }
}
void FrameGroup::OnRoomEnd(uint64_t room_id){
    for(FrameGroupObserver* observer : observers_){
        observer->OnRoomEnd(room_id);
    }
}
void FrameGroup::OnCaptured(){
    for(FrameGroupObserver* observer : observers_){
        observer->OnCaptured();
    }
}
void FrameGroup::OnUpdateId(int captured, const std::string& object_type, uint64_t remote_id){
    for(FrameGroupObserver* observer : observers_){
        observer->OnUpdateId(captured, object_type, remote_id);
    }
}
void FrameGroup::OnSyncIFrame(int captured, int succeed, uint64_t remote_id){
    for(FrameGroupObserver* observer : observers_){
        observer->OnSyncIFrame(captured, succeed, remote_id);
    }
}
int FrameGroup::OnEffect(uint64_t decider_remote_id, const std::string& process_type, std::vector<std::string>& args, uint64_t other_remote_id, const std::string& state_type, std::vector<std::string>& values){
    for(FrameGroupObserver* observer : observers_){
        observer->OnEffect(decider_remote_id, process_type, args, other_remote_id, state_type, values);
    }
}
void FrameGroup::OnConsumeItem(int succeed, uint64_t remote_id, uint64_t item_id, int count){
    for(FrameGroupObserver* observer : observers_){
        observer->OnConsumeItem(succeed, remote_id, item_id, count);
    }
}
void FrameGroup::OnObtainItem(int succeed, uint64_t remote_id, uint64_t item_id, int count){
    for(FrameGroupObserver* observer : observers_){
        observer->OnObtainItem(succeed, remote_id, item_id, count);
    }
}
void FrameGroup::OnIterateItem(uint64_t remote_id, uint64_t item_id, int count){
    for(FrameGroupObserver* observer : observers_){
        observer->OnIterateItem(remote_id, item_id, count);
    }
}
