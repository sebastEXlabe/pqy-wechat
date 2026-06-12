#include "crc_bypass.h"
#include "hook_manager.h"
#include <spdlog/spdlog.h>

namespace pqy {

// 原始函数指针
static IntegrityCheckTable_Verify_t Original_IntegrityCheckTable_Verify = nullptr;

// 函数签名 (Weixin.dll 4.1.10.31)
// 地址: 0x1805C4C70
// 签名: 48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 20
static const char* INTEGRITY_CHECK_SIGNATURE =
    "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 20";

// Hook 函数 - 跳过完整性校验
bool __fastcall Hook_IntegrityCheckTable_Verify(void* this_ptr) {
    spdlog::debug("IntegrityCheckTable::Verify bypassed");

    // 直接返回 true，跳过实际校验
    return true;
}

// 安装 Hook
bool InstallCRCBypass() {
    // 获取 Weixin.dll 模块
    HMODULE weixin = GetModuleHandleA("Weixin.dll");
    if (!weixin) {
        spdlog::error("Weixin.dll not found");
        return false;
    }

    // 方法 1: 使用硬编码偏移 (4.1.10.31)
    // 偏移 = 0x1805C4C70 - 0x180000000 = 0x5C4C70
    DWORD offset = 0x5C4C70;
    void* target = (BYTE*)weixin + offset;

    // 验证签名
    BYTE* code = (BYTE*)target;
    if (code[0] != 0x48 || code[1] != 0x89 || code[2] != 0x5C) {
        spdlog::warn("Signature mismatch at offset 0x{:X}, trying signature scan", offset);

        // 方法 2: 签名扫描 (备用)
        // TODO: 实现签名扫描
        spdlog::error("Signature scan not implemented yet");
        return false;
    }

    // 安装 Hook
    bool result = g_hook_mgr.InstallHook(
        HookType::CRC_BYPASS,
        target,
        (void*)Hook_IntegrityCheckTable_Verify,
        (void**)&Original_IntegrityCheckTable_Verify
    );

    if (result) {
        spdlog::info("CRC bypass installed successfully");
    } else {
        spdlog::error("Failed to install CRC bypass");
    }

    return result;
}

// 卸载 Hook
bool UninstallCRCBypass() {
    return g_hook_mgr.UninstallHook(HookType::CRC_BYPASS);
}

// 获取原始函数
IntegrityCheckTable_Verify_t GetOriginalIntegrityCheck() {
    return Original_IntegrityCheckTable_Verify;
}

} // namespace pqy
