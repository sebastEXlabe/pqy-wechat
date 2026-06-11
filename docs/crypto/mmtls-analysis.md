# MMTLS 加密协议分析

## 概述

MMTLS (Mars Mobile TLS) 是腾讯自研的传输层加密协议，基于 TLS 1.3 裁剪并添加自定义扩展。WeChat 4.1.10.31 使用 MMTLS 2.0 版本。分析基于 `mars-wechat/mars/mm-ext/src/mmtls/` 源码路径。

## 一、加密算法栈

### 1.1 已识别的加密原语

| 算法 | 代码证据 | 用途 |
|------|----------|------|
| X25519 | "X25519 primitives for x86_64" (0x1864befc0) | ECDH 密钥交换 |
| Camellia | "Camellia for x86_64" (0x1851706bc) | 对称加密 |
| RC4 | "rc4(8x,int)" (0x18343b480) | 流加密(遗留) |
| SHA1 | "SHA1 block transform" (0x18512a3b0) | 哈希 |
| SHA256 | "SHA256 block transform" (0x1833aa160) | 哈希 + HMAC |
| SHA512 | "SHA512 block transform" (0x18512bda0) | 哈希 |
| Montgomery 乘法 | "Montgomery Multiplication" (0x184e5e623) | 大数运算 (ECC) |
| GF(2^m) 乘法 | "GF(2^m) Multiplication" (0x186703348) | 伽罗瓦域运算 |

### 1.2 OpenSSL 集成

微信使用定制版 OpenSSL (`crypto/`):
- `crypto/o_str.c` (0x1887b6279)
- `crypto/asn1/a_strex.c` (0x18887f04f)
- `crypto/asn1/a_bitstr.c` (0x188b56f12)
- `crypto/asn1/a_mbstr.c` (0x188bc8760)
- `crypto/asn1/a_strnid.c` (0x188d9bd6c)

CryptoAPI 导入:
- `CryptAcquireContextW`, `CryptCreateHash`, `CryptDecrypt`
- `CryptDestroyHash`, `CryptDestroyKey`
- `CryptEnumProvidersW`, `CryptExportKey`

### 1.3 密钥派生

**函数**: `derive_secret_key_and_iv` (0x188eb7c76)
- 从主密钥派生会话密钥和 IV
- 用于 Camellia 对称加密的密钥素材

**URL 加密**:
- `encrypturlstring` (0x188306027)
- `aeskey` (AES 密钥)
- 格式: "md5: %s productid: %s cdnurlstring: %s encrypturlstring: %s aeskey: %s"

## 二、握手协议

### 2.1 握手流程

```
Client                                    Server
  |---------- ClientHello ---------->|
  |  - 支持的密码套件                  |
  |  - X25519 公钥                    |
  |  - PSK 会话 Ticket (可选)         |
  |                                   |
  |<--------- ServerHello -----------|
  |  - 选定的密码套件                  |
  |  - X25519 公钥                    |
  |  - 证书链                        |
  |  - PSK 确认 (可选)               |
  |                                   |
  |--- 密钥派生 (ECDH + HKDF) ----->|
  |                                   |
  |<------ EncryptedExtensions ------|
  |<--------- Finished --------------|
  |                                   |
  |---------- Finished ------------->|
  |                                   |
  |<===== 加密应用数据 =============>|
```

### 2.2 ShortLink TLS 事件回调

**TLSShortLinkEventCallback** (0x188adcf7c):
| 方法 | 地址 | 功能 |
|------|------|------|
| IsFinishHandShake | 0x188adcf7c | 查询握手是否完成 |
| OnSendTo | 0x184905A50 | **监控每个发送字节** |
| OnReceive | 0x184905C30 | **监控每个接收字节** |
| OnFinishHandshake | 0x184905EB0 | 握手完成回调 |

**TLSLongLinkEventCallback** (0x188adf35c):
| 方法 | 地址 | 功能 |
|------|------|------|
| OnSendTo | 0x18492AEA0 | **监控每个发送字节** |
| OnReceive | 0x18492B1C0 | **监控每个接收字节** |
| OnFinishHandshake | 0x18492B430 | 握手完成回调 |

### 2.3 PSK (Pre-Shared Key) 会话复用

**MMTLS2ClientSessionCache**:
**源码**: `mars-wechat/mars/mm-ext/src/mmtls/mmtls_lib/client/client_channel/mmtls2_client_session_cache.cpp`

```
LoadFileCache (0x186473B10):
  ├── 从磁盘读取缓存的 SessionTicket
  ├── 验证有效期:
  │     current_time - store_time < valid_seconds
  ├── 如果过期:
  │     "LoadFileCache CheckSessionInfoValid fail,
  │      store_time:%llu valid_seconds:%u current_time:%llu
  │      delete_ret:%d"
  │     → 删除过期文件
  └── 如果有效:
        → 使用 PSK 快速握手
```

