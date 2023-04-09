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
    void FrameGroup_SetCallBack_OnUpdateCapturedLocalId(void* group, void(*cb)(uint64_t local_id, uint64_t remote_id)){
        framegroup::FrameGroup* frame_group = (framegroup::FrameGroup*)group;
        frame_group->OnUpdateCapturedLocalId = std::move(cb);
    }
    void FrameGroup_SetCallBack_OnUpdateUncapturedRemoteId(void* group, void(*cb)(uint64_t remote_id)){
        framegroup::FrameGroup* frame_group = (framegroup::FrameGroup*)group;
        frame_group->OnUpdateUncapturedRemoteId = std::move(cb);
    }