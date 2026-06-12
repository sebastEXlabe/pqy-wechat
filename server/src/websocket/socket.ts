import { FastifyInstance } from "fastify";
import { Server } from "socket.io";

let io: Server;

export async function setupWebSocket(app: FastifyInstance) {
  io = new Server(app.server, {
    cors: {
      origin: process.env.WS_CORS || "http://localhost:5173",
      credentials: true,
    },
  });

  // 认证中间件
  io.use(async (socket, next) => {
    try {
      const token = socket.handshake.auth.token;
      if (!token) {
        // 允许无认证连接（用于状态监控）
        socket.data.user = { id: "anonymous", role: "viewer" };
        return next();
      }

      const user = await app.jwt.verify(token);
      socket.data.user = user;
      next();
    } catch (err) {
      // 允许连接但限制权限
      socket.data.user = { id: "anonymous", role: "viewer" };
      next();
    }
  });

  // 连接处理
  io.on("connection", (socket) => {
    console.log(`Client connected: ${socket.id} (user: ${socket.data.user?.id})`);

    // 加入用户房间
    if (socket.data.user?.id !== "anonymous") {
      socket.join(`user:${socket.data.user.id}`);
    }
    socket.join("broadcast");

    // 断开连接
    socket.on("disconnect", () => {
      console.log(`Client disconnected: ${socket.id}`);
    });

    // 订阅事件
    socket.on("subscribe", (events: string[]) => {
      events.forEach((event) => socket.join(`event:${event}`));
    });

    // 取消订阅
    socket.on("unsubscribe", (events: string[]) => {
      events.forEach((event) => socket.leave(`event:${event}`));
    });
  });

  return io;
}

// 发送事件到指定用户
export function emitToUser(userId: string, event: string, data: any) {
  if (io) {
    io.to(`user:${userId}`).emit(event, data);
  }
}

// 广播到所有客户端
export function broadcast(event: string, data: any) {
  if (io) {
    io.emit(event, data);
  }
}

// 发送到订阅了特定事件的客户端
export function emitToSubscribers(event: string, data: any) {
  if (io) {
    io.to(`event:${event}`).emit(event, data);
  }
}

// 获取 Socket.IO 实例
export function getIO(): Server {
  return io;
}
