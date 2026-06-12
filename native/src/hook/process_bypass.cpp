#include "process_bypass.h"
#include "hook_manager.h"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace pqy {

// 原始函数指针
static CreateToolhelp32Snapshot_t Original_CreateToolhelp32Snapshot = nullptr;
static Process32FirstW_t Original_Process32FirstW = nullptr;
static Process32NextW_t Original_Process32NextW = nullptr;
static Module32FirstW_t Original_Module32FirstW = nullptr;
static Module32NextW_t Original_Module32NextW = nullptr;

// 隐藏列表
static std::set<std::string> g_hidden_processes;
static std::set<std::string> g_hidden_modules;

// 当前快照句柄
static HANDLE g_current_snapshot = nullptr;

// 初始化默认隐藏列表
static void InitDefaultHiddenLists() {
    // 隐藏我们的进程
    g_hidden_processes.insert("pqy-engine.exe");
    g_hidden_processes.insert("pqy-hook.dll");
    g_hidden_processes.insert("pqy-helper.exe");

    // 隐藏我们的模块
    g_hidden_modules.insert("pqy_hook.dll");
    g_hidden_modules.insert("pqy_proxy.dll");
    g_hidden_modules.insert("minhook.dll");
}

// 宽字符转多字节
static std::string WideToMultiByte(const std::wstring& wide) {
    if (wide.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wide[0], (int)wide.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wide[0], (int)wide.size(), &str[0], size_needed, NULL, NULL);
    return str;
}

// 检查是否是需要隐藏的进程
static bool IsHiddenProcess(const std::wstring& name) {
    // 转换为小写进行比较
    std::string name_str = WideToMultiByte(name);
    std::transform(name_str.begin(), name_str.end(), name_str.begin(), ::tolower);

    for (const auto& hidden : g_hidden_processes) {
        std::string hidden_lower = hidden;
        std::transform(hidden_lower.begin(), hidden_lower.end(), hidden_lower.begin(), ::tolower);

        if (name_str.find(hidden_lower) != std::string::npos) {
            return true;
        }
    }
    return false;
}

// 检查是否是需要隐藏的模块
static bool IsHiddenModule(const std::wstring& name) {
    std::string name_str = WideToMultiByte(name);
    std::transform(name_str.begin(), name_str.end(), name_str.begin(), ::tolower);

    for (const auto& hidden : g_hidden_modules) {
        std::string hidden_lower = hidden;
        std::transform(hidden_lower.begin(), hidden_lower.end(), hidden_lower.begin(), ::tolower);

        if (name_str.find(hidden_lower) != std::string::npos) {
            return true;
        }
    }
    return false;
}

// Hook CreateToolhelp32Snapshot
HANDLE WINAPI Hook_CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID) {
    HANDLE hSnapshot = Original_CreateToolhelp32Snapshot(dwFlags, th32ProcessID);

    // 记录进程快照句柄
    if (dwFlags & TH32CS_SNAPPROCESS) {
        g_current_snapshot = hSnapshot;
        spdlog::debug("Process snapshot created: {}", (void*)hSnapshot);
    }

    return hSnapshot;
}

// Hook Process32FirstW
BOOL WINAPI Hook_Process32FirstW(HANDLE hSnapshot, LPPROCESSENTRY32W lppe) {
    BOOL result = Original_Process32FirstW(hSnapshot, lppe);

    // 如果是隐藏进程，继续下一个
    while (result && IsHiddenProcess(lppe->szExeFile)) {
        spdlog::debug("Hidden process skipped: {}",
            WideToMultiByte(lppe->szExeFile));
        result = Original_Process32NextW(hSnapshot, lppe);
    }

    return result;
}

// Hook Process32NextW
BOOL WINAPI Hook_Process32NextW(HANDLE hSnapshot, LPPROCESSENTRY32W lppe) {
    BOOL result = Original_Process32NextW(hSnapshot, lppe);

    // 跳过隐藏进程
    while (result && IsHiddenProcess(lppe->szExeFile)) {
        spdlog::debug("Hidden process skipped: {}",
            WideToMultiByte(lppe->szExeFile));
        result = Original_Process32NextW(hSnapshot, lppe);
    }

    return result;
}

