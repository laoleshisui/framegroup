//
// Created by 杨纤 on 2023/3/23.
//

#ifndef FRAMEOBJECT_H
#define FRAMEOBJECT_H
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <mutex>

#include <core/Type.h>

#include "FrameItf.h"
#include "FrameEncoder.h"
#include "FrameDecoder.h"
#include "PacketSinkItf.h"
#include "FrameSinkItf.h"


namespace aoles{

class FrameObject : public FrameSinkItf, 
    public PacketSinkItf,
    public FrameSourceItf
{
public:
    friend class FrameGroup;

    FrameObject();
    FrameObject(FrameObject&)=delete;
    explicit FrameObject(FrameObject&& from)=default;
    ~FrameObject();

private:
    uint64_t id_;
    // Monotonically increasing frame idx
    std::vector<std::shared_ptr<FrameItf>> local_frames_;//from ui_thread, to socket thread
    std::vector<std::shared_ptr<FrameItf>> remote_frames_;//from socket_thread, to ui_thread
    std::unique_ptr<FrameEncoder> encoder_;
    std::unique_ptr<FrameDecoder> decoder_;

    CORE_MAP<uint64_t, CORE_SET<uint64_t>> effected_map_;//idx --> effected object ids

    // inputs:
    // Receive from capturer
    virtual void OnFrame(std::shared_ptr<FrameItf> frame);
    // Receive from Server‘s data
    virtual void OnPacket(std::shared_ptr<PacketItf> packet);

    // outputs:
    // Send to Server
    std::function<void(std::shared_ptr<PacketItf>)> SendPacket;
    // Send to Render
    void SendFrame(std::shared_ptr<FrameItf> frame);
};


}

#endif