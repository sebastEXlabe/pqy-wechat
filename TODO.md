# PQY 任务计划

## 项目状态

| 模块 | 状态 | 说明 |
|------|------|------|
| 文档 | ✅ 完成 | 全面更新完成 |
| 前端框架 | ✅ 完成 | Electron + React 19 + HeroUI |
| API 网关 | ✅ 完成 | Fastify + MCP Server |
| Python 引擎 | ✅ 完成 | 事件驱动 + UIAutomation |
| HyperDbg 集成 | 🔄 进行中 | 从 DdiMon 迁移到 HyperDbg |
| 逆向分析 | ✅ 完成 | Phase 1 & 2 完成 |

---

## Phase 1: HyperDbg 集成 [当前]

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

## Phase 2: Mmmojo 功能实现

### 任务清单

- [ ] 2.1 实现 SendMMMojoWriteInfo Hook
- [ ] 2.2 实现 CreateMMMojoEnvironment Hook
- [ ] 2.3 实现 RemoveMMMojoEnvironment Hook
- [ ] 2.4 解析 Mmmojo 消息格式
- [ ] 2.5 实现消息拦截与转发
- [ ] 2.6 测试消息收发功能

### 预计时间：3-4 天

---

## Phase 3: API 网关完善

### 任务清单

- [ ] 3.1 实现消息 API（发送/接收）
- [ ] 3.2 实现联系人 API
- [ ] 3.3 实现朋友圈 API
- [ ] 3.4 完善 MCP Server 工具
- [ ] 3.5 添加频率限制
- [ ] 3.6 添加风控状态监控

### 预计时间：2-3 天

---

## Phase 4: 前端页面开发

### 任务清单

- [ ] 4.1 完善 Dashboard 页面
- [ ] 4.2 完善 Chat 页面（消息收发）
- [ ] 4.3 完善 Contacts 页面
- [ ] 4.4 完善 Moments 页面
- [ ] 4.5 完善 AI Agent 页面
- [ ] 4.6 完善 Settings 页面
- [ ] 4.7 添加实时状态推送

### 预计时间：3-4 天

---

## Phase 5: AI Agent 集成

### 任务清单

- [ ] 5.1 完善 MCP 工具定义
- [ ] 5.2 实现 AI 对话功能
- [ ] 5.3 实现技能系统
- [ ] 5.4 实现记忆系统
- [ ] 5.5 测试 Claude Desktop 集成
- [ ] 5.6 测试 Cursor 集成

### 预计时间：2-3 天

---

## Phase 6: 测试与优化

### 任务清单

- [ ] 6.1 功能测试（消息/联系人/朋友圈）
- [ ] 6.2 风控测试（24小时运行）
- [ ] 6.3 性能优化
- [ ] 6.4 安全审计
- [ ] 6.5 文档完善

### 预计时间：2-3 天

---

## 总计预计时间：15-21 天

---

## 里程碑

| 里程碑 | 目标 | 预计完成 |
|--------|------|----------|
| M1 | HyperDbg EPT Hook 成功 | Day 4 |
| M2 | Mmmojo 消息收发 | Day 8 |
| M3 | API 完整可用 | Day 11 |
| M4 | 前端完整可用 | Day 15 |
| M5 | AI Agent 集成 | Day 18 |
| M6 | 项目完成 | Day 21 |

---

## 技术栈对比

| 组件 | 旧方案 (DdiMon) | 新方案 (HyperDbg) |
|------|-----------------|-------------------|
| Hypervisor | HyperPlatform | HyperDbg VMM |
| EPT Hook | 自定义实现 | 成熟实现 |
| 反检测 | 无 | HyperEvade |
| 脚本引擎 | 无 | 内核级脚本 |
| SDK | 自定义 | libhyperdbg |
| Windows 11 | ❌ 不支持 | ✅ 支持 |
| AMD-V | ❌ 仅 Intel | ✅ 支持 |
| 维护状态 | 停更 (2020) | 活跃 (2026) |

---

## 风险与应对

| 风险 | 概率 | 影响 | 应对 |
|------|------|------|------|
| HyperDbg 编译失败 | 低 | 中 | 使用预编译版本 |
| EPT Hook 不稳定 | 低 | 高 | 使用 HyperDbg 成熟实现 |
| 驱动签名问题 | 中 | 中 | 启用测试签名 |
| Mmmojo 协议变化 | 低 | 高 | 持续逆向分析 |
| 微信更新 | 中 | 中 | 版本适配机制 |
