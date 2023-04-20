#include "FrameCapturer.h"

using namespace framegroup;

acore::Recycler<acore::Task> FrameCapturer::available_tasks_ = acore::Recycler<acore::Task>();

FrameCapturer::FrameCapturer()
:first_frame_time_(0),
send_task_pool_(1),
frame_(std::make_unique<FrameItf>())
{
}
FrameCapturer::~FrameCapturer(){}

void FrameCapturer::Capture(){
    std::lock_guard<std::mutex> lg(frame_mutex_);
    std::shared_ptr<acore::Recycler<acore::Task>::Recyclable> task = std::shared_ptr<acore::Recycler<acore::Task>::Recyclable>(available_tasks_.Request());

    std::shared_ptr<FrameItf> frame_copy = std::make_shared<FrameItf>(*(frame_.get()));
    frame_->operations_.clear();
    (*task)->run_ = [=, this]{
        for (FrameSinkItf* sink : sinks_){
            // copy frame_
            sink->OnFrame(frame_copy);
        }
    };
    send_task_pool_.PostTask((*task));
}

void FrameCapturer::AddOperation(Operation op){
    std::lock_guard<std::mutex> lg(frame_mutex_);
    frame_->operations_.push_back(std::move(op));
}
void FrameCapturer::AddDeltaHealth(int32_t delta){
    std::lock_guard<std::mutex> lg(frame_mutex_);
    frame_->health_ += delta;
}
void FrameCapturer::MoveTo(Position pos){
    std::lock_guard<std::mutex> lg(frame_mutex_);
    frame_->position_ = std::move(pos);
}