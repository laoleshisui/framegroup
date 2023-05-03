#ifndef FRAMERENDER_H
#define FRAMERENDER_H

#include <memory>
#include <shared_mutex>
#include "FrameSinkItf.h"

namespace framegroup{

// Render Frame to ui 
// Run operations in every Frame
class FrameRender : public FrameSinkItf
{
public:
    typedef void (OnState_FUNC)(const std::string& type, const std::vector<std::string>& values);
    typedef void (OnProcess_FUNC)(const std::string& type, const std::vector<std::string>& args);

    FrameRender();
    ~FrameRender();
    
    virtual void Render(std::shared_ptr<FrameItf> frame);
    virtual void OnFrame(std::shared_ptr<FrameItf> frame) override;


    void SetCallBackOnState(std::function<OnState_FUNC> cb);
    void SetCallBackOnProcess(std::function<OnProcess_FUNC> cb);
private:
    std::shared_mutex cb_mutex_;
    std::function<OnState_FUNC> OnState;
    std::function<OnProcess_FUNC> OnProcess;
};

}
#endif