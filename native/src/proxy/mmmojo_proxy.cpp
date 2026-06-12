#include "mmmojo_proxy.h"
#include <map>
#include <string>
#include <fstream>

// 真正的 mmmojo_64.dll 模块句柄
static HMODULE g_real_mmojo = nullptr;

// 函数指针缓存
static std::map<std::string, void*> g_functions;

// 简单日志
static void Log(const char* msg) {
    // 输出到调试器
    OutputDebugStringA(msg);
    OutputDebugStringA("\n");

    // 写入文件（使用固定路径）
    static std::ofstream log_file;
    static bool initialized = false;

    if (!initialized) {
        log_file.open("C:\\Users\\woshi\\Downloads\\PQY\\logs\\mmojo_proxy.log", std::ios::app);
        initialized = true;
    }

    if (log_file.is_open()) {
        log_file << msg << std::endl;
    }
}

// 加载真正的 mmmojo_64.dll
bool InitMmmojoProxy() {
    if (g_real_mmojo) {
        return true;
    }

    Log("InitMmmojoProxy: Loading mmmojo_real.dll...");

    // 加载真正的 mmmojo_64.dll（重命名后的文件）
    g_real_mmojo = LoadLibraryA("mmmojo_real.dll");
    if (!g_real_mmojo) {
        Log("InitMmmojoProxy: Failed to load mmmojo_real.dll");
        return false;
    }

    Log("InitMmmojoProxy: Loaded mmmojo_real.dll successfully");
    return true;
}

// 清理
void CleanupMmmojoProxy() {
    if (g_real_mmojo) {
        FreeLibrary(g_real_mmojo);
        g_real_mmojo = nullptr;
    }
    g_functions.clear();
}

// 获取真正的函数地址
void* GetRealFunction(const char* name) {
    if (!g_real_mmojo) {
        return nullptr;
    }

    // 检查缓存
    auto it = g_functions.find(name);
    if (it != g_functions.end()) {
        return it->second;
    }

    // 获取函数地址
    void* addr = (void*)GetProcAddress(g_real_mmojo, name);
    if (addr) {
        g_functions[name] = addr;
    }

    return addr;
}

// ============================================================
// 代理函数实现（基于 IDA Pro 分析的真正签名）
// ============================================================

// CreateMMMojoEnvironment
extern "C" __declspec(dllexport)
__int64 CreateMMMojoEnvironment() {
    auto real_func = (CreateMMMojoEnvironment_t)GetRealFunction("CreateMMMojoEnvironment");
    if (!real_func) {
        Log("CreateMMMojoEnvironment: real function not found");
        return 0;
    }

    return real_func();
}

// RemoveMMMojoEnvironment
extern "C" __declspec(dllexport)
__int64 RemoveMMMojoEnvironment(__int64 env) {
    auto real_func = (RemoveMMMojoEnvironment_t)GetRealFunction("RemoveMMMojoEnvironment");
    if (!real_func) {
        return 0;
    }

    return real_func(env);
}

// StartMMMojoEnvironment
extern "C" __declspec(dllexport)
__int64 StartMMMojoEnvironment(__int64 env) {
    auto real_func = (StartMMMojoEnvironment_t)GetRealFunction("StartMMMojoEnvironment");
    if (!real_func) {
        return 0;
    }

    return real_func(env);
}

// StopMMMojoEnvironment
extern "C" __declspec(dllexport)
__int64 StopMMMojoEnvironment(__int64 env) {
    auto real_func = (StopMMMojoEnvironment_t)GetRealFunction("StopMMMojoEnvironment");
    if (!real_func) {
        return 0;
    }

    return real_func(env);
}

// ShutdownMMMojo
extern "C" __declspec(dllexport)
__int64 ShutdownMMMojo() {
    auto real_func = (ShutdownMMMojo_t)GetRealFunction("ShutdownMMMojo");
    if (!real_func) {
        return 0;
    }

    return real_func();
}

// InitializeMMMojo
extern "C" __declspec(dllexport)
__int64 InitializeMMMojo() {
    auto real_func = (InitializeMMMojo_t)GetRealFunction("InitializeMMMojo");
    if (!real_func) {
        return 0;
    }

    return real_func();
}

