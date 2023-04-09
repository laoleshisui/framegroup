#ifndef FRAMEDECODER_H
#define FRAMEDECODER_H

#include <core/utils/Recycler.h>

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
    
    std::shared_ptr<FrameItf> Decode(std::shared_ptr<PacketItf> packet);
private:
    Core::Recycler<FrameItf> available_frames_;
};

}
#endif