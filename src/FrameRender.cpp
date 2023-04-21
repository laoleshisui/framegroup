#include "FrameRender.h"

using namespace framegroup;

FrameRender::FrameRender()
:OnMoveTo(0),
OnOperate(0),
OnHealth(0)
{}
FrameRender::~FrameRender(){}

void FrameRender::Render(std::shared_ptr<FrameItf> frame){
    std::cout << frame->position_.x_<< " " << frame->idx_ <<std::endl;
    if(OnMoveTo){
        OnMoveTo(frame->position_.x_, frame->position_.y_);
    }
    if(OnOperate){
        for(Operation& opration : frame->operations_){
            const char** args = (const char**)malloc(opration.args_.size());
            for(int i = 0; i < opration.args_.size(); ++i){
                args[i] = opration.args_[i].c_str();
            }
            OnOperate(opration.type_, args, opration.args_.size());
            free(args);
        }
    }
    if(OnHealth){
        OnHealth(frame->health_);
    }
}

void FrameRender::OnFrame(std::shared_ptr<FrameItf> frame){
    Render(frame);
}