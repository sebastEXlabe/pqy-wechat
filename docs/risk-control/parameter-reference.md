# 风控参数量化参考手册

基于 WeChat 4.1.10.31 (Weixin.dll) 提取的所有可量化风控参数及触发条件。

## 一、频率限制参数

### 1.1 操作频率（客户端侧）
| 参数 | 存储位置 | 用途 | 推测阈值 |
|------|----------|------|----------|
| `times_click_per_day` | 0x1885d1bdb | 每日点击次数限制 | 服务端配置 |
| `max_times_per_day` | 0x1885d1bef | 每日最大操作次数 | 服务端配置 |
| `max_day` | 同函数 | 最大天数窗口 | 服务端配置 |
| `mmkv_flowlayer_quitguide_use_times_in_day` | MMKV键 | 每日退出引导使用次数 | 本地持久化 |
| `mmkv_flowlayer_quitguide_show_times_in_day` | MMKV键 | 每日退出引导展示次数 | 本地持久化 |

### 1.2 流量控制
| 参数 | 地址 | 用途 |
|------|------|------|
| `CdnManager::SetFlowlimitPerhour` | 0x1883adf88 | CDN每小时流量限制 |
| `mars::cdn::SetFlowlimitPerhour` | 0x18865c7d5 | CDN每小时流量限制(全局) |

### 1.3 报告频率
| 参数 | 地址 | 说明 |
|------|------|------|
| `hit freqlimit line` | 0x18875393a | KV报告频率限制触发器 |
| `LogId Ban` | 0x184946160 | LogId被封禁超限计数器 |
| `bancount` | 0x184946160 | 连续超限次数（封禁前累积） |
| `maxcount` | 0x184946160 | 最大允许次数（超则封禁） |

## 二、超时与时间阈值

### 2.1 任务超时
| 参数 | 地址 | 说明 |
|------|------|------|
| `anr_timeout` | 0x188750f88 | 任务ANR超时阈值 |
| `session timeout` | 0x188ac6fd2 | 任务会话超时 |
| `delay autotask %_ seconds` | 0x188ae4aa0 | 服务端过载延迟秒数 |
| `restart_delay` | 0x1800f90f0 | 重启延迟时间 |
| `allowed_time` | 0x1800f90f0 | 允许操作的时间窗口 |

### 2.2 网络检测超时
| 参数 | 地址 | 说明 |
|------|------|------|
| NetStat threshold | 0x182847850 | `%_ - %_ = %_ ms, threshold %_ s` |
| `maxValidDataTime` | 策略服务器 | 策略数据最大有效时间 |

### 2.3 会话超时
| 参数 | 来源 | 说明 |
|------|------|------|
| PSK valid_seconds | MMTLS会话缓存 | PSK票据有效期（推测2小时） |
| session.store_time | MMTLS会话缓存 | 会话存储时间戳 |
| MMTLS expired_time | 0x188d3bc06 | 加密通道过期时间 |

## 三、计数与大小限制

### 3.1 消息与队列
| 参数 | 地址 | 限制类型 |
|------|------|----------|
| `MAX_MQ_SIZE` | 0x18875117c | 消息队列最大大小 |
| `Over MAX_MQ_SIZE, size:%d` | 0x18875117c | 超限日志 |

### 3.2 缓存与数据
| 参数 | 地址 | 限制类型 |
|------|------|----------|
| `max_page_count` | 0x188453568 | SQLCipher最大页数 |
| `itemSizeLimit` | 0x18823d0f2 | MMKV条目大小限制 |
| `max_count` | 0x1885d1b10 | 视图数量限制 |

### 3.3 重试
| 参数 | 地址 | 限制类型 |
|------|------|----------|
| `max-retry-count` | 0x188285c5d | 最大重试次数 |
| `max_retry_count` | 0x18865e3f0 | 任务级最大重试次数 |

## 四、多开与环境检测参数

### 4.1 多实例检测 (sub_1800F90F0)
| 参数 | 类型 | 说明 |
|------|------|------|
| `allow_multi_open` | bool | 允许多开总开关 |
| `enable` | bool | 检测启用开关 |
| `check_four_screen` | bool | 检测4屏配置（物理显示器数） |
| `require_hidden` | bool | 要求进程隐藏（反窗口检测） |
| `require_lock` | bool | 要求锁屏状态 |
| `require_no_input` | bool | 要求无用户输入（空闲检测） |
| `restart_delay` | int | 重启冷却时间（毫秒） |
| `allowed_time` | int | 允许的操作时间窗口 |

