# 离线模拟验证方案 — 零风险绕过所有已知风控点

基于 WeChat 4.1.10.31 完整逆向分析（36 次 IDB 保存，90+ 风控点，95+ 精确阈值）。

## 核心原则

1. **不修改 Weixin.dll 本体** — 规避 PKCS7 签名 + IntegrityCheckTable + MMKV CRC
2. **只读 Hook 优先** — 先只监听数据流，不修改任何行为
3. **虚拟化隔离** — 无真实微信账号，无网络连接（或全流量代理）
4. **每次只验证一类参数** — 最小化风险面

---

## Phase 0: 环境准备

### 0.1 虚拟机配置

```
CPU: 4+ cores (与真实物理机一致，避免 GetLogicalProcessorInformation 检测)
RAM: 8+ GB
GPU: 启用 3D 加速（避免 DWM 检测 → DwmIsCompositionEnabled 必须返回 TRUE）
系统: Windows 10/11 x64 (与物理机版本一致)
磁盘: 固定大小 VHD（避免 GetVolumeInformationW 返回虚拟机特征）
网络: NAT 或 Bridged（必须与物理机相同的网络类型 → WiFi 对 WiFi）
```

### 0.2 微信安装

```powershell
# 安装路径必须与真实环境一致
C:\Program Files\Tencent\Weixin\4.1.10.31\

# 确保以下文件存在（完整性检查表遍历目标）：
Weixin.dll
mmojo_64.dll
VoipEngine.dll
所有 .dll 文件必须保持原始数字签名
```

### 0.3 风控规避清单

| # | 规避项 | 措施 | 风险点引用 |
|---|--------|------|-----------|
| 1 | 不修改 DLL | 原始安装，不 patch 任何字节 | PKCS7 + CRC + IntegrityCheckTable |
| 2 | 不开启调试器 | 使用 ETW 或内存快照替代实时调试 | IsDebuggerPresent + NtQueryInfo |
| 3 | 单实例运行 | 只启动一个微信进程 | allow_multi_open + check_four_screen |
| 4 | 正常桌面环境 | 不禁用 DWM，正常显示器配置 | DwmIsCompositionEnabled |
| 5 | 真实网络栈 | 不禁用 IPv6，不修改适配器 | Fake IPv6 检测 (0x3A3A) |
| 6 | 标准父进程 | 从 explorer.exe 或开始菜单启动 | explorer.exe/ncpa.cpl 检测 |
| 7 | 不挂起线程 | 不用 SuspendThread | Handle 追踪系统 |
| 8 | 不修改系统时间 | 保持时间同步 | RtlGetVersion + GetTickCount 一致性 |
| 9 | 不 Hook Windows API | 避免 CloseHandle/DuplicateHandle | Handle Hook 验证 |
| 10 | 标准蓝牙/WiFi | 虚拟蓝牙和 WiFi 适配器 | WMI Win32_PnPEntity 枚举 |

---

## Phase 1: 静态参数验证（零风险，无代码注入）

### 1.1 直接读取配置值

从文件系统和注册表提取运行时参数：

```powershell
# 1. MMKV 存储值（加密存储，需先确认加密密钥）
# 路径通常在: %APPDATA%\Tencent\WeChat\mmkv\
# 每个 .mmkv 文件由 SQLCipher 加密

# 2. 注册表读取（无需注入）
reg query "HKCU\Software\Tencent\WeChat" /s
reg query "HKLM\SOFTWARE\Tencent\WeChat" /s

# 3. INI 配置文件
dir "%APPDATA%\Tencent\WeChat\*.ini"
dir "%APPDATA%\Tencent\WeChat\config\*.ini"

# 4. 日志文件（只读）
dir "%APPDATA%\Tencent\WeChat\log\*.xlog"
```

### 1.2 内存快照分析

在不注入代码的情况下提取参数：

```powershell
# 使用 Windows 内置工具创建完整内存转储
# 在微信运行时执行（需要管理员权限）：
procdump -ma WeChat.exe wechat_memory.dmp

# 然后用 IDA/Volatility 分析转储文件中已知偏移量的值
# 基于我们的签名映射表 (golden-reference.md)
```

