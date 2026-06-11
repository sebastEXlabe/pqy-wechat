# CGI 路由表与上报体系

## 一、CGI 路由配置

微信的网络请求路由由内嵌的签名 XML 配置控制。该配置包含 `<md5>` 和 `<signature>` 字段确保完整性。

### 1.1 长连接域名 (connecttype="long")

| IDC | 域名 |
|-----|------|
| 1 (华南) | long.weixin.qq.com, shlong.weixin.qq.com |
| 2 (香港) | hklong.weixin.qq.com |
| 3 (深圳) | szlong.weixin.qq.com |
| 4 (加拿大) | calong.weixin.qq.com |
| 5 (天津) | sh2tjlong.weixin.qq.com, sz2tjlong.weixin.qq.com, tjlong.weixin.qq.com |
| 8 (马来西亚) | mllong.weixin.qq.com |
| 9 (新加坡) | sglong.wechat.com |

### 1.2 短连接域名 (connecttype="short")

| IDC | 域名 |
|-----|------|
| 1 | short.weixin.qq.com, shshort.weixin.qq.com |
| 2 | hkshort.weixin.qq.com |
| 3 | szshort.weixin.qq.com |
| 4 | cashort.weixin.qq.com |
| 5 | sh2tjshort, sz2tjshort, tjshort |
| 8 | mlshort.weixin.qq.com |
| 9 | sgshort.wechat.com |

### 1.3 短连接扩展域名 (extshort)

| IDC | 域名 |
|-----|------|
| 1 | short.weixin.qq.com, extshort.weixin.qq.com |
| 2 | hkextshort.weixin.qq.com |
| 3 | szextshort.weixin.qq.com |
| 4 | caextshort.weixin.qq.com |
| 5 | sh2tjextshort, sz2tjextshort, tjextshort |
| 8 | mlshort.weixin.qq.com |
| 9 | sgshort.wechat.com |

### 1.4 统计上报域名 (minorshort)

| IDC | 域名 |
|-----|------|
| 1 | minorshort.weixin.qq.com |
| 2 | hkminorshort.weixin.qq.com |
| 3 | szminorshort.weixin.qq.com |
| 4 | caminorshort.weixin.qq.com |
| 5 | sh2tjminorshort, sz2tjminorshort |
| 8 | mlminorshort.weixin.qq.com |
| 9 | sgminorshort.wechat.com |

## 二、基础功能 CGI (prefix="/cgi-bin/micromsg-bin/")

### 2.1 认证类

| CGI | reqid | respid | nettype | netstrategy | 说明 |
|-----|-------|--------|---------|-------------|------|
| newauth | 178 | 1000000178 | 3 | 0 | 新认证 |
| manualauth | 253 | 1000000253 | 3 | 0 | 手动认证 |
| autoauth | 254 | 1000000254 | 3 | 0 | 自动认证 |
| newinit | 27 | 1000000027 | 3 | 0 | 初始化 |
| heartbeat | 238 | 1000000238 | 3 | 0 | 心跳 |
| logout | 133 | 1000000133 | 1 | 0 | 登出 |

### 2.2 消息类

| CGI | reqid | respid | nettype | 说明 |
|-----|-------|--------|---------|------|
| newsync | 121 | 1000000121 | 3 | 新同步 |
| newsendmsg | 237 | 1000000237 | 3 | 新发送消息 |
| sendmsg | 2 | 1000000002 | 3 | 发送消息 |
| sendappmsg | 107 | 1000000107 | 3 | 发送应用消息 |
| sendemoji | 68 | 1000000068 | 3 | 发送表情 |
| uploadvoice | 19 | 1000000019 | 3 | 上传语音 |
| revokemsg | 0 | 0 | 1 | 撤回消息 |

### 2.3 联系人/群组类

| CGI | reqid | respid | nettype | 说明 |
|-----|-------|--------|---------|------|
| getcontact | 71 | 1000000071 | 1 | 获取联系人 |
| getprofile | 118 | 1000000118 | 3 | 获取资料 |
| searchcontact | 34 | 1000000034 | 3 | 搜索联系人 |
| addchatroommember | 36 | 1000000036 | 3 | 添加群成员 |
| createchatroom | 37 | 1000000037 | 3 | 创建群聊 |
| delchatroommember | 0 | 0 | 1 | 删除群成员 |
| getchatroommemberdetail | 0 | 0 | 1 | 获取群成员详情 |

