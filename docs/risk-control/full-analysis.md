# 微信风控系统全量分析

基于 Weixin.dll (4.1.10.31) 的深度逆向分析。覆盖 10 个独立团队、60+ 风控点。

---

## 第一层：文件完整性 (5项)

### 1.1 PKCS7 数字签名
- 字符串: `PKCS7_signatureVerify` @ 0x188872f15
- DLL 加载时验证数字签名

### 1.2 完整性检查表
- 函数: `IntegrityCheckTable__Verify` @ 0x1805C4C70
- 93 个基本块，圈复杂度 31
- 遍历多个组件进行哈希/签名比对

### 1.3 MMKV 文件级 CRC
- 函数: `MMKV__checkFileCRCValid` @ 0x1803FEF70
- 源文件: `MMKV.cpp:446`
- 每个 MMKV 文件计算 CRC32 与 `m_crcDigest` 比对

### 1.4 MMKV 数据恢复
- 函数: `checkDataValid` @ 0x18040FD10
- 源文件: `MMKV_IO.cpp:355`
- CRC 失败后触发恢复策略

### 1.5 配置 XML 签名
- 内嵌 CGI 路由表有 `<md5>` 和 `<signature>` (ECDSA)
- 防止 CGI 路由表篡改

---

## 第二层：运行时环境检测 (13+项)

### 2.1 调试器检测
- `IsDebuggerPresent`
- `CheckRemoteDebuggerPresent`
- `OutputDebugStringA/W` (监听)

### 2.2 NtQuery 进程信息
- `NtQueryInformationProcess`
- ProcessDebugPort (0x07)
- ProcessDebugFlags (0x1F)
- ProcessDebugObjectHandle (0x1E)

### 2.3 进程枚举
- 函数: `sub_181415920` @ `gdi_debug_util_win.cc:321`
- `CreateToolhelp32Snapshot` → `Process32FirstW/NextW`
- 遍历所有运行进程

### 2.4 模块枚举
- `Module32First` — 检测注入的未签名 DLL

### 2.5 线程枚举
- `Thread32First`

### 2.6 WOW64 检测
- `IsWow64Process`, `IsWow64Process2`
- `NtWow64ReadVirtualMemory64`
- `NtWow64QueryInformationProcess64`

### 2.7 PEB 访问
- 字符串: `" PEB Failed "` @ 0x18844075e

### 2.8 Windows 版本
- `VerifyVersionInfoW`, `VerSetConditionMask`
- `RtlGetVersion`, `GetNativeSystemInfo`

### 2.9 WMI 硬件枚举
- WQL: `SELECT * FROM Win32_PnPEntity WHERE PNPClass = 'Bluetooth'`
- 函数: `sub_180077AA0` (104BB)

### 2.10 磁盘卷检查（含计时检测）
- `GetVolumeInformationW`, `DeviceIoControl`
- `GetVolume` + `[TIMING] GetVolume check took %.3f ms`

### 2.11 DWM 检测
- `DwmIsCompositionEnabled`

### 2.12 注册表扫描
- `RegOpenKeyEx`, `RegQueryValueEx`, `RegEnumKeyEx`
- `RegNotifyChangeKeyValue` (监控注册表变更)

### 2.13 栈保护
- `protect_stack_`, `mprotect`
- NVIDIA driver check: `nvidia-smi`

---

## 第三层：Handle 追踪（关键）

### 3.1 Handle 操作验证
- "Handle Already Tracked" — 检测重复跟踪
- "Closing an untracked handle" — 检测未跟踪句柄
- "Closing a handle owned by something else" — 检测句柄所有权违规

### 3.2 API Hook 完整性
- "CloseHandleHook validation failure" — 检测 CloseHandle Hook 被篡改
- "DuplicateHandleHook validation failure" — 检测 DuplicateHandle Hook 被篡改

**含义**: 微信自己 Hook 了 CloseHandle/DuplicateHandle，并验证自己的 Hook 未被覆盖。如果我们的代码也 Hook 了这些函数，会被检测。

