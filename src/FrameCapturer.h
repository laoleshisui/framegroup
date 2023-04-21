#ifndef FRAMECAPTURER_H
#define FRAMECAPTURER_H

#include <mutex>
#include <memory>
#include "FrameItf.h"
#include "FrameSinkItf.h"
#include "FrameTimeController.h"
#include <acore/utils/TaskPool.h>
#include <acore/utils/Recycler.h>

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

    void AttachTimeController(FrameTimeController* time_controller);

    static acore::Recycler<acore::Task> available_tasks_;
private:
    FrameTimeController* time_controller_;

    std::mutex frame_mutex_;
    acore::TaskPool send_task_pool_;

    std::unique_ptr<FrameItf> frame_;
};

}
#endif