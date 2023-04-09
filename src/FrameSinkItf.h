#ifndef FRAMESINK_H
#define FRAMESINK_H

#include <memory>

#include "FrameItf.h"
#include <acore/Type.h>

namespace framegroup{

class FrameSinkItf
{
public:
    virtual void OnFrame(std::shared_ptr<FrameItf> frame){}

protected:
};

class FrameSourceItf
{
public:
    virtual void AddSink(FrameSinkItf* sink){
        sinks_.insert(sink);
    }
    virtual void RemoveSink(FrameSinkItf* sink){
        sinks_.erase(sink);
    }

protected:
    CORE_SET<FrameSinkItf*> sinks_;
};
}
#endif