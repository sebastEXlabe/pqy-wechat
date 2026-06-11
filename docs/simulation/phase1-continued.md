# Phase 1 静态提取 — 续

## 全局变量运行时状态快照

| 变量 | 地址 | 初始值 | 状态 |
|------|------|--------|------|
| `dword_18A869838` | LogSizeReporter TLS | 0 | 未使用(运行时分配) |
| `dword_18A8697E8` | LogSizeReporter 锁 | 0 | 未初始化 |
| `dword_18A868D08/D10/D18` | SignallingKeeper 定时器参数 | 0,0,0 | 运行时设置 |
| `dword_18A86A1D8` | MAX_MQ_SIZE 回调时间 | 0 | 未触发 |
| `dword_18A84F718` | 热补丁临界区 | 0 | 未初始化(HeapCreate后设置) |
| `dword_18A845338` | XWeb JSAPI 管理器 | 0 | 运行时创建 |
| `dword_18A892340` | AudioManager 单例 | 0 | 首次创建时设置 |
| `dword_18A88CDF8/CE08` | LogSizeReporter 路径缓存 | 0,0 | 运行时填充 |
| `dword_18A79A200` | SmartHeartbeat 全局 | **-1** | unset→INI加载 |

**结论**: 绝大多数运行时变量初始为 0 或 -1(unset)，运行后才被赋值。只有函数指针引用在 .data 段中有非零值。

## 嵌入资源配置

| 配置 | 路径 |
|------|------|
| GPU 黑名单 | `:/gui/gpu_config/block_list.json` (Qt 嵌入资源) |

## clicfg 完整清单补充

| clicfg 项 | 地址 | 分类 |
|-----------|------|------|
| clicfg_opensdk_fastlogin_enable | 0x188786D06 | OpenSDK 快速登录默认=1 |
| clicfg_opensdk_share_enable | 0x188789F56 | OpenSDK 分享默认=1 |
| clicfg_opensdk_pay_enable | 0x18878E25A | OpenSDK 支付默认=1 |
| clicfg_xwechat_voip_device_cfg | 0x1883E6E5B | VoIP 设备配置 |
| clicfg_xwechat_gamut_wxam_encode | 0x1883FEBC8 | 广色域编码 |
| clicfg_xwechat_autodownload_config | 0x1887EF237 | 自动下载配置 |
| wechat_install_path | 0x188261640 | 安装路径(环境变量/注册表) |

## 关键观察

1. **所有运行时变量初始为 0/-1** — 微信在启动过程中的 `sub_1800033C0` 才初始化这些变量
2. **GPU 黑名单是嵌入资源** — 不是远程下载，编译时嵌入
3. **OpenSDK 功能默认启用** (值为1) — fastlogin/share/pay
4. **SmartHeartbeat 使用 -1 表示"未设置"** — 需要从 INI 文件加载实际值
5. **函数指针引用在 .data 段中已有值** — `0x18A8E0680`=sub_18040F9D0 (loadMetaInfoAndCheck), `0x18A8E0038`=sub_18040A390 (isDiskOfMMAPFileCorrupted)
