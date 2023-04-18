#include "FrameGroupApi.h"

    #include <FrameGroup.h>
    #include <FrameCapturer.h>
    #include <FrameRender.h>

void* CreateFrameGroup(){
    return new framegroup::FrameGroup();
}
void FrameGroup_Connect(void* group, char* ip, int port){
    framegroup::FrameGroup* frame_group = (framegroup::FrameGroup*)group;
    frame_group->Connect(std::string(ip), port);
}
void FrameGroup_Login(void* group){
    framegroup::FrameGroup* frame_group = (framegroup::FrameGroup*)group;
    frame_group->Login();
}
void FrameGroup_EnterRoom(void* group, uint64_t room_id){
    framegroup::FrameGroup* frame_group = (framegroup::FrameGroup*)group;
    frame_group->EnterRoom(room_id);
}
void FrameGroup_ExitRoom(void* group, uint64_t room_id){
    framegroup::FrameGroup* frame_group = (framegroup::FrameGroup*)group;
    frame_group->ExitRoom(room_id);
}
void FrameGroup_AddCapturer(void* group, uint64_t local_id, void* capturer){
    framegroup::FrameGroup* frame_group = (framegroup::FrameGroup*)group;
    framegroup::FrameCapturer* frame_capturer = (framegroup::FrameCapturer*)capturer;
    frame_group->AddCapturer(local_id, frame_capturer);
}
void FrameGroup_RegisterCaptureredOnServer(void* group){
    framegroup::FrameGroup* frame_group = (framegroup::FrameGroup*)group;
    frame_group->RegisterCaptureredOnServer();
}
void FrameGroup_AddRender(void* group, uint64_t remote_id, void* render){
    framegroup::FrameGroup* frame_group = (framegroup::FrameGroup*)group;
    framegroup::FrameRender* frame_render = (framegroup::FrameRender*)render;
    frame_group->AddRender(remote_id, frame_render);
}
void FrameGroup_SetCallBack_OnLogin(void* group, FrameGroup_OnLogin cb){
    framegroup::FrameGroup* frame_group = (framegroup::FrameGroup*)group;
    frame_group->OnLogin = std::move(cb);
}
void FrameGroup_SetCallBack_OnUpdateId(void* group, FrameGroup_OnUpdateId cb){
    framegroup::FrameGroup* frame_group = (framegroup::FrameGroup*)group;
    frame_group->OnUpdateId = std::move(cb);
}

void* CreateFrameCapturer(){
    return new framegroup::FrameCapturer();
}
void FrameCapturer_Capture(void* capturer){
    framegroup::FrameCapturer* frame_capturer = (framegroup::FrameCapturer*)capturer;
    frame_capturer->Capture();
}
void FrameCapturer_AddOperation(void* capturer, uint32_t type, char** args, uint32_t rows){
    framegroup::FrameCapturer* frame_capturer = (framegroup::FrameCapturer*)capturer;
    framegroup::Operation opration;
    opration.type_ = pframe::OperationType(type);
    for(int i = 0; i < rows; ++i){
        opration.args_.push_back(std::string(args[i]));
    }
    frame_capturer->AddOperation(std::move(opration));
}
void FrameCapturer_AddDeltaHealth(void* capturer, int32_t delta){
    framegroup::FrameCapturer* frame_capturer = (framegroup::FrameCapturer*)capturer;
    frame_capturer->AddDeltaHealth(delta);
}
void FrameCapturer_MoveTo(void* capturer, float x, float y){
    framegroup::FrameCapturer* frame_capturer = (framegroup::FrameCapturer*)capturer;
    framegroup::Position pos;
    pos.x_ = x;
    pos.y_ = y;
    frame_capturer->MoveTo(std::move(pos));
}

void* CreateFrameRender(){
    return new framegroup::FrameRender();
}

void FrameRender_SetCallBack_OnMoveTo(void* render, FrameRender_OnMoveTo cb){
    framegroup::FrameRender* frame_render = (framegroup::FrameRender*)render;
    frame_render->OnMoveTo = cb;
}
void FrameRender_SetCallBack_OnOperate(void* render, FrameRender_OnOperate cb){
    framegroup::FrameRender* frame_render = (framegroup::FrameRender*)render;
    frame_render->OnOperate = cb;
}
void FrameRender_SetCallBack_OnHealth(void* render, FrameRender_OnHealth cb){
    framegroup::FrameRender* frame_render = (framegroup::FrameRender*)render;
    frame_render->OnHealth = cb;
}