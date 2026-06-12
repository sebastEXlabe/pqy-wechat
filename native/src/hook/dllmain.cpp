#include <windows.h>
#include <spdlog/spdlog.h>
#include "hook_manager.h"
#include "crc_bypass.h"
#include "process_bypass.h"
#include "debugger_bypass.h"

// 前向声明
namespace pqy {
    void InitLogger(const std::string& log_file, bool console_output);
    void ShutdownLogger();
    bool InstallFreqMonitor();
    bool InstallRemoteCmdBypass();
}

// DLL 入口点
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            // 禁用 DLL_THREAD_ATTACH/DETACH 通知
            DisableThreadLibraryCalls(hModule);

            // 初始化日志
            pqy::InitLogger("logs/pqy_hook.log", true);
            spdlog::info("PQY Hook DLL loaded");

            // 初始化 Hook 管理器
            if (!pqy::g_hook_mgr.Init()) {
                spdlog::error("Failed to initialize HookManager");
                return FALSE;
            }

            spdlog::info("Hook manager initialized (no hooks installed yet)");
            spdlog::info("Use PQY_InstallHooks() to install hooks manually");

            // 暂时禁用所有自动 Hook，用于调试
            // // 1. 调试器检测绕过（低风险）
            // if (!pqy::InstallDebuggerBypass()) {
            //     spdlog::warn("Failed to install debugger bypass");
            // }
            //
            // // 2. 进程/模块枚举绕过（低风险）
            // if (!pqy::InstallProcessBypass()) {
            //     spdlog::warn("Failed to install process bypass");
            // }
            //
            // // 3. CRC 完整性绕过（中风险）
            // if (!pqy::InstallCRCBypass()) {
            //     spdlog::warn("Failed to install CRC bypass");
            // }
            //
            // // 4. 频率监控（中风险，纯监控）
            // if (!pqy::InstallFreqMonitor()) {
            //     spdlog::warn("Failed to install frequency monitor");
            // }

            break;
        }

        case DLL_PROCESS_DETACH: {
            spdlog::info("PQY Hook DLL unloading");

            // 卸载所有 Hook
            pqy::g_hook_mgr.Shutdown();

            // 关闭日志
            pqy::ShutdownLogger();
            break;
        }

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;
    }

    return TRUE;
}

// 导出函数 - 手动安装 Hook
extern "C" __declspec(dllexport)
bool __cdecl PQY_InstallHooks() {
    return pqy::g_hook_mgr.EnableAll();
}

// 导出函数 - 手动卸载 Hook
extern "C" __declspec(dllexport)
bool __cdecl PQY_UninstallHooks() {
    return pqy::g_hook_mgr.DisableAll();
}

// 导出函数 - 获取状态
extern "C" __declspec(dllexport)
int __cdecl PQY_GetHookCount() {
    return (int)pqy::g_hook_mgr.GetAllHooks().size();
}

// 导出函数 - 添加隐藏进程
extern "C" __declspec(dllexport)
void __cdecl PQY_AddHiddenProcess(const char* name) {
    pqy::AddHiddenProcess(name);
}

// 导出函数 - 添加隐藏模块
extern "C" __declspec(dllexport)
void __cdecl PQY_AddHiddenModule(const char* name) {
    pqy::AddHiddenModule(name);
}
