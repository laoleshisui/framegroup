#include "FrameEncoder.h"
#include <thread>

#include <aproto/pframe/frame_data.pb.h>

using namespace framegroup;

FrameEncoder::FrameEncoder()
{}
FrameEncoder::~FrameEncoder(){}

std::shared_ptr<PacketItf> FrameEncoder::Encode(std::shared_ptr<FrameItf> frame){
    pframe::FrameData pframe;
    frame->ToProto(pframe);

    std::shared_ptr<PacketItf> packet = available_packets_.Request();
    if(!packet){
        packet = std::make_shared<PacketItf>();
    }

    packet->data_ = pframe.SerializeAsString();
    return packet;
}