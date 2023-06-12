#include <iostream>
#include <thread>

#include "../itf/FrameGroupApi.h"
#include <acore/log/Log.h>


#define IP "127.0.0.1"
#define PORT 10002
#define ROOM_ID 1
#define CAPTURE_DELAY_MS 16

#define DECLARE_CAPTUTER(x) \
    void* frame_group_##x = CreateFrameGroup();\
    void* frame_capturer_##x = CreateFrameCapturer();\
    std::atomic<bool> is_capture_##x = false;\
    void thread_capture_##x(){\
        static float px = 0, py = 0;\
        static char* values[10];\
        while(is_capture_##x){\
            values[0] = (char*)std::to_string(px).c_str();\
            values[1] = (char*)std::to_string(py).c_str();\
            px += 1.f;\
            py += 1.f;\
            FrameCapturer_AddProcess(frame_capturer_##x, "Attack" , values, 1);\
            std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(CAPTURE_DELAY_MS)));\
        }\
    }\
    void cb_FrameGroup_OnLogin_##x(int code, int id){\
        CORE_LOG(INFO)<< "cb_FrameGroup_OnLogin_##x:" << code << " " << id;\
        if(code != 1){\
            is_capture_##x = false;\
        }else{\
            is_capture_##x = true;\
            FrameGroup_EnterRoom(frame_group_##x, ROOM_ID);\
            FrameGroup_AddCaptureredObjects(frame_group_##x, "test_type_##x", 1, true);\
            std::thread capturer_thread_##x(thread_capture_##x);\
            capturer_thread_##x.detach();\
        }\
    }\
    void cb_FrameGroup_OnUpdateId_##x(int captured, const char* object_type, uint64_t remote_id){\
        CORE_LOG(INFO)<< "cb_FrameGroup_OnUpdateId_##x:" << captured << " " << remote_id;\
        if(captured){\
            FrameGroup_AddCapturer(frame_group_##x, remote_id, frame_capturer_##x);\
        }else{\
        }\
    }

#define DECLARE_RENDER(x) \
    void* frame_group_##x = CreateFrameGroup();\
    void cb_OnState_##x(uint64_t remote_id, const char* type, const char** values, int rows){\
        CORE_LOG(INFO)<<"cb_OnState_"<< remote_id << ":" << type << values[0];\
    }\
    void cb_OnProcess_##x(uint64_t remote_id, const char* type, const char** values, int rows){\
        CORE_LOG(INFO)<<"cb_OnProcess_"<< remote_id <<":" << type << " " << values[0];\
    }\    
    void cb_FrameGroup_OnLogin_##x(int code, int id){\
        CORE_LOG(INFO) << "cb_FrameGroup_OnLogin_##x:" << code << " " << id;\
        if(code != 1){\
            return;\
        }\
        FrameGroup_EnterRoom(frame_group_##x, ROOM_ID);\
    }\
    void cb_FrameGroup_OnUpdateId_##x(int captured, const char* object_type, uint64_t remote_id){\
        CORE_LOG(INFO) << "cb_FrameGroup_OnUpdateId_##x:" << captured << " " << remote_id;\
        if(captured){\
        }else{\
            void* frame_render = CreateFrameRender();\
            FrameRender_SetCallBack_OnState(frame_render, remote_id, cb_OnState_##x);\
            FrameRender_SetCallBack_OnProcess(frame_render, remote_id, cb_OnProcess_##x);\
            FrameGroup_AddRender(frame_group_##x, remote_id, frame_render);\
        }\
    }

#define RUN(x) \
    FrameGroup_SetCallBack_OnLogin(frame_group_##x, cb_FrameGroup_OnLogin_##x);\
    FrameGroup_SetCallBack_OnUpdateId(frame_group_##x, cb_FrameGroup_OnUpdateId_##x);\
    FrameGroup_Connect(frame_group_##x, IP, PORT);\
    FrameGroup_Login(frame_group_##x);
