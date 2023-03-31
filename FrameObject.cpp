#include "FrameObject.h"

using namespace aoles;

FrameObject::FrameObject()
:id_(0)
{}
FrameObject::~FrameObject(){}

void FrameObject::OnFrame(std::shared_ptr<FrameItf> frame){
    std::shared_ptr<PacketItf> packet = encoder_->Encode(frame);
    local_frames_.emplace_back(frame);
    // send to server
    if(SendPacket){
        SendPacket(packet);
    }

    // Then:
    // Effect Caculation with objects which have remotes Frame.
    // Actually the objects uncaptured 
}

void FrameObject::OnPacket(std::shared_ptr<PacketItf> packet){
    std::shared_ptr<FrameItf> frame = decoder_->Decode(std::move(packet));
    remote_frames_.emplace_back(frame);

    // send to render
    SendFrame(frame);

    // Then:
    // Effect Caculation with objects which have local Frame.
    // Actually the objects captured except this object. 
}

void FrameObject::SendFrame(std::shared_ptr<FrameItf> frame){
    for(FrameSinkItf* sink : sinks_){
        sink->OnFrame(frame);
    }
}