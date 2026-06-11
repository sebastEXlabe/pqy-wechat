# 函数签名映射表 (Golden Reference)

WeChat 4.1.10.31 — Weixin.dll 关键函数地址

## Mars STN 网络层

| 地址 | 真实名称 | 原始名称 | 大小 |
|------|----------|----------|------|
| 0x180D4D870 | StnManager__StnManager | sub_180D4D870 | 0x22f |
| 0x180D4E140 | StnManager__OnInitConfigBeforeOnCreate | sub_180D4E140 | 0x1bc |
| 0x180D4E6B0 | StnManager__OnCreate | sub_180D4E6B0 | 0x2e9 |
| 0x180D4EAE0 | StnManager__OnDestroy | sub_180D4EAE0 | 0x350 |
| 0x180D4F0C0 | StnManager__OnNetworkDataChange | sub_180D4F0C0 | 0x10c |
| 0x180D4F320 | StnManager__SetStnCallbackBridge | sub_180D4F320 | 0xd4 |
| 0x180D4F460 | StnManager__MakesureAuthed | sub_180D4F460 | 0x126 |
| 0x180D4F5F0 | StnManager__TrafficData | sub_180D4F5F0 | 0x14b |
| 0x180D4F7B0 | StnManager__OnNewDns | sub_180D4F7B0 | 0x132 |
| 0x180D4F950 | StnManager__Req2Buf | sub_180D4F950 | 0x17d |
| 0x180D4FB40 | StnManager__Buf2Resp | sub_180D4FB40 | 0x188 |
| 0x180D4FD40 | StnManager__OnTaskEnd | sub_180D4FD40 | 0x1c0 |
| 0x180D4FFA0 | StnManager__ReportConnectStatus | sub_180D4FFA0 | 0x10a |
| 0x180D50110 | StnManager__OnLongLinkNetworkError | sub_180D50110 | 0x124 |
| 0x180D50430 | StnManager__GetLonglinkIdentifyCheckBuffer | sub_180D50430 | 0x102 |
| 0x180D505A0 | StnManager__OnLongLinkStatusChange | sub_180D505A0 | 0x13c |
| 0x180D50750 | StnManager__OnLonglinkIdentifyResponse | sub_180D50750 | 0x130 |

## MMTLS ShortLink 协议层

| 地址 | 真实名称 | 大小 |
|------|----------|------|
| 0x1848D7B80 | ShortLinkWithMMTLS___PackMMtls | 0x671 |
| 0x1848D6510 | ShortLinkWithMMTLS___MakeHttpPayload | ~0xC00 |
| 0x1848D55A0 | ShortLinkWithMMTLS___MakeTlsPayload | ~0xCE0 |
| 0x1848D54E0 | ShortLinkWithMMTLS___ReportTlsError | - |

## NetSource 地址管理

| 地址 | 真实名称 | 大小 |
|------|----------|------|
| 0x182832370 | NetSource__SetLongLink | 0x7f6 |
| 0x182835EB0 | NetSource__ReportLongIP | 0x20a |
| 0x182836EC0 | NetSource__GetShortLinkItems | 0x54b |

## StnCallbackBridge

| 地址 | 真实名称 | 大小 |
|------|----------|------|
| 0x18282C4B0 | StnCallbackBridge__SetCallback | 0xd6 |

## 风控系统

| 地址 | 真实名称 | 大小 |
|------|----------|------|
| 0x1803FEF70 | MMKV__checkFileCRCValid | 0x86 |
| 0x1805C4C70 | IntegrityCheckTable__Verify | 0xa35 |
| 0x18154D3F0 | IntegrityCheck__Process | 0x2dd |
| 0x180623B30 | LoginState__SerializeSession | 0xfbf |

## 网络检测

| 地址 | 真实名称 | 大小 |
|------|----------|------|
| 0x182887490 | NewGetDns__BuildRequest | 0x15aa |
| 0x1847C3810 | NetCheckLogic___ShouldNetCheck | 0x91a |
| 0x184796FE0 | ShortLink___AsyncCheckAuth | 0xb59 |
| 0x18487E3B0 | BadNetDetectLogic__BadNetDetectLogic | 0x7f2 |

## 会话管理

| 地址 | 真实名称 | 大小 |
|------|----------|------|
| 0x186473B10 | MMTLS2ClientSessionCache__LoadFileCache | 0x4ac |

## 频率控制

| 地址 | 真实名称 | 大小 |
|------|----------|------|
| 0x184946160 | FreqLimit___FreshCacheCount | 0xc07 |

## 总重命名函数: 42 个

---

## StnManager 对象布局 (运行时偏移量)

| 偏移 | 大小 | 字段 | 值 |
|------|------|------|-----|
| +0x00 | 8 | vtable ptr | 运行时地址 |
| +0x08 | 8 | net_callback_ctx | a2参数 |
| +0x58 | 8 | callback_bridge_ ptr | StnCallbackBridge* |
| +0x60 | 8 | SRWLock | 保护bridge |
| +0x1B10 | 8 | extra_callback | 另一个回调 |
| +0x1B30 | 4 | mmtls_state | 0/1/4 |
| +0x1B34 | 1 | forward_secrecy | bool |
| +0x1B58 | 8 | server_url | string ptr |

## StnCallbackBridge VTable (19 虚函数)

| 序号 | 偏移 | 方法 | 调用保护 |
|------|------|------|----------|
| 1 | +0x08 | SetCallback | 无 |
| 2 | +0x10 | MakesureAuthed | SRWLock Shared |
| 3 | +0x18 | TrafficData | SRWLock推断 |
| 4 | +0x20 | OnNewDns | SRWLock推断 |
| 5 | +0x28 | OnPush | SRWLock推断 |
| 6 | +0x30 | Req2Buf | SRWLock推断 ★Hook |
| 7 | +0x38 | Buf2Resp | SRWLock推断 ★Hook |
| 8 | +0x40 | OnTaskEnd | SRWLock Shared ★Hook |
| 9-19 | +0x48~+0x98 | (Report/Request方法) | — |

## Phase 1 验证的全局变量

| 变量 | 地址 | 类型 | 初始值 |
|------|------|------|--------|
| `dword_18A3B2B20` | SignallingKeeper period | i32 | 5000 |
| `dword_18A3B2B24` | SignallingKeeper keep | i32 | 20000 |
| `dword_18A79A200` | SmartHeartbeat | i32 | -1 |
| `dword_18A891E60` | Watchdog | i32 | 0 |
| `qword_18A8997F8` | HTTP vtable | i64 | 0 |
