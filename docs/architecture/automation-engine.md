# 自动化引擎架构

## 技术栈

| 技术 | 版本 | 用途 |
|------|------|------|
| Python | 3.12 | 运行时 |
| FastAPI | Latest | 异步框架 |
| ZeroMQ | Latest | 进程间通信 |
| PaddleOCR | Latest | OCR 引擎 |
| OpenCV | Latest | 图像处理 |
| uiautomation | Latest | Windows UI 自动化 |
| comtypes | Latest | COM 接口 |

---

## 项目结构

```
engine/
├── src/
│   ├── main.py                  # 入口
│   │
│   ├── core/                    # 事件驱动核心
│   │   ├── __init__.py
│   │   ├── events.py            # 事件定义
│   │   ├── bus.py               # 事件总线
│   │   ├── publisher.py         # 事件发布者
│   │   └── watchers.py          # 事件监听者
│   │
│   ├── locator/                 # 版本无关定位
│   │   ├── __init__.py
│   │   ├── ocr_locator.py       # OCR 定位
│   │   ├── feature_locator.py   # 特征匹配定位
│   │   ├── anchor_locator.py    # 锚点定位
│   │   ├── fast_ocr.py          # 快速 OCR
│   │   ├── template_extractor.py # 模板提取
│   │   └── router.py            # 路由器
│   │
│   ├── executor/                # 执行层
│   │   ├── __init__.py
│   │   ├── operator.py          # 操作器
│   │   ├── human_sim.py         # 类人行为模拟
│   │   ├── state_machine.py     # 状态机
│   │   ├── uia_bridge.py        # UIAutomation 桥接
│   │   ├── file_dialog.py       # 文件对话框
│   │   └── wechat_discovery.py  # 微信窗口发现
│   │
│   ├── monitor/                 # 监控层
│   │   ├── __init__.py
│   │   ├── risk_detector.py     # 风控检测
│   │   ├── popup_handler.py     # 弹窗处理
│   │   └── notifier.py          # 通知器
│   │
│   ├── recovery/                # 恢复层
│   │   ├── __init__.py
│   │   └── error_recovery.py    # 错误恢复
│   │
│   ├── bridge/                  # 通信桥接
│   │   ├── __init__.py
│   │   ├── zmq_server.py        # ZeroMQ 服务器
│   │   └── protocol.py          # 通信协议
│   │
│   ├── plugins/                 # 插件系统
│   │   ├── __init__.py
│   │   ├── base.py              # 插件基类
│   │   ├── message.py           # 消息插件
│   │   ├── contact.py           # 联系人插件
│   │   └── moments.py           # 朋友圈插件
│   │
│   ├── config/                  # 配置
│   │   ├── __init__.py
│   │   ├── settings.py          # 设置
│   │   └── constants.py         # 常量
│   │
│   └── utils/                   # 工具函数
│       ├── __init__.py
│       ├── logger.py            # 日志
│       ├── image.py             # 图像处理
│       └── timing.py            # 时序控制
│
├── tests/
├── requirements.txt
└── pyproject.toml
```

---

## 事件驱动核心

### 事件定义

```python
# src/core/events.py
from dataclasses import dataclass
from typing import Any, Optional
from datetime import datetime

@dataclass
class Event:
    """事件基类"""
    type: str
    timestamp: datetime
    data: Any
    source: str = ""

@dataclass
class MessageEvent(Event):
    """消息事件"""
    chat_id: str = ""
    sender: str = ""
    content: str = ""
    message_type: str = "text"

@dataclass
class ContactEvent(Event):
    """联系人事件"""
    contact_id: str = ""
    nickname: str = ""

@dataclass
class MomentsEvent(Event):
    """朋友圈事件"""
    moment_id: str = ""
    author: str = ""

@dataclass
class RiskEvent(Event):
    """风控事件"""
    risk_level: str = "low"
    description: str = ""
```

### 事件总线

