#include "FrameEncoder.h"
#include <thread>

#include <AProto/pframe/frame_data.pb.h>

using namespace aoles;

FrameEncoder::FrameEncoder()
{}
FrameEncoder::~FrameEncoder(){}

std::shared_ptr<PacketItf> FrameEncoder::Encode(std::shared_ptr<FrameItf> frame){
    pframe::FrameData pframe = frame->ToProto();

    std::shared_ptr<PacketItf> packet = std::make_shared<PacketItf>();
    packet->data_ = pframe.SerializeAsString();
    return packet;
}