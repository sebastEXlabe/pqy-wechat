# PQY 技术选型文档

## 项目场景分析

### 核心需求

| 需求 | 优先级 | 说明 |
|------|--------|------|
| Windows 兼容 | P0 | 微信只在 Windows 运行 |
| 内核级操作 | P0 | HyperDbg EPT Hook |
| 低延迟 | P0 | 消息实时性要求 |
| 高稳定性 | P0 | 24h+ 长时间运行 |
| 低内存占用 | P1 | 用户电脑资源有限 |
| 低检测风险 | P0 | 风控规避 |
| MCP 协议 | P1 | AI Agent 集成 |
| 易于维护 | P1 | 快速迭代 |

### 约束条件

- 运行环境：Windows 10/11 x64
- 目标进程：WeChat.exe (4.1.10.31)
- 网络环境：用户本地网络
- 部署方式：本地安装，无需服务器

---

## 1. 前端技术选型

### 场景分析

**使用场景：**
- 用户本地运行的桌面应用
- 显示微信消息、联系人、朋友圈
- AI Agent 对话界面
- 设置和监控面板

**关键需求：**
- Windows 兼容性（必须）
- 低内存占用（用户电脑资源有限）
- 稳定性（长时间运行）
- 与本地后端通信（WebSocket/IPC）

### 选项对比

| 框架 | 包大小 | 内存占用 | Win 兼容 | WebView 稳定性 | 生态 |
|------|--------|----------|----------|----------------|------|
| **Electron** | 50-150 MB | 150-300 MB | ✅ 完美 | ✅ Chromium 内置 | 成熟 |
| **Tauri 2.0** | 3-10 MB | 50-100 MB | ⚠️ 依赖 WebView2 | ⚠️ 系统 WebView | 成长中 |
| **WPF/WinUI** | 10-20 MB | 30-80 MB | ✅ 原生 | ✅ 原生 | 成熟 |

### 推荐：Electron

**理由：**

1. **Windows 兼容性最好**
   - 微信用户主要在 Windows 10/11
   - Electron 自带 Chromium，不依赖系统 WebView
   - Tauri 依赖 WebView2，某些精简版 Win10 可能没有

2. **WebView 稳定性**
   - 微信本身就是 Electron 架构（PC 版）
   - Chromium 渲染一致性有保障
   - 不会出现 WebView 版本差异导致的渲染问题

3. **与微信进程交互**
   - 需要频繁的 IPC 通信（WebSocket）
   - Electron 的 IPC 机制成熟稳定
   - 可以直接调用 Node.js API 操作本地资源

4. **生态成熟**
   - 大量微信相关工具都是 Electron
   - 社区支持好，问题容易解决
   - 插件丰富（electron-store, electron-updater 等）

5. **内存占用可接受**
   - 用户电脑通常 8GB+ 内存
   - 150-300MB 占用可接受
   - 可通过优化减少内存占用

**技术栈：**
- Electron 35 (桌面框架)
- React 19 (UI 框架)
- Tailwind CSS 4 (样式)
- Zustand (状态管理)
- electron-vite (构建工具)

### 为什么不选 Tauri 2.0？

1. **WebView2 依赖问题**
   - 某些精简版 Windows 10 没有预装 WebView2
   - 需要用户手动安装，增加使用门槛
   - 微信用户群体技术水平参差不齐

2. **渲染一致性问题**
   - 不同 Windows 版本的 WebView2 版本不同
   - 可能导致 UI 渲染差异
   - 调试困难

3. **Rust 后端学习成本**
   - 团队可能不熟悉 Rust
   - 调试和维护成本高
   - Node.js 生态更丰富

---

## 2. 后端 API 网关技术选型

### 场景分析

**使用场景：**
- 本地运行的 API 服务器
- 处理前端请求（消息、联系人、朋友圈）
- WebSocket 实时推送
- MCP Server（AI Agent 调用）
- 与 Python 引擎通信

**关键需求：**
- 低延迟（消息实时性）
- 高稳定性（长时间运行）
- WebSocket 支持（实时推送）
- MCP 协议支持
- 与 Python 引擎 IPC 通信

### 选项对比

