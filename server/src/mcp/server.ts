import { Server } from "@modelcontextprotocol/sdk/server/index.js";
import { StdioServerTransport } from "@modelcontextprotocol/sdk/server/stdio.js";
import {
  CallToolRequestSchema,
  ListToolsRequestSchema,
} from "@modelcontextprotocol/sdk/types.js";
import { FastifyInstance } from "fastify";
import { MCP_TOOLS } from "./tools.js";
import { EngineBridge } from "../bridge/engine.js";

export class WeChatMCPServer {
  private server: Server;
  private engine: EngineBridge;

  constructor() {
    this.server = new Server({
      name: "pqy-wechat",
      version: "1.0.0",
    });

    this.engine = new EngineBridge();
    this.setupHandlers();
  }

  private setupHandlers() {
    // 列出所有可用工具
    this.server.setRequestHandler(ListToolsRequestSchema, async () => ({
      tools: MCP_TOOLS,
    }));

    // 执行工具调用
    this.server.setRequestHandler(CallToolRequestSchema, async (request) => {
      const { name, arguments: args } = request.params;

      try {
        // 确保引擎已连接
        if (!this.engine.isConnected()) {
          await this.engine.connect();
        }

        // 调用引擎执行
        const result = await this.engine.callTool(name, args || {});

        return {
          content: [
            {
              type: "text",
              text: JSON.stringify(result, null, 2),
            },
          ],
        };
      } catch (error: any) {
        return {
          content: [
            {
              type: "text",
              text: `Error: ${error.message}`,
            },
          ],
          isError: true,
        };
      }
    });
  }

  async start() {
    const transport = new StdioServerTransport();
    await this.server.connect(transport);
    console.error("WeChat MCP Server started on stdio");
  }

  async stop() {
    await this.server.close();
    await this.engine.disconnect();
  }
}

// setupMCP 函数用于 Fastify 集成
export async function setupMCP(app: FastifyInstance) {
  // 如果是 stdio 模式，在单独的进程中启动
  if (process.argv.includes("--mcp-stdio")) {
    const mcpServer = new WeChatMCPServer();
    await mcpServer.start();

    // 优雅关闭
    process.on("SIGINT", async () => {
      await mcpServer.stop();
      process.exit(0);
    });

    process.on("SIGTERM", async () => {
      await mcpServer.stop();
      process.exit(0);
    });
  }
}
