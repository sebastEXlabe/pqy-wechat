import { useState, useRef, useEffect } from "react";
import {
  Card,
  CardBody,
  CardHeader,
  Input,
  Button,
  Avatar,
  ScrollShadow,
  Chip,
} from "@heroui/react";
import { Send, Bot, User, Sparkles, History, Settings } from "lucide-react";

interface ChatMessage {
  id: string;
  role: "user" | "assistant";
  content: string;
  timestamp: string;
}

const mockMessages: ChatMessage[] = [
  {
    id: "1",
    role: "user",
    content: "帮我给张三发一条消息，说明天的会议改到下午3点",
    timestamp: "10:30",
  },
  {
    id: "2",
    role: "assistant",
    content: "好的，我来帮你发送消息。\n\n[Claude 调用 wechat.send_message]\n\n工具返回: 消息已成功发送给张三。\n\n已经帮您给张三发送了消息，说明天的会议改到下午3点。",
    timestamp: "10:30",
  },
  {
    id: "3",
    role: "user",
    content: "查看一下李四的朋友圈",
    timestamp: "10:35",
  },
  {
    id: "4",
    role: "assistant",
    content: "好的，我来查看李四的朋友圈。\n\n[Claude 调用 wechat.get_moments]\n\n李四最近的朋友圈：\n\n1. 「分享一篇好文章给大家！」 - 1小时前\n   ❤️ 无点赞 💬 2条评论\n\n2. 「周末去爬山了！」 - 昨天\n   ❤️ 5个点赞 💬 3条评论",
    timestamp: "10:35",
  },
];

export default function AIAgent() {
  const [messages, setMessages] = useState<ChatMessage[]>(mockMessages);
  const [input, setInput] = useState("");
  const messagesEndRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    messagesEndRef.current?.scrollIntoView({ behavior: "smooth" });
  }, [messages]);

  const handleSend = () => {
    if (input.trim()) {
      const userMessage: ChatMessage = {
        id: Date.now().toString(),
        role: "user",
        content: input,
        timestamp: new Date().toLocaleTimeString("zh-CN", {
          hour: "2-digit",
          minute: "2-digit",
        }),
      };
      setMessages((prev) => [...prev, userMessage]);
      setInput("");

      // TODO: 调用 AI API
      setTimeout(() => {
        const aiMessage: ChatMessage = {
          id: (Date.now() + 1).toString(),
          role: "assistant",
          content: "收到，正在处理...",
          timestamp: new Date().toLocaleTimeString("zh-CN", {
            hour: "2-digit",
            minute: "2-digit",
          }),
        };
        setMessages((prev) => [...prev, aiMessage]);
      }, 1000);
    }
  };

  return (
    <div className="flex h-[calc(100vh-180px)] gap-4">
      {/* 主聊天区 */}
      <Card className="flex-1">
        <CardBody className="flex flex-col p-0">
          {/* 头部 */}
          <div className="p-4 border-b border-divider flex items-center justify-between">
            <div className="flex items-center gap-3">
              <div className="w-10 h-10 bg-primary/10 rounded-full flex items-center justify-center">
                <Bot className="text-primary" size={20} />
              </div>
              <div>
                <h3 className="font-semibold">AI Agent</h3>
                <p className="text-xs text-default-500">MCP Protocol · Claude</p>
              </div>
            </div>
            <div className="flex gap-2">
              <Button isIconOnly variant="light" size="sm">
                <History size={18} />
              </Button>
              <Button isIconOnly variant="light" size="sm">
                <Settings size={18} />
              </Button>
            </div>
          </div>

          {/* 消息列表 */}
          <ScrollShadow className="flex-1 p-4">
            <div className="space-y-4">
              {messages.map((msg) => (
                <div
                  key={msg.id}
                  className={`flex ${msg.role === "user" ? "justify-end" : "justify-start"}`}
                >
                  {msg.role === "assistant" && (
                    <Avatar
                      icon={<Bot size={18} />}
                      className="bg-primary/10 text-primary mr-2"
                      size="sm"
                    />
                  )}
                  <div
                    className={`max-w-[80%] rounded-lg px-4 py-3 ${
                      msg.role === "user"
                        ? "bg-primary text-primary-foreground"
                        : "bg-default-100"
                    }`}
                  >
                    <p className="text-sm whitespace-pre-wrap">{msg.content}</p>
                    <span className="text-xs opacity-70 mt-2 block">{msg.timestamp}</span>
                  </div>
                  {msg.role === "user" && (
                    <Avatar
                      icon={<User size={18} />}
                      className="bg-secondary/10 text-secondary ml-2"
                      size="sm"
                    />
                  )}
                </div>
              ))}
              <div ref={messagesEndRef} />
            </div>
          </ScrollShadow>

          {/* 输入框 */}
          <div className="p-4 border-t border-divider">
            <div className="flex items-center gap-2">
              <Input
                placeholder="输入指令... (例如: 帮我给张三发一条消息)"
                value={input}
                onChange={(e) => setInput(e.target.value)}
                onKeyDown={(e) => e.key === "Enter" && handleSend()}
                className="flex-1"
                startContent={<Sparkles size={18} className="text-default-400" />}
              />
              <Button
                color="primary"
                isIconOnly
                onPress={handleSend}
                isDisabled={!input.trim()}
              >
                <Send size={18} />
              </Button>
            </div>
          </div>
        </CardBody>
      </Card>

      {/* 侧边栏 */}
      <div className="w-72 space-y-4">
        {/* 可用工具 */}
        <Card>
          <CardHeader>
            <h4 className="text-sm font-semibold">可用工具</h4>
          </CardHeader>
          <CardBody className="space-y-2">
            {[
              "wechat.send_message",
              "wechat.get_contacts",
              "wechat.get_moments",
              "wechat.post_moments",
            ].map((tool) => (
              <Chip key={tool} variant="flat" size="sm" className="w-full justify-start">
                {tool}
              </Chip>
            ))}
          </CardBody>
        </Card>

        {/* 快捷指令 */}
        <Card>
          <CardHeader>
            <h4 className="text-sm font-semibold">快捷指令</h4>
          </CardHeader>
          <CardBody className="space-y-2">
            {[
              "查看未读消息",
              "发布朋友圈",
              "查看联系人",
              "获取系统状态",
            ].map((cmd) => (
              <Button
                key={cmd}
                variant="flat"
                size="sm"
                className="w-full justify-start"
                onPress={() => setInput(cmd)}
              >
                {cmd}
              </Button>
            ))}
          </CardBody>
        </Card>
      </div>
    </div>
  );
}
