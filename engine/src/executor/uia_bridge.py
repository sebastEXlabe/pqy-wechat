"""UIAutomation 桥接器"""

from typing import Optional, List, Dict, Any
from loguru import logger


class UIAutomationBridge:
    """UIAutomation 桥接器"""

    def __init__(self):
        self.wechat_window = None
        self._uia = None

    def _init_uia(self):
        """初始化 UIAutomation"""
        if self._uia is None:
            try:
                import uiautomation as auto
                self._uia = auto
            except ImportError:
                logger.error("uiautomation not available")
                raise

    def find_wechat_window(self) -> bool:
        """查找微信窗口"""
        self._init_uia()

        try:
            self.wechat_window = self._uia.WindowControl(
                searchDepth=1,
                Name="微信"
            )
            return self.wechat_window.Exists(maxSearchSeconds=3)
        except Exception as e:
            logger.error(f"Failed to find WeChat window: {e}")
            return False

    def click_chat(self, name: str) -> bool:
        """点击聊天"""
        if not self.wechat_window:
            if not self.find_wechat_window():
                return False

        try:
            chat_item = self.wechat_window.ListItemControl(Name=name)
            if chat_item.Exists(maxSearchSeconds=2):
                chat_item.Click()
                return True
            return False
        except Exception as e:
            logger.error(f"Failed to click chat {name}: {e}")
            return False

    def send_message(self, text: str) -> bool:
        """发送消息"""
        if not self.wechat_window:
            if not self.find_wechat_window():
                return False

        try:
            edit = self.wechat_window.EditControl(Name="输入")
            if not edit.Exists(maxSearchSeconds=2):
                return False

            edit.Click()
            self._uia.SendKeys(text)
            self._uia.SendKeys("{ENTER}")
            return True
        except Exception as e:
            logger.error(f"Failed to send message: {e}")
            return False

    def get_message_list(self, limit: int = 20) -> List[Dict[str, Any]]:
        """获取消息列表"""
        if not self.wechat_window:
            if not self.find_wechat_window():
                return []

        try:
            msg_list = self.wechat_window.ListControl(Name="消息")
            if not msg_list.Exists():
                return []

            messages = []
            for item in msg_list.GetChildren()[:limit]:
                messages.append({
                    "content": item.Name,
                    "type": "text"
                })
            return messages
        except Exception as e:
            logger.error(f"Failed to get messages: {e}")
            return []

    def get_contacts(self, keyword: Optional[str] = None) -> List[Dict[str, Any]]:
        """获取联系人列表"""
        # TODO: 实现联系人获取
        return []

    def get_groups(self, keyword: Optional[str] = None) -> List[Dict[str, Any]]:
        """获取群聊列表"""
        # TODO: 实现群聊获取
        return []

    def get_moments(self, contact: Optional[str] = None) -> List[Dict[str, Any]]:
        """获取朋友圈动态"""
        # TODO: 实现朋友圈获取
        return []

    def open_moments(self) -> bool:
        """打开朋友圈"""
        # TODO: 实现打开朋友圈
        return False

    def post_moments(self, content: str, images: Optional[List[str]] = None) -> bool:
        """发布朋友圈"""
        # TODO: 实现发布朋友圈
        return False

    def like_moments(self, moment_id: str) -> bool:
        """点赞朋友圈"""
        # TODO: 实现点赞
        return False

    def comment_moments(self, moment_id: str, content: str) -> bool:
        """评论朋友圈"""
        # TODO: 实现评论
        return False


# 全局实例
uia_bridge = UIAutomationBridge()
