/*------------------------------------------------------------------------------
 * @file    FrameRenderApi.h
 * @author  yangxian.aoles@foxmail.com
 * @date    2023/08/21 15:33:19
 *----------------------------------------------------------------------------*/
#ifndef __FRAMERENDERAPI_H
#define __FRAMERENDERAPI_H

#include <memory>
#include <acore/Api.h>

namespace framegroup{
class FrameRenderObserver{
public:
    virtual void OnState(const std::string& type, const std::vector<std::string>& values){}
    virtual void OnProcess(const std::string& type, const std::vector<std::string>& args){}
    virtual void OnState(u_int64_t id, const std::string& type, const std::vector<std::string>& values){}
    virtual void OnProcess(u_int64_t id, const std::string& type, const std::vector<std::string>& args){}
};

class FrameRenderApi : public acore::Api
{
public:
    FrameRenderApi();
    FrameRenderApi(u_int64_t id);
    ~FrameRenderApi();
    void* Get();
    void AddObserver(FrameRenderObserver* observer);
};

}

#endif