import { FastifyInstance } from "fastify";
import { engine } from "../../bridge/engine.js";

export async function momentsRoutes(app: FastifyInstance) {
  // 获取朋友圈动态
  app.get("/", {
    preHandler: [app.authenticate],
    schema: {
      querystring: {
        type: "object",
        properties: {
          contact: { type: "string" },
          limit: { type: "number", default: 20 },
        },
      },
    },
    handler: async (request) => {
      const { contact, limit } = request.query as any;
      return engine.getMoments(contact, limit);
    },
  });

  // 发布朋友圈
  app.post("/", {
    preHandler: [app.authenticate],
    schema: {
      body: {
        type: "object",
        properties: {
          content: { type: "string" },
          images: { type: "array", items: { type: "string" } },
          visibility: { type: "string", enum: ["all", "partial", "private"] },
          mention: { type: "array", items: { type: "string" } },
        },
        required: ["content"],
      },
    },
    handler: async (request) => {
      const data = request.body as any;
      return engine.postMoments(data.content, data.images, data.visibility, data.mention);
    },
  });

  // 点赞
  app.post("/:id/like", {
    preHandler: [app.authenticate],
    handler: async (request) => {
      const { id } = request.params as any;
      return engine.likeMoments(id);
    },
  });

  // 评论
  app.post("/:id/comment", {
    preHandler: [app.authenticate],
    schema: {
      body: {
        type: "object",
        properties: {
          content: { type: "string" },
        },
        required: ["content"],
      },
    },
    handler: async (request) => {
      const { id } = request.params as any;
      const { content } = request.body as any;
      return engine.commentMoments(id, content);
    },
  });
}
