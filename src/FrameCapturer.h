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

    void SetState(std::string type, std::vector<std::string> values);
    void AddProcess(std::string type, std::vector<std::string> args);

    void SendIFrame();

    void AttachTimeController(FrameTimeController* time_controller);
    void DetachTimeController();

    static acore::Recycler<acore::Task> available_tasks_;
private:
    FrameTimeController* time_controller_;
    FrameTimeController::Key tc_key_;

    std::mutex frame_mutex_;
    acore::TaskPool send_task_pool_;

    std::unique_ptr<FrameItf> frame_;
    bool has_update_;

    void Capture();
};

}
#endif