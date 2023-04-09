#include "FrameRender.h"

using namespace aoles;

FrameRender::FrameRender()
:OnMoveTo(0),
OnOperate(0),
health_(0)
{}
FrameRender::~FrameRender(){}

void FrameRender::Render(std::shared_ptr<FrameItf> frame){
    if(OnMoveTo){
        move_to_(frame->position_);
    }
    if(OnOperate){
        for(Operation& opration : frame->operations_){
            operate_(opration);
        }
    }
    if(OnHealth){
        health_(frame->health_);
    }
}

void FrameRender::OnFrame(std::shared_ptr<FrameItf> frame){
    Render(frame);
}