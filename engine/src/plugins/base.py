"""插件基类"""

from abc import ABC, abstractmethod
from typing import Callable, Dict, Any


class Plugin(ABC):
    """插件基类"""

    @property
    @abstractmethod
    def name(self) -> str:
        """插件名称"""
        pass

    @property
    @abstractmethod
    def version(self) -> str:
        """插件版本"""
        pass

    @abstractmethod
    async def on_init(self):
        """初始化"""
        pass

    @abstractmethod
    async def on_destroy(self):
        """销毁"""
        pass

    @abstractmethod
    def get_tools(self) -> Dict[str, Callable]:
        """获取工具列表"""
        pass
