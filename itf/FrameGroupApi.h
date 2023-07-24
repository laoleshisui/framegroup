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

typedef void(*FrameGroup_OnConn)(int succeed);
typedef void(*FrameGroup_OnLogin)(int code, int id);
typedef void(*FrameGroup_OnCaptured)();
typedef void(*FrameGroup_OnUpdateId)(int captured, const char* object_type, uint64_t remote_id);
typedef int(*FrameGroup_OnEffect)(uint64_t decider_remote_id, const char* process_type, const char** args, uint64_t other_remote_id, int args_rows, const char* state_type, const char** values, int values_rows);
typedef void (*FrameRender_OnState)(uint64_t remote_id, const char* type, const char** values, int rows);
typedef void (*FrameRender_OnProcess)(uint64_t remote_id, const char* type, const char** args, int rows);

extern void* CreateFrameGroup();
extern void DeleteFrameGroup(void* group);
extern void FrameGroup_Connect(void* group, char* ip, int port);
extern void FrameGroup_DisConnect(void* group);
extern void FrameGroup_Login(void* group);
extern void FrameGroup_Logout(void* group);
extern void FrameGroup_EnterRoom(void* group, uint64_t room_id);
extern void FrameGroup_ExitRoom(void* group, uint64_t room_id);
extern void FrameGroup_SetSaveFrameFilePath(void* group, char* file_path);
extern void FrameGroup_AddCapturer(void* group, uint64_t remote_id, void* capturer);
extern void FrameGroup_RemoveCapturer(void* group, uint64_t remote_id);
extern void FrameGroup_AddCaptureredObjects(void* group, char* object_type, int num_of_objects, int commit);
extern void FrameGroup_AddRender(void* group, uint64_t remote_id, void* render);
extern void FrameGroup_RemoveRender(void* group, uint64_t remote_id);
extern void FrameGroup_RemoveAllIDs(void* group);
extern void FrameGroup_SetCallBack_OnConn(void* group, FrameGroup_OnConn cb);
extern void FrameGroup_SetCallBack_OnCaptured(void* group, FrameGroup_OnCaptured cb);
extern void FrameGroup_SetCallBack_OnLogin(void* group, FrameGroup_OnLogin cb);
extern void FrameGroup_SetCallBack_OnUpdateId(void* group, FrameGroup_OnUpdateId cb);
extern void FrameGroup_SetCallBack_OnEffect(void* group, FrameGroup_OnEffect cb);

extern void* CreateFrameCapturer();
extern void DeleteFrameCapturer(void* capturer);
extern void FrameCapturer_SetState(void* capturer, char* type, char** values, uint32_t rows);
extern void FrameCapturer_AddProcess(void* capturer, char* type, char** args, uint32_t rows);

extern void* CreateFrameRender();
extern void DeleteFrameRender(void* render);
extern void FrameRender_SetCallBack_OnState(void* render, uint64_t remote_id, FrameRender_OnState cb);
extern void FrameRender_SetCallBack_OnProcess(void* render, uint64_t remote_id, FrameRender_OnProcess cb);
#ifdef __cplusplus
}
#endif

#endif