# 整体架构

## 架构设计原则

### 1. 安全第一
- **不修改 Weixin.dll**：规避 CRC 校验和签名检测
- **不注入 DLL**：规避进程/模块枚举检测
- **不 Hook CloseHandle/DuplicateHandle**：规避 Handle 验证
- **复用微信原生协议**：避免异常网络流量

### 2. 分层解耦
- 每层职责单一，可独立开发、测试、部署
- 通过标准协议通信，降低耦合度
- 支持水平扩展和故障隔离

### 3. AI-First 设计
- MCP 协议原生支持
- 工具化思维，每个功能都是可调用的工具
- 结构化输出，便于 AI 理解和处理

---

## 系统架构图

```
┌─────────────────────────────────────────────────────────────────────┐
│                         用户交互层                                    │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐              │
│  │   Electron   │  │   Claude     │  │   第三方      │              │
│  │   Desktop    │  │   Desktop    │  │   AI Agent    │              │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘              │
│         │                 │                  │                      │
│         └─────────────────┼──────────────────┘                      │
│                           │                                         │
│                    MCP Protocol                                      │
└───────────────────────────┼─────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────────┐
│                         API 网关层                                    │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    Fastify Server                            │   │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐      │   │
│  │  │  Auth   │  │  Rate   │  │  Route  │  │  Log    │      │   │
│  │  │ Module  │  │ Limiter │  │ Manager │  │ System  │      │   │
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘      │   │
│  └─────────────────────────────────────────────────────────────┘   │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    MCP Server                                │   │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐                    │   │
│  │  │  Tool   │  │  Tool   │  │  Tool   │                    │   │
│  │  │ Router  │  │  Exec   │  │  Resp   │                    │   │
│  │  └─────────┘  └─────────┘  └─────────┘                    │   │
│  └─────────────────────────────────────────────────────────────┘   │
└───────────────────────────┼─────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────────┐
│                         核心业务层                                    │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐│
│  │ Message │  │ Contact │  │ Moments │  │  File   │  │  Media  ││
│  │ Service │  │ Service │  │ Service │  │ Service │  │ Service ││
│  └─────────┘  └─────────┘  └─────────┘  └─────────┘  └─────────┘│
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    AI Agent Manager                          │   │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐      │   │
│  │  │  Skill  │  │  Memory │  │  Prompt │  │  Tool   │      │   │
│  │  │ System  │  │ System  │  │ Manager │  │  Exec   │      │   │
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘      │   │
│  └─────────────────────────────────────────────────────────────┘   │
└───────────────────────────┼─────────────────────────────────────────┘
                            │
                       ZeroMQ + Protobuf
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────────┐
│                       自动化引擎层                                    │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    Event-Driven Core                         │   │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐      │   │
│  │  │ Event   │  │ Watcher │  │Publisher│  │ State   │      │   │
│  │  │  Bus    │  │ Manager │  │  Chain  │  │ Machine │      │   │
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘      │   │
│  └─────────────────────────────────────────────────────────────┘   │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐│
│  │  OCR    │  │ Feature │  │ Anchor  │  │ Human   │  │  Risk   ││
│  │ Locator │  │ Locator │  │ Locator │  │  Sim    │  │Detector ││
│  └─────────┘  └─────────┘  └─────────┘  └─────────┘  └─────────┘│
└───────────────────────────┼─────────────────────────────────────────┘
                            │
                     UIAutomation API
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────────┐
│                        微信原生层                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    WeChat Process                            │   │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐      │   │
│  │  │ Weixin  │  │  Mars   │  │ MMTLS   │  │  XLog   │      │   │
│  │  │  .dll   │  │  STN    │  │ Crypto  │  │ System  │      │   │
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘      │   │
│  └─────────────────────────────────────────────────────────────┘   │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    Windows APIs                              │   │
│  │  UIAutomation │ Accessibility │ Memory Read │ File System   │   │
│  └─────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 层间通信协议

### 1. 前端 ↔ API 网关

**协议**：REST + WebSocket

```typescript
// REST API
GET    /api/messages
POST   /api/messages
GET    /api/contacts
POST   /api/moments

