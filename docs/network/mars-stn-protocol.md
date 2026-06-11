# Mars STN (Signaling Transport Network) 协议全量分析

## 概述

Mars STN 是腾讯自研网络库 Mars 的核心传输层，负责微信所有网络通信的传输管理。分析基于 WeChat 4.1.10.31，源文件位于 `mars/mars/stn/` 和 `mars-wechat/mars/mm-ext/src/`。

## 一、整体架构

```
应用层 (WeChat UI / 自动化系统)
  ↕
StnManager (0x180D4D870) — 任务调度中枢
  ↕
StnCallbackBridge — 19个虚函数的回调接口
  ├── ShortLink (短连接) → MMTLS 加密 → TCP → 服务器
  └── LongLink (长连接)  → MMTLS 加密 → TCP → 服务器
       ↕
    NetSource (0x182832370) — DNS + 地址管理
       ↕
    NewDnsResolver — 14参数 DNS 指纹查询
       ↕
    NetCheckLogic — 连接质量监控
       ↕
    BadNetDetectLogic — 坏网检测
```

## 二、StnManager — 传输管理核心

### 2.1 构造函数
**地址**: `0x180D4D870`
**源码**: `mars/mars/stn/stn_manager.cc:54`

```
StnManager::StnManager(a1, a2):
  this+0x00 → vtable ptr (off_1883A9508)
  this+0x08 → a2 (net callback context)
  this+0x48 → internal state structures
  this+0x68 → network data structures
  this+0x88 → task queue
  this+0xA0 → long link state
  this+0xA3 → magic number: 0x746E6573 (ASCII "sent")
  this+0xA0 → magic number: 0x74726174 (ASCII "tart")
  初始化日志标签: "mars2"
```

### 2.2 虚函数表
**vtable**: `off_1883A9508` @ 0x1883a9508

StnManager 继承自多个接口，其虚函数表包含以下方法：

#### 生命周期方法
| 偏移 | 方法 | 地址 | 功能 |
|------|------|------|------|
| +0x00 | ~StnManager | — | 析构 |
| +0x08 | OnInitConfigBeforeOnCreate | 0x180D4E140 | 创建前初始化配置 |
| +0x10 | OnInitConfigBeforeOnCreateV2 | — | V2版本配置初始化 |
| +0x18 | OnInitConfigBeforeOnCreateWithEncoder | — | 带编码器的配置初始化 |
| +0x20 | OnCreate | 0x180D4E6B0 | 创建网络任务 |
| +0x28 | OnDestroy | 0x180D4EAE0 | 销毁网络任务 |
| +0x30 | OnNetworkDataChange | 0x180D4F0C0 | 网络状态变化回调 |

#### 网络操作方法
| 偏移 | 方法 | 地址 | 功能 |
|------|------|------|------|
| +0x38 | SetStnCallbackBridge | 0x180D4F320 | 设置回调桥 |
| +0x40 | MakesureAuthed | 0x180D4F460 | 确认认证状态 (SRWLock保护) |
| +0x48 | TrafficData | 0x180D4F5F0 | 流量数据上报 |
| +0x50 | OnNewDns | 0x180D4F7B0 | 新DNS通知 |
| +0x58 | OnPush | 0x180D4F7B0 | 服务端推送(与OnNewDns同函数) |
| +0x60 | Req2Buf | 0x180D4F950 | 请求序列化到缓冲区 |
| +0x68 | Buf2Resp | 0x180D4FB40 | 缓冲区反序列化为响应 |
| +0x70 | OnTaskEnd | 0x180D4FD40 | 任务结束回调 (调用 Bridge+0x40) |

#### 状态上报方法
| 偏移 | 方法 | 地址 | 功能 |
|------|------|------|------|
| +0x78 | ReportConnectStatus | 0x180D4FFA0 | 上报连接状态 |
| +0x80 | OnLongLinkNetworkError | 0x180D50110 | 长连接网络错误 |
| +0x88 | OnShortLinkNetworkError | — | 短连接网络错误 |
| +0x90 | OnLongLinkStatusChange | 0x180D505A0 | 长连接状态变更 |
| +0x98 | GetLonglinkIdentifyCheckBuffer | 0x180D50430 | 获取长连接身份校验数据 |
| +0xA0 | OnLonglinkIdentifyResponse | 0x180D50750 | 长连接身份校验响应 |
| +0xA8 | RequestSync | — | 请求同步 |
| +0xB0 | RequestNetCheckShortLinkHosts | — | 请求网络检测 |
| +0xB8 | ReportTaskProfile | — | 上报任务配置 |
| +0xC0 | ReportTaskLimited | — | 上报任务限制 |
| +0xC8 | __ReportDnsProfile | — | 上报DNS配置 |
| +0xD0 | GenSequenceId | — | 生成序列号 |
| +0xD8 | SetLonglinkSvrAddr | — | 设置长连接服务器地址 |
| +0xE0 | SetShortlinkSvrAddr | — | 设置短连接服务器地址 |

### 2.3 StnCallbackBridge 虚函数表

StnCallbackBridge 是 StnManager 与应用层的回调接口，共 19 个虚函数：