// CreateMMMojoWriteInfo
extern "C" __declspec(dllexport)
__int64 CreateMMMojoWriteInfo(unsigned int a1, unsigned int a2, unsigned int a3) {
    auto real_func = (CreateMMMojoWriteInfo_t)GetRealFunction("CreateMMMojoWriteInfo");
    if (!real_func) {
        return 0;
    }

    return real_func(a1, a2, a3);
}

// RemoveMMMojoWriteInfo
extern "C" __declspec(dllexport)
__int64 RemoveMMMojoWriteInfo(__int64 info) {
    auto real_func = (RemoveMMMojoWriteInfo_t)GetRealFunction("RemoveMMMojoWriteInfo");
    if (!real_func) {
        return 0;
    }

    return real_func(info);
}

// SendMMMojoWriteInfo
extern "C" __declspec(dllexport)
__int64 SendMMMojoWriteInfo(__int64 env, __int64 info) {
    auto real_func = (SendMMMojoWriteInfo_t)GetRealFunction("SendMMMojoWriteInfo");
    if (!real_func) {
        return 0;
    }

    return real_func(env, info);
}

// GetMMMojoWriteInfoAttach
extern "C" __declspec(dllexport)
__int64 GetMMMojoWriteInfoAttach(__int64 info, unsigned int size) {
    auto real_func = (GetMMMojoWriteInfoAttach_t)GetRealFunction("GetMMMojoWriteInfoAttach");
    if (!real_func) {
        return 0;
    }

    return real_func(info, size);
}

// GetMMMojoWriteInfoRequest
extern "C" __declspec(dllexport)
unsigned __int64 GetMMMojoWriteInfoRequest(__int64 info, unsigned int size) {
    auto real_func = (GetMMMojoWriteInfoRequest_t)GetRealFunction("GetMMMojoWriteInfoRequest");
    if (!real_func) {
        return 0;
    }

    return real_func(info, size);
}

// SetMMMojoWriteInfoMessagePipe
extern "C" __declspec(dllexport)
void SetMMMojoWriteInfoMessagePipe(__int64 info, int pipe) {
    auto real_func = (SetMMMojoWriteInfoMessagePipe_t)GetRealFunction("SetMMMojoWriteInfoMessagePipe");
    if (real_func) {
        real_func(info, pipe);
    }
}

// SetMMMojoWriteInfoResponseSync
extern "C" __declspec(dllexport)
void SetMMMojoWriteInfoResponseSync(__int64 info, __int64 response) {
    auto real_func = (SetMMMojoWriteInfoResponseSync_t)GetRealFunction("SetMMMojoWriteInfoResponseSync");
    if (real_func) {
        real_func(info, response);
    }
}

// SwapMMMojoWriteInfoCallback
extern "C" __declspec(dllexport)
__int64 SwapMMMojoWriteInfoCallback(__int64 info, __int64 callback) {
    auto real_func = (SwapMMMojoWriteInfoCallback_t)GetRealFunction("SwapMMMojoWriteInfoCallback");
    if (!real_func) {
        return 0;
    }

    return real_func(info, callback);
}

// SwapMMMojoWriteInfoMessage
extern "C" __declspec(dllexport)
__int64 SwapMMMojoWriteInfoMessage(__int64 info, __int64 message) {
    auto real_func = (SwapMMMojoWriteInfoMessage_t)GetRealFunction("SwapMMMojoWriteInfoMessage");
    if (!real_func) {
        return 0;
    }

    return real_func(info, message);
}

// RemoveMMMojoReadInfo
extern "C" __declspec(dllexport)
__int64 RemoveMMMojoReadInfo(__int64 info) {
    auto real_func = (RemoveMMMojoReadInfo_t)GetRealFunction("RemoveMMMojoReadInfo");
    if (!real_func) {
        return 0;
    }

    return real_func(info);
}

// GetMMMojoReadInfoAttach
extern "C" __declspec(dllexport)
__int64 GetMMMojoReadInfoAttach(__int64 info, DWORD* size) {
    auto real_func = (GetMMMojoReadInfoAttach_t)GetRealFunction("GetMMMojoReadInfoAttach");
    if (!real_func) {
        return 0;
    }

    return real_func(info, size);
}

// GetMMMojoReadInfoMethod
extern "C" __declspec(dllexport)
__int64 GetMMMojoReadInfoMethod(unsigned int* info) {
    auto real_func = (GetMMMojoReadInfoMethod_t)GetRealFunction("GetMMMojoReadInfoMethod");
    if (!real_func) {
        return 0;
    }

    return real_func(info);
}

