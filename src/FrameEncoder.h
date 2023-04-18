#ifndef FRAMEENCODER_H
#define FRAMEENCODER_H

#include <acore/utils/Recycler.h>

#include <memory>
#include <string>
#include <PacketItf.h>
#include <FrameItf.h>

namespace framegroup{

// Encode Frame to Packet
class FrameEncoder
{
public:    
    FrameEncoder();
    ~FrameEncoder();
    
    std::shared_ptr<PacketItf> Encode(std::shared_ptr<FrameItf> frame);

    static acore::Recycler<PacketItf> available_packets_;
};

}
#endif