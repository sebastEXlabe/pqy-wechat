# DdiMon 二次开发方案

## 目标

基于 DdiMon 实现用户态函数的隐形 Hook，用于 Hook mmojo_64.dll 的函数。

## DdiMon 原理

```
┌─────────────────────────────────────────────────────────────┐
│                    EPT 内存影子                               │
│                                                             │
│  原始页面 (0x1000):                                          │
│  ┌─────────────────────────────────────────────────────┐   │
│  │ push rbp                                            │   │
│  │ mov rbp, rsp                                        │   │
│  │ sub rsp, 0x20                                       │   │
│  │ ...                                                 │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  执行页面 (0x2000) - 执行时看到：                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │ 0xcc (断点)     ← 触发 VM-exit                       │   │
│  │ mov rbp, rsp                                        │   │
│  │ sub rsp, 0x20                                       │   │
│  │ ...                                                 │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  读写页面 (0x3000) - 读写时看到：                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │ push rbp          ← 原始代码（不可检测）              │   │
│  │ mov rbp, rsp                                        │   │
│  │ sub rsp, 0x20                                       │   │
│  │ ...                                                 │   │
│  └─────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

## 修改点

### 1. 添加 DeviceIoControl 接口

原版 DdiMon 只在 DriverEntry 时安装 Hook。
我们需要添加 DeviceIoControl 接口，让用户态可以动态安装/卸载 Hook。

```cpp
// 新增文件：device_control.cpp

#define IOCTL_INSTALL_HOOK   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_REMOVE_HOOK    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_CALL_ORIGINAL  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _HOOK_REQUEST {
    ULONG64 target_address;     // 要 Hook 的地址
    ULONG64 handler_address;    // Hook 处理器地址
    ULONG64 original_address;   // 原始函数地址（输出）
} HOOK_REQUEST, *PHOOK_REQUEST;
```

### 2. 添加用户态模块枚举

原版 DdiMon 只枚举 ntoskrnl.exe 的导出。
我们需要添加枚举用户态模块导出的功能。

```cpp
// 新增函数：枚举用户态模块导出
NTSTATUS DdimonEnumUserModuleExports(
    PEPROCESS process,
    ULONG_PTR module_base,
    ShadowHookTarget* targets,
    ULONG target_count
);
```

### 3. 修改 Hook 处理器

原版 DdiMon 的 Hook 处理器在内核态执行。
我们需要修改它来支持用户态函数调用。

```cpp
// 修改：支持用户态参数传递
void DdimonHandleUserCall(ShadowHookData* sh_data, void* guest_ip) {
    // 读取用户态参数
    ULONG64 rcx = UtilVmRead(VmcsField::kGuestRcx);
    ULONG64 rdx = UtilVmRead(VmcsField::kGuestRdx);
    ULONG64 r8 = UtilVmRead(VmcsField::kGuestR8);
    ULONG64 r9 = UtilVmRead(VmcsField::kGuestR9);
    
    // 通过 DeviceIoControl 传递给用户态
    // ...
}
```

---

## 文件修改清单

### 需要修改的文件

| 文件 | 修改内容 |
|------|----------|
| `DdiMon/ddi_mon.cpp` | 添加用户态模块枚举 |
| `DdiMon/ddi_mon.h` | 添加新函数声明 |
| `DdiMon/shadow_hook.cpp` | 修改 Hook 处理器 |
| `HyperPlatform/HyperPlatform/util.h` | 添加用户态内存读写函数 |

### 需要新增的文件

| 文件 | 内容 |
|------|------|
| `DdiMon/device_control.cpp` | DeviceIoControl 处理 |
| `DdiMon/device_control.h` | DeviceIoControl 头文件 |
| `DdiMon/user_hook.cpp` | 用户态 Hook 管理 |
| `DdiMon/user_hook.h` | 用户态 Hook 头文件 |

---

## 工作流程

```
1. 用户态加载驱动
   └─ sc create DdiMon type= kernel binPath= ...

2. 用户态发送 Hook 请求
   └─ DeviceIoControl(IOCTL_INSTALL_HOOK, {target, handler})

3. 驱动安装隐形 Hook
   └─ ShInstallHook(shared_sh_data, target, &hook_target)

4. 目标函数被调用
   └─ 触发断点 VM-exit
   └─ 驱动修改 Guest RIP 跳转到处理器

5. 处理器执行
   └─ 读取用户态参数
   └─ 通过 DeviceIoControl 通知用户态
   └─ 用户态决定是否调用原始函数

6. 调用原始函数
   └─ 通过 trampoline 调用原始代码
   └─ 返回结果给用户态
```

---

## 优势

| 对比项 | 代理 DLL | DdiMon 二次开发 |
|--------|----------|-----------------|
| 检测风险 | 🔴 高 | 🟢 极低 |
| CFG 问题 | ❌ 有 | ✅ 无 |
| 模块枚举 | ❌ 会被检测 | ✅ 不会检测 |
| 签名验证 | ❌ 会被检测 | ✅ 不会检测 |
| 实现难度 | 🟡 中 | 🔴 高 |

---

## 实施计划

### Phase 1: 修改 DdiMon 驱动（1-2天）
- 添加 DeviceIoControl 接口
- 添加用户态模块枚举
- 修改 Hook 处理器

### Phase 2: 用户态代理（1天）
- 创建代理 DLL
- 实现与驱动的通信
- 实现函数转发

### Phase 3: 测试（1天）
- 测试 Hook 安装
- 测试函数调用
- 测试稳定性

---

## 风险评估

| 风险 | 概率 | 影响 | 应对 |
|------|------|------|------|
| Intel VT-x 不支持 | 低 | 高 | 检查 CPU 支持 |
| 测试签名问题 | 中 | 中 | 启用测试签名 |
| 驱动加载失败 | 低 | 高 | 检查 Secure Boot |
| 用户态通信延迟 | 低 | 中 | 优化通信机制 |
