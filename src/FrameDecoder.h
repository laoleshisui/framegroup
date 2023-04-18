#ifndef FRAMEDECODER_H
#define FRAMEDECODER_H

#include <acore/utils/Recycler.h>

#include <memory>
#include <PacketItf.h>
#include <FrameItf.h>

namespace framegroup{

// Decode Packet to Frame.
class FrameDecoder
{
public:
    FrameDecoder();
    ~FrameDecoder();
    
    std::shared_ptr<acore::Recycler<FrameItf>::Recyclable> Decode(std::shared_ptr<PacketItf> packet);

    static acore::Recycler<FrameItf> available_frames_;
};

}
#endif