### 2.4 媒体类

| CGI | reqid | respid | nettype | 说明 |
|-----|-------|--------|---------|------|
| uploadmsgimg | 9 | 1000000009 | 1 | 上传消息图片 |
| getmsgimg | 10 | 1000000010 | 1 | 获取消息图片 |
| downloadvoice | 20 | 1000000020 | 1 | 下载语音 |
| downloadvideo | 40 | 1000000040 | 1 | 下载视频 |
| uploadvideo | 39 | 1000000039 | 1 | 上传视频 |
| downloadappattach | 106 | 1000000106 | 1 | 下载应用附件 |
| uploadappattach | 105 | 1000000105 | 1 | 上传应用附件 |
| receiveemoji | 69 | 1000000069 | 1 | 接收表情 |

### 2.5 CDN 类

| CGI | reqid | respid | nettype | 说明 |
|-----|-------|--------|---------|------|
| cdnuploadimgcommit | 0 | 0 | 1 | CDN 图片上传提交 |
| cdnuploadimgprepare | 0 | 0 | 1 | CDN 图片上传准备 |

### 2.6 支付类

| CGI | reqid | respid | nettype | netstrategy | 说明 |
|-----|-------|--------|---------|-------------|------|
| geta8key | 155 | 1000000155 | 3 | 1 | 获取 A8 Key |
| tenpay | 185 | 1000000185 | 3 | 1 | 财付通支付 |

## 三、统计上报 CGI (prefix="/cgi-bin/micromsg-bin/")

所有统计上报使用短连接 (minorshort 域名)，nettype=1:

| CGI | 说明 | 加密 |
|-----|------|------|
| statreport | 统计上报 | 无 |
| clientperfreport | 客户端性能上报 | 无 |
| useractionreport | **用户行为上报** | 无 |
| reportstrategy | 策略上报 | 无 |
| kvreport | KV 上报 | 无 |
| kvreportrsa | KV 上报 (reqid=218) | RSA |
| gamereportkv | 游戏 KV 上报 | 无 |
| reportkvcomm | KV 通用上报 | 无 |
| reportkvcommrsa | KV 通用上报 | RSA |
| rtkvreport | 实时 KV 上报 | 无 |
| newreportkvcomm | 新通用 KV 上报 | 无 |
| newreportkvcommrsa | 新通用 KV 上报 | RSA |
| reportidkey | ID/Key 上报 | 无 |
| reportidkeyrsa | ID/Key 上报 | RSA |
| newreportidkey | 新 ID/Key 上报 | 无 |
| newreportidkeyrsa | 新 ID/Key 上报 | RSA |
| getkvidkeystrategy | 获取上报策略 | 可选 RSA |
| getkvidkeystrategyrsa | 获取上报策略 | RSA |
| mmuploadmedia | 媒体上传 (reqid=111) | 无 |

### 3.1 useractionreport — 用户行为上报
这是对自动化系统**威胁最大**的上报通道。它上报用户的操作序列:
- 点击位置和顺序
- 滑动模式和速度
- 输入节奏和间隔
- 页面浏览时长
- 功能使用频率

服务端可通过机器学习建立正常用户行为基线，检测机械化的自动化操作。

### 3.2 ID/Key 上报架构

**SmcManager 接口** (独立的 SMC 团队维护):
| 方法 | 字符串 |
|------|--------|
| WriteKvData | mars::smc::SmcManager::WriteKvData |
| WriteImportKvData | mars::smc::SmcManager::WriteImportKvData |
| WriteListKVData | mars::smc::SmcManager::WriteListKVData |
| WriteIDKeyData | mars::smc::SmcManager::WriteIDKeyData |
| WriteListIDKeyData | mars::smc::SmcManager::WriteListIDKeyData |
| ReportGroupIDKey | mars::smc::SmcManager::ReportGroupIDKey |
| ReportIDKey | mars::stn::MMStnManager::ReportIDKey |
| ReportSpecialIDKey | mars::smc::DataManager::ReportSpecialIDKey |

**数据流**:
```
CGIProfile/LongLinkProfile/ShortLinkProfile
  → SmcCore::ReportIDKeyWithUin
  → SmcCore::ReportListIDKeyWithUin
  → DataManager::ReportSpecialIDKey
  → FreqLimit::__FreshCacheCount (Ban检查)
  → 网络发送 (可选 RSA 加密)
```