### 4.2 显示屏检测
| 参数 | 来源 | 说明 |
|------|------|------|
| `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Avalon.Graphics\DISPLAY1` | 0x1886d1746 | 显卡信息注册表 |
| `WM_DISPLAYCHANGE` | 消息 | 显示器变更通知 |

## 五、设备指纹采集参数

### 5.1 蓝牙枚举
| WQL查询 | 地址 | 说明 |
|----------|------|------|
| `SELECT * FROM Win32_PnPEntity WHERE PNPClass = 'Bluetooth'` | 0x180077AA0 | 蓝牙适配器硬件ID |

### 5.2 网络适配器
| API | 说明 | 采集数据 |
|-----|------|----------|
| `GetAdaptersAddresses` | 枚举网络适配器 | MAC地址、适配器GUID |
| `GetAdaptersInfo` | 适配器详细信息 | IP、子网掩码、网关 |

### 5.3 设备标识
| 键名 | 地址 | 说明 |
|------|------|------|
| `key_wechat_device_id` | 0x1881bb59e | 设备唯一ID(MMKV存储) |
| `qbMachineId` | 0x1881bb5df | QB浏览器机器ID前缀 |
| `MachineId` | 0x1881bb5ec | 机器ID前缀 |
| `deviceuuid` | 0x1886445d4 | 设备UUID |

### 5.4 系统信息
| API | 采集数据 |
|-----|----------|
| `GetNativeSystemInfo` | CPU架构、核数 |
| `GetLogicalProcessorInformation` | CPU拓扑详情 |
| `VerifyVersionInfoW` | Windows精确版本 |
| `GetVolumeInformationW` | 磁盘卷序列号 |

## 六、DNS指纹参数(14个)

| 参数 | 类型 | 来源 |
|------|------|------|
| uin | uint32 | 用户唯一ID |
| clientversion | uint32 | 客户端版本号 |
| scene | int32 | 场景标识 |
| net | int32 | 网络类型(0-5) |
| md5 | string | 客户端MD5 |
| devicetype | string | 设备类型 |
| lan | string | 语言/地区 |
| sigver | int32 | 签名版本 |
| lasteffecttime | string | 上次生效时间 |
| xagreementid | uint32 | 用户协议ID |
| networkid | string | 基于MAC的网络ID |
| networkidctx | string | 网络上下文 |
| mccmnc | string | 移动国家码/网络码 |
| regctx | uint32 | 注册上下文 |

## 七、完整性校验参数

### 7.1 MMKV CRC校验
| 参数 | 地址 | 说明 |
|------|------|------|
| `m_crcDigest` | 0x18823cf3a | 存储的CRC32值 |
| `crc32` | 计算值 | 文件实时CRC32 |
| `isDiskOfMMAPFileCorrupted` | 0x18823cb80 | MMAP文件损坏检测 |
| `compareBeforeSet` | 0x18823d1eb | 写入前比对验证 |

### 7.2 元数据校验
| 参数 | 地址 | 说明 |
|------|------|------|
| `loadMetaInfoAndCheck` | 0x18823cf93 | 元数据完整性验证 |
| `checkFileCRCValid` | 0x18823b536 | 文件CRC有效性检查 |
| `specious state` | 0x18823cfa8 | 元数据可疑状态检测 |

## 八、加密参数

### 8.1 业务层加密
| 算法 | 密钥大小 | 说明 |
|------|----------|------|
| AES-128-CBC-HMAC-SHA1 | 16字节 | 旧版 |
| AES-128-CBC-HMAC-SHA256 | 16字节 | 改进版 |
| AES-256-CBC-HMAC-SHA1 | 32字节 | 旧版高安全 |
| AES-256-CBC-HMAC-SHA256 | 32字节 | 改进高安全 |
| AES-128-GCM | 16字节 | 新标准 |
| AES-256-GCM | 32字节 | 新标准高安全 |
| AesGcmEncryptWithCompress | 可变 | ZLib+GCM加密 |
| AesGcmDecryptWithUncompress | 可变 | GCM解密+ZLib解压 |

### 8.2 MMTLS层
| 算法 | 用途 |
|------|------|
| X25519 | ECDH密钥交换 |
| Camellia | 对称加密 |
| Ed25519 | CA证书签名 |
| SHA256 | 哈希/HMAC |

## 九、多团队独立上报汇总

