# Phase 1 静态提取结果

## 全局变量初始运行时值

| 变量 | 地址 | 初始值 | 含义 |
|------|------|--------|------|
| `dword_18A3B2B20` | SignallingKeeper | 5000 (5秒) | 心跳发送周期 |
| `dword_18A3B2B24` | SignallingKeeper | 20000 (20秒) | 保活超时 |
| `dword_18A79A200` | SmartHeartbeat | -1 (unset) | 从INI加载 |
| `dword_18A891E60` | Watchdog | 0 | 等待首次触发 |
| `byte_18A6566C0` | Flag | 1 | 已启用 |
| `qword_18A8997F8` | HTTP虚表 | 0 (运行时设置) | — |

## StnManager → StnCallbackBridge 调用链偏移量

```
StnManager对象:
  +0x00: vtable ptr
  +0x58: callback_bridge_ ptr (a1[11].Ptr)
  +0x60: SRWLock (a1+12) — 保护 callback_bridge_
  +0x1B10: 另一个回调指针
  +0x1B30: mmtls_state (0=初始,1=错误,4=完成)

StnCallbackBridge VTable (19虚函数):
  vtable[1] +0x08: SetCallback
  vtable[2] +0x10: MakesureAuthed      ← AcquireSRWLockShared(a1+0x60)
  vtable[3] +0x18: TrafficData
  vtable[4] +0x20: OnNewDns
  vtable[5] +0x28: OnPush
  vtable[6] +0x30: Req2Buf             ← 请求序列化Hook点
  vtable[7] +0x38: Buf2Resp            ← 响应反序列化Hook点
  vtable[8] +0x40: OnTaskEnd           ← 任务结束Hook点
  ... (剩余11个)
```

## MMKV 存储键

| Key | 用途 |
|-----|------|
| mmkv_key_user_name | 用户名 |
| mmkv_key_push_login_url_expired_time | 推送登录URL过期时间 |
| mmkv_key_auto_auth_key | 自动认证密钥 |
| mmkv_key_pc_account_name | PC账号名 |
| mmkv_key_old_wechat_auto_login_loaded | 旧版自动登录加载标志 |
| mmkv_key_latest_login_uin | 最近登录UIN |
| mmkv_key_latest_login_username | 最近登录用户名 |
| mmkv_key_notify_device_name | 通知设备名 |
| mmkv_flowlayer_quitguide_use_times_in_day | 退出引导每日使用次数 |
| mmkv_flowlayer_quitguide_show_times_in_day | 退出引导每日展示次数 |

## 风控配置键

| Key | 类别 |
|-----|------|
| rpa_strike_for_pc | RPA打击 |
| pc_forbid_screenshot | 截图禁止 |
| clicfg_xwechat_agent_check | Agent检测 |
| clicfg_xwechat_agent_protection_level | Agent防护等级 |
| silent_restart | 静默重启 |
| max-retry-count | 最大重试 |
| max_page_count | 最大页数 |
| times_click_per_day | 每日点击 |
| max_times_per_day | 每日最大操作 |
