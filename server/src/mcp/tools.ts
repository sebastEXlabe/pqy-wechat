export const MCP_TOOLS = [
  // ========== 消息相关 ==========
  {
    name: "wechat.send_message",
    description: "发送微信消息给指定联系人或群聊。支持文字、图片、文件、链接等类型。",
    inputSchema: {
      type: "object" as const,
      properties: {
        to: {
          type: "string",
          description: "接收者，可以是昵称、备注名或群聊名称",
        },
        content: {
          type: "string",
          description: "消息内容。文字消息直接填文字，图片/文件填路径",
        },
        type: {
          type: "string",
          enum: ["text", "image", "file", "link"],
          description: "消息类型，默认 text",
        },
        mention: {
          type: "string",
          description: "群聊时 @某人，填昵称或备注名",
        },
      },
      required: ["to", "content"],
    },
  },
  {
    name: "wechat.get_messages",
    description: "获取与指定联系人或群聊的聊天记录",
    inputSchema: {
      type: "object" as const,
      properties: {
        chat: {
          type: "string",
          description: "聊天对象，可以是昵称、备注名或群聊名称",
        },
        limit: {
          type: "number",
          description: "获取消息条数，默认 20，最大 100",
        },
        before: {
          type: "string",
          description: "获取此消息ID之前的消息，用于分页",
        },
      },
      required: ["chat"],
    },
  },

  // ========== 联系人相关 ==========
  {
    name: "wechat.get_contacts",
    description: "获取微信联系人列表，支持关键词搜索和标签筛选",
    inputSchema: {
      type: "object" as const,
      properties: {
        keyword: {
          type: "string",
          description: "搜索关键词，匹配昵称、备注、微信号",
        },
        tag: {
          type: "string",
          description: "按标签筛选",
        },
        limit: {
          type: "number",
          description: "返回数量，默认 50，最大 200",
        },
      },
    },
  },
  {
    name: "wechat.get_groups",
    description: "获取微信群聊列表",
    inputSchema: {
      type: "object" as const,
      properties: {
        keyword: {
          type: "string",
          description: "搜索关键词，匹配群名称",
        },
        limit: {
          type: "number",
          description: "返回数量，默认 50",
        },
      },
    },
  },

  // ========== 朋友圈相关 ==========
  {
    name: "wechat.get_moments",
    description: "获取朋友圈动态，可指定查看某人的朋友圈",
    inputSchema: {
      type: "object" as const,
      properties: {
        contact: {
          type: "string",
          description: "指定好友的朋友圈，不填则获取全部",
        },
        limit: {
          type: "number",
          description: "获取条数，默认 20",
        },
      },
    },
  },
  {
    name: "wechat.post_moments",
    description: "发布朋友圈动态",
    inputSchema: {
      type: "object" as const,
      properties: {
        content: {
          type: "string",
          description: "文字内容",
        },
        images: {
          type: "array",
          items: { type: "string" },
          description: "图片路径列表，最多 9 张",
        },
        visibility: {
          type: "string",
          enum: ["all", "partial", "private"],
          description: "可见范围：all=公开，partial=部分可见，private=私密",
        },
        mention: {
          type: "array",
          items: { type: "string" },
          description: "提醒谁看，填昵称列表",
        },
      },
      required: ["content"],
    },
  },
  {
    name: "wechat.like_moments",
    description: "点赞指定的朋友圈动态",
    inputSchema: {
      type: "object" as const,
      properties: {
        moment_id: {
          type: "string",
          description: "朋友圈ID",
        },
      },
      required: ["moment_id"],
    },
  },
  {
    name: "wechat.comment_moments",
    description: "评论指定的朋友圈动态",
    inputSchema: {
      type: "object" as const,
      properties: {
        moment_id: {
          type: "string",
          description: "朋友圈ID",
        },
        content: {
          type: "string",
          description: "评论内容",
        },
      },
      required: ["moment_id", "content"],
    },
  },

  // ========== 系统相关 ==========
  {
    name: "wechat.get_status",
    description: "获取微信登录状态、版本信息、系统状态",
    inputSchema: {
      type: "object" as const,
      properties: {},
    },
  },
  {
    name: "wechat.get_chats",
    description: "获取最近的聊天列表",
    inputSchema: {
      type: "object" as const,
      properties: {
        limit: {
          type: "number",
          description: "返回数量，默认 50",
        },
        unread_only: {
          type: "boolean",
          description: "是否只返回未读消息的聊天，默认 false",
        },
      },
    },
  },
];