| 团队 | 上报CGI数 | 加密通道数 | 独立频率控制 |
|------|----------|----------|------------|
| STN | 26 | 3 (短/长/统计) | ✓ |
| SMC | 5 | 2 (KV/IDKey) | ✓ Ban/UnBan |
| Andromeda | 1 | 2 (HTTPx/QUIC) | ✓ |
| CDN | 2 | 1 | ✓ hourly |
| MagicBox | 3 | 1 | ✓ |
| SDT | 6 | 1 | ✓ |
| SelfMonitor | 1 | 1 | ✓ (元监控) |
| mmui | 0 | 0 | 客户端侧 |
| Kernel | 2 | 1 | ✓ clicfg |
| MMTLS | 2 | 1 | ✓ PSK expire |

## 十、使用时序检测点

每个检测点的**延迟容忍度**：

| 检测点 | 最大延迟 | 测量方式 |
|--------|----------|----------|
| GetVolume 卷检查 | 推测 <100ms | `[TIMING] GetVolume check took %.3f ms` |
| SetCaptureDevice | 推测 <200ms | `[TIMING] SetCaptureDevice took %.3f ms` |
| TLS握手完成 | 正常RTT范围 | OnFinishHandshake回调 |
| 协程恢复 | 精确同步 | `await() failed, unexpected resume detected` |
| ANR超时 | anr_timeout参数 | `anr_timeout:%lld < cost:%llu` |
| 任务超时 | 服务端配置 | `delay autotask %_ seconds` |

---

## 使用说明

### 关键原则
1. **所有标注"服务端配置"的参数** → 值由 `clicfg` / `GetClientConfig` 动态下发
2. **所有MMKV存储的参数** → 值在本地持久化，可在运行时读取
3. **硬编码参数** → 需要从 `.rdata` 段提取具体数值
4. **时间参数** → 单位已在参数名中标注（ms/s/min/hour/day）

---

## 十二、新发现精确阈值 (Phase 5-6 验证)

### 12.1 线程池参数
| 参数 | 精确值 | 验证地址 |
|------|--------|----------|
| ThreadPoolMaxSize 默认 | **50** (0x32) | `sub_1847DC7F0` @ 0x1847dcab7 |
| ThreadPoolKeepAliveTime 默认 | **300,000 ms (5分钟)** | `sub_1847DC7F0` @ 0x1847dcbb6 |
| 初始线程数 | **20** | `sub_1847720A0` a3参数 |
| 最小线程数 | **4** | `sub_1847720A0` a4参数 |
| 线程池状态初始值 | **2** | `sub_1847721D0` +0xA8 |

### 12.2 信号保活参数
| 参数 | 精确值 | 地址 |
|------|--------|------|
| SignallingKeeper period | **5,000 ms (5秒)** | `dword_18A3B2B20` |
| SignallingKeeper keep_time | **20,000 ms (20秒)** | `dword_18A3B2B24` |

### 12.3 心跳参数
| 参数 | 精确值 | 地址 |
|------|--------|------|
| 默认心跳间隔 | **210,000 ms (3.5分)** | `GetNextHeartbeatInterval` |
| 最大心跳测试值 | **580,000 ms (9.67分)** | 同函数 |
| 心跳溢出保护 | **600,000 ms (10分)** | `0x927C0` 常量 |
| SmartHeartbeat 日志 | `%0 find the smart heart interval = %1` | `sub_185C0F6B0` |
| INI边界验证 | `last_heart_ < MaxHeartInterval && last_heart_ >= MinHeartInterval` | `sub_185C0F6B0` |

### 12.4 频率限制参数
| 参数 | 精确值 | 来源 |
|------|--------|------|
| Anti-Avalanche 最大记录数 | **31** (0x1F) | `__InsertRecord` |
| Anti-Avalanche 滑动窗口 | **30** (0x1E) | `__InsertRecord` |
| 频率计数阈值 | **106** | `__CheckRecord` @ 0x185C125A0 |
| 频率记录清除间隔 | **3,600,000 ms (1小时)** | `FrequencyLimit::Check` |
| DisasterFrequencyLimit 硬上限 | **600,000 ms (10分)** | `sub_185C5F150` @ 0x927C0 |
| Socket BAN 过期时间 | **300,000 ms (5分)** | `_isBaned` @ 0x1847E0070 |
| LogId Ban 触发 | 上报次数 > maxcount | `FreqLimit___FreshCacheCount` |
| LogId UnBan 触发 | 上报次数 < maxcount | 同函数 |

