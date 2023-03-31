#ifndef FRAMERENDER_H
#define FRAMERENDER_H

#include <memory>
#include "FrameSinkItf.h"

namespace aoles{

// Render Frame to ui 
// Run operations in every Frame
class FrameRender : public FrameSinkItf
{
public:
    FrameRender();
    ~FrameRender();
    
    virtual void Render(std::shared_ptr<FrameItf> frame) = 0;
private:
};

}
#endif