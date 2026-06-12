# API 网关架构

## 技术栈

| 技术 | 版本 | 用途 |
|------|------|------|
| Node.js | 22 | 运行时 |
| Fastify | 5 | Web 框架 |
| Socket.IO | Latest | WebSocket |
| @modelcontextprotocol/sdk | Latest | MCP Server |
| JWT | - | 认证 |
| Pino | - | 日志 |

---

## 项目结构

```
server/
├── src/
│   ├── index.ts                 # 入口
│   ├── app.ts                   # Fastify 实例
│   │
│   ├── api/                     # REST API 路由
│   │   ├── routes/
│   │   │   ├── auth.ts          # 认证路由
│   │   │   ├── messages.ts      # 消息路由
│   │   │   ├── contacts.ts      # 联系人路由
│   │   │   ├── moments.ts       # 朋友圈路由
│   │   │   ├── chats.ts         # 聊天路由
│   │   │   └── system.ts        # 系统路由
│   │   │
│   │   ├── schemas/             # 请求/响应 Schema
│   │   │   ├── message.ts
│   │   │   ├── contact.ts
│   │   │   └── moments.ts
│   │   │
│   │   └── middleware/          # 中间件
│   │       ├── auth.ts          # 认证中间件
│   │       ├── rateLimit.ts     # 频率限制
│   │       └── validation.ts    # 请求验证
│   │
│   ├── mcp/                     # MCP Server
│   │   ├── server.ts            # MCP Server 实现
│   │   ├── tools.ts             # 工具定义
│   │   └── handlers/            # 工具处理器
│   │       ├── message.ts
│   │       ├── contact.ts
│   │       └── moments.ts
│   │
│   ├── services/                # 业务服务
│   │   ├── message.service.ts
│   │   ├── contact.service.ts
│   │   ├── moments.service.ts
│   │   └── ai.service.ts
│   │
│   ├── bridge/                  # 引擎桥接
│   │   ├── engine.ts            # ZeroMQ 客户端
│   │   └── protocol.ts          # 通信协议
│   │
│   ├── websocket/               # WebSocket
│   │   ├── socket.ts            # Socket.IO 实例
│   │   └── handlers/            # 事件处理器
│   │
│   ├── config/                  # 配置
│   │   ├── index.ts
│   │   ├── env.ts
│   │   └── constants.ts
│   │
│   └── utils/                   # 工具函数
│       ├── logger.ts
│       ├── crypto.ts
│       └── helpers.ts
│
├── tests/
├── tsconfig.json
└── package.json
```

---

## Fastify 应用配置

```typescript
// src/app.ts
import Fastify from "fastify";
import cors from "@fastify/cors";
import jwt from "@fastify/jwt";
import websocket from "@fastify/websocket";
import { setupRoutes } from "./api/routes";
import { setupWebSocket } from "./websocket/socket";
import { setupMCP } from "./mcp/server";

export async function buildApp() {
  const app = Fastify({
    logger: {
      level: "info",
      transport: {
        target: "pino-pretty",
      },
    },
  });

  // 注册插件
  await app.register(cors, {
    origin: ["http://localhost:5173", "file://"],
    credentials: true,
  });

  await app.register(jwt, {
    secret: process.env.JWT_SECRET || "pqy-secret-key",
  });

  await app.register(websocket);

  // 注册路由
  await setupRoutes(app);

  // 注册 WebSocket
  await setupWebSocket(app);

  // 注册 MCP Server
  await setupMCP(app);

  return app;
}
```

---

## REST API 路由

### 认证路由

```typescript
// src/api/routes/auth.ts
import { FastifyInstance } from "fastify";

export async function authRoutes(app: FastifyInstance) {
  // 登录
  app.post("/api/auth/login", {
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

      // 验证用户
      const user = await authService.validate(username, password);
      if (!user) {
        return reply.status(401).send({ error: "Invalid credentials" });
      }

      // 生成 JWT
      const token = app.jwt.sign({ id: user.id, role: user.role });

      return { token, user };
    },
  });

  // 登出
  app.post("/api/auth/logout", {
    preHandler: [app.authenticate],
    handler: async (request, reply) => {
      // 清理会话
      await authService.logout(request.user.id);
      return { success: true };
    },
  });

  // 获取当前用户
  app.get("/api/auth/me", {
    preHandler: [app.authenticate],
    handler: async (request) => {
      return request.user;
    },
  });
}
```

### 消息路由

