#ifndef FRAMERENDER_H
#define FRAMERENDER_H

#include <memory>
#include <vector>
#include <shared_mutex>
#include "FrameSinkItf.h"

#include "api/FrameRenderApi.h"

namespace framegroup{
class FrameRenderObserver;
// Render Frame to ui 
// Run operations in every Frame
class FrameRender : public FrameSinkItf
{
public:
    FrameRender();
    FrameRender(u_int64_t id);
    ~FrameRender();
    
    virtual void Render(std::shared_ptr<FrameItf> frame);
    virtual void OnFrame(std::shared_ptr<FrameItf> frame) override;

    virtual void AddObserver(FrameRenderObserver* observer);
private:
    std::shared_mutex cb_mutex_;
    std::vector<FrameRenderObserver*> observers_;
    uint64_t id_;
};

}
#endif