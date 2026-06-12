import { FastifyInstance } from "fastify";
import { authRoutes } from "./auth.js";
import { messageRoutes } from "./messages.js";
import { contactRoutes } from "./contacts.js";
import { momentsRoutes } from "./moments.js";
import { systemRoutes } from "./system.js";

export async function setupRoutes(app: FastifyInstance) {
  // 认证路由
  await app.register(authRoutes, { prefix: "/api/auth" });

  // 消息路由
  await app.register(messageRoutes, { prefix: "/api/messages" });

  // 联系人路由
  await app.register(contactRoutes, { prefix: "/api/contacts" });

  // 朋友圈路由
  await app.register(momentsRoutes, { prefix: "/api/moments" });

  // 系统路由
  await app.register(systemRoutes, { prefix: "/api/system" });
}
