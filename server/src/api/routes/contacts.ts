import { FastifyInstance } from "fastify";
import { engine } from "../../bridge/engine.js";

export async function contactRoutes(app: FastifyInstance) {
  // 获取联系人列表
  app.get("/", {
    preHandler: [app.authenticate],
    schema: {
      querystring: {
        type: "object",
        properties: {
          keyword: { type: "string" },
          tag: { type: "string" },
          limit: { type: "number", default: 50 },
        },
      },
    },
    handler: async (request) => {
      const { keyword, tag, limit } = request.query as any;
      return engine.getContacts(keyword, tag, limit);
    },
  });

  // 获取群聊列表
  app.get("/groups", {
    preHandler: [app.authenticate],
    schema: {
      querystring: {
        type: "object",
        properties: {
          keyword: { type: "string" },
          limit: { type: "number", default: 50 },
        },
      },
    },
    handler: async (request) => {
      const { keyword, limit } = request.query as any;
      return engine.getGroups(keyword, limit);
    },
  });

  // 获取联系人详情
  app.get("/:id", {
    preHandler: [app.authenticate],
    handler: async (request) => {
      const { id } = request.params as any;
      // TODO: 实现获取联系人详情
      return { id, nickname: "Unknown" };
    },
  });

  // 更新联系人
  app.put("/:id", {
    preHandler: [app.authenticate],
    schema: {
      body: {
        type: "object",
        properties: {
          remark: { type: "string" },
          tags: { type: "array", items: { type: "string" } },
        },
      },
    },
    handler: async (request) => {
      const { id } = request.params as any;
      const updates = request.body as any;
      // TODO: 实现更新联系人
      return { success: true, id, ...updates };
    },
  });
}
