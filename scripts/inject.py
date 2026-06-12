#!/usr/bin/env python3
"""
PQY DLL 注入工具
将 pqy_hook.dll 注入到微信进程中
"""

import ctypes
import ctypes.wintypes
import sys
import os
import time
import psutil

# Windows API 常量
PROCESS_ALL_ACCESS = 0x1F0FFF
MEM_COMMIT = 0x1000
MEM_RESERVE = 0x2000
PAGE_READWRITE = 0x04
INFINITE = 0xFFFFFFFF

# 获取 Windows API
kernel32 = ctypes.windll.kernel32

def find_wechat_process():
    """查找微信主进程"""
    wechat_processes = []

    for proc in psutil.process_iter(['pid', 'name', 'memory_info']):
        try:
            if proc.info['name'] and 'Weixin.exe' in proc.info['name']:
                wechat_processes.append({
                    'pid': proc.info['pid'],
                    'name': proc.info['name'],
                    'memory': proc.info['memory_info'].rss if proc.info['memory_info'] else 0
                })
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            pass

    if not wechat_processes:
        return None

    # 返回内存占用最大的进程（通常是主进程）
    return max(wechat_processes, key=lambda x: x['memory'])

def inject_dll(pid, dll_path):
    """注入 DLL 到目标进程"""
    # 获取完整路径
    dll_path = os.path.abspath(dll_path)
    dll_path_bytes = dll_path.encode('utf-8')

    print(f"[1] 打开进程 PID={pid}...")

    # 打开目标进程
    h_process = kernel32.OpenProcess(PROCESS_ALL_ACCESS, False, pid)
    if not h_process:
        print(f"❌ 无法打开进程: {ctypes.get_last_error()}")
        return False

    print(f"[2] 分配内存...")

    # 在目标进程中分配内存
    dll_path_size = len(dll_path_bytes) + 1
    remote_memory = kernel32.VirtualAllocEx(
        h_process,
        None,
        dll_path_size,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    )

    if not remote_memory:
        error_code = ctypes.get_last_error()
        print(f"❌ 无法分配内存: 错误码 {error_code}")
        kernel32.CloseHandle(h_process)
        return False

    print(f"    分配到地址: 0x{remote_memory:08X}")

    print(f"[3] 写入 DLL 路径...")

    # 写入 DLL 路径
    written = ctypes.c_size_t(0)
    result = kernel32.WriteProcessMemory(
        h_process,
        remote_memory,
        dll_path_bytes,
        dll_path_size,
        ctypes.byref(written)
    )

    if not result:
        error_code = ctypes.get_last_error()
        print(f"❌ 无法写入内存: 错误码 {error_code}")
        print(f"    DLL 路径: {dll_path}")
        print(f"    路径长度: {dll_path_size}")
        print(f"    目标地址: 0x{remote_memory:08X}")

        # 尝试使用 NtWriteVirtualMemory
        print(f"    尝试使用 NtWriteVirtualMemory...")
        ntdll = ctypes.windll.ntdll
        status = ctypes.c_long(0)
        ntdll.NtWriteVirtualMemory(
            h_process,
            remote_memory,
            dll_path_bytes,
            dll_path_size,
            ctypes.byref(written)
        )
        if status.value == 0:
            print(f"    NtWriteVirtualMemory 成功!")
        else:
            print(f"    NtWriteVirtualMemory 失败: 0x{status.value & 0xFFFFFFFF:08X}")
            kernel32.VirtualFreeEx(h_process, remote_memory, 0, 0x8000)
            kernel32.CloseHandle(h_process)
            return False

    print(f"[4] 获取 LoadLibraryA 地址...")

    # 获取 LoadLibraryA 的地址
    h_kernel32 = kernel32.GetModuleHandleA(b"kernel32.dll")
    print(f"    kernel32.dll 句柄: 0x{h_kernel32:08X}")

    load_library_addr = kernel32.GetProcAddress(h_kernel32, b"LoadLibraryA")
    print(f"    LoadLibraryA 地址: 0x{load_library_addr:08X}")

    if not load_library_addr:
        error_code = ctypes.get_last_error()
        print(f"❌ 无法获取 LoadLibraryA 地址: 错误码 {error_code}")

        # 尝试使用 NtGetProcAddress
        print(f"    尝试使用 GetProcAddressW...")
        try:
            # 直接获取 kernel32 中的函数
            kernel32_mod = ctypes.WinDLL('kernel32.dll')
            load_library_addr = ctypes.cast(kernel32_mod.LoadLibraryA, ctypes.c_void_p).value
            print(f"    LoadLibraryA 地址 (备选): 0x{load_library_addr:08X}")
        except Exception as e:
            print(f"    备选方法失败: {e}")
            kernel32.VirtualFreeEx(h_process, remote_memory, 0, 0x8000)
            kernel32.CloseHandle(h_process)
            return False

    print(f"[5] 创建远程线程...")

    # 创建远程线程执行 LoadLibraryA
    thread_id = ctypes.c_ulong(0)

    # 将地址转换为正确的类型
    start_address = ctypes.c_void_p(load_library_addr)
    parameter = ctypes.c_void_p(remote_memory)

    h_thread = kernel32.CreateRemoteThread(
        h_process,
        None,
        0,
        start_address,
        parameter,
        0,
        ctypes.byref(thread_id)
    )

    if not h_thread:
        print(f"❌ 无法创建远程线程: {ctypes.get_last_error()}")
        kernel32.VirtualFreeEx(h_process, remote_memory, 0, 0x8000)
        kernel32.CloseHandle(h_process)
        return False

    print(f"[6] 等待线程完成...")

    # 等待线程完成
    kernel32.WaitForSingleObject(h_thread, INFINITE)

    # 获取返回值（DLL 模块句柄）
    exit_code = ctypes.c_ulong(0)
    kernel32.GetExitCodeThread(h_thread, ctypes.byref(exit_code))

    print(f"[7] 清理资源...")

    # 清理
    kernel32.VirtualFreeEx(h_process, remote_memory, 0, 0x8000)
    kernel32.CloseHandle(h_thread)
    kernel32.CloseHandle(h_process)

    if exit_code.value:
        print(f"✅ DLL 注入成功! 模块句柄: 0x{exit_code.value:08X}")
        return True
    else:
        print(f"❌ DLL 注入失败 (LoadLibrary 返回 0)")
        return False

