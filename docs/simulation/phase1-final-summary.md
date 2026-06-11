# Phase 1 离线模拟 — 最终汇总

## 提取方法

全部数据从 `.data` 段直接读取（`get_int`/`get_string`/`get_bytes`），无需注入代码、无需运行微信、零风险。

## 1. 全局变量运行时初始值

| 变量 | 地址 | 初始值 | 含义 | 运行时状态 |
|------|------|--------|------|-----------|
| SignallingKeeper period | 0x18A3B2B20 | 5000 | 心跳间隔(ms) | ✅ 已确认 |
| SignallingKeeper keep_time | 0x18A3B2B24 | 20000 | 保活超时(ms) | ✅ 已确认 |
| SmartHeartbeat | 0x18A79A200 | -1 | 全局默认(unset) | 运行时从INI加载 |
| Watchdog | 0x18A891E60 | 0 | 状态 | 运行时激活 |
| AudioManager | 0x18A892340 | 0 | 单例指针 | 首次创建时赋值 |
| LogSizeReporter TLS | 0x18A869838 | 0 | 线程局部锁 | 运行时创建 |
| SignallingKeeper timer | 0x18A868D08 | 0 | 定时器参数 | 运行时设置 |
| MAX_MQ last_cb | 0x18A86A1D8 | 0 | 上次回调时间 | 首次溢出时 |
| 热补丁临界区 | 0x18A84F718 | 0 | CS 对象 | HeapCreate后 |

## 2. 函数指针引用 (静态链接)

| 引用地址 | 指向函数 | 函数作用 |
|----------|----------|----------|
| 0x18A8E0680 | sub_18040F9D0 | loadMetaInfoAndCheck |
| 0x18A8E0038 | sub_18040A390 | isDiskOfMMAPFileCorrupted |

## 3. MMKV 键值清单 (18个)

### 登录/认证类
- `mmkv_key_user_name` — 用户名
- `mmkv_key_auto_auth_key` — 自动认证密钥
- `mmkv_key_pc_account_name` — PC账号名
- `mmkv_key_old_wechat_auto_login_loaded` — 旧版加载标志
- `mmkv_key_latest_login_uin` — 最近UIN
- `mmkv_key_latest_login_username` — 最近用户名
- `mmkv_key_push_login_url_expired_time` — 登录URL过期时间
- `mmkv_key_notify_device_name` — 通知设备名
- `mmkv_key_notify_device_version` — 通知设备版本

### 风控/流量控制类
- `mmkv_flowlayer_quitguide_use_times_in_day` — 每日使用计数
- `mmkv_flowlayer_quitguide_show_times_in_day` — 每日展示计数
- `mmkv_flowlayer_quitguide_show_time_ms` — 展示时间戳(ms)
- `mmkv_flowlayer_quitguide_show_days` — 展示天数

### 即时上报类
- `default_key_reportnow_` — 默认即时上报键
- `default_reportnow_` — 默认即时上报
- `key_reportnow_` — 即时上报键
- `reportnow_` — 即时上报数据

## 4. 嵌入资源

| 资源 | 路径 | 格式 |
|------|------|------|
| GPU 黑名单 | `:/gui/gpu_config/block_list.json` | JSON |
| CGI 路由配置 | 内嵌 XML (含 MD5+签名) | XML |
| MMTLS CA 证书 | 内嵌 PEM (4个Ed25519证书) | X.509 |

## 5. clicfg 所有项 (66项，含默认值)

### 风控/安全 (2项)
| 项 | 默认值 |
|----|--------|
| `clicfg_xwechat_agent_check` | 0 (未激活) |
| `clicfg_xwechat_agent_protection_level` | 0 (未激活) |

### CDN/网络 (15项)
| 项 | min | max | default |
|----|-----|-----|---------|
| cdn_ipv6_expire_seconds | 30 | 180 | 60 |
| cdn_quic_expire_seconds | 60 | 600 | 300 |
| cdn_ipv6_continuous_failcount | 5 | 30 | 10 |
| finder_cdn_redirect_slowspeed_threshold | 1000 | 3000 | 300 |
| finder_cdn_ugc_slowspeed_threshold | 20 | 300 | 75 |
| finder_cdn_reserve_safetime_ms | 1000 | 30000 | 2000 |
| finder_cdn_redirect_connect_timeout_ms | 1000 | 30000 | 2000 |
| finder_cdn_redirect_rwtimeoutms | 1000 | 30000 | 3000 |
| finder_cdn_pcdn_rwtimeoutms | 1000 | 10000 | 2000 |
| finder_cdn_ugc_rwtimeoutms | 1000 | 30000 | 3000 |
| finder_cdn_ugc_blocksize_kbytes | 256 | 4096 | 256 |
| marscdn_snsimage_maxretry | — | — | 9 |
| marscdn_snsimage_maxtimeout_seconds | — | — | 20 |

### OpenSDK (4项)
| 项 | 默认值 |
|----|--------|
| opensdk_fastlogin_enable | 1 (true) |
| opensdk_share_enable | 1 (true) |
| opensdk_pay_enable | 1 (true) |
| opensdk_native_enable | 1 (true) |

### UI/交互 (8项)
- voice_input_win: 默认 0
- voice_input_high_potential_days_win: 默认 30
- launch_service_remind_window_seconds: int
- launch_service_remind_applet_enable: bool
- sync_mobile_msg_days: 默认 14
- pc_need_replace_public_user_avatar: bool

### 更新 (4项)
- sparkle_check_update_interval
- sparkle_update_alert_interval
- mac_appstore_check_update_url_to_official
- mac_appstore_check_update_url_to_appstore

### 其他 (~30项)
- 大文件发送、文件提供者、地图、游戏编码、VoIP设备、元宝品牌等

## 6. 策略服务器默认值

| 参数 | 地址 | 默认值 |
|------|------|--------|
| maxValidDataTime | sub_184954510 | **7200秒** (2小时) |
| askSvrStrategyInterval | sub_184954510 | **86400秒** (1天) |
| channel | sub_184954510 | 0 |
| general_version | sub_184954510 | 0 |

## 7. 关键结论

1. **所有运行时变量初始为 0 或 -1** — 微信启动时才初始化
2. **函数指针引用在 .data 段中已有静态值** — 不需要运行时计算
3. **MMKV 键名全部明文存储** — 在 .rdata 段可直接读取
4. **clicfg 默认值硬编码在配置读取函数中** — 已全部提取
5. **GPU 黑名单和 CGI 路由是嵌入资源** — 编译时固定
6. **Phase 1 完全零风险** — 所有数据从静态分析获取，无需运行微信
