"""风控检测器"""

from enum import Enum
from typing import Optional, Dict, Any
from datetime import datetime, timedelta
from loguru import logger


class RiskLevel(Enum):
    """风控等级"""
    LOW = "low"
    MEDIUM = "medium"
    HIGH = "high"
    CRITICAL = "critical"


class RiskDetector:
    """风控检测器"""

    def __init__(self):
        self.current_level = RiskLevel.LOW
        self.counters: Dict[str, Dict[str, Any]] = {}
        self.limits = {
            "send_message": {"max_per_minute": 5, "max_per_hour": 50},
            "post_moments": {"max_per_day": 2},
            "like_moments": {"max_per_minute": 2},
        }

    def check_frequency(self, action: str) -> bool:
        """检查频率限制"""
        now = datetime.now()
        limit = self.limits.get(action)

        if not limit:
            return True

        # 检查分钟限制
        if "max_per_minute" in limit:
            key = f"{action}_minute"
            if key not in self.counters:
                self.counters[key] = {"count": 0, "reset_time": now + timedelta(minutes=1)}

            counter = self.counters[key]
            if now > counter["reset_time"]:
                counter["count"] = 0
                counter["reset_time"] = now + timedelta(minutes=1)

            if counter["count"] >= limit["max_per_minute"]:
                logger.warning(f"Rate limited: {action} ({counter['count']}/{limit['max_per_minute']})")
                self.current_level = RiskLevel.MEDIUM
                return False

            counter["count"] += 1

        # 检查小时限制
        if "max_per_hour" in limit:
            key = f"{action}_hour"
            if key not in self.counters:
                self.counters[key] = {"count": 0, "reset_time": now + timedelta(hours=1)}

            counter = self.counters[key]
            if now > counter["reset_time"]:
                counter["count"] = 0
                counter["reset_time"] = now + timedelta(hours=1)

            if counter["count"] >= limit["max_per_hour"]:
                logger.warning(f"Rate limited: {action} ({counter['count']}/{limit['max_per_hour']})")
                self.current_level = RiskLevel.HIGH
                return False

            counter["count"] += 1

        # 检查天限制
        if "max_per_day" in limit:
            key = f"{action}_day"
            if key not in self.counters:
                self.counters[key] = {"count": 0, "reset_time": now + timedelta(days=1)}

            counter = self.counters[key]
            if now > counter["reset_time"]:
                counter["count"] = 0
                counter["reset_time"] = now + timedelta(days=1)

            if counter["count"] >= limit["max_per_day"]:
                logger.warning(f"Rate limited: {action} ({counter['count']}/{limit['max_per_day']})")
                self.current_level = RiskLevel.HIGH
                return False

            counter["count"] += 1

        return True

    def detect_risk_signals(self) -> Optional[str]:
        """检测风控信号"""
        # TODO: 实现风控信号检测
        return None

    def reset(self):
        """重置风控状态"""
        self.current_level = RiskLevel.LOW
        self.counters.clear()

    def get_status(self) -> Dict[str, Any]:
        """获取风控状态"""
        return {
            "level": self.current_level.value,
            "counters": {
                k: v["count"] for k, v in self.counters.items()
            },
        }


# 全局实例
risk_detector = RiskDetector()