```python
# src/core/bus.py
from typing import Callable, Dict, List
from .events import Event

class EventBus:
    """事件总线"""
    
    def __init__(self):
        self._handlers: Dict[str, List[Callable]] = {}
    
    def on(self, event_type: str, handler: Callable):
        """注册事件处理器"""
        if event_type not in self._handlers:
            self._handlers[event_type] = []
        self._handlers[event_type].append(handler)
    
    def off(self, event_type: str, handler: Callable):
        """注销事件处理器"""
        if event_type in self._handlers:
            self._handlers[event_type].remove(handler)
    
    async def emit(self, event: Event):
        """触发事件"""
        handlers = self._handlers.get(event.type, [])
        for handler in handlers:
            try:
                if asyncio.iscoroutinefunction(handler):
                    await handler(event)
                else:
                    handler(event)
            except Exception as e:
                logger.error(f"Event handler error: {e}")

# 全局事件总线
event_bus = EventBus()
```

---

## 人行为模拟

### 鼠标轨迹

```python
# src/executor/human_sim.py
import numpy as np
from typing import Tuple
import random

class HumanSimulator:
    """类人行为模拟器"""
    
    def __init__(self):
        self.mouse_speed_range = (0.3, 1.2)  # 秒
        self.pause_probability = 0.1
        self.overshoot_range = (2, 5)  # 像素
    
    def generate_bezier_curve(
        self,
        start: Tuple[int, int],
        end: Tuple[int, int],
        num_points: int = 50
    ) -> list:
        """生成贝塞尔曲线轨迹"""
        # 控制点（添加随机偏移）
        control1 = (
            start[0] + (end[0] - start[0]) * 0.3 + random.randint(-20, 20),
            start[1] + (end[1] - start[1]) * 0.3 + random.randint(-20, 20)
        )
        control2 = (
            start[0] + (end[0] - start[0]) * 0.7 + random.randint(-20, 20),
            start[1] + (end[1] - start[1]) * 0.7 + random.randint(-20, 20)
        )
        
        points = []
        for t in np.linspace(0, 1, num_points):
            # 三阶贝塞尔曲线
            x = (1-t)**3 * start[0] + 3*(1-t)**2*t * control1[0] + \
                3*(1-t)*t**2 * control2[0] + t**3 * end[0]
            y = (1-t)**3 * start[1] + 3*(1-t)**2*t * control1[1] + \
                3*(1-t)*t**2 * control2[1] + t**3 * end[1]
            points.append((int(x), int(y)))
        
        return points
    
    def get_move_duration(self) -> float:
        """获取移动持续时间（Gamma 分布）"""
        return np.random.gamma(shape=2.0, scale=0.3)
    
    def get_typing_delay(self) -> float:
        """获取打字延迟（模拟真实打字节奏）"""
        # WPM 40-80
        wpm = random.uniform(40, 80)
        return 60.0 / (wpm * 5)  # 每个字符的秒数
    
    async def human_click(self, x: int, y: int):
        """类人点击"""
        import pyautogui
        
        # 获取当前位置
        current_x, current_y = pyautogui.position()
        
        # 生成轨迹
        points = self.generate_bezier_curve(
            (current_x, current_y),
            (x, y)
        )
        
        # 移动鼠标
        duration = self.get_move_duration()
        for point in points:
            pyautogui.moveTo(point[0], point[1], _pause=False)
            await asyncio.sleep(duration / len(points))
        
        # 随机停顿
        if random.random() < self.pause_probability:
            await asyncio.sleep(random.uniform(0.1, 0.3))
        
        # 点击
        pyautogui.click()
    
    async def human_type(self, text: str):
        """类人打字"""
        import pyautogui
        
        for char in text:
            pyautogui.write(char, interval=0)
            await asyncio.sleep(self.get_typing_delay())
            
            # 随机打字错误
            if random.random() < 0.02:
                # 输入错误字符
                wrong_char = chr(ord(char) + random.randint(-2, 2))
                pyautogui.write(wrong_char, interval=0)
                await asyncio.sleep(self.get_typing_delay())
                # 删除错误字符
                pyautogui.press('backspace')
                await asyncio.sleep(self.get_typing_delay())

# 全局实例
human_sim = HumanSimulator()
```

---

## OCR 定位

