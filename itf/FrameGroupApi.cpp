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
void FrameGroup_SetSaveFrameFilePath(void* group, char* file_path){
    framegroup::FrameGroup* frame_group = (framegroup::FrameGroup*)group;
    frame_group->SetSaveFrameFilePath(std::string(file_path));
}
void FrameGroup_AddCapturer(void* group, uint64_t remote_id, void* capturer){
    framegroup::FrameGroup* frame_group = (framegroup::FrameGroup*)group;
    framegroup::FrameCapturer* frame_capturer = (framegroup::FrameCapturer*)capturer;
    frame_group->AddCapturer(remote_id, frame_capturer);
}
void FrameGroup_AddCaptureredObjects(void* group, int num_of_objects){
    framegroup::FrameGroup* frame_group = (framegroup::FrameGroup*)group;
    frame_group->AddCaptureredObjects(num_of_objects);
}
void FrameGroup_AddRender(void* group, uint64_t remote_id, void* render){
    framegroup::FrameGroup* frame_group = (framegroup::FrameGroup*)group;
    framegroup::FrameRender* frame_render = (framegroup::FrameRender*)render;
    frame_group->AddRender(remote_id, frame_render);
}
void FrameGroup_SetCallBack_OnLogin(void* group, FrameGroup_OnLogin cb){
    framegroup::FrameGroup* frame_group = (framegroup::FrameGroup*)group;
    frame_group->SetCallBackOnLogin(std::move(cb));
}
void FrameGroup_SetCallBack_OnUpdateId(void* group, FrameGroup_OnUpdateId cb){
    framegroup::FrameGroup* frame_group = (framegroup::FrameGroup*)group;
    frame_group->SetCallBackOnUpdateId(std::move(cb));
}
void FrameGroup_SetCallBack_OnEffect(void* group, FrameGroup_OnEffect cb){
    framegroup::FrameGroup* frame_group = (framegroup::FrameGroup*)group;
    std::function<framegroup::FrameGroup::OnEffect_FUNC> jcb = [=](uint64_t decider_remote_id, 
    const std::string& process_type, std::vector<std::string>& args, uint64_t other_remote_id, 
    const std::string& state_type, std::vector<std::string>& values)->int{
        std::vector<const char*> jargs;
        for(int i = 0; i < args.size(); ++i){
            jargs.push_back(args[i].c_str());
        }
        std::vector<const char*> jvalues;
        for(int i = 0; i < values.size(); ++i){
            jvalues.push_back(values[i].c_str());
        }
        return cb(decider_remote_id, process_type.c_str(), jargs.data(), jargs.size(), other_remote_id, state_type.c_str(), jvalues.data(), jvalues.size());
    };
    frame_group->SetCallBackOnEffect(std::move(jcb));
}
void* CreateFrameCapturer(){
    return new framegroup::FrameCapturer();
}
void FrameCapturer_SetState(void* capturer, char* type, char** values, uint32_t rows){
    framegroup::FrameCapturer* frame_capturer = (framegroup::FrameCapturer*)capturer;
    std::vector<std::string> jvalues;
    for(int i = 0; i < rows; ++i){
        jvalues.push_back(std::string(values[i]));
    }
    frame_capturer->SetState(std::string(type), std::move(jvalues));
}
void FrameCapturer_AddProcess(void* capturer, char* type, char** args, uint32_t rows){
    framegroup::FrameCapturer* frame_capturer = (framegroup::FrameCapturer*)capturer;
    std::vector<std::string> jargs;
    for(int i = 0; i < rows; ++i){
        jargs.push_back(std::string(args[i]));
    }
    frame_capturer->AddProcess(std::string(type), std::move(jargs));
}

void* CreateFrameRender(){
    return new framegroup::FrameRender();
}
void FrameRender_SetCallBack_OnState(void* render, uint64_t remote_id, FrameRender_OnState cb){
    framegroup::FrameRender* frame_render = (framegroup::FrameRender*)render;
    std::function<framegroup::FrameRender::OnState_FUNC> jcb = [=](const std::string& type, const std::vector<std::string>& values){
        std::vector<const char*> jvalues;
        for(int i = 0; i < values.size(); ++i){
            jvalues.push_back(values[i].c_str());
        }
        cb(remote_id, type.c_str(), jvalues.data(), jvalues.size());
    };
    frame_render->SetCallBackOnState(std::move(jcb));
}
void FrameRender_SetCallBack_OnProcess(void* render, uint64_t remote_id, FrameRender_OnProcess cb){
    framegroup::FrameRender* frame_render = (framegroup::FrameRender*)render;
    std::function<framegroup::FrameRender::OnProcess_FUNC> jcb = [=](const std::string& type, const std::vector<std::string>& args){
        std::vector<const char*> jargs;
        for(int i = 0; i < args.size(); ++i){
            jargs.push_back(args[i].c_str());
        }
        cb(remote_id, type.c_str(), jargs.data(), jargs.size());
    };
    frame_render->SetCallBackOnProcess(std::move(jcb));
}