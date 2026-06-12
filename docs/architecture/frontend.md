# 前端架构

## 技术栈

| 技术 | 版本 | 用途 |
|------|------|------|
| Electron | 35 | 桌面应用框架 |
| React | 19 | UI 框架 |
| HeroUI | Latest | UI 组件库 (基于 NextUI) |
| Tailwind CSS | 4 | 样式系统 |
| Zustand | Latest | 状态管理 |
| React Router | 7 | 路由 |
| TanStack Query | Latest | 数据获取 |
| Socket.IO Client | Latest | WebSocket |

---

## 项目结构

```
frontend/
├── src/
│   ├── main/                    # Electron 主进程
│   │   ├── index.ts             # 入口
│   │   ├── window.ts            # 窗口管理
│   │   ├── tray.ts              # 系统托盘
│   │   └── ipc.ts               # IPC 通信
│   │
│   ├── renderer/                # React 渲染进程
│   │   ├── App.tsx              # 根组件
│   │   ├── index.tsx            # 入口
│   │   │
│   │   ├── components/          # 通用组件
│   │   │   ├── layout/          # 布局组件
│   │   │   │   ├── Sidebar.tsx
│   │   │   │   ├── Header.tsx
│   │   │   │   └── MainLayout.tsx
│   │   │   │
│   │   │   ├── chat/            # 聊天组件
│   │   │   │   ├── ChatList.tsx
│   │   │   │   ├── ChatWindow.tsx
│   │   │   │   ├── MessageBubble.tsx
│   │   │   │   ├── MessageInput.tsx
│   │   │   │   └── AIMention.tsx
│   │   │   │
│   │   │   ├── contacts/        # 联系人组件
│   │   │   │   ├── ContactList.tsx
│   │   │   │   ├── ContactCard.tsx
│   │   │   │   └── ContactDetail.tsx
│   │   │   │
│   │   │   ├── moments/         # 朋友圈组件
│   │   │   │   ├── MomentsFeed.tsx
│   │   │   │   ├── MomentsCard.tsx
│   │   │   │   ├── MomentsPublisher.tsx
│   │   │   │   └── MomentsComment.tsx
│   │   │   │
│   │   │   ├── ai/              # AI Agent 组件
│   │   │   │   ├── AgentChat.tsx
│   │   │   │   ├── SkillCard.tsx
│   │   │   │   ├── ConfigPanel.tsx
│   │   │   │   └── HistoryList.tsx
│   │   │   │
│   │   │   └── common/          # 通用组件
│   │   │       ├── StatusIndicator.tsx
│   │   │       ├── LogViewer.tsx
│   │   │       ├── RiskAlert.tsx
│   │   │       └── LoadingSpinner.tsx
│   │   │
│   │   ├── pages/               # 页面
│   │   │   ├── Dashboard.tsx
│   │   │   ├── Chat.tsx
│   │   │   ├── Contacts.tsx
│   │   │   ├── Moments.tsx
│   │   │   ├── AIAgent.tsx
│   │   │   └── Settings.tsx
│   │   │
│   │   ├── stores/              # Zustand 状态
│   │   │   ├── useChatStore.ts
│   │   │   ├── useContactStore.ts
│   │   │   ├── useMomentsStore.ts
│   │   │   ├── useAIStore.ts
│   │   │   └── useSystemStore.ts
│   │   │
│   │   ├── hooks/               # 自定义 Hooks
│   │   │   ├── useWebSocket.ts
│   │   │   ├── useMCP.ts
│   │   │   └── useEngine.ts
│   │   │
│   │   ├── services/            # API 服务
│   │   │   ├── api.ts
│   │   │   ├── message.ts
│   │   │   ├── contact.ts
│   │   │   └── moments.ts
│   │   │
│   │   ├── types/               # 类型定义
│   │   │   ├── message.ts
│   │   │   ├── contact.ts
│   │   │   ├── moments.ts
│   │   │   └── common.ts
│   │   │
│   │   └── utils/               # 工具函数
│   │       ├── format.ts
│   │       ├── validation.ts
│   │       └── constants.ts
│   │
│   └── assets/                  # 静态资源
│       ├── icons/
│       ├── images/
│       └── fonts/
│
├── electron.config.js           # Electron 配置
├── tailwind.config.js           # Tailwind 配置
├── tsconfig.json
├── vite.config.ts
└── package.json
```

---

## 页面设计

### 1. Dashboard 页面