| 框架 | 性能 | WebSocket | MCP 兼容 | 稳定性 | 生态 | 学习曲线 |
|------|------|-----------|----------|--------|------|----------|
| **Fastify** | 高 | ✅ 插件 | ✅ 好 | ✅ 成熟 | ✅ 丰富 | 中 |
| **Hono** | 极高 | ✅ 原生 | ⚠️ 需适配 | ⚠️ 较新 | ⚠️ 成长中 | 低 |
| **Express** | 中 | ✅ 插件 | ✅ 好 | ✅ 成熟 | ✅ 丰富 | 低 |
| **NestJS** | 中 | ✅ 原生 | ✅ 好 | ✅ 成熟 | ✅ 丰富 | 高 |

### 推荐：Fastify

**理由：**

1. **MCP 协议兼容性好**
   - MCP SDK 官方示例使用 Express/Fastify
   - SSE (Server-Sent Events) 支持完善
   - 与 @modelcontextprotocol/sdk 集成顺畅

2. **WebSocket 支持成熟**
   - @fastify/websocket 插件稳定
   - 支持房间、命名空间
   - 与 Socket.IO 协议兼容

3. **性能优秀**
   - 比 Express 快 2-3 倍
   - JSON Schema 验证，减少运行时开销
   - 插件系统高效

4. **本地运行场景匹配**
   - 不需要 Edge 部署（Hono 的优势场景）
   - 不需要跨运行时（只在 Node.js 运行）
   - 需要稳定性和生态（Fastify 优势）

5. **与 Python 引擎通信**
   - 可以通过 ZeroMQ/IPC 与 Python 引擎通信
   - Fastify 插件系统支持自定义协议

**技术栈：**
- Fastify 5 (Web 框架)
- TypeScript (语言)
- @fastify/websocket (WebSocket)
- @modelcontextprotocol/sdk (MCP)
- ZeroMQ (与 Python 引擎通信)

### 为什么不选 Hono？

1. **Edge 部署不需要**
   - 我们的场景是本地运行，不需要 Edge
   - Hono 的跨运行时优势用不上

2. **MCP 兼容性待验证**
   - MCP SDK 官方示例没有 Hono
   - 可能需要额外适配工作

3. **生态相对较新**
   - 插件没有 Fastify 丰富
   - 社区支持相对较少

---

## 3. 后端引擎技术选型

### 场景分析

**使用场景：**
- 微信自动化操作（消息发送、朋友圈发布等）
- 与 C++ 原生层通信（HyperDbg/Mmmojo）
- 事件驱动架构

**关键需求：**
- 异步支持（并发处理多个操作）
- 与 C++ 通信（ZeroMQ/IPC）
- 长时间运行稳定性
- 低检测风险

### 自动化方案对比

#### 旧方案：UI 自动化 + PaddleOCR

```
用户请求 → Python 引擎 → UI 自动化 → 模拟点击/输入 → OCR 识别结果
```

**问题：**
- ❌ UI 自动化容易被检测（pyautogui/pywinauto 进程）
- ❌ OCR 识别有延迟，不稳定
- ❌ 依赖 UI 状态，容易失败
- ❌ 额外依赖（PaddleOCR），增加包大小

#### 新方案：内核级 Hook + Mmmojo IPC

```
用户请求 → Python 引擎 → Mmmojo IPC → 微信原生函数 → 直接返回结果
```

**优势：**
- ✅ 无 UI 操作，不可检测
- ✅ 直接调用原生 API，稳定高效
- ✅ 不依赖 UI 状态
- ✅ 无额外依赖

### 核心架构

```
┌─────────────────────────────────────────────────────────────┐
│                    用户请求                                    │
│                          │                                    │
│                          ▼                                    │
│  ┌─────────────────────────────────────────────────────┐    │
│  │              Python 引擎 (FastAPI)                    │    │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐ │    │
│  │  │ 消息管理    │  │ 联系人管理  │  │ 朋友圈管理  │ │    │
│  │  └─────────────┘  └─────────────┘  └─────────────┘ │    │
│  └─────────────────────────────────────────────────────┘    │
│                          │                                    │
│                          ▼                                    │
│  ┌─────────────────────────────────────────────────────┐    │
│  │              Mmmojo IPC 层                            │    │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐ │    │
│  │  │ SendMsg     │  │ GetContacts │  │ PostMoments │ │    │
│  │  └─────────────┘  └─────────────┘  └─────────────┘ │    │
│  └─────────────────────────────────────────────────────┘    │
│                          │                                    │
│                          ▼                                    │
│  ┌─────────────────────────────────────────────────────┐    │
│  │              HyperDbg EPT Hook                       │    │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐ │    │
│  │  │ 拦截函数    │  │ 参数修改    │  │ 返回值控制  │ │    │
│  │  └─────────────┘  └─────────────┘  └─────────────┘ │    │
│  └─────────────────────────────────────────────────────┘    │
│                          │                                    │
│                          ▼                                    │
│  ┌─────────────────────────────────────────────────────┐    │
│  │              微信原生 (Weixin.dll)                    │    │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐ │    │
│  │  │ mmojo_64.dll│  │ Weixin.dll  │  │ 其他模块    │ │    │
│  │  └─────────────┘  └─────────────┘  └─────────────┘ │    │
│  └─────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
```

