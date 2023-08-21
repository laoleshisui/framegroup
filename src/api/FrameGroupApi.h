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
    virtual void OnLogin(int code, int id){}
    virtual void OnCaptured(){}
    virtual void OnUpdateId(int captured, const std::string& object_type, uint64_t remote_id){}
    virtual int OnEffect(uint64_t decider_remote_id, const std::string& process_type, std::vector<std::string>& args, uint64_t other_remote_id, const std::string& state_type, std::vector<std::string>& values){}
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

    void RemoveAllIDs();

    void AddObserver(FrameGroupObserver* observer);
};
}
#endif