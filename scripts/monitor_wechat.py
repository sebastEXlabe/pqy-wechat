#!/usr/bin/env python3
"""
微信崩溃监控脚本
"""

import time
import subprocess
import psutil
from datetime import datetime
from pathlib import Path

def get_wechat_process():
    """获取微信进程"""
    for proc in psutil.process_iter(['pid', 'name', 'create_time']):
        try:
            if proc.info['name'] and 'Weixin.exe' in proc.info['name']:
                return proc
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            pass
    return None

def check_crash_dumps():
    """检查崩溃转储"""
    crash_dir = Path("C:/Users/woshi/Downloads/PQY/crash_dumps")
    if not crash_dir.exists():
        return []

    dumps = list(crash_dir.glob("Weixin.exe.*.dmp"))
    return sorted(dumps, key=lambda x: x.stat().st_mtime, reverse=True)

def check_event_log():
    """检查 Windows 事件日志"""
    try:
        result = subprocess.run(
            ['powershell', '-Command',
             "Get-EventLog -LogName Application -Newest 20 | Where-Object { $_.Message -like '*Weixin*' } | Select-Object TimeGenerated, EntryType, Message | Format-List"],
            capture_output=True, text=True, timeout=10
        )
        return result.stdout
    except Exception as e:
        return f"Error: {e}"

def monitor_wechat():
    """监控微信状态"""
    print("=" * 60)
    print("微信崩溃监控")
    print("=" * 60)

    # 记录启动时间
    start_time = datetime.now()
    last_check = start_time

    # 检查微信是否运行
    wechat = get_wechat_process()
    if not wechat:
        print("[!] 微信未运行，请先启动微信")
        return

    print(f"[+] 微信已运行，PID: {wechat.pid}")
    print(f"[+] 开始监控...")
    print()

    # 记录初始状态
    initial_dumps = check_crash_dumps()
    initial_dump_count = len(initial_dumps)

    try:
        while True:
            time.sleep(1)  # 每秒检查一次

            # 检查微信是否还在运行
            wechat = get_wechat_process()
            if not wechat:
                print(f"\n[!] 微信已退出！")

                # 检查是否有新的崩溃转储
                current_dumps = check_crash_dumps()
                if len(current_dumps) > initial_dump_count:
                    print(f"[!] 检测到新的崩溃转储:")
                    for dump in current_dumps[:initial_dump_count]:
                        print(f"    - {dump.name} ({dump.stat().st_size / 1024 / 1024:.1f} MB)")

                # 检查事件日志
                print("\n[!] 检查 Windows 事件日志:")
                event_log = check_event_log()
                print(event_log)

                break

            # 每5秒输出一次状态
            now = datetime.now()
            if (now - last_check).seconds >= 5:
                elapsed = (now - start_time).seconds
                print(f"[+] 微信运行中... ({elapsed}s)")
                last_check = now

    except KeyboardInterrupt:
        print("\n[!] 监控已停止")

if __name__ == "__main__":
    monitor_wechat()