```python
# src/locator/ocr_locator.py
from typing import Optional, Tuple, List
import numpy as np

class OCRLocator:
    """OCR 定位器"""
    
    def __init__(self, engine: str = "paddleocr"):
        self.engine = engine
        self._init_engine()
    
    def _init_engine(self):
        """初始化 OCR 引擎"""
        if self.engine == "paddleocr":
            from paddleocr import PaddleOCR
            self.ocr = PaddleOCR(use_angle_cls=True, lang="ch")
        elif self.engine == "wechat_native":
            # 使用微信原生 OCR
            from .wechat_native_ocr import WeChatNativeOCR
            self.ocr = WeChatNativeOCR()
    
    async def find_text(
        self,
        text: str,
        region: Optional[Tuple[int, int, int, int]] = None
    ) -> Optional[Tuple[int, int]]:
        """
        查找文本位置
        
        Args:
            text: 要查找的文本
            region: 搜索区域 (x, y, width, height)
        
        Returns:
            文本中心坐标 (x, y) 或 None
        """
        # 截图
        screenshot = await self._take_screenshot(region)
        
        # OCR 识别
        results = self.ocr.ocr(screenshot, cls=True)
        
        # 查找匹配文本
        for line in results[0]:
            box, (detected_text, confidence) = line
            if text in detected_text and confidence > 0.8:
                # 计算中心点
                center_x = sum(p[0] for p in box) / 4
                center_y = sum(p[1] for p in box) / 4
                
                # 如果指定了区域，需要加上区域偏移
                if region:
                    center_x += region[0]
                    center_y += region[1]
                
                return (int(center_x), int(center_y))
        
        return None
    
    async def find_all_text(
        self,
        text: str,
        region: Optional[Tuple[int, int, int, int]] = None
    ) -> List[Tuple[int, int]]:
        """查找所有匹配文本的位置"""
        screenshot = await self._take_screenshot(region)
        results = self.ocr.ocr(screenshot, cls=True)
        
        positions = []
        for line in results[0]:
            box, (detected_text, confidence) = line
            if text in detected_text and confidence > 0.8:
                center_x = sum(p[0] for p in box) / 4
                center_y = sum(p[1] for p in box) / 4
                
                if region:
                    center_x += region[0]
                    center_y += region[1]
                
                positions.append((int(center_x), int(center_y)))
        
        return positions
    
    async def get_text_at(
        self,
        x: int,
        y: int,
        width: int = 200,
        height: int = 50
    ) -> str:
        """获取指定位置的文本"""
        region = (x - width//2, y - height//2, width, height)
        screenshot = await self._take_screenshot(region)
        results = self.ocr.ocr(screenshot, cls=True)
        
        if results[0]:
            return results[0][0][1][0]
        return ""

# 全局实例
ocr_locator = OCRLocator()
```

---

## UIAutomation 桥接

```python
# src/executor/uia_bridge.py
import uiautomation as auto
from typing import Optional, List

class UIAutomationBridge:
    """UIAutomation 桥接器"""
    
    def __init__(self):
        self.wechat_window: Optional[auto.WindowControl] = None
    
    def find_wechat_window(self) -> bool:
        """查找微信窗口"""
        try:
            self.wechat_window = auto.WindowControl(
                searchDepth=1,
                Name="微信"
            )
            return self.wechat_window.Exists(maxSearchSeconds=3)
        except Exception as e:
            logger.error(f"Failed to find WeChat window: {e}")
            return False
    
    def get_chat_list(self) -> List[dict]:
        """获取聊天列表"""
        if not self.wechat_window:
            return []
        
        # 查找聊天列表控件
        chat_list = self.wechat_window.ListControl(Name="会话")
        if not chat_list.Exists():
            return []
        
        chats = []
        for item in chat_list.GetChildren():
            name = item.Name
            chats.append({"name": name, "element": item})
        
        return chats
    
    def get_message_list(self) -> List[dict]:
        """获取消息列表"""
        if not self.wechat_window:
            return []
        
        # 查找消息列表控件
        msg_list = self.wechat_window.ListControl(Name="消息")
        if not msg_list.Exists():
            return []
        
        messages = []
        for item in msg_list.GetChildren():
            name = item.Name
            messages.append({"content": name, "element": item})
        
        return messages
    
    def click_chat(self, name: str) -> bool:
        """点击聊天"""
        chats = self.get_chat_list()
        for chat in chats:
            if chat["name"] == name:
                chat["element"].Click()
                return True
        return False
    
    def send_message(self, text: str) -> bool:
        """发送消息"""
        if not self.wechat_window:
            return False
        
        # 查找输入框
        edit = self.wechat_window.EditControl(Name="输入")
        if not edit.Exists():
            return False
        
        # 输入文本
        edit.Click()
        auto.SendKeys(text)
        auto.SendKeys("{ENTER}")
        
        return True

# 全局实例
uia_bridge = UIAutomationBridge()
```

