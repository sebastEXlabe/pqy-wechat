"""朋友圈插件"""

from typing import Callable, Dict, Any, List, Optional
from loguru import logger

from .base import Plugin
from ..executor.human_sim import human_sim
from ..executor.uia_bridge import uia_bridge
from ..monitor.risk_detector import risk_detector


class MomentsPlugin(Plugin):
    """朋友圈插件"""

    @property
    def name(self) -> str:
        return "moments"

    @property
    def version(self) -> str:
        return "1.0.0"

    async def on_init(self):
        """初始化"""
        logger.info("MomentsPlugin initialized")

    async def on_destroy(self):
        """销毁"""
        logger.info("MomentsPlugin destroyed")

    def get_tools(self) -> Dict[str, Callable]:
        return {
            "wechat.get_moments": self.get_moments,
            "wechat.post_moments": self.post_moments,
            "wechat.like_moments": self.like_moments,
            "wechat.comment_moments": self.comment_moments,
        }

    async def get_moments(
        self,
        contact: Optional[str] = None,
        limit: int = 20
    ) -> Dict[str, Any]:
        """获取朋友圈动态"""
        moments = uia_bridge.get_moments(contact)

        return {
            "moments": moments[:limit],
            "total": len(moments),
        }

    async def post_moments(
        self,
        content: str,
        images: Optional[List[str]] = None,
        visibility: str = "all",
        mention: Optional[List[str]] = None
    ) -> Dict[str, Any]:
        """发布朋友圈"""
        # 检查频率限制
        if not risk_detector.check_frequency("post_moments"):
            raise Exception("Rate limited: too many moments today")

        # 打开朋友圈
        if not uia_bridge.open_moments():
            raise Exception("Failed to open moments")

        # 类人延迟
        await human_sim.random_delay()

        # 发布朋友圈
        if not uia_bridge.post_moments(content, images):
            raise Exception("Failed to post moments")

        logger.info(f"Moments posted: {content[:20]}...")
        return {"success": True}

    async def like_moments(self, moment_id: str) -> Dict[str, Any]:
        """点赞朋友圈"""
        # 检查频率限制
        if not risk_detector.check_frequency("like_moments"):
            raise Exception("Rate limited: too many likes")

        # 点赞
        if not uia_bridge.like_moments(moment_id):
            raise Exception("Failed to like moments")

        return {"success": True, "moment_id": moment_id}

    async def comment_moments(self, moment_id: str, content: str) -> Dict[str, Any]:
        """评论朋友圈"""
        # 评论
        if not uia_bridge.comment_moments(moment_id, content):
            raise Exception("Failed to comment moments")

        return {"success": True, "moment_id": moment_id, "content": content}
