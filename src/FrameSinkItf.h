#ifndef FRAMESINK_H
#define FRAMESINK_H

#include <memory>
#include <mutex>

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
    FrameSourceItf()
    :mutex_()
    {}

    virtual void AddSink(FrameSinkItf* sink){
        std::lock_guard<std::mutex> lg(mutex_);
        sinks_.insert(sink);
    }
    virtual void RemoveSink(FrameSinkItf* sink){
        std::lock_guard<std::mutex> lg(mutex_);
        sinks_.erase(sink);
    }

protected:
    virtual void RunOnEverySink(std::function<void(FrameSinkItf*)> run){
        std::lock_guard<std::mutex> lg(mutex_);
        
        for(FrameSinkItf* sink : sinks_){
            run(sink);
        }
    }

private:
    std::mutex mutex_;
    CORE_SET<FrameSinkItf*> sinks_;
};
}
#endif