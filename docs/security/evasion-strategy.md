# 风控规避策略

## 核心原则

### 绝对禁止（红线）

| 禁止操作 | 原因 | 检测机制 |
|----------|------|----------|
| 修改 Weixin.dll | CRC 校验 + PKCS7 签名 | IntegrityCheckTable__Verify |
| 注入 DLL | 进程/模块枚举 | CreateToolhelp32Snapshot |
| Hook CloseHandle | Handle 验证 | CloseHandleHook validation |
| Hook DuplicateHandle | Handle 验证 | DuplicateHandleHook validation |
| Inline Hook .text 段 | 完整性校验 | IntegrityCheckTable |

### 必须遵守

| 要求 | 原因 | 实现方式 |
|------|------|----------|
| 单实例运行 | 多开检测 | 检查进程互斥量 |
| 从 explorer.exe 启动 | 父进程检测 | 不从 IDE/终端启动 |
| 正常桌面环境 | DWM 检测 | 不禁用桌面合成 |
| 真实网络栈 | 网络检测 | 不禁用 IPv6 |
| 不挂起线程 | 线程状态检测 | 不使用 SuspendThread |
| 不修改系统时间 | 时序检测 | 使用相对时间 |

---

## 威胁等级与应对

### 🔴🔴🔴 最高威胁

#### 1. 服务端远程开启 agent_check

**威胁**：`clicfg_xwechat_agent_check` 可由服务端随时远程开启，无需客户端升级。

**应对策略**：
```python
# 监控 clicfg 配置变更
async def monitor_clicfg_changes():
    """监控 clicfg 配置变更"""
    while True:
        # 读取当前配置
        config = read_clicfg("xwechat_agent_check")
        
        # 检查是否开启
        if config == 1:
            logger.warning("agent_check enabled by server!")
            # 降低操作频率
            await reduce_activity()
        
        await asyncio.sleep(60)  # 每分钟检查一次
```

#### 2. CmdProcessor 远程取证

**威胁**：服务端可随时、静默、无用户确认地上传完整日志和崩溃转储。

**应对策略**：
```python
# 确保日志中不包含异常信息
async def clean_logs():
    """清理可能暴露的日志"""
    # 删除包含敏感关键词的日志
    sensitive_keywords = ["hook", "inject", "automation", "frida"]
    
    for log_file in get_log_files():
        content = read_file(log_file)
        for keyword in sensitive_keywords:
            if keyword in content:
                remove_file(log_file)
                break
```

#### 3. Handle Hook 验证

**威胁**：微信自行 Hook 了 CloseHandle/DuplicateHandle，并验证 Hook 未被覆盖。

**应对策略**：
```python
# 绝对不 Hook 这些函数
FORBIDDEN_HOOKS = [
    "CloseHandle",
    "DuplicateHandle",
    "NtClose",
]

# 使用 UIAutomation 而非 Hook
# UIAutomation 是 Windows 官方 API，不会触发检测
```

---

### 🔴🔴 高威胁

#### 4. PKCS7 签名 + IntegrityCheckTable

**威胁**：三层文件完整性验证。

**应对策略**：
```python
# 不修改任何文件
# 使用内存读取（只读）获取信息
async def read_memory_only(address: int, size: int) -> bytes:
    """只读内存，不修改"""
    import ctypes
    
    buffer = ctypes.create_string_buffer(size)
    bytes_read = ctypes.c_size_t()
    
    ctypes.windll.kernel32.ReadProcessMemory(
        process_handle,
        address,
        buffer,
        size,
        ctypes.byref(bytes_read)
    )
    
    return buffer.raw[:bytes_read.value]
```

#### 5. 进程/模块枚举

**威胁**：CreateToolhelp32Snapshot 遍历所有进程和加载模块。

**应对策略**：
```python
# 不注入 DLL
# 使用外部进程通信（ZeroMQ）
# 使用 UIAutomation（不注入）

# 如果必须使用 Frida，使用 Gadget 模式
# 而不是 frida-inject
```

#### 6. TLS 事件回调监控

**威胁**：OnSendTo/OnReceive 监控每个发送/接收字节。

**应对策略**：
```python
# 复用微信原生 TLS 会话
# 不自建 MMTLS 连接
# 使用微信原生 API 发送消息
```

---

### 🟠 中威胁

#### 7. FreqLimit Ban/UnBan

**威胁**：超过 maxcount 则 LogId 被静默封禁。