### 功能实现方式

#### 1. 发送消息

**旧方案（UI 自动化）：**
```python
# 找到聊天窗口 → 模拟输入 → 点击发送
pyautogui.click(100, 200)  # 点击输入框
pyautogui.typewrite('Hello')  # 输入文字
pyautogui.click(300, 400)  # 点击发送
```

**新方案（Mmmojo IPC）：**
```python
# 直接调用微信原生函数
await mmojo.send_message(contact_id='wxid_xxx', content='Hello')
```

#### 2. 获取联系人

**旧方案（UI 自动化）：**
```python
# 打开联系人列表 → OCR 识别 → 解析结果
screenshot = pyautogui.screenshot()
contacts = paddleocr.ocr(screenshot)  # OCR 识别
```

**新方案（Mmmojo IPC）：**
```python
# 直接调用微信原生函数
contacts = await mmojo.get_contacts()
```

#### 3. 发布朋友圈

**旧方案（UI 自动化）：**
```python
# 打开朋友圈 → 模拟点击 → 输入内容 → 发布
pyautogui.click(100, 200)  # 点击朋友圈入口
pyautogui.typewrite('今天天气真好')  # 输入内容
pyautogui.click(300, 400)  # 点击发布
```

**新方案（Mmmojo IPC）：**
```python
# 直接调用微信原生函数
await mmojo.post_moments(content='今天天气真好', images=['photo.jpg'])
```

#### 4. OCR 识别

**旧方案（PaddleOCR）：**
```python
# 截图 → PaddleOCR 识别 → 解析结果
screenshot = pyautogui.screenshot()
text = paddleocr.ocr(screenshot)
```

**新方案（微信 OCR）：**
```python
# 调用微信内置 OCR
text = await mmojo.ocr(image_path='screenshot.png')
```

### 推荐：FastAPI + Mmmojo IPC

**理由：**

1. **内核级操作，不可检测**
   - 不使用 UI 自动化，避免被检测
   - 直接调用原生 API，绕过 UI 层
   - 通过 HyperDbg EPT Hook，隐形操作

2. **稳定高效**
   - 不依赖 UI 状态，不会因为窗口变化失败
   - 直接调用原生函数，延迟低
   - 无 OCR 识别误差

3. **与 C++ 通信成熟**
   - ZeroMQ Python 绑定成熟
   - FastAPI 可以轻松集成 ZeroMQ
   - 异步支持，不会阻塞主线程

4. **长时间运行稳定**
   - FastAPI 基于 Starlette，稳定性有保障
   - uvicorn 服务器成熟
   - 社区有大量生产环境案例

5. **开发效率**
   - 自动 OpenAPI 文档，方便调试
   - Pydantic 数据验证，减少错误
   - 依赖注入，代码组织清晰

**技术栈：**
- FastAPI (Web 框架)
- Python 3.12+ (语言)
- Pydantic v2 (数据验证)
- uvicorn (ASGI 服务器)
- ZeroMQ (与 C++ 通信)
- Mmmojo IPC (微信原生 API)

### 为什么不选 Litestar？

1. **生态相对较新**
   - 社区支持相对较少
   - 问题解决方案少

2. **团队熟悉度**
   - FastAPI 更多人熟悉
   - 学习成本低

---

## 4. AI Agent 技术选型

### 场景分析

**使用场景：**
- AI 代理调用微信功能（发消息、发朋友圈等）
- 自然语言理解用户意图
- 多轮对话管理
- 与 Claude/Cursor 等 AI 工具集成

**关键需求：**
- MCP 协议支持（Claude/Cursor 集成）
- 工具定义和调用
- 上下文管理
- 错误处理

