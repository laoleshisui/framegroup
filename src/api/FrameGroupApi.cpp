/*------------------------------------------------------------------------------
 * @file    FrameGroupApi.cpp
 * @author  yangxian.aoles@foxmail.com
 * @date    2023/08/21 17:19:07
 *----------------------------------------------------------------------------*/
#include "FrameGroupApi.h"

#include "FrameRenderApi.h"
#include "FrameCapturerApi.h"

#include <FrameGroup.h>

namespace {
class PIMPL : public acore::Api::PImplItf{
public:
    PIMPL()
    :group_(std::make_unique<framegroup::FrameGroup>())
    {}
    virtual ~PIMPL(){}
    std::unique_ptr<framegroup::FrameGroup> group_;
};
}

using namespace framegroup;

FrameGroupApi::FrameGroupApi()
{
    pimpl_ = std::make_unique<PIMPL>();
}
FrameGroupApi::~FrameGroupApi(){}

void FrameGroupApi::Connect(std::string ip, int port){
    return ((PIMPL*)pimpl_.get())->group_->Connect(std::move(ip), port);
}
void FrameGroupApi::DisConnect(){
    return ((PIMPL*)pimpl_.get())->group_->DisConnect();
}
void FrameGroupApi::Login(){
    return ((PIMPL*)pimpl_.get())->group_->Login();
}
void FrameGroupApi::Logout(){
    return ((PIMPL*)pimpl_.get())->group_->Logout();
}
void FrameGroupApi::EnterRoom(uint64_t room_id){
    return ((PIMPL*)pimpl_.get())->group_->EnterRoom(room_id);
}
void FrameGroupApi::ExitRoom(uint64_t room_id){
    return ((PIMPL*)pimpl_.get())->group_->ExitRoom(room_id);
}

void FrameGroupApi::SetSaveFrameFilePath(std::string file_path){
    return ((PIMPL*)pimpl_.get())->group_->SetSaveFrameFilePath(std::move(file_path));
}

void FrameGroupApi::EnableEffectCaculate(bool enabled){
    return ((PIMPL*)pimpl_.get())->group_->EnableEffectCaculate(enabled);
}

void FrameGroupApi::AddCaptureredObjects(std::string object_type, int num_of_objects, bool commit){
    return ((PIMPL*)pimpl_.get())->group_->AddCaptureredObjects(std::move(object_type), num_of_objects, commit);
}
void FrameGroupApi::AddCapturer(uint64_t remote_id, FrameCapturerApi* capturer){
    return ((PIMPL*)pimpl_.get())->group_->AddCapturer(remote_id, (FrameCapturer*)(capturer->Get()));
}
void FrameGroupApi::RemoveCapturer(uint64_t remote_id){
    return ((PIMPL*)pimpl_.get())->group_->RemoveCapturer(remote_id);
}
void FrameGroupApi::AddRender(uint64_t remote_id, FrameRenderApi* render){
    return ((PIMPL*)pimpl_.get())->group_->AddRender(remote_id, (FrameRender*)(render->Get()));
}
void FrameGroupApi::RemoveRender(uint64_t remote_id){
    return ((PIMPL*)pimpl_.get())->group_->RemoveRender(remote_id);
}
void FrameGroupApi::SyncIFrames(uint64_t remote_id){
    return ((PIMPL*)pimpl_.get())->group_->SyncIFrames(remote_id);
}
void FrameGroupApi::ConsumeItem(uint64_t remote_id, uint64_t item_id, int count){
    return ((PIMPL*)pimpl_.get())->group_->ConsumeItem(remote_id, item_id, count);
}
void FrameGroupApi::ObtainItem(uint64_t remote_id, uint64_t item_id, int count){
    return ((PIMPL*)pimpl_.get())->group_->ObtainItem(remote_id, item_id, count);
}
void FrameGroupApi::IterateItems(uint64_t remote_id, uint64_t item_id){
    return ((PIMPL*)pimpl_.get())->group_->IterateItems(remote_id, item_id);
}

void FrameGroupApi::RemoveAllIDs(){
    return ((PIMPL*)pimpl_.get())->group_->RemoveAllIDs();
}

void FrameGroupApi::AddObserver(FrameGroupObserver* observer){
    return ((PIMPL*)pimpl_.get())->group_->AddObserver(observer);
}