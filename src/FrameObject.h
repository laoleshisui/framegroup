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
#include <shared_mutex>
#include <deque>

#include <acore/Type.h>

#include "FrameItf.h"
#include "FrameEncoder.h"
#include "FrameDecoder.h"
#include "PacketSinkItf.h"
#include "FrameSinkItf.h"


namespace framegroup{

class FrameObject : public FrameSinkItf, 
    public PacketSinkItf,
    public FrameSourceItf
{
public:
    friend class FrameGroup;

    FrameObject();
    FrameObject(FrameObject&)=delete;
    explicit FrameObject(FrameObject&& from)=delete;
    ~FrameObject();

private:
    uint64_t id_;

    std::shared_mutex frames_mutex_;
    int max_frame_queue_size_;
    // Monotonically increasing frame idx
    std::deque<std::shared_ptr<FrameItf>> local_frames_;
    std::deque<std::shared_ptr<FrameItf>> remote_frames_;
    
    std::unique_ptr<FrameEncoder> encoder_;
    std::unique_ptr<FrameDecoder> decoder_;

    std::mutex effected_mutex_;
    CORE_MAP<uint64_t, CORE_SET<uint64_t>> effected_map_;//idx --> effected object ids

    void AddFrame(bool is_local, std::shared_ptr<FrameItf> frame);

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