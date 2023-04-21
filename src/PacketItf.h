/*------------------------------------------------------------------------------
 * @file    PacketItf.h
 * @author  yangxian.aoles@foxmail.com
 * @date    2023/04/03 11:11:49
 *----------------------------------------------------------------------------*/
#ifndef __PACKETITF_H
#define __PACKETITF_H

#include <string>

#include <aproto/pframe/frame.pb.h>

namespace framegroup{
class PacketItf
{
public:
    void ToProto(pframe::Frame& pframe){
        pframe.set_idx(idx_);
        pframe.set_data(std::move(data_));
    }
    void ParseFrom(const pframe::Frame& pframe){
        idx_ = pframe.idx();
        data_ = std::move(pframe.data());
    }

    uint64_t idx_;
    std::string data_;
};
    
}
#endif