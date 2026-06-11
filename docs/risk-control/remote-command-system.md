# 服务端远程命令与控制系统

## 概述

WeChat 包含完整的服务端远程控制能力，通过 Mars MagicBox 的 CmdProcessor 和 clicfg 两个独立通道实现。

---

## 一、CmdProcessor — 服务端命令处理器

**源文件**: `mars-private/mars/magicbox/src/cmd_processor.cc`
**总入口**: `CmdProcessor::ExecuteCmd`
**XML 命令解析**: `CmdProcessor::OnIPxx` (0x1828E36C0, 7197字节, 273BB, 复杂度131)

### 1.1 命令列表

| 命令 | 函数 | 风险 |
|------|------|------|
| SetLog | `__SetLog` (0x188751b0d) | 🔴🔴 |
| SetDebugIP | `__SetDebugIP` (0x188751c5c) | 🔴🔴🔴 |
| CancelDebugIP | `__CancelDebugIP` (0x188751cdb) | 🔴 |
| UploadLog | `__UploadLog` (0x188751d53) | 🔴🔴🔴 |
| UploadCrash | `__UploadCrash` (0x188751e75) | 🔴🔴🔴 |
| UploadNetcheckErrFile | `__UploadNetcheckErrFile` (0x188751f8a) | 🔴🔴 |

### 1.2 __SetLog — 远程调整日志
**功能**: 服务端可远程调整客户端日志级别
- 可开启详细日志 (包括网络包内容)
- 可静默开启，用户无感知
- 用于远程取证

### 1.3 __SetDebugIP — 远程设置调试 IP
**功能**: 服务端可强制客户端连接到指定 IP 地址
- 绕过 DNS 解析
- 可将客户端流量重定向到蜜罐/监控服务器
- 用于检测是否使用了代理/VPN

### 1.4 __UploadLog — 远程上传日志
**功能**: 服务端可请求客户端上传完整日志文件
- 函数: `CmdProcessor::__UploadLog`
- 流程:
  1. `GetXlogFolderPaths` → 获取日志文件夹路径
  2. 遍历日志文件 (支持时间段过滤: `upload path:%_ period log:%_-%_`)
  3. `AddFilesToUploadElement` → 添加到上传队列
  4. `InitUploadElement` → 初始化上传元素
  5. 发送到服务器

**日志路径获取**:
- `appender_get_current_log_cache_path` — 当前日志缓存路径
- `appender_get_current_log_path` — 当前日志文件路径
- 如果路径为空: "appender_get_current_log_cache_path false"

### 1.5 __UploadCrash — 远程上传崩溃
**功能**: 服务端可请求上传客户端崩溃转储文件
- 包含调用栈、寄存器状态、内存快照
- 用于分析客户端异常 (包括被注入/被篡改的证据)

### 1.6 __UploadNetcheckErrFile — 上传网络检测错误
**函数**: `CmdProcessor::__BuildNetcheckErrFile` (0x188751f08)
- 构建网络检测错误文件: `/netcheck_%04d%02d%02d.err`
- 上传到服务器

### 1.7 XML 命令格式

从 `CmdProcessor::OnIPxx` 解析逻辑推断的 XML 结构:

```xml
<ipxx>
  <devicetype>设备类型</devicetype>
  <choice>
    <!-- 服务端可选的控制指令 -->
    <!-- SetLog/SetDebugIP/UploadLog/UploadCrash -->
  </choice>
</ipxx>
```

关键解析日志:
- "ipxx xml content:%_" — 原始 XML 内容
- "parse xml error,parseRet:%0" — 解析错误
- "Enter Set Choice..." — 进入指令选择
- "Enter UploadLOG Choice..." — 进入日志上传选择

---

## 二、clicfg — 远程配置系统

### 2.1 架构

```
服务端
  ↓ (推送配置更新)
AppManager::GetClientConfig
AppManager::GetClientConfigInteger
  ↓
ClientConfigManager
  ↓ (应用配置)
各模块读取
```

### 2.2 已知风控配置项

