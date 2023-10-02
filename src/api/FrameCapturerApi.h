/*------------------------------------------------------------------------------
 * @file    FrameCapturerApi.h
 * @author  yangxian.aoles@foxmail.com
 * @date    2023/08/21 16:29:49
 *----------------------------------------------------------------------------*/
#ifndef __FRAMECAPTURERAPI_H
#define __FRAMECAPTURERAPI_H

#include <acore/Api.h>

namespace framegroup{
class FrameCapturerApi : public acore::Api
{
public:
    FrameCapturerApi();
    virtual ~FrameCapturerApi();

    void* Get();
    void SetState(std::string type, std::vector<std::string> values);
    void SendIFrame();
    void AddProcess(std::string type, std::vector<std::string> args);
};

}

#endif