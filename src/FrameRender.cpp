#include "FrameRender.h"

using namespace framegroup;

FrameRender::FrameRender()
:OnState(0),
OnProcess(0)
{}
FrameRender::~FrameRender(){}

void FrameRender::Render(std::shared_ptr<FrameItf> frame){
    std::cout << " " << frame->idx_ <<std::endl;
    if(OnState){
        for(State& i : frame->states_){
            OnState(i.type_, i.values_);
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