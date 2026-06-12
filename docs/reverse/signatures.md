# 函数签名映射表

基于 Weixin.dll (4.1.10.31) 的逆向分析。所有地址均通过 IDA Pro 验证。

## 统计

- 已重命名函数：42 个
- 覆盖模块：Mars STN、MMTLS、NetSource、StnCallbackBridge、风控系统、网络检测、会话管理、频率控制

---

## Mars STN 网络层

| 地址 | 真实名称 | 原始名称 | 大小 |
|------|----------|----------|------|
| 0x1848D7B80 | ShortLinkWithMMTLS::__PackMMtls | sub_1848D7B80 | 0x671 |
| 0x1848D6510 | ShortLinkWithMMTLS::__MakeHttpPayload | sub_1848D6510 | ~0xC00 |
| 0x1848D55A0 | ShortLinkWithMMTLS::__MakeTlsPayload | sub_1848D55A0 | ~0xCE0 |
| 0x180D4E6B0 | StnManager__OnCreate | sub_180D4E6B0 | 745B |
| 0x182832370 | NetSource__SetLongLink | sub_182832370 | 2038B |

---

## MMTLS 协议层

| 地址 | 真实名称 | 原始名称 | 大小 |
|------|----------|----------|------|
| 0x186473B10 | MMTLS2ClientSessionCache__LoadFileCache | sub_186473B10 | 1196B |
| 0x1848D7B80 | ShortLinkWithMMTLS__PackMMtls | sub_1848D7B80 | 1649B |

---

## 风控系统

| 地址 | 真实名称 | 原始名称 | 大小 |
|------|----------|----------|------|
| 0x1805C4C70 | IntegrityCheckTable__Verify | sub_1805C4C70 | 2613B |
| 0x184946160 | FreqLimit__FreshCacheCount | sub_184946160 | 3079B |
| 0x1828E36C0 | CmdProcessor__OnIPxx | sub_1828E36C0 | 7197B |

---

## 网络检测

| 地址 | 真实名称 | 原始名称 | 大小 |
|------|----------|----------|------|
| 0x1847C3810 | NetCheckLogic__ShouldNetCheck | sub_1847C3810 | 2330B |
| 0x18487E3B0 | BadNetDetectLogic | sub_18487E3B0 | 2034B |
| 0x184796FE0 | ShortLink__AsyncCheckAuth | sub_184796FE0 | 2905B |

---

## 会话管理

| 地址 | 真实名称 | 原始名称 | 大小 |
|------|----------|----------|------|
| 0x180623B30 | LoginState__SerializeSession | sub_180623B30 | 4031B |

---

## StnCallbackBridge 虚函数表

| 偏移 | 方法名 | 说明 |
|------|--------|------|
| +0x08 | SetCallback | 设置回调 |
| +0x10 | MakesureAuthed | 确认认证状态 |
| +0x18 | TrafficData | 流量数据 |
| +0x20 | OnNewDns | DNS 变更 |
| +0x28 | OnPush | 服务端推送 |
| +0x30 | Req2Buf | 请求序列化 |
| +0x38 | Buf2Resp | 响应反序列化 |
| +0x40 | OnTaskEnd | 任务结束 |
| +0x48 | ReportConnectStatus | 上报连接状态 |
| +0x50 | OnLongLinkNetworkError | 长连接网络错误 |
| +0x58 | OnShortLinkNetworkError | 短连接网络错误 |
| +0x60 | OnLongLinkStatusChange | 长连接状态变更 |
| +0x68 | GetLonglinkIdentifyCheckBuffer | 获取身份校验缓冲区 |
| +0x70 | OnLonglinkIdentifyResponse | 身份校验响应 |
| +0x78 | RequestSync | 请求同步 |
| +0x80 | RequestNetCheckShortLinkHosts | 请求网络检查 |
| +0x88 | ReportTaskProfile | 上报任务配置 |
| +0x90 | ReportTaskLimited | 上报任务限制 |
| +0x98 | ReportDnsProfile | 上报 DNS 配置 |

---

## StnManager 对象布局

| 偏移 | 大小 | 内容 |
|------|------|------|
| +0x00 | 8 | vtable ptr |
| +0x08 | 8 | net callback context |
| +0x10 | 8 | 内部指针 |
| +0x48 | 32 | 状态结构 |
| +0x68 | 32 | 网络数据结构 |
| +0x88 | 32 | 任务队列 |
| +0xA8 | 24 | 连接状态 |
| +0x1310 | 8 | callback_bridge_ ptr |
| +0x1B30 | 4 | mmtls_state |
| +0x1B34 | 1 | forward_secrecy 标志 |
| +0x1B58 | 8 | SetServerUrl ptr |
| +0x1BAC | 8 | debug_host ptr |
| +0x1BC8 | 8 | user_headers ptr |
