# 风控量化数据表

基于 Weixin.dll (4.1.10.31) 的精确逆向数据。所有地址和大小均通过 IDA Pro 验证。

## 一、风控函数复杂度量化

| 函数 | 地址 | 大小(字节) | 基本块数 | 圈复杂度 | 团队 |
|------|------|-----------|---------|---------|------|
| IntegrityCheckTable__Verify | 0x1805C4C70 | 2,613 | 93 | 31 | Kernel |
| FreqLimit___FreshCacheCount | 0x184946160 | 3,079 | 108 | 56 | SMC |
| CmdProcessor::OnIPxx | 0x1828E36C0 | 7,197 | 273 | 131 | MagicBox |
| NewGetDns__BuildRequest | 0x182887490 | 5,546 | 245 | 107 | STN |
| LoginState__SerializeSession | 0x180623B30 | 4,031 | 168 | 76 | Kernel |
| ShortLink___AsyncCheckAuth | 0x184796FE0 | 2,905 | 84 | 37 | STN |
| StnManager__OnCreate | 0x180D4E6B0 | 745 | 45 | - | STN |
| BadNetDetectLogic | 0x18487E3B0 | 2,034 | 56 | 26 | SDT |
| NetCheckLogic___ShouldNetCheck | 0x1847C3810 | 2,330 | 68 | 33 | STN |
| MessageQueue::RunLoop::Run | 0x1828D82C0 | 3,246 | 140 | 63 | Comm |
| MMTLS2ClientSessionCache | 0x186473B10 | 1,196 | 65 | 28 | MMTLS |
| ShortLinkWithMMTLS___PackMMtls | 0x1848D7B80 | 1,649 | - | - | MMTLS |
| ShortLinkWithMMTLS___MakeHttpPayload | 0x1848D6510 | ~3,072 | - | - | MMTLS |
| ShortLinkWithMMTLS___MakeTlsPayload | 0x1848D55A0 | ~3,296 | - | - | MMTLS |
| NetSource__SetLongLink | 0x182832370 | 2,038 | - | - | STN |
| MMDnsResolver::__RecvResponse | 0x185C28EC0 | 2,636 | 92 | 41 | MMDNS |
| WMI Bluetooth Enum | 0x180077AA0 | 2,430 | 104 | 49 | Kernel |
| Coroutine await handler | 0x1803C9AE0 | 2,161 | 109 | 55 | Kernel |

## 二、上报 CGI 量化

| CGI 名称 | reqid | 加密方式 | 域名 | 连接类型 |
|----------|-------|----------|------|----------|
| statreport | 0 | 无 | minorshort | short |
| clientperfreport | 0 | 无 | minorshort | short |
| useractionreport | 0 | 无 | minorshort | short |
| reportstrategy | 0 | 无 | minorshort | short |
| kvreport | 0 | 无 | minorshort | short |
| kvreportrsa | 218 | RSA | minorshort | short |
| rtkvreport | 0 | 无 | minorshort | short |
| newreportkvcomm | 0 | 无 | minorshort | short |
| newreportkvcommrsa | 0 | RSA | minorshort | short |
| reportidkey | 0 | 无 | minorshort | short |
| reportidkeyrsa | 0 | RSA | minorshort | short |
| newreportidkey | 0 | 无 | minorshort | short |
| newreportidkeyrsa | 0 | RSA | minorshort | short |
| getkvidkeystrategy | 0 | 可选RSA | minorshort | short |
| gamereportkv | 0 | 无 | minorshort | short |
| mmuploadmedia | 111 | 无 | minorshort | short |
| heartbeat | 238 | 无 | long | long |
| newauth | 178 | 无 | long | long |
| autoauth | 254 | 无 | long | long |
| manualauth | 253 | 无 | long | long |
| newinit | 27 | 无 | long | long |
| logout | 133 | 无 | long | short |
| newsync | 121 | 无 | long | long |
| newsendmsg | 237 | 无 | long | long |
| geta8key | 155 | 无 | long | long |
| tenpay | 185 | 无 | long | long |

