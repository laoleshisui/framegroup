#include "FrameCapturer.h"

#include <taskqueue/time_utils.h>
using namespace framegroup;

acore::Recycler<acore::Task> FrameCapturer::available_tasks_ = acore::Recycler<acore::Task>();

FrameCapturer::FrameCapturer()
:time_controller_(nullptr),
send_task_pool_(1),
frame_(std::make_unique<FrameItf>())
{
}
FrameCapturer::~FrameCapturer(){}

void FrameCapturer::Capture(){
    std::lock_guard<std::mutex> lg(frame_mutex_);

    if(!time_controller_ || !time_controller_->UpdateFrameIdx(frame_->idx_)){
        return;
    }
    // std::cout << "Capture : one" << frame_->idx_ <<std::endl;

    std::shared_ptr<acore::Recycler<acore::Task>::Recyclable> task = std::shared_ptr<acore::Recycler<acore::Task>::Recyclable>(available_tasks_.Request());
    
    std::shared_ptr<FrameItf> frame_copy = std::make_shared<FrameItf>(*(frame_.get()));
    frame_->processes_.clear();
    (*task)->run_ = [=, this]{
        for (FrameSinkItf* sink : sinks_){
            // copy frame_
            sink->OnFrame(frame_copy);
        }
    };
    send_task_pool_.PostTask((*task));
}


void FrameCapturer::AttachTimeController(FrameTimeController* time_controller){
    time_controller_ = time_controller;
}


void FrameCapturer::AddState(pframe::StateType type, std::vector<std::string> values){
    std::lock_guard<std::mutex> lg(frame_mutex_);
    frame_->states_.emplace_back();
    frame_->states_.back().type_ = type;
    frame_->states_.back().values_ = std::move(values);
}
void FrameCapturer::AddProcess(pframe::ProcessType type, std::vector<std::string> args){
    std::lock_guard<std::mutex> lg(frame_mutex_);
    frame_->processes_.emplace_back();
    frame_->processes_.back().type_ = type;
    frame_->processes_.back().args_ = std::move(args);
}