
#include <mutex>
#include <condition_variable>


std::mutex mutex = {};
std::unique_lock<std::mutex> ul(mutex);
std::condition_variable cv;

#if 0
#include "Template.h"
    
DECLARE_CAPTUTER(a)
DECLARE_CAPTUTER(b)
DECLARE_CAPTUTER(c)
DECLARE_CAPTUTER(d)
DECLARE_CAPTUTER(e)

DECLARE_RENDER(01)
DECLARE_RENDER(02)
DECLARE_RENDER(03)
DECLARE_RENDER(04)
DECLARE_RENDER(05)

DECLARE_CAPTUTER_RENDER(A)
DECLARE_CAPTUTER_RENDER(B)
DECLARE_CAPTUTER_RENDER(C)
DECLARE_CAPTUTER_RENDER(D)
DECLARE_CAPTUTER_RENDER(E)


int main(){

    RUN(a)
    RUN(b)
    RUN(c)
    RUN(d)
    RUN(e)

    RUN(01)
    RUN(02)
    RUN(03)
    RUN(04)
    RUN(05)

    RUN(A)
    RUN(B)
    RUN(C)
    RUN(D)
    RUN(E)

    cv.wait_for(ul, std::chrono::duration(std::chrono::milliseconds(5000)));

    DELETE_CAPTUTER(a)
    DELETE_CAPTUTER(b)
    DELETE_CAPTUTER(c)
    DELETE_CAPTUTER(d)
    DELETE_CAPTUTER(e)

    DELETE_RENDER(01)
    DELETE_RENDER(02)
    DELETE_RENDER(03)
    DELETE_RENDER(04)
    DELETE_RENDER(05)

    DELETE_CAPTUTER_RENDER(A)
    DELETE_CAPTUTER_RENDER(B)
    DELETE_CAPTUTER_RENDER(C)
    DELETE_CAPTUTER_RENDER(D)
    DELETE_CAPTUTER_RENDER(E)

    CORE_LOG(INFO)<< "main() finished.";     
}
#endif

#include <api/FrameGroupApi.h>
#include <api/FrameRenderApi.h>
#include <api/FrameCapturerApi.h>

#include <mutex>
#include <condition_variable>
#include <thread>
#include <string>
#include <vector>

#include <acore/log/Log.h>
using namespace framegroup;

#define IP "1.15.134.166"
#define PORT 10002
#define ROOM_ID 1
#define CAPTURE_DELAY_MS 16


#define DECLARE_CAPTUTER_RENDER(x) \
std::unique_ptr<FrameGroupApi> group##x = std::make_unique<FrameGroupApi>();\
std::unique_ptr<FrameCapturerApi> frame_capturer##x = std::make_unique<FrameCapturerApi>();\
bool is_capturing_##x;\
namespace{\
    class FRender##x : public FrameRenderObserver\
    {\
    public:\
        virtual void OnState(const std::string& type, const std::vector<std::string>& values){\
            CORE_LOG(INFO) << "OnState" << #x << " "<< type << " " << values.size();\
        }\
        virtual void OnProcess(const std::string& type, const std::vector<std::string>& args){\
            CORE_LOG(INFO) << "OnState" << #x << " "<< type << " " << args.size();\
        }\
    };\
}\
std::unique_ptr<FRender##x> render_observer##x = std::make_unique<FRender##x>();\
namespace{\
    class GObserver##x : public FrameGroupObserver{\
    public:\
        void CaptureThread(){\
            static float px = 0, py = 0;\
            static std::vector<std::string> values(2, "");\
            while(is_capturing_##x){\
            values[0] = std::to_string(px);\
            values[1] = std::to_string(py);\
            px += 1.f;\
            py += 1.f;\
            frame_capturer##x->AddProcess("Attack" , values, false);\
            std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(CAPTURE_DELAY_MS)));\
        }\
        }\
        virtual void OnConn(int succeed){\
            CORE_LOG(INFO) << "OnConn:" << succeed;\
            group##x->Login();\
        }\
        virtual void OnLogin(int code, uint64_t id){\
            CORE_LOG(INFO) << "OnLogin:" << code <<" " << id;\
            if(code != 1){\
                is_capturing_##x = false;\
            }else{\
                is_capturing_##x = true;\
                group##x->AddCaptureredObjects("test_type_##x", 1, true);\
            }\
        }\
        virtual void OnCaptured(){\
            CORE_LOG(INFO) << "OnCaptured";\
            group##x->EnterRoom(ROOM_ID);\
        }\
        virtual void OnUpdateId(int captured, const std::string& object_type, uint64_t remote_id){\
            CORE_LOG(INFO) << "OnUpdateId:" << object_type <<" " << remote_id;\
            if(captured){\
                group##x->AddCapturer(remote_id, frame_capturer##x.get());\
                std::thread capturer_thread##x(&GObserver##x::CaptureThread, this);\
                capturer_thread##x.detach();\
            }else{\
                FrameRenderApi* frame_render = new FrameRenderApi();\
                frame_render->AddObserver(render_observer##x.get());\
                group##x->AddRender(remote_id, frame_render);\
            }\
        }\
        virtual int OnEffect(uint64_t decider_remote_id, const std::string& process_type, std::vector<std::string>& args, uint64_t other_remote_id, const std::string& state_type, std::vector<std::string>& values){}\
    };\
}\
std::unique_ptr<GObserver##x> group_observer##x = std::make_unique<GObserver##x>();


#define RUN(x) \
    group##x->AddObserver(group_observer##x.get());\
    group##x->Connect(IP, PORT);

DECLARE_CAPTUTER_RENDER(1)
DECLARE_CAPTUTER_RENDER(2)

int main(){
    RUN(1)
    RUN(2)

    cv.wait_for(ul, std::chrono::duration(std::chrono::milliseconds(5000)));
    return 0;
}