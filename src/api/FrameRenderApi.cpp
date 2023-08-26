/*------------------------------------------------------------------------------
 * @file    FrameRenderApi.cpp
 * @author  yangxian.aoles@foxmail.com
 * @date    2023/08/21 15:45:59
 *----------------------------------------------------------------------------*/
#include "FrameRenderApi.h"

#include <FrameRender.h>

namespace {
class PIMPL : public acore::Api::PImplItf{
public:
    PIMPL()
    :PIMPL(0)
    {}
    PIMPL(u_int64_t id)
    :render_(std::make_unique<framegroup::FrameRender>(id))
    {}
    virtual ~PIMPL(){}
    std::unique_ptr<framegroup::FrameRender> render_;
};
}

using namespace framegroup;

FrameRenderApi::FrameRenderApi()
:FrameRenderApi(0)
{}

FrameRenderApi::FrameRenderApi(u_int64_t id){
    pimpl_ = std::make_unique<PIMPL>(id);
}

FrameRenderApi::~FrameRenderApi(){}

void* FrameRenderApi::Get(){
    return ((PIMPL*)pimpl_.get())->render_.get();
}

void FrameRenderApi::AddObserver(FrameRenderObserver* observer) {
    return ((PIMPL*)pimpl_.get())->render_->AddObserver(observer);
}