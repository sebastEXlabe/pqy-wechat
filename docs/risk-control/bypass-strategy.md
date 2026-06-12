# 风控绕过策略

## 概述

本文档详细说明如何通过 Hook 内核层绕过微信的所有风控检测。这是整个系统的**安全基石**。

## 绕过优先级

| 优先级 | 检测点 | 绕过方式 | 风险 |
|--------|--------|----------|------|
| 🔴 P0 | CRC 完整性校验 | Hook 跳过 | 必须 |
| 🔴 P0 | Handle Hook 验证 | 不 Hook | 必须 |
| 🔴 P0 | 进程/模块枚举 | Hook 过滤 | 必须 |
| 🔴 P0 | 调试器检测 | Hook 返回假值 | 必须 |
| 🟠 P1 | 频率限制 | 监控+控制 | 高 |
| 🟠 P1 | 用户行为上报 | 类人模拟 | 高 |
| 🟠 P1 | 远程命令拦截 | Hook 拦截 | 高 |
| 🟡 P2 | 网络指纹 | 复用原生 | 中 |
| 🟡 P2 | 日志系统 | Hook 重定向 | 中 |

---

## P0: 必须绕过的检测

### 1. CRC 完整性校验

**检测机制**：
- 函数：`IntegrityCheckTable__Verify` @ 0x1805C4C70
- 圈复杂度：31
- 基本块数：93

**绕过方案**：

```cpp
// 方案 A: Hook 函数直接返回 true
bool Hook_IntegrityCheckTable__Verify(void* this_ptr) {
    return true;
}

// 方案 B: Hook 特定检查点
// 如果函数太复杂，可以只 Hook 关键检查点
bool Hook_CheckFileCRCValid(const char* path) {
    return true;  // 跳过 CRC 校验
}
```

**安装 Hook**：
```cpp
void InstallIntegrityHook() {
    void* target = (void*)0x1805C4C70;
    void* detour = (void*)Hook_IntegrityCheckTable__Verify;
    void** original = (void**)&Original_IntegrityCheckTable__Verify;
    
    MH_CreateHook(target, detour, original);
    MH_EnableHook(target);
}
```

### 2. Handle Hook 验证

**检测机制**：
- 微信 Hook 了 `CloseHandle` 和 `DuplicateHandle`
- 验证自己的 Hook 是否被覆盖

**绕过方案**：**绝对不 Hook 这两个函数**

```cpp
// ❌ 错误做法
MH_CreateHook(CloseHandle, ...);  // 会被检测

// ✅ 正确做法
// 使用 UIAutomation 或 Mmmojo，不直接操作 Handle
```

### 3. 进程/模块枚举

**检测机制**：
- `CreateToolhelp32Snapshot` — 创建进程快照
- `Process32FirstW/NextW` — 遍历进程
- `Module32FirstW/NextW` — 遍历模块

**绕过方案**：

```cpp
// 记录我们的进程/模块信息
std::set<std::string> g_our_processes = {
    "pqy-engine.exe",
    "pqy-helper.exe"
};

std::set<std::string> g_our_modules = {
    "pqy_hook.dll",
    "pqy_proxy.dll"
};

// Hook Process32NextW，过滤我们的进程
BOOL Hook_Process32NextW(HANDLE hSnapshot, LPPROCESSENTRY32W lppe) {
    BOOL result = Original_Process32NextW(hSnapshot, lppe);
    
    while (result) {
        // 检查是否是我们的进程
        std::wstring name(lppe->szExeFile);
        bool is_ours = false;
        
        for (const auto& proc : g_our_processes) {
            if (name.find(std::wstring(proc.begin(), proc.end())) != std::wstring::npos) {
                is_ours = true;
                break;
            }
        }
        
        if (!is_ours) {
            return result;  // 不是我们的，返回
        }
        
        // 是我们的，跳过
        result = Original_Process32NextW(hSnapshot, lppe);
    }
    
    return result;
}

// Hook Module32NextW，过滤我们的模块
BOOL Hook_Module32NextW(HANDLE hSnapshot, LPMODULEENTRY32W lpme) {
    BOOL result = Original_Module32NextW(hSnapshot, lpme);
    
    while (result) {
        std::wstring name(lpme->szModule);
        bool is_ours = false;
        
        for (const auto& mod : g_our_modules) {
            if (name.find(std::wstring(mod.begin(), mod.end())) != std::wstring::npos) {
                is_ours = true;
                break;
            }
        }
        
        if (!is_ours) {
            return result;
        }
        
        result = Original_Module32NextW(hSnapshot, lpme);
    }
    
    return result;
}
```

