/*------------------------------------------------------------------------------
 * @file    FrameController.cpp
 * @author  yangxian.aoles@foxmail.com
 * @date    2023/04/21 14:48:33
 *----------------------------------------------------------------------------*/
#include "FrameTimeController.h"

#include <taskqueue/time_utils.h>
#include <acore/log/Log.h>

#include <effolkronium/random.hpp>
// get base random alias which is auto seeded and has static API and internal state
using Random = effolkronium::random_static;

using namespace framegroup;

FrameTimeController::FrameTimeController()
:mutex_(),
delay_ms_(32),
first_frame_time_(0),
first_frame_idx_(0),
timer_mutex_(),
timer_(std::make_unique<acore::Timer>(delay_ms_, std::bind(&FrameTimeController::Run, this)))
{
    timer_->Start();
}

void FrameTimeController::SetFPS(int fps){
    std::lock_guard<std::mutex> lg(mutex_);
    delay_ms_ = 1000 / fps;
}

void FrameTimeController::Start(uint64_t first_frame_idx){
    std::lock_guard<std::mutex> lg(mutex_);
    first_frame_time_ = taskqueue::TimeMillis();
    first_frame_idx_ = first_frame_idx;
}

bool FrameTimeController::IsStarted(){
    std::lock_guard<std::mutex> lg(mutex_);
    return first_frame_time_ != 0;
}
void FrameTimeController::Reset(){
    std::lock_guard<std::mutex> lg(mutex_);
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
    for(CORE_MAP<Key, std::function<void()>>::value_type& runnable : runnables_){
        runnable.second();
    }
}

FrameTimeController::Key FrameTimeController::AddRunable(std::function<void()> runnable){
    std::unique_lock<std::shared_mutex> ul(timer_mutex_);
    Key key = Random::get<Key>(0, UINT32_MAX);

    runnables_[key] = std::move(runnable);
    CORE_LOG(INFO) << "AddRunable succeed, key:" << key << ", number of runnables: " << runnables_.size();
    return key;
}

void FrameTimeController::RemoveRunable(Key runnable_key){
    std::unique_lock<std::shared_mutex> ul(timer_mutex_);
    if(runnables_.contains(runnable_key)){
        runnables_.erase(runnable_key);
    }else{
        CORE_LOG(ERROR) << "Do not contain key:" << runnable_key;
    }
    CORE_LOG(INFO) << "RemoveRunable succeed, key:" << runnable_key << ", number of runnables: " << runnables_.size();
}