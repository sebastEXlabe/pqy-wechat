# DdiMon 使用指南

## 前提条件

1. **Intel VT-x 支持** - CPU 必须支持虚拟化技术
2. **EPT 支持** - CPU 必须支持 Extended Page Table
3. **测试签名模式** - 需要启用测试签名来加载驱动
4. **管理员权限** - 加载驱动需要管理员权限

## 步骤 1: 检查系统支持

```powershell
# 检查 VT-x 支持
systeminfo | findstr /i "virtualization"

# 检查 EPT 支持 (需要 CPU-Z 或类似工具)
# 或者直接尝试加载驱动
```

## 步骤 2: 启用测试签名

```powershell
# 以管理员身份运行
bcdedit /set testsigning on
# 重启系统
```

## 步骤 3: 编译 DdiMon 驱动

```bash
# 打开 Visual Studio 2026
# 打开 native/ddimon/DdiMon.sln
# 编译 Release x64 配置
# 输出: native/ddimon/x64/Release/DdiMon.sys
```

## 步骤 4: 加载驱动

```powershell
# 以管理员身份运行
sc create DdiMon type= kernel binPath= C:\path\to\DdiMon.sys
sc start DdiMon

# 检查驱动状态
sc query DdiMon
```

## 步骤 5: 编译代理 DLL

```bash
cd native/ddimon_proxy
mkdir build && cd build
cmake .. -G "Visual Studio 18 2026" -A x64
cmake --build . --config Release
```

## 步骤 6: 部署代理 DLL

```powershell
# 以管理员身份运行
# 1. 关闭微信
Get-Process Weixin | Stop-Process -Force

# 2. 备份原始 DLL
Copy-Item "C:\Program Files\Tencent\Weixin\4.1.10.31\mmmojo_64.dll" `
          "C:\Program Files\Tencent\Weixin\4.1.10.31\mmmojo_real.dll"

# 3. 部署代理 DLL
Copy-Item "native\ddimon_proxy\build\Release\mmmojo_64.dll" `
          "C:\Program Files\Tencent\Weixin\4.1.10.31\mmmojo_64.dll"

# 4. 启动微信
Start-Process "C:\Program Files\Tencent\Weixin\Weixin.exe"
```

## 步骤 7: 验证

```powershell
# 检查日志
Get-Content C:\Users\woshi\Downloads\PQY\logs\ddimon_proxy.log

# 检查微信是否正常运行
Get-Process Weixin
```

## 卸载

```powershell
# 1. 停止微信
Get-Process Weixin | Stop-Process -Force

# 2. 恢复原始 DLL
Copy-Item "C:\Program Files\Tencent\Weixin\4.1.10.31\mmmojo_real.dll" `
          "C:\Program Files\Tencent\Weixin\4.1.10.31\mmmojo_64.dll"

# 3. 卸载驱动
sc stop DdiMon
sc delete DdiMon

# 4. 重启系统
bcdedit /deletevalue testsigning
# 重启
```

## 故障排除

### 驱动加载失败

```
错误: 系统无法加载驱动程序
原因: 测试签名未启用
解决: bcdedit /set testsigning on，然后重启
```

### 代理 DLL 无法连接驱动

```
错误: Failed to connect to DdiMon driver
原因: 驱动未加载或权限不足
解决: 以管理员身份运行，确保驱动已加载
```

### 微信崩溃

```
错误: 微信启动后立即崩溃
原因: 代理 DLL 代码错误
解决: 恢复原始 DLL，检查日志
```

## 架构图

```
┌─────────────────────────────────────────────────────────────┐
│                    用户态                                     │
│                                                             │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐    │
│  │   微信      │    │  代理 DLL   │    │  DdiMon     │    │
│  │  (Weixin)   │    │ (mmojo_64)  │    │  Client     │    │
│  └──────┬──────┘    └──────┬──────┘    └──────┬──────┘    │
│         │                  │                  │            │
│         │   LoadLibrary    │   DeviceIoControl │            │
│         └──────────────────┴──────────────────┘            │
│                                    │                        │
└────────────────────────────────────┼────────────────────────┘
                                     │
                                     ▼
┌─────────────────────────────────────────────────────────────┐
│                    内核态                                     │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                 DdiMon 驱动                          │   │
│  │                                                     │   │
│  │  ┌─────────────┐    ┌─────────────┐                │   │
│  │  │ EPT 影子    │    │  Hook       │                │   │
│  │  │ 页面管理    │    │  处理器     │                │   │
│  │  └─────────────┘    └─────────────┘                │   │
│  │                                                     │   │
│  │  ┌─────────────────────────────────────────────┐   │   │
│  │  │              mmojo_64.dll 内存               │   │   │
│  │  │  ┌─────────────────────────────────────┐    │   │   │
│  │  │  │ 读写: 原始代码 (不可检测)           │    │   │   │
│  │  │  │ 执行: Hook 代码 (EPT 保护)          │    │   │   │
│  │  │  └─────────────────────────────────────┘    │   │   │
│  │  └─────────────────────────────────────────────┘   │   │
│  └─────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

## 安全性

| 检测方式 | 风险 | 说明 |
|----------|------|------|
| 代码完整性检查 | 🟢 极低 | EPT 保护，读写看到原始代码 |
| 内存扫描 | 🟢 极低 | 不可见 |
| 模块枚举 | 🟢 极低 | 无新模块 |
| 调用栈检测 | 🟢 极低 | 正常调用栈 |
| CFG 检测 | 🟢 极低 | 不修改代码 |

## 下一步

1. 编译 DdiMon 驱动
2. 加载驱动
3. 编译并部署代理 DLL
4. 测试 Hook 功能
5. 扩展更多 Hook