### 12.5 超时参数
| 参数 | 精确值 | 来源 |
|------|--------|------|
| 超时延长 (timequota) | **120,000 ms (2分)** | `cdntask %_ timequota %_ extend 120s` |
| 过慢检测阈值 | **> 3,000 ms** 无响应 (0xBB8) | `TaskBase::OnRecvedData` |
| 首包超时 | WiFi/GPRS 分别配置 | `UpdateFirstPkgTimeoutConfig` |
| QC超时 | 服务端按 CGI 单独下发 | `recved cgi %_ qctimeout %_` |
| QRW超时 | 服务端按 CGI 单独下发 | `recved cgi %_ qrwtimeout %_` |
| DNS读/写超时 | `kTotalReadWriteTimeout` 运行时配置 | `newdns read/write timeout` |
| TCP用户超时 | `TCP_USER_TIMEOUT` socket选项 | 连接管理 |

### 12.6 慢速检测参数
| 参数 | 精确值 | 触发条件 |
|------|--------|----------|
| CDN慢速阈值 | ≤ **199 bytes/ms** | `bytes_received / elapsed_ms <= 0xC7` |
| 超时判定 | elapsed >= **1,000 ms** | `v9 < 0xF4628` 才跳过 |
| 错误码 | **4294957260** (0xFFFFF60C) | 慢速回调 |
| clicfg CDN慢速 | `clicfg_finder_cdn_redirect_slowspeed_threshold` | 默认1000-3000 |
| clicfg UGC慢速 | `clicfg_finder_cdn_ugc_slowspeed_threshold` | 默认1000-3000 |
| AVG速度监控 | `AvgSpeedMonitor::IsTooSlow` | `mars/cdn/src/net/listener/avg_speed_monitor.cc` |

### 12.7 流量配额参数
| 参数 | 说明 | 来源 |
|------|------|------|
| WiFi发送/接收阈值 | `wifi_data_threshold` | NetCheckTrafficMonitor |
| Mobile发送/接收阈值 | `mobile_data_threshold` | NetCheckTrafficMonitor |
| 流量雪崩检测 | `usedBytes > maxBytes` + 漏斗溢出 | `FlowAvalancheChecker` |
| limitReason=1 错误码 | **4294946175** (0xFFFFC0FF) | 限速 |
| limitReason=2 错误码 | **4294946066** (0xFFFFC0D2) | 雪崩触发 |

### 12.8 代理/环境检测参数
| 参数 | 说明 | 来源 |
|------|------|------|
| `clicfg_xwechat_agent_check` | 代理检测总开关 | 服务端 clicfg |
| `clicfg_xwechat_agent_protection_level` | 防护等级 | 服务端 clicfg |
| 多开检测参数 | `allow_multi_open` + `check_four_screen` + `require_hidden` + `require_lock` + `require_no_input` | `sub_1800F90F0` |
| Fake IPv6 检测 | 网关字符串前2字节 == **0x3A3A** ("::") | `local_ipstack_detect` |
| GetAdaptersAddresses 缓冲区 | **15,000 字节**，最多重试 **3次** | `GetAdaptersAddressesWrapper` |
| IP栈返回值 | 0=None / 1=IPv4 / 2=IPv6 / 3=Dual | `__local_ipstack_detect` |

### 12.9 RPA/风控打击参数
| 参数 | 说明 | 来源 |
|------|------|------|
| `rpa_strike_for_pc` | RPA打击配置键 | 服务器 sysmsg CMD |
| `acc_strike_status` | 打击状态 (0/1/2) | sysmsg 字段 |
| `acc_strike_expire_ts` | 打击过期时间戳 | sysmsg 字段 |
| `pc_forbid_screenshot` | 截图禁止配置 | 服务器 sysmsg CMD |
| `screenshot_status` | 截图禁止状态 | sysmsg 字段 |
| `screenshot_expire_ts` | 截图禁止过期时间戳 | sysmsg 字段 |

---

## 十三、LogSizeReporter 精确参数
| 参数 | 值 | 地址 |
|------|-----|------|
| 上报周期 | **600,000 ms (10分)** | MMCore 定时器 |
| KV ID | **16982** (0x4256) | `sub_1848C4290` |
| 文件格式 | `_YYYYMMDD.xlog` | `__GetTodayFilePath` |
| 最大延迟抖动 | **255 ms** | `__StartLogSizeReport` |

## 十四、CDN 错误码完整体系
共计 **204 个** CDN 错误码，分为 25 类别:
- **风控类**: FlowLimit, ClientFlowLimit, FlowControl, UploadFlowControl, DownloadFlowControl, PostBackFlowControl, OverloadControl, FlowAvalanche, UploadTooFast1-4, ForbidIllegalContent
- **速度类**: SpeedTooSlow, UgcSpeedTooSlow, UploadTooFast1-4, BtfsTimeout
- **安全类**: ForbidIllegalContent, UploadHevcIllegal, InvalidAuthKey, EncryptUrlNoToken, InvalidCertificate, RouteTlsDisabled
- **HTTP类**: Http00x~Http60plus, 14个子类

