#include <iostream>


#include <acore/net/Server.h>
#include <acore/net/Client.h>

#include "../itf/FrameGroupApi.h"

#include <mutex>
#include <condition_variable>

void* frame_group_01;
void* frame_capturer_01;
void* frame_render_01;

void* frame_group_02;
void* frame_capturer_02;
void* frame_render_02;


std::mutex mutex;
std::unique_lock<std::mutex> ul(mutex);
std::condition_variable cv;

void cb_FrameGroup_OnUpdateId_01(int captured, uint64_t remote_id){
    std::cout << "cb_FrameGroup_OnUpdateId_01:" << captured << " " << remote_id << std::endl;
    if(captured){
        FrameGroup_AddCapturer(frame_group_01, remote_id, frame_capturer_01);
    }else{
        FrameGroup_AddRender(frame_group_01, remote_id, frame_render_01);
    }
}
void cb_FrameGroup_OnUpdateId_02(int captured, uint64_t remote_id){
    std::cout << "cb_FrameGroup_OnUpdateId_02:" << captured << " " << remote_id << std::endl;
    if(captured){
        FrameGroup_AddCapturer(frame_group_02, remote_id, frame_capturer_02);
    }else{
        FrameGroup_AddRender(frame_group_02, remote_id, frame_render_02);
    }
}

void cb_OnState_01(const char* type, const char** values, int rows){
    // std::cout<<"cb_OnState_01:" << type << values[0] << std::endl;
}
void cb_OnState_02(const char* type, const char** values, int rows){
    // std::cout<<"cb_OnState_02:" << type << values[0] << std::endl;
}
void cb_OnProcess_01(const char* type, const char** values, int rows){
    std::cout<<"cb_OnProcess_01:" << type << " " << values[0] << std::endl;
}
void cb_OnProcess_02(const char* type, const char** values, int rows){
    std::cout<<"cb_OnProcess_02:" << type << " " << values[0] << std::endl;
}

void thread_capture_01(){
    static float x = 0, y = 0;
    static char* values[10];
    return;
    while(true){
        values[0] = (char*)std::to_string(x).c_str();
        values[1] = (char*)std::to_string(y).c_str();
        x += 1.f;
        y += 1.f;
        FrameCapturer_SetState(frame_capturer_01, "POSITION" , values, 2);
    }
}
void thread_capture_02(){
    static float x = 1, y = 1;
    static char* values[10];
    int attack = 1;
    // return;
    while(true){
        values[0] = (char*)std::to_string(attack).c_str();
        // values[1] = (char*)std::to_string(y).c_str();
        // x += 0.1f;
        // y += 0.1f;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        FrameCapturer_AddProcess(frame_capturer_02, "Attack" , values, 1);
    }

}


void cb_FrameGroup_OnLogin_01(int code, int id){
    std::cout << "cb_FrameGroup_OnLogin_01:" << code << " " << id << std::endl;

    FrameGroup_EnterRoom(frame_group_01,1);
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    FrameGroup_AddCaptureredObjects(frame_group_01, 1);

    std::thread capturer_thread_01(thread_capture_01);
    capturer_thread_01.detach();
}
void cb_FrameGroup_OnLogin_02(int code, int id){
    std::cout << "cb_FrameGroup_OnLogin_02:" << code << " " << id << std::endl;

    FrameGroup_EnterRoom(frame_group_02, 1);
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    FrameGroup_AddCaptureredObjects(frame_group_02, 1);

    std::thread capturer_thread_02(thread_capture_02);
    capturer_thread_02.detach();
}


int main(){
    //01
    frame_group_01 = CreateFrameGroup();
    frame_capturer_01 = CreateFrameCapturer();
    frame_render_01 = CreateFrameRender();
    FrameRender_SetCallBack_OnState(frame_render_01, cb_OnState_01);
    FrameRender_SetCallBack_OnProcess(frame_render_01, cb_OnProcess_01);

    FrameGroup_Connect(frame_group_01, "127.0.0.1", 10002);
    FrameGroup_SetCallBack_OnLogin(frame_group_01, cb_FrameGroup_OnLogin_01);
    FrameGroup_SetCallBack_OnUpdateId(frame_group_01, cb_FrameGroup_OnUpdateId_01);
    FrameGroup_Login(frame_group_01);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    //02
    frame_group_02 = CreateFrameGroup();
    frame_capturer_02 = CreateFrameCapturer();
    frame_render_02 = CreateFrameRender();
    FrameRender_SetCallBack_OnState(frame_render_02, cb_OnState_02);
    FrameRender_SetCallBack_OnProcess(frame_render_02, cb_OnProcess_02);
    FrameGroup_SetCallBack_OnUpdateId(frame_group_02, cb_FrameGroup_OnUpdateId_02);

    FrameGroup_Connect(frame_group_02, "127.0.0.1", 10002);
    FrameGroup_SetCallBack_OnLogin(frame_group_02, cb_FrameGroup_OnLogin_02);
    FrameGroup_Login(frame_group_02);

    cv.wait(ul);
}