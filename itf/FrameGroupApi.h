/*------------------------------------------------------------------------------
 * @file    FrameGroupAPI.h
 * @author  YangXian(1539807453@qq.com)
 * @date    2023/04/09 20:55:11
 *----------------------------------------------------------------------------*/
#ifndef __FRAMEGROUPAPI_H
#define __FRAMEGROUPAPI_H
#include <math.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef void(*FrameGroup_OnLogin)(int code, int id);
typedef void(*FrameGroup_OnUpdateId)(uint64_t local_id, uint64_t remote_id);
typedef void(*FrameRender_OnMoveTo)(float x, float y);
typedef void(*FrameRender_OnOperate)(uint32_t type, const char** args, uint32_t rows);
typedef void(*FrameRender_OnHealth)(uint32_t health);


extern void* CreateFrameGroup();
extern void FrameGroup_Connect(void* group, char* ip, int port);
extern void FrameGroup_Login(void* group);
extern void FrameGroup_EnterRoom(void* group, uint64_t room_id);
extern void FrameGroup_ExitRoom(void* group, uint64_t room_id);
extern void FrameGroup_AddCapturer(void* group, uint64_t local_id, void* capturer);
extern void FrameGroup_RegisterCaptureredOnServer(void* group);
extern void FrameGroup_AddRender(void* group, uint64_t local_or_remote_id, void* render);
extern void FrameGroup_SetCallBack_OnLogin(void* group, FrameGroup_OnLogin cb);
extern void FrameGroup_SetCallBack_OnUpdateId(void* group, FrameGroup_OnUpdateId cb);

extern void* CreateFrameCapturer();
extern void FrameCapturer_Capture(void* capturer);
extern void FrameCapturer_AddOperation(void* capturer, uint32_t type, char** args, uint32_t rows);
extern void FrameCapturer_AddDeltaHealth(void* capturer, int32_t delta);
extern void FrameCapturer_MoveTo(void* capturer, float x, float y);

extern void* CreateFrameRender();
extern void FrameRender_SetCallBack_OnMoveTo(void* render, FrameRender_OnMoveTo cb);
extern void FrameRender_SetCallBack_OnOperate(void* render, FrameRender_OnOperate cb);
extern void FrameRender_SetCallBack_OnHealth(void* render, FrameRender_OnHealth cb);
#ifdef __cplusplus
}
#endif

#endif