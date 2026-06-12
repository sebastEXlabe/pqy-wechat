import {
  Navbar,
  NavbarContent,
  NavbarItem,
  Badge,
  Avatar,
  Dropdown,
  DropdownTrigger,
  DropdownMenu,
  DropdownItem,
} from "@heroui/react";
import { Bell, Wifi, WifiOff } from "lucide-react";
import { useSystemStore } from "@/stores/useSystemStore";

export function Header() {
  const { isConnected, riskLevel } = useSystemStore();

  return (
    <Navbar
      isBordered
      className="bg-content1"
      maxWidth="full"
      height="56px"
    >
      <NavbarContent justify="start">
        <NavbarItem>
          <h1 className="text-lg font-semibold">PQY 微信自动化</h1>
        </NavbarItem>
      </NavbarContent>

      <NavbarContent justify="end">
        {/* 连接状态 */}
        <NavbarItem>
          {isConnected ? (
            <Wifi className="text-success" size={20} />
          ) : (
            <WifiOff className="text-danger" size={20} />
          )}
        </NavbarItem>

        {/* 风控等级 */}
        <NavbarItem>
          <div
            className={cn(
              "px-2 py-1 rounded text-xs font-medium",
              riskLevel === "low" && "bg-success/20 text-success",
              riskLevel === "medium" && "bg-warning/20 text-warning",
              riskLevel === "high" && "bg-danger/20 text-danger"
            )}
          >
            {riskLevel === "low" ? "低风险" : riskLevel === "medium" ? "中风险" : "高风险"}
          </div>
        </NavbarItem>

        {/* 通知 */}
        <NavbarItem>
          <Badge content="3" color="danger">
            <Bell size={20} className="text-default-500 cursor-pointer" />
          </Badge>
        </NavbarItem>

        {/* 用户头像 */}
        <NavbarItem>
          <Dropdown placement="bottom-end">
            <DropdownTrigger>
              <Avatar
                isBordered
                as="button"
                className="transition-transform"
                name="User"
                size="sm"
              />
            </DropdownTrigger>
            <DropdownMenu aria-label="Profile Actions" variant="flat">
              <DropdownItem key="profile" className="h-14 gap-2">
                <p className="font-semibold">已登录</p>
                <p className="text-xs text-default-500">wxid_my123</p>
              </DropdownItem>
              <DropdownItem key="settings">设置</DropdownItem>
              <DropdownItem key="logout" color="danger">
                退出登录
              </DropdownItem>
            </DropdownMenu>
          </Dropdown>
        </NavbarItem>
      </NavbarContent>
    </Navbar>
  );
}
