import { FastifyInstance } from "fastify";
import { engine } from "../../bridge/engine.js";

export async function systemRoutes(app: FastifyInstance) {
  // 获取系统状态
  app.get("/status", {
    handler: async () => {
      try {
        const status = await engine.getStatus();
        return {
          connected: true,
          engine_status: "running",
          risk_level: status.risk_level || "low",
          user: status.user || null,
          wechat_version: status.wechat_version || "4.1.10.31",
          uptime: status.uptime || "0m",
        };
      } catch (error) {
        return {
          connected: false,
          engine_status: "error",
          risk_level: "unknown",
          user: null,
          wechat_version: "4.1.10.31",
          uptime: "0m",
        };
      }
    },
  });

  // 获取聊天列表
  app.get("/chats", {
    preHandler: [app.authenticate],
    schema: {
      querystring: {
        type: "object",
        properties: {
          limit: { type: "number", default: 50 },
          unread_only: { type: "boolean", default: false },
        },
      },
    },
    handler: async (request) => {
      const { limit, unread_only } = request.query as any;
      return engine.getChats(limit, unread_only);
    },
  });

  // 获取引擎信息
  app.get("/engine", {
    handler: async () => {
      return {
        host: process.env.ENGINE_HOST || "127.0.0.1",
        port: parseInt(process.env.ENGINE_PORT || "5555"),
        connected: engine.isConnected(),
      };
    },
  });
}
