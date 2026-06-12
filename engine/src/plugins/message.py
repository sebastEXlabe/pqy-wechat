"""消息插件"""

from typing import Callable, Dict, Any
from loguru import logger

from .base import Plugin
from ..executor.human_sim import human_sim
from ..executor.uia_bridge import uia_bridge
from ..monitor.risk_detector import risk_detector


class MessagePlugin(Plugin):
    """消息插件"""

    @property
    def name(self) -> str:
        return "message"

    @property
    def version(self) -> str:
        return "1.0.0"

    async def on_init(self):
        """初始化"""
        logger.info("MessagePlugin initialized")

    async def on_destroy(self):
        """销毁"""
        logger.info("MessagePlugin destroyed")

    def get_tools(self) -> Dict[str, Callable]:
        return {
            "wechat.send_message": self.send_message,
            "wechat.get_messages": self.get_messages,
        }

    async def send_message(self, to: str, content: str, type: str = "text", mention: str = None) -> Dict[str, Any]:
        """发送消息"""
        # 检查频率限制
        if not risk_detector.check_frequency("send_message"):
            raise Exception("Rate limited: too many messages")

        # 查找聊天
        if not uia_bridge.click_chat(to):
            raise Exception(f"Chat not found: {to}")

        # 类人延迟
        await human_sim.random_delay()

        # 输入消息
        if not uia_bridge.send_message(content):
            raise Exception("Failed to send message")

        logger.info(f"Message sent to {to}: {content[:20]}...")
        return {"success": True, "to": to}

    async def get_messages(self, chat: str, limit: int = 20, before: str = None) -> Dict[str, Any]:
        """获取消息"""
        # 查找聊天
        if not uia_bridge.click_chat(chat):
            raise Exception(f"Chat not found: {chat}")

        # 获取消息列表
        messages = uia_bridge.get_message_list(limit)

        return {
            "messages": messages[:limit],
            "total": len(messages),
        }
