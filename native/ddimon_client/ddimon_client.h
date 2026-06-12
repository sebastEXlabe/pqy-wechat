// PQY DdiMon Client Library
// User mode library for communicating with DdiMon driver

#pragma once

#include <windows.h>
#include <string>
#include <functional>
#include <map>
#include <mutex>

// IOCTL codes (must match driver)
#define IOCTL_INSTALL_USER_HOOK \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_REMOVE_USER_HOOK \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_CALL_ORIGINAL \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_GET_HOOK_INFO \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Request structures (must match driver)
typedef struct _INSTALL_HOOK_REQUEST {
    ULONG64 target_address;
    ULONG64 handler_address;
    ULONG64 original_address;
    ULONG process_id;
    WCHAR module_name[260];
    CHAR function_name[260];
} INSTALL_HOOK_REQUEST, *PINSTALL_HOOK_REQUEST;

typedef struct _REMOVE_HOOK_REQUEST {
    ULONG64 target_address;
    ULONG process_id;
} REMOVE_HOOK_REQUEST, *PREMOVE_HOOK_REQUEST;

typedef struct _CALL_ORIGINAL_REQUEST {
    ULONG64 original_address;
    ULONG64 args[4];
    ULONG64 result;
    ULONG process_id;
} CALL_ORIGINAL_REQUEST, *PCALL_ORIGINAL_REQUEST;

typedef struct _HOOK_INFO {
    ULONG64 target_address;
    ULONG64 handler_address;
    ULONG64 original_address;
    ULONG process_id;
    BOOL is_active;
} HOOK_INFO, *PHOOK_INFO;

typedef struct _GET_HOOK_INFO_REQUEST {
    ULONG64 target_address;
    ULONG process_id;
    HOOK_INFO info;
} GET_HOOK_INFO_REQUEST, *PGET_HOOK_INFO_REQUEST;

// Hook handler callback type
typedef void (*HookHandlerCallback)(
    ULONG64* args,      // rcx, rdx, r8, r9
    ULONG64* result,    // Return value
    void* context
);

// DdiMon Client class
class DdiMonClient {
public:
    DdiMonClient();
    ~DdiMonClient();

    // Connect to driver
    bool Connect();
    void Disconnect();
    bool IsConnected() const { return device_handle_ != INVALID_HANDLE_VALUE; }

    // Hook management
    bool InstallHook(
        const std::string& module_name,
        const std::string& function_name,
        HookHandlerCallback handler,
        void* context = nullptr
    );

    bool InstallHook(
        ULONG64 target_address,
        HookHandlerCallback handler,
        void* context = nullptr
    );

    bool RemoveHook(ULONG64 target_address);

    // Get hook info
    bool GetHookInfo(ULONG64 target_address, HOOK_INFO* info);

    // Call original function
    ULONG64 CallOriginal(ULONG64 original_address, ULONG64* args);

private:
    HANDLE device_handle_;
    std::mutex hooks_mutex_;
    std::map<ULONG64, std::pair<HookHandlerCallback, void*>> hooks_;

    // Internal helper
    bool SendIoControl(DWORD code, void* in_buf, DWORD in_size, void* out_buf, DWORD out_size);
};

// Global instance
extern DdiMonClient g_ddimon_client;