## 十五、QUIC 全局封禁
| 触发条件 | 效果 | 恢复 |
|----------|------|------|
| QUIC连接太慢 | `global ban quic` → 全部回退TCP | 新任务/主机评估 |

## 十六、代码热补丁系统
| 参数 | 说明 |
|------|------|
| 补丁算法 | Courgette (Chromium bsdiff) |
| 补丁格式 | JMP rel32 (0xE9, 5字节) |
| 补丁验证 | **无签名/哈希验证** |
| 补丁来源 | `patch.xml` → `DoPatchDownload` → `CourgettePatch` |
| 补丁应用 | VirtualProtect(PAGE_EXECUTE_READWRITE) → 写JMP → FlushInstructionCache |
| 线程安全 | SuspendThread → 应用补丁 → ResumeThread |

---

### 自动化前的必要步骤
1. 在运行时通过Hook读取所有MMKV存储的风控参数当前值
2. 监听 `GetClientConfig` 获取服务端下发的阈值
3. 基于实际值设定自动化操作的频率/间隔/模式
4. 确保所有操作参数保持在阈值的50%以下（安全边际）

---

## 十一、策略服务器参数（服务端下发）

### 11.1 策略版本控制
| 参数 | 说明 |
|------|------|
| `general_version` | 通用策略版本号 |
| `specail_version` | 特殊策略版本号 |
| `uin_version` | 用户级策略版本号 |
| `maxValidDataTime` | 策略数据最大有效时间 |
| `asksvrstrategyinterval` | 向服务器请求策略的间隔 |

### 11.2 采样控制
| 参数 | 说明 |
|------|------|
| `sample_mode` | 采样模式（全量/比例/关闭） |
| `sample_ratio` | 采样比例（如 0.01 = 1%） |
| `sample_valid_interval` | 采样有效间隔 |
| `monitor_flag` | 监控标志位 |
| `cycley` | 上报周期 |
| `opflag` | 操作标志位 |

## 十二、任务超时参数（CDN/STN任务系统）

| 参数 | 地址 | 说明 |
|------|------|------|
| `task first-pkg timeout` | 0x188acceee | 首包超时阈值 |
| `task pkg-pkg timeout` | 0x188accf38 | 包间超时阈值 |
| `task read-write timeout` | 0x188accf7e | 读写超时阈值 |
| `task timeout` | 0x188accfcc | 任务总超时 |
| `delay autotask %_ seconds` | 0x188ae4aa0 | 服务端过载延迟（秒） |

## 十三、ANR检测条件

**代码**: `0 >= anr_timeout || anr_timeout >= (int64_t)(timeend - timestart)`

**触发逻辑**:
1. 如果 `anr_timeout` <= 0 → 跳过检测
2. 如果 `timeend - timestart` < `anr_timeout` → 正常
3. 如果 `timeend - timestart` >= `anr_timeout` → 触发ANR告警

## 十四、多开检测触发条件（完整）

**函数**: `sub_1800F90F0` (3376字节, 119BB)

**参数列表**（零结尾字符串 @ 0x1881bf9a0）:
```
screen              ← 屏幕参数检测
allow_multi_open    ← 多开总开关
restart_delay       ← 重启冷却延迟
allowed_time        ← 允许时间窗口
require_lock        ← 需要锁屏状态
require_no_input    ← 需要无输入状态
```

**每个参数子函数** `sub_1805AA010` → `sub_1800FBB20` 从配置源读取实际值。

## 十五、IP缓存超时

**代码**: `ip is timeout, last_time=%0, cur_time=%1, _host_name:%2`

IP 地址缓存的 TTL 由 `last_time` vs `cur_time` 差值判断。

## 十六、心跳间隔

**代码**: `%0 find the smart heart interval = %1`

长连接心跳间隔由服务端智能调整。

## 十七、SM2/SM3/SM4（国密算法支持）

二进制中包含完整 OpenSSL SMx 支持（用于国内合规场景）:
- SM2 → 椭圆曲线(ECC)国密标准
- SM3 → 哈希算法国密标准
- SM4 → 对称加密国密标准

（来源：`crypto/sm2`、`crypto/sm3`、`crypto/sm4` 相关字符串）

## 十八、关键触发条件速查表