// Hook Module32FirstW
BOOL WINAPI Hook_Module32FirstW(HANDLE hSnapshot, LPMODULEENTRY32W lpme) {
    BOOL result = Original_Module32FirstW(hSnapshot, lpme);

    // 如果是隐藏模块，继续下一个
    while (result && IsHiddenModule(lpme->szModule)) {
        spdlog::debug("Hidden module skipped: {}",
            WideToMultiByte(lpme->szModule));
        result = Original_Module32NextW(hSnapshot, lpme);
    }

    return result;
}

// Hook Module32NextW
BOOL WINAPI Hook_Module32NextW(HANDLE hSnapshot, LPMODULEENTRY32W lpme) {
    BOOL result = Original_Module32NextW(hSnapshot, lpme);

    // 跳过隐藏模块
    while (result && IsHiddenModule(lpme->szModule)) {
        spdlog::debug("Hidden module skipped: {}",
            WideToMultiByte(lpme->szModule));
        result = Original_Module32NextW(hSnapshot, lpme);
    }

    return result;
}

// 安装 Hook
bool InstallProcessBypass() {
    InitDefaultHiddenLists();

    HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
    if (!kernel32) {
        spdlog::error("kernel32.dll not found");
        return false;
    }

    bool success = true;

    // Hook CreateToolhelp32Snapshot
    void* pCreateToolhelp32Snapshot = (void*)GetProcAddress(kernel32, "CreateToolhelp32Snapshot");
    if (pCreateToolhelp32Snapshot) {
        success &= g_hook_mgr.InstallHook(
            HookType::PROCESS_BYPASS,
            pCreateToolhelp32Snapshot,
            (void*)Hook_CreateToolhelp32Snapshot,
            (void**)&Original_CreateToolhelp32Snapshot
        );
    }

    // Hook Process32FirstW
    void* pProcess32FirstW = (void*)GetProcAddress(kernel32, "Process32FirstW");
    if (pProcess32FirstW) {
        success &= g_hook_mgr.InstallHook(
            HookType::PROCESS_BYPASS,
            pProcess32FirstW,
            (void*)Hook_Process32FirstW,
            (void**)&Original_Process32FirstW
        );
    }

    // Hook Process32NextW
    void* pProcess32NextW = (void*)GetProcAddress(kernel32, "Process32NextW");
    if (pProcess32NextW) {
        success &= g_hook_mgr.InstallHook(
            HookType::PROCESS_BYPASS,
            pProcess32NextW,
            (void*)Hook_Process32NextW,
            (void**)&Original_Process32NextW
        );
    }

    // Hook Module32FirstW
    void* pModule32FirstW = (void*)GetProcAddress(kernel32, "Module32FirstW");
    if (pModule32FirstW) {
        success &= g_hook_mgr.InstallHook(
            HookType::MODULE_BYPASS,
            pModule32FirstW,
            (void*)Hook_Module32FirstW,
            (void**)&Original_Module32FirstW
        );
    }

    // Hook Module32NextW
    void* pModule32NextW = (void*)GetProcAddress(kernel32, "Module32NextW");
    if (pModule32NextW) {
        success &= g_hook_mgr.InstallHook(
            HookType::MODULE_BYPASS,
            pModule32NextW,
            (void*)Hook_Module32NextW,
            (void**)&Original_Module32NextW
        );
    }

    if (success) {
        spdlog::info("Process bypass installed successfully");
    } else {
        spdlog::error("Failed to install some process bypass hooks");
    }

    return success;
}

// 卸载 Hook
bool UninstallProcessBypass() {
    g_hook_mgr.UninstallHook(HookType::PROCESS_BYPASS);
    g_hook_mgr.UninstallHook(HookType::MODULE_BYPASS);
    return true;
}

// 添加隐藏进程
void AddHiddenProcess(const std::string& name) {
    g_hidden_processes.insert(name);
}

// 添加隐藏模块
void AddHiddenModule(const std::string& name) {
    g_hidden_modules.insert(name);
}

// 清空隐藏列表
void ClearHiddenLists() {
    g_hidden_processes.clear();
    g_hidden_modules.clear();
}

} // namespace pqy
