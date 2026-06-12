# PQY - 微信自动化中间层

<div align="center">

**安全 · 稳定 · 高性能 · 高扩展 · AI-Ready**

[![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)]()
[![License](https://img.shields.io/badge/license-MIT-green.svg)]()
[![WeChat](https://img.shields.io/badge/WeChat-4.1.10.31-brightgreen.svg)]()
[![HyperDbg](https://img.shields.io/badge/HyperDbg-0.19.0-orange.svg)]()

</div>

## 简介

PQY 是一个基于深度逆向分析的微信 PC 版自动化中间层。通过 **Mmmojo 代理 + XPlugin 注入 + HyperDbg 硬件虚拟化 Hook** 的技术路线，实现对微信原生功能的完整访问，同时通过 EPT 内存影子技术规避所有风控检测。

## 核心特性

| 特性 | 说明 |
|------|------|
| 🔌 **MCP 协议** | Claude/Cursor 等 AI 直接调用 |
| 🛡️ **HyperDbg 硬件虚拟化** | EPT Hook + 反检测，生产级稳定性 |
| 🔗 **Mmmojo 代理** | 通过 IPC 消息代理访问微信原生功能 |
| 🧩 **XPlugin 注入** | 合法插件机制获取原生 API |
| 📱 **功能完整** | 消息、联系人、朋友圈、支付全覆盖 |
| 🖥️ **现代前端** | Electron + React 19 + HeroUI |
| 🔍 **脚本引擎** | 内核级脚本，实时监控和过滤 |
| 🕵️ **反检测** | HyperEvade 透明模式，抵抗反调试 |

## 技术路线

```
AI Agent (MCP)
      │
      ▼
前端 (Electron + React 19 + HeroUI)
      │
      ▼
API 网关 (Node.js + Fastify)
      │
      ▼
核心引擎 (Python / C++)
      │
      ├─→ Mmmojo 代理 (IPC 消息拦截/重放)
      ├─→ XPlugin 注入 (原生 API 访问)
      └─→ HyperDbg VMM (硬件虚拟化 Hook)
           ├─→ EPT Hook (隐形函数钩子)
           ├─→ 脚本引擎 (实时过滤)
           └─→ HyperEvade (反检测)
      │
      ▼
微信原生 (Weixin.dll)
```

---

## 快速开始

### 环境要求

- Windows 10/11 (64-bit)
- Node.js 22+
- Python 3.12+
- Visual Studio 2022 (C++ 开发，WDK 支持)
- WeChat PC 4.1.10.31
- Intel VT-x + EPT 支持（用于 HyperDbg）

### 安装

```bash
# 克隆项目
git clone --recursive https://github.com/sebastEXlabe/pqy-wechat.git
cd pqy-wechat

# 安装依赖
setup.bat  # Windows
```

### 启动

```bash
# 开发模式
npm run dev

# 或分别启动
npm run dev:frontend    # 前端
npm run dev:server      # API 网关
npm run dev:engine      # 自动化引擎
```

### 配置 AI Agent

```json
// claude_desktop_config.json
{
  "mcpServers": {
    "wechat": {
      "command": "node",
      "args": ["path/to/pqy/server/dist/mcp/server.js"]
    }
  }
}
```

---

## 文档

### 架构文档
- [整体架构](architecture/overview.md)
- [HyperDbg 集成方案](architecture/hyperdbg-integration.md)
- [EPT Hook 设计](architecture/ept-hook-design.md)
- [Hook 内核层](architecture/hook-layer.md)
- [Mmmojo 代理](architecture/mmmojo-proxy.md)
- [XPlugin 注入](architecture/xplugin-inject.md)
- [MCP 协议](architecture/mcp-protocol.md)
- [前端架构](architecture/frontend.md)
- [API 网关](architecture/api-gateway.md)
- [自动化引擎](architecture/automation-engine.md)

### 安全文档
- [风控全量分析](risk-control/full-analysis.md)
- [风控规避策略](risk-control/bypass-strategy.md)
- [风险评估](risk-control/risk-assessment.md)
- [量化分析](risk-control/quantitative-analysis.md)

### 逆向文档
- [函数签名](reverse/signatures.md)
- [Mars STN](reverse/mars-stn.md)
- [MMTLS](reverse/mmtls.md)
- [Mmmojo](reverse/mmmojo.md)

---

## 项目结构

```
pqy-wechat/
├── frontend/                # Electron 前端
│   └── src/
│       ├── main/            # Electron 主进程
│       └── renderer/        # React 渲染进程
├── server/                  # Node.js API 网关
│   └── src/
│       ├── api/             # REST API
│       └── mcp/             # MCP Server
├── engine/                  # Python 自动化引擎
│   └── src/
│       ├── core/            # 事件驱动核心
│       ├── executor/        # 执行层
│       └── monitor/         # 监控层
├── native/                  # C++ 原生代码
│   ├── hyperdbg/            # HyperDbg SDK 集成
│   ├── ept_hook/            # EPT Hook 实现
│   └── src/proxy/           # Mmmojo 代理 DLL
├── docs/                    # 文档
├── scripts/                 # 辅助脚本
├── tools/                   # 调试工具
│   └── HyperDbg/            # HyperDbg 源码
└── third-party/             # 第三方库
```

---

## 技术栈（Rust 原生方案）

### 设计原则
- **高原生性**：编译为原生机器码，无运行时开销
- **高隐蔽性**：无解释器，不可反编译
- **高稳定性**：编译时检查，内存安全
- **低依赖**：标准库完善，无虚拟机依赖
- **技术先进**：Rust 现代语言特性

### 前端（Tauri 2.0 应用）
- **Tauri 2.0** (Rust 后端，原生性高，包大小 3-10 MB)
- **React 19** (UI 框架)
- **Tailwind CSS 4** (样式)
- **Zustand** (状态管理)
- **Vite** (构建工具)

### 后端 API 网关（Rust 原生）
- **Axum** (Rust Web 框架，原生性高)
- **Tokio** (异步运行时)
- **Serde** (序列化)
- **Tower** (中间件)
- **ZeroMQ** (与 C++ 通信)

### 后端引擎（Rust 原生）
- **Axum** (Rust Web 框架)
- **Tokio** (异步运行时)
- **Serde** (序列化)
- **ZeroMQ** (与 C++ 通信)
- **Mmmojo IPC** (微信原生 API，直接调用原生函数)

**注意：** 不使用 PaddleOCR 和 pyautogui/pywinauto，直接通过 Mmmojo IPC 调用微信原生功能，更隐蔽、更稳定。

### 原生层（C++ 原生）
- **HyperDbg VMM** (硬件虚拟化核心)
  - EPT Hook (隐形函数钩子)
  - 脚本引擎 (内核级过滤)
  - HyperEvade (反检测)
- **libhyperdbg** (SDK 库)
- **Zydis** (反汇编引擎)
- **ZeroMQ** (与 Rust 通信)
- **Mmmojo IPC** (微信原生 API)

### AI Agent
- **自定义 MCP 实现** (Rust 原生)
- **ZeroMQ** (与 Rust 引擎通信)

### 数据库
- **Mmmojo IPC** (调用微信原生函数获取数据，无需额外数据库)

### 通信层
- **原生 WebSocket** (Rust tungstenite)
- **ZeroMQ** (跨语言通信)

### 测试
- **Rust 测试框架** (cargo test)
- **Tokio 测试** (异步测试)

### 部署
- **Tauri 打包** (原生安装包)
- **GitHub Actions** (CI/CD)

---

## 风控规避

| 检测方式 | 风险等级 | 应对策略 |
|----------|----------|----------|
| 代码完整性检查 | 🟢 极低 | EPT 影子页面 |
| 内存扫描 | 🟢 极低 | 不可见 |
| 模块枚举 | 🟢 极低 | 无新模块 |
| CFG 检测 | 🟢 极低 | 不修改代码 |
| 签名验证 | 🟢 极低 | 不修改文件 |
| 反调试检测 | 🟢 极低 | HyperEvade 透明模式 |
| 时间检测 | 🟢 极低 | RDTSC 模拟 |

---

## 许可证

MIT License

## 致谢

- [HyperDbg](https://github.com/HyperDbg/HyperDbg) — 硬件虚拟化调试器
- [Mars](https://github.com/nicebub/Mars) — 腾讯移动网络框架
- [HeroUI](https://heroui.com/) — React UI 组件库
- [Model Context Protocol](https://modelcontextprotocol.io/) — AI 工具协议
