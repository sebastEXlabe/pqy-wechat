# Phase 2 — 实机只读分析（零风险）

## 核心原则
- ❌ 不注入任何代码到 WeChat 进程
- ❌ 不加载任何 DLL
- ❌ 不 attach 调试器
- ❌ 不修改微信注册表/文件/内存
- ✅ 只用操作系统级工具
- ✅ 只抓包、转储、读日志

---

## Step 1: 网络抓包（最安全，信息量最大）

### 1.1 pktmon（Windows 10/11 内置）

```cmd
REM 管理员权限运行 cmd
REM 开始抓包
pktmon start --etw -p 0

REM 启动微信，正常使用 5-10 分钟

REM 停止抓包
pktmon stop

REM 转换为 Wireshark 可读格式
pktmon etl2pcap PktMon.etl --out wechat_live.pcapng

REM 清理
del PktMon.etl
```

### 1.2 从抓包中提取的 DNS 指纹

用 Wireshark 打开 `wechat_live.pcapng`，过滤：
```
dns.qry.name contains "newgetdns"
```

期望看到 14 参数 DNS 查询：
```
/cgi-bin/micromsg-bin/newgetdns?uin=%u&clientversion=%u&scene=%d&net=%d&md5=%s&devicetype=%s&lan=%s&sigver=%d&lasteffecttime=%s&xagreementid=%u&networkid=%s&networkidctx=%s&mccmnc=%s&regctx=%u
```

### 1.3 从抓包中验证的其他项目

| 过滤条件 | 验证目标 |
|----------|----------|
| `dns.qry.name contains "long.weixin"` | 长连接域名选择 |
| `dns.qry.name contains "short.weixin"` | 短连接域名选择 |
| `tls.handshake.type == 1` | MMTLS ClientHello，验证cipher suites |
| `tcp.port == 443 and tls` | 验证 TLS 1.3 + Ed25519 证书 |

---

## Step 2: 注册表读取

### 2.1 只读导出微信注册表

```cmd
REM 导出所有微信注册表项（只读操作）
reg export "HKCU\Software\Tencent\WeChat" wechat_hkcu.reg
reg export "HKLM\SOFTWARE\Tencent\WeChat" wechat_hklm.reg
reg export "HKLM\SOFTWARE\WOW6432Node\Tencent\WeChat" wechat_wow64.reg
```

### 2.2 期望找到的键

- 安装路径: `wechat_install_path`
- 设备标识: `key_wechat_device_id`, `MachineId`
- 更新配置: `sparkle_check_update_interval`
- 语音/设备: `voip_device_cfg`, `voice_input_win`

---

## Step 3: 日志文件只读分析

### 3.1 定位日志目录

```cmd
dir /s "%APPDATA%\Tencent\WeChat\*.xlog"
dir /s "%APPDATA%\Tencent\WeChat\log\*"
```

### 3.2 关键日志搜索词

```cmd
REM 搜索风控关键词（无需解密日志，直接在文件中搜索明文）
findstr /i /s "freqlimit" *.xlog
findstr /i /s "maxcount" *.xlog
findstr /i /s "LogId Ban" *.xlog
findstr /i /s "heart interval" *.xlog
findstr /i /s "clicfg" *.xlog
findstr /i /s "agent_check" *.xlog
findstr /i /s "isban" *.xlog
```

---

## Step 4: 进程快照（只读）

### 4.1 进程信息

```cmd
REM 查看微信进程信息（只读）
tasklist /fi "imagename eq WeChat.exe" /v
wmic process where "name='WeChat.exe'" get ProcessId,ThreadCount,HandleCount,WorkingSetSize
```

### 4.2 加载模块清单

```cmd
REM 列出微信加载的所有 DLL（验证是否有注入检测）
tasklist /fi "imagename eq WeChat.exe" /m
```

---

## Step 5: 内存转储（高风险，仅做一次）

### 5.1 使用 procdump（Sysinternals，微软官方工具）

```cmd
REM 下载 procdump
REM https://learn.microsoft.com/en-us/sysinternals/downloads/procdump

REM 创建完整内存转储（管理员运行）
procdump -ma WeChat.exe wechat_full.dmp
```

⚠️ **风险注意**：
- `procdump` 使用 `CreateToolhelp32Snapshot` — 但这不是调试器，风险远低于 x64dbg/OllyDbg
- 只做一次，不要在微信运行时反复执行
- 转储完成后立即关闭 procdump

---

## Step 6: 数据提取工作表

### 使用表格记录所有发现

| 日期 | 提取项 | 方法 | 预期值(逆向) | 实际值 | 一致? |
|------|--------|------|-------------|--------|-------|
| | DNS uin | 抓包 | uint32 | | |
| | DNS clientversion | 抓包 | 4.1.10.31对应值 | | |
| | DNS devicetype | 抓包 | string | | |
| | DNS networkid | 抓包 | MAC-based | | |
| | DNS mccmnc | 抓包 | 46000/46001 | | |
| | MMTLS cipher | 抓包 | AES_256_GCM | | |
| | Heartbeat interval | 日志 | 210000ms | | |
| | FreqLimit maxcount | 日志 | 100 | | |
| | agent_check | 注册表 | 0 | | |
| | ThreadPoolMaxSize | 日志 | 50 | | |
| | ThreadPoolKeepAlive | 日志 | 300000ms | | |
| | PSK valid_seconds | 内存 | 服务端下发 | | |

---

## 执行顺序

```
Step 1 (网络抓包) → 启动微信 → 正常使用 5-10 分钟 → 停止抓包
    ↓
Step 2 (注册表) → 立即执行，2 分钟
    ↓
Step 3 (日志搜索) → 立即执行，5 分钟
    ↓
Step 4 (进程快照) → 立即执行，1 分钟  
    ↓
Step 5 (内存转储) → 最后执行（仅一次）→ 关闭微信
    ↓
Step 6 (数据整理) → 离线进行
```

---

## 不要做的事（直接触发风控）

1. ❌ 使用 x64dbg/OllyDbg/Windbg 附加微信进程
2. ❌ 使用任何 DLL 注入工具
3. ❌ 修改微信安装目录的任何文件
4. ❌ 使用 Cheat Engine 等内存修改器
5. ❌ 在微信运行时修改注册表
6. ❌ 反复启动/关闭微信（短时间多次）
7. ❌ 使用代理/VPN 截获微信流量（触发 VPN 检测）
8. ❌ 安装/卸载微信在短时间反复操作
