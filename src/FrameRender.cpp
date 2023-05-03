#include "FrameRender.h"

using namespace framegroup;

FrameRender::FrameRender()
:OnState(nullptr),
OnProcess(nullptr)
{}
FrameRender::~FrameRender(){}

void FrameRender::Render(std::shared_ptr<FrameItf> frame){
    std::shared_lock<std::shared_mutex> sl(cb_mutex_);
    if(OnState){
        for(CORE_MAP<std::string, std::vector<std::string>>::value_type& i : frame->states_){
            OnState(i.first, i.second);
        }
    }
    if(OnProcess){
        for(Process& i : frame->processes_){
            OnProcess(i.type_, i.args_);
        }
    }
}

void FrameRender::OnFrame(std::shared_ptr<FrameItf> frame){
    Render(frame);
}

void FrameRender::SetCallBackOnState(std::function<OnState_FUNC> cb){
    std::unique_lock<std::shared_mutex> ul(cb_mutex_);
    OnState = cb;
}
void FrameRender::SetCallBackOnProcess(std::function<OnProcess_FUNC> cb){
    std::unique_lock<std::shared_mutex> ul(cb_mutex_);
    OnProcess = cb;
}