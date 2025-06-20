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
    :type_(pframe::Frametype::P),
    idx_(0)
    {}
    FrameItf(FrameItf&) = default;
    FrameItf& operator=(FrameItf&) = default;
    
    void ToProto(pframe::FrameData& pframe){
        pframe.set_type(type_);
        pframe.set_idx(idx_);
        //I only Encode states and P only encode processes.
        if(type_ == pframe::Frametype::I){
            for(CORE_MAP<std::string, std::vector<std::string>>::value_type& i : states_){
                pframe::State* p = pframe.add_states();
                p->set_type(i.first);
                for(std::string& value : i.second){
                    p->add_values(value);
                }
            }
        }else{
            // P frame
            for(CORE_MAP<std::string, std::vector<std::string>>::value_type& i : processes_){
                pframe::Process* p = pframe.add_processes();
                p->set_type(i.first);
                for(std::string& args : i.second){
                    p->add_args(std::move(args));
                }
            }
        }
    }
    void ParseFrom(const pframe::FrameData& pframe){
        states_.clear();
        processes_.clear();

        type_ = pframe.type();
        idx_ = pframe.idx();
        if(type_ == pframe::Frametype::I){
            for(const pframe::State& i : pframe.states()){
                for(const std::string& value : i.values()){
                    states_[i.type()].push_back(value);
                }
            }
        }
        for(const pframe::Process& i : pframe.processes()){
            std::vector<std::string> args;
            for(const std::string& arg : i.args()){
                args.push_back(arg);
            }
            processes_.insert({i.type(), std::move(args)});
        }
    }

    pframe::Frametype type_;
    uint64_t idx_;
    CORE_MAP<std::string, std::vector<std::string>> states_;
    std::unordered_multimap<std::string, std::vector<std::string>> processes_;
};
}
#endif