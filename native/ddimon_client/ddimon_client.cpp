// PQY DdiMon Client Library
// User mode library for communicating with DdiMon driver

#include "ddimon_client.h"
#include <iostream>
#include <spdlog/spdlog.h>

// Global instance
DdiMonClient g_ddimon_client;

DdiMonClient::DdiMonClient()
    : device_handle_(INVALID_HANDLE_VALUE) {
}

DdiMonClient::~DdiMonClient() {
    Disconnect();
}

bool DdiMonClient::Connect() {
    if (device_handle_ != INVALID_HANDLE_VALUE) {
        return true;  // Already connected
    }

    device_handle_ = CreateFileA(
        "\\\\.\\DdiMon",
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (device_handle_ == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        spdlog::error("Failed to connect to DdiMon driver: error={}", error);
        return false;
    }

    spdlog::info("Connected to DdiMon driver");
    return true;
}

void DdiMonClient::Disconnect() {
    if (device_handle_ != INVALID_HANDLE_VALUE) {
        CloseHandle(device_handle_);
        device_handle_ = INVALID_HANDLE_VALUE;
    }
}

bool DdiMonClient::InstallHook(
    const std::string& module_name,
    const std::string& function_name,
    HookHandlerCallback handler,
    void* context
) {
    // Find function address
    HMODULE module = GetModuleHandleA(module_name.c_str());
    if (!module) {
        spdlog::error("Module not found: {}", module_name);
        return false;
    }

    FARPROC func_addr = GetProcAddress(module, function_name.c_str());
    if (!func_addr) {
        spdlog::error("Function not found: {}", function_name);
        return false;
    }

    return InstallHook(reinterpret_cast<ULONG64>(func_addr), handler, context);
}

bool DdiMonClient::InstallHook(
    ULONG64 target_address,
    HookHandlerCallback handler,
    void* context
) {
    if (!Connect()) {
        return false;
    }

    // Create a trampoline function that calls our handler
    // For now, we'll use a simple approach with a wrapper function

    INSTALL_HOOK_REQUEST request = {};
    request.target_address = target_address;
    request.handler_address = 0;  // Will be set by driver
    request.original_address = 0;
    request.process_id = GetCurrentProcessId();

    // Send request to driver
    if (!SendIoControl(IOCTL_INSTALL_USER_HOOK, &request, sizeof(request), &request, sizeof(request))) {
        spdlog::error("Failed to install hook at {:016x}", target_address);
        return false;
    }

    // Store handler callback
    {
        std::lock_guard<std::mutex> lock(hooks_mutex_);
        hooks_[target_address] = {handler, context};
    }

    spdlog::info("Hook installed at {:016x}, original={:016x}",
        target_address, request.original_address);

    return true;
}

bool DdiMonClient::RemoveHook(ULONG64 target_address) {
    if (!Connect()) {
        return false;
    }

    REMOVE_HOOK_REQUEST request = {};
    request.target_address = target_address;
    request.process_id = GetCurrentProcessId();

    if (!SendIoControl(IOCTL_REMOVE_USER_HOOK, &request, sizeof(request), nullptr, 0)) {
        spdlog::error("Failed to remove hook at {:016x}", target_address);
        return false;
    }

    // Remove handler callback
    {
        std::lock_guard<std::mutex> lock(hooks_mutex_);
        hooks_.erase(target_address);
    }

    spdlog::info("Hook removed at {:016x}", target_address);
    return true;
}

bool DdiMonClient::GetHookInfo(ULONG64 target_address, HOOK_INFO* info) {
    if (!Connect()) {
        return false;
    }

    GET_HOOK_INFO_REQUEST request = {};
    request.target_address = target_address;
    request.process_id = GetCurrentProcessId();

    if (!SendIoControl(IOCTL_GET_HOOK_INFO, &request, sizeof(request), &request, sizeof(request))) {
        return false;
    }

    if (info) {
        *info = request.info;
    }

    return true;
}

ULONG64 DdiMonClient::CallOriginal(ULONG64 original_address, ULONG64* args) {
    if (!Connect()) {
        return 0;
    }

    CALL_ORIGINAL_REQUEST request = {};
    request.original_address = original_address;
    request.process_id = GetCurrentProcessId();

    if (args) {
        memcpy(request.args, args, sizeof(request.args));
    }

    if (!SendIoControl(IOCTL_CALL_ORIGINAL, &request, sizeof(request), &request, sizeof(request))) {
        spdlog::error("Failed to call original at {:016x}", original_address);
        return 0;
    }

    return request.result;
}

bool DdiMonClient::SendIoControl(DWORD code, void* in_buf, DWORD in_size, void* out_buf, DWORD out_size) {
    if (device_handle_ == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD bytes_returned = 0;
    BOOL result = DeviceIoControl(
        device_handle_,
        code,
        in_buf,
        in_size,
        out_buf,
        out_size,
        &bytes_returned,
        nullptr
    );

    return result != FALSE;
}
