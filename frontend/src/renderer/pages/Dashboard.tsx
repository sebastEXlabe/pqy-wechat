import { useEffect } from "react";
import {
  Card,
  CardBody,
  CardHeader,
  Button,
  Chip,
  Progress,
} from "@heroui/react";
import {
  MessageCircle,
  Users,
  Compass,
  Bot,
  Activity,
  Shield,
  Clock,
} from "lucide-react";
import { useSystemStore } from "@/stores/useSystemStore";

export default function Dashboard() {
  const { isConnected, engineStatus, riskLevel, user, wechatVersion, uptime, fetchStatus } =
    useSystemStore();

  useEffect(() => {
    fetchStatus();
    const interval = setInterval(fetchStatus, 30000); // 每 30 秒刷新
    return () => clearInterval(interval);
  }, [fetchStatus]);

  return (
    <div className="space-y-6">
      <h2 className="text-2xl font-bold">仪表盘</h2>

      {/* 状态卡片 */}
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
        <Card>
          <CardBody className="flex flex-row items-center gap-4">
            <div className="p-3 bg-primary/10 rounded-lg">
              <MessageCircle className="text-primary" size={24} />
            </div>
            <div>
              <p className="text-sm text-default-500">微信状态</p>
              <p className="text-lg font-semibold">
                {isConnected ? "已连接" : "未连接"}
              </p>
            </div>
          </CardBody>
        </Card>

        <Card>
          <CardBody className="flex flex-row items-center gap-4">
            <div className="p-3 bg-success/10 rounded-lg">
              <Activity className="text-success" size={24} />
            </div>
            <div>
              <p className="text-sm text-default-500">引擎状态</p>
              <p className="text-lg font-semibold">
                {engineStatus === "running" ? "运行中" : engineStatus === "stopped" ? "已停止" : "错误"}
              </p>
            </div>
          </CardBody>
        </Card>

        <Card>
          <CardBody className="flex flex-row items-center gap-4">
            <div className="p-3 bg-warning/10 rounded-lg">
              <Shield className="text-warning" size={24} />
            </div>
            <div>
              <p className="text-sm text-default-500">风控等级</p>
              <p className="text-lg font-semibold">
                {riskLevel === "low" ? "低风险" : riskLevel === "medium" ? "中风险" : "高风险"}
              </p>
            </div>
          </CardBody>
        </Card>

        <Card>
          <CardBody className="flex flex-row items-center gap-4">
            <div className="p-3 bg-secondary/10 rounded-lg">
              <Clock className="text-secondary" size={24} />
            </div>
            <div>
              <p className="text-sm text-default-500">运行时间</p>
              <p className="text-lg font-semibold">{uptime}</p>
            </div>
          </CardBody>
        </Card>
      </div>

      {/* 快速操作 */}
      <Card>
        <CardHeader>
          <h3 className="text-lg font-semibold">快速操作</h3>
        </CardHeader>
        <CardBody>
          <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
            <Button
              color="primary"
              variant="flat"
              startContent={<MessageCircle size={18} />}
              className="h-20"
            >
              发消息
            </Button>
            <Button
              color="secondary"
              variant="flat"
              startContent={<Compass size={18} />}
              className="h-20"
            >
              发朋友圈
            </Button>
            <Button
              color="success"
              variant="flat"
              startContent={<Users size={18} />}
              className="h-20"
            >
              查看联系人
            </Button>
            <Button
              color="warning"
              variant="flat"
              startContent={<Bot size={18} />}
              className="h-20"
            >
              AI 对话
            </Button>
          </div>
        </CardBody>
      </Card>

      {/* 系统信息 */}
      <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
        <Card>
          <CardHeader>
            <h3 className="text-lg font-semibold">系统信息</h3>
          </CardHeader>
          <CardBody>
            <div className="space-y-3">
              <div className="flex justify-between">
                <span className="text-default-500">微信版本</span>
                <span>{wechatVersion}</span>
              </div>
              <div className="flex justify-between">
                <span className="text-default-500">登录用户</span>
                <span>{user?.nickname || "未登录"}</span>
              </div>
              <div className="flex justify-between">
                <span className="text-default-500">用户 ID</span>
                <span>{user?.wxid || "-"}</span>
              </div>
              <div className="flex justify-between">
                <span className="text-default-500">引擎版本</span>
                <span>1.0.0</span>
              </div>
            </div>
          </CardBody>
        </Card>

        <Card>
          <CardHeader>
            <h3 className="text-lg font-semibold">今日统计</h3>
          </CardHeader>
          <CardBody>
            <div className="space-y-4">
              <div>
                <div className="flex justify-between mb-1">
                  <span className="text-sm text-default-500">发送消息</span>
                  <span className="text-sm">45 / 100</span>
                </div>
                <Progress value={45} color="primary" size="sm" />
              </div>
              <div>
                <div className="flex justify-between mb-1">
                  <span className="text-sm text-default-500">朋友圈发布</span>
                  <span className="text-sm">2 / 5</span>
                </div>
                <Progress value={40} color="secondary" size="sm" />
              </div>
              <div>
                <div className="flex justify-between mb-1">
                  <span className="text-sm text-default-500">点赞</span>
                  <span className="text-sm">15 / 50</span>
                </div>
                <Progress value={30} color="success" size="sm" />
              </div>
            </div>
          </CardBody>
        </Card>
      </div>

      {/* 实时日志 */}
      <Card>
        <CardHeader>
          <h3 className="text-lg font-semibold">实时日志</h3>
        </CardHeader>
        <CardBody>
          <div className="bg-content2 rounded-lg p-4 font-mono text-sm h-48 overflow-auto">
            <div className="text-default-500">[10:30:00] 系统启动</div>
            <div className="text-success">[10:30:01] 引擎连接成功</div>
            <div className="text-default-500">[10:30:02] 微信窗口已找到</div>
            <div className="text-primary">[10:31:00] 消息已发送给 张三</div>
            <div className="text-primary">[10:32:00] 收到新消息 from 李四</div>
            <div className="text-warning">[10:33:00] 风控等级: 低风险</div>
          </div>
        </CardBody>
      </Card>
    </div>
  );
}