### 选项对比

| 框架 | 用途 | MCP 支持 | 复杂度 | 生态 | 适用场景 |
|------|------|----------|--------|------|----------|
| **MCP SDK** | 工具集成 | ✅ 原生 | 低 | 成长中 | 工具调用 |
| **LangChain** | 链式调用 | ⚠️ 需适配 | 高 | 成熟 | 复杂链式 |
| **LangGraph** | 状态图 | ⚠️ 需适配 | 高 | 成长中 | 多步工作流 |
| **自定义** | 灵活 | ✅ 完全控制 | 中 | 无 | 定制化 |

### 推荐：MCP SDK + 自定义

**理由：**

1. **MCP 协议原生支持**
   - Anthropic 官方协议
   - Claude/Cursor 直接调用
   - 标准化工具定义

2. **适合我们的场景**
   - 主要是工具调用（发消息、发朋友圈等）
   - 不需要复杂的链式调用
   - 不需要多 Agent 协作

3. **轻量高效**
   - 无需引入 LangChain 等重量级框架
   - 减少依赖，降低复杂度
   - 更容易维护和调试

4. **灵活扩展**
   - 可以自定义 Agent 逻辑
   - 可以集成任何 LLM
   - 可以添加自定义工具

**技术栈：**
- @modelcontextprotocol/sdk (TypeScript MCP SDK)
- mcp (Python MCP SDK)
- 自定义 Agent 框架
- OpenAI/Anthropic SDK (LLM 调用)

---

## 5. 数据库技术选型

### 场景分析

**使用场景：**
- 获取聊天记录
- 获取联系人信息
- 获取朋友圈数据
- 获取配置信息

**关键需求：**
- 与微信数据一致
- 实时性高
- 隐蔽性高
- 无额外依赖

### 方案对比

| 方案 | 数据一致性 | 实时性 | 隐蔽性 | 依赖 | 复杂度 |
|------|------------|--------|--------|------|--------|
| **独立数据库** | 🔴 需同步 | 🔴 低 | 🔴 低 | 🔴 多 | 🔴 高 |
| **微信数据库** | 🟢 一致 | 🟢 高 | 🟢 高 | 🟢 少 | 🟢 低 |

### 推荐：直接使用微信数据库

**理由：**

1. **数据一致性**
   - 微信已经有完整数据库
   - 数据与微信显示一致
   - 无需同步，无数据不一致问题

2. **实时性高**
   - 数据是最新的
   - 无需等待同步
   - 直接读取即可

3. **隐蔽性高**
   - 无需创建额外数据库文件
   - 无额外进程
   - 无额外依赖

4. **依赖少**
   - 无需独立数据库服务
   - 无需数据库驱动
   - 减少包大小

### 微信数据库分析

**微信数据库：**
- **类型**：SQLite（加密，SQLCipher）
- **位置**：`%APPDATA%\Tencent\WeChat\` 或类似目录
- **内容**：聊天记录、联系人、朋友圈、设置等
- **加密**：使用 SQLCipher 加密

**数据获取方式：**

1. **方式一：Mmmojo IPC（推荐）**
   - 调用微信原生函数获取数据
   - 无需解密数据库
   - 隐蔽性最高

2. **方式二：直接读取数据库**
   - 解密微信 SQLite 数据库
   - 需要获取加密密钥
   - 数据最完整

**技术栈：**
- **Mmmojo IPC** (调用微信原生函数获取数据)
- **rusqlite** (Rust SQLite 绑定，用于读取微信数据库)
- **SQLCipher** (解密微信数据库，如果需要)

---

## 6. 实时通信技术选型

### 场景分析

**使用场景：**
- 前端 ↔ API 网关：消息推送、状态更新
- API 网关 ↔ Python 引擎：操作指令、结果返回
- Python 引擎 ↔ C++ 原生层：Hook 事件、函数调用

**关键需求：**
- 低延迟（消息实时性）
- 高可靠性（不能丢消息）
- 双向通信
- 跨语言支持（Node.js/Python/C++）

### 通信架构

```
前端 (Electron)
    │
    │ Socket.IO (WebSocket)
    ▼
API 网关 (Fastify)
    │
    │ ZeroMQ (IPC)
    ▼
Python 引擎 (FastAPI)
    │
    │ ZeroMQ (IPC)
    ▼
