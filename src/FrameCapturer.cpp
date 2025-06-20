#include "FrameCapturer.h"

#include <taskqueue/time_utils.h>

#include <acore/log/Log.h>

using namespace framegroup;

acore::Recycler<acore::Task> FrameCapturer::available_tasks_ = acore::Recycler<acore::Task>();

FrameCapturer::FrameCapturer()
:time_controller_(nullptr),
tc_key_(0),
frame_mutex_(),
send_task_pool_(1),
frame_(std::make_unique<FrameItf>()),
has_update_(false)
{
    CORE_LOG(INFO) << "new FrameCapturer:" << this;
}
FrameCapturer::~FrameCapturer(){}

void FrameCapturer::Capture(){
    std::lock_guard<std::mutex> lg(frame_mutex_);
    if(!has_update_){
        return;
    }
    if(!time_controller_ || !time_controller_->UpdateFrameIdx(frame_->idx_)){
        return;
    }
    // std::cout << "Capture : one" << frame_->idx_ <<std::endl;

    std::shared_ptr<acore::Recycler<acore::Task>::Recyclable> task = std::shared_ptr<acore::Recycler<acore::Task>::Recyclable>(available_tasks_.Request());
    
    std::shared_ptr<FrameItf> frame_copy = std::make_shared<FrameItf>(*(frame_.get()));
    if(frame_->type_ == pframe::Frametype::I){
        frame_->type_ = pframe::Frametype::P;
    }else{
        //P frame
        frame_->processes_.clear();
    }

    (*task)->run_ = [=, this]{
        RunOnEverySink([=](FrameSinkItf* sink){
            sink->OnFrame(frame_copy);
        });
    };
    send_task_pool_.PostTask((*task));

    has_update_ = false;
}


void FrameCapturer::AttachTimeController(FrameTimeController* time_controller){
    CORE_LOG(INFO) << "AttachTimeController";
    std::lock_guard<std::mutex> lg(frame_mutex_);
    CORE_LOG(INFO) << "tc_key_: " << tc_key_;
    assert(!tc_key_);
    time_controller_ = time_controller;
    tc_key_ = time_controller_->AddRunable(std::bind(&FrameCapturer::Capture, this));
}

void FrameCapturer::DetachTimeController(){
    CORE_LOG(INFO) << "DetachTimeController";
    std::lock_guard<std::mutex> lg(frame_mutex_);
    CORE_LOG(INFO) << "tc_key_: " << tc_key_;
    assert(tc_key_);
    time_controller_->RemoveRunable(tc_key_);
    tc_key_ = 0;
    time_controller_ = nullptr;
}


void FrameCapturer::SetState(std::string type, std::vector<std::string> values){
    std::lock_guard<std::mutex> lg(frame_mutex_);
    frame_->states_[type] = std::move(values);
}
void FrameCapturer::AddProcess(std::string type, std::vector<std::string> args, bool override){
    // CORE_LOG(INFO) << "AddProcess:" << type;
    std::lock_guard<std::mutex> lg(frame_mutex_);
    if(override){
        if(frame_->processes_.contains(type)){
            frame_->processes_.erase(type);
        }
    }
    frame_->processes_.insert({std::move(type), std::move(args)});

    has_update_ = true;
}

void FrameCapturer::SendIFrame(){
    std::lock_guard<std::mutex> lg(frame_mutex_);
    frame_->type_ = pframe::Frametype::I;
    has_update_ = true;
}