---

## 第四层：网络指纹与流量监控 (10+项)

### 4.1 DNS 查询 14 参数指纹
- 函数: `NewGetDns__BuildRequest` @ 0x182887490 (245BB)
- 参数: uin, clientversion, scene, net, md5, devicetype, lan, sigver, lasteffecttime, xagreementid, networkid, networkidctx, mccmnc, regctx

### 4.2 长连接认证链
- `MakesureAuthed` → `GetLonglinkIdentifyCheckBuffer` → `OnLonglinkIdentifyResponse`

### 4.3 网络质量自动检测
- `NetCheckLogic___ShouldNetCheck` — 成功率监控
- `BadNetDetectLogic` — 三层检测器 (longlink/shortlink/basenet)

### 4.4 网络嗅探
- `NetChecker::StartSniffIfNeed` — 按需启动数据包嗅探

### 4.5 网络监控
- `NetworkMonitor` — 连接变化实时监控

### 4.6 HTTP 主动探测
- `sub_186449000` (20KB, 100+BB) — HTTP Detector

### 4.7 短连接认证
- `ShortLink___AsyncCheckAuth` — 异步认证检查

### 4.8 MMTLS 事件监控
- `TLSShortLinkEventCallback::OnSendTo/OnReceive/OnFinishHandshake`
- `TLSLongLinkEventCallback::OnSendTo/OnReceive/OnFinishHandshake`
- 每条收发数据都被监控

### 4.9 NetcheckDispatcher 全诊断
- Ping/DNS/TCP/HTTP/Traceroute/ReqBuf 六种检测

### 4.10 MMTLS 控制信息
- `DispatchMmtlsCtrlInfo` — 服务端远程控制 MMTLS 参数
- `MMTLSCtrlInfo::ClearAllMMtlsPsk` — 清除所有 PSK 密钥

---

## 第五层：数据上报体系 (18+个 CGI)

### CGI 路由表 (内嵌签名 XML)

| CGI | 加密 | 用途 |
|-----|------|------|
| statreport | 无 | 统计上报 |
| clientperfreport | 无 | 客户端性能 |
| useractionreport | 无 | **用户行为上报** |
| reportstrategy | 无 | 策略上报 |
| kvreport | 无 | KV 上报 |
| kvreportrsa | RSA | KV 上报(加密) |
| rtkvreport | 无 | 实时 KV 上报 |
| newreportkvcomm | 无 | 新通用 KV 上报 |
| newreportkvcommrsa | RSA | 新通用 KV 上报(加密) |
| reportidkey | 无 | ID/Key 上报 |
| reportidkeyrsa | RSA | ID/Key 上报(加密) |
| newreportidkey | 无 | 新 ID/Key 上报 |
| newreportidkeyrsa | RSA | 新 ID/Key 上报(加密) |
| getkvidkeystrategy | 可选RSA | 获取上报策略 |
| gamereportkv | 无 | 游戏 KV 上报 |
| mmuploadmedia | 无 | 媒体上传 |
| heartbeat | 无 | 心跳 |
| newinit | 无 | 初始化 |

---

## 第六层：频率控制

### FreqLimit — LogId Ban/UnBan
- 函数: `FreqLimit___FreshCacheCount` @ 0x184946160 (108BB)
- `"!!!!!!LogId Ban!!!!!!"` — 封禁超限 LogId
- `"!!!!!!LogId UnBan!!!!!!"` — 解封低于阈值的 LogId
- 封禁后所有该类型上报被静默丢弃

---

## 第七层：会话/登录安全

### 7.1 登录异常检测
- `difflogintime` — 对比本次/上次登录时间
- `loginsid` — 会话唯一标识
- `isautologin`, `auto_login_switch`, `pc_login_type`

### 7.2 验证码
- `LOGIN_CAPTCHA_SUCCESS` / `LOGIN_CAPTCHA_CANCEL`
- 函数: `sub_183FFD710` (95BB)

