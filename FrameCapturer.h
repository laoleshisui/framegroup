#ifndef FRAMECAPTURER_H
#define FRAMECAPTURER_H

#include <memory>
#include "FrameItf.h"
#include "FrameSinkItf.h"

namespace aoles{

// Capture Frame of one object from ui
// Collect operations in every Frame
class FrameCapturer : public FrameSourceItf
{
public:
    FrameCapturer();
    ~FrameCapturer();
    
    virtual std::shared_ptr<FrameItf> Capture() = 0;
private:
};

}
#endif