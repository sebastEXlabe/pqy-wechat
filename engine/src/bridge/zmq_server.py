"""ZeroMQ 服务器"""

import asyncio
import json
from typing import Callable, Dict, Any
import zmq
import zmq.asyncio
from loguru import logger


class ZMQServer:
    """ZeroMQ 服务器"""

    def __init__(self, host: str = "127.0.0.1", port: int = 5555):
        self.host = host
        self.port = port
        self.context = zmq.asyncio.Context()
        self.socket = self.context.socket(zmq.REP)
        self.handlers: Dict[str, Callable] = {}
        self._running = False

    def register_handler(self, tool: str, handler: Callable):
        """注册工具处理器"""
        self.handlers[tool] = handler
        logger.info(f"Registered tool handler: {tool}")

    async def start(self):
        """启动服务器"""
        self.socket.bind(f"tcp://{self.host}:{self.port}")
        self._running = True
        logger.info(f"ZMQ Server listening on {self.host}:{self.port}")

        while self._running:
            try:
                # 接收请求
                message = await self.socket.recv()
                request = json.loads(message)
                logger.debug(f"Received request: {request.get('tool')}")

                # 处理请求
                response = await self._handle_request(request)

                # 发送响应
                await self.socket.send(json.dumps(response, ensure_ascii=False).encode())

            except zmq.ZMQError as e:
                if e.errno == zmq.ETERM:
                    break
                logger.error(f"ZMQ Error: {e}")
            except json.JSONDecodeError as e:
                logger.error(f"JSON decode error: {e}")
                error_response = {"id": "", "error": f"Invalid JSON: {e}"}
                await self.socket.send(json.dumps(error_response).encode())
            except Exception as e:
                logger.error(f"Error handling request: {e}")
                error_response = {"id": request.get("id", ""), "error": str(e)}
                await self.socket.send(json.dumps(error_response).encode())

    async def _handle_request(self, request: dict) -> dict:
        """处理请求"""
        tool = request.get("tool")
        args = request.get("args", {})
        request_id = request.get("id", "")

        handler = self.handlers.get(tool)
        if not handler:
            return {
                "id": request_id,
                "error": f"Unknown tool: {tool}"
            }

        try:
            # 调用处理器
            if asyncio.iscoroutinefunction(handler):
                result = await handler(**args)
            else:
                result = handler(**args)

            return {
                "id": request_id,
                "result": result
            }
        except Exception as e:
            logger.error(f"Handler error for {tool}: {e}")
            return {
                "id": request_id,
                "error": str(e)
            }

    async def stop(self):
        """停止服务器"""
        self._running = False
        self.socket.close()
        self.context.term()
        logger.info("ZMQ Server stopped")


# 全局实例
zmq_server = ZMQServer()
