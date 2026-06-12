#!/usr/bin/env python3
"""
PQY 代码自动修复工具
基于审查结果自动修复简单违规
"""

import re
import sys
from pathlib import Path


def fix_missing_exception_handler(content: str, func_name: str) -> str:
    """为 Hook 函数添加异常处理"""
    # 匹配函数定义
    pattern = r'((?:BOOL|void|NTSTATUS).*' + func_name + r'[^\{]*\{)'
    match = re.search(pattern, content)

    if not match:
        return content

    func_start = match.start()
    func_body_start = match.end()

    # 找到函数结束位置
    brace_count = 1
    pos = func_body_start
    while pos < len(content) and brace_count > 0:
        if content[pos] == '{':
            brace_count += 1
        elif content[pos] == '}':
            brace_count -= 1
        pos += 1

    func_end = pos
    func_body = content[func_body_start:func_end-1]

    # 检查是否已有异常处理
    if '__try' in func_body or 'try' in func_body:
        return content

    # 提取原始逻辑
    original_body = func_body.strip()

    # 生成新的函数体
    new_body = f"""
    __try {{
        {original_body}
    }}
    __except (EXCEPTION_EXECUTE_HANDLER) {{
        spdlog::error("Exception in {func_name}: 0x{{:08X}}", GetExceptionCode());
        // 调用原始函数作为 fallback
        return Original_{func_name}(__VA_ARGS__);
    }}
"""

    # 替换函数体
    new_content = content[:func_body_start] + new_body + content[func_end-1:]

    return new_content


def fix_missing_logging(content: str, func_name: str) -> str:
    """为关键操作添加日志"""
    # 在函数开始添加日志
    pattern = r'(' + func_name + r'[^\{]*\{)'
    match = re.search(pattern, content)

    if not match:
        return content

    insert_pos = match.end()
    log_line = f'\n    spdlog::debug("{func_name} called");'

    new_content = content[:insert_pos] + log_line + content[insert_pos:]

    return new_content


def fix_process_filter(content: str) -> str:
    """确保进程过滤包含 pqy 相关进程"""
    # 检查是否已有 pqy 过滤
    if 'pqy' in content.lower():
        return content

    # 在隐藏列表中添加 pqy
    if 'g_hidden_processes' in content:
        content = content.replace(
            'g_hidden_processes.insert("',
            'g_hidden_processes.insert("pqy-");\n    g_hidden_processes.insert("'
        )

    return content


def apply_fixes(file_path: str) -> bool:
    """应用所有修复"""
    path = Path(file_path)

    if not path.exists():
        print(f"Error: {file_path} not found")
        return False

    try:
        content = path.read_text(encoding='utf-8')
    except Exception as e:
        print(f"Error reading {file_path}: {e}")
        return False

    original_content = content

    # 应用修复
    # 1. 为 CRC 绕过添加异常处理
    if 'Hook_IntegrityCheckTable_Verify' in content:
        content = fix_missing_exception_handler(content, 'Hook_IntegrityCheckTable_Verify')

    # 2. 为调试器绕过添加异常处理
    if 'Hook_IsDebuggerPresent' in content:
        content = fix_missing_exception_handler(content, 'Hook_IsDebuggerPresent')

    # 3. 添加日志
    for func in ['Hook_Process32NextW', 'Hook_Module32NextW']:
        if func in content:
            content = fix_missing_logging(content, func)

    # 4. 确保进程过滤完整
    if 'Hook_Process32NextW' in content:
        content = fix_process_filter(content)

    # 保存修复后的文件
    if content != original_content:
        # 备份原文件
        backup_path = path.with_suffix('.bak')
        backup_path.write_text(original_content, encoding='utf-8')

        # 保存修复后的文件
        path.write_text(content, encoding='utf-8')

        print(f"✅ Fixed: {file_path}")
        print(f"   Backup: {backup_path}")
        return True
    else:
        print(f"ℹ️  No fixes needed: {file_path}")
        return False


def main():
    if len(sys.argv) < 2:
        print("Usage: python auto_fix.py <file_or_directory>")
        sys.exit(1)

    target = sys.argv[1]
    path = Path(target)

    fixed_count = 0

    if path.is_file():
        if apply_fixes(str(path)):
            fixed_count += 1
    elif path.is_dir():
        for file in path.rglob("*.cpp"):
            if apply_fixes(str(file)):
                fixed_count += 1
        for file in path.rglob("*.h"):
            if apply_fixes(str(file)):
                fixed_count += 1

    print(f"\n{'='*50}")
    print(f"Fixed {fixed_count} files")

    if fixed_count > 0:
        print("\n⚠️  Please review the fixes and run code_review.py again.")


if __name__ == "__main__":
    main()
