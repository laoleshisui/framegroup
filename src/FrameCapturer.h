#ifndef FRAMECAPTURER_H
#define FRAMECAPTURER_H

#include <memory>
#include "FrameItf.h"
#include "FrameSinkItf.h"

#include <libtaskqueue/task_queue_stdlib.h>
namespace framegroup{

// Capture Frame of one object from ui
// Collect operations in every Frame
class FrameCapturer : public FrameSourceItf
{
public:
    FrameCapturer();
    ~FrameCapturer();
    
    void Capture();
    
    void AddOperation(Operation op);
    void AddDeltaHealth(int32_t delta);
    void MoveTo(Position pos);
private:
    uint32_t fps_;
    std::shared_ptr<FrameItf> frame_;
    std::unique_ptr<taskqueue::TaskQueueFactory> task_queue_factory_;
    //for updating frame_
    std::unique_ptr<taskqueue::TaskQueueBase, taskqueue::TaskQueueDeleter> task_queue_frame_;
};

}
#endif