# PQY 代码安全审查工具

## 概述

基于 `docs/risk-control/bypass-strategy.md` 和 `docs/risk-control/risk-assessment.md` 的风险规则，自动审查代码安全性。

## 工具

| 工具 | 用途 | 使用方式 |
|------|------|----------|
| `code_review.py` | 代码审查 | `python scripts/code_review.py <file_or_dir>` |
| `auto_fix.py` | 自动修复 | `python scripts/auto_fix.py <file_or_dir>` |

## 使用方法

### 1. 审查单个文件

```bash
python scripts/code_review.py native/src/hook/crc_bypass.cpp
```

### 2. 审查整个目录

```bash
python scripts/code_review.py native/src/
```

### 3. 自动修复

```bash
python scripts/auto_fix.py native/src/
```

### 4. 审查并修复

```bash
# 先审查
python scripts/code_review.py native/src/

# 如果有违规，自动修复
python scripts/auto_fix.py native/src/

# 再次审查确认
python scripts/code_review.py native/src/
```

## 审查规则

### 🔴 红线规则（必须拒绝）

| 规则 | 检测模式 | 原因 |
|------|----------|------|
| 禁止 Hook CloseHandle | `MH_CreateHook.*CloseHandle` | 微信验证这些 Hook |
| 禁止 Hook DuplicateHandle | `MH_CreateHook.*DuplicateHandle` | 微信验证这些 Hook |
| 禁止修改 Weixin.dll | `WriteFile.*Weixin.dll` | CRC 完整性校验 |
| 禁止 CreateRemoteThread | `CreateRemoteThread` | 进程枚举检测 |
| 禁止 SuspendThread | `SuspendThread` | 线程状态检测 |
| 禁止修改系统时间 | `SetSystemTime` | 时序检测 |

### 🟠 高风险规则（警告）

| 规则 | 要求 |
|------|------|
| CRC 绕过 | 必须有异常处理 |
| 进程枚举绕过 | 必须过滤 pqy 相关进程 |
| 调试器绕过 | 必须同时 Hook NtQueryInformationProcess |

### 🟡 中风险规则（建议）

| 规则 | 建议 |
|------|------|
| Hook 函数 | 添加异常处理 |
| 关键操作 | 添加日志记录 |

## 自动修复能力

`auto_fix.py` 可以自动修复以下问题：

1. **添加异常处理** - 为 Hook 函数添加 `__try/__except`
2. **添加日志** - 为关键操作添加 `spdlog` 日志
3. **完善进程过滤** - 确保包含 pqy 相关进程

## 集成到 CI/CD

### Git Pre-commit Hook

```bash
#!/bin/bash
# .git/hooks/pre-commit

echo "Running code review..."
python scripts/code_review.py native/src/

if [ $? -ne 0 ]; then
    echo "❌ Code review failed. Please fix violations before committing."
    exit 1
fi

echo "✅ Code review passed."
```

### GitHub Actions

```yaml
name: Code Review

on: [push, pull_request]

jobs:
  review:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v3
      - name: Run Code Review
        run: python scripts/code_review.py native/src/
```

## 常驻子代理

代码审查子代理配置在 `.claude/agents/code-reviewer.md`，可以：

1. **自动审查** - 当代码变更时自动触发
2. **手动审查** - 用户请求时执行
3. **自动修复** - 对简单违规尝试修复
4. **报告输出** - 清晰的审查报告

### 触发方式

```bash
# 在 Claude Code 中
/code-review native/src/
审查代码 native/src/hook/
安全审查
```

## 输出示例

### 审查通过

```
✅ 代码审查通过

审查文件: native/src/hook/crc_bypass.cpp
审查规则: 全部通过
风险等级: 无
```

### 审查未通过

```
❌ 代码审查未通过

审查文件: native/src/hook/example.cpp
发现违规: 2 个

🔴 红线违规:
1. 第 42 行: 禁止 Hook CloseHandle
   代码: MH_CreateHook(CloseHandle, ...)
   修复: 移除此 Hook

🟠 高风险:
2. 第 55 行: CRC 绕过需要异常处理
   代码: bool Hook_IntegrityCheck() {
   修复: 添加 __try/__except
```

## 相关文档

- [风险规避策略](../docs/risk-control/bypass-strategy.md)
- [风险评估](../docs/risk-control/risk-assessment.md)
- [审查规则](../.claude/skills/code-reviewer/SKILL.md)
