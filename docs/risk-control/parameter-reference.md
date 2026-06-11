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
