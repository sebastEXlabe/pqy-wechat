import { FastifyInstance } from "fastify";
import { engine } from "../../bridge/engine.js";

export async function messageRoutes(app: FastifyInstance) {
  // 获取消息列表
  app.get("/", {
    preHandler: [app.authenticate],
    schema: {
      querystring: {
        type: "object",
        properties: {
          chat: { type: "string" },
          limit: { type: "number", default: 20 },
          before: { type: "string" },
        },
      },
    },
    handler: async (request) => {
      const { chat, limit, before } = request.query as any;
      return engine.getMessages(chat, limit, before);
    },
  });

  // 发送消息
  app.post("/", {
    preHandler: [app.authenticate],
    schema: {
      body: {
        type: "object",
        properties: {
          to: { type: "string" },
          content: { type: "string" },
          type: { type: "string", enum: ["text", "image", "file", "link"] },
          mention: { type: "string" },
        },
        required: ["to", "content"],
      },
    },
    handler: async (request) => {
      const { to, content, type, mention } = request.body as any;
      return engine.sendMessage(to, content, type);
    },
  });

  // 撤回消息
  app.delete("/:id", {
    preHandler: [app.authenticate],
    handler: async (request) => {
      const { id } = request.params as any;
      // TODO: 实现撤回功能
      return { success: true, message_id: id };
    },
  });
}
