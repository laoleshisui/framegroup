#include "FrameCapturer.h"
#include <libtaskqueue/to_queued_task.h>

using namespace aoles;

FrameCapturer::FrameCapturer()
:fps_(30),
frame_(std::make_shared<FrameItf>()),
task_queue_factory_(taskqueue::CreateTaskQueueStdlibFactory()),
task_queue_frame_(task_queue_factory_->CreateTaskQueue("capture_frame", taskqueue::TaskQueueFactory::Priority::NORMAL))
{}
FrameCapturer::~FrameCapturer(){}

void FrameCapturer::Capture(){
    task_queue_frame_->PostDelayedTask(taskqueue::ToQueuedTask([this]{
        Capture();
        for (FrameSinkItf* sink : sinks_){
            // copy frame_
            sink->OnFrame(std::make_shared<FrameItf>(*(frame_.get())));
        }
    }), 1000 / fps_);
}

void FrameCapturer::AddOperation(Operation op){
    task_queue_frame_->PostTask(taskqueue::ToQueuedTask([this, &op]{
        frame_->operations_.push_back(std::move(op));
    }));
}
void FrameCapturer::AddDeltaHealth(int32_t delta){
    task_queue_frame_->PostTask(taskqueue::ToQueuedTask([this, &delta]{
        frame_->health_ += delta;
    }));
}