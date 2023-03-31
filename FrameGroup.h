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

#include "FrameObject.h"
#include "FrameCapturer.h"
#include "FrameRender.h"

namespace aoles{

/**
 * Multi clients support multicast by server;
 * Just two client had better support p2p.
*/
class FrameGroup
{
public:
    FrameGroup();
    ~FrameGroup();

    void Login();

    void AddCapturer(uint64_t local_id, FrameCapturer* capturer);
    void RegisterCaptureredOnServer();
    void AddRender(uint64_t remote_id, FrameRender* render);
    std::function<void(uint64_t local_id, uint64_t remote_id)> OnUpdateCapturedLocalId;
    std::function<void(uint64_t remote_id)> OnUpdateUncapturedRemoteId;
private:
    uint64_t id_;
    CORE_MAP<uint64_t, FrameObject> frame_objects_;
    CORE_SET<uint64_t> captured_objects_id_;
    CORE_SET<uint64_t> uncaptured_objects_id_;
    Core::Client client_;
    std::thread client_loop_;

    // All data is received, will start soon;
    // Maybe a signal from server to start.
    void InitFrameObjects();

    void RecvCB(Core::Server::Client* client, const char* msg);
    void EventCB(Core::Server::Client* client, const Core::NET_EVENT event);

    // Send to Server
    void SendPacket(uint64_t id, std::shared_ptr<PacketItf> packet);

    void ReviseEffect(FrameObject* decider, FrameItf* decider_frame, FrameObject* other, FrameItf* other_frame);
    
    // when local frame added.
    // Therefore there is no need to Caculate effect again when a remote frame with a calulated frame idx in this side arrived, or it calculated twice.
    void EffectCaculate(uint64_t object_id);
};
}
#endif