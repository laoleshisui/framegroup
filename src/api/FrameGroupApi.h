/*------------------------------------------------------------------------------
 * @file    FrameGroupApi.h
 * @author  yangxian.aoles@foxmail.com
 * @date    2023/08/21 16:47:13
 *----------------------------------------------------------------------------*/
#ifndef __FRAMEGROUPAPI_H
#define __FRAMEGROUPAPI_H
#include <acore/Api.h>
#include <string>

namespace framegroup{

class FrameGroupObserver{
public:
    virtual void OnConn(int succeed){}
    virtual void OnLogin(int code, uint64_t id){}
    virtual void OnEnterRoom(int succeed, uint64_t room_id, int64_t remaining_ms){}
    virtual void OnExitRoom(uint64_t room_id, uint64_t group_id, std::string type, uint64_t remote_id){}
    virtual void OnRoomEnd(uint64_t room_id){}
    virtual void OnCaptured(){}
    virtual void OnUpdateId(int captured, const std::string& object_type, uint64_t remote_id){}
    virtual void OnSyncIFrame(int captured, int succeed, uint64_t remote_id){}
    virtual int OnEffect(uint64_t decider_remote_id, const std::string& process_type, std::vector<std::string>& args, uint64_t other_remote_id, const std::string& state_type, std::vector<std::string>& values){}
    virtual void OnConsumeItem(int succeed, uint64_t remote_id, uint64_t item_id, int count){}
    virtual void OnObtainItem(int succeed, uint64_t remote_id, uint64_t item_id, int count){}
    virtual void OnIterateItem(uint64_t remote_id, uint64_t item_id, int count){}
};

class FrameCapturerApi;
class FrameRenderApi;

class FrameGroupApi : public acore::Api
{
public:
    FrameGroupApi();
    virtual ~FrameGroupApi();

    void Connect(std::string ip, int port);
    void DisConnect();
    void Login();
    void Logout();
    void EnterRoom(uint64_t room_id);
    void ExitRoom(uint64_t room_id);
    
    void SetSaveFrameFilePath(std::string file_path);
    void EnableEffectCaculate(bool enabled);

    void AddCaptureredObjects(std::string object_type, int num_of_objects, bool commit);
    void AddCapturer(uint64_t remote_id, FrameCapturerApi* capturer);
    void RemoveCapturer(uint64_t remote_id);
    void AddRender(uint64_t remote_id, FrameRenderApi* render);
    void RemoveRender(uint64_t remote_id);
    void SyncIFrames(uint64_t remote_id);
    void ConsumeItem(uint64_t remote_id, uint64_t item_id, int count);
    void ObtainItem(uint64_t remote_id, uint64_t item_id, int count);
    void IterateItems(uint64_t remote_id, uint64_t item_id);

    void RemoveAllIDs();

    void AddObserver(FrameGroupObserver* observer);
};
}
#endif