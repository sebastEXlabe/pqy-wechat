"""联系人插件"""

from typing import Callable, Dict, Any, List, Optional
from loguru import logger

from .base import Plugin
from ..executor.uia_bridge import uia_bridge


class ContactPlugin(Plugin):
    """联系人插件"""

    @property
    def name(self) -> str:
        return "contact"

    @property
    def version(self) -> str:
        return "1.0.0"

    async def on_init(self):
        """初始化"""
        logger.info("ContactPlugin initialized")

    async def on_destroy(self):
        """销毁"""
        logger.info("ContactPlugin destroyed")

    def get_tools(self) -> Dict[str, Callable]:
        return {
            "wechat.get_contacts": self.get_contacts,
            "wechat.get_groups": self.get_groups,
        }

    async def get_contacts(
        self,
        keyword: Optional[str] = None,
        tag: Optional[str] = None,
        limit: int = 50
    ) -> Dict[str, Any]:
        """获取联系人列表"""
        contacts = uia_bridge.get_contacts(keyword)

        return {
            "contacts": contacts[:limit],
            "total": len(contacts),
        }

    async def get_groups(
        self,
        keyword: Optional[str] = None,
        limit: int = 50
    ) -> Dict[str, Any]:
        """获取群聊列表"""
        groups = uia_bridge.get_groups(keyword)

        return {
            "groups": groups[:limit],
            "total": len(groups),
        }
