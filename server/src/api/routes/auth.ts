import { FastifyInstance } from "fastify";

export async function authRoutes(app: FastifyInstance) {
  // 登录
  app.post("/login", {
    schema: {
      body: {
        type: "object",
        properties: {
          username: { type: "string" },
          password: { type: "string" },
        },
        required: ["username", "password"],
      },
    },
    handler: async (request, reply) => {
      const { username, password } = request.body as any;

      // 简单验证（生产环境应使用数据库）
      if (username === "admin" && password === "pqy2026") {
        const token = app.jwt.sign({
          id: "1",
          username: "admin",
          role: "admin",
        });

        return {
          token,
          user: {
            id: "1",
            username: "admin",
            role: "admin",
          },
        };
      }

      return reply.status(401).send({
        error: "Invalid credentials",
      });
    },
  });

  // 登出
  app.post("/logout", {
    preHandler: [app.authenticate],
    handler: async () => {
      return { success: true };
    },
  });

  // 获取当前用户
  app.get("/me", {
    preHandler: [app.authenticate],
    handler: async (request) => {
      return request.user;
    },
  });
}