---

## 风控检测

```python
# src/monitor/risk_detector.py
from enum import Enum
from typing import Optional
from datetime import datetime, timedelta

class RiskLevel(Enum):
    LOW = "low"
    MEDIUM = "medium"
    HIGH = "high"
    CRITICAL = "critical"

class RiskDetector:
    """风控检测器"""
    
    def __init__(self):
        self.current_level = RiskLevel.LOW
        self.message_count = 0
        self.last_message_time: Optional[datetime] = None
        self.counters = {}
    
    def check_frequency(self, action: str) -> bool:
        """检查频率限制"""
        now = datetime.now()
        
        # 获取限制配置
        limits = {
            "send_message": {"max_per_minute": 10, "max_per_hour": 100},
            "post_moments": {"max_per_day": 5},
            "like_moments": {"max_per_minute": 5},
        }
        
        limit = limits.get(action)
        if not limit:
            return True
        
        # 检查分钟限制
        if "max_per_minute" in limit:
            key = f"{action}_minute"
            if key not in self.counters:
                self.counters[key] = {"count": 0, "reset_time": now + timedelta(minutes=1)}
            
            counter = self.counters[key]
            if now > counter["reset_time"]:
                counter["count"] = 0
                counter["reset_time"] = now + timedelta(minutes=1)
            
            if counter["count"] >= limit["max_per_minute"]:
                self.current_level = RiskLevel.MEDIUM
                return False
            
            counter["count"] += 1
        
        return True
    
    def detect_risk_signals(self) -> Optional[str]:
        """检测风控信号"""
        # 检测弹窗
        if self._check_popup():
            return "检测到风控弹窗"
        
        # 检测功能限制
        if self._check_function_limit():
            return "功能被限制"
        
        # 检测异常提示
        if self._check_abnormal_tip():
            return "检测到异常提示"
        
        return None
    
    def _check_popup(self) -> bool:
        """检查风控弹窗"""
        import uiautomation as auto
        
        # 查找可能的风控弹窗
        risk_keywords = ["异常", "风险", "限制", "封禁", "违规"]
        
        for keyword in risk_keywords:
            popup = auto.WindowControl(Name=keyword)
            if popup.Exists(maxSearchSeconds=0.1):
                return True
        
        return False
    
    def _check_function_limit(self) -> bool:
        """检查功能限制"""
        # TODO: 实现功能限制检测
        return False
    
    def _check_abnormal_tip(self) -> bool:
        """检查异常提示"""
        # TODO: 实现异常提示检测
        return False

# 全局实例
risk_detector = RiskDetector()
```

---

## ZeroMQ 通信

### 服务器实现

```python
# src/bridge/zmq_server.py
import zmq
import zmq.asyncio
import json
from typing import Callable, Any

class ZMQServer:
    """ZeroMQ 服务器"""
    
    def __init__(self, host: str = "127.0.0.1", port: int = 5555):
        self.host = host
        self.port = port
        self.context = zmq.asyncio.Context()
        self.socket = self.context.socket(zmq.REP)
        self.handlers: dict[str, Callable] = {}
    
    def register_handler(self, tool: str, handler: Callable):
        """注册工具处理器"""
        self.handlers[tool] = handler
    
    async def start(self):
        """启动服务器"""
        self.socket.bind(f"tcp://{self.host}:{self.port}")
        print(f"ZMQ Server listening on {self.host}:{self.port}")
        
        while True:
            try:
                # 接收请求
                message = await self.socket.recv()
                request = json.loads(message)
                
                # 处理请求
                response = await self._handle_request(request)
                
                # 发送响应
                await self.socket.send(json.dumps(response).encode())
            
            except Exception as e:
                print(f"Error handling request: {e}")
                error_response = {
                    "id": request.get("id", ""),
                    "error": str(e)
                }
                await self.socket.send(json.dumps(error_response).encode())
    
    async def _handle_request(self, request: dict) -> dict:
        """处理请求"""
        tool = request.get("tool")
        args = request.get("args", {})
        request_id = request.get("id", "")
        
        handler = self.handlers.get(tool)
        if not handler:
            return {
                "id": request_id,
                "error": f"Unknown tool: {tool}"
            }
        
        try:
            result = await handler(**args)
            return {
                "id": request_id,
                "result": result
            }
        except Exception as e:
            return {
                "id": request_id,
                "error": str(e)
            }
    
    async def stop(self):
        """停止服务器"""
        self.socket.close()
        self.context.term()

# 全局实例
zmq_server = ZMQServer()
```

