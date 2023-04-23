/*------------------------------------------------------------------------------
 * @file    FrameGroupAPI.h
 * @author  yangxian.aoles@foxmail.com
 * @date    2023/04/09 20:55:11
 *----------------------------------------------------------------------------*/
#ifndef __FRAMEGROUPAPI_H
#define __FRAMEGROUPAPI_H
#include <math.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef void(*FrameGroup_OnLogin)(int code, int id);
typedef void(*FrameGroup_OnUpdateId)(int captured, uint64_t remote_id);
typedef void (*FrameRender_OnState)(const uint32_t type, const char** values, int rows);
typedef void (*FrameRender_OnProcess)(const uint32_t type, const char** args, int rows);

extern void* CreateFrameGroup();
extern void FrameGroup_Connect(void* group, char* ip, int port);
extern void FrameGroup_Login(void* group);
extern void FrameGroup_EnterRoom(void* group, uint64_t room_id);
extern void FrameGroup_ExitRoom(void* group, uint64_t room_id);
extern void FrameGroup_AddCapturer(void* group, uint64_t local_id, void* capturer);
extern void FrameGroup_AddCaptureredObjects(void* group, int num_of_objects);
extern void FrameGroup_AddRender(void* group, uint64_t local_or_remote_id, void* render);
extern void FrameGroup_SetCallBack_OnLogin(void* group, FrameGroup_OnLogin cb);
extern void FrameGroup_SetCallBack_OnUpdateId(void* group, FrameGroup_OnUpdateId cb);

extern void* CreateFrameCapturer();
extern void FrameCapturer_Capture(void* capturer);
extern void FrameCapturer_SetState(void* capturer, uint32_t type, char** values, uint32_t rows);
extern void FrameCapturer_AddProcess(void* capturer, uint32_t type, char** args, uint32_t rows);

extern void* CreateFrameRender();
extern void FrameRender_SetCallBack_OnState(void* render, FrameRender_OnState cb);
extern void FrameRender_SetCallBack_OnProcess(void* render, FrameRender_OnProcess cb);
#ifdef __cplusplus
}
#endif

#endif