# MCP 协议实现

## 概述

MCP (Model Context Protocol) 是 Anthropic 推出的标准化协议，用于 AI 模型与外部工具的交互。PQY 通过实现 MCP Server，让 Claude Desktop、Cursor 等 AI 工具可以直接调用微信功能。

---

## MCP Server 架构

```
┌─────────────────────────────────────────────────────────┐
│                    MCP Client                           │
│         (Claude Desktop / Cursor / Custom AI)           │
└───────────────────────────┬─────────────────────────────┘
                            │
                       MCP Protocol
                       (stdio/SSE)
                            │
                            ▼
┌─────────────────────────────────────────────────────────┐
│                    MCP Server                           │
│  ┌─────────────────────────────────────────────────┐   │
│  │                 Tool Registry                    │   │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐          │   │
│  │  │ Message │ │ Contact │ │ Moments │  ...      │   │
│  │  │  Tools  │ │  Tools  │ │  Tools  │          │   │
│  │  └─────────┘ └─────────┘ └─────────┘          │   │
│  └─────────────────────────────────────────────────┘   │
│  ┌─────────────────────────────────────────────────┐   │
│  │                 Tool Executor                    │   │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐          │   │
│  │  │ Request │ │   Run   │ │ Response│          │   │
│  │  │ Parser  │ │  Tool   │ │ Builder │          │   │
│  │  └─────────┘ └─────────┘ └─────────┘          │   │
│  └─────────────────────────────────────────────────┘   │
└───────────────────────────┬─────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────┐
│                 Core Business Layer                      │
└─────────────────────────────────────────────────────────┘
```

---

## 工具定义

### 消息相关工具

#### `wechat.send_message`

发送微信消息给指定联系人或群聊。

```typescript
{
  name: "wechat.send_message",
  description: "发送微信消息给指定联系人或群聊。支持文字、图片、文件、链接等类型。",
  inputSchema: {
    type: "object",
    properties: {
      to: {
        type: "string",
        description: "接收者，可以是昵称、备注名或群聊名称"
      },
      content: {
        type: "string",
        description: "消息内容。文字消息直接填文字，图片/文件填路径"
      },
      type: {
        type: "string",
        enum: ["text", "image", "file", "link"],
        description: "消息类型，默认 text"
      },
      mention: {
        type: "string",
        description: "群聊时 @某人，填昵称或备注名"
      }
    },
    required: ["to", "content"]
  }
}
```

**示例调用**：
```json
{
  "to": "文件传输助手",
  "content": "Hello from PQY!",
  "type": "text"
}
```

**示例返回**：
```json
{
  "success": true,
  "message_id": "msg_123456",
  "timestamp": "2026-06-12T10:30:00Z"
}
```

#### `wechat.get_messages`

获取聊天记录。

```typescript
{
  name: "wechat.get_messages",
  description: "获取与指定联系人或群聊的聊天记录",
  inputSchema: {
    type: "object",
    properties: {
      chat: {
        type: "string",
        description: "聊天对象，可以是昵称、备注名或群聊名称"
      },
      limit: {
        type: "number",
        description: "获取消息条数，默认 20，最大 100"
      },
      before: {
        type: "string",
        description: "获取此消息ID之前的消息，用于分页"
      }
    },
    required: ["chat"]
  }
}
```

**示例返回**：
```json
{
  "messages": [
    {
      "id": "msg_123456",
      "from": "张三",
      "content": "你好",
      "timestamp": "2026-06-12T10:30:00Z",
      "type": "text"
    },
    {
      "id": "msg_123457",
      "from": "我",
      "content": "你好！",
      "timestamp": "2026-06-12T10:31:00Z",
      "type": "text"
    }
  ],
  "has_more": true,
  "next_cursor": "msg_123457"
}
```

---

### 联系人相关工具

#### `wechat.get_contacts`

获取联系人列表。

```typescript
{
  name: "wechat.get_contacts",
  description: "获取微信联系人列表，支持关键词搜索和标签筛选",
  inputSchema: {
    type: "object",
    properties: {
      keyword: {
        type: "string",
        description: "搜索关键词，匹配昵称、备注、微信号"
      },
      tag: {
        type: "string",
        description: "按标签筛选"
      },
      limit: {
        type: "number",
        description: "返回数量，默认 50，最大 200"
      }
    }
  }
}
```

