import { NavLink } from "react-router-dom";
import {
  LayoutDashboard,
  MessageCircle,
  Users,
  Compass,
  Bot,
  Settings,
} from "lucide-react";
import { cn } from "@/utils/cn";

const navItems = [
  { path: "/", icon: LayoutDashboard, label: "仪表盘" },
  { path: "/chat", icon: MessageCircle, label: "聊天" },
  { path: "/contacts", icon: Users, label: "联系人" },
  { path: "/moments", icon: Compass, label: "朋友圈" },
  { path: "/ai", icon: Bot, label: "AI Agent" },
  { path: "/settings", icon: Settings, label: "设置" },
];

export function Sidebar() {
  return (
    <aside className="w-16 bg-content1 border-r border-divider flex flex-col items-center py-4 gap-2">
      {/* Logo */}
      <div className="w-10 h-10 bg-primary rounded-lg flex items-center justify-center mb-4">
        <span className="text-primary-foreground font-bold text-lg">P</span>
      </div>

      {/* 导航项 */}
      {navItems.map((item) => (
        <NavLink
          key={item.path}
          to={item.path}
          end={item.path === "/"}
          className={({ isActive }) =>
            cn(
              "w-12 h-12 flex flex-col items-center justify-center rounded-lg transition-colors",
              "hover:bg-primary/10",
              isActive ? "bg-primary/20 text-primary" : "text-default-500"
            )
          }
          title={item.label}
        >
          <item.icon size={20} />
          <span className="text-[10px] mt-1">{item.label}</span>
        </NavLink>
      ))}
    </aside>
  );
}
