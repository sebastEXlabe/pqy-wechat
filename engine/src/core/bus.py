"""事件总线"""

import asyncio
from typing import Callable, Dict, List, Any
from loguru import logger

from .events import Event


class EventBus:
    """事件总线"""

    def __init__(self):
        self._handlers: Dict[str, List[Callable]] = {}

    def on(self, event_type: str, handler: Callable):
        """注册事件处理器"""
        if event_type not in self._handlers:
            self._handlers[event_type] = []
        self._handlers[event_type].append(handler)
        logger.debug(f"Registered handler for event: {event_type}")

    def off(self, event_type: str, handler: Callable):
        """注销事件处理器"""
        if event_type in self._handlers:
            self._handlers[event_type].remove(handler)

    async def emit(self, event: Event):
        """触发事件"""
        handlers = self._handlers.get(event.type, [])
        if not handlers:
            return

        logger.debug(f"Emitting event: {event.type} to {len(handlers)} handlers")

        for handler in handlers:
            try:
                if asyncio.iscoroutinefunction(handler):
                    await handler(event)
                else:
                    handler(event)
            except Exception as e:
                logger.error(f"Event handler error for {event.type}: {e}")

    def get_handlers(self, event_type: str) -> List[Callable]:
        """获取事件处理器列表"""
        return self._handlers.get(event_type, [])


# 全局事件总线
event_bus = EventBus()