```
┌─────────────────────────────────────────────────────────────┐
│  ┌─────────┐                                                │
│  │  PQY    │  Dashboard                    ⚙️  🔔  👤      │
│  └─────────┘                                                │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐     │
│  │   微信状态    │  │   消息统计    │  │   风控状态    │     │
│  │   ● 已登录   │  │   1,234 条   │  │   🟢 低风险  │     │
│  └──────────────┘  └──────────────┘  └──────────────┘     │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                    快速操作                          │   │
│  │  [发消息]  [发朋友圈]  [查看联系人]  [AI 对话]       │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                    实时日志                          │   │
│  │  [10:30:00] 消息已发送给 张三                        │   │
│  │  [10:31:00] 收到新消息 from 李四                     │   │
│  │  [10:32:00] 朋友圈发布成功                          │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                    最近聊天                          │   │
│  │  👤 张三      明天的会议改到下午3点          10:30   │   │
│  │  👥 项目群    收到，了解                     10:25   │   │
│  │  👤 李四      你好！                        10:20   │   │
│  └─────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

### 2. Chat 页面

```
┌─────────────────────────────────────────────────────────────┐
│  Chat                                         ⚙️  🔔  👤   │
├──────────┬──────────────────────────────────────────────────┤
│          │                                                  │
│ 🔍 搜索  │  👤 张三                          📞  📹  ⋮     │
│          │                                                  │
│ ┌──────┐ │  ┌─────────────────────────────────────────┐    │
│ │ 张三  │ │  │                                         │    │
│ │ 你好  │ │  │  张三  10:20                            │    │
│ └──────┘ │  │  ┌─────────────────────┐                │    │
│          │  │  │ 你好！最近忙吗？      │                │    │
│ ┌──────┐ │  │  └─────────────────────┘                │    │
│ │项目群 │ │  │                                         │    │
│ │收到   │ │  │                    10:25  我 ─────────┐ │    │
│ └──────┘ │  │                    ┌─────────────────┐│ │    │
│          │  │                    │ 还行，你呢？      ││ │    │
│ ┌──────┐ │  │                    └─────────────────┘│ │    │
│ │李四   │ │  │                    ──────────────────┘ │    │
│ │你好   │ │  │                                         │    │
│ └──────┘ │  │  张三  10:30                            │    │
│          │  │  ┌─────────────────────┐                │    │
│          │  │  │ 明天的会议改到下午3点 │                │    │
│          │  │  └─────────────────────┘                │    │
│          │  │                                         │    │
│          │  └─────────────────────────────────────────┘    │
│          │                                                  │
│          │  ┌─────────────────────────────────────────┐    │
│          │  │ 📎  😊  输入消息...              [发送]  │    │
│          │  └─────────────────────────────────────────┘    │
└──────────┴──────────────────────────────────────────────────┘
```

### 3. Moments 页面

```
┌─────────────────────────────────────────────────────────────┐
│  Moments                                      ⚙️  🔔  👤   │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  [📷 发布朋友圈]                                      │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  👤 张三                              10分钟前       │   │
│  │                                                     │   │
│  │  今天天气真好！☀️                                     │   │
│  │                                                     │   │
│  │  ┌─────┐ ┌─────┐ ┌─────┐                           │   │
│  │  │     │ │     │ │     │                           │   │
│  │  │ 🌳  │ │ 🌸  │ │ ☀️  │                           │   │
│  │  └─────┘ └─────┘ └─────┘                           │   │
│  │                                                     │   │
│  │  ❤️ 李四、王五                                        │   │
│  │  💬 李四: 是啊！                                     │   │
│  │                                                     │   │
│  │  [❤️ 点赞]  [💬 评论]                                │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  👥 项目群                              30分钟前     │   │
│  │                                                     │   │
│  │  项目进展汇报                                        │   │
│  │                                                     │   │
│  │  ┌─────────────────────────────────────────────┐   │   │
│  │  │ 📄 项目报告.pdf                              │   │   │
│  │  └─────────────────────────────────────────────┘   │   │
│  │                                                     │   │
│  │  [❤️ 点赞]  [💬 评论]                                │   │
│  └─────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

---

## 状态管理

### Zustand Store 设计

```typescript
// stores/useChatStore.ts
import { create } from "zustand";

interface ChatState {
  // 数据
  chats: Chat[];
  messages: Map<string, Message[]>;
  activeChat: string | null;

  // 操作
  setActiveChat: (chatId: string) => void;
  addMessage: (chatId: string, message: Message) => void;
  updateChats: (chats: Chat[]) => void;

  // 异步操作
  fetchChats: () => Promise<void>;
  fetchMessages: (chatId: string) => Promise<void>;
  sendMessage: (to: string, content: string) => Promise<void>;
}

export const useChatStore = create<ChatState>((set, get) => ({
  chats: [],
  messages: new Map(),
  activeChat: null,

  setActiveChat: (chatId) => set({ activeChat: chatId }),

  addMessage: (chatId, message) =>
    set((state) => {
      const messages = new Map(state.messages);
      const chatMessages = messages.get(chatId) || [];
      messages.set(chatId, [...chatMessages, message]);
      return { messages };
    }),

  updateChats: (chats) => set({ chats }),

  fetchChats: async () => {
    const response = await fetch("/api/chats");
    const chats = await response.json();
    set({ chats });
  },

  fetchMessages: async (chatId) => {
    const response = await fetch(`/api/messages?chat=${chatId}`);
    const messages = await response.json();
    set((state) => {
      const newMessages = new Map(state.messages);
      newMessages.set(chatId, messages);
      return { messages: newMessages };
    });
  },

  sendMessage: async (to, content) => {
    await fetch("/api/messages", {
      method: "POST",
      body: JSON.stringify({ to, content }),
    });
  },
}));
```

---

## 组件设计

### MessageBubble 组件

