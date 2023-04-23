#ifndef FRAMEITF_H
#define FRAMEITF_H

#include <string>
#include <cmath>
#include <aproto/pframe/frame_data.pb.h>

#include <acore/Type.h>

namespace framegroup{

class Process{
public:
    void ToProto(pframe::Process& pprocess){
        pprocess.set_type(type_);
        for(std::string& i : args_){
            pprocess.add_args(i);
        }
    }

    void ParseFrom(const pframe::Process& pprocess){
        type_ = pprocess.type();

        for(const std::string& i : pprocess.args()){
            args_.push_back(i);
        }
    }
    std::string type_;
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
        for(CORE_MAP<std::string, std::vector<std::string>>::value_type& i : states_){
            pframe::State* p = pframe.add_states();
            p->set_type(i.first);
            for(std::string& value : i.second){
                p->add_values(value);
            }
        }
        for(Process& i : processes_){
            pframe::Process* p = pframe.add_processes();
            i.ToProto(*p);
        }
    }
    void ParseFrom(const pframe::FrameData& pframe){
        states_.clear();
        processes_.clear();

        idx_ = pframe.idx();
        for(const pframe::State& i : pframe.states()){
            for(const std::string& value : i.values()){
                states_[i.type()].push_back(value);
            }
        }
        for(const pframe::Process& i : pframe.processes()){
            processes_.emplace_back();
            processes_.back().ParseFrom(i);
        }
    }

    uint64_t idx_;
    CORE_MAP<std::string, std::vector<std::string>> states_;
    std::vector<Process> processes_;
};
}
#endif