def main():
    # 检查参数
    force_mode = '--force' in sys.argv or '-f' in sys.argv

    print("=" * 60)
    print("PQY DLL 注入工具")
    print("=" * 60)

    # 检查 DLL 是否存在
    dll_path = "native/bin/Release/pqy_hook.dll"
    if not os.path.exists(dll_path):
        print(f"❌ 找不到 DLL: {dll_path}")
        print("请先编译项目: cmake --build . --config Release")
        return 1

    print(f"[✓] DLL 路径: {os.path.abspath(dll_path)}")

    # 查找微信进程
    print("\n[...] 查找微信进程...")
    wechat = find_wechat_process()

    if not wechat:
        print("❌ 找不到微信进程")
        print("请先启动微信 4.1.10.31")
        return 1

    print(f"[✓] 找到微信进程:")
    print(f"    PID: {wechat['pid']}")
    print(f"    名称: {wechat['name']}")
    print(f"    内存: {wechat['memory'] / 1024 / 1024:.1f} MB")

    # 确认注入
    print("\n" + "=" * 60)
    print("⚠️  警告: 即将注入 DLL 到微信进程")
    print("    - 请确保微信已登录")
    print("    - 注入后请观察微信是否正常")
    print("    - 如有异常请立即关闭微信")
    print("=" * 60)

    if not force_mode:
        response = input("\n确认注入? (y/N): ").strip().lower()
        if response != 'y':
            print("已取消")
            return 0
    else:
        print("\n[!] Force 模式，跳过确认")

    # 注入 DLL
    print("\n[...] 开始注入...")
    if inject_dll(wechat['pid'], dll_path):
        print("\n" + "=" * 60)
        print("✅ 注入完成!")
        print("=" * 60)
        print("\n请检查:")
        print("1. 微信窗口是否正常显示")
        print("2. 能否正常收发消息")
        print("3. 查看日志: logs/pqy_hook.log")
        print("\n如需卸载 Hook，重启微信即可")
        return 0
    else:
        print("\n❌ 注入失败")
        return 1

if __name__ == "__main__":
    sys.exit(main())
