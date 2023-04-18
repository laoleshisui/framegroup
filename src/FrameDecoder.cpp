#include "FrameDecoder.h"
#include <thread>

#include <aproto/pframe/frame_data.pb.h>

using namespace framegroup;

acore::Recycler<FrameItf> FrameDecoder::available_frames_ = acore::Recycler<FrameItf>();

FrameDecoder::FrameDecoder()
{}
FrameDecoder::~FrameDecoder(){}

std::shared_ptr<acore::Recycler<FrameItf>::Recyclable> FrameDecoder::Decode(std::shared_ptr<PacketItf> packet){
    pframe::FrameData pframe;
    pframe.ParseFromString(packet->data_);
    std::shared_ptr<acore::Recycler<FrameItf>::Recyclable> frame = available_frames_.Request();
    
    (*frame)->ParseFrom(pframe);
    return frame;
}