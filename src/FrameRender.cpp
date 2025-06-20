#include "FrameRender.h"
#include "api/FrameRenderApi.h"
#include <acore/log/Log.h>

using namespace framegroup;

FrameRender::FrameRender()
:FrameRender(0)
{}

FrameRender::FrameRender(uint64_t id)
:cb_mutex_(),
observers_(),
id_(id)
{}

FrameRender::~FrameRender(){}

void FrameRender::Render(std::shared_ptr<FrameItf> frame){
    std::shared_lock<std::shared_mutex> sl(cb_mutex_);
    for(FrameRenderObserver* observer : observers_){
        for(CORE_MAP<std::string, std::vector<std::string>>::value_type& i : frame->states_){
            if(id_){
                observer->OnState(id_, i.first, i.second);
            }else{
                observer->OnState(i.first, i.second);
            }
        }
        for(std::unordered_multimap<std::string, std::vector<std::string>>::value_type& i : frame->processes_){
            if(id_){
                observer->OnProcess(id_, i.first, i.second);
            }else{
                observer->OnProcess(i.first, i.second);
            }
        }
    }
}

void FrameRender::OnFrame(std::shared_ptr<FrameItf> frame){
    Render(frame);
}

void FrameRender::AddObserver(FrameRenderObserver* observer){
    std::unique_lock<std::shared_mutex> ul(cb_mutex_);
    observers_.push_back(observer);
}