**ID/Key 数据来源**:
- `CGIProfile::__ReportCGIIDKeys` (0x188d3a8cf)
- `CGIProfile::__ReportCGITimeoutIDKeys` (0x188d3ab41)
- `LongLinkProfile::__ReportKVIDKeyData` (0x188d3b1dc)
- `LongLinkProfile::__ReportKVIDKeyDataForMMTLS` (0x188d3b213)
- `ShortLinkProfile::__ReportKVIDKeyData` (0x188d3b527)
- `ShortLinkProfile::__ReportIDKeyDataForMMTLS` (0x188d3b56f)
- `PayCgiTaskReporter::__ReportIDKey` (0x188ad6fd4)
- `PayNewDnsReporter::__ReportIDKey` (0x188adbc70)
- `ReportBuilder::ReportBandVideoKeys` (0x188d5020c)
- `ReportBuilder::ReportC2CErrorKeys` (0x188d500b8)
- `ReportBuilder::ReportSnsErrorKeys` (0x188d5014e)

**映射文件**: `profile_idkeys_mapping.cc` (0x188e3097c)

## 四、频率控制系统

### 4.1 FreqLimit — 双向封禁机制
**函数**: `FreqLimit___FreshCacheCount` (0x184946160)
**源码**: `mars-private/mars/smc/src/util/freq_limit.cc`

```
对每个 LogId 维护两个计数器:
  maxcount → 最大允许上报次数
  bancount → 连续超限次数

当超过 maxcount:
  → "!!!!!!LogId Ban!!!!!! uin_:%_, id:%_ maxcount:%_ bancount:%_"
  → 后续上报被静默丢弃

当降回阈值以下:
  → "!!!!!!LogId UnBan!!!!!! uin:%_, id:%_"
  → 恢复正常上报
```

**影响**: 如果自动化产生异常频率，某类型的 report ID 被 Ban 后，该类所有上报被阻断。服务端发现该类型的报告突然中断，可能触发反向甄别。

### 4.2 频率控制覆盖范围
- `mars::smc::SmcCore::UpdateFreqLimitConfig` — 从服务端更新频率限制配置
- `mars::cdn::CdnManager::reportKvNoFreqLimit` — CDN 层无频率限制上报(特殊通道)
- `hit freqlimit line. check your' logic` — 开发者调试日志

## 五、SelfMonitor — 元监控系统

### 5.1 系统架构
**源码**: `mars-private/mars/smc/src/selfmonitor/self_monitor.cc`

```
MonitorDataMemFile → MonitorDataFile → SelfMonitor
     (内存映射)        (文件持久化)      (上报管理器)
```

### 5.2 监控数据格式
```
kvselfmonitor data logid(output):%_, date:%_, action:%_, count:%_, uin:%_, version:%_
```

跟踪维度:
- logid — 被监控的上报 ID
- date — 日期
- action — 操作类型
- count — 计数
- uin — 用户 ID
- version — 客户端版本

### 5.3 关键方法
| 方法 | 功能 |
|------|------|
| `MonitorDataFile::AddData` (0x188a79a7b) | 添加监控数据点 |
| `MonitorDataFile::__TryReport` (0x188a799b5) | 尝试上报 |
| `MonitorDataFile::__ReportByKV` (0x188a79b47) | 通过 KV 系统上报 |
| `MonitorDataFile::__GetSelfMonitorOpLogAndReport` (0x188a79baf) | 获取自监控日志 |
| `SelfMonitor::ReportWithDate` (0x188a79cd4) | 按日期上报 |
| `SmcManager::OnSelfMonitorOpLogReady` (0x1883ad9c0) | 自监控回调 |
| `MonitorDataFile::Flush` (0x188a79a47) | 刷新到磁盘 |

**含义**: 如果自动化系统的 Hook 干预了正常的上报管道，SelfMonitor 会发现上报数据异常（某些类型数据缺失或数量异常）。

## 六、消息 Push 系统

| CGI | 说明 |
|-----|------|
| OnPush | 服务端主动推送消息 |
| stn::MMStnManager::DispatchMmtlsCtrlInfo | MMTLS 控制信息推送 |
| stn::DispatchHostInfo | 主机信息推送 |
| stn::DispatchExtHostInfo | 扩展主机信息推送 |
