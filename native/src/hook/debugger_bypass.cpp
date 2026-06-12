#include "debugger_bypass.h"
#include "hook_manager.h"
#include <spdlog/spdlog.h>

// 定义缺失的常量
#ifndef ProcessDebugObjectHandle
#define ProcessDebugObjectHandle ((PROCESSINFOCLASS)0x1E)
#endif

#ifndef ProcessDebugFlags
#define ProcessDebugFlags ((PROCESSINFOCLASS)0x1F)
#endif

#ifndef STATUS_PORT_NOT_SET
#define STATUS_PORT_NOT_SET ((NTSTATUS)0xC0000353L)
#endif

namespace pqy {

// 原始函数指针
static IsDebuggerPresent_t Original_IsDebuggerPresent = nullptr;
static CheckRemoteDebuggerPresent_t Original_CheckRemoteDebuggerPresent = nullptr;
static NtQueryInformationProcess_t Original_NtQueryInformationProcess = nullptr;

// Hook IsDebuggerPresent
BOOL WINAPI Hook_IsDebuggerPresent() {
    spdlog::debug("IsDebuggerPresent bypassed");
    return FALSE;
}

// Hook CheckRemoteDebuggerPresent
BOOL WINAPI Hook_CheckRemoteDebuggerPresent(HANDLE hProcess, PBOOL pbDebuggerPresent) {
    // 调用原始函数
    BOOL result = Original_CheckRemoteDebuggerPresent(hProcess, pbDebuggerPresent);

    // 修改结果
    if (pbDebuggerPresent) {
        *pbDebuggerPresent = FALSE;
    }

    spdlog::debug("CheckRemoteDebuggerPresent bypassed");
    return result;
}

// Hook NtQueryInformationProcess
NTSTATUS NTAPI Hook_NtQueryInformationProcess(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
) {
    // 调用原始函数
    NTSTATUS status = Original_NtQueryInformationProcess(
        ProcessHandle,
        ProcessInformationClass,
        ProcessInformation,
        ProcessInformationLength,
        ReturnLength
    );

    // 处理调试相关的查询
    if (NT_SUCCESS(status) && ProcessInformation) {
        switch (ProcessInformationClass) {
            case ProcessDebugPort:  // 0x07
                // 返回 0 表示无调试器
                *(DWORD_PTR*)ProcessInformation = 0;
                spdlog::debug("ProcessDebugPort bypassed");
                break;

            case ProcessDebugObjectHandle:  // 0x1E
                // 返回错误表示无调试对象
                status = STATUS_PORT_NOT_SET;
                spdlog::debug("ProcessDebugObjectHandle bypassed");
                break;

            case ProcessDebugFlags:  // 0x1F
                // 返回 0 表示调试已启用（反向逻辑）
                *(DWORD*)ProcessInformation = 0;
                spdlog::debug("ProcessDebugFlags bypassed");
                break;

            default:
                break;
        }
    }

    return status;
}

// 安装 Hook
bool InstallDebuggerBypass() {
    HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");

    if (!kernel32 || !ntdll) {
        spdlog::error("Required DLLs not found");
        return false;
    }

    bool success = true;

    // Hook IsDebuggerPresent
    void* pIsDebuggerPresent = (void*)GetProcAddress(kernel32, "IsDebuggerPresent");
    if (pIsDebuggerPresent) {
        success &= g_hook_mgr.InstallHook(
            HookType::DEBUGGER_BYPASS,
            pIsDebuggerPresent,
            (void*)Hook_IsDebuggerPresent,
            (void**)&Original_IsDebuggerPresent
        );
    }

    // Hook CheckRemoteDebuggerPresent
    void* pCheckRemoteDebuggerPresent = (void*)GetProcAddress(kernel32, "CheckRemoteDebuggerPresent");
    if (pCheckRemoteDebuggerPresent) {
        success &= g_hook_mgr.InstallHook(
            HookType::DEBUGGER_BYPASS,
            pCheckRemoteDebuggerPresent,
            (void*)Hook_CheckRemoteDebuggerPresent,
            (void**)&Original_CheckRemoteDebuggerPresent
        );
    }

    // Hook NtQueryInformationProcess
    void* pNtQueryInformationProcess = (void*)GetProcAddress(ntdll, "NtQueryInformationProcess");
    if (pNtQueryInformationProcess) {
        success &= g_hook_mgr.InstallHook(
            HookType::DEBUGGER_BYPASS,
            pNtQueryInformationProcess,
            (void*)Hook_NtQueryInformationProcess,
            (void**)&Original_NtQueryInformationProcess
        );
    }

    if (success) {
        spdlog::info("Debugger bypass installed successfully");
    } else {
        spdlog::error("Failed to install some debugger bypass hooks");
    }

    return success;
}

// 卸载 Hook
bool UninstallDebuggerBypass() {
    return g_hook_mgr.UninstallHook(HookType::DEBUGGER_BYPASS);
}

} // namespace pqy
