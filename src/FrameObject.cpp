#include "FrameObject.h"

using namespace framegroup;

FrameObject::FrameObject()
:id_(0)
{}

FrameObject::~FrameObject(){}

void FrameObject::OnFrame(std::shared_ptr<FrameItf> frame){
    // Not necessary to send local frame to render. it's from rendered.
    // SendFrame(frame);

    std::shared_ptr<PacketItf> packet = encoder_->Encode(frame);
    local_frames_.emplace_back(frame);
    // send to server
    if(SendPacket){
        SendPacket(packet);
    }
}

void FrameObject::OnPacket(std::shared_ptr<PacketItf> packet){
    std::shared_ptr<FrameItf> frame = decoder_->Decode(std::move(packet));
    remote_frames_.emplace_back(frame);

    // send to render
    SendFrame(frame);
}

void FrameObject::SendFrame(std::shared_ptr<FrameItf> frame){
    for(FrameSinkItf* sink : sinks_){
        sink->OnFrame(frame);
    }
}