## 三、IDC 节点量化

| IDC | 位置 | 长连接域名数 | 短连接域名数 | 上报域名数 |
|-----|------|-----------|-----------|----------|
| 1 | 华南 | 2 | 2 | 1 |
| 2 | 香港 | 1 | 1 | 1 |
| 3 | 深圳 | 1 | 1 | 1 |
| 4 | 加拿大 | 1 | 1 | 1 |
| 5 | 天津 | 3 | 3 | 2 |
| 8 | 马来西亚 | 1 | 1 | 1 |
| 9 | 新加坡 | 1 | 1 | 1 |

## 四、DNS 指纹参数量化

NewGetDns__BuildRequest 构造的 URL 包含 14 个参数:

| 参数 | 类型 | 示例/范围 |
|------|------|-----------|
| uin | uint32 | 用户唯一ID |
| clientversion | uint32 | 4.1.10.31 → 0x04010A1F |
| scene | int32 | 0-? |
| net | int32 | 0=无,1=WiFi,2=2G,3=3G,4=4G,5=5G |
| md5 | string(32) | 客户端MD5哈希 |
| devicetype | string | 设备类型标识符 |
| lan | string | 语言代码(如 zh_CN) |
| sigver | int32 | 签名版本号 |
| lasteffecttime | string | 上次生效时间戳 |
| xagreementid | uint32 | 用户协议ID |
| networkid | string | 基于MAC的网络标识 |
| networkidctx | string | 网络上下文 |
| mccmnc | string | 移动国家码+网络码 |
| regctx | uint32 | 注册上下文 |

## 五、StnCallbackBridge 虚函数量化

| 序号 | 偏移 | 方法名 | 调用保护 |
|------|------|--------|----------|
| 1 | +0x08 | SetCallback | 无 |
| 2 | +0x10 | MakesureAuthed | SRWLock Shared |
| 3 | +0x18 | TrafficData | 推测 SRWLock |
| 4 | +0x20 | OnNewDns | 推测 SRWLock |
| 5 | +0x28 | OnPush | 推测 SRWLock |
| 6 | +0x30 | Req2Buf | 推测 SRWLock |
| 7 | +0x38 | Buf2Resp | 推测 SRWLock |
| 8 | +0x40 | OnTaskEnd | SRWLock Shared |
| 9 | +0x48 | ReportConnectStatus | 推测 SRWLock |
| 10 | +0x50 | OnLongLinkNetworkError | 推测 SRWLock |
| 11 | +0x58 | OnShortLinkNetworkError | 推测 SRWLock |
| 12 | +0x60 | OnLongLinkStatusChange | 推测 SRWLock |
| 13 | +0x68 | GetLonglinkIdentifyCheckBuffer | 推测 SRWLock |
| 14 | +0x70 | OnLonglinkIdentifyResponse | 推测 SRWLock |
| 15 | +0x78 | RequestSync | 推测 SRWLock |
| 16 | +0x80 | RequestNetCheckShortLinkHosts | 推测 SRWLock |
| 17 | +0x88 | ReportTaskProfile | 推测 SRWLock |
| 18 | +0x90 | ReportTaskLimited | 推测 SRWLock |
| 19 | +0x98 | ReportDnsProfile | 推测 SRWLock |

## 六、文件路径量化（源文件映射）

