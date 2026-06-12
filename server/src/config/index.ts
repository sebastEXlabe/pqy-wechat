import dotenv from "dotenv";

dotenv.config();

export const config = {
  // 服务器配置
  server: {
    host: process.env.HOST || "127.0.0.1",
    port: parseInt(process.env.PORT || "3000"),
  },

  // JWT 配置
  jwt: {
    secret: process.env.JWT_SECRET || "pqy-wechat-secret-key-2026",
    expiresIn: process.env.JWT_EXPIRES_IN || "7d",
  },

  // 引擎配置
  engine: {
    host: process.env.ENGINE_HOST || "127.0.0.1",
    port: parseInt(process.env.ENGINE_PORT || "5555"),
    timeout: parseInt(process.env.ENGINE_TIMEOUT || "30000"),
  },

  // MCP 配置
  mcp: {
    enabled: process.env.MCP_ENABLED !== "false",
    transport: process.env.MCP_TRANSPORT || "stdio", // stdio | sse
    port: parseInt(process.env.MCP_PORT || "3001"),
  },

  // 频率限制
  rateLimit: {
    message: {
      maxPerMinute: parseInt(process.env.RATE_MESSAGE_MINUTE || "10"),
      maxPerHour: parseInt(process.env.RATE_MESSAGE_HOUR || "100"),
    },
    moments: {
      maxPerDay: parseInt(process.env.RATE_MOMENTS_DAY || "5"),
    },
    like: {
      maxPerMinute: parseInt(process.env.RATE_LIKE_MINUTE || "5"),
    },
  },

  // WebSocket
  websocket: {
    cors: process.env.WS_CORS || "http://localhost:5173",
  },
};
