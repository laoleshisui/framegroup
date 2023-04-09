#ifndef FRAMEITF_H
#define FRAMEITF_H

#include <string>
#include <cmath>
#include <AProto/pframe/frame_data.pb.h>

namespace framegroup{

using pframe::FrameType;
using pframe::OperationType;

class Position{
public:
    void ParseFrom(const pframe::Position& pos){
        x_ = pos.x();
        y_ = pos.y();
    }
    void ToProto(pframe::Position& pos){
        pos.set_x(x_);
        pos.set_y(y_);
    }
    static float Distance(Position& p1, Position& p2){
        return std::hypot((p1.x_ - p2.x_),(p1.y_ - p2.y_));
    }

    float x_;
    float y_;
};

class Operation{
public:
    void ParseFrom(const pframe::Operation& pop){
        type_ = pop.type();
        for(int i = 0; i < pop.args_size(); i++){
            args_.emplace_back(pop.args(i));
        }
    }
    void ToProto(pframe::Operation& pop){
        pop.set_type(type_);
        for(std::string& arg : args_){
            pop.add_args()->assign(arg);
        }
    }
    
    OperationType type_;
    std::vector<std::string> args_;

};

class FrameItf
{
public:
    void ToProto(pframe::FrameData& pframe){
        pframe.set_idx(idx_);
        pframe.set_frame_type(type_);
        pframe.set_ref_frame_idx(ref_frame_idx_);
        position_.ToProto(*(pframe.mutable_position()));

        for(Operation& op : operations_){
            op.ToProto(*(pframe.add_operations()));
        }
        pframe.set_health(health_);
    }
    void ParseFrom(const pframe::FrameData& pframe){
        idx_ = pframe.idx();
        type_ = pframe.frame_type();
        ref_frame_idx_ = pframe.ref_frame_idx();
        position_.ParseFrom(pframe.position());
        for(int i = 0; i < pframe.operations_size(); i++){
            Operation op;
            op.ParseFrom(pframe.operations(i));
            operations_.push_back(std::move(op));
        }
        health_ = pframe.health();
    }

    uint64_t idx_;
    FrameType type_;
    uint64_t ref_frame_idx_;
    Position position_;
    std::vector<Operation> operations_;
    uint32_t health_;
};
}
#endif