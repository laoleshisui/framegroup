#include <iostream>


#include <acore/net/Server.h>
#include <acore/net/Client.h>

#include "../itf/FrameGroupApi.h"

#include <mutex>
#include <condition_variable>

void* frame_group;
void* frame_capturer;


std::mutex mutex;
std::unique_lock<std::mutex> ul(mutex);
std::condition_variable cv;


void cb_FrameGroup_OnLogin(int code, int id){
    cv.notify_all();
}

void cb_FrameGroup_OnUpdateId(uint64_t local_id, uint64_t remote_id){
    std::cout << local_id << " " << remote_id << std::endl;
}

void thread_capture(){
    static int x = 0, y = 0;
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        FrameCapturer_MoveTo(frame_capturer, ++x, ++y);
        FrameCapturer_Capture(frame_capturer);
    }

}

int main(){
    frame_group = CreateFrameGroup();
    frame_capturer = CreateFrameCapturer();

    FrameGroup_Connect(frame_group, "127.0.0.1", 10002);
    FrameGroup_SetCallBack_OnLogin(frame_group, cb_FrameGroup_OnLogin);
    FrameGroup_Login(frame_group);
    //wait 
    cv.wait(ul);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    FrameGroup_SetCallBack_OnUpdateId(frame_group, cb_FrameGroup_OnUpdateId);

    FrameGroup_EnterRoom(frame_group,1);
    FrameGroup_AddCapturer(frame_group, 100, frame_capturer);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    FrameGroup_RegisterCaptureredOnServer(frame_group);
    
    std::thread capturer_thread(thread_capture);
    capturer_thread.detach();

    cv.wait(ul);
}