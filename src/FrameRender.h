#ifndef FRAMERENDER_H
#define FRAMERENDER_H

#include <memory>
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

    std::function<OnState_FUNC> OnState;
    std::function<OnProcess_FUNC> OnProcess;
private:
};

}
#endif