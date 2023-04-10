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

    void (*OnMoveTo)(float x, float y);
    void (*OnOperate)(uint32_t type, const char** args, uint32_t rows);
    void (*OnHealth)(uint32_t health);
private:
};

}
#endif