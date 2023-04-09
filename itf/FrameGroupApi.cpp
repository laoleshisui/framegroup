#include "FrameGroupApi.h"

#include <FrameGroup.h>
#include <FrameCapturer.h>
#include <FrameRender.h>

void* CreateFrameGroup(){
        return new aoles::FrameGroup();
    }
    void FrameGroup_Connect(void* group, char* ip, int port){
        aoles::FrameGroup* frame_group = (aoles::FrameGroup*)group;
        frame_group->Connect(std::string(ip), port);
    }
    void FrameGroup_Login(void* group){
        aoles::FrameGroup* frame_group = (aoles::FrameGroup*)group;
        frame_group->Login();
    }
    void FrameGroup_EnterRoom(void* group, uint64_t room_id){
        aoles::FrameGroup* frame_group = (aoles::FrameGroup*)group;
        frame_group->EnterRoom(room_id);
    }
    void FrameGroup_ExitRoom(void* group, uint64_t room_id){
        aoles::FrameGroup* frame_group = (aoles::FrameGroup*)group;
        frame_group->ExitRoom(room_id);
    }
    void FrameGroup_AddCapturer(void* group, uint64_t local_id, void* capturer){
        aoles::FrameGroup* frame_group = (aoles::FrameGroup*)group;
        aoles::FrameCapturer* frame_capturer = (aoles::FrameCapturer*)capturer;
        frame_group->AddCapturer(local_id, frame_capturer);
    }
    void FrameGroup_RegisterCaptureredOnServer(void* group){
        aoles::FrameGroup* frame_group = (aoles::FrameGroup*)group;
        frame_group->RegisterCaptureredOnServer();
    }
    void FrameGroup_AddRender(void* group, uint64_t remote_id, void* render){
        aoles::FrameGroup* frame_group = (aoles::FrameGroup*)group;
        aoles::FrameRender* frame_render = (aoles::FrameRender*)render;
        frame_group->AddRender(remote_id, frame_render);
    }
    void FrameGroup_SetCallBack_OnUpdateCapturedLocalId(void* group, void(*cb)(uint64_t local_id, uint64_t remote_id)){
        aoles::FrameGroup* frame_group = (aoles::FrameGroup*)group;
        frame_group->OnUpdateCapturedLocalId = std::move(cb);
    }
    void FrameGroup_SetCallBack_OnUpdateUncapturedRemoteId(void* group, void(*cb)(uint64_t remote_id)){
        aoles::FrameGroup* frame_group = (aoles::FrameGroup*)group;
        frame_group->OnUpdateUncapturedRemoteId = std::move(cb);
    }