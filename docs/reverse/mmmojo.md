# Mmmojo IPC 协议分析

## 概述

Mmmojo 是微信的进程间通信（IPC）框架，用于主进程与子进程之间的通信。

## 关键导入函数

| 函数 | 地址 | 用途 |
|------|------|------|
| CreateMMMojoEnvironment | 0x188ee370c | 创建 Mojo 环境 |
| CreateMMMojoWriteInfo | 0x188ee3726 | 创建写入信息 |
| GetMMMojoReadInfoAttach | 0x188ee373e | 获取读取附件 |
| GetMMMojoReadInfoRequest | 0x188ee3758 | 获取读取请求 |
| SendMMMojoWriteInfo | mmmojo_64.dll | 发送写入信息 |

## 子进程类型

通过 Phase 2 实机验证，确认了 5 种 Mmmojo 子进程类型：

| 子进程 | 用途 |
|--------|------|
| WeChatApp.exe | 小程序/插件 |
| WeChatWeb.exe | 网页版 |
| WeChatUpdate.exe | 更新服务 |
| WeChatCrashReport.exe | 崩溃报告 |
| WeChatHelper.exe | 辅助进程 |

## 消息流程

```
主进程 (WeChat.exe)
    │
    ├─→ CreateMMMojoEnvironment()
    │
    ├─→ CreateMMMojoWriteInfo()
    │       │
    │       ▼
    │   填充消息数据
    │       │
    │       ▼
    └─→ SendMMMojoWriteInfo()
            │
            ▼
        IPC 通道
            │
            ▼
        子进程接收
```

## 安全考虑

1. **消息验证** — 微信可能验证消息来源
2. **序列化格式** — 需要精确匹配微信的序列化格式
3. **通道权限** — 不同通道可能有不同的权限

## 下一步

- [Mmmojo 代理设计](../architecture/mmmojo-proxy.md) — 代理实现方案
- [Hook 内核层](../architecture/hook-layer.md) — 风控规避
