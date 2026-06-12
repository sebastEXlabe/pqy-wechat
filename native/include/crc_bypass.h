#pragma once

#include <windows.h>

namespace pqy {

// IntegrityCheckTable__Verify 函数签名
// 地址: 0x1805C4C70 (Weixin.dll 4.1.10.31)
// 大小: 2613 字节
// 基本块: 93
// 圈复杂度: 31

// 原始函数类型
using IntegrityCheckTable_Verify_t = bool(__fastcall*)(void* this_ptr);

// Hook 函数
bool __fastcall Hook_IntegrityCheckTable_Verify(void* this_ptr);

// 安装 Hook
bool InstallCRCBypass();

// 卸载 Hook
bool UninstallCRCBypass();

// 获取原始函数
IntegrityCheckTable_Verify_t GetOriginalIntegrityCheck();

} // namespace pqy
