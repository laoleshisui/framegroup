/*
 * @Author: YangXian 
 * @Date: 2023-03-30 11:23:54 
 * @Last Modified by: YangXian
 * @Last Modified time: 2023-03-30 15:56:50
 */
#ifndef FRAMEGROUP_H
#define FRAMEGROUP_H

#include <memory>
#include <thread>
#include <mutex>

#include <acore/Type.h>
#include <acore/net/Client.h>

#include "FrameCapturer.h"
#include "FrameRender.h"
#include "PacketItf.h"


namespace framegroup{

class FrameObject;

/**
 * Multi clients support multicast by server;
 * Just two client had better support p2p.
*/
class FrameGroup
{
public:
    typedef void(OnLogin_FUNC)(int code, int id);
    typedef void(OnUpdateId_FUNC)(int captured, const std::string& object_type, uint64_t remote_id);
    typedef int(OnEffect_FUNC)(uint64_t decider_remote_id, const std::string& process_type, std::vector<std::string>& args, uint64_t other_remote_id, const std::string& state_type, std::vector<std::string>& values);

    FrameGroup();
    ~FrameGroup();

    void Connect(std::string ip, int port);
    void DisConnect();
    void Login();
    void Logout();
    void EnterRoom(uint64_t room_id = 0);
    void ExitRoom(uint64_t room_id = 0);
    
    void SetSaveFrameFilePath(std::string file_path);

    void AddCaptureredObjects(std::string object_type, int num_of_objects, bool commit=false);
    void AddCapturer(uint64_t remote_id, FrameCapturer* capturer);
    void AddRender(uint64_t remote_id, FrameRender* render);

    // Set these cbs before connection to ensure thread-safe
    void SetCallBackOnUpdateId(std::function<OnUpdateId_FUNC> cb);
    void SetCallBackOnLogin(std::function<OnLogin_FUNC> cb);
    void SetCallBackOnEffect(std::function<OnEffect_FUNC> cb);
private:
    std::atomic<uint64_t> id_;
    CORE_MAP<uint64_t, FrameCapturer*> frame_capturers_;//captured_objects_id --> std::shared_ptr<FrameCapturer>

    std::unique_ptr<FrameTimeController> time_controller_;

    //objects_id_mutex_ ensure all apis of this class which not invoked frequently are thread-safe and all data-staructures of this class are thread-safe
    std::recursive_mutex objects_id_mutex_;
    CORE_MAP<uint64_t, std::unique_ptr<FrameObject>> frame_objects_;
    CORE_SET<uint64_t> captured_objects_id_;
    CORE_SET<uint64_t> uncaptured_objects_id_;
    CORE_MAP<std::string, int> pending_captured_objects_nums_;

    acore::Client client_;
    std::thread client_loop_;

    std::function<OnUpdateId_FUNC> OnUpdateId;
    std::function<OnLogin_FUNC> OnLogin;
    std::function<OnEffect_FUNC> OnEffect;

    acore::TaskPool save_frame_file_task_pool_;
    FILE* save_frame_file_;

    // All data is received, will start soon;
    // Maybe a signal from server to start.
    void InitCapturedFrameObjects();

    void RecvCB(acore::Server::Client* client, struct evbuffer* evb, u_int32_t packet_len);
    void EventCB(acore::Server::Client* client, const short event);

    // Send to Server
    void SendPacket(uint64_t object_id, std::shared_ptr<PacketItf> packet);

    void SaveFrame(uint64_t object_id, std::shared_ptr<PacketItf> frame);

    //decider is always the captured, and other is always the uncaptured.
    void ReviseEffect(FrameObject* decider, FrameItf* decider_frame, FrameObject* other, FrameItf* other_frame);
    
    // 1. When local frame added. 
    // 2. When a remote frame received.
    // The effects on captured object will become effective in the nearest frame;
    void EffectCaculate(uint64_t object_id);
};
}
#endif