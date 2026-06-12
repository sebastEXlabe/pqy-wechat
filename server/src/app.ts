import Fastify from "fastify";
import cors from "@fastify/cors";
import jwt from "@fastify/jwt";
import websocket from "@fastify/websocket";
import { setupRoutes } from "./api/routes/index.js";
import { setupWebSocket } from "./websocket/socket.js";
import { setupMCP } from "./mcp/server.js";
import { config } from "./config/index.js";

export async function buildApp() {
  const app = Fastify({
    logger: {
      level: "info",
      transport: {
        target: "pino-pretty",
        options: {
          colorize: true,
        },
      },
    },
  });

  // 注册插件
  await app.register(cors, {
    origin: ["http://localhost:5173", "file://"],
    credentials: true,
  });

  await app.register(jwt, {
    secret: config.jwt.secret,
  });

  await app.register(websocket);

  // 注册路由
  await setupRoutes(app);

  // 注册 WebSocket
  await setupWebSocket(app);

  // 注册 MCP Server（如果启用）
  if (config.mcp.enabled) {
    await setupMCP(app);
  }

  // 健康检查
  app.get("/health", async () => ({
    status: "ok",
    timestamp: new Date().toISOString(),
    version: "1.0.0",
  }));

  return app;
}
