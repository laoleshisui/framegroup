#ifndef FRAMEDECODER_H
#define FRAMEDECODER_H

#include <memory>
#include "FrameSinkItf.h"

namespace aoles{

// Decode Packet to Frame.
class FrameDecoder
{
public:
    FrameDecoder();
    ~FrameDecoder();
    
    std::shared_ptr<FrameItf> Decode(std::shared_ptr<PacketItf> packet);
private:
};

}
#endif