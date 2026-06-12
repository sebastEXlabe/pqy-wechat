import { useState } from "react";
import {
  Card,
  CardBody,
  Input,
  Avatar,
  Chip,
  ScrollShadow,
  Button,
} from "@heroui/react";
import { Search, UserPlus, Tag } from "lucide-react";

interface Contact {
  id: string;
  nickname: string;
  remark: string;
  tags: string[];
  avatar: string;
}

const mockContacts: Contact[] = [
  { id: "1", nickname: "张三", remark: "同事-张三", tags: ["同事", "项目组"], avatar: "" },
  { id: "2", nickname: "李四", remark: "朋友-李四", tags: ["朋友"], avatar: "" },
  { id: "3", nickname: "王五", remark: "客户-王五", tags: ["客户", "VIP"], avatar: "" },
  { id: "4", nickname: "赵六", remark: "同学-赵六", tags: ["同学"], avatar: "" },
  { id: "5", nickname: "钱七", remark: "", tags: ["同事"], avatar: "" },
];

export default function Contacts() {
  const [searchQuery, setSearchQuery] = useState("");
  const [selectedTag, setSelectedTag] = useState<string | null>(null);

  // 获取所有标签
  const allTags = Array.from(new Set(mockContacts.flatMap((c) => c.tags)));

  // 过滤联系人
  const filteredContacts = mockContacts.filter((contact) => {
    const matchSearch =
      contact.nickname.toLowerCase().includes(searchQuery.toLowerCase()) ||
      contact.remark.toLowerCase().includes(searchQuery.toLowerCase());
    const matchTag = !selectedTag || contact.tags.includes(selectedTag);
    return matchSearch && matchTag;
  });

  return (
    <div className="space-y-4">
      <div className="flex justify-between items-center">
        <h2 className="text-2xl font-bold">联系人</h2>
        <Button color="primary" startContent={<UserPlus size={18} />}>
          添加联系人
        </Button>
      </div>

      {/* 搜索和筛选 */}
      <div className="flex gap-4">
        <Input
          placeholder="搜索联系人..."
          value={searchQuery}
          onChange={(e) => setSearchQuery(e.target.value)}
          startContent={<Search size={18} />}
          className="flex-1"
        />
      </div>

      {/* 标签筛选 */}
      <div className="flex gap-2 flex-wrap">
        <Chip
          variant={selectedTag === null ? "solid" : "flat"}
          color="primary"
          onClick={() => setSelectedTag(null)}
          className="cursor-pointer"
        >
          全部
        </Chip>
        {allTags.map((tag) => (
          <Chip
            key={tag}
            variant={selectedTag === tag ? "solid" : "flat"}
            color="secondary"
            onClick={() => setSelectedTag(selectedTag === tag ? null : tag)}
            className="cursor-pointer"
          >
            {tag}
          </Chip>
        ))}
      </div>

      {/* 联系人列表 */}
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
        {filteredContacts.map((contact) => (
          <Card key={contact.id} isPressable isHoverable>
            <CardBody className="flex flex-row items-center gap-4">
              <Avatar name={contact.nickname} size="lg" />
              <div className="flex-1 min-w-0">
                <p className="font-semibold truncate">
                  {contact.remark || contact.nickname}
                </p>
                {contact.remark && (
                  <p className="text-sm text-default-500 truncate">
                    {contact.nickname}
                  </p>
                )}
                <div className="flex gap-1 mt-2 flex-wrap">
                  {contact.tags.map((tag) => (
                    <Chip key={tag} size="sm" variant="flat">
                      {tag}
                    </Chip>
                  ))}
                </div>
              </div>
            </CardBody>
          </Card>
        ))}
      </div>
    </div>
  );
}