**示例返回**：
```json
{
  "contacts": [
    {
      "id": "wxid_abc123",
      "nickname": "张三",
      "remark": "同事-张三",
      "avatar": "https://...",
      "tags": ["同事", "项目组"],
      "last_message_time": "2026-06-12T10:00:00Z"
    }
  ],
  "total": 150
}
```

#### `wechat.get_groups`

获取群聊列表。

```typescript
{
  name: "wechat.get_groups",
  description: "获取微信群聊列表",
  inputSchema: {
    type: "object",
    properties: {
      keyword: {
        type: "string",
        description: "搜索关键词，匹配群名称"
      },
      limit: {
        type: "number",
        description: "返回数量，默认 50"
      }
    }
  }
}
```

---

### 朋友圈相关工具

#### `wechat.get_moments`

获取朋友圈动态。

```typescript
{
  name: "wechat.get_moments",
  description: "获取朋友圈动态，可指定查看某人的朋友圈",
  inputSchema: {
    type: "object",
    properties: {
      contact: {
        type: "string",
        description: "指定好友的朋友圈，不填则获取全部"
      },
      limit: {
        type: "number",
        description: "获取条数，默认 20"
      }
    }
  }
}
```

**示例返回**：
```json
{
  "moments": [
    {
      "id": "moment_789",
      "author": {
        "nickname": "张三",
        "avatar": "https://..."
      },
      "content": "今天天气真好！",
      "images": ["https://..."],
      "likes": ["李四", "王五"],
      "comments": [
        {
          "author": "李四",
          "content": "是啊！"
        }
      ],
      "timestamp": "2026-06-12T09:00:00Z"
    }
  ]
}
```

#### `wechat.post_moments`

发布朋友圈。

```typescript
{
  name: "wechat.post_moments",
  description: "发布朋友圈动态",
  inputSchema: {
    type: "object",
    properties: {
      content: {
        type: "string",
        description: "文字内容"
      },
      images: {
        type: "array",
        items: { type: "string" },
        description: "图片路径列表，最多 9 张"
      },
      visibility: {
        type: "string",
        enum: ["all", "partial", "private"],
        description: "可见范围：all=公开，partial=部分可见，private=私密"
      },
      mention: {
        type: "array",
        items: { type: "string" },
        description: "提醒谁看，填昵称列表"
      }
    },
    required: ["content"]
  }
}
```

#### `wechat.like_moments`

点赞朋友圈。

```typescript
{
  name: "wechat.like_moments",
  description: "点赞指定的朋友圈动态",
  inputSchema: {
    type: "object",
    properties: {
      moment_id: {
        type: "string",
        description: "朋友圈ID"
      }
    },
    required: ["moment_id"]
  }
}
```

#### `wechat.comment_moments`

评论朋友圈。

```typescript
{
  name: "wechat.comment_moments",
  description: "评论指定的朋友圈动态",
  inputSchema: {
    type: "object",
    properties: {
      moment_id: {
        type: "string",
        description: "朋友圈ID"
      },
      content: {
        type: "string",
        description: "评论内容"
      }
    },
    required: ["moment_id", "content"]
  }
}
```

---

### 系统相关工具

#### `wechat.get_status`

获取微信登录状态和系统信息。

```typescript
{
  name: "wechat.get_status",
  description: "获取微信登录状态、版本信息、系统状态"
}
```

**示例返回**：
```json
{
  "logged_in": true,
  "user": {
    "nickname": "我的微信",
    "wxid": "wxid_my123"
  },
  "wechat_version": "4.1.10.31",
  "engine_status": "running",
  "risk_level": "low",
  "uptime": "2h 30m"
}
```

#### `wechat.get_chats`

获取聊天列表。

```typescript
{
  name: "wechat.get_chats",
  description: "获取最近的聊天列表",
  inputSchema: {
    type: "object",
    properties: {
      limit: {
        type: "number",
        description: "返回数量，默认 50"
      },
      unread_only: {
        type: "boolean",
        description: "是否只返回未读消息的聊天，默认 false"
      }
    }
  }
}
```

---

## Server 实现

### 核心代码

