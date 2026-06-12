"""PQY 自动化引擎主入口"""

import asyncio
import signal
import sys
from pathlib import Path
from loguru import logger

# 配置日志
logger.remove()
logger.add(sys.stderr, level="INFO")
logger.add("logs/engine.log", rotation="10 MB", level="DEBUG")

from .bridge.zmq_server import zmq_server
from .plugins.message import MessagePlugin
from .plugins.contact import ContactPlugin
from .plugins.moments import MomentsPlugin


async def main():
    """主函数"""
    logger.info("=" * 60)
    logger.info("PQY 微信自动化引擎启动中...")
    logger.info("=" * 60)

    # 注册插件
    plugins = [
        MessagePlugin(),
        ContactPlugin(),
        MomentsPlugin(),
    ]

    for plugin in plugins:
        await plugin.on_init()
        for tool_name, handler in plugin.get_tools().items():
            zmq_server.register_handler(tool_name, handler)
            logger.info(f"Registered tool: {tool_name}")

    logger.info(f"Total tools registered: {len(zmq_server.handlers)}")
    logger.info("=" * 60)

    # 启动服务器
    try:
        await zmq_server.start()
    except KeyboardInterrupt:
        logger.info("Received interrupt signal, shutting down...")
    except Exception as e:
        logger.error(f"Error: {e}")
    finally:
        # 清理
        logger.info("Cleaning up...")
        for plugin in plugins:
            await plugin.on_destroy()
        await zmq_server.stop()
        logger.info("Engine stopped")


def run():
    """运行引擎"""
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        logger.info("Engine stopped by user")
    except Exception as e:
        logger.error(f"Engine crashed: {e}")
        sys.exit(1)


if __name__ == "__main__":
    run()
