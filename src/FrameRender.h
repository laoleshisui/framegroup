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
    FrameRender();
    ~FrameRender();
    
    virtual void Render(std::shared_ptr<FrameItf> frame);
    virtual void OnFrame(std::shared_ptr<FrameItf> frame) override;

    void (*OnState)(pframe::StateType& type, std::vector<std::string>& values);
    void (*OnProcess)(pframe::ProcessType& type, std::vector<std::string>& args);
private:
};

}
#endif