### 7.3 设备锁
- "You logged out of WeChat in the locked status last time. Please confirm login on your phone."

### 7.4 登录状态机
- 函数: `LoginState__SerializeSession` @ 0x180623B30 (168BB)

### 7.5 Session 有效期
- `MMTLS2ClientSessionCache__LoadFileCache` @ 0x186473B10
- "LoadFileCache CheckSessionInfoValid fail"
- "Get expired sessionticket"

---

## 第八层：服务端远程控制

### 8.1 clicfg 远程配置
- `clicfg_xwechat_agent_check` — 代理检测开关
- `clicfg_xwechat_agent_protection_level` — 防护等级
- `clicfg_try_multiphase_check_hit` — 多阶段检测

### 8.2 CmdProcessor 远程命令
- `__SetLog` — 远程调日志
- `__SetDebugIP` — 远程设调试 IP
- `__UploadLog` — 远程上传客户端日志
- `__UploadCrash` — 远程上传崩溃

### 8.3 远程补丁
- `CourgettePatch` — Google 二进制差分补丁
- `DoPatchDownload` — 自动下载补丁

---

## 第九层：运行时监控

### 9.1 Watchdog
- 函数: `sub_184E3D370` (49BB)
- SRWLock + ConditionVariable 状态机

### 9.2 ANR 超时
- `anr_timeout:%lld < cost:%llu`
- `MessageQueue::RunLoop::Run` 中检测

### 9.3 协程完整性
- `"await() failed, unexpected resume detected"`
- `"Fatal error: "` — 致命错误级别
- `"%@() must be called from coroutine context"`

### 9.4 线程死亡检测
- `"the thread should be dead, status:%0"`
- `"the thread should be dead"`

### 9.5 线程回调验证
- `"not allow callback under current thread, check your logic."`

---

## 第十层：设备/环境监控

### 10.1 输入监控
- `SetWindowsHookExW` — 键盘/鼠标钩子
- `GetAsyncKeyState`, `GetKeyState`
- `WM_INPUT`, `WM_KEYDOWN`, `WM_KEYUP`, `WM_CHAR`

### 10.2 文件系统监控
- `QFileSystemWatcher` + `FindFirstChangeNotificationW`

### 10.3 剪贴板监控
- `AddClipboardFormatListener` + `QWindowsClipboard::clipboardViewerWndProc`

### 10.4 桌面捕获检测
- `DesktopCapturerFullscreenDetector`

### 10.5 电源状态
- `WM_POWER`, `WM_POWERBROADCAST`
- `RegisterSuspendResumeNotification`

### 10.6 GPU 黑名单
- `gpu_control::GPUBlackList::CheckGPU`

### 10.7 未成年人设备
- `GetClientCheckForMinorDevice`

### 10.8 USB 外设
- `SetUsbDeviceCallback`, `RoamServer`

### 10.9 摄像头/音频设备
- `SetCaptureDevice` + 计时检测
- `device_enumeration_win.cc`

---

## 威胁等级总览

| 威胁 | 等级 | 触发条件 |
|------|------|----------|
| 服务端远程开 agent_check | 🔴🔴🔴 | 服务端检测异常 |
| CmdProcessor 远程取证 | 🔴🔴🔴 | 服务端主动拉取 |
| Handle Hook 验证 | 🔴🔴 | 注入 Hook Windows API |
| PKCS7/完整性检查表 | 🔴🔴 | DLL 被修改 |
| 进程/模块枚举 | 🔴🔴 | 注入 DLL 无签名 |
| 频率封禁 Ban/UnBan | 🟠 | 超额上报 |
| useractionreport | 🟠 | 机械化操作 |
| 协程完整性检测 | 🟠 | 干预调度 |
| 网络质量多源交叉 | 🟡 | 网络异常 |
| 剪贴板/输入监控 | 🟡 | 自动化操作 |
| 摄像头/桌面捕获 | 🟢 | 屏幕操作 |
| 电源/文件系统 | 🟢 | 环境变化 |
