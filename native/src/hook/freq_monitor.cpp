#include "hook_manager.h"
#include <spdlog/spdlog.h>
#include <map>
#include <string>
#include <mutex>

namespace pqy {

// FreqLimit::FreshCacheCount 函数类型
// 地址: 0x184946160 (Weixin.dll 4.1.10.31)
// 大小: 3079 字节
// 基本块: 108
// 圈复杂度: 56
using FreqLimit_FreshCacheCount_t = void(__fastcall*)(void* this_ptr, int log_id);

// 原始函数指针
static FreqLimit_FreshCacheCount_t Original_FreqLimit_FreshCacheCount = nullptr;

// 频率限制信息
struct FreqLimitInfo {
    int current_count;
    int max_count;
    std::string log_id;
};

// 频率限制缓存
static std::map<int, FreqLimitInfo> g_freq_limits;
static std::mutex g_freq_mutex;

// 风险阈值
static const double RISK_THRESHOLD = 0.7;  // 70% 时警告
static const double DANGER_THRESHOLD = 0.9; // 90% 时停止

// Hook FreqLimit::FreshCacheCount
void __fastcall Hook_FreqLimit_FreshCacheCount(void* this_ptr, int log_id) {
    // 调用原始函数
    Original_FreqLimit_FreshCacheCount(this_ptr, log_id);

    // 读取频率信息（需要根据实际结构解析）
    // 这里简化处理，只记录调用
    {
        std::lock_guard<std::mutex> lock(g_freq_mutex);

        auto& info = g_freq_limits[log_id];
        info.current_count++;
        info.log_id = std::to_string(log_id);

        // 默认限制（实际应该从微信读取）
        if (info.max_count == 0) {
            info.max_count = 100;  // 假设默认 100
        }

        // 检查风险
        double ratio = (double)info.current_count / info.max_count;

        if (ratio >= DANGER_THRESHOLD) {
            spdlog::warn("FREQ DANGER: log_id={} count={}/{} ({:.1f}%)",
                log_id, info.current_count, info.max_count, ratio * 100);
            g_hook_mgr.NotifyRiskLevel(3);  // HIGH
        } else if (ratio >= RISK_THRESHOLD) {
            spdlog::warn("FREQ WARNING: log_id={} count={}/{} ({:.1f}%)",
                log_id, info.current_count, info.max_count, ratio * 100);
            g_hook_mgr.NotifyRiskLevel(2);  // MEDIUM
        }
    }
}

// 安装频率监控
bool InstallFreqMonitor() {
    HMODULE weixin = GetModuleHandleA("Weixin.dll");
    if (!weixin) {
        spdlog::error("Weixin.dll not found");
        return false;
    }

    // 偏移 = 0x184946160 - 0x180000000 = 0x4946160
    DWORD offset = 0x4946160;
    void* target = (BYTE*)weixin + offset;

    bool result = g_hook_mgr.InstallHook(
        HookType::FREQ_MONITOR,
        target,
        (void*)Hook_FreqLimit_FreshCacheCount,
        (void**)&Original_FreqLimit_FreshCacheCount
    );

    if (result) {
        spdlog::info("Frequency monitor installed successfully");
    }

    return result;
}

// 卸载频率监控
bool UninstallFreqMonitor() {
    return g_hook_mgr.UninstallHook(HookType::FREQ_MONITOR);
}

// 获取频率信息
std::map<int, FreqLimitInfo> GetFreqLimitInfo() {
    std::lock_guard<std::mutex> lock(g_freq_mutex);
    return g_freq_limits;
}

// 检查是否应该继续操作
bool ShouldContinueOperation(int log_id) {
    std::lock_guard<std::mutex> lock(g_freq_mutex);

    auto it = g_freq_limits.find(log_id);
    if (it == g_freq_limits.end()) {
        return true;  // 无限制
    }

    double ratio = (double)it->second.current_count / it->second.max_count;
    return ratio < 0.5;  // 保持在 50% 以下
}

// 重置计数器
void ResetFreqCounter(int log_id) {
    std::lock_guard<std::mutex> lock(g_freq_mutex);
    g_freq_limits.erase(log_id);
}

} // namespace pqy
