#include "FrameRender.h"

using namespace framegroup;

FrameRender::FrameRender()
:OnMoveTo(0),
OnOperate(0),
OnHealth(0)
{}
FrameRender::~FrameRender(){}

void FrameRender::Render(std::shared_ptr<FrameItf> frame){
    if(OnMoveTo){
        OnMoveTo(frame->position_);
    }
    if(OnOperate){
        for(Operation& opration : frame->operations_){
            OnOperate(opration);
        }
    }
    if(OnHealth){
        OnHealth(frame->health_);
    }
}

void FrameRender::OnFrame(std::shared_ptr<FrameItf> frame){
    Render(frame);
}