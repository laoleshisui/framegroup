
#include <mutex>
#include <condition_variable>


std::mutex mutex = {};
std::unique_lock<std::mutex> ul(mutex);
std::condition_variable cv;

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

#define IP "127.0.0.1"
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
            CORE_LOG(INFO) << "OnState" << #x << " "<< type << " args:" << values.size();\
        }\
        virtual void OnProcess(const std::string& type, const std::vector<std::string>& args){\
            CORE_LOG(INFO) << "OnProcess" << #x << " "<< type << " args:" << args.size();\
        }\
    };\
}\
std::unique_ptr<FRender##x> render_observer##x = std::make_unique<FRender##x>();\
namespace{\
    class GObserver##x : public FrameGroupObserver{\
    public:\
        uint64_t group_id_;\
        uint64_t remote_id_;\
        uint64_t room_id_;\
        void CaptureThread(){\
            static float px = 0, py = 0;\
            static std::vector<std::string> values(2, "");\
            int n_processes = 150;\
            while(is_capturing_##x && n_processes){\
                --n_processes;\
                values[0] = std::to_string(px);\
                values[1] = std::to_string(py);\
                px += 1.f;\
                py += 1.f;\
                frame_capturer##x->AddProcess("Attack" + std::string(#x) , values, false);\
                std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(CAPTURE_DELAY_MS)));\
            }\
            group##x->ExitRoom(room_id_);\
        }\
        virtual void OnConn(int succeed){\
            CORE_LOG(INFO) << "OnConn:" << succeed;\
            group##x->Login(0);\
        }\
        virtual void OnLogin(int code, uint64_t id){\
            CORE_LOG(INFO) << "OnLogin:" << code <<" " << id;\
            if(code != 1){\
                is_capturing_##x = false;\
            }else{\
                is_capturing_##x = true;\
                group_id_ = id;\
                group##x->EnterRoom(ROOM_ID);\
            }\
        }\
        virtual void OnEnterRoom(int succeed, uint64_t room_id, int64_t remaining_ms){\
            CORE_LOG(INFO) << "OnEnterRoom:" << std::string(#x) << " enter " << room_id;\
            group##x->AddCaptureredObjects(#x, 1, true);\
            room_id_ = room_id;\
        }\
        virtual void OnExitRoom(uint64_t room_id, uint64_t group_id, std::string type, uint64_t remote_id){\
            CORE_LOG(INFO) << "OnExitRoom:" << std::string(#x) << " exit " << room_id << "(is me :)" << (group_id_ == group_id);\
            if(group_id_ == group_id){\
                group##x->RemoveAllIDs();\
                group##x->EnterRoom(room_id + 1);/*Enter next room*/\
            }\
        }\
        virtual void OnCaptured(){\
            CORE_LOG(INFO) << "OnCaptured";\
        }\
        virtual void OnUpdateId(int captured, const std::string& object_type, uint64_t remote_id){\
            CORE_LOG(INFO) << "OnUpdateId:" << #x <<"+" << captured << ", " << object_type <<" " << remote_id;\
            if(captured){\
                group##x->AddCapturer(remote_id, frame_capturer##x.get());\
                std::thread capturer_thread##x(&GObserver##x::CaptureThread, this);\
                capturer_thread##x.detach();\
                remote_id_ = remote_id;\
                CORE_LOG(INFO) << "group" << #x << " AddCapturer:" << remote_id;\
                group##x->ObtainItem(remote_id_, 1, 101);\
                std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(CAPTURE_DELAY_MS)));\
                group##x->ConsumeItem(remote_id_, 1, 10);\
                std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(CAPTURE_DELAY_MS)));\
                group##x->IterateItems(0, 0);\
            }else if(!group##x->RenderExisted(remote_id)){\
                    FrameRenderApi* frame_render = new FrameRenderApi();\
                    frame_render->AddObserver(render_observer##x.get());\
                    group##x->AddRender(remote_id, frame_render);\
                    CORE_LOG(INFO) << "group" << #x << " AddRender:" << remote_id;\
            }\
        }\
        virtual int OnEffect(uint64_t decider_remote_id, const std::string& process_type, std::vector<std::string>& args, uint64_t other_remote_id, const std::string& state_type, std::vector<std::string>& values){}\
        virtual void OnConsumeItem(int succeed, uint64_t remote_id, uint64_t item_id, int count){\
            CORE_LOG(INFO) << "OnConsumeItem:" << #x << " " << succeed << ", " << remote_id << " "<< item_id << " " << count;\
        }\
        virtual void OnObtainItem(int succeed, uint64_t remote_id, uint64_t item_id, int count){\
            CORE_LOG(INFO) << "OnObtainItem:" << #x << " " << succeed << ", " << remote_id << " " << item_id << " " << count;\
        }\
        virtual void OnIterateItem(uint64_t remote_id, uint64_t item_id, int count){\
            CORE_LOG(INFO) << "OnIterateItem:" << #x << " "<< remote_id << ", "<< item_id << ", " << count;\
        }\
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
    // std::this_thread::sleep_for(std::chrono::duration(std::chrono::milliseconds(CAPTURE_DELAY_MS)));
    RUN(2)

    cv.wait_for(ul, std::chrono::duration(std::chrono::milliseconds(50000)));
    CORE_LOG(INFO) << "main finished.";
    return 0;
}