### 1.3 网络流量分析（离线捕获）

```powershell
# 使用 netsh trace 而不 Hook 微信进程
netsh trace start capture=yes tracefile=c:\traces\wechat.etl
# 启动微信，等待 30 秒
netsh trace stop

# 或使用 pktmon（Windows 10+ 内置）
pktmon start --etw -p 0
# ... 抓包 ...
pktmon stop
pktmon etl2pcap wechat.etl --out wechat.pcapng
```

### 1.4 预期可从静态分析获取的参数

| 参数 | 提取方式 | 难度 |
|------|----------|------|
| clicfg 值 | 内存转储 → 搜索字符串 "clicfg_" | 低 |
| MMKV keys | SQLCipher 解密 → 解析键值对 | 中 |
| SmartHeartbeat 间隔 | 日志文件 `_YYYYMMDD.xlog` 关键词过滤 | 低 |
| DNS 14 参数 | 抓包 → 解析 UDP 53 / TCP 443 | 低 |
| PSK 票据内容 | 内存转储 → 搜索 ".psk.key" | 中 |
| 设备指纹值 | 注册表 + MMKV 读取 | 低 |

---

## Phase 2: 只读 Hook — 网络层（极低风险）

### 2.1 Hook 点选择

只 Hook **StnCallbackBridge 虚表中的 Req2Buf 和 Buf2Resp**，只读不写：

| Hook 点 | 地址 | 偏移 | 数据方向 | 风险 |
|---------|------|------|----------|------|
| `StnManager__Req2Buf` | 0x180D4F950 | vtable+0x60 | 请求序列化 | 🟢 只读 |
| `StnManager__Buf2Resp` | 0x180D4FB40 | vtable+0x68 | 响应反序列化 | 🟢 只读 |
| `StnManager__OnTaskEnd` | 0x180D4FD40 | vtable+0x70 | 任务结束 | 🟢 只读 |

### 2.2 Hook 实现（EAT Hook，非 inline hook）

```cpp
// 使用 EAT (Export Address Table) Hook 而非 inline hook
// 避免触发 IntegrityCheckTable（仅检查 .text 段）
// 避免触发 Handle 验证

typedef void* (*Req2Buf_t)(void* bridge, void* task, void* buf, void* len);
Req2Buf_t original_Req2Buf = nullptr;

void* Hook_Req2Buf(void* bridge, void* task, void* buf, void* len) {
    // 🔒 只读：记录请求数据，不修改
    uint32_t buflen = *(uint32_t*)len;
    hexdump("REQ", buf, buflen > 256 ? 256 : buflen);
    
    // 🔒 直通：不修改返回值
    return original_Req2Buf(bridge, task, buf, len);
}
```

### 2.3 VTable Hook 方式（更隐蔽）

```cpp
// 直接替换 StnCallbackBridge 虚表中的函数指针
// 优势：不修改 .text 段，不触发代码完整性检查
// 风险：WeChat 自有的热补丁系统可能检测到 VTable 修改

struct StnCallbackBridge_VTable {
    void* SetCallback;                    // +0x08
    void* MakesureAuthed;                 // +0x10
    void* TrafficData;                    // +0x18
    // ... 共 19 个虚函数
};

// 找到 StnManager 对象 → 读取 callback_bridge_ (offset +0x1B10)
// 读取虚表指针 → 替换 Req2Buf (vtable[7]) 和 Buf2Resp (vtable[8])
```

### 2.4 风险规避检查表

| 检查点 | 规避措施 |
|--------|----------|
| `SRWLock` 保护 | Hook 的执行时间 < 1μs，不持有锁跨操作 |
| 时序检测 | 不增加超过 1ms 的延迟 |
| 线程回调限制 | 在原始调用线程执行 Hook（不跨线程） |
| `Anr_timeout` 检测 | Hook 逻辑不阻塞 |

---

## Phase 3: 被动监听 — 全参数提取（低风险）

### 3.1 扩展只读 Hook 点

在 Phase 2 基础上增加以下只读监测点：

