#include "FrameObject.h"

using namespace framegroup;

FrameObject::FrameObject()
:id_(0),
encoder_(std::make_unique<FrameEncoder>()),
decoder_(std::make_unique<FrameDecoder>())
{}

FrameObject::~FrameObject(){}

void FrameObject::OnFrame(std::shared_ptr<FrameItf> frame){
    // Not necessary to send local frame to render. it's from rendered.
    // SendFrame(frame);

    std::shared_ptr<acore::Recycler<PacketItf>::Recyclable> rpacket = encoder_->Encode(frame);
    std::shared_ptr<PacketItf> packet = rpacket->Get();
    {
        std::unique_lock<std::shared_mutex> lock(frames_mutex_);
        local_frames_.emplace_back(frame);
    }
    // send to server
    if(SendPacket){
        SendPacket(packet);
    }
}

void FrameObject::OnPacket(std::shared_ptr<PacketItf> packet){
    std::shared_ptr<acore::Recycler<FrameItf>::Recyclable> rframe = decoder_->Decode(packet);
    std::shared_ptr<FrameItf> frame = rframe->Get();
    {
        std::unique_lock<std::shared_mutex> lock(frames_mutex_);
        remote_frames_.emplace_back(frame);
    }

    // send to render
    SendFrame(frame);
}

void FrameObject::SendFrame(std::shared_ptr<FrameItf> frame){
    for(FrameSinkItf* sink : sinks_){
        sink->OnFrame(frame);
    }
}