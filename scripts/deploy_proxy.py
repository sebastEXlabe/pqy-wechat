#!/usr/bin/env python3
"""
部署 mmojo 代理 DLL
"""

import os
import sys
import shutil
import subprocess
from pathlib import Path

# 路径配置
WECHAT_DIR = Path("C:/Program Files/Tencent/Weixin/4.1.10.31")
PROXY_DLL = Path("native/bin/Release/mmojo_64.dll")
REAL_DLL = WECHAT_DIR / "mmojo_64.dll"
BACKUP_DLL = WECHAT_DIR / "mmojo_real.dll"

def check_admin():
    """检查管理员权限"""
    import ctypes
    return ctypes.windll.shell32.IsUserAnAdmin() != 0

def build_proxy():
    """编译代理 DLL"""
    print("[1/4] 编译代理 DLL...")

    build_dir = Path("native/build_proxy")
    build_dir.mkdir(exist_ok=True)

    # 配置 CMake
    result = subprocess.run(
        ["cmake", "../src/proxy", "-G", "Visual Studio 17 2022", "-A", "x64"],
        cwd=build_dir,
        capture_output=True,
        text=True
    )

    if result.returncode != 0:
        print(f"CMake 配置失败: {result.stderr}")
        return False

    # 编译
    result = subprocess.run(
        ["cmake", "--build", ".", "--config", "Release"],
        cwd=build_dir,
        capture_output=True,
        text=True
    )

    if result.returncode != 0:
        print(f"编译失败: {result.stderr}")
        return False

    print("编译成功")
    return True

def backup_real_dll():
    """备份真正的 DLL"""
    print("[2/4] 备份真正的 mmojo_64.dll...")

    if BACKUP_DLL.exists():
        print("备份已存在，跳过")
        return True

    if not REAL_DLL.exists():
        print(f"错误: {REAL_DLL} 不存在")
        return False

    shutil.copy2(REAL_DLL, BACKUP_DLL)
    print(f"备份完成: {BACKUP_DLL}")
    return True

def deploy_proxy():
    """部署代理 DLL"""
    print("[3/4] 部署代理 DLL...")

    if not PROXY_DLL.exists():
        print(f"错误: {PROXY_DLL} 不存在")
        return False

    # 复制代理 DLL 到微信目录
    shutil.copy2(PROXY_DLL, REAL_DLL)
    print(f"部署完成: {REAL_DLL}")
    return True

def verify_deployment():
    """验证部署"""
    print("[4/4] 验证部署...")

    if not BACKUP_DLL.exists():
        print("警告: 备份 DLL 不存在")
        return False

    if not REAL_DLL.exists():
        print("错误: 代理 DLL 不存在")
        return False

    print("部署验证成功")
    print(f"  真正的 DLL: {BACKUP_DLL}")
    print(f"  代理 DLL: {REAL_DLL}")
    return True

def main():
    print("=" * 60)
    print("部署 mmojo 代理 DLL")
    print("=" * 60)

    # 检查管理员权限
    if not check_admin():
        print("错误: 需要管理员权限")
        print("请以管理员身份运行此脚本")
        return 1

    # 检查微信目录
    if not WECHAT_DIR.exists():
        print(f"错误: 微信目录不存在: {WECHAT_DIR}")
        return 1

    # 编译代理 DLL
    if not build_proxy():
        print("编译失败")
        return 1

    # 备份真正的 DLL
    if not backup_real_dll():
        print("备份失败")
        return 1

    # 部署代理 DLL
    if not deploy_proxy():
        print("部署失败")
        return 1

    # 验证部署
    if not verify_deployment():
        print("验证失败")
        return 1

    print("\n" + "=" * 60)
    print("部署完成！")
    print("=" * 60)
    print("\n注意:")
    print("1. 重启微信后生效")
    print("2. 如需恢复，运行 restore_proxy.py")
    return 0

if __name__ == "__main__":
    sys.exit(main())
