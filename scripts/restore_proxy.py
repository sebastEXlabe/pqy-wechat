#!/usr/bin/env python3
"""
恢复 mmojo 代理 DLL
"""

import os
import sys
import shutil
from pathlib import Path

# 路径配置
WECHAT_DIR = Path("C:/Program Files/Tencent/Weixin/4.1.10.31")
REAL_DLL = WECHAT_DIR / "mmojo_64.dll"
BACKUP_DLL = WECHAT_DIR / "mmojo_real.dll"

def check_admin():
    """检查管理员权限"""
    import ctypes
    return ctypes.windll.shell32.IsUserAnAdmin() != 0

def restore_dll():
    """恢复真正的 DLL"""
    print("恢复真正的 mmojo_64.dll...")

    if not BACKUP_DLL.exists():
        print(f"错误: 备份 DLL 不存在: {BACKUP_DLL}")
        return False

    # 删除代理 DLL
    if REAL_DLL.exists():
        os.remove(REAL_DLL)
        print(f"删除代理 DLL: {REAL_DLL}")

    # 恢复真正的 DLL
    shutil.copy2(BACKUP_DLL, REAL_DLL)
    print(f"恢复完成: {REAL_DLL}")

    # 删除备份
    os.remove(BACKUP_DLL)
    print(f"删除备份: {BACKUP_DLL}")

    return True

def main():
    print("=" * 60)
    print("恢复 mmojo 代理 DLL")
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

    # 恢复 DLL
    if not restore_dll():
        print("恢复失败")
        return 1

    print("\n" + "=" * 60)
    print("恢复完成！")
    print("=" * 60)
    print("\n注意:")
    print("1. 重启微信后生效")
    print("2. 代理 DLL 已被删除")
    return 0

if __name__ == "__main__":
    sys.exit(main())
