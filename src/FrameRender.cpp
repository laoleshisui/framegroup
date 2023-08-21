#include "FrameRender.h"
#include "api/FrameRenderApi.h"
#include <acore/log/Log.h>

using namespace framegroup;

FrameRender::FrameRender()
:cb_mutex_(),
observers_()
{}
FrameRender::~FrameRender(){}

void FrameRender::Render(std::shared_ptr<FrameItf> frame){
    std::shared_lock<std::shared_mutex> sl(cb_mutex_);
    for(FrameRenderObserver* observer : observers_){
        for(CORE_MAP<std::string, std::vector<std::string>>::value_type& i : frame->states_){
            observer->OnState(i.first, i.second);
        }
        for(Process& i : frame->processes_){
            observer->OnProcess(i.type_, i.args_);
        }
    }
}

void FrameRender::OnFrame(std::shared_ptr<FrameItf> frame){
    Render(frame);
}

void FrameRender::AddObserver(FrameRenderObserver* observer){
    std::unique_lock<std::shared_mutex> ul(cb_mutex_);
    observers_.push_back(observer);
}