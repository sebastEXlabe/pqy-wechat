# 跨团队独立风控系统架构

WeChat 4.1.10.31 (Weixin.dll) 包含至少 **10 个独立团队** 构建的风控/监控系统，每套系统独立运作、独立上报、互相校验。

---

## 团队 1: Mars STN — 传输网络层
**源文件路径**: `mars/mars/stn/src/`

| 组件 | 函数地址 | 风控功能 |
|------|----------|----------|
| StnManager | 0x180D4D870 | 网络任务管理 + 认证链 |
| NetCheckLogic::__ShouldNetCheck | 0x1847C3810 | 长/短连接成功率监控，自动触发网络检测 |
| WeakNetworkLogic | 0x188ace920 | RTT/信号强度/错误类型事件监控 |
| NetChecker::StartSniffIfNeed | 0x184829490 | 按需网络嗅探 |
| NetSource | 0x182832370 | DNS/短连接地址管理 |
| ShortLink::__AsyncCheckAuth | 0x184796FE0 | 短连接异步认证检查 |
| NewGetDns__BuildRequest | 0x182887490 | DNS 14参数指纹构造 (245BB) |

## 团队 2: Mars SMC — 状态机核心 + 元监控
**源文件路径**: `mars-private/mars/smc/src/`

| 组件 | 风控功能 |
|------|----------|
| SmcManager | 独立 KV/IDKey 上报管理 (`WriteKvData`, `WriteIDKeyData`, `WriteListKVData`, `WriteListIDKeyData`) |
| SelfMonitor | **元监控** — 监控上报系统本身是否异常 |
| FreqLimit::__FreshCacheCount | LogId Ban/UnBan 双向封禁 (108BB) |
| MonitorDataFile/MemFile | 监控数据持久化 + 自监控操作日志 |
| DataMemoryStorage::__CheckOpenImpl | 数据存储完整性检查 |

## 团队 3: Mars MagicBox — 服务端远程命令
**源文件路径**: `mars-private/mars/magicbox/src/`

| 远程命令 | 功能 | 风险等级 |
|----------|------|----------|
| CmdProcessor::__SetLog | 远程调整日志级别 | 🔴 |
| CmdProcessor::__SetDebugIP | 远程设置调试 IP | 🔴 |
| CmdProcessor::__UploadLog | **远程上传客户端完整日志** | 🔴🔴🔴 |
| CmdProcessor::__UploadCrash | **远程上传崩溃转储** | 🔴🔴🔴 |
| CmdProcessor::__BuildNetcheckErrFile | 构建网络检测错误文件 | 🟠 |
| CmdProcessor::__UploadNetcheckErrFile | 上传网络检测错误文件 | 🟠 |
| CmdProcessor::ExecuteCmd | 远程命令总入口 | 🔴 |
| IPxxManager | IP 信息独立上报系统 | 🟡 |
| CmdProcessor::OnIPxx | XML 远程命令解析 (273BB/131) | 🔴🔴🔴 |

## 团队 4: Mars SDT — 软件定义测试
| 组件 | 功能 |
|------|------|
| SdtManager::StartActiveCheck | 主动安全检查 |
| NetcheckDispatcher | Ping/DNS/TCP/HTTP/Traceroute/ReqBuf 全诊断 |
| CheckContentHandler | 检测内容处理 |

## 团队 5: Mars CDN — 内容分发网络
| 组件 | 功能 |
|------|------|
| CdnManager | CDN 层独立上报 (reportKV/reportKvNoFreqLimit) |
| CdnManager::requestSnsGetCdnDistance | CDN 地理定位 (province/city 级别) |
| TaskManager::_RunLoop_Check* | 任务配额/超时/重试/用户请求多维度检查 |
| TaskSuspendTracker | 任务暂停/恢复追踪 |

## 团队 6: Andromeda — Cronet HTTP 层
**源文件路径**: `mars-private/mars/andromeda/cronet/`

| 组件 | 功能 |
|------|------|
| CronetCore | Cronet 引擎管理 |
| CronetReporter::DoReport | 独立 Cronet 上报 |
| NetworkEstimate | RTT/吞吐量/连接类型变化通知 |
| CronetNetworkQualityEstimator | 网络质量估计 |
| 多 DNS 回调 | NewDns/HttpDns/BackupDns/CellularDns/SimpleDns |
| 协议使用监控 | HTTP1.x/HTTP2/QUIC 使用统计 |

## 团队 7: Kernel — 核心层
| 组件 | 功能 |
|------|------|
| clicfg_xwechat_agent_check | **服务端可远程开关的代理检测** |
| clicfg_xwechat_agent_protection_level | **可远程调整的防护等级** |
| ext_device_oplog | 设备操作日志 |
| Handle Tracking | CloseHandle/DuplicateHandle Hook 验证 |
| AccessibleChecker | UI Accessibility API 检测 (反自动化) |
| GetClientCheckForMinorDevice | 未成年人设备检测 |
| SQLCipher 加密数据库 | PRAGMA rekey + sqlcipher_export |
| GPUBlackList::CheckGPU | GPU 驱动黑名单 |
| CourgettePatch | 远程二进制补丁系统 |

## 团队 8: Kernel XWeb/WMPF — WebView/小程序
| 组件 | 功能 |
|------|------|
| WMPFDrm | 小程序 DRM 保护 |
| xweb_call_browser_window_activated | 浏览器窗口激活监控 |
| xweb_call_connect_job_net_error | 网络错误监控 |
| xweb_call_appmsgreport | 应用消息上报 |
| xweb_call_wechat_game_launched | 游戏启动监控 |

## 团队 9: mmui — UI 层
| 组件 | 功能 |
|------|------|
| LoginState/NonLoginState | 登录状态机 |
| AccessibleChecker | UI 自动化检测 |
| XValidatorTextEdit | 输入验证器 |
| XRegularExpressionValidator | 正则表达式验证 |
| PermissionSettingItemUi | 权限管理 |
| QFileSystemWatcher | 文件系统实时监控 |
| QClipboard monitoring | 剪贴板操作监控 |

## 团队 10: Chromium/WebRTC — 第三方定制
| 组件 | 功能 |
|------|------|
| GPUBlackList | GPU 黑名单 |
| CronetNetworkQualityEstimator | 网络质量估计 |
| DesktopCapturerFullscreenDetector | 全屏/桌面捕获检测 |
| device_enumeration_win | 音频设备枚举 |
| device_info_ds | 视频采集设备枚举 |
| Clipboard Format Listener | 剪贴板监控 |

---

## 关键交叉校验点

由于多套系统独立运行，存在以下**交叉发现**风险：

1. **SelfMonitor 监控 SMC 上报管道** → 如果某层 Hook 阻断上报，SelfMonitor 发现管道异常
2. **TaskManager 监控任务执行** → 如果自动化操作导致任务超时/异常重试，TaskManager 检测
3. **WeakNetworkLogic + NetCheckLogic + NetworkEstimate** → 三个团队独立监控网络质量
4. **CmdProcessor + clicfg** → 两个独立的远程控制通道
5. **Handle Tracking + Process Enumeration** → 多维度检测注入代码
