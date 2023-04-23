#include "FrameRender.h"

using namespace framegroup;

FrameRender::FrameRender()
:OnState(nullptr),
OnProcess(nullptr)
{}
FrameRender::~FrameRender(){}

void FrameRender::Render(std::shared_ptr<FrameItf> frame){
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