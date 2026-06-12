import { useState } from "react";
import { Card, CardBody, Input, Button, Avatar, ScrollShadow } from "@heroui/react";
import { Search, Send, Paperclip, Smile } from "lucide-react";

interface ChatItem {
  id: string;
  name: string;
  lastMessage: string;
  time: string;
  unread: number;
  avatar: string;
}

interface Message {
  id: string;
  from: string;
  content: string;
  time: string;
  isOwn: boolean;
}

const mockChats: ChatItem[] = [
  { id: "1", name: "张三", lastMessage: "明天的会议改到下午3点", time: "10:30", unread: 1, avatar: "" },
  { id: "2", name: "项目群", lastMessage: "收到，了解", time: "10:25", unread: 0, avatar: "" },
  { id: "3", name: "李四", lastMessage: "你好！", time: "10:20", unread: 2, avatar: "" },
  { id: "4", name: "文件传输助手", lastMessage: "文件已保存", time: "09:15", unread: 0, avatar: "" },
];

const mockMessages: Message[] = [
  { id: "1", from: "张三", content: "你好！最近忙吗？", time: "10:20", isOwn: false },
  { id: "2", from: "我", content: "还行，你呢？", time: "10:25", isOwn: true },
  { id: "3", from: "张三", content: "明天的会议改到下午3点", time: "10:30", isOwn: false },
];

export default function Chat() {
  const [selectedChat, setSelectedChat] = useState<string>("1");
  const [messageInput, setMessageInput] = useState("");
  const [searchQuery, setSearchQuery] = useState("");

  const filteredChats = mockChats.filter((chat) =>
    chat.name.toLowerCase().includes(searchQuery.toLowerCase())
  );

  const handleSend = () => {
    if (messageInput.trim()) {
      // TODO: 发送消息
      setMessageInput("");
    }
  };

  return (
    <div className="flex h-[calc(100vh-180px)] gap-4">
      {/* 聊天列表 */}
      <Card className="w-80 flex-shrink-0">
        <CardBody className="p-0">
          {/* 搜索 */}
          <div className="p-3 border-b border-divider">
            <Input
              placeholder="搜索聊天..."
              value={searchQuery}
              onChange={(e) => setSearchQuery(e.target.value)}
              startContent={<Search size={18} />}
              size="sm"
            />
          </div>

          {/* 聊天列表 */}
          <ScrollShadow className="flex-1">
            {filteredChats.map((chat) => (
              <div
                key={chat.id}
                className={`flex items-center gap-3 p-3 cursor-pointer hover:bg-default-100 transition-colors ${
                  selectedChat === chat.id ? "bg-primary/10" : ""
                }`}
                onClick={() => setSelectedChat(chat.id)}
              >
                <Avatar name={chat.name} size="md" />
                <div className="flex-1 min-w-0">
                  <div className="flex justify-between items-center">
                    <span className="font-medium truncate">{chat.name}</span>
                    <span className="text-xs text-default-500">{chat.time}</span>
                  </div>
                  <p className="text-sm text-default-500 truncate">{chat.lastMessage}</p>
                </div>
                {chat.unread > 0 && (
                  <div className="w-5 h-5 bg-danger rounded-full flex items-center justify-center">
                    <span className="text-xs text-white">{chat.unread}</span>
                  </div>
                )}
              </div>
            ))}
          </ScrollShadow>
        </CardBody>
      </Card>

      {/* 聊天窗口 */}
      <Card className="flex-1">
        <CardBody className="flex flex-col p-0">
          {/* 聊天头部 */}
          <div className="p-4 border-b border-divider">
            <h3 className="text-lg font-semibold">
              {mockChats.find((c) => c.id === selectedChat)?.name || "选择聊天"}
            </h3>
          </div>

          {/* 消息列表 */}
          <ScrollShadow className="flex-1 p-4">
            <div className="space-y-4">
              {mockMessages.map((msg) => (
                <div
                  key={msg.id}
                  className={`flex ${msg.isOwn ? "justify-end" : "justify-start"}`}
                >
                  {!msg.isOwn && (
                    <Avatar name={msg.from} size="sm" className="mr-2" />
                  )}
                  <div
                    className={`max-w-[70%] rounded-lg px-4 py-2 ${
                      msg.isOwn
                        ? "bg-primary text-primary-foreground"
                        : "bg-default-100"
                    }`}
                  >
                    <p className="text-sm">{msg.content}</p>
                    <span className="text-xs opacity-70 mt-1 block">{msg.time}</span>
                  </div>
                  {msg.isOwn && (
                    <Avatar name="我" size="sm" className="ml-2" />
                  )}
                </div>
              ))}
            </div>
          </ScrollShadow>

          {/* 输入框 */}
          <div className="p-4 border-t border-divider">
            <div className="flex items-center gap-2">
              <Button isIconOnly variant="light" size="sm">
                <Paperclip size={18} />
              </Button>
              <Button isIconOnly variant="light" size="sm">
                <Smile size={18} />
              </Button>
              <Input
                placeholder="输入消息..."
                value={messageInput}
                onChange={(e) => setMessageInput(e.target.value)}
                onKeyDown={(e) => e.key === "Enter" && handleSend()}
                className="flex-1"
              />
              <Button
                color="primary"
                isIconOnly
                onPress={handleSend}
                isDisabled={!messageInput.trim()}
              >
                <Send size={18} />
              </Button>
            </div>
          </div>
        </CardBody>
      </Card>
    </div>
  );
}
