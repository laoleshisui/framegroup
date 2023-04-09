/*------------------------------------------------------------------------------
 * @file    FrameGroupAPI.h
 * @author  YangXian(1539807453@qq.com)
 * @date    2023/04/09 20:55:11
 *----------------------------------------------------------------------------*/
#ifndef __FRAMEGROUPAPI_H
#define __FRAMEGROUPAPI_H

#include <math.h>

extern "C"{

extern void* CreateFrameGroup();
extern void FrameGroup_Connect(void* group, char* ip, int port);
extern void FrameGroup_Login(void* group);
extern void FrameGroup_EnterRoom();
extern void FrameGroup_ExitRoom(void* group, uint64_t room_id);
extern void FrameGroup_AddCapturer(void* group, uint64_t local_id, void* capturer);
extern void FrameGroup_RegisterCaptureredOnServer(void* group);
extern void FrameGroup_AddRender(void* group, uint64_t remote_id, void* render);
extern void FrameGroup_SetCallBack_OnUpdateCapturedLocalId(void* group, void(*cb)(uint64_t local_id, uint64_t remote_id));
extern void FrameGroup_SetCallBack_OnUpdateUncapturedRemoteId(void* group, void(*cb)(uint64_t remote_id));
    
}

#endif