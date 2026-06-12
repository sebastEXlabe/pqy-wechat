#pragma once

#include <windows.h>
#include <winternl.h>

namespace pqy {

// 调试器检测函数类型
using IsDebuggerPresent_t = BOOL(WINAPI*)();
using CheckRemoteDebuggerPresent_t = BOOL(WINAPI*)(HANDLE, PBOOL);
using NtQueryInformationProcess_t = NTSTATUS(NTAPI*)(
    HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG
);

// Hook 函数
BOOL WINAPI Hook_IsDebuggerPresent();
BOOL WINAPI Hook_CheckRemoteDebuggerPresent(HANDLE hProcess, PBOOL pbDebuggerPresent);
NTSTATUS NTAPI Hook_NtQueryInformationProcess(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
);

// 安装/卸载 Hook
bool InstallDebuggerBypass();
bool UninstallDebuggerBypass();

} // namespace pqy