```typescript
// src/api/routes/messages.ts
import { FastifyInstance } from "fastify";

export async function messageRoutes(app: FastifyInstance) {
  // 获取消息列表
  app.get("/api/messages", {
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
      return messageService.getMessages(chat, limit, before);
    },
  });

  // 发送消息
  app.post("/api/messages", {
    preHandler: [app.authenticate],
    schema: {
      body: {
        type: "object",
        properties: {
          to: { type: "string" },
          content: { type: "string" },
          type: { type: "string", enum: ["text", "image", "file"] },
        },
        required: ["to", "content"],
      },
    },
    handler: async (request) => {
      const { to, content, type } = request.body as any;
      return messageService.sendMessage(to, content, type);
    },
  });

  // 撤回消息
  app.delete("/api/messages/:id", {
    preHandler: [app.authenticate],
    handler: async (request) => {
      const { id } = request.params as any;
      return messageService.recallMessage(id);
    },
  });
}
```

### 联系人路由

```typescript
// src/api/routes/contacts.ts
import { FastifyInstance } from "fastify";

export async function contactRoutes(app: FastifyInstance) {
  // 获取联系人列表
  app.get("/api/contacts", {
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
      return contactService.getContacts(keyword, tag, limit);
    },
  });

  // 获取联系人详情
  app.get("/api/contacts/:id", {
    preHandler: [app.authenticate],
    handler: async (request) => {
      const { id } = request.params as any;
      return contactService.getContact(id);
    },
  });

  // 更新联系人
  app.put("/api/contacts/:id", {
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
      return contactService.updateContact(id, updates);
    },
  });
}
```

### 朋友圈路由

```typescript
// src/api/routes/moments.ts
import { FastifyInstance } from "fastify";

export async function momentsRoutes(app: FastifyInstance) {
  // 获取朋友圈动态
  app.get("/api/moments", {
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
      return momentsService.getMoments(contact, limit);
    },
  });

  // 发布朋友圈
  app.post("/api/moments", {
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
      return momentsService.postMoments(data);
    },
  });

  // 点赞
  app.post("/api/moments/:id/like", {
    preHandler: [app.authenticate],
    handler: async (request) => {
      const { id } = request.params as any;
      return momentsService.likeMoments(id);
    },
  });

  // 评论
  app.post("/api/moments/:id/comment", {
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
      return momentsService.commentMoments(id, content);
    },
  });
}
```

---

## 中间件

### 认证中间件

```typescript
// src/api/middleware/auth.ts
import { FastifyRequest, FastifyReply } from "fastify";

export async function authMiddleware(
  request: FastifyRequest,
  reply: FastifyReply
) {
  try {
    await request.jwtVerify();
  } catch (err) {
    reply.status(401).send({ error: "Unauthorized" });
  }
}
```

### 频率限制中间件

```typescript
// src/api/middleware/rateLimit.ts
import { FastifyRequest, FastifyReply } from "fastify";

const rateLimits = new Map<string, { count: number; resetTime: number }>();

export function rateLimit(maxRequests: number, windowMs: number) {
  return async (request: FastifyRequest, reply: FastifyReply) => {
    const key = `${request.ip}:${request.url}`;
    const now = Date.now();

    const limit = rateLimits.get(key);

    if (!limit || now > limit.resetTime) {
      rateLimits.set(key, { count: 1, resetTime: now + windowMs });
      return;
    }

    if (limit.count >= maxRequests) {
      reply.status(429).send({ error: "Too Many Requests" });
      return;
    }

    limit.count++;
  };
}

// 预定义的限制
export const messageRateLimit = rateLimit(10, 60 * 1000); // 10/分钟
export const momentsRateLimit = rateLimit(5, 60 * 60 * 1000); // 5/小时
```

---

## 引擎桥接

### ZeroMQ 客户端

```typescript
// src/bridge/engine.ts
import * as zmq from "zeromq";
import { v4 as uuidv4 } from "uuid";

interface EngineRequest {
  id: string;
  tool: string;
  args: any;
  timestamp: number;
}

interface EngineResponse {
  id: string;
  result?: any;
  error?: string;
}

export class EngineBridge {
  private sock: zmq.Request;
  private connected: boolean = false;

  constructor(private host: string = "127.0.0.1", private port: number = 5555) {
    this.sock = new zmq.Request();
  }

  async connect() {
    this.sock.connect(`tcp://${this.host}:${this.port}`);
    this.connected = true;
    console.log(`Connected to engine at ${this.host}:${this.port}`);
  }

  async disconnect() {
    this.sock.close();
    this.connected = false;
  }

  async callTool(tool: string, args: any): Promise<any> {
    if (!this.connected) {
      throw new Error("Engine not connected");
    }

    const request: EngineRequest = {
      id: uuidv4(),
      tool,
      args,
      timestamp: Date.now(),
    };

    try {
      await this.sock.send(JSON.stringify(request));
      const [reply] = await this.sock.receive();
      const response: EngineResponse = JSON.parse(reply.toString());

      if (response.error) {
        throw new Error(response.error);
      }

      return response.result;
    } catch (error) {
      console.error("Engine call failed:", error);
      throw error;
    }
  }

  // 便捷方法
  async sendMessage(to: string, content: string, type: string = "text") {
    return this.callTool("wechat.send_message", { to, content, type });
  }

  async getMessages(chat: string, limit: number = 20) {
    return this.callTool("wechat.get_messages", { chat, limit });
  }

  async getContacts(keyword?: string) {
    return this.callTool("wechat.get_contacts", { keyword });
  }

  async postMoments(content: string, images?: string[]) {
    return this.callTool("wechat.post_moments", { content, images });
  }
}

