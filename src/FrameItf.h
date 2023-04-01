#ifndef FRAMEITF_H
#define FRAMEITF_H

#include <string>
#include <pframe/frame_data.pb.h>

namespace aoles{

using pframe::FrameType;
using pframe::Operation;

class Position{
public:
    Position() = default;
    Position(float x, float y)
    :x_(x),
    y_(y)
    {}
    
    void ParseFrom(pframe::Position& pos){
        x_ = pos.x();
        y_ = pos.y();
    }
    pframe::Position ToProto(){
        pframe::Position pos;
        pos.set_x(x_);
        pos.set_y(y_);
        return std::move(pos);
    }

    float x_;
    float y_;
};

class FrameItf
{
public:
    FrameItf()=default;

    pframe::FrameData ToProto(){
        pframe::FrameData pframe;
        pframe.set_idx(idx_);
        pframe.set_frame_type(type_);
        pframe.set_ref_frame_idx(ref_frame_idx_);
        pframe::Position pos = position_.ToProto();
        pframe.set_allocated_position(&pos);
        for(Operation& op : operation_){
                pframe.add_operation(op);
        }

        return std::move(pframe);
    }
    void ParseFrom(pframe::FrameData& pframe){
        idx_ = pframe.idx();
        type_ = pframe.frame_type();
        ref_frame_idx_ = pframe.ref_frame_idx();
        position_ = Position(pframe.position().x(), pframe.position().y());
        for(int i = 0; i < pframe.operation_size(); i++){
            operation_.emplace_back(pframe.operation(i));
        }
    }

    uint64_t idx_;
    FrameType type_;
    uint64_t ref_frame_idx_;
    Position position_;
    std::vector<Operation> operation_;
};

class PacketItf
{
public:
    std::string data_;
};
}
#endif