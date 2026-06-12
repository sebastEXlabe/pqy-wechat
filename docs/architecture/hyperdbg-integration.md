# HyperDbg 集成方案

## 概述

本文档描述如何将 HyperDbg 集成到 PQY 项目中，用于实现微信函数的 EPT Hook。

## 为什么选择 HyperDbg

| 对比项 | DdiMon (旧) | HyperDbg (新) |
|--------|-------------|---------------|
| Windows 11 支持 | ❌ | ✅ |
| AMD-V 支持 | ❌ | ✅ |
| EPT Hook 成熟度 | 低 | 高 |
| 反检测能力 | 无 | HyperEvade |
| 脚本引擎 | 无 | 有 |
| SDK 支持 | 无 | libhyperdbg |
| 维护状态 | 停更 (2020) | 活跃 (2026) |
| 社区活跃度 | 低 | 高 |

## 架构设计

```
┌─────────────────────────────────────────────────────────────┐
│                    PQY 应用层                                 │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐          │
│  │   前端 UI   │  │  API 网关   │  │  Python 引擎 │          │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘          │
│         └────────────────┼────────────────┘                  │
│                          ▼                                    │
│  ┌─────────────────────────────────────────────────────┐    │
│  │              libhyperdbg SDK                         │    │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐ │    │
│  │  │ EPT Hook    │  │ 脚本引擎    │  │ HyperEvade  │ │    │
│  │  └─────────────┘  └─────────────┘  └─────────────┘ │    │
│  └─────────────────────────────────────────────────────┘    │
│                          ▼                                    │
│  ┌─────────────────────────────────────────────────────┐    │
│  │              HyperDbg VMM (内核态)                    │    │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐ │    │
│  │  │ Intel VT-x  │  │ EPT 管理    │  │ VM-exit 处理│ │    │
│  │  └─────────────┘  └─────────────┘  └─────────────┘ │    │
│  └─────────────────────────────────────────────────────┘    │
│                          ▼                                    │
│  ┌─────────────────────────────────────────────────────┐    │
│  │              微信进程 (WeChat.exe)                    │    │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐ │    │
│  │  │ mmojo_64.dll│  │ Weixin.dll  │  │ 其他模块    │ │    │
│  │  └─────────────┘  └─────────────┘  └─────────────┘ │    │
│  └─────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
```

## EPT Hook 原理

### 传统 Inline Hook (高风险)

```
原始代码:
  48 89 5C 24 08    mov [rsp+8], rbx
  48 89 6C 24 10    mov [rsp+10h], rbp
  ...

Hook 后:
  E9 XX XX XX XX    jmp handler      ← 修改代码，容易被检测
  48 89 6C 24 10    mov [rsp+10h], rbp
  ...
```

### EPT Hook (低风险)

```
物理页面 (原始):
  48 89 5C 24 08    mov [rsp+8], rbx
  48 89 6C 24 10    mov [rsp+10h], rbp
  ...

EPT 映射:
  读/写视图 → 原始页面 (不可见修改)
  执行视图 → 影子页面 (包含 0xcc 断点)

当执行到该地址时:
  1. CPU 触发 EPT Violation VM-exit
  2. HyperDbg 拦截并调用处理函数
  3. 处理函数可以:
     - 记录参数
     - 修改参数
     - 调用原始函数
     - 修改返回值
```

## 集成步骤

### 1. 编译 HyperDbg

```bash
# 克隆项目
git clone --recursive https://github.com/HyperDbg/HyperDbg.git

# 使用 VS 2022 打开解决方案
# hyperdbg/hyperdbg.sln

# 编译 Release x64
```

### 2. 集成 libhyperdbg SDK

```cpp
// 包含头文件
#include "libhyperdbg.h"

// 链接库文件
#pragma comment(lib, "libhyperdbg.lib")

// 初始化
HyperDbgLoadVmmModule();
```

### 3. 实现 EPT Hook

```cpp
// 使用 HyperDbg 脚本引擎
// !epthook <address> script { <handler> }

// 或使用 SDK API
// HyperDbgInterpreter("!epthook mmojo_64+0x1234 script { ... }");
```

### 4. 实现 Hook 处理函数

```cpp
// 脚本引擎方式
const char* script = R"(
    !epthook mmojo_64!SendMMMojoWriteInfo script {
        printf("SendMMMojoWriteInfo called\n");
        printf("  rcx = %llx\n", @rcx);
        printf("  rdx = %llx\n", @rdx);
        // 调用原始函数
        continue;
    }
)";

HyperDbgInterpreter(script);
```

## 关键 API

### libhyperdbg API

```cpp
// 加载 VMM 模块
INT HyperDbgLoadVmmModule();

// 卸载 VMM 模块
INT HyperDbgUnloadVmm();

// 执行命令
INT HyperDbgInterpreter(CHAR* Command);

// 获取 CPU 厂商
GENERIC_PROCESSOR_VENDOR HyperDbgGetProcessorVendor();
```

### HyperDbg 命令

```bash
# 加载 VMM
load vmm

# 设置 EPT Hook
!epthook <address> script { <handler> }

# 设置 Inline EPT Hook
!epthook2 <address> script { <handler> }

# 监控内存读写
!monitor <address> <size> r/w

# 隐藏调试器
!hide

# 卸载 VMM
unload vmm
```

## 风控规避

| 检测方式 | 风险等级 | HyperDbg 应对 |
|----------|----------|---------------|
| 代码完整性检查 | 🟢 极低 | EPT 影子页面 |
| 内存扫描 | 🟢 极低 | 不可见 |
| 模块枚举 | 🟢 极低 | 无新模块 |
| CFG 检测 | 🟢 极低 | 不修改代码 |
| 签名验证 | 🟢 极低 | 不修改文件 |
| 反调试检测 | 🟢 极低 | HyperEvade |
| 时间检测 | 🟢 极低 | RDTSC 模拟 |

## 下一步

1. 编译 HyperDbg
2. 测试 VMM 加载
3. 测试 EPT Hook
4. 集成到 PQY 项目
5. 实现 mmojo_64.dll Hook
