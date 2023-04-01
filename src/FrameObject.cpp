#include "FrameObject.h"

using namespace aoles;

FrameObject::FrameObject()
:id_(0)
{}

// FrameObject::FrameObject(FrameObject&& from)
// :id_(from.id_),
// local_frames_(std::move(from.local_frames_)),
// remote_frames_(std::move(from.remote_frames_)),
// encoder_(std::move(from.encoder_)),
// decoder_(std::move(from.decoder_)),
// effected_map_(std::move(from.effected_map_)),   
// SendPacket(from.SendPacket)
// {
// }

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