| 序号 | 偏移 | 方法 | 功能 | 调用者 |
|------|------|------|------|--------|
| 1 | +0x08 | SetCallback | 设置回调指针 | 初始化 |
| 2 | +0x10 | MakesureAuthed | 确认认证 | StnManager+0x40 |
| 3 | +0x18 | TrafficData | 流量数据 | StnManager+0x48 |
| 4 | +0x20 | OnNewDns | DNS变更 | StnManager+0x50 |
| 5 | +0x28 | OnPush | 推送 | StnManager+0x58 |
| 6 | +0x30 | Req2Buf | 请求序列化 | StnManager+0x60 |
| 7 | +0x38 | Buf2Resp | 响应反序列化 | StnManager+0x68 |
| 8 | +0x40 | OnTaskEnd | 任务结束 | StnManager+0x70 |
| 9 | +0x48 | ReportConnectStatus | 连接状态 | StnManager+0x78 |
| 10 | +0x50 | OnLongLinkNetworkError | 长连接错误 | StnManager+0x80 |
| 11 | +0x58 | OnShortLinkNetworkError | 短连接错误 | StnManager+0x88 |
| 12 | +0x60 | OnLongLinkStatusChange | 长连接状态 | StnManager+0x90 |
| 13 | +0x68 | GetLonglinkIdentifyCheckBuffer | 身份校验 | StnManager+0x98 |
| 14 | +0x70 | OnLonglinkIdentifyResponse | 身份响应 | StnManager+0xA0 |
| 15 | +0x78 | RequestSync | 请求同步 | StnManager+0xA8 |
| 16 | +0x80 | RequestNetCheckShortLinkHosts | 网络检测 | StnManager+0xB0 |
| 17 | +0x88 | ReportTaskProfile | 任务配置 | StnManager+0xB8 |
| 18 | +0x90 | ReportTaskLimited | 任务限制 | StnManager+0xC0 |
| 19 | +0x98 | ReportDnsProfile | DNS配置 | StnManager+0xC8 |

## 三、ShortLinkWithMMTLS — 短连接 MMTLS 协议

### 3.1 调用链

```
__PackMMtls (0x1848D7B80, 1649字节)
  ├── 参数检查: 验证 task_id, sock_id
  ├── __MakeHttpPayload (0x1848D6510, ~3072字节)
  │     ├── 生成唯一路径: "/mmtls/%08llx"
  │     ├── 构建完整URL: "http://" + host + path
  │     ├── HTTP头构造:
  │     │     ├── "Upgrade: mmtls"
  │     │     ├── "Host: <服务器地址>"
  │     │     ├── "X-Online-Host: <在线主机>"
  │     │     └── 用户自定义 Headers (遍历链表)
  │     └── 调用虚函数发送 HTTP 升级请求
  │           (qword_18A8997F8+0x10)
  └── __MakeTlsPayload (0x1848D55A0, ~3296字节)
        ├── 获取 MMTLS 客户端通道 (a5[46])
        ├── 检查前向安全状态 (a1+6964)
        ├── TLS 握手 (如果未完成):
        │     ├── 构建 TLS ClientHello
        │     ├── X25519 密钥交换
        │     ├── 证书验证
        │     └── PSK 会话复用(如果可用)
        ├── 加密应用数据
        ├── 调用 SetServerUrl (this+0x1B58)
        └── 返回码:
              ├── 0: OK — TLS 握手成功
              ├── -1: ERR_APP_DATA_NOT_SENT
              └── 其他: 通道初始化失败
```

### 3.2 详细函数分析

#### __PackMMtls (0x1848D7B80)
**源码位置**: `shortlink_with_mmtls.cc`, 行 497-502
**函数签名**: 
```cpp
bool ShortLinkWithMMTLS::__PackMMtls(
    uint64_t task_id,
    uint64_t sock_id, 
    int* error_code_out,
    int* error_subcode_out,
    void* output_buffer,
    void* task_context
);
```

**关键日志**:
- 成功: "send length: %_, %_, %_" (行 502)
- 失败: "mmtls error :(%_, %_) with sock %_" (行 497)

#### __MakeHttpPayload (0x1848D6510)
**源码位置**: `shortlink_with_mmtls.cc`, 行 410, 415, 450

**URL 构造流程**:
1. 生成随机路径: 调用 `sub_186866110(0)` 获取随机种子
2. 格式化: `/mmtls/%08llx` (64字节缓冲区)
3. URL 前缀: `http://` (7字节)
4. 拼接 Host: 从 task_context+224 读取
5. 添加 HTTP 头

**关键日志**:
- 行 410: "%_, complete_url=%_ host=%_"
- 行 415: "debug_host: %_"
- 行 450: "user headers:(%_, %_)"

#### __MakeTlsPayload (0x1848D55A0)
**源码位置**: `shortlink_with_mmtls.cc`, 行 341, 354, 362, 365, 373, 381

**TLS 状态机**:
```
+0x1B30 (a1+6960): mmtls_state
  0 → 未初始化
  1 → ERR_APP_DATA_NOT_SENT
  4 → TLS 握手完成

+0x1B64 (a1+6964): forward_secrecy 标志
```