```
读取配置值（只读，不修改）：
  AppConfig::UpdateConfig      @ 0x182949FC0       — 拦截 clicfg 更新
  SmcManager::SetDebugFlag     @ 0x188309d15       — 拦截 debug flag 变化
  CmdProcessor::OnIPxx         @ 0x1828E36C0       — 监控服务端命令（只读日志）

读取运行时状态（只读，不修改）：  
  FreqLimit::Check             — 收集当前 ban 状态
  SmartHeartbeat::GetNextHeartbeatInterval — 收集心跳间隔
  MMTLS2ClientSessionCache::LoadFileCache — 收集 PSK 有效期
  NetCheckTrafficMonitor::sendLimitCheck — 收集流量配额当前值

读取指纹数据（只读，不修改）：
  NewDns__BuildRequest         @ 0x182887490       — 收集 DNS 14 参数
  GetAdaptersAddressesWrapper  @ 0x184762A20       — 收集适配器信息
  WMI 蓝牙枚举                  @ 0x180077AA0       — 收集蓝牙 MAC
```

### 3.2 数据收集格式

```json
{
  "session_id": "uuid",
  "timestamp": 1234567890,
  "captured_at": "Req2Buf",
  "task_type": "shortlink",
  "data": {
    "cgi": "/cgi-bin/micromsg-bin/newsync",
    "is_mmtls": true,
    "psk_reused": true,
    "req_size": 2048,
    "fingerprint": {
      "uin": 123456789,
      "clientversion": "4.1.10.31",
      "devicetype": "...",
      "networkid": "...",
      "mccmnc": "46000"
    }
  }
}
```

---

## Phase 4: 主动参数验证（中高风险 — 需要确认后执行）

### 4.1 仅在确认安全后执行的操作

| 验证目标 | 方法 | 风险 |
|----------|------|------|
| clicfg xwechat_agent_check 当前值 | Hook `sub_180ED76F0` 只读 | 🟢 |
| 最大重试次数实际值 | Hook `sub_18294E690` 只读 | 🟢 |
| 定时器精度验证 | 日志分析 + 内存时间戳对比 | 🟢 |
| 心跳间隔动态变化 | 长期监控 SmartHeartbeat | 🟢 |
| FreqLimit ban 阈值触发 | 模拟上报（需真实账号⚠️） | 🔴 |

### 4.2 严禁操作（必然触发风控）

- ❌ 在任何生产微信账号上测试
- ❌ 在联网状态下修改微信行为
- ❌ 使用 inline hook 修改 .text 段
- ❌ 反复启动/停止微信（触发多开检测）
- ❌ 修改 MMKV 存储的数据
- ❌ Hook CloseHandle/DuplicateHandle（与微信自带 Hook 冲突）

---

## Phase 5: 验证结果记录模板

每个验证项记录：

```markdown
### [参数名]
- **预期值**: [从逆向分析提取的值]
- **实际值**: [运行时采集的值]
- **采样时间**: [时间戳]
- **采样方式**: [内存转储/网络抓包/Hook日志]
- **一致性**: [✅/⚠️/❌]
- **备注**: [差异说明]
```

---

## 执行优先级

| 优先级 | Phase | 风险 | 时间 | 价值 |
|--------|-------|------|------|------|
| P0 | Phase 0 (环境) | 无 | 2-4h | 基础设施 |
| P0 | Phase 1 (静态) | 无 | 4-8h | 配置值+流量指纹 |
| P1 | Phase 2 (只读网络) | 🟢 极低 | 8-16h | 请求/响应明文 |
| P2 | Phase 3 (全参数) | 🟢 低 | 16-32h | 所有运行时状态 |
| P3 | Phase 4 (主动验证) | 🟠 中高 | 不确定 | 阈值动态行为 |

---

## 下一步行动

1. 准备 Windows 10/11 VM 环境（满足 Phase 0 所有配置要求）
2. 安装 WeChat 4.1.10.31 并生成初始 MMKV 数据
3. 执行 Phase 1 静态分析：内存转储 + 网络抓包
4. 基于 Phase 1 结果，确定是否进入 Phase 2