**应对策略**：
```python
# 频率限制配置（保持在阈值 50% 以下）
FREQ_LIMITS = {
    "send_message": {
        "max_per_minute": 5,      # 实际限制 10
        "max_per_hour": 50,       # 实际限制 100
        "cooldown_ms": 3000,
    },
    "post_moments": {
        "max_per_day": 2,         # 实际限制 5
        "cooldown_ms": 300000,    # 5 分钟
    },
    "like_moments": {
        "max_per_minute": 2,      # 实际限制 5
        "cooldown_ms": 10000,
    },
}

# 动态调整频率
async def adjust_frequency_based_on_risk():
    """根据风控等级动态调整频率"""
    if risk_detector.current_level == RiskLevel.HIGH:
        # 降低到 25%
        for action in FREQ_LIMITS:
            FREQ_LIMITS[action]["max_per_minute"] //= 2
    elif risk_detector.current_level == RiskLevel.MEDIUM:
        # 降低到 50%
        for action in FREQ_LIMITS:
            FREQ_LIMITS[action]["max_per_minute"] = int(
                FREQ_LIMITS[action]["max_per_minute"] * 0.5
            )
```

#### 8. useractionreport 用户行为上报

**威胁**：上报点击位置/顺序、滑动模式/速度、输入节奏/间隔。

**应对策略**：
```python
# 类人行为模拟
class HumanSimulator:
    def __init__(self):
        # 鼠标轨迹：贝塞尔曲线
        self.mouse_trajectory = "bezier"
        
        # 速度变化：Gamma 分布
        self.speed_distribution = "gamma"
        self.speed_shape = 2.0
        self.speed_scale = 0.3
        
        # 停顿概率
        self.pause_probability = 0.1
        
        # 打字错误概率
        self.typo_probability = 0.02
        
        # 随机多余动作
        self.extra_action_probability = 0.05
    
    async def random_extra_action(self):
        """随机多余动作"""
        if random.random() < self.extra_action_probability:
            # 随机滚动
            if random.random() < 0.5:
                pyautogui.scroll(random.randint(-3, 3))
            # 随机移动鼠标
            else:
                x = random.randint(100, 1800)
                y = random.randint(100, 900)
                await self.human_move(x, y)
```

#### 9. 协程完整性检测

**威胁**：非协程上下文调用协程 API 会触发致命错误。

**应对策略**：
```python
# 不干预协程调度
# 使用标准 asyncio
# 不使用 greenlet 或其他协程库
```

#### 10. 多开检测

**威胁**：检测多个微信实例。

**应对策略**：
```python
# 确保只运行一个微信实例
async def ensure_single_instance():
    """确保单实例运行"""
    import psutil
    
    wechat_count = 0
    for proc in psutil.process_iter(['name']):
        if proc.info['name'] == 'WeChat.exe':
            wechat_count += 1
    
    if wechat_count > 1:
        raise Exception("Multiple WeChat instances detected")
    
    if wechat_count == 0:
        raise Exception("WeChat not running")
```

---

## 时序约束

### 关键检测点

| 检测点 | 最大延迟 | 说明 |
|--------|----------|------|
| GetVolume 卷检查 | <100ms | 有计时检测 |
| SetCaptureDevice | <200ms | 有计时检测 |
| Hook 回调执行 | <1ms | 避免触发时序检测 |
| 心跳默认间隔 | 210,000ms (3.5分) | SmartHeartbeat 服务端可调 |
| Socket BAN 过期 | 300,000ms (5分) | 被封后等待恢复 |
| 首包超时硬上限 | 3,600,000ms (60分) | 8个参数分别配置 |

### 实现

```python
# 时序控制
class TimingController:
    """时序控制器"""
    
    def __init__(self):
        self.min_delay = 0.1  # 最小延迟
        self.max_delay = 2.0  # 最大延迟
    
    async def safe_delay(self, min_ms: int = 100, max_ms: int = 500):
        """安全延迟（避免时序检测）"""
        delay = random.uniform(min_ms / 1000, max_ms / 1000)
        await asyncio.sleep(delay)
    
    async def human_delay(self):
        """人类延迟（Gamma 分布）"""
        delay = np.random.gamma(shape=2.0, scale=0.3)
        await asyncio.sleep(delay)

timing = TimingController()
```

---

## 检测与恢复

### 风控信号检测

