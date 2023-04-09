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

#include <core/Type.h>
#include <core/net/Client.h>

#include "FrameCapturer.h"
#include "FrameRender.h"
#include <PacketItf.h>


namespace framegroup{

class FrameObject;

/**
 * Multi clients support multicast by server;
 * Just two client had better support p2p.
*/
class FrameGroup
{
public:
    typedef void(OnUpdateCapturedLocalId_FUNC)(uint64_t local_id, uint64_t remote_id);
    typedef void(OnUpdateUncapturedRemoteId_FUNC)(uint64_t remote_id);

    FrameGroup();
    ~FrameGroup();

    void Connect(std::string ip, int port);
    void Login();
    void EnterRoom(uint64_t room_id = 0);
    void ExitRoom(uint64_t room_id = 0);

    void AddCapturer(uint64_t local_id, FrameCapturer* capturer);
    void RegisterCaptureredOnServer();
    void AddRender(uint64_t remote_id, FrameRender* render);
    std::function<OnUpdateCapturedLocalId_FUNC> OnUpdateCapturedLocalId;
    std::function<OnUpdateUncapturedRemoteId_FUNC> OnUpdateUncapturedRemoteId;
private:
    uint64_t id_;
    CORE_MAP<uint64_t, std::unique_ptr<FrameObject>> frame_objects_;
    CORE_MAP<uint64_t, FrameCapturer*> frame_capturers_;//captured_objects_id --> std::shared_ptr<FrameCapturer>
    CORE_SET<uint64_t> captured_objects_id_;
    CORE_SET<uint64_t> uncaptured_objects_id_;
    Core::Client client_;
    std::thread client_loop_;

    // All data is received, will start soon;
    // Maybe a signal from server to start.
    void InitFrameObjects();

    void RecvCB(Core::Server::Client* client, struct evbuffer* evb, u_int32_t packet_len);
    void EventCB(Core::Server::Client* client, const short event);

    // Send to Server
    void SendPacket(uint64_t object_id, std::shared_ptr<PacketItf> packet);

    //decider is always the captured, and other is always the uncaptured.
    void ReviseEffect(FrameObject* decider, FrameItf* decider_frame, FrameObject* other, FrameItf* other_frame);
    
    // 1. When local frame added. 
    // 2. When a remote frame received.
    // The effects on captured object will become effective in the nearest frame;
    void EffectCaculate(uint64_t object_id);
};
}
#endif