**Get expired sessionticket** (0x186474F80):
```
检查:
  current_time - memory_session_info.store_time
  > psk_info.valid_seconds
→ 如果过期: "Get expired sessionticket,
             current_time:%llu
             memory_session_info.store_time:%llu
             psk_info.valid_seconds:%u"
```

**会话根目录验证**:
- `mmtls2::ClientSessionCacheImpl::__CheckRootDir` (0x188e3589b)
- 验证会话缓存存储路径的有效性

**PSK 过期时间**:
- `"get psk expired time fail"` (0x188e31099)
- 从服务器配置文件解析 PSK 有效期

### 2.4 连接池过期管理

**ConnectionPool::__UpdateNextExpireTimestamp** (0x188e36a03):
- 维护连接池中每个连接的超时时间
- 日志: "next expire timestamp %_"

## 三、MMTLS 控制信息

### 3.1 MMTLSCtrlInfo
**源码**: `mars-wechat/mars/mm-ext/src/mmtls/mmtls_ctrl_info.cc`

**dispatch 通道**:
- `DispatchMmtlsCtrlInfo` → `MMCore::DispatchMmtlsCtrlInfo` (0x188749308)
- `DispatchHostInfo` → `MMCore::DispatchHostInfo` (0x188749133)
- `DispatchExtHostInfo` → `MMCore::DispatchExtHostInfo` (0x18874916F)

**MMTLSCtrlInfo 方法**:
| 方法 | 功能 |
|------|------|
| ClearAllMMtlsPsk (0x188ad304a) | 清除所有 PSK 密钥 — 强制重新握手 |
| RemoveMMTlsHostInfo (0x188ad30a3) | 移除特定主机的 MMTLS 信息 |
| IsMMTLS2HostEnabled (0x188ad30e0) | 检查主机是否启用 MMTLS 2.0 |

**服务端控制**:
- 服务端可通过 `DispatchMmtlsCtrlInfo` 下发控制指令
- 可以远程关闭特定主机的 MMTLS 支持
- 可以清除 PSK 密钥池，强制客户端重新进行完整握手
- `MmtlsCtrlInfo.use_mmtls=%_` 日志记录 MMTLS 使用状态

### 3.2 HostInfoLogic
**方法**:
- `DispatchHostInfo` (0x188ad3c54)
- `DispatchHostInfoNoMainHost` (0x188ad3cf4)

## 四、证书验证

### 4.1 OSVerifyCertificate
**字符串位置**: `mars::comm::OSVerifyCertificate` (0x18826711d)
- 调用操作系统级证书验证 API
- 验证服务器证书链的有效性

### 4.2 PKCS7 签名验证
**字符串位置**: `PKCS7_signatureVerify` (0x188872f15)
- 验证二进制和配置文件的 PKCS7 签名
- 用于客户端完整性验证

### 4.3 OpenSSL CMS
- `CMS_decrypt_set1_password` (0x188878a54)
- `CMS_RecipientInfo_set0_password` (0x188878ef7)
- 用于加密消息的 CMS (Cryptographic Message Syntax) 支持

## 五、MMTLS 错误处理

### 5.1 错误上报
| 函数 | 说明 |
|------|------|
| `ReportMMTLSError` (0x188ad1499) | MMTLS 层错误上报 |
| `__ReportKVIDKeyDataForMMTLS` (0x188d3b213) | 长连接 MMTLS KV 上报 |
| `__ReportIDKeyDataForMMTLS` (0x188d3b56f) | 短连接 MMTLS IDKey 上报 |
| `__ReportTlsError` (0x1848D54E0) | TLS 错误详细报告 |

### 5.2 会话超时
- `mmtls: timeNow=%_, expired_time=%_` (0x188d3bc06)
- 比较当前时间和过期时间，判断会话是否有效

## 六、自动化安全影响

### 6.1 关键风险点
1. **TLS 事件回调监控每条数据** — 任何异常大小的数据包会被 OnSendTo/OnReceive 捕获
2. **PSK 会话有严格有效期** — 超时后必须重新握手
3. **服务端可远程清除 PSK** — 通过 DispatchMmtlsCtrlInfo
4. **MMTLS 错误被独立上报** — ReportMMTLSError/ReportKVIDKeyDataForMMTLS

### 6.2 推荐策略
1. 复用微信原生 TLS 会话，不要自建 MMTLS 连接
2. Hook TLS 事件回调以获取明文 (OnSendTo/OnReceive 之后)
3. 监控 PSK 有效期，在过期前触发新握手
4. 不要修改 MMTLS 协议参数