| 配置键 | 功能 | 当前状态 |
|--------|------|----------|
| `clicfg_xwechat_agent_check` | 代理/外挂检测总开关 | 服务端可控 |
| `clicfg_xwechat_agent_protection_level` | 防护等级(整数) | 服务端可控 |
| `clicfg_try_multiphase_check_hit` | 多阶段检测开关 | 服务端可控 |
| `clicfg_xwechat_sparkle_check_update_interval` | Sparkle 更新检测间隔 | 可调整 |
| `clicfg_xwechat_sparkle_update_alert_interval` | 更新提示间隔 | 可调整 |
| `clicfg_xwechat_open_large_than_1g_file_send` | 大文件发送 | 功能开关 |
| `clicfg_yuanbao_xweb_brand_switch` | 元宝 XWeb 品牌切换 | 品牌控制 |
| `clicfg_xwechat_agent_protection_level` | Agent 检测等级 | 0-? |

### 2.3 配置更新机制
- `mars::smc::SmcManager::OnRequestGetStrategy` — 主动请求最新策略
- `mars::smc::SmcManager::OnStrategyResp` — 策略响应处理
- `mars::smc::SmcManager::getStrategyVersions` — 策略版本管理
- 策略数据: `KVDATAFLOW(strategy_svr): channel:%_, general_version=%_, special_version=%_, uin_version=%_, maxValidDataTime=%_`

---

## 三、远程更新/补丁系统

### 3.1 Courgette 补丁
**组件**: CourgettePatch
- Google 开发的二进制差分补丁算法
- 比 bsdiff 更小 (特定场景)
- 支持热更新

### 3.2 补丁流程
```
DoPatchDownload
  → patch.zip 下载
  → patch_unzip 解压
  → patch.xml 读取补丁信息
  → CourgettePatch 应用补丁
```

### 3.3 Sparkle 更新框架
- `clicfg_xwechat_sparkle_check_update_interval` — 检测间隔
- `clicfg_xwechat_sparkle_update_alert_interval` — 提示间隔

---

## 四、MMTLS 远程控制

### 4.1 MMTLSCtrlInfo
**服务端推送**: `DispatchMmtlsCtrlInfo`
**客户端接收**: `MMCore::DispatchMmtlsCtrlInfo` (0x188749308)

### 4.2 服务端能力
| 操作 | 方法 |
|------|------|
| 清除所有 PSK | ClearAllMMtlsPsk (0x188ad304a) |
| 移除主机信息 | RemoveMMTlsHostInfo (0x188ad30a3) |
| 检查 MMTLS2 启用 | IsMMTLS2HostEnabled (0x188ad30e0) |

### 4.3 PSK 清除的影响
服务端执行 ClearAllMMtlsPsk 后:
1. 客户端下次连接时必须进行完整 TLS 握手
2. 无法使用 PSK 会话复用
3. 需要重新进行 X25519 密钥交换 + 证书验证
4. 增加连接延迟 (RTT x 2-3)

**风控意义**: 如果服务端怀疑某个客户端的 PSK 被窃取，可以远程清除所有 PSK，强制重新认证。

---

## 五、自动化系统的风险

### 5.1 CmdProcessor 远程取证威胁
- 服务端可以**随时、静默、无用户确认**地上传客户端的完整日志
- 日志包含: XLog 网络日志、应用日志、系统日志
- 如果自动化引入了异常行为，日志中会留下痕迹
- 服务端发现异常后可以第一时间取证

### 5.2 clicfg 动态升级威胁
- 风控规则可以**不升级客户端**就更新
- `agent_protection_level` 可以从 0 调到更高等级
- 新的检测规则可以通过策略系统下发

### 5.3 补丁系统威胁
- 微信可以通过 CourgettePatch 推送新的检测代码
- 补丁可以修改风控逻辑
- 可以修复被利用的漏洞

### 5.4 防御策略
1. 监控 `GetClientConfig` 的调用，提前发现策略变更
2. Hook `CmdProcessor::OnIPxx` 的 XML 解析，拦截敏感命令
3. 不要阻止正常日志写入，而是确保日志中不包含异常信息
4. 定期检查 clicfg 配置值的变化
