# WeChat 4.1.10.31 (Weixin.dll) 逆向分析文档

## 目标
构建依附于微信原生工具链的**安全、稳定、自适应、模块化**自动化系统。

## 分析对象
- 文件: `C:\Program Files\Tencent\Weixin\4.1.10.31\Weixin.dll`
- MD5: `6824220b1a8fd856efa6df287d91bf5e`
- SHA256: `da2e334cd7f7fd981a7db1053e0beb0c75569c32da3d7c298cac0e803ef3c275`
- 基址: `0x180000000`
- 大小: `0xaf0e000` (~183MB)
- 函数总数: 499,269
- 字符串总数: 295,281

## 文档索引
- [风控系统全量分析](risk-control/full-analysis.md)
- [跨团队风控架构](teams/cross-team-architecture.md)
- [网络协议分析](network/mars-stn-protocol.md)
- [MMTLS 加密层分析](crypto/mmtls-analysis.md)
- [函数签名映射表](signatures/golden-reference.md)

## 当前进度
- 已重命名函数: 42 个
- 已确认风控层: 10 个独立团队 > 90 个风控点
- 精确阈值: 20+ 项定时器/超时常数
- 伪装检测点: 15+ 个
- CDN 错误码: 204 个完整列表
- clicfg 配置项: 66 个 (含 min/max/default)
- MMTLS Cipher Suite: TLS_AES_256_GCM_SHA384 > CHACHA20 > AES_128_GCM
- Mmmojo 环境: 5 种子进程类型, 21 个导入函数
- MMTLS Certificates: 4 个嵌入式 Ed25519 X.509 证书
- 配置层级: 5 级 (MMKV → clicfg → ConfigFile → AppConfig → sysmsg)
- 构建信息: release/xwechat/2026-t2, May 19 2026, commit 8bb49abc
- IDB 保存: Weixin.dll.i64 (33次)
- 子代理分析: 18 个并行代理
- 覆盖模块: 99%+ 的 Weixin.dll 模块
- 实机验证: Phase 2 完成 ✅ — 版本/进程/模块/注册表全部确认
