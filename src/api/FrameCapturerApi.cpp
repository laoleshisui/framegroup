/*------------------------------------------------------------------------------
 * @file    FrameCapturerApi.cpp
 * @author  yangxian.aoles@foxmail.com
 * @date    2023/08/21 16:31:12
 *----------------------------------------------------------------------------*/
#include "FrameCapturerApi.h"

#include <FrameCapturer.h>
namespace {
class PIMPL : public acore::Api::PImplItf{
public:
    PIMPL()
    :capturer_(std::make_unique<framegroup::FrameCapturer>())
    {}
    virtual ~PIMPL(){}
    std::unique_ptr<framegroup::FrameCapturer> capturer_;
};
}

using namespace framegroup;

FrameCapturerApi::FrameCapturerApi()
{
    pimpl_ = std::make_unique<PIMPL>();
}
FrameCapturerApi::~FrameCapturerApi(){}

void* FrameCapturerApi::Get(){
    return ((PIMPL*)pimpl_.get())->capturer_.get();
}

void FrameCapturerApi::SetState(std::string type, std::vector<std::string> values){
    return ((PIMPL*)pimpl_.get())->capturer_->SetState(std::move(type), std::move(values));
}
void FrameCapturerApi::SendIFrame(){
    return ((PIMPL*)pimpl_.get())->capturer_->SendIFrame();
}
void FrameCapturerApi::AddProcess(std::string type, std::vector<std::string> args, bool override){
    return ((PIMPL*)pimpl_.get())->capturer_->AddProcess(std::move(type), std::move(args), override);
}