// 单例
export const engine = new EngineBridge();
```

---

## WebSocket

### Socket.IO 配置

```typescript
// src/websocket/socket.ts
import { FastifyInstance } from "fastify";
import { Server } from "socket.io";

let io: Server;

export function setupWebSocket(app: FastifyInstance) {
  io = new Server(app.server, {
    cors: {
      origin: ["http://localhost:5173", "file://"],
      credentials: true,
    },
  });

  // 认证中间件
  io.use(async (socket, next) => {
    try {
      const token = socket.handshake.auth.token;
      const user = await app.jwt.verify(token);
      socket.data.user = user;
      next();
    } catch (err) {
      next(new Error("Authentication error"));
    }
  });

  // 连接处理
  io.on("connection", (socket) => {
    console.log(`User connected: ${socket.data.user.id}`);

    // 加入用户房间
    socket.join(`user:${socket.data.user.id}`);

    socket.on("disconnect", () => {
      console.log(`User disconnected: ${socket.data.user.id}`);
    });
  });

  return io;
}

// 发送事件到客户端
export function emitToUser(userId: string, event: string, data: any) {
  io.to(`user:${userId}`).emit(event, data);
}

// 广播到所有客户端
export function broadcast(event: string, data: any) {
  io.emit(event, data);
}
```

### 事件类型

```typescript
// 事件定义
export const WS_EVENTS = {
  // 消息事件
  MESSAGE_NEW: "message.new",
  MESSAGE_RECALL: "message.recall",

  // 联系人事件
  CONTACT_UPDATE: "contact.update",
  CONTACT_ADD: "contact.add",

  // 朋友圈事件
  MOMENTS_NEW: "moments.new",
  MOMENTS_LIKE: "moments.like",
  MOMENTS_COMMENT: "moments.comment",

  // 系统事件
  SYSTEM_STATUS: "system.status",
  ENGINE_CONNECTED: "engine.connected",
  ENGINE_DISCONNECTED: "engine.disconnected",

  // 风控事件
  RISK_ALERT: "risk.alert",
  RISK_LEVEL_CHANGE: "risk.level_change",
};
```

---

## MCP Server 集成

```typescript
// src/mcp/server.ts
import { FastifyInstance } from "fastify";
import { WeChatMCPServer } from "./mcp-server";

let mcpServer: WeChatMCPServer;

export async function setupMCP(app: FastifyInstance) {
  // HTTP SSE 传输
  app.get("/mcp/sse", async (request, reply) => {
    reply.raw.writeHead(200, {
      "Content-Type": "text/event-stream",
      "Cache-Control": "no-cache",
      Connection: "keep-alive",
    });

    // MCP SSE 处理逻辑
    // ...
  });

  // stdio 传输（用于 Claude Desktop）
  if (process.argv.includes("--mcp-stdio")) {
    mcpServer = new WeChatMCPServer();
    await mcpServer.start();
  }
}
```

---

## 配置管理

```typescript
// src/config/index.ts
import dotenv from "dotenv";

dotenv.config();

export const config = {
  // 服务器
  server: {
    host: process.env.HOST || "127.0.0.1",
    port: parseInt(process.env.PORT || "3000"),
  },

  // JWT
  jwt: {
    secret: process.env.JWT_SECRET || "pqy-secret-key",
    expiresIn: process.env.JWT_EXPIRES_IN || "7d",
  },

  // 引擎
  engine: {
    host: process.env.ENGINE_HOST || "127.0.0.1",
    port: parseInt(process.env.ENGINE_PORT || "5555"),
  },

  // MCP
  mcp: {
    enabled: process.env.MCP_ENABLED !== "false",
    transport: process.env.MCP_TRANSPORT || "stdio",
  },

  // 频率限制
  rateLimit: {
    message: { max: 10, window: 60 * 1000 },
    moments: { max: 5, window: 60 * 60 * 1000 },
  },
};
```

---

## 启动脚本

```json
// package.json
{
  "scripts": {
    "dev": "tsx watch src/index.ts",
    "build": "tsc",
    "start": "node dist/index.js",
    "start:mcp": "node dist/mcp/server.js --mcp-stdio",
    "test": "jest"
  }
}
```

---

## 下一步

- [自动化引擎](automation-engine.md) - Python 引擎设计
- [MCP 协议](mcp-protocol.md) - MCP 工具定义
- [前端架构](frontend.md) - Electron 前端设计