// WebSocket 事件
WS /ws
  → message.new        // 新消息
  → contact.update     // 联系人更新
  → moments.new        // 新朋友圈
  → system.status      // 系统状态
```

### 2. API 网关 ↔ 自动化引擎

**协议**：ZeroMQ + Protocol Buffers

```protobuf
// 消息定义
message EngineCommand {
  string id = 1;
  CommandType type = 2;
  bytes payload = 3;
  int64 timestamp = 4;
}

message EngineResponse {
  string id = 1;
  ResponseStatus status = 2;
  bytes payload = 3;
  string error = 4;
}
```

### 3. 自动化引擎 ↔ 微信原生

**协议**：UIAutomation COM + 内存读取

```python
# UIAutomation
import uiautomation as auto
root = auto.GetRootControl()
wechat = auto.WindowControl(Name="微信")

# 内存读取（只读）
import ctypes
ReadProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead)
```

---

## 数据流示例

### 发送消息流程

```
用户/AI
    │
    ▼
POST /api/messages {to: "张三", content: "你好"}
    │
    ▼
API Gateway
    ├── 验证请求
    ├── 频率限制检查
    └── 转发到 Message Service
            │
            ▼
    Message Service
        ├── 构造 EngineCommand
        └── 发送到 ZeroMQ
                │
                ▼
        Automation Engine
            ├── 接收命令
            ├── OCR 定位输入框
            ├── 类人行为模拟输入
            └── 点击发送
                    │
                    ▼
            WeChat Process
                ├── UI 更新
                └── 通过 Mars STN 发送
```

---

## 关键设计决策

### 1. 为什么选择 Node.js + Python 双引擎？

| 层 | 技术 | 原因 |
|----|------|------|
| API 网关 | Node.js | 高并发、异步 I/O、丰富的 MCP SDK |
| 自动化引擎 | Python | 成熟的 OCR/UIAutomation 库、快速原型开发 |

### 2. 为什么选择 ZeroMQ？

- 高性能：微秒级延迟
- 跨语言：Node.js 和 Python 都有良好支持
- 灵活模式：支持 PUB/SUB、REQ/REP、PUSH/PULL
- 轻量级：无 broker，直接通信

### 3. 为什么选择 MCP 协议？

- 原生支持：Claude Desktop、Cursor 等 AI 工具直接调用
- 标准化：统一的工具定义和调用协议
- 可扩展：易于添加新工具和功能

### 4. 为什么选择 UIAutomation？

- 安全性：Windows 官方辅助功能 API，不触发风控
- 稳定性：COM 接口，版本兼容性好
- 足够性：覆盖 60%+ 的微信功能

---

## 扩展性设计

### 水平扩展

```
                    ┌─────────────┐
                    │  Load       │
                    │  Balancer   │
                    └──────┬──────┘
                           │
            ┌──────────────┼──────────────┐
            │              │              │
            ▼              ▼              ▼
     ┌─────────┐    ┌─────────┐    ┌─────────┐
     │ Server  │    │ Server  │    │ Server  │
     │ Node 1  │    │ Node 2  │    │ Node 3  │
     └────┬────┘    └────┬────┘    └────┬────┘
          │              │              │
          └──────────────┼──────────────┘
                         │
                    ┌────┴────┐
                    │ Engine  │
                    │ Cluster │
                    └─────────┘
```

### 插件扩展

```typescript
// 插件接口
interface Plugin {
  name: string;
  version: string;
  
  // 生命周期
  onInit(): Promise<void>;
  onDestroy(): Promise<void>;
  
  // MCP 工具注册
  getTools(): MCPTool[];
  
  // 事件监听
  onEvent(event: string, handler: Function): void;
}
```

---

## 下一步

- [前端架构](frontend.md) - Electron + React 详细设计
- [API 网关](api-gateway.md) - Fastify + MCP Server 实现
- [自动化引擎](automation-engine.md) - Python 引擎详细设计
- [MCP 协议](mcp-protocol.md) - MCP 工具定义和实现
