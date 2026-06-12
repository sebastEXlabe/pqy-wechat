#include "hook_manager.h"
#include <spdlog/spdlog.h>
#include <set>
#include <string>

namespace pqy {

// CmdProcessor::OnIPxx 函数类型
// 地址: 0x1828E36C0 (Weixin.dll 4.1.10.31)
// 大小: 7197 字节
// 基本块: 273
// 圈复杂度: 131
using CmdProcessor_OnIPxx_t = void(__fastcall*)(void* this_ptr, void* xml_data);

// 原始函数指针
static CmdProcessor_OnIPxx_t Original_CmdProcessor_OnIPxx = nullptr;

// 敏感命令列表
static std::set<std::string> g_sensitive_commands = {
    "__SetLog",           // 远程调日志
    "__SetDebugIP",       // 远程设调试 IP
    "__UploadLog",        // 上传日志
    "__UploadCrash",      // 上传崩溃
    "__SetAgentCheck",    // 开启 agent 检测
    "__SetAgentLevel",    // 设置 agent 等级
    "__SetLogLevel",      // 设置日志级别
};

// 从 XML 中提取命令名（简化实现）
static std::string ExtractCommandFromXml(void* xml_data) {
    // TODO: 实际实现需要解析 XML
    // 这里返回空字符串，允许所有命令通过
    return "";
}

// Hook CmdProcessor::OnIPxx
void __fastcall Hook_CmdProcessor_OnIPxx(void* this_ptr, void* xml_data) {
    // 提取命令
    std::string command = ExtractCommandFromXml(xml_data);

    // 检查是否是敏感命令
    if (!command.empty() && g_sensitive_commands.count(command)) {
        spdlog::warn("Blocked remote command: {}", command);
        return;  // 不执行
    }

    // 其他命令正常执行
    Original_CmdProcessor_OnIPxx(this_ptr, xml_data);
}

// 安装远程命令拦截
bool InstallRemoteCmdBypass() {
    HMODULE weixin = GetModuleHandleA("Weixin.dll");
    if (!weixin) {
        spdlog::error("Weixin.dll not found");
        return false;
    }

    // 偏移 = 0x1828E36C0 - 0x180000000 = 0x28E36C0
    DWORD offset = 0x28E36C0;
    void* target = (BYTE*)weixin + offset;

    bool result = g_hook_mgr.InstallHook(
        HookType::REMOTE_CMD_BYPASS,
        target,
        (void*)Hook_CmdProcessor_OnIPxx,
        (void**)&Original_CmdProcessor_OnIPxx
    );

    if (result) {
        spdlog::info("Remote command bypass installed successfully");
    }

    return result;
}

// 卸载远程命令拦截
bool UninstallRemoteCmdBypass() {
    return g_hook_mgr.UninstallHook(HookType::REMOTE_CMD_BYPASS);
}

// 添加敏感命令
void AddSensitiveCommand(const std::string& cmd) {
    g_sensitive_commands.insert(cmd);
}

// 移除敏感命令
void RemoveSensitiveCommand(const std::string& cmd) {
    g_sensitive_commands.erase(cmd);
}

} // namespace pqy
