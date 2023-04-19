#include <iostream>


#include <acore/net/Server.h>
#include <acore/net/Client.h>

#include "../itf/FrameGroupApi.h"
#include "FrameClient.h"

#include <mutex>
#include <condition_variable>

void* frame_group_01;
void* frame_capturer_01;

void* frame_group_02;
// void* frame_capturer_02;
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

void cb_FrameGroup_OnUpdateId_01(uint64_t local_id, uint64_t remote_id){
    std::cout << local_id << " " << remote_id << std::endl;
}
void cb_FrameGroup_OnUpdateId_02(uint64_t local_id, uint64_t remote_id){
    std::cout << local_id << " " << remote_id << std::endl;
    FrameGroup_AddRender(frame_group_02, remote_id, frame_render_02);
}

void cb_OnMoveTo(float x, float y){
    std::cout<<"cb_OnMoveTo:" << x <<", " <<y << std::endl;
}

void thread_capture_01(){
    static int x = 0, y = 0;
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        FrameCapturer_MoveTo(frame_capturer_01, ++x, ++y);
        FrameCapturer_Capture(frame_capturer_01);
    }

}

int main(){
    //01
    frame_group_01 = CreateFrameGroup();
    frame_capturer_01 = CreateFrameCapturer();

    FrameGroup_Connect(frame_group_01, "127.0.0.1", 10002);
    FrameGroup_SetCallBack_OnLogin(frame_group_01, cb_FrameGroup_OnLogin_01);
    FrameGroup_Login(frame_group_01);
    cv.wait(ul);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    FrameGroup_SetCallBack_OnUpdateId(frame_group_01, cb_FrameGroup_OnUpdateId_01);

    FrameGroup_EnterRoom(frame_group_01,1);
    FrameGroup_AddCapturer(frame_group_01, 100, frame_capturer_01);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    FrameGroup_RegisterCaptureredOnServer(frame_group_01);

    std::thread capturer_thread(thread_capture_01);
    capturer_thread.detach();

    //02
    frame_group_02 = CreateFrameGroup();
    frame_render_02 = CreateFrameRender();
    FrameRender_SetCallBack_OnMoveTo(frame_render_02, cb_OnMoveTo);

    FrameGroup_Connect(frame_group_02, "127.0.0.1", 10002);
    FrameGroup_SetCallBack_OnLogin(frame_group_02, cb_FrameGroup_OnLogin_02);
    FrameGroup_Login(frame_group_02);
    cv.wait(ul);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    FrameGroup_SetCallBack_OnUpdateId(frame_group_02, cb_FrameGroup_OnUpdateId_02);

    FrameGroup_EnterRoom(frame_group_02, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    cv.wait(ul);
}