```tsx
// components/chat/MessageBubble.tsx
import { Avatar } from "@heroui/react";

interface MessageBubbleProps {
  message: Message;
  isOwn: boolean;
}

export function MessageBubble({ message, isOwn }: MessageBubbleProps) {
  return (
    <div className={`flex ${isOwn ? "justify-end" : "justify-start"} mb-4`}>
      {!isOwn && (
        <Avatar src={message.from.avatar} className="mr-2" />
      )}

      <div
        className={`max-w-[70%] rounded-lg px-4 py-2 ${
          isOwn
            ? "bg-primary text-primary-foreground"
            : "bg-default-100"
        }`}
      >
        <p className="text-sm">{message.content}</p>
        <span className="text-xs opacity-70 mt-1 block">
          {formatTime(message.timestamp)}
        </span>
      </div>

      {isOwn && (
        <Avatar src={message.from.avatar} className="ml-2" />
      )}
    </div>
  );
}
```

### MomentsCard 组件

```tsx
// components/moments/MomentsCard.tsx
import { Card, CardBody, Image, Button } from "@heroui/react";
import { Heart, MessageCircle } from "lucide-react";

interface MomentsCardProps {
  moment: Moment;
  onLike: (id: string) => void;
  onComment: (id: string, content: string) => void;
}

export function MomentsCard({ moment, onLike, onComment }: MomentsCardProps) {
  return (
    <Card className="mb-4">
      <CardBody>
        <div className="flex items-center mb-3">
          <Avatar src={moment.author.avatar} />
          <div className="ml-3">
            <p className="font-semibold">{moment.author.nickname}</p>
            <p className="text-xs text-default-500">
              {formatTime(moment.timestamp)}
            </p>
          </div>
        </div>

        <p className="mb-3">{moment.content}</p>

        {moment.images.length > 0 && (
          <div className="grid grid-cols-3 gap-2 mb-3">
            {moment.images.map((img, i) => (
              <Image key={i} src={img} className="rounded-lg" />
            ))}
          </div>
        )}

        <div className="flex items-center gap-4 text-default-500">
          <Button
            variant="light"
            size="sm"
            startContent={<Heart size={16} />}
            onPress={() => onLike(moment.id)}
          >
            {moment.likes.length}
          </Button>
          <Button
            variant="light"
            size="sm"
            startContent={<MessageCircle size={16} />}
          >
            {moment.comments.length}
          </Button>
        </div>
      </CardBody>
    </Card>
  );
}
```

---

## Electron 配置

### 主进程

```typescript
// main/index.ts
import { app, BrowserWindow, Tray } from "electron";
import { createWindow } from "./window";
import { createTray } from "./tray";
import { setupIPC } from "./ipc";

let mainWindow: BrowserWindow;
let tray: Tray;

app.whenReady().then(() => {
  mainWindow = createWindow();
  tray = createTray(mainWindow);
  setupIPC(mainWindow);
});

app.on("window-all-closed", () => {
  if (process.platform !== "darwin") {
    app.quit();
  }
});
```

### 窗口配置

```typescript
// main/window.ts
import { BrowserWindow } from "electron";
import path from "path";

export function createWindow(): BrowserWindow {
  const mainWindow = new BrowserWindow({
    width: 1200,
    height: 800,
    minWidth: 800,
    minHeight: 600,
    webPreferences: {
      preload: path.join(__dirname, "preload.js"),
      contextIsolation: true,
      nodeIntegration: false,
    },
    titleBarStyle: "hiddenInset",
    show: false,
  });

  // 开发模式加载本地服务器
  if (process.env.NODE_ENV === "development") {
    mainWindow.loadURL("http://localhost:5173");
    mainWindow.webContents.openDevTools();
  } else {
    mainWindow.loadFile(path.join(__dirname, "../dist/index.html"));
  }

  mainWindow.once("ready-to-show", () => {
    mainWindow.show();
  });

  return mainWindow;
}
```

---

## 样式系统

### Tailwind 配置

```javascript
// tailwind.config.js
import { heroui } from "@heroui/react";

export default {
  content: [
    "./src/**/*.{js,ts,jsx,tsx}",
    "./node_modules/@heroui/theme/dist/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      colors: {
        primary: {
          DEFAULT: "#07C160", // 微信绿
          foreground: "#FFFFFF",
        },
      },
    },
  },
  darkMode: "class",
  plugins: [heroui()],
};
```

### HeroUI 主题

```typescript
// styles/heroui.ts
import { heroui } from "@heroui/react";

export default heroui({
  themes: {
    light: {
      colors: {
        primary: {
          DEFAULT: "#07C160",
          foreground: "#FFFFFF",
        },
        focus: "#07C160",
      },
    },
    dark: {
      colors: {
        primary: {
          DEFAULT: "#07C160",
          foreground: "#FFFFFF",
        },
        focus: "#07C160",
      },
    },
  },
});
```

---

## 下一步

- [API 网关](api-gateway.md) - Node.js API 实现
- [自动化引擎](automation-engine.md) - Python 引擎设计
- [MCP 协议](mcp-protocol.md) - MCP 工具定义