### 4. 调试器检测

**检测机制**：
- `IsDebuggerPresent` — 检测本地调试器
- `CheckRemoteDebuggerPresent` — 检测远程调试器
- `NtQueryInformationProcess` — 查询调试端口

**绕过方案**：

```cpp
// Hook IsDebuggerPresent
BOOL Hook_IsDebuggerPresent() {
    return FALSE;
}

// Hook CheckRemoteDebuggerPresent
BOOL Hook_CheckRemoteDebuggerPresent(HANDLE hProcess, PBOOL pbDebuggerPresent) {
    // 调用原始函数
    BOOL result = Original_CheckRemoteDebuggerPresent(hProcess, pbDebuggerPresent);
    
    // 修改结果
    if (pbDebuggerPresent) {
        *pbDebuggerPresent = FALSE;
    }
    
    return result;
}

// Hook NtQueryInformationProcess
NTSTATUS Hook_NtQueryInformationProcess(
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
    if (NT_SUCCESS(status)) {
        switch (ProcessInformationClass) {
            case ProcessDebugPort:  // 0x07
                // 返回 0 表示无调试器
                *(DWORD_PTR*)ProcessInformation = 0;
                break;
                
            case ProcessDebugObjectHandle:  // 0x1E
                // 返回错误表示无调试对象
                return STATUS_PORT_NOT_SET;
                
            case ProcessDebugFlags:  // 0x1F
                // 返回 0 表示调试已启用（反向逻辑）
                *(DWORD*)ProcessInformation = 0;
                break;
        }
    }
    
    return status;
}
```

---

## P1: 高优先级绕过

### 5. 频率限制监控

**检测机制**：
- 函数：`FreqLimit___FreshCacheCount` @ 0x184946160
- 圈复杂度：56
- 超过阈值后 LogId 被封禁

**绕过方案**：监控而非绕过

```cpp
// 监控频率计数器
struct FreqLimitInfo {
    int current_count;
    int max_count;
    std::string log_id;
};

std::map<std::string, FreqLimitInfo> g_freq_limits;

// Hook FreqLimit 函数
void Hook_FreqLimit_FreshCacheCount(void* this_ptr, ...) {
    // 调用原始函数
    Original_FreqLimit_FreshCacheCount(this_ptr, ...);
    
    // 读取计数器
    FreqLimitInfo info = ReadFreqLimitInfo(this_ptr);
    
    // 记录
    g_freq_limits[info.log_id] = info;
    
    // 检查是否接近限制
    if (info.current_count > info.max_count * 0.7) {
        // 发出警告
        NotifyRiskLevel(RiskLevel::HIGH);
        LogWarning("FreqLimit approaching: %s (%d/%d)",
            info.log_id.c_str(),
            info.current_count,
            info.max_count);
    }
}

// 在操作前检查频率
bool CheckFrequencyBeforeAction(const std::string& action) {
    auto it = g_freq_limits.find(action);
    if (it == g_freq_limits.end()) {
        return true;  // 无限制
    }
    
    // 保持在 50% 以下
    return it->second.current_count < it->second.max_count * 0.5;
}
```

### 6. 用户行为上报

**检测机制**：
- CGI：`useractionreport`
- 上报内容：点击位置、滑动速度、输入节奏

**绕过方案**：类人行为模拟