| 检测项 | 触发条件 | 结果 |
|--------|----------|------|
| MMKV CRC | `计算CRC32 != m_crcDigest` | 触发恢复策略 |
| 完整性表 | 表项遍历失败 | 上报服务器 |
| FreqLimit Ban | `上报次数 > maxcount` | LogId被静默封禁 |
| FreqLimit UnBan | `上报次数 < maxcount` | LogId恢复 |
| VPN 切换 | `当前VPN状态 != 缓存VPN状态` | 日志"VPN Changed" |
| 网络ID变化 | 新网络ID不在缓存中 | "add new one" |
| ANR | `耗时 >= anr_timeout` | ANR告警 |
| 线程死亡 | 线程状态异常 | "thread should be dead" |
| 协程异常 | 非协程上下文调用协程API | 致命错误 |
| IP超时 | `cur_time - last_time > TTL` | IP重新解析 |
| 会话过期 | `current_time - store_time > valid_seconds` | 删除缓存，重新握手 |
| PSK过期 | 同上 | "Get expired sessionticket" |
| 灾难频率 | `时间窗口内超过限制` | "Freq Limit, in X milsec" |
| NetStat阈值 | `time_diff < threshold_s` | 跳过上报 |
| 进程句柄 | `handle == GetCurrentProcess()` | DCHECK断言 |
| 堆损坏 | HeapValidate检测到异常 | 取决于策略 |
| 模块签名 | 枚举DLL签名不匹配 | 上报/记录 |

---

## 十九、Runtime动态超时系统

| 参数 | 精确值 | 来源 |
|------|--------|------|
| DynamicTimeout 触发窗口 | **300,000 ms (5分钟)** | `0x493E1` @ `__StatusSwitch` |
| DynamicTimeout 状态数 | **6** (0=Reset/1=Check/2-4=Level/Default) | `__StatusSwitch` |
| 首包超时硬上限 | **3,600,000 ms (60分钟)** | `3600 * 1000 >= _init_first_pkg_timeout` |
| 首包超时参数数 | **8** (WiFi/GPRS/WiFiDyn/GPRSDyn × first_pkg + min_rate + task_delay) | `__FirstPkgTimeout` |

## 二十、IPv6 管理系统

| 参数 | 精确值 | 来源 |
|------|--------|------|
| IPv6 关闭过期 | **604,800 秒 (7天)** | `v4 + 604800` @ `CloseCurrentNetworkIPv6` |
| IPv6 连续失败阈值 | min=5, max=30, **default=10** | `clicfg_cdn_ipv6_continuous_failcount` |
| IPv6 过期 | min=30s, max=180s, **default=60s** | `clicfg_cdn_ipv6_expire_seconds` |
| _ExtremeAttemptToLoadCdnInfo | CDN 紧急降级加载 | CDNHostService |

## 二十一、配置系统 5 级层级

| 层级 | 来源 | 更新方式 | 示例 |
|------|------|----------|------|
| 1. MMKV 本地 | 客户端持久化 | 代码写入 | mmkv_key_*, mmkv_flowlayer_* |
| 2. clicfg 远程 | 服务端 KV 下发 | StrategyManager 拉取 | clicfg_xwechat_*, clicfg_cdn_* (66项) |
| 3. ConfigFile 策略 | 服务端文件下发 | GetStrategy→Parse→MergeToFile | freq_limit, strategy_svr |
| 4. AppConfig 代码 | 代码内配置 | UpdateConfig 分发 | AppConfig::UpdateConfig |
| 5. sysmsg CMD | 服务端推送 | 消息分发器 | rpa_strike, pc_forbid_screenshot |

## 二十二、报告系统三层 bypass 机制

| 标志 | 作用 |
|------|------|
| `is_reportnow` | 强制立即上报 — 绕过频率限制 |
| `_ignore_freq_check` | 忽略频率检查 — 绕过 Ban 封禁 |
| `_is_important` | 重要标记 — 免除频率控制 |
| ReportNow 文件系统 | `reportnow_` / `key_reportnow_` — 文件级即时上报队列 |

## 二十三、全部定时器/超时常数汇总

