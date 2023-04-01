#ifndef FRAMEENCODER_H
#define FRAMEENCODER_H

#include <memory>
#include <string>
#include "FrameSinkItf.h"

namespace aoles{

// Encode Frame to Packet
class FrameEncoder
{
public:
    FrameEncoder();
    ~FrameEncoder();
    
    std::shared_ptr<PacketItf> Encode(std::shared_ptr<FrameItf> frame);
private:
};

}
#endif