```cpp
class HumanBehaviorSimulator {
public:
    // 模拟鼠标点击
    void SimulateClick(int x, int y) {
        // 1. 移动鼠标（贝塞尔曲线）
        SimulateMouseMove(x, y);
        
        // 2. 随机延迟
        Sleep(RandomDelay(50, 200));
        
        // 3. 点击
        mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
        Sleep(RandomDelay(30, 80));
        mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
    }
    
    // 模拟键盘输入
    void SimulateTyping(const std::string& text) {
        for (char c : text) {
            // 随机延迟（模拟打字节奏）
            Sleep(RandomDelay(50, 200));
            
            // 按键
            keybd_event(c, 0, 0, 0);
            Sleep(RandomDelay(30, 80));
            keybd_event(c, 0, KEYEVENTF_KEYUP, 0);
            
            // 随机打字错误
            if (Random() < 0.02) {
                SimulateTypo(c);
            }
        }
    }
    
private:
    // 鼠标移动（贝塞尔曲线）
    void SimulateMouseMove(int target_x, int target_y) {
        int start_x, start_y;
        GetCursorPos(&start_x, &start_y);
        
        // 生成贝塞尔曲线
        auto points = GenerateBezierCurve(
            start_x, start_y,
            target_x, target_y,
            Random(30, 50)  // 点数
        );
        
        // 沿曲线移动
        for (const auto& point : points) {
            SetCursorPos(point.x, point.y);
            Sleep(Random(5, 15));
        }
    }
    
    // 随机延迟（Gamma 分布）
    int RandomDelay(int min_ms, int max_ms) {
        // Gamma 分布模拟人类行为
        double gamma = RandomGamma(2.0, 0.3);
        int delay = (int)(gamma * (max_ms - min_ms) + min_ms);
        return std::clamp(delay, min_ms, max_ms);
    }
};
```

### 7. 远程命令拦截

**检测机制**：
- 函数：`CmdProcessor::OnIPxx` @ 0x1828E36C0
- 圈复杂度：131
- 可远程上传日志、崩溃转储

**绕过方案**：

```cpp
// 敏感命令列表
std::set<std::string> g_sensitive_commands = {
    "__SetLog",           // 远程调日志
    "__SetDebugIP",       // 远程设调试 IP
    "__UploadLog",        // 上传日志
    "__UploadCrash",      // 上传崩溃
    "__SetAgentCheck",    // 开启 agent 检测
};

// Hook CmdProcessor::OnIPxx
void Hook_CmdProcessor_OnIPxx(void* this_ptr, void* xml_data) {
    // 解析 XML 命令
    std::string command = ParseXMLCommand(xml_data);
    
    // 检查是否是敏感命令
    if (g_sensitive_commands.count(command)) {
        // 记录但不执行
        LogWarning("Blocked remote command: %s", command.c_str());
        return;
    }
    
    // 其他命令正常执行
    Original_CmdProcessor_OnIPxx(this_ptr, xml_data);
}
```

---

## P2: 中优先级绕过

### 8. 网络指纹

**策略**：复用微信原生网络栈，不自建连接

```cpp
// 使用微信原生的 Mars STN
// 不自建 MMTLS 连接
// 通过 Mmmojo 代理发送消息
```

### 9. 日志系统

**策略**：Hook 日志输出，用于监控

```cpp
// Hook XLogger
void Hook_XLogger_Write(int level, const char* module, const char* msg) {
    // 记录到我们的日志
    OurLogger::Log(level, module, msg);
    
    // 检查是否有敏感信息泄露
    if (ContainsSensitiveInfo(msg)) {
        LogWarning("Sensitive info in log: %s", msg);
    }
}
```

---

## Hook 安装顺序

```cpp
bool InstallAllHooks() {
    // 1. 初始化 Hook 框架
    MH_Initialize();
    
    // 2. 安装 P0 级别的 Hook（必须成功）
    if (!InstallIntegrityHook()) return false;
    if (!InstallProcessEnumHook()) return false;
    if (!InstallDebuggerHook()) return false;
    
    // 3. 安装 P1 级别的 Hook
    InstallFreqLimitHook();
    InstallRemoteCommandHook();
    
    // 4. 安装 P2 级别的 Hook
    InstallLoggerHook();
    
    // 5. 启用所有 Hook
    MH_EnableHook(MH_ALL_HOOKS);
    
    return true;
}
```

---

## 安全检查清单

在启动前，确保：

- [ ] 不 Hook CloseHandle
- [ ] 不 Hook DuplicateHandle
- [ ] 不修改 Weixin.dll 文件
- [ ] 单实例运行（检查互斥量）
- [ ] 从 explorer.exe 启动（检查父进程）
- [ ] 正常桌面环境（不禁用 DWM）
- [ ] 真实网络栈（不禁用 IPv6）
- [ ] 不挂起线程
- [ ] 不修改系统时间

---

## 下一步

- [Hook 实现](hook-implementation.md) — 代码级实现
- [Mmmojo 代理](../architecture/mmmojo-proxy.md) — IPC 通信
- [XPlugin 注入](../architecture/xplugin-inject.md) — 原生 API
