# Hook 内核层设计

## 概述

Hook 内核层是整个系统的**安全基石**。通过 Hook 微信的关键检测函数，绕过所有风控检查，确保后续的 Mmmojo 代理和 XPlugin 注入不会被检测。

## 设计原则

1. **不修改 Weixin.dll** — 所有 Hook 在内存中进行
2. **不 Hook CloseHandle/DuplicateHandle** — 避免 Handle 验证
3. **最小化 Hook 范围** — 只 Hook 必要的检测函数
4. **保持原始行为** — Hook 后的行为尽量接近原始

---

## Hook 架构

```
┌─────────────────────────────────────────────────────────────┐
│                    Hook 管理器                                │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │ Hook 安装   │  │ Hook 卸载   │  │ Hook 监控   │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
└─────────────────────────────────────────────────────────────┘
                              │
      ┌───────────────────────┼───────────────────────┐
      │                       │                       │
      ▼                       ▼                       ▼
┌───────────┐         ┌───────────┐         ┌───────────┐
│ CRC Hook  │         │ 进程 Hook │         │ 调试 Hook │
│           │         │           │         │           │
│ Integrity │         │ Toolhelp  │         │ IsDebug   │
│ CheckTable│         │ Snapshot  │         │ gerPresent│
└───────────┘         └───────────┘         └───────────┘
```

---

## Hook 实现清单

### 1. CRC 完整性绕过 [🔴🔴🔴]

**目标函数**: `IntegrityCheckTable__Verify` @ 0x1805C4C70

**绕过策略**: Hook 函数，始终返回成功

```cpp
// 原始函数签名
bool IntegrityCheckTable__Verify(void* this_ptr);

// Hook 实现
bool Hook_IntegrityCheckTable__Verify(void* this_ptr) {
    // 跳过实际校验，直接返回 true
    return true;
}
```

**风险**: 修改 DLL 会被此函数检测。由于我们在内存中 Hook，不修改文件，但仍需确保 Hook 不被检测。

### 2. 进程枚举绕过 [🔴🔴]

**目标函数**:
- `CreateToolhelp32Snapshot` @ KERNEL32
- `Process32FirstW` @ KERNEL32
- `Process32NextW` @ KERNEL32

**绕过策略**: Hook Snapshot，过滤掉我们的进程

```cpp
// Hook CreateToolhelp32Snapshot
HANDLE Hook_CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID) {
    HANDLE hSnapshot = Original_CreateToolhelp32Snapshot(dwFlags, th32ProcessID);
    
    // 如果是进程枚举，记录句柄用于后续过滤
    if (dwFlags & TH32CS_SNAPPROCESS) {
        g_processSnapshot = hSnapshot;
    }
    
    return hSnapshot;
}

// Hook Process32NextW，过滤我们的进程
BOOL Hook_Process32NextW(HANDLE hSnapshot, LPPROCESSENTRY32W lppe) {
    BOOL result = Original_Process32NextW(hSnapshot, lppe);
    
    while (result) {
        // 检查是否是我们的进程
        if (!IsOurProcess(lppe->szExeFile)) {
            return result;  // 不是我们的进程，返回
        }
        // 是我们的进程，跳过
        result = Original_Process32NextW(hSnapshot, lppe);
    }
    
    return result;
}
```

### 3. 模块枚举绕过 [🔴🔴]

**目标函数**:
- `Module32FirstW` @ KERNEL32
- `Module32NextW` @ KERNEL32

**绕过策略**: 过滤掉我们注入的模块

```cpp
// Hook Module32NextW，过滤我们的模块
BOOL Hook_Module32NextW(HANDLE hSnapshot, LPMODULEENTRY32W lpme) {
    BOOL result = Original_Module32NextW(hSnapshot, lpme);
    
    while (result) {
        // 检查是否是我们的模块
        if (!IsOurModule(lpme->szModule)) {
            return result;
        }
        result = Original_Module32NextW(hSnapshot, lpme);
    }
    
    return result;
}
```

### 4. 调试器检测绕过 [🔴🔴]

**目标函数**:
- `IsDebuggerPresent` @ KERNEL32
- `CheckRemoteDebuggerPresent` @ KERNEL32
- `NtQueryInformationProcess` @ NTDLL

**绕过策略**: 始终返回"无调试器"

```cpp
// Hook IsDebuggerPresent
BOOL Hook_IsDebuggerPresent() {
    return FALSE;
}

// Hook CheckRemoteDebuggerPresent
BOOL Hook_CheckRemoteDebuggerPresent(HANDLE hProcess, PBOOL pbDebuggerPresent) {
    *pbDebuggerPresent = FALSE;
    return TRUE;
}

// Hook NtQueryInformationProcess
NTSTATUS Hook_NtQueryInformationProcess(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
) {
    NTSTATUS status = Original_NtQueryInformationProcess(
        ProcessHandle, ProcessInformationClass,
        ProcessInformation, ProcessInformationLength, ReturnLength
    );
    
    // 处理调试相关的查询
    if (NT_SUCCESS(status)) {
        switch (ProcessInformationClass) {
            case ProcessDebugPort:
                *(DWORD_PTR*)ProcessInformation = 0;
                break;
            case ProcessDebugObjectHandle:
                return STATUS_PORT_NOT_SET;
            case ProcessDebugFlags:
                *(DWORD*)ProcessInformation = 0;
                break;
        }
    }
    
    return status;
}
```

