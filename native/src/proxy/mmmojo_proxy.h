#pragma once

#include <windows.h>

// mmmojo_64.dll 导出函数类型定义
// 基于 IDA Pro 分析的真正函数签名（已验证）

// 环境管理
typedef __int64 (*CreateMMMojoEnvironment_t)();
typedef __int64 (*RemoveMMMojoEnvironment_t)(__int64 env);
typedef __int64 (*StartMMMojoEnvironment_t)(__int64 env);
typedef __int64 (*StopMMMojoEnvironment_t)(__int64 env);
typedef __int64 (*ShutdownMMMojo_t)();
typedef __int64 (*InitializeMMMojo_t)();

// 写入信息
typedef __int64 (*CreateMMMojoWriteInfo_t)(unsigned int a1, unsigned int a2, unsigned int a3);
typedef __int64 (*RemoveMMMojoWriteInfo_t)(__int64 info);
typedef __int64 (*SendMMMojoWriteInfo_t)(__int64 env, __int64 info);
typedef __int64 (*GetMMMojoWriteInfoAttach_t)(__int64 info, unsigned int size);
typedef unsigned __int64 (*GetMMMojoWriteInfoRequest_t)(__int64 info, unsigned int size);
typedef void (*SetMMMojoWriteInfoMessagePipe_t)(__int64 info, int pipe);
typedef void (*SetMMMojoWriteInfoResponseSync_t)(__int64 info, __int64 response);
typedef __int64 (*SwapMMMojoWriteInfoCallback_t)(__int64 info, __int64 callback);
typedef __int64 (*SwapMMMojoWriteInfoMessage_t)(__int64 info, __int64 message);

// 读取信息
typedef __int64 (*RemoveMMMojoReadInfo_t)(__int64 info);
typedef __int64 (*GetMMMojoReadInfoAttach_t)(__int64 info, DWORD* size);
typedef __int64 (*GetMMMojoReadInfoMethod_t)(unsigned int* info);
typedef __int64 (*GetMMMojoReadInfoRequest_t)(__int64 info, DWORD* size);
typedef bool (*GetMMMojoReadInfoSync_t)(__int64 info);

// 配置
typedef va_list (*SetMMMojoConfiguration_t)(int a1, ...);
typedef unsigned __int64 (*SetMMMojoEnvironmentCallbacks_t)(__int64 env, __int64 callbacks, ...);
typedef unsigned __int64 (*SetMMMojoEnvironmentInitParams_t)(__int64 env, __int64 params, ...);

// 其他
typedef void (*AppendMMSubProcessSwitchNative_t)(__int64 env, __int64 name, __int64 value);
typedef __int64 (*GetHandleVerifier_t)();
typedef bool (*IsSandboxedProcess_t)();

// 代理 DLL 初始化
bool InitMmmojoProxy();
void CleanupMmmojoProxy();

// 获取真正的函数地址
void* GetRealFunction(const char* name);
