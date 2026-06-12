// PQY DdiMon Proxy DLL
// Uses DdiMon driver for invisible inline hooks

#include <windows.h>
#include <string>
#include <map>
#include <mutex>
#include <fstream>
#include "../ddimon_client/ddimon_client.h"

// Function signatures (from IDA Pro analysis)
typedef __int64 (*SendMMMojoWriteInfo_t)(__int64 env, __int64 info);
typedef __int64 (*CreateMMMojoEnvironment_t)();
typedef __int64 (*RemoveMMMojoEnvironment_t)(__int64 env);
typedef __int64 (*InitializeMMMojo_t)();

// Original function addresses
static SendMMMojoWriteInfo_t Original_SendMMMojoWriteInfo = nullptr;
static CreateMMMojoEnvironment_t Original_CreateMMMojoEnvironment = nullptr;
static RemoveMMMojoEnvironment_t Original_RemoveMMMojoEnvironment = nullptr;
static InitializeMMMojo_t Original_InitializeMMMojo = nullptr;

// Module handle
static HMODULE g_mmojo_module = nullptr;

// Simple logger
static void Log(const char* msg) {
    OutputDebugStringA(msg);
    OutputDebugStringA("\n");

    static std::ofstream log_file;
    static bool initialized = false;
    if (!initialized) {
        log_file.open("C:\\Users\\woshi\\Downloads\\PQY\\logs\\ddimon_proxy.log", std::ios::app);
        initialized = true;
    }
    if (log_file.is_open()) {
        log_file << msg << std::endl;
    }
}

// Hook handlers
void HandleSendMMMojoWriteInfo(ULONG64* args, ULONG64* result, void* context) {
    __int64 env = (__int64)args[0];
    __int64 info = (__int64)args[1];

    char buf[256];
    snprintf(buf, sizeof(buf), "SendMMMojoWriteInfo called: env=%lld info=%lld", env, info);
    Log(buf);

    // Call original function
    if (Original_SendMMMojoWriteInfo) {
        *result = (ULONG64)Original_SendMMMojoWriteInfo(env, info);
    } else {
        *result = 0;
    }
}

void HandleCreateMMMojoEnvironment(ULONG64* args, ULONG64* result, void* context) {
    Log("CreateMMMojoEnvironment called");

    // Call original function
    if (Original_CreateMMMojoEnvironment) {
        *result = (ULONG64)Original_CreateMMMojoEnvironment();
    } else {
        *result = 0;
    }
}

void HandleRemoveMMMojoEnvironment(ULONG64* args, ULONG64* result, void* context) {
    __int64 env = (__int64)args[0];

    char buf[256];
    snprintf(buf, sizeof(buf), "RemoveMMMojoEnvironment called: env=%lld", env);
    Log(buf);

    // Call original function
    if (Original_RemoveMMMojoEnvironment) {
        *result = (ULONG64)Original_RemoveMMMojoEnvironment(env);
    } else {
        *result = 0;
    }
}

// Install hooks using DdiMon
bool InstallDdiMonHooks() {
    Log("Installing DdiMon hooks...");

    // Get mmojo module
    g_mmojo_module = GetModuleHandleA("mmmojo_64.dll");
    if (!g_mmojo_module) {
        Log("mmmojo_64.dll not loaded");
        return false;
    }

    // Connect to DdiMon driver
    if (!g_ddimon_client.Connect()) {
        Log("Failed to connect to DdiMon driver");
        return false;
    }

    // Hook SendMMMojoWriteInfo
    FARPROC send_func = GetProcAddress(g_mmojo_module, "SendMMMojoWriteInfo");
    if (send_func) {
        if (g_ddimon_client.InstallHook(
            reinterpret_cast<ULONG64>(send_func),
            HandleSendMMMojoWriteInfo
        )) {
            // Get original function address
            HOOK_INFO info;
            if (g_ddimon_client.GetHookInfo(reinterpret_cast<ULONG64>(send_func), &info)) {
                Original_SendMMMojoWriteInfo = reinterpret_cast<SendMMMojoWriteInfo_t>(info.original_address);
            }
            Log("Hooked SendMMMojoWriteInfo");
        }
    }

    // Hook CreateMMMojoEnvironment
    FARPROC create_func = GetProcAddress(g_mmojo_module, "CreateMMMojoEnvironment");
    if (create_func) {
        if (g_ddimon_client.InstallHook(
            reinterpret_cast<ULONG64>(create_func),
            HandleCreateMMMojoEnvironment
        )) {
            HOOK_INFO info;
            if (g_ddimon_client.GetHookInfo(reinterpret_cast<ULONG64>(create_func), &info)) {
                Original_CreateMMMojoEnvironment = reinterpret_cast<CreateMMMojoEnvironment_t>(info.original_address);
            }
            Log("Hooked CreateMMMojoEnvironment");
        }
    }

    // Hook RemoveMMMojoEnvironment
    FARPROC remove_func = GetProcAddress(g_mmojo_module, "RemoveMMMojoEnvironment");
    if (remove_func) {
        if (g_ddimon_client.InstallHook(
            reinterpret_cast<ULONG64>(remove_func),
            HandleRemoveMMMojoEnvironment
        )) {
            HOOK_INFO info;
            if (g_ddimon_client.GetHookInfo(reinterpret_cast<ULONG64>(remove_func), &info)) {
                Original_RemoveMMMojoEnvironment = reinterpret_cast<RemoveMMMojoEnvironment_t>(info.original_address);
            }
            Log("Hooked RemoveMMMojoEnvironment");
        }
    }

    Log("DdiMon hooks installed successfully");
    return true;
}

// DLL entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(hModule);
            Log("DllMain: DLL_PROCESS_ATTACH");

            // Install hooks
            if (!InstallDdiMonHooks()) {
                Log("Failed to install DdiMon hooks");
                // Don't return FALSE - we can still work without hooks
            }

            Log("DllMain: DdiMon proxy initialized");
            break;
        }

        case DLL_PROCESS_DETACH: {
            Log("DllMain: DLL_PROCESS_DETACH");

            // Remove hooks
            for (auto& [target, handler] : g_ddimon_client.GetHooks()) {
                g_ddimon_client.RemoveHook(target);
            }

            g_ddimon_client.Disconnect();
            break;
        }

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;
    }

    return TRUE;
}
