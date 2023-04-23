#include "FrameRender.h"

using namespace framegroup;

FrameRender::FrameRender()
:OnState(nullptr),
OnProcess(nullptr)
{}
FrameRender::~FrameRender(){}

void FrameRender::Render(std::shared_ptr<FrameItf> frame){
    std::cout << " " << frame->idx_ <<std::endl;
    if(OnState){
        for(CORE_MAP<StateType, std::vector<std::string>>::value_type& i : frame->states_){
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