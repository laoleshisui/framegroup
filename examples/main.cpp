#include <iostream>


#include <acore/net/Server.h>
#include <acore/net/Client.h>

#include "../itf/FrameGroupApi.h"
#include "FrameClient.h"

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


void cb_FrameGroup_OnLogin_01(int code, int id){
    cv.notify_all();
}
void cb_FrameGroup_OnLogin_02(int code, int id){
    cv.notify_all();
}

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

void cb_OnMoveTo_01(float x, float y){
    std::cout<<"cb_OnMoveTo_01:" << x <<", " <<y << std::endl;
}
void cb_OnMoveTo_02(float x, float y){
    std::cout<<"cb_OnMoveTo_02:" << x <<", " <<y << std::endl;
}

void thread_capture_01(){
    static int x = 0, y = 0;
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        FrameCapturer_MoveTo(frame_capturer_01, ++x, ++y);
        FrameCapturer_Capture(frame_capturer_01);
    }
}
void thread_capture_02(){
    static int x = 0, y = 0;
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        FrameCapturer_MoveTo(frame_capturer_02, ++x, ++y);
        FrameCapturer_Capture(frame_capturer_02);
    }

}


int main(){
    //01
    frame_group_01 = CreateFrameGroup();
    FrameGroup_AddObject(frame_group_01);
    frame_capturer_01 = CreateFrameCapturer();
    frame_render_01 = CreateFrameRender();
    FrameRender_SetCallBack_OnMoveTo(frame_render_01, cb_OnMoveTo_01);

    FrameGroup_Connect(frame_group_01, "127.0.0.1", 10002);
    FrameGroup_SetCallBack_OnLogin(frame_group_01, cb_FrameGroup_OnLogin_01);
    FrameGroup_Login(frame_group_01);
    cv.wait(ul);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    FrameGroup_SetCallBack_OnUpdateId(frame_group_01, cb_FrameGroup_OnUpdateId_01);

    FrameGroup_EnterRoom(frame_group_01,1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    FrameGroup_RegisterCaptureredOnServer(frame_group_01);

    std::thread capturer_thread_01(thread_capture_01);
    capturer_thread_01.detach();

    //02
    frame_group_02 = CreateFrameGroup();
    FrameGroup_AddObject(frame_group_02);
    frame_capturer_02 = CreateFrameCapturer();
    frame_render_02 = CreateFrameRender();
    FrameRender_SetCallBack_OnMoveTo(frame_render_02, cb_OnMoveTo_02);

    FrameGroup_Connect(frame_group_02, "127.0.0.1", 10002);
    FrameGroup_SetCallBack_OnLogin(frame_group_02, cb_FrameGroup_OnLogin_02);
    FrameGroup_Login(frame_group_02);
    cv.wait(ul);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    FrameGroup_SetCallBack_OnUpdateId(frame_group_02, cb_FrameGroup_OnUpdateId_02);

    FrameGroup_EnterRoom(frame_group_02, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    FrameGroup_RegisterCaptureredOnServer(frame_group_02);

    std::thread capturer_thread_02(thread_capture_02);
    capturer_thread_02.detach();

    cv.wait(ul);
}