### 5. Handle Hook 验证规避 [🔴🔴🔴]

**策略**: 绝对不 Hook `CloseHandle` 和 `DuplicateHandle`

微信自己 Hook 了这两个函数并验证 Hook 是否被覆盖。如果我们 Hook，会被检测。

**替代方案**:
- 使用 UIAutomation（不涉及 Handle 操作）
- 使用 Mmmojo 代理（通过 IPC 通信）
- 如果必须操作 Handle，使用原始 API

### 6. 日志系统监控 [🟡]

**目标函数**: XLogger 相关函数

**策略**: Hook 日志输出，重定向到我们的日志系统

```cpp
// Hook XLogger 输出
void Hook_XLogger_Write(int level, const char* module, const char* message) {
    // 记录到我们的日志
    OurLogger::Log(level, module, message);
    
    // 也调用原始函数（可选）
    // Original_XLogger_Write(level, module, message);
}
```

### 7. 频率限制监控 [🟠]

**目标函数**: `FreqLimit___FreshCacheCount` @ 0x184946160

**策略**: 监控频率计数器，提前预警

```cpp
// Hook FreqLimit
void Hook_FreqLimit_FreshCacheCount(void* this_ptr, ...) {
    // 调用原始函数
    Original_FreqLimit_FreshCacheCount(this_ptr, ...);
    
    // 读取当前计数
    int count = GetFreqLimitCount(this_ptr);
    int maxCount = GetFreqLimitMax(this_ptr);
    
    // 如果接近限制，发出警告
    if (count > maxCount * 0.7) {
        NotifyRiskLevel(HIGH);
    }
}
```

### 8. 远程命令拦截 [🔴🔴🔴]

**目标函数**: `CmdProcessor::OnIPxx` @ 0x1828E36C0

**策略**: Hook 远程命令处理器，拦截敏感命令

```cpp
// Hook CmdProcessor
void Hook_CmdProcessor_OnIPxx(void* this_ptr, void* xml_data) {
    // 解析 XML 命令
    Command cmd = ParseCommand(xml_data);
    
    // 检查是否是敏感命令
    if (cmd.type == "UploadLog" || cmd.type == "UploadCrash") {
        // 拦截或修改命令
        LogWarning("Remote command intercepted: %s", cmd.type.c_str());
        return;  // 不执行
    }
    
    // 其他命令正常执行
    Original_CmdProcessor_OnIPxx(this_ptr, xml_data);
}
```

---

## Hook 框架实现

### 使用 MinHook

```cpp
#include <MinHook.h>

// 初始化 Hook 框架
bool InitHookFramework() {
    MH_STATUS status = MH_Initialize();
    return status == MH_OK;
}

// 安装 Hook
bool InstallHook(void* target, void* detour, void** original) {
    MH_STATUS status = MH_CreateHook(target, detour, original);
    if (status != MH_OK) return false;
    
    status = MH_EnableHook(target);
    return status == MH_OK;
}

// 卸载 Hook
bool UninstallHook(void* target) {
    MH_STATUS status = MH_DisableHook(target);
    return status == MH_OK;
}

// 清理
void CleanupHooks() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
}
```

### Hook 注册表

```cpp
struct HookEntry {
    const char* name;         // Hook 名称
    const char* module;       // 目标模块
    const char* function;     // 目标函数
    void* detour;            // Hook 函数
    void* original;          // 原始函数
    bool enabled;            // 是否启用
};

// Hook 注册表
HookEntry g_hooks[] = {
    {
        "IntegrityCheck",
        "Weixin.dll",
        "IntegrityCheckTable__Verify",
        Hook_IntegrityCheckTable__Verify,
        nullptr,
        true
    },
    {
        "ProcessEnum",
        "kernel32.dll",
        "CreateToolhelp32Snapshot",
        Hook_CreateToolhelp32Snapshot,
        nullptr,
        true
    },
    // ... 更多 Hook
};
```

---

## 风控等级监控

```cpp
enum class RiskLevel {
    LOW,        // 正常操作
    MEDIUM,     // 需要降低频率
    HIGH,       // 停止操作
    CRITICAL    // 立即退出
};

class RiskMonitor {
public:
    void UpdateRiskLevel(RiskLevel level) {
        currentLevel_ = level;
        NotifyObservers(level);
    }
    
    RiskLevel GetCurrentLevel() const {
        return currentLevel_;
    }
    
    bool ShouldContinue() const {
        return currentLevel_ < RiskLevel::HIGH;
    }
    
private:
    RiskLevel currentLevel_ = RiskLevel::LOW;
    std::vector<Observer*> observers_;
};
```

---

## 安全检查清单

在启动 Hook 之前，确保：

- [ ] 不 Hook CloseHandle
- [ ] 不 Hook DuplicateHandle
- [ ] 不修改 Weixin.dll 文件
- [ ] 单实例运行
- [ ] 从 explorer.exe 启动
- [ ] 正常桌面环境（不禁用 DWM）
- [ ] 真实网络栈（不禁用 IPv6）

---

## 下一步

- [Mmmojo 代理](mmmojo-proxy.md) — IPC 消息代理实现
- [XPlugin 注入](xplugin-inject.md) — 原生 API 访问
- [Hook 实现](../risk-control/hook-implementation.md) — 代码级实现细节
