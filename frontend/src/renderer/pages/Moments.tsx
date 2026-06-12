import { useState } from "react";
import {
  Card,
  CardBody,
  CardHeader,
  Button,
  Avatar,
  Image,
  Textarea,
  Modal,
  ModalContent,
  ModalHeader,
  ModalBody,
  ModalFooter,
  useDisclosure,
} from "@heroui/react";
import { Heart, MessageCircle, Camera, Send } from "lucide-react";

interface Moment {
  id: string;
  author: {
    nickname: string;
    avatar: string;
  };
  content: string;
  images: string[];
  likes: string[];
  comments: {
    author: string;
    content: string;
  }[];
  timestamp: string;
}

const mockMoments: Moment[] = [
  {
    id: "1",
    author: { nickname: "张三", avatar: "" },
    content: "今天天气真好！☀️",
    images: ["https://picsum.photos/300/200?random=1", "https://picsum.photos/300/200?random=2"],
    likes: ["李四", "王五"],
    comments: [{ author: "李四", content: "是啊！" }],
    timestamp: "10分钟前",
  },
  {
    id: "2",
    author: { nickname: "项目群", avatar: "" },
    content: "项目进展汇报：本周完成了核心模块开发，下周进入测试阶段。",
    images: [],
    likes: ["张三"],
    comments: [],
    timestamp: "30分钟前",
  },
  {
    id: "3",
    author: { nickname: "李四", avatar: "" },
    content: "分享一篇好文章给大家！",
    images: ["https://picsum.photos/300/200?random=3"],
    likes: [],
    comments: [
      { author: "张三", content: "感谢分享！" },
      { author: "王五", content: "收藏了" },
    ],
    timestamp: "1小时前",
  },
];

export default function Moments() {
  const { isOpen, onOpen, onOpenChange } = useDisclosure();
  const [newContent, setNewContent] = useState("");

  return (
    <div className="space-y-4">
      <div className="flex justify-between items-center">
        <h2 className="text-2xl font-bold">朋友圈</h2>
        <Button color="primary" startContent={<Camera size={18} />} onPress={onOpen}>
          发布朋友圈
        </Button>
      </div>

      {/* 朋友圈列表 */}
      <div className="space-y-4 max-w-2xl mx-auto">
        {mockMoments.map((moment) => (
          <Card key={moment.id}>
            <CardBody>
              {/* 作者信息 */}
              <div className="flex items-center mb-3">
                <Avatar name={moment.author.nickname} size="md" />
                <div className="ml-3">
                  <p className="font-semibold">{moment.author.nickname}</p>
                  <p className="text-xs text-default-500">{moment.timestamp}</p>
                </div>
              </div>

              {/* 内容 */}
              <p className="mb-3">{moment.content}</p>

              {/* 图片 */}
              {moment.images.length > 0 && (
                <div className="grid grid-cols-3 gap-2 mb-3">
                  {moment.images.map((img, i) => (
                    <Image
                      key={i}
                      src={img}
                      alt={`图片 ${i + 1}`}
                      className="rounded-lg object-cover"
                      width={200}
                      height={150}
                    />
                  ))}
                </div>
              )}

              {/* 点赞和评论 */}
              {moment.likes.length > 0 && (
                <div className="flex items-center gap-2 mb-2 text-sm text-default-500">
                  <Heart size={14} className="text-danger" />
                  <span>{moment.likes.join("、")}</span>
                </div>
              )}

              {moment.comments.length > 0 && (
                <div className="bg-default-50 rounded-lg p-3 space-y-2">
                  {moment.comments.map((comment, i) => (
                    <p key={i} className="text-sm">
                      <span className="font-semibold">{comment.author}：</span>
                      {comment.content}
                    </p>
                  ))}
                </div>
              )}

              {/* 操作按钮 */}
              <div className="flex gap-4 mt-3">
                <Button
                  variant="light"
                  size="sm"
                  startContent={<Heart size={16} />}
                >
                  点赞
                </Button>
                <Button
                  variant="light"
                  size="sm"
                  startContent={<MessageCircle size={16} />}
                >
                  评论
                </Button>
              </div>
            </CardBody>
          </Card>
        ))}
      </div>

      {/* 发布朋友圈弹窗 */}
      <Modal isOpen={isOpen} onOpenChange={onOpenChange} size="2xl">
        <ModalContent>
          {(onClose) => (
            <>
              <ModalHeader>发布朋友圈</ModalHeader>
              <ModalBody>
                <Textarea
                  placeholder="分享新鲜事..."
                  value={newContent}
                  onChange={(e) => setNewContent(e.target.value)}
                  minRows={4}
                />
                <div className="flex gap-2 mt-4">
                  <Button variant="flat" startContent={<Camera size={18} />}>
                    添加图片
                  </Button>
                </div>
              </ModalBody>
              <ModalFooter>
                <Button variant="light" onPress={onClose}>
                  取消
                </Button>
                <Button
                  color="primary"
                  startContent={<Send size={18} />}
                  onPress={onClose}
                  isDisabled={!newContent.trim()}
                >
                  发布
                </Button>
              </ModalFooter>
            </>
          )}
        </ModalContent>
      </Modal>
    </div>
  );
}