```typescript
// src/mcp/server.ts
import { Server } from "@modelcontextprotocol/sdk/server/index.js";
import { StdioServerTransport } from "@modelcontextprotocol/sdk/server/stdio.js";
import {
  CallToolRequestSchema,
  ListToolsRequestSchema,
} from "@modelcontextprotocol/sdk/types.js";
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
        // 调用引擎层执行
        const result = await this.engine.callTool(name, args);

        return {
          content: [
            {
              type: "text",
              text: JSON.stringify(result, null, 2),
            },
          ],
        };
      } catch (error) {
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
}
```

### 引擎桥接

```typescript
// src/bridge/engine.ts
import * as zmq from "zeromq";

export class EngineBridge {
  private sock: zmq.Request;

  constructor() {
    this.sock = new zmq.Request();
    this.sock.connect("tcp://127.0.0.1:5555");
  }

  async callTool(name: string, args: any): Promise<any> {
    const request = {
      id: crypto.randomUUID(),
      tool: name,
      args: args,
      timestamp: Date.now(),
    };

    await this.sock.send(JSON.stringify(request));
    const [reply] = await this.sock.receive();
    const response = JSON.parse(reply.toString());

    if (response.error) {
      throw new Error(response.error);
    }

    return response.result;
  }
}
```

---

## Claude Desktop 配置

### 配置文件位置

- Windows: `%APPDATA%\Claude\claude_desktop_config.json`
- macOS: `~/Library/Application Support/Claude/claude_desktop_config.json`

### 配置示例

```json
{
  "mcpServers": {
    "wechat": {
      "command": "node",
      "args": ["C:/path/to/pqy/server/dist/mcp/server.js"],
      "env": {
        "ENGINE_HOST": "127.0.0.1",
        "ENGINE_PORT": "5555"
      }
    }
  }
}
```

### 使用示例

在 Claude Desktop 中：

```
用户: 帮我给张三发一条消息，说明天的会议改到下午3点

Claude: 我来帮你发送消息。

[Claude 调用 wechat.send_message]

工具返回: 消息已成功发送给张三。

Claude: 已经帮您给张三发送了消息，说明天的会议改到下午3点。
```

---

## 错误处理

### 错误类型

```typescript
enum MCPErrorCode {
  // 工具错误
  TOOL_NOT_FOUND = "tool_not_found",
  INVALID_ARGS = "invalid_args",
  EXECUTION_FAILED = "execution_failed",

  // 引擎错误
  ENGINE_NOT_CONNECTED = "engine_not_connected",
  ENGINE_TIMEOUT = "engine_timeout",

  // 微信错误
  WECHAT_NOT_RUNNING = "wechat_not_running",
  WECHAT_NOT_LOGGED_IN = "wechat_not_logged_in",
  CONTACT_NOT_FOUND = "contact_not_found",

  // 风控错误
  RATE_LIMITED = "rate_limited",
  RISK_DETECTED = "risk_detected",
}
```

### 错误响应格式

```json
{
  "isError": true,
  "content": [
    {
      "type": "text",
      "text": "Error: Contact '张三' not found. Please check the name and try again."
    }
  ]
}
```

---

## 安全考虑

### 1. 频率限制

```typescript
const RATE_LIMITS = {
  "wechat.send_message": { maxPerMinute: 10, maxPerHour: 100 },
  "wechat.post_moments": { maxPerDay: 5 },
  "wechat.like_moments": { maxPerMinute: 5 },
};
```

### 2. 权限控制

```typescript
// 可配置的权限系统
const PERMISSIONS = {
  "wechat.send_message": ["user", "admin"],
  "wechat.post_moments": ["admin"],
  "wechat.get_contacts": ["user", "admin"],
};
```

### 3. 敏感操作确认

```typescript
// 高风险操作需要确认
const SENSITIVE_TOOLS = [
  "wechat.post_moments",
  "wechat.send_message", // 发送给多人时
];
```

---

## 扩展工具

### 自定义工具注册

```typescript
// 插件可以注册自定义工具
class MyPlugin implements Plugin {
  getTools(): MCPTool[] {
    return [
      {
        name: "my_plugin.do_something",
        description: "自定义功能",
        inputSchema: { /* ... */ },
      },
    ];
  }
}

// 注册插件
mcpServer.registerPlugin(new MyPlugin());
```

---

## 下一步

- [REST API 文档](../api/rest-api.md) - REST API 详细说明
- [WebSocket 文档](../api/websocket.md) - WebSocket 事件说明
- [AI Agent 功能](../features/ai-agent.md) - AI Agent 详细功能
