#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <functional>
#include <map>

namespace pqy {

// Hook 类型
enum class HookType {
    CRC_BYPASS,           // CRC 完整性绕过
    PROCESS_BYPASS,       // 进程枚举绕过
    MODULE_BYPASS,        // 模块枚举绕过
    DEBUGGER_BYPASS,      // 调试器检测绕过
    FREQ_MONITOR,         // 频率限制监控
    REMOTE_CMD_BYPASS,    // 远程命令拦截
};

// Hook 状态
enum class HookStatus {
    DISABLED = 0,
    ENABLED,
    ERROR_STATUS,
};

// Hook 信息
struct HookInfo {
    HookType type;
    std::string name;
    void* target;         // 目标函数地址
    void* detour;         // Hook 函数地址
    void* original;       // 原始函数地址
    HookStatus status;
    std::string module;   // 目标模块
    std::string function; // 目标函数名
};

// Hook 管理器
class HookManager {
public:
    static HookManager& GetInstance();

    // 初始化
    bool Init();
    void Shutdown();

    // 安装/卸载 Hook
    bool InstallHook(HookType type, void* target, void* detour, void** original);
    bool UninstallHook(HookType type);

    // 启用/禁用
    bool EnableHook(HookType type);
    bool DisableHook(HookType type);

    // 批量操作
    bool EnableAll();
    bool DisableAll();

    // 状态查询
    HookStatus GetStatus(HookType type) const;
    std::vector<HookInfo> GetAllHooks() const;

    // 风险监控
    void SetRiskCallback(std::function<void(int level)> callback);
    void NotifyRiskLevel(int level);

private:
    HookManager() = default;
    ~HookManager() = default;

    HookManager(const HookManager&) = delete;
    HookManager& operator=(const HookManager&) = delete;

    std::map<int, HookInfo> hooks_;
    std::function<void(int)> risk_callback_;
    bool initialized_ = false;
};

// 全局实例
#define g_hook_mgr HookManager::GetInstance()

} // namespace pqy