| 源文件路径 | 编译单元 | 团队 |
|-----------|----------|------|
| E:\xwechat\agent\workspace\mars_win_llvm_2\mars\mars\stn\stn_manager.cc | StnManager | STN |
| E:\xwechat\agent\workspace\mars_win_llvm_2\mars\mars\stn\src\net_source.cc | NetSource | STN |
| E:\xwechat\agent\workspace\mars_win_llvm_2\mars\mars\stn\src\net_check_logic.cc | NetCheckLogic | STN |
| E:\xwechat\agent\workspace\mars_win_llvm_2\mars\mars\stn\src\shortlink.cc | ShortLink | STN |
| E:\xwechat\agent\workspace\mars_win_llvm_2\mars\mars\stn\src\weak_network_logic.cc | WeakNetworkLogic | STN |
| E:\xwechat\agent\workspace\mars_win_llvm_2\mars-private\mars\smc\src\util\freq_limit.cc | FreqLimit | SMC |
| E:\xwechat\agent\workspace\mars_win_llvm_2\mars-private\mars\smc\src\selfmonitor\self_monitor.cc | SelfMonitor | SMC |
| E:\xwechat\agent\workspace\mars_win_llvm_2\mars-private\mars\magicbox\src\cmd_processor.cc | CmdProcessor | MagicBox |
| E:\xwechat\agent\workspace\mars_win_llvm_2\mars-private\mars\magicbox\ipxx_manager.cc | IPxxManager | MagicBox |
| E:\xwechat\agent\workspace\mars_win_llvm_2\mars-wechat\mars\mm-ext\src\mmtls\shortlink_with_mmtls.cc | ShortLinkWithMMTLS | MMTLS |
| E:\xwechat\agent\workspace\mars_win_llvm_2\mars-wechat\mars\mm-ext\src\mmtls\mmtls_lib\client\client_channel\mmtls2_client_session_cache.cpp | SessionCache | MMTLS |
| E:\xwechat\agent\workspace\mars_win_llvm_2\mars-wechat\mars\mm-ext\src\newdns\newdns_resolver.cc | NewDnsResolver | DNS |
| E:\xwechat\agent\workspace\mars_win_llvm_2\mars-wechat\mars\mmdns\src\mmdns_resolver.cc | MMDnsResolver | DNS |
| E:\xwechat\agent\workspace\mars_win_llvm_2\mars-wechat\mars\sdt-ext\src\wechat_netcheck\bad_net_detect_logic.cc | BadNetDetect | SDT |
| E:\xwechat\agent\workspace\mars_win_llvm_2\mars-wechat\mars\sdt-ext\src\netcheck_dispatcher.cc | NetcheckDispatcher | SDT |
| E:\xwechat\agent\workspace\mars_win_llvm_2\mars\mars\sdt\src\checkimpl\http_detector.cc | HttpDetector | SDT |
| E:\xwechat\agent\workspace\mars_win_llvm_2\mars-wechat\mars\mm-ext\src\net_checker.cc | NetChecker | MMTLS |
| E:\xwechat\agent\workspace\mars_win_llvm_2\mars-wechat\mars\sdt-ext\src\wechat_netcheck\bad_net_detect_logic.cc | BadNetDetect | SDT |

## 七、加密算法使用量化

| 算法库 | 使用位置数 | 具体用途 |
|--------|----------|----------|
| SHA256 | 3 | 哈希、HMAC、签名验证 |
| SHA1 | 2 | 遗留兼容、证书指纹 |
| SHA512 | 1 | 高强度哈希 |
| X25519 | 1 | ECDH密钥交换 |
| Camellia | 1 | 对称加密 |
| RC4 | 1 | 遗留流加密 |
| Montgomery | 1 | ECC大数运算 |
| GF(2^m) | 1 | 伽罗瓦域 |

## 八、StnManager 内部状态量化

StnManager 对象大小: ~0x1C08 字节 (this+0x00 ~ this+0x1BFF)

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
| +0x1B30 | 4 | mmtls_state (0=未初始化,1=错误,4=完成) |
| +0x1B34 | 1 | forward_secrecy 标志 |
| +0x1B58 | 8 | SetServerUrl ptr |
| +0x1BAC | 8 | debug_host ptr |
| +0x1BC8 | 8 | user_headers ptr |