// GetMMMojoReadInfoRequest
extern "C" __declspec(dllexport)
__int64 GetMMMojoReadInfoRequest(__int64 info, DWORD* size) {
    auto real_func = (GetMMMojoReadInfoRequest_t)GetRealFunction("GetMMMojoReadInfoRequest");
    if (!real_func) {
        return 0;
    }

    return real_func(info, size);
}

// GetMMMojoReadInfoSync
extern "C" __declspec(dllexport)
bool GetMMMojoReadInfoSync(__int64 info) {
    auto real_func = (GetMMMojoReadInfoSync_t)GetRealFunction("GetMMMojoReadInfoSync");
    if (!real_func) {
        return false;
    }

    return real_func(info);
}

// SetMMMojoConfiguration (可变参数函数)
extern "C" __declspec(dllexport)
va_list SetMMMojoConfiguration(int a1, ...) {
    auto real_func = (SetMMMojoConfiguration_t)GetRealFunction("SetMMMojoConfiguration");
    if (!real_func) {
        return nullptr;
    }

    va_list args;
    va_start(args, a1);
    va_list result = real_func(a1, args);
    va_end(args);
    return result;
}

// SetMMMojoEnvironmentCallbacks (可变参数函数)
extern "C" __declspec(dllexport)
unsigned __int64 SetMMMojoEnvironmentCallbacks(__int64 env, __int64 callbacks, ...) {
    auto real_func = (SetMMMojoEnvironmentCallbacks_t)GetRealFunction("SetMMMojoEnvironmentCallbacks");
    if (!real_func) {
        return 0;
    }

    va_list args;
    va_start(args, callbacks);
    unsigned __int64 result = real_func(env, callbacks, args);
    va_end(args);
    return result;
}

// SetMMMojoEnvironmentInitParams (可变参数函数)
extern "C" __declspec(dllexport)
unsigned __int64 SetMMMojoEnvironmentInitParams(__int64 env, __int64 params, ...) {
    auto real_func = (SetMMMojoEnvironmentInitParams_t)GetRealFunction("SetMMMojoEnvironmentInitParams");
    if (!real_func) {
        return 0;
    }

    va_list args;
    va_start(args, params);
    unsigned __int64 result = real_func(env, params, args);
    va_end(args);
    return result;
}

// AppendMMSubProcessSwitchNative
extern "C" __declspec(dllexport)
void AppendMMSubProcessSwitchNative(__int64 env, __int64 name, __int64 value) {
    auto real_func = (AppendMMSubProcessSwitchNative_t)GetRealFunction("AppendMMSubProcessSwitchNative");
    if (real_func) {
        real_func(env, name, value);
    }
}

// GetHandleVerifier
extern "C" __declspec(dllexport)
__int64 GetHandleVerifier() {
    auto real_func = (GetHandleVerifier_t)GetRealFunction("GetHandleVerifier");
    if (!real_func) {
        return 0;
    }

    return real_func();
}

// IsSandboxedProcess
extern "C" __declspec(dllexport)
bool IsSandboxedProcess() {
    auto real_func = (IsSandboxedProcess_t)GetRealFunction("IsSandboxedProcess");
    if (!real_func) {
        return false;
    }

    return real_func();
}

// ============================================================
// DLL 入口点
// ============================================================

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(hModule);
            Log("DllMain: DLL_PROCESS_ATTACH");

            // 初始化代理
            if (!InitMmmojoProxy()) {
                Log("DllMain: Failed to initialize mmmojo proxy");
                return FALSE;
            }

            // 调用 InitializeMMMojo 初始化 mmojo
            auto init_func = (InitializeMMMojo_t)GetRealFunction("InitializeMMMojo");
            if (init_func) {
                Log("DllMain: Calling InitializeMMMojo...");
                init_func();
                Log("DllMain: InitializeMMMojo completed");
            }

            Log("DllMain: Mmmojo proxy initialized successfully");
            break;
        }

        case DLL_PROCESS_DETACH: {
            Log("DllMain: DLL_PROCESS_DETACH");
            CleanupMmmojoProxy();
            break;
        }

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;
    }

    return TRUE;
}


