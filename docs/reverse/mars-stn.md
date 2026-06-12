# Mars STN 协议分析

## 概述

Mars 是腾讯自研的移动网络框架，STN（Signaling Transport Network）是其核心子模块，负责所有网络通信。

## 关键类

### StnManager

管理所有网络任务的核心类。

| 方法 | 地址 | 说明 |
|------|------|------|
| StnManager | 构造函数 | 初始化 |
| MakesureAuthed | 确认认证状态 | 长连接认证 |
| OnTaskEnd | 任务结束回调 | 任务完成处理 |
| Req2Buf | 请求序列化 | 请求转字节 |
| Buf2Resp | 响应反序列化 | 字节转响应 |
| OnPush | 服务端推送 | 推送消息处理 |
| OnNewDns | DNS 变更通知 | DNS 更新 |

### StnCallbackBridge

网络事件回调中枢，19 个虚函数。

### NetSource

管理长连接/短连接地址。

| 方法 | 说明 |
|------|------|
| SetLongLink | 设置长连接地址 |
| SetShortlink | 设置短连接地址 |
| SetDebugIP | 设置调试 IP |
| GetShortLinkItems | 获取短连接列表 |

## 协议栈

```
应用层
    │
    ▼
Mars STN (任务调度)
    │
    ├─→ LongLink (长连接)
    │       │
    │       ▼
    │   MMTLS (加密)
    │       │
    │       ▼
    │   TCP Socket
    │
    └─→ ShortLink (短连接)
            │
            ▼
        MMTLS (加密)
            │
            ▼
        HTTP/HTTPS
```

## 源码路径

| 源文件 | 编译单元 | 团队 |
|--------|----------|------|
| mars/stn/stn_manager.cc | StnManager | STN |
| mars/stn/src/net_source.cc | NetSource | STN |
| mars/stn/src/net_check_logic.cc | NetCheckLogic | STN |
| mars/stn/src/shortlink.cc | ShortLink | STN |

## 下一步

- [Mars STN 集成](../architecture/mars-stn.md) — 集成方案
- [MMTLS 分析](mmtls.md) — 加密协议
