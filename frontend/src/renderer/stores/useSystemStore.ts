import { create } from "zustand";

interface SystemState {
  // 连接状态
  isConnected: boolean;
  engineStatus: "running" | "stopped" | "error";

  // 风控状态
  riskLevel: "low" | "medium" | "high";

  // 用户信息
  user: {
    nickname: string;
    wxid: string;
    avatar: string;
  } | null;

  // 系统信息
  wechatVersion: string;
  uptime: string;

  // 操作
  setConnected: (connected: boolean) => void;
  setEngineStatus: (status: "running" | "stopped" | "error") => void;
  setRiskLevel: (level: "low" | "medium" | "high") => void;
  setUser: (user: SystemState["user"]) => void;

  // 异步操作
  fetchStatus: () => Promise<void>;
}

export const useSystemStore = create<SystemState>((set) => ({
  isConnected: false,
  engineStatus: "stopped",
  riskLevel: "low",
  user: null,
  wechatVersion: "4.1.10.31",
  uptime: "0m",

  setConnected: (connected) => set({ isConnected: connected }),
  setEngineStatus: (status) => set({ engineStatus: status }),
  setRiskLevel: (level) => set({ riskLevel: level }),
  setUser: (user) => set({ user }),

  fetchStatus: async () => {
    try {
      const response = await fetch("/api/system/status");
      const data = await response.json();

      set({
        isConnected: data.connected,
        engineStatus: data.engine_status,
        riskLevel: data.risk_level,
        user: data.user,
        wechatVersion: data.wechat_version,
        uptime: data.uptime,
      });
    } catch (error) {
      console.error("Failed to fetch system status:", error);
      set({ isConnected: false, engineStatus: "error" });
    }
  },
}));