| 超时类型 | 毫秒 | 秒 | 分钟 | 小时 | 天 |
|----------|------|-----|------|------|-----|
| SignallingKeeper period | 5,000 | 5 | — | — | — |
| SignallingKeeper keep | 20,000 | 20 | — | — | — |
| LogSizeReporter抖动 | 255 | 0.255 | — | — | — |
| 超时延长(quota) | 120,000 | 120 | 2 | — | — |
| 慢速检测 | 1,000 | 1 | — | — | — |
| 过慢无响应 | 3,000 | 3 | — | — | — |
| 心跳默认 | 210,000 | 210 | 3.5 | — | — |
| Socket BAN | 300,000 | 300 | 5 | — | — |
| ThreadPool KeepAlive | 300,000 | 300 | 5 | — | — |
| DynamicTimeout窗口 | 300,000 | 300 | 5 | — | — |
| 心跳最大 | 600,000 | 600 | 10 | — | — |
| 灾难频率上限 | 600,000 | 600 | 10 | — | — |
| LogSizeReporter周期 | 600,000 | 600 | 10 | — | — |
| FreqLimit缓存刷新 | 1,200,000 | 1,200 | 20 | — | — |
| 策略数据有效期 | 7,200,000 | 7,200 | 120 | 2 | — |
| SpeedTest基础超时 | 10,000 | 10 | — | — | — |
| 首包超时上限 | 3,600,000 | 3,600 | 60 | 1 | — |
| 策略询问间隔 | 86,400,000 | 86,400 | 1,440 | 24 | 1 |
| IPv6关闭过期 | 604,800,000 | 604,800 | 10,080 | 168 | 7 |
| BuiltinIP TTL | 691,200,000 | 691,200 | 11,520 | 192 | 8 |

---

## 二十四、Phase 1 离线模拟验证结果

### 24.1 全局变量运行时初始值 (从 .data 段直接读取)

| 变量 | 地址 | 初始值 | 含义 |
|------|------|--------|------|
| `dword_18A3B2B20` | SignallingKeeper period | **5000** (5秒) | ✅ 与22.1节一致 |
| `dword_18A3B2B24` | SignallingKeeper keep_time | **20000** (20秒) | ✅ 与22.1节一致 |
| `dword_18A79A200` | SmartHeartbeat global | **-1** (unset→INI加载) | ✅ 与12.3节一致 |
| `dword_18A891E60` | Watchdog state | **0** (未初始化) | — |
| `byte_18A6566C0` | 短连接标志 | **1** (已启用) | — |
| `qword_18A8997F8` | HTTP请求虚表 | **0** (运行时设置) | — |

### 24.2 StnManager 对象布局 (运行时验证)

```
StnManager 对象:
  +0x00: vtable ptr            → 指向 30+ 虚函数表
  +0x08: net_callback_ctx      → 存储网络回调上下文
  +0x58: callback_bridge_ ptr  → StnCallbackBridge* (a1[11].Ptr)
  +0x60: SRWLock               → 保护 callback_bridge_ (a1+12)
  +0x1B10: 另一个回调指针
  +0x1B30: mmtls_state         → 0=初始 / 1=错误 / 4=完成
  +0x1B34: forward_secrecy     → 前向安全标志
  +0x1B58: SetServerUrl ptr    → 服务端URL

StnCallbackBridge VTable (19个虚函数):
  [0] +0x00: 析构函数
  [1] +0x08: SetCallback              ← 初始化时调用
  [2] +0x10: MakesureAuthed           ← SRWLock保护，Bridge+0x10
  [3] +0x18: TrafficData
  [4] +0x20: OnNewDns
  [5] +0x28: OnPush
  [6] +0x30: Req2Buf                  ★ 请求序列化Hook点
  [7] +0x38: Buf2Resp                 ★ 响应反序列化Hook点
  [8] +0x40: OnTaskEnd                ★ 任务结束Hook点
  [9] +0x48: ReportConnectStatus
  [10] +0x50: OnLongLinkNetworkError
  [11] +0x58: OnShortLinkNetworkError
  [12] +0x60: OnLongLinkStatusChange
  [13] +0x68: GetLonglinkIdentifyCheckBuffer
  [14] +0x70: OnLonglinkIdentifyResponse
  [15] +0x78: RequestSync
  [16] +0x80: RequestNetCheckShortLinkHosts
  [17] +0x88: ReportTaskProfile
  [18] +0x90: ReportTaskLimited
  [19] +0x98: ReportDnsProfile
```

### 24.3 从 .data 段确认的运行时 MMKV 键值

