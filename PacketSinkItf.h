#ifndef PACKETSINK_H
#define PACKETSINK_H

#include <memory>
#include "FrameItf.h"

namespace aoles{

class PacketSinkItf
{
public:
    virtual void OnPacket(std::shared_ptr<PacketItf> packet){
}

private:
};
}
#endif