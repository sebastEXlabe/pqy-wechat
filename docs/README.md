# PQY - 微信自动化中间层

<div align="center">

**安全 · 稳定 · 模块化 · AI-Ready**

[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)]()
[![License](https://img.shields.io/badge/license-MIT-green.svg)]()
[![WeChat](https://img.shields.io/badge/WeChat-4.1.10.31-brightgreen.svg)]()

</div>

## 简介

PQY 是一个基于深度逆向分析的微信 PC 版自动化中间层。通过 **Mmmojo 代理 + XPlugin 注入 + DdiMon 硬件虚拟化 Hook** 的技术路线，实现对微信原生功能的完整访问，同时通过 EPT 内存影子技术规避所有风控检测。

## 核心特性

| 特性 | 说明 |
|------|------|
| 🔌 **MCP 协议** | Claude/Cursor 等 AI 直接调用 |
| 🛡️ **DdiMon 硬件虚拟化** | EPT 影子页面，检测风险极低 |
| 🔗 **Mmmojo 代理** | 通过 IPC 消息代理访问微信原生功能 |
| 🧩 **XPlugin 注入** | 合法插件机制获取原生 API |
| 📱 **功能完整** | 消息、联系人、朋友圈、支付全覆盖 |
| 🖥️ **现代前端** | Electron + React 19 + HeroUI |

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
      └─→ DdiMon 驱动 (硬件虚拟化 Hook)
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
- Visual Studio 2026 (C++ 开发)
- WeChat PC 4.1.10.31
- Intel VT-x + EPT 支持（用于 DdiMon）

### 安装

```bash
# 克隆项目
git clone https://github.com/pqy/pqy-wechat.git
cd pqy-wechat

# 安装依赖
setup.bat  # Windows
# 或
./setup.sh # Linux/Mac
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
- [DdiMon 扩展方案](architecture/ddimon-extension.md)
- [DdiMon 使用指南](architecture/ddimon-usage.md)
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
│   ├── ddimon/              # DdiMon 驱动（修改版）
│   ├── ddimon_client/       # DdiMon 客户端库
│   ├── ddimon_proxy/        # DdiMon 代理 DLL
│   └── src/proxy/           # Mmmojo 代理 DLL
├── docs/                    # 文档
├── scripts/                 # 辅助脚本
├── tools/                   # 调试工具
└── third-party/             # 第三方库
```

---

## 技术栈

### 前端
- Electron 35 + React 19
- HeroUI + Tailwind CSS 4
- Zustand (状态管理)
- Socket.IO Client

### 后端 (API 网关)
- Node.js 22 + Fastify 5
- @modelcontextprotocol/sdk (MCP)
- Socket.IO (WebSocket)

### 后端 (自动化引擎)
- Python 3.12 + FastAPI
- PaddleOCR
- UIAutomation

### 原生层 (C++)
- DdiMon (硬件虚拟化 Hook)
- MinHook (备用 Hook)
- ZeroMQ (进程间通信)

---

## 风控规避

| 检测方式 | 风险等级 | 应对策略 |
|----------|----------|----------|
| 代码完整性检查 | 🟢 极低 | EPT 影子页面 |
| 内存扫描 | 🟢 极低 | 不可见 |
| 模块枚举 | 🟢 极低 | 无新模块 |
| CFG 检测 | 🟢 极低 | 不修改代码 |
| 签名验证 | 🟢 极低 | 不修改文件 |

---

## 许可证

MIT License

## 致谢

- [DdiMon](https://github.com/tandasat/DdiMon) — 硬件虚拟化 Hook
- [Mars](https://github.com/nicebub/Mars) — 腾讯移动网络框架
- [HeroUI](https://heroui.com/) — React UI 组件库
- [Model Context Protocol](https://modelcontextprotocol.io/) — AI 工具协议
