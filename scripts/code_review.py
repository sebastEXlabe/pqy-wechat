#!/usr/bin/env python3
"""
PQY 代码安全审查工具
基于 docs/risk-control/bypass-strategy.md 的风险规则
"""

import re
import sys
import os
from pathlib import Path
from dataclasses import dataclass
from typing import List, Optional
from enum import Enum

class RiskLevel(Enum):
    CRITICAL = "🔴 红线"
    HIGH = "🟠 高风险"
    MEDIUM = "🟡 中风险"
    LOW = "🟢 低风险"

@dataclass
class Violation:
    file: str
    line: int
    rule: str
    level: RiskLevel
    code: str
    fix: Optional[str] = None

# 审查规则
RULES = [
    # 🔴 红线规则
    {
        "name": "禁止 Hook CloseHandle",
        "pattern": r"MH_CreateHook.*CloseHandle",
        "level": RiskLevel.CRITICAL,
        "fix": "移除此 Hook，使用 UIAutomation 代替"
    },
    {
        "name": "禁止 Hook DuplicateHandle",
        "pattern": r"MH_CreateHook.*DuplicateHandle",
        "level": RiskLevel.CRITICAL,
        "fix": "移除此 Hook，使用 UIAutomation 代替"
    },
    {
        "name": "禁止修改 Weixin.dll",
        "pattern": r"(WriteFile|CreateFile).*Weixin\.dll",
        "level": RiskLevel.CRITICAL,
        "fix": "移除文件写入操作"
    },
    {
        "name": "禁止 CreateRemoteThread",
        "pattern": r"CreateRemoteThread",
        "level": RiskLevel.CRITICAL,
        "fix": "使用 XPlugin 合法机制代替"
    },
    {
        "name": "禁止 SuspendThread",
        "pattern": r"(SuspendThread|NtSuspendThread)",
        "level": RiskLevel.CRITICAL,
        "fix": "移除线程挂起操作"
    },
    {
        "name": "禁止修改系统时间",
        "pattern": r"(SetSystemTime|SetLocalTime)",
        "level": RiskLevel.CRITICAL,
        "fix": "移除时间修改操作"
    },
    # 🟠 高风险规则
    {
        "name": "CRC 绕过需要异常处理",
        "pattern": r"Hook_IntegrityCheckTable_Verify",
        "level": RiskLevel.HIGH,
        "check": lambda code: "__try" not in code,
        "fix": "添加 __try/__except 异常处理"
    },
    {
        "name": "进程枚举绕过需要完整过滤",
        "pattern": r"Hook_Process32NextW",
        "level": RiskLevel.HIGH,
        "check": lambda code: "pqy" not in code.lower(),
        "fix": "添加 pqy 相关进程过滤"
    },
    {
        "name": "调试器绕过需要完整",
        "pattern": r"Hook_IsDebuggerPresent",
        "level": RiskLevel.HIGH,
        "check": lambda code: "NtQueryInformationProcess" not in code,
        "fix": "同时 Hook NtQueryInformationProcess"
    },
    # 🟡 中风险规则
    {
        "name": "Hook 函数需要异常处理",
        "pattern": r"(BOOL|void|NTSTATUS).*Hook_",
        "level": RiskLevel.MEDIUM,
        "check": lambda code: "__try" not in code and "try" not in code,
        "fix": "添加异常处理"
    },
    {
        "name": "关键操作需要日志",
        "pattern": r"(SendMessage|PostMoments|GetContacts)",
        "level": RiskLevel.MEDIUM,
        "check": lambda code: "spdlog" not in code and "Log" not in code,
        "fix": "添加 spdlog 日志记录"
    },
]


def review_file(file_path: str) -> List[Violation]:
    """审查单个文件"""
    violations = []
    path = Path(file_path)

    if not path.exists():
        return violations

    try:
        content = path.read_text(encoding='utf-8')
        lines = content.split('\n')
    except Exception as e:
        print(f"Error reading {file_path}: {e}")
        return violations

    for i, line in enumerate(lines, 1):
        for rule in RULES:
            if re.search(rule["pattern"], line, re.IGNORECASE):
                # 如果有额外检查
                if "check" in rule:
                    if rule["check"](content):
                        violations.append(Violation(
                            file=str(path),
                            line=i,
                            rule=rule["name"],
                            level=rule["level"],
                            code=line.strip(),
                            fix=rule.get("fix")
                        ))
                else:
                    violations.append(Violation(
                        file=str(path),
                        line=i,
                        rule=rule["name"],
                        level=rule["level"],
                        code=line.strip(),
                        fix=rule.get("fix")
                    ))

    return violations


def review_directory(dir_path: str) -> List[Violation]:
    """审查目录下所有文件"""
    violations = []
    path = Path(dir_path)

    for file in path.rglob("*.cpp"):
        violations.extend(review_file(str(file)))

    for file in path.rglob("*.h"):
        violations.extend(review_file(str(file)))

    return violations


def generate_report(violations: List[Violation]) -> str:
    """生成审查报告"""
    if not violations:
        return "# ✅ 代码审查通过\n\n未发现安全风险。"

    report = "# ❌ 代码审查未通过\n\n"

    # 按风险等级分组
    critical = [v for v in violations if v.level == RiskLevel.CRITICAL]
    high = [v for v in violations if v.level == RiskLevel.HIGH]
    medium = [v for v in violations if v.level == RiskLevel.MEDIUM]
    low = [v for v in violations if v.level == RiskLevel.LOW]

    if critical:
        report += "## 🔴 红线违规（必须修复）\n\n"
        for i, v in enumerate(critical, 1):
            report += f"{i}. **文件**: `{v.file}:{v.line}`\n"
            report += f"   **规则**: {v.rule}\n"
            report += f"   **代码**: `{v.code}`\n"
            if v.fix:
                report += f"   **修复**: {v.fix}\n"
            report += "\n"

    if high:
        report += "## 🟠 高风险警告（建议修复）\n\n"
        for i, v in enumerate(high, 1):
            report += f"{i}. **文件**: `{v.file}:{v.line}`\n"
            report += f"   **规则**: {v.rule}\n"
            report += f"   **代码**: `{v.code}`\n"
            if v.fix:
                report += f"   **修复**: {v.fix}\n"
            report += "\n"

    if medium:
        report += "## 🟡 中风险提示\n\n"
        for i, v in enumerate(medium, 1):
            report += f"{i}. **文件**: `{v.file}:{v.line}`\n"
            report += f"   **规则**: {v.rule}\n"
            report += "\n"

    # 统计
    report += "## 统计\n\n"
    report += f"- 🔴 红线: {len(critical)}\n"
    report += f"- 🟠 高风险: {len(high)}\n"
    report += f"- 🟡 中风险: {len(medium)}\n"
    report += f"- 🟢 低风险: {len(low)}\n"

    return report


def main():
    if len(sys.argv) < 2:
        print("Usage: python code_review.py <file_or_directory>")
        sys.exit(1)

    target = sys.argv[1]
    path = Path(target)

    if path.is_file():
        violations = review_file(target)
    elif path.is_dir():
        violations = review_directory(target)
    else:
        print(f"Error: {target} not found")
        sys.exit(1)

    report = generate_report(violations)
    print(report)

    # 如果有红线违规，返回非零退出码
    critical = [v for v in violations if v.level == RiskLevel.CRITICAL]
    if critical:
        sys.exit(1)


if __name__ == "__main__":
    main()
