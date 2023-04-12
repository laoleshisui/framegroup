#include "FrameDecoder.h"
#include <thread>

#include <aproto/pframe/frame_data.pb.h>

using namespace framegroup;

FrameDecoder::FrameDecoder()
{}
FrameDecoder::~FrameDecoder(){}

std::shared_ptr<FrameItf> FrameDecoder::Decode(std::shared_ptr<PacketItf> packet){
    pframe::FrameData pframe;
    pframe.ParseFromString(packet->data_);
    std::shared_ptr<FrameItf> frame = available_frames_.Request();
    if(!frame){
        frame = std::make_shared<FrameItf>();
    }
    
    frame->ParseFrom(pframe);
    return frame;
}