## 四、NetSource — 地址管理

### 4.1 NetSource::SetLongLink (0x182832370)
**源码**: `mars/mars/stn/src/net_source.cc:148`

设置长连接服务器地址列表。参数:
- host 列表 (string vector)
- port 列表 (uint16 vector)
- debug_ip (可选 string)

日志: "task set longlink server addr, host:%_ port:%_ debugip:%_"

### 4.2 DNS 地址获取
**字符串**: `mars::stn::NetSource::GetLongLinkItems`, `NetSource::GetShortLinkItems`, `NetSource::GetShortLinkCellularItems`

## 五、网络检测系统

### 5.1 NetCheckLogic (0x1847C3810)
**源码**: `mars/mars/stn/src/net_check_logic.cc:168`

`__ShouldNetCheck` — 判断是否需要触发网络质量检测:
1. 读取成功率标志位 (a1+56)
2. 统计长连接/短连接最近N次请求的成功次数
3. 如果连续失败次数超过阈值 → 返回 true
4. 如果连续命中 → "continous hit netcheck strategy, skip this"

### 5.2 BadNetDetectLogic (0x18487E3B0)
**源码**: `mars-wechat/mars/sdt-ext/src/wechat_netcheck/bad_net_detect_logic.cc`

三层检测器:
- `longlink_checker_` — 长连接质量
- `shortlink_checker_` — 短连接质量
- `basenet_checker_` — 基础网络(ICMP/HTTP)

### 5.3 NetcheckDispatcher
**源文件**: `mars-wechat/mars/sdt-ext/src/netcheck_dispatcher.cc`

六种主动检测:
- `StartPingCheck` (0x1866E8190)
- `StartDnsCheck` (0x1866E8CD0)
- `StartTcpCheck`
- `StartHttpCheck`
- `StartTracerouteCheck`
- `StartReqBufCheck`

### 5.4 NetChecker::StartSniffIfNeed (0x184829490)
**源码**: `mars-wechat/mars/mm-ext/src/net_checker.cc:104`

按需启动网络数据包嗅探。通过时间戳间隔控制嗅探频率。
日志: "interval:%_ status_:%_, %_ ms"

## 六、DNS 解析系统

### 6.1 NewGetDns__BuildRequest (0x182887490)
**源码**: `mars-wechat/mars/mm-ext/src/newdns/newdns_resolver.cc`
**复杂度**: 245 基本块, 107 圈复杂度

构造 14 参数 DNS 请求 URL:
```
/cgi-bin/micromsg-bin/newgetdns?
  uin=%u              — 用户唯一标识
  clientversion=%u    — 客户端版本号
  scene=%d            — 场景标识
  net=%d              — 网络类型
  md5=%s              — 客户端 MD5 校验值
  devicetype=%s       — 设备类型
  lan=%s              — 语言设置
  sigver=%d           — 签名版本
  lasteffecttime=%s   — 最后生效时间
  xagreementid=%u     — 用户协议 ID
  networkid=%s        — 网络标识(基于 MAC)
  networkidctx=%s     — 网络上下文
  mccmnc=%s           — 移动国家码/网络码
  regctx=%u           — 注册上下文
```

### 6.2 MMDNS (MMDNS Resolver)
**函数**: `sub_185C28EC0` (0xA4C字节)
**源码**: `mars-wechat/mars/mmdns/src/mmdns_resolver.cc`

DNS 响应验证:
- 签名检查: "mmdns SignatureChecker verify_code %_"
- 响应完整性: "mmdns udp response is not entire"
- 最小长度检查: "mmdns receive length less than header length."

## 七、连接到 StnCallbackBridge 的调用模式

所有 StnManager 方法通过 `this+0x58` 处存储的 StnCallbackBridge 指针调用虚函数:

```cpp
// StnManager__MakesureAuthed (0x180D4F460)
void* bridge = this->callback_bridge_;  // this+0x58
if (bridge) {
    AcquireSRWLockShared(this+0x60);    // SRWLock 保护
    result = bridge->vtable[2](bridge, a2, a3);  // +0x10
    ReleaseSRWLockShared(this+0x60);
}
```

## 八、自动化接入点分析

### 8.1 可 Hook 的接口
1. **StnCallbackBridge 虚表** — 替换虚函数指针可拦截所有网络事件
2. **StnManager 方法** — Method hook 可监控网络状态变化
3. **Req2Buf/Buf2Resp** — 请求/响应序列化点，可捕获明文数据
4. **OnTaskEnd** — 任务结束点，可获取完整的请求/响应上下文

### 8.2 Hook 顺序要求
- 必须在 `SetStnCallbackBridge` 之后 Hook
- `MakesureAuthed` 使用 SRWLock，必须在锁释放前完成操作
- 所有回调需保证延迟 < 1ms 以避免触发时序检测

### 8.3 关键全局变量
- `qword_18A8997F8` — HTTP 请求发送虚函数表指针
- `off_1883A9508` — StnManager vtable
- `dword_18A891E60` — TLS 全局状态