---

## 插件系统

### 插件基类

```python
# src/plugins/base.py
from abc import ABC, abstractmethod
from typing import Any, Dict

class Plugin(ABC):
    """插件基类"""
    
    @property
    @abstractmethod
    def name(self) -> str:
        """插件名称"""
        pass
    
    @property
    @abstractmethod
    def version(self) -> str:
        """插件版本"""
        pass
    
    @abstractmethod
    async def on_init(self):
        """初始化"""
        pass
    
    @abstractmethod
    async def on_destroy(self):
        """销毁"""
        pass
    
    @abstractmethod
    def get_tools(self) -> Dict[str, Callable]:
        """获取工具列表"""
        pass
```

### 消息插件

```python
# src/plugins/message.py
from .base import Plugin
from ..executor.uia_bridge import uia_bridge
from ..executor.human_sim import human_sim
from ..monitor.risk_detector import risk_detector

class MessagePlugin(Plugin):
    """消息插件"""
    
    @property
    def name(self) -> str:
        return "message"
    
    @property
    def version(self) -> str:
        return "1.0.0"
    
    async def on_init(self):
        """初始化"""
        pass
    
    async def on_destroy(self):
        """销毁"""
        pass
    
    def get_tools(self):
        return {
            "wechat.send_message": self.send_message,
            "wechat.get_messages": self.get_messages,
        }
    
    async def send_message(self, to: str, content: str, type: str = "text"):
        """发送消息"""
        # 检查频率限制
        if not risk_detector.check_frequency("send_message"):
            raise Exception("Rate limited")
        
        # 查找聊天
        if not uia_bridge.click_chat(to):
            raise Exception(f"Chat not found: {to}")
        
        # 类人延迟
        await human_sim.human_type(content)
        
        # 发送
        if not uia_bridge.send_message(content):
            raise Exception("Failed to send message")
        
        return {"success": True}
    
    async def get_messages(self, chat: str, limit: int = 20):
        """获取消息"""
        # 查找聊天
        if not uia_bridge.click_chat(chat):
            raise Exception(f"Chat not found: {chat}")
        
        # 获取消息列表
        messages = uia_bridge.get_message_list()
        
        return {
            "messages": messages[:limit]
        }
```

---

## 主入口

```python
# src/main.py
import asyncio
import signal
from bridge.zmq_server import zmq_server
from plugins.message import MessagePlugin
from plugins.contact import ContactPlugin
from plugins.moments import MomentsPlugin

async def main():
    """主函数"""
    print("Starting PQY Automation Engine...")
    
    # 注册插件
    plugins = [
        MessagePlugin(),
        ContactPlugin(),
        MomentsPlugin(),
    ]
    
    for plugin in plugins:
        await plugin.on_init()
        for tool_name, handler in plugin.get_tools().items():
            zmq_server.register_handler(tool_name, handler)
            print(f"Registered tool: {tool_name}")
    
    # 启动服务器
    try:
        await zmq_server.start()
    except KeyboardInterrupt:
        print("Shutting down...")
    finally:
        # 清理
        for plugin in plugins:
            await plugin.on_destroy()
        await zmq_server.stop()

if __name__ == "__main__":
    asyncio.run(main())
```

---

## 配置文件

```yaml
# config/settings.yaml
engine:
  host: "127.0.0.1"
  port: 5555

ocr:
  engine: "paddleocr"  # paddleocr | wechat_native
  language: "ch"

human_sim:
  mouse:
    speed_range: [0.3, 1.2]
    pause_probability: 0.1
    overshoot_range: [2, 5]
  keyboard:
    wpm_range: [40, 80]
    typo_probability: 0.02

risk_control:
  message:
    max_per_minute: 10
    max_per_hour: 100
  moments:
    max_per_day: 5
```

---

## 下一步

- [前端架构](frontend.md) - Electron 前端设计
- [API 网关](api-gateway.md) - Node.js API 实现
- [MCP 协议](mcp-protocol.md) - MCP 工具定义
- [安全策略](../security/evasion-strategy.md) - 风控规避策略
