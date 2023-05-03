/*------------------------------------------------------------------------------
 * @file    FrameController.cpp
 * @author  yangxian.aoles@foxmail.com
 * @date    2023/04/21 14:48:33
 *----------------------------------------------------------------------------*/
#include "FrameTimeController.h"

#include <taskqueue/time_utils.h>

using namespace framegroup;

FrameTimeController::FrameTimeController()
:delay_ms_(32),
first_frame_time_(0),
first_frame_idx_(0),
timer_(std::make_unique<acore::Timer>(delay_ms_, std::bind(&FrameTimeController::Run, this)))
{
    timer_->Start();
}

void FrameTimeController::SetFPS(int fps){
    std::lock_guard<std::mutex> lg(mutex_);
    delay_ms_ = 1000 / fps;
}

void FrameTimeController::Start(uint64_t first_frame_idx){
    first_frame_time_ = taskqueue::TimeMillis();
    first_frame_idx_ = first_frame_idx;
}

bool FrameTimeController::IsStarted(){
    return first_frame_time_ != 0;
}
void FrameTimeController::Reset(){
    first_frame_time_ = 0;
    first_frame_idx_ = 0;
}

bool FrameTimeController::UpdateFrameIdx(uint64_t& frame_idx){
    std::lock_guard<std::mutex> lg(mutex_);
    uint64_t now_ms = taskqueue::TimeMillis();
    if(first_frame_time_){
        uint64_t delay_ms = taskqueue::TimeDiff(now_ms, first_frame_time_);
        int n = first_frame_idx_ + delay_ms / delay_ms_;
        if(n == frame_idx){
            return false;
        }else{
            frame_idx = n;
            return true;
        }
    }else{
        //do nothing
        return false;
    }
}

void FrameTimeController::Tune(int32_t num_of_frame){
    std::lock_guard<std::mutex> lg(mutex_);
    first_frame_idx_ += num_of_frame * delay_ms_;
}

void FrameTimeController::Run(){
    std::shared_lock<std::shared_mutex> sl(timer_mutex_);
    for(std::function<void()>& runnable : runnables_){
        runnable();
    }
}

void FrameTimeController::AddRunable(std::function<void()> runnable){
    std::unique_lock<std::shared_mutex> ul(timer_mutex_);
    runnables_.push_back(runnable);
}