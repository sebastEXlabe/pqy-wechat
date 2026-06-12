# DdiMon 扩展 - 用户态函数 Hook

## 原理

DdiMon 使用 EPT（Extended Page Table）实现隐形 inline hook：
- **读写操作**：看到原始代码（不可检测）
- **执行操作**：触发 VM-exit，执行我们的代码

## 修改点

### 1. 添加用户态 DLL 支持

原版 DdiMon 只支持 Hook 内核函数（ntoskrnl.exe）。
我们需要修改它来支持 Hook 用户态函数（mmojo_64.dll）。

### 2. 添加 DeviceIoControl 通信

添加与用户态的通信机制，用于：
- 安装/卸载 Hook
- 传递参数和返回值
- 接收用户态的指令

### 3. 修改 Hook 处理器

修改 Hook 处理器来支持用户态函数调用。

## 文件结构

```
ddimon_ext/
├── ddi_mon_user.cpp      # 用户态扩展
├── ddi_mon_user.h        # 用户态扩展头文件
├── device_control.cpp    # DeviceIoControl 处理
└── README.md             # 本文件
```

## 使用方式

1. 编译修改后的 DdiMon 驱动
2. 加载驱动
3. 通过 DeviceIoControl 安装 Hook
4. 代理 DLL 通过 DeviceIoControl 与驱动通信

## 安全性

| 检测方式 | 风险 |
|----------|------|
| 代码完整性检查 | 🟢 极低（EPT 保护） |
| 内存扫描 | 🟢 极低（不可见） |
| 模块枚举 | 🟢 极低（无新模块） |
| 调用栈检测 | 🟢 极低（正常调用栈） |
