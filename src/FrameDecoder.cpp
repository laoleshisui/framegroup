#include "FrameDecoder.h"
#include <thread>

#include <AProto/pframe/frame_data.pb.h>

using namespace aoles;

FrameDecoder::FrameDecoder()
{}
FrameDecoder::~FrameDecoder(){}

std::shared_ptr<FrameItf> FrameDecoder::Decode(std::shared_ptr<PacketItf> packet){
    pframe::FrameData pframe;
    pframe.ParseFromString(packet->data_);
    std::shared_ptr<FrameItf> frame = std::make_shared<FrameItf>();
    frame->ParseFrom(pframe);
    return frame;
}