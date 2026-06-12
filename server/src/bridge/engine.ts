import * as zmq from "zeromq";
import { v4 as uuidv4 } from "uuid";
import { config } from "../config/index.js";

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
  private sock: zmq.Request | null = null;
  private _connected: boolean = false;
  private host: string;
  private port: number;
  private timeout: number;

  constructor() {
    this.host = config.engine.host;
    this.port = config.engine.port;
    this.timeout = config.engine.timeout;
  }

  isConnected(): boolean {
    return this._connected;
  }

  async connect() {
    try {
      this.sock = new zmq.Request({
        receiveTimeout: this.timeout,
        sendTimeout: this.timeout,
      });
      this.sock.connect(`tcp://${this.host}:${this.port}`);
      this._connected = true;
      console.log(`Connected to engine at ${this.host}:${this.port}`);
    } catch (error) {
      console.error("Failed to connect to engine:", error);
      this._connected = false;
      throw error;
    }
  }

  async disconnect() {
    if (this.sock) {
      this.sock.close();
      this.sock = null;
    }
    this._connected = false;
  }

  async callTool(tool: string, args: any): Promise<any> {
    if (!this._connected || !this.sock) {
      await this.connect();
    }

    const request: EngineRequest = {
      id: uuidv4(),
      tool,
      args,
      timestamp: Date.now(),
    };

    try {
      await this.sock!.send(JSON.stringify(request));
      const [reply] = await this.sock!.receive();
      const response: EngineResponse = JSON.parse(reply.toString());

      if (response.error) {
        throw new Error(response.error);
      }

      return response.result;
    } catch (error: any) {
      console.error("Engine call failed:", error);

      // 尝试重连
      if (error.message?.includes("timeout") || error.message?.includes("EAGAIN")) {
        this._connected = false;
        await this.connect();
        throw new Error("Engine timeout, please retry");
      }

      throw error;
    }
  }

  // 便捷方法
  async sendMessage(to: string, content: string, type: string = "text") {
    return this.callTool("wechat.send_message", { to, content, type });
  }

  async getMessages(chat: string, limit: number = 20, before?: string) {
    return this.callTool("wechat.get_messages", { chat, limit, before });
  }

  async getContacts(keyword?: string, tag?: string, limit?: number) {
    return this.callTool("wechat.get_contacts", { keyword, tag, limit });
  }

  async getGroups(keyword?: string, limit?: number) {
    return this.callTool("wechat.get_groups", { keyword, limit });
  }

  async getMoments(contact?: string, limit?: number) {
    return this.callTool("wechat.get_moments", { contact, limit });
  }

  async postMoments(content: string, images?: string[], visibility?: string, mention?: string[]) {
    return this.callTool("wechat.post_moments", { content, images, visibility, mention });
  }

  async likeMoments(momentId: string) {
    return this.callTool("wechat.like_moments", { moment_id: momentId });
  }

  async commentMoments(momentId: string, content: string) {
    return this.callTool("wechat.comment_moments", { moment_id: momentId, content });
  }

  async getStatus() {
    return this.callTool("wechat.get_status", {});
  }

  async getChats(limit?: number, unreadOnly?: boolean) {
    return this.callTool("wechat.get_chats", { limit, unread_only: unreadOnly });
  }
}

// 单例实例
export const engine = new EngineBridge();
