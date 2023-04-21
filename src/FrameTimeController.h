/*------------------------------------------------------------------------------
 * @file    FrameTimeController.h
 * @author  yangxian.aoles@foxmail.com
 * @date    2023/04/21 14:47:49
 *----------------------------------------------------------------------------*/
#ifndef __FRAMETIMECONTROLLER_H
#define __FRAMETIMECONTROLLER_H

#include <cstdint>
#include <mutex>

namespace framegroup{
class FrameTimeController
{
public:
    FrameTimeController();

    void SetFPS(int fps);
    void Start(uint64_t first_frame_idx);
    bool IsStarted();
    void Reset();
    bool UpdateFrameIdx(uint64_t& frame_idx);

    /**
     * @brief if forward_ms > 0, then frame idx updating will delay, or ahead of time
     * 
     * @param forward_ms 
     */
    void Tune(int32_t num_of_frame);

private:
    std::mutex mutex_;
    uint64_t delay_ms_;
    //TODO tuning latency between cs frame by tuning first_frame_time_
    uint64_t first_frame_time_;
    uint64_t first_frame_idx_;
};

}

#endif