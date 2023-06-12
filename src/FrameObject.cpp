#include "FrameObject.h"
#include <acore/log/Log.h>

using namespace framegroup;

FrameObject::FrameObject()
:id_(0),
frames_mutex_(),
max_frame_queue_size_(1),
encoder_(std::make_unique<FrameEncoder>()),
decoder_(std::make_unique<FrameDecoder>()),
effected_mutex_(),
SendPacket(nullptr)
{}

FrameObject::~FrameObject(){}

void FrameObject::AddFrame(bool is_local, std::shared_ptr<FrameItf> frame){
    std::deque<std::shared_ptr<FrameItf>>* target_queue = &local_frames_;
    if(!is_local){
        target_queue = &remote_frames_;
    }
    {
        std::unique_lock<std::shared_mutex> lock(frames_mutex_);
        target_queue->push_back(frame);

        if(target_queue->size() > max_frame_queue_size_){
            target_queue->pop_front();
        }
    }
}

void FrameObject::OnFrame(std::shared_ptr<FrameItf> frame){
    // Not necessary to send local frame to render. it's from rendered.
    // SendFrame(frame);

    std::shared_ptr<acore::Recycler<PacketItf>::Recyclable> rpacket = encoder_->Encode(frame);
    std::shared_ptr<PacketItf> packet = rpacket->Get();
    AddFrame(true, frame);
    // send to server
    if(SendPacket){
        SendPacket(packet);
    }
}

void FrameObject::OnPacket(std::shared_ptr<PacketItf> packet){
    std::shared_ptr<acore::Recycler<FrameItf>::Recyclable> rframe = decoder_->Decode(packet);
    std::shared_ptr<FrameItf> frame = rframe->Get();
    std::shared_ptr<FrameItf> frame_copy = std::make_shared<FrameItf>(*(frame.get()));
    AddFrame(false, frame_copy);

    // send to render
    SendFrame(frame_copy);

    // std::cout << id_ << ": " << frame->idx_ << " "<< frame->states_["POSITION"][0]<< " " << frame->states_["POSITION"][1]<<std::endl;
}

void FrameObject::SendFrame(std::shared_ptr<FrameItf> frame){
    RunOnEverySink([&](FrameSinkItf* sink){
        sink->OnFrame(frame);
    });
}