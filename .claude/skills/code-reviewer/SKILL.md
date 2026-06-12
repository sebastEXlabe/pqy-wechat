# 代码安全审查工具

基于 `docs/risk-control/bypass-strategy.md` 和 `docs/risk-control/risk-assessment.md` 的风险规则，自动审查代码安全性。

## 触发条件

当用户说 `/code-review` 或提交代码变更时自动触发。

## 审查规则

### 🔴 红线规则（必须拒绝）

1. **禁止 Hook CloseHandle / DuplicateHandle**
   - 检测：`MH_CreateHook.*CloseHandle` 或 `MH_CreateHook.*DuplicateHandle`
   - 处理：立即拒绝，要求移除

2. **禁止修改 Weixin.dll 文件**
   - 检测：`WriteFile.*Weixin.dll` 或 `CreateFile.*Weixin.dll.*GENERIC_WRITE`
   - 处理：立即拒绝

3. **禁止 DLL 注入（使用 XPlugin 合法机制代替）**
   - 检测：`CreateRemoteThread` + `LoadLibrary`
   - 处理：立即拒绝，建议使用 XPlugin

4. **禁止挂起线程**
   - 检测：`SuspendThread` 或 `NtSuspendThread`
   - 处理：立即拒绝

5. **禁止修改系统时间**
   - 检测：`SetSystemTime` 或 `SetLocalTime`
   - 处理：立即拒绝

### 🟠 高风险规则（警告 + 建议修改）

6. **CRC 完整性绕过需要隐藏**
   - 检测：`Hook_IntegrityCheckTable_Verify`
   - 要求：必须有异常处理，建议使用 DdiMon

7. **进程枚举绕过需要完整**
   - 检测：`Hook_Process32NextW`
   - 要求：必须过滤所有 pqy 相关进程/模块

8. **调试器检测绕过需要完整**
   - 检测：`Hook_IsDebuggerPresent`
   - 要求：必须同时 Hook NtQueryInformationProcess

9. **频率限制必须监控**
   - 检测：发送消息/朋友圈相关代码
   - 要求：必须调用 CheckFrequencyBeforeAction

10. **远程命令拦截需要谨慎**
    - 检测：`Hook_CmdProcessor_OnIPxx`
    - 要求：必须有白名单机制，只拦截敏感命令

### 🟡 中风险规则（建议）

11. **类人行为模拟**
    - 检测：鼠标/键盘操作代码
    - 要求：建议使用 Gamma 分布延迟、贝塞尔曲线

12. **异常处理**
    - 检测：所有 Hook 函数
    - 要求：建议使用 __try/__except 包裹

13. **日志记录**
    - 检测：所有关键操作
    - 要求：建议使用 spdlog 记录

### 🟢 低风险规则（提示）

14. **单实例运行**
    - 检测：进程创建代码
    - 要求：建议检查互斥量

15. **父进程检查**
    - 检测：进程启动代码
    - 要求：建议检查父进程是否为 explorer.exe

## 审查流程

```
1. 读取变更文件
2. 逐行检查规则
3. 发现违规 → 生成报告
4. 自动修复（如果可能）
5. 重新检查
6. 输出审查结果
```

## 输出格式

```markdown
# 代码审查报告

## 审查结果：❌ 未通过

### 🔴 红线违规（必须修复）

1. **文件**: `src/hook/example.cpp:42`
   **规则**: 禁止 Hook CloseHandle
   **代码**: `MH_CreateHook(CloseHandle, ...)`
   **修复**: 移除此 Hook，使用 UIAutomation 代替

### 🟠 高风险警告（建议修复）

2. **文件**: `src/hook/crc_bypass.cpp:15`
   **规则**: CRC 绕过需要异常处理
   **代码**: `bool Hook_IntegrityCheckTable_Verify(void* this_ptr) {`
   **修复**: 添加 __try/__except 包裹

### ✅ 已通过规则

- [x] 不修改 Weixin.dll
- [x] 不挂起线程
- [x] 不修改系统时间
- [x] 频率限制已监控

## 自动修复建议

```cpp
// 修复前
bool Hook_IntegrityCheckTable_Verify(void* this_ptr) {
    return true;
}

// 修复后
bool __fastcall Hook_IntegrityCheckTable_Verify(void* this_ptr) {
    __try {
        spdlog::debug("IntegrityCheckTable::Verify bypassed");
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        spdlog::error("Exception in CRC bypass: 0x{:08X}", GetExceptionCode());
        return GetOriginalIntegrityCheck()(this_ptr);
    }
}
```
```

## 使用方式

```bash
# 审查单个文件
/code-review native/src/hook/crc_bypass.cpp

# 审查整个目录
/code-review native/src/

# 审查最近变更
/code-review --diff
```
