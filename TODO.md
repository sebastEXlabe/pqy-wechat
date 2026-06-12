# PQY 开发计划

## 项目状态

| 模块 | 状态 | 说明 |
|------|------|------|
| 文档 | ✅ 完成 | 技术选型完成，架构设计完成 |
| 前端框架 | 🔄 进行中 | Tauri 2.0 (Rust) |
| API 网关 | ⏳ 待开始 | Axum (Rust) |
| 引擎 | ⏳ 待开始 | Axum (Rust) + Mmmojo IPC |
| 原生层 | 🔄 进行中 | HyperDbg 集成 |
| 通信层 | ⏳ 待开始 | WebSocket + ZeroMQ |
| 测试 | ⏳ 待开始 | cargo test |
| 部署 | ⏳ 待开始 | Tauri 打包 |

---

## 技术栈

| 层级 | 技术 | 说明 |
|------|------|------|
| 前端 | Tauri 2.0 (Rust) | 原生性高，包大小 3-10 MB |
| API 网关 | Axum (Rust) | 原生性高，性能极好 |
| 引擎 | Axum (Rust) | 原生性高，性能极好 |
| 原生层 | HyperDbg (C++) | EPT Hook，反检测 |
| 通信 | WebSocket + ZeroMQ | 跨语言通信 |
| 数据库 | Mmmojo IPC | 调用微信原生函数获取数据 |

---

## Phase 1: 原生层集成 [当前]

### 任务清单

- [x] 1.1 克隆 HyperDbg 项目
- [ ] 1.2 编译 HyperDbg（需要 VS 2022）
- [ ] 1.3 测试 HyperDbg VMM 加载
- [ ] 1.4 测试 EPT Hook 功能
- [ ] 1.5 集成 libhyperdbg SDK
- [ ] 1.6 实现 mmojo_64.dll 函数 Hook
- [ ] 1.7 测试 Hook 稳定性

### 预计时间：3-4 天

---

## Phase 2: Mmmojo IPC 实现

### 任务清单

- [ ] 2.1 逆向分析 Mmmojo IPC 协议
- [ ] 2.2 实现 Mmmojo IPC 客户端（C++）
- [ ] 2.3 实现发送消息功能
- [ ] 2.4 实现获取联系人功能
- [ ] 2.5 实现获取聊天记录功能
- [ ] 2.6 实现发布朋友圈功能
- [ ] 2.7 测试所有功能

### 预计时间：5-7 天

---

## Phase 3: Rust 引擎开发

### 任务清单

- [ ] 3.1 搭建 Rust 项目结构
- [ ] 3.2 实现 Axum API 网关
- [ ] 3.3 实现 ZeroMQ 通信层
- [ ] 3.4 集成 HyperDbg SDK（Rust 绑定）
- [ ] 3.5 集成 Mmmojo IPC（Rust 绑定）
- [ ] 3.6 实现消息管理 API
- [ ] 3.7 实现联系人管理 API
- [ ] 3.8 实现朋友圈管理 API
- [ ] 3.9 测试所有 API

### 预计时间：5-7 天

---

## Phase 4: 前端开发

### 任务清单

- [ ] 4.1 搭建 Tauri 2.0 项目
- [ ] 4.2 实现 Dashboard 页面
- [ ] 4.3 实现 Chat 页面（消息收发）
- [ ] 4.4 实现 Contacts 页面
- [ ] 4.5 实现 Moments 页面
- [ ] 4.6 实现 AI Agent 页面
- [ ] 4.7 实现 Settings 页面
- [ ] 4.8 实现实时状态推送
- [ ] 4.9 测试所有页面

### 预计时间：5-7 天

---

## Phase 5: AI Agent 集成

### 任务清单

- [ ] 5.1 实现 MCP 协议（Rust 原生）
- [ ] 5.2 实现工具定义（发消息、发朋友圈等）
- [ ] 5.3 实现 AI 对话功能
- [ ] 5.4 测试 Claude Desktop 集成
- [ ] 5.5 测试 Cursor 集成

### 预计时间：3-4 天

---

## Phase 6: 测试与优化

### 任务清单

- [ ] 6.1 单元测试（cargo test）
- [ ] 6.2 集成测试
- [ ] 6.3 E2E 测试
- [ ] 6.4 性能测试
- [ ] 6.5 风控测试（24小时运行）
- [ ] 6.6 安全审计
- [ ] 6.7 文档完善

### 预计时间：3-4 天

---

## Phase 7: 部署与发布

### 任务清单

- [ ] 7.1 Tauri 打包（Windows exe/msi）
- [ ] 7.2 自动更新机制
- [ ] 7.3 错误报告机制
- [ ] 7.4 使用统计
- [ ] 7.5 发布 v1.0

### 预计时间：2-3 天

---

## 总计预计时间：26-36 天

---

## 里程碑

| 里程碑 | 目标 | 预计完成 |
|--------|------|----------|
| M1 | HyperDbg EPT Hook 成功 | Day 4 |
| M2 | Mmmojo IPC 功能完整 | Day 11 |
| M3 | Rust 引擎 API 完整 | Day 18 |
| M4 | 前端页面完整 | Day 25 |
| M5 | AI Agent 集成 | Day 29 |
| M6 | 测试通过 | Day 33 |
| M7 | 项目发布 | Day 36 |

---

## 风险与应对

| 风险 | 概率 | 影响 | 应对 |
|------|------|------|------|
| HyperDbg 编译失败 | 低 | 中 | 使用预编译版本 |
| Mmmojo IPC 逆向困难 | 中 | 高 | 持续逆向分析 |
| Rust 学习曲线高 | 中 | 中 | 参考现有项目 |
| Tauri 兼容性问题 | 低 | 中 | 测试多种环境 |
| 微信更新 | 中 | 中 | 版本适配机制 |

---

## 技术栈对比

| 组件 | 旧方案 | 新方案 | 优化点 |
|------|--------|--------|--------|
| 前端 | Electron (Node.js) | Tauri 2.0 (Rust) | 原生性↑，包大小↓，内存↓ |
| API 网关 | Fastify (Node.js) | Axum (Rust) | 原生性↑，性能↑，依赖↓ |
| 引擎 | FastAPI (Python) | Axum (Rust) | 原生性↑，性能↑，依赖↓ |
| 原生层 | DdiMon (C++) | HyperDbg (C++) | Windows 11 支持，反检测 |
| 通信 | Socket.IO + ZeroMQ | WebSocket + ZeroMQ | 依赖↓，原生性↑ |
| 数据库 | SQLite + LowDB | Mmmojo IPC | 依赖↓↓，隐蔽性↑，实时性↑ |
| 测试 | Vitest + Playwright | cargo test | 原生性↑ |
| 部署 | electron-builder | Tauri 打包 | 原生性↑ |
