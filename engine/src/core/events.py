"""事件定义"""

from dataclasses import dataclass, field
from typing import Any, Optional
from datetime import datetime


@dataclass
class Event:
    """事件基类"""
    type: str
    timestamp: datetime = field(default_factory=datetime.now)
    data: Any = None
    source: str = ""


@dataclass
class MessageEvent(Event):
    """消息事件"""
    chat_id: str = ""
    sender: str = ""
    content: str = ""
    message_type: str = "text"


@dataclass
class ContactEvent(Event):
    """联系人事件"""
    contact_id: str = ""
    nickname: str = ""


@dataclass
class MomentsEvent(Event):
    """朋友圈事件"""
    moment_id: str = ""
    author: str = ""


@dataclass
class RiskEvent(Event):
    """风控事件"""
    risk_level: str = "low"
    description: str = ""