| 键名 | 存储内容 | 地址 |
|------|----------|------|
| mmkv_key_user_name | 用户名 | 0x1881B29F9 |
| mmkv_key_push_login_url_expired_time | 登录URL过期时间戳 | 0x1881B2A25 |
| mmkv_key_auto_auth_key | 自动认证密钥 | 0x1881B2A5D |
| mmkv_key_pc_account_name | PC账号名 | 0x1881B2A8A |
| mmkv_key_old_wechat_auto_login_loaded | 旧版自动登录标志 | 0x1881B2AD3 |
| mmkv_key_latest_login_uin | 最近登录UIN | 0x1881B2AF9 |
| mmkv_key_latest_login_username | 最近登录用户名 | 0x1881B2B13 |
| mmkv_key_notify_device_name | 通知设备名 | 0x1883B6513 |
| mmkv_flowlayer_quitguide_use_times_in_day | 每日使用计数 | 0x188970B73 |
| mmkv_flowlayer_quitguide_show_times_in_day | 每日展示计数 | 0x188970BC3 |

---

## 二十五、MMKV 自动过期参数

| 参数 | 值/来源 | 地址 |
|------|---------|------|
| `enableAutoKeyExpire` | 启用/禁用 MMKV 键自动过期 | `sub_18041A600` |
| `expiredInSeconds` | 用户配置的过期秒数 | MMKV_IO.cpp:1836 |
| 实际过期时间 | `expiredInSeconds + sub_186866110(0)` (随机抖动) | 防时序攻击 |
| `m_expiredInSeconds` | 存储的过期秒数 | `filtering expired keys... m_expiredInSeconds: %u` |
| `enableCompareBeforeSet` | 与自动过期互斥 | 开启过期时强制关闭 |
| `filtering expired keys` | 定期过滤过期键 | `filtering expired keys inside [%s] now: %u, m_expiredInSeconds: %u` |

---

## 二十六、关键结构体大小速查

| 结构体 | 大小 | 用途 |
|--------|------|------|
| StnManager | ~0x1C08 字节 | 网络任务管理核心 |
| MMTLS2 SessionInfo | 136 字节 | PSK 会话信息 |
| PSK Object (v65) | 56 字节 | PSK 密钥对象 |
| PSK Entry | 32 字节 | PSK 条目 |
| WriteInfo | 48 字节 | Mmmojo 写消息 |
| ReadInfo | 48 字节 | Mmmojo 读消息 |
| ThreadPool 对象 | 792 字节 | 线程池状态 |
| SpeedTest 结果 | 123+ 字节 | 测速报告 |
| FlowAvalancheChecker | 72 字节 | 流量雪崩检测 |
| ConnectionPool bucket_group | 48 字节 | 连接池分组 |

---

## 二十七、Phase 2 实机验证结果

### 27.1 进程架构验证

| PID | 类型 | 线程数 | 内存 | 角色 |
|-----|------|--------|------|------|
| 33712 | 主进程 | 133 | 314 MB | UI + 网络主控 |
| 36624 | 主进程 | 125 | 314 MB | 第二主进程 |
| 7312 | 主进程 | 32 | 69 MB | 轻量主进程 |
| 35884 | 子进程 | 14 | 24 MB | Mmmojo: wxpublic/wxutility |
| 38812 | 子进程 | 14 | 24 MB | Mmmojo: wxpublic/wxutility |
| 35908 | 子进程 | 8 | 18 MB | Mmmojo: wxplayer/xweb |
| 38840 | 子进程 | 8 | 18 MB | Mmmojo: wxplayer/xweb |
| 35940 | 子进程 | 12 | 14 MB | Mmmojo: worker |
| 38868 | 子进程 | 12 | 14 MB | Mmmojo: worker |
| 38380 | 子进程 | 11 | 17 MB | Mmmojo: 其他 |

**✅ 验证了逆向分析中的 5 种 Mmmojo 环境类型各启动约 2 个实例**

### 27.2 版本确认

| 项目 | 值 | 状态 |
|------|-----|------|
| 安装路径 | `C:\Program Files\Tencent\Weixin\4.1.10.31\` | ✅ |
| Weixin.dll 大小 | 183,098,416 字节 | ✅ 匹配 |
| 进程名 | `Weixin.exe` | ✅ |
| 注册表键 | `HKCU\Software\Tencent\Weixin` | ✅ |
| mmmojo_64.dll | 已加载 | ✅ |

### 27.3 注册表实际值

| 键 | 值 |
|----|-----|
| Version | 4065597983 (编码格式) |
| InstallPath | `C:\Program Files\Tencent\Weixin` |
| OldFileSavePath | `MyDocument:` |

### 27.4 实机内存分布

| 进程类别 | 数量 | 总内存 | 平均线程数 |
|----------|------|--------|-----------|
| 主进程 | 2 | 628 MB | 129 |
| 轻量进程 | 1 | 69 MB | 32 |
| Mmmojo 子进程 | 8 | ~144 MB | 11 |
| **总计** | **10** | **~900 MB** | — |
