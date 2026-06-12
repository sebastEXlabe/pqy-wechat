import {
  Card,
  CardBody,
  CardHeader,
  Input,
  Button,
  Switch,
  Select,
  SelectItem,
  Divider,
} from "@heroui/react";
import { Save, RefreshCw, Shield, Bell, Globe, Bot } from "lucide-react";

export default function Settings() {
  return (
    <div className="space-y-6 max-w-4xl">
      <h2 className="text-2xl font-bold">设置</h2>

      {/* 引擎设置 */}
      <Card>
        <CardHeader>
          <div className="flex items-center gap-2">
            <Globe size={20} />
            <h3 className="text-lg font-semibold">引擎设置</h3>
          </div>
        </CardHeader>
        <CardBody className="space-y-4">
          <div className="grid grid-cols-2 gap-4">
            <Input label="引擎主机" defaultValue="127.0.0.1" />
            <Input label="引擎端口" defaultValue="5555" />
          </div>
          <div className="grid grid-cols-2 gap-4">
            <Input label="API 主机" defaultValue="127.0.0.1" />
            <Input label="API 端口" defaultValue="3000" />
          </div>
          <Select label="OCR 引擎" defaultSelectedKeys={["paddleocr"]}>
            <SelectItem key="paddleocr" value="paddleocr">PaddleOCR</SelectItem>
            <SelectItem key="wechat_native" value="wechat_native">微信原生 OCR</SelectItem>
          </Select>
        </CardBody>
      </Card>

      {/* 风控设置 */}
      <Card>
        <CardHeader>
          <div className="flex items-center gap-2">
            <Shield size={20} />
            <h3 className="text-lg font-semibold">风控设置</h3>
          </div>
        </CardHeader>
        <CardBody className="space-y-4">
          <div className="flex justify-between items-center">
            <div>
              <p className="font-medium">启用风控检测</p>
              <p className="text-sm text-default-500">自动检测并响应风控信号</p>
            </div>
            <Switch defaultSelected />
          </div>
          <Divider />
          <div className="grid grid-cols-2 gap-4">
            <Input label="每分钟最大消息数" type="number" defaultValue="5" />
            <Input label="每小时最大消息数" type="number" defaultValue="50" />
          </div>
          <div className="grid grid-cols-2 gap-4">
            <Input label="每天最大朋友圈数" type="number" defaultValue="2" />
            <Input label="每分钟最大点赞数" type="number" defaultValue="2" />
          </div>
          <div className="flex justify-between items-center">
            <div>
              <p className="font-medium">类人行为模拟</p>
              <p className="text-sm text-default-500">启用鼠标轨迹、打字延迟等模拟</p>
            </div>
            <Switch defaultSelected />
          </div>
        </CardBody>
      </Card>

      {/* AI Agent 设置 */}
      <Card>
        <CardHeader>
          <div className="flex items-center gap-2">
            <Bot size={20} />
            <h3 className="text-lg font-semibold">AI Agent 设置</h3>
          </div>
        </CardHeader>
        <CardBody className="space-y-4">
          <div className="flex justify-between items-center">
            <div>
              <p className="font-medium">启用 MCP Server</p>
              <p className="text-sm text-default-500">允许 AI 工具通过 MCP 协议调用</p>
            </div>
            <Switch defaultSelected />
          </div>
          <Divider />
          <Select label="MCP 传输方式" defaultSelectedKeys={["stdio"]}>
            <SelectItem key="stdio" value="stdio">stdio (Claude Desktop)</SelectItem>
            <SelectItem key="sse" value="sse">SSE (HTTP)</SelectItem>
          </Select>
          <Input label="MCP 端口" defaultValue="3001" />
        </CardBody>
      </Card>

      {/* 通知设置 */}
      <Card>
        <CardHeader>
          <div className="flex items-center gap-2">
            <Bell size={20} />
            <h3 className="text-lg font-semibold">通知设置</h3>
          </div>
        </CardHeader>
        <CardBody className="space-y-4">
          <div className="flex justify-between items-center">
            <div>
              <p className="font-medium">桌面通知</p>
              <p className="text-sm text-default-500">收到新消息时显示桌面通知</p>
            </div>
            <Switch defaultSelected />
          </div>
          <Divider />
          <div className="flex justify-between items-center">
            <div>
              <p className="font-medium">风控告警</p>
              <p className="text-sm text-default-500">检测到风控信号时告警</p>
            </div>
            <Switch defaultSelected />
          </div>
        </CardBody>
      </Card>

      {/* 保存按钮 */}
      <div className="flex justify-end gap-4">
        <Button variant="flat" startContent={<RefreshCw size={18} />}>
          重置默认
        </Button>
        <Button color="primary" startContent={<Save size={18} />}>
          保存设置
        </Button>
      </div>
    </div>
  );
}