```python
class RiskSignalDetector:
    """风控信号检测器"""
    
    def __init__(self):
        self.signals = []
    
    async def check_all(self) -> List[str]:
        """检查所有风控信号"""
        detected = []
        
        # 检查弹窗
        if await self._check_popup():
            detected.append("popup_detected")
        
        # 检查功能限制
        if await self._check_function_limit():
            detected.append("function_limited")
        
        # 检查网络异常
        if await self._check_network_anomaly():
            detected.append("network_anomaly")
        
        # 检查账号状态
        if await self._check_account_status():
            detected.append("account_risk")
        
        return detected
    
    async def _check_popup(self) -> bool:
        """检查风控弹窗"""
        import uiautomation as auto
        
        risk_keywords = [
            "异常", "风险", "限制", "封禁", "违规",
            "安全", "验证", "身份", "登录"
        ]
        
        for keyword in risk_keywords:
            popup = auto.WindowControl(Name=keyword)
            if popup.Exists(maxSearchSeconds=0.1):
                return True
        
        return False
```

### 恢复策略

```python
class RecoveryManager:
    """恢复管理器"""
    
    async def handle_risk(self, risk_type: str):
        """处理风控"""
        if risk_type == "popup_detected":
            await self._handle_popup()
        elif risk_type == "function_limited":
            await self._handle_function_limit()
        elif risk_type == "network_anomaly":
            await self._handle_network_anomaly()
        elif risk_type == "account_risk":
            await self._handle_account_risk()
    
    async def _handle_popup(self):
        """处理弹窗"""
        # 等待一段时间
        await asyncio.sleep(60)
        
        # 关闭弹窗
        import uiautomation as auto
        popup = auto.WindowControl(Name="确定")
        if popup.Exists():
            popup.Click()
    
    async def _handle_function_limit(self):
        """处理功能限制"""
        # 降低操作频率
        risk_detector.current_level = RiskLevel.HIGH
        
        # 等待恢复
        await asyncio.sleep(300)  # 5 分钟
    
    async def _handle_account_risk(self):
        """处理账号风险"""
        # 停止所有操作
        risk_detector.current_level = RiskLevel.CRITICAL
        
        # 通知用户
        notifier.send_alert("账号存在风险，已停止所有操作")
```

---

## 最佳实践

### 1. 操作前检查

```python
async def safe_execute(action: str, func: Callable, *args, **kwargs):
    """安全执行操作"""
    # 检查风控等级
    if risk_detector.current_level == RiskLevel.CRITICAL:
        raise Exception("Risk level critical, operation blocked")
    
    # 检查频率限制
    if not risk_detector.check_frequency(action):
        raise Exception(f"Rate limited for {action}")
    
    # 检查风控信号
    signals = await risk_detector.check_all()
    if signals:
        logger.warning(f"Risk signals detected: {signals}")
        await recovery.handle_risk(signals[0])
        raise Exception("Risk detected, operation postponed")
    
    # 执行操作
    return await func(*args, **kwargs)
```

### 2. 日志安全

```python
# 不记录敏感信息
SAFE_LOG_TEMPLATE = """
Action: {action}
Time: {timestamp}
Status: {status}
"""

def safe_log(action: str, status: str):
    """安全日志"""
    logger.info(SAFE_LOG_TEMPLATE.format(
        action=action,
        timestamp=datetime.now().isoformat(),
        status=status
    ))
    # 不记录具体参数、坐标、内容等
```

### 3. 配置安全

```yaml
# config/safe_settings.yaml
security:
  # 隐藏模式
  hide_mode: true
  
  # 随机化
  randomize_delays: true
  randomize_actions: true
  
  # 限制
  max_session_duration: 3600  # 1 小时
  max_actions_per_session: 100
  
  # 恢复
  auto_recovery: true
  recovery_delay: 300  # 5 分钟
```

---

## 总结

| 威胁等级 | 数量 | 应对策略 |
|----------|------|----------|
| 🔴🔴🔴 | 3 | 绝对禁止相关操作 |
| 🔴🔴 | 3 | 使用安全替代方案 |
| 🟠 | 4 | 限制频率 + 类人行为 |
| 🟡 | 多项 | 监控 + 自动恢复 |

**核心思路**：
1. 不修改、不注入、不 Hook 关键函数
2. 使用 Windows 官方 API（UIAutomation）
3. 复用微信原生协议和通道
4. 类人行为模拟
5. 频率限制在阈值 50% 以下
6. 实时监控 + 自动恢复