C++ 原生层 (HyperDbg)
```

### 推荐：Socket.IO + ZeroMQ

**理由：**

1. **Socket.IO（前端 ↔ API 网关）**
   - 自动重连，断线恢复
   - 房间管理，消息广播
   - 前后端库完善
   - 开发效率高

2. **ZeroMQ（API 网关 ↔ Python ↔ C++）**
   - 高性能，低延迟
   - 跨语言支持（Node.js/Python/C++）
   - 多种通信模式（REQ/REP, PUB/SUB, PUSH/PULL）
   - 无需消息队列服务

3. **为什么不选原生 WebSocket？**
   - 需要自己实现重连、房间管理
   - 开发成本高
   - Socket.IO 已经封装好了

4. **为什么不选 gRPC？**
   - 复杂度高
   - 需要定义 proto 文件
   - 调试困难

**技术栈：**
- Socket.IO (前端 ↔ API 网关)
- ZeroMQ (API 网关 ↔ Python ↔ C++)
- @zeromq/node (Node.js ZeroMQ 绑定)
- pyzmq (Python ZeroMQ 绑定)

---

## 7. 构建工具技术选型

### 场景分析

**使用场景：**
- Electron 主进程构建
- React 渲染进程构建
- TypeScript 编译
- 开发热重载
- 生产打包

**关键需求：**
- Electron 支持
- React 支持
- TypeScript 支持
- 开发效率
- 生产优化

### 推荐：electron-vite

**理由：**

1. **Electron 专为优化**
   - 专为 Electron 项目设计
   - 主进程、渲染进程、预加载脚本分别构建
   - 自动处理 Electron 特有问题

2. **基于 Vite**
   - HMR 极快
   - 插件丰富
   - 生态成熟

3. **开箱即用**
   - 零配置启动
   - TypeScript 支持
   - React 支持

**技术栈：**
- electron-vite (Electron 构建工具)
- TypeScript (语言)
- ESLint + Prettier (代码规范)
- electron-builder (打包发布)

---

## 8. 测试技术选型

### 场景分析

**使用场景：**
- 单元测试（API、工具函数）
- 集成测试（前后端通信）
- E2E 测试（完整流程）
- 性能测试（长时间运行）

**关键需求：**
- Electron 测试支持
- WebSocket 测试
- Mock 能力
- 覆盖率统计

### 推荐：Vitest + Playwright + electron-playwright

**理由：**

1. **Vitest（单元测试）**
   - 与 Vite 深度集成
   - 性能极高
   - TypeScript 原生支持

2. **Playwright + electron-playwright（E2E 测试）**
   - 原生 Electron 测试支持
   - 可以测试 IPC 通信
   - 可以模拟用户操作

3. **MSW（API Mock）**
   - Mock WebSocket/HTTP
   - 模拟微信 API
   - 隔离测试环境

**技术栈：**
- Vitest (单元测试)
- Playwright (E2E 测试)
- electron-playwright (Electron 测试)
- MSW (API Mock)

---

## 9. 部署技术选型

### 场景分析

**使用场景：**
- 本地安装（用户电脑）
- 自动更新
- 错误报告
- 使用统计

**关键需求：**
- Windows 安装包（exe/msi）
- 自动更新机制
- 静默安装
- 卸载清理

### 推荐：electron-builder + electron-updater

**理由：**

1. **electron-builder（打包）**
   - 支持 Windows exe/msi 安装包
   - 支持代码签名
   - 支持 NSIS 安装器
   - 社区活跃，文档完善

2. **electron-updater（自动更新）**
   - 增量更新，节省带宽
   - 静默更新，用户体验好
   - 支持 GitHub Releases
   - 与 electron-builder 集成好

3. **GitHub Actions（CI/CD）**
   - 自动构建
   - 自动测试
   - 自动发布
   - 免费额度足够

**技术栈：**
- electron-builder (打包)
- electron-updater (自动更新)
- GitHub Actions (CI/CD)
- GitHub Releases (版本发布)

---

## 最终技术栈（优化版）

### 前端（Tauri 2.0 应用）

**选择 Tauri 2.0 替代 Electron：**

| 对比项 | Electron | Tauri 2.0 |
|--------|----------|-----------|
| 包大小 | 50-150 MB | 3-10 MB |
| 内存占用 | 150-300 MB | 50-100 MB |
| 后端语言 | Node.js | Rust |
| 原生性 | 低 | 高 |
| 安全性 | 中 | 高 |
| WebView | Chromium 内置 | 系统 WebView2 |

**技术栈：**
- **Tauri 2.0** (Rust 后端，原生性高)
- **React 19** (UI 框架)
- **Tailwind CSS 4** (样式)
- **Zustand** (状态管理)
- **Vite** (构建工具)

**为什么不选 WPF/WinUI？**
- 开发效率低
- 跨平台能力差
- 社区生态少

### 后端 API 网关（Rust 原生）

**选择 Rust 替代 Node.js：**

| 对比项 | Node.js (Fastify) | Rust (Axum) |
|--------|-------------------|-------------|
| 性能 | 高 | 极高 |
| 内存占用 | 中 | 低 |
| 原生性 | 低 | 高 |
| 依赖 | 多 | 少 |
| 安全性 | 中 | 高 |

**技术栈：**
- **Axum** (Rust Web 框架，原生性高)
- **Tokio** (异步运行时)
- **Serde** (序列化)
- **Tower** (中间件)
- **ZeroMQ** (与 Python 通信)

**为什么不选 Fastify？**
- Node.js 非原生
- 内存占用相对较高
- 依赖较多

### 后端引擎（Rust 原生）

**选择 Rust 替代 Python：**

| 对比项 | Python (FastAPI) | Rust (Axum) |
|--------|------------------|-------------|
| 性能 | 高 | 极高 |
| 内存占用 | 中 | 低 |
| 原生性 | 低 | 高 |
| 依赖 | 多 | 少 |
| 类型安全 | 中 | 高 |

**技术栈：**
- **Axum** (Rust Web 框架)
- **Tokio** (异步运行时)
- **Serde** (序列化)
- **ZeroMQ** (与 C++ 通信)
- **Mmmojo IPC** (微信原生 API)

**为什么不选 FastAPI？**
- Python 非原生
- 性能相对较低
- 依赖较多

### 原生层（C++ 原生）

**保持 C++ 原生：**

**技术栈：**
- **HyperDbg VMM** (硬件虚拟化)
- **libhyperdbg** (SDK)
- **Zydis** (反汇编引擎)
- **ZeroMQ** (与 Rust 通信)
- **Mmmojo IPC** (微信原生 API)

### AI Agent

**选择原生 MCP 实现：**

**技术栈：**
- **自定义 MCP 实现** (Rust 原生)
- **ZeroMQ** (与 Rust 引擎通信)

**为什么不选 MCP SDK？**
- 减少依赖
- 原生性更高
- 性能更好

### 数据库

**直接使用微信数据库：**

**技术栈：**
- **Mmmojo IPC** (调用微信原生函数获取数据)
- **rusqlite** (Rust SQLite 绑定，用于读取微信数据库)
- **SQLCipher** (解密微信数据库，如果需要)

**为什么不创建独立数据库？**
- 微信已经有完整数据库
- 数据与微信一致，无需同步
- 减少依赖，提高隐蔽性
- 实时性更高

### 通信层

**选择原生 WebSocket：**

| 对比项 | Socket.IO | 原生 WebSocket |
|--------|-----------|----------------|
| 依赖 | 多 | 少 |
| 原生性 | 低 | 高 |
| 性能 | 高 | 极高 |
| 复杂度 | 低 | 中 |

**技术栈：**
- **原生 WebSocket** (Rust tungstenite)
- **ZeroMQ** (跨语言通信)

### 测试

**选择 Rust 原生测试：**

**技术栈：**
- **Rust 测试框架** (cargo test)
- **Tokio 测试** (异步测试)

### 部署

**选择 Tauri 原生打包：**

**技术栈：**
- **Tauri 打包** (原生安装包)
- **GitHub Actions** (CI/CD)

---

## 技术栈总结（优化版）

| 层级 | 技术 | 原生性 | 隐蔽性 | 稳定性 | 依赖 | 先进性 |
|------|------|--------|--------|--------|------|--------|
| 前端 | Tauri 2.0 (Rust) | 🟢 高 | 🟢 高 | 🟢 高 | 🟢 低 | 🟢 高 |
| API 网关 | Axum (Rust) | 🟢 高 | 🟢 高 | 🟢 高 | 🟢 低 | 🟢 高 |
| 引擎 | Axum (Rust) | 🟢 高 | 🟢 高 | 🟢 高 | 🟢 低 | 🟢 高 |
| 原生层 | HyperDbg (C++) | 🟢 高 | 🟢 高 | 🟢 高 | 🟢 低 | 🟢 高 |
| 通信 | WebSocket + ZeroMQ | 🟢 高 | 🟢 高 | 🟢 高 | 🟢 低 | 🟢 高 |
| 数据库 | 微信数据库 (Mmmojo IPC) | 🟢 高 | 🟢 高 | 🟢 高 | 🟢 极低 | 🟢 高 |

## 优化前后对比

| 层级 | 旧方案 | 新方案 | 优化点 |
|------|--------|--------|--------|
| 前端 | Electron (Node.js) | Tauri 2.0 (Rust) | 原生性↑，包大小↓，内存↓ |
| API 网关 | Fastify (Node.js) | Axum (Rust) | 原生性↑，性能↑，依赖↓ |
| 引擎 | FastAPI (Python) | Axum (Rust) | 原生性↑，性能↑，依赖↓ |
| 原生层 | HyperDbg (C++) | HyperDbg (C++) | 保持 |
| 通信 | Socket.IO + ZeroMQ | WebSocket + ZeroMQ | 依赖↓，原生性↑ |
| 数据库 | SQLite + LowDB | 微信数据库 (Mmmojo IPC) | 依赖↓↓，隐蔽性↑，实时性↑ |

## Rust 技术栈优势

### 为什么选择 Rust？

1. **原生性高**
   - 编译为原生机器码
   - 无运行时开销
   - 直接调用系统 API

2. **隐蔽性高**
   - 无解释器，不可反编译
   - 内存安全，无漏洞
   - 无垃圾回收，行为可预测

3. **稳定性高**
   - 编译时检查，无运行时错误
   - 内存安全，无崩溃
   - 并发安全，无竞态条件

4. **依赖低**
   - 标准库完善
   - 无虚拟机依赖
   - 无额外运行时

5. **技术先进**
   - 现代语言特性
   - 活跃社区
   - 快速发展

### Rust vs Node.js vs Python

| 对比项 | Rust | Node.js | Python |
|--------|------|---------|--------|
| 性能 | 极高 | 高 | 中 |
| 内存占用 | 低 | 中 | 高 |
| 原生性 | 高 | 低 | 低 |
| 类型安全 | 高 | 中 | 低 |
| 依赖 | 低 | 多 | 多 |
| 学习曲线 | 高 | 低 | 低 |

## 自动化方案对比

| 方案 | 检测风险 | 稳定性 | 性能 | 依赖 |
|------|----------|--------|------|------|
| **UI 自动化 + PaddleOCR** | 🔴 高 | 🔴 低 | 🔴 低 | 🔴 多 |
| **Mmmojo IPC + HyperDbg** | 🟢 极低 | 🟢 高 | 🟢 高 | 🟢 少 |

---

## 与旧方案对比

| 组件 | 旧方案 | 新方案 | 改进点 |
|------|--------|--------|--------|
| 前端 | Electron | Electron (保留) | 已经最优 |
| API 网关 | Fastify | Fastify (保留) | MCP 兼容好 |
| 引擎 | FastAPI | FastAPI (保留) | OCR 集成好 |
| 原生层 | DdiMon | HyperDbg | Windows 11 支持，反检测 |
| 通信 | 自定义 | Socket.IO + ZeroMQ | 标准化，跨语言 |
| 数据库 | 无 | SQLite + LowDB | 新增，数据持久化 |
| 测试 | 无 | Vitest + Playwright | 新增，质量保障 |
| 部署 | 手动 | electron-builder | 新增，自动更新 |

---

## 迁移计划

### Phase 1: 原生层迁移（当前）
1. ✅ 克隆 HyperDbg
2. 编译 HyperDbg
3. 集成 libhyperdbg SDK
4. 测试 EPT Hook

### Phase 2: 通信层标准化
1. 集成 ZeroMQ
2. 标准化 IPC 协议
3. 测试跨语言通信

### Phase 3: 数据层完善
1. 集成 SQLite
2. 集成 LowDB
3. 实现数据持久化

### Phase 4: 测试覆盖
1. 集成 Vitest
2. 集成 Playwright
3. 实现自动化测试

### Phase 5: 部署自动化
1. 集成 electron-builder
2. 集成 electron-updater
3. 实现自动更新

### 预计时间：3-4 周
