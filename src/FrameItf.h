#ifndef FRAMEITF_H
#define FRAMEITF_H

#include <string>
#include <cmath>
#include <aproto/pframe/frame_data.pb.h>

namespace framegroup{

using pframe::StateType;
using pframe::ProcessType;

class State{
public:
    void ToProto(pframe::State& pstate){
        // pframe::StateType ptype;
        // if(type_ == "FRAME_TYPE"){
        //     ptype = pframe::StateType::FRAME_TYPE;
        // }
        // else if(type_ == "POSITION"){
        //     ptype = pframe::StateType::POSITION;
        // }
        // else{
        //     //err
        //     assert(0);
        //     return;
        // }
        pstate.set_type(type_);
        for(std::string& i : values_){
            pstate.add_values(i);
        }
    }
    void ParseFrom(const pframe::State& pstate){
        // if(pstate.type() == pframe::StateType::FRAME_TYPE){
        //     type_ = "FRAME_TYPE";
        // }else if(pstate.type() == pframe::StateType::POSITION){
        //     type_ = "POSITION";
        // }else{
        //     assert(0);
        //     return;
        // }
        type_ = pstate.type();
        for(const std::string& i : pstate.values()){
            values_.push_back(i);
        }
    }
    StateType type_;
    std::vector<std::string> values_;
};
class Process{
public:
    void ToProto(pframe::Process& pprocess){
        // pframe::ProcessType ptype;
        // if(type_ == "FRAME_TYPE"){
        //     ptype = pframe::ProcessType::FRAME_TYPE;
        // }
        // else if(type_ == "POSITION"){
        //     ptype = pframe::ProcessType::POSITION;
        // }
        // else{
        //     //err
        //     assert(0);
        //     return;
        // }
        pprocess.set_type(type_);
        for(std::string& i : args_){
            pprocess.add_args(i);
        }
    }

    void ParseFrom(const pframe::Process& pprocess){
        // if(pprocess.type() == pframe::ProcessType::RANGE_DAMAGE){
        //     type_ = "RANGE_DAMAGE";
        // }else if(pprocess.type() == pframe::ProcessType::DAMAGED){
        //     type_ = "DAMAGED";
        // }else{
        //     assert(0);
        //     return;
        // }
        type_ = pprocess.type();

        for(const std::string& i : pprocess.args()){
            args_.push_back(i);
        }
    }
    ProcessType type_;
    std::vector<std::string> args_;
};

class FrameItf
{
public:
    FrameItf()
    :idx_(0)
    {}
    void ToProto(pframe::FrameData& pframe){
        pframe.set_idx(idx_);
        for(State& i : states_){
            pframe::State* p = pframe.add_states();
            i.ToProto(*p);
        }
        for(Process& i : processes_){
            pframe::Process* p = pframe.add_processes();
            i.ToProto(*p);
        }
    }
    void ParseFrom(const pframe::FrameData& pframe){
        idx_ = pframe.idx();
        for(const pframe::State& i : pframe.states()){
            states_.emplace_back();
            states_.back().ParseFrom(i);
        }
        for(const pframe::Process& i : pframe.processes()){
            processes_.emplace_back();
            processes_.back().ParseFrom(i);
        }
    }

    uint64_t idx_;
    std::vector<State> states_;
    std::vector<Process> processes_;
};
}
#endif