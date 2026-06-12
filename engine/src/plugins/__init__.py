"""插件系统"""

from .base import Plugin
from .message import MessagePlugin
from .contact import ContactPlugin
from .moments import MomentsPlugin

__all__ = [
    "Plugin",
    "MessagePlugin",
    "ContactPlugin",
    "MomentsPlugin",
]
