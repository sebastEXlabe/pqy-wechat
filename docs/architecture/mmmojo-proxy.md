# Mmmojo 代理设计

## 概述

Mmmojo 是微信的进程间通信（IPC）框架。通过代理 Mmmojo 消息，我们可以拦截和重放微信的内部通信，实现对微信功能的访问。

## 技术原理

```
┌─────────────────┐                    ┌─────────────────┐
│   微信主进程     │                    │   微信子进程     │
│                 │     Mmmojo IPC     │                 │
│  ┌───────────┐  │ ◄═══════════════► │  ┌───────────┐  │
│  │ Mmmojo    │  │                    │  │ Mmmojo    │  │
│  │ Endpoint  │  │                    │  │ Endpoint  │  │
│  └───────────┘  │                    │  └───────────┘  │
└─────────────────┘                    └─────────────────┘
         │                                       │
         │            ┌─────────┐                │
         └───────────►│ Proxy   │◄───────────────┘
                      │ Layer   │
                      └────┬────┘
                           │
                           ▼
                    ┌─────────────┐
                    │  PQY API    │
                    │  Gateway    │
                    └─────────────┘
```

---

## Mmmojo 消息格式

### 消息结构

```cpp
struct MmmojoMessage {
    uint32_t magic;           // 魔数：0x4D4D4A52 ("MMJR")
    uint32_t version;         // 版本号
    uint32_t type;            // 消息类型
    uint32_t sequence;        // 序列号
    uint32_t payload_size;    // 载荷大小
    uint8_t  payload[];       // 可变长度载荷
};
```

### 消息类型

```cpp
enum class MmmojoMessageType : uint32_t {
    // 连接管理
    CONNECT_REQUEST     = 0x0001,
    CONNECT_RESPONSE    = 0x0002,
    DISCONNECT          = 0x0003,
    
    // 数据传输
    DATA_REQUEST        = 0x0010,
    DATA_RESPONSE       = 0x0011,
    DATA_PUSH           = 0x0012,
    
    // 功能调用
    FUNCTION_CALL       = 0x0020,
    FUNCTION_RESULT     = 0x0021,
    
    // 事件通知
    EVENT_NOTIFY        = 0x0030,
    EVENT_ACK           = 0x0031,
};
```

---

## 关键 API

### 已识别的 Mmmojo 函数

| 函数 | 地址 | 用途 |
|------|------|------|
| `CreateMMMojoEnvironment` | 0x188ee370c | 创建 Mojo 环境 |
| `CreateMMMojoWriteInfo` | 0x188ee3726 | 创建写入信息 |
| `GetMMMojoReadInfoAttach` | 0x188ee373e | 获取读取附件 |
| `GetMMMojoReadInfoRequest` | 0x188ee3758 | 获取读取请求 |
| `SendMMMojoWriteInfo` | mmmojo_64.dll | 发送写入信息 |

---

## 代理实现

### 1. 消息拦截器

```cpp
class MmmojoInterceptor {
public:
    // Hook SendMMMojoWriteInfo
    static bool Hook_SendMMMojoWriteInfo(
        void* env,
        void* write_info,
        uint32_t channel
    ) {
        // 解析消息
        MmmojoMessage* msg = ParseWriteInfo(write_info);
        
        // 记录消息
        LogMessage(msg, Direction::OUTGOING);
        
        // 发送到代理层
        ProxyLayer::GetInstance()->OnMessageSent(msg);
        
        // 调用原始函数
        return Original_SendMMMojoWriteInfo(env, write_info, channel);
    }
    
    // Hook GetMMMojoReadInfoRequest
    static void* Hook_GetMMMojoReadInfoRequest(void* read_info) {
        void* result = Original_GetMMMojoReadInfoRequest(read_info);
        
        // 解析响应
        MmmojoMessage* msg = ParseReadInfo(read_info);
        
        // 记录消息
        LogMessage(msg, Direction::INCOMING);
        
        // 发送到代理层
        ProxyLayer::GetInstance()->OnMessageReceived(msg);
        
        return result;
    }
};
```

### 2. 消息解析器

```cpp
class MmmojoParser {
public:
    // 解析消息
    static MmmojoMessage* Parse(const uint8_t* data, size_t size) {
        if (size < sizeof(MmmojoMessage)) {
            return nullptr;
        }
        
        MmmojoMessage* msg = (MmmojoMessage*)data;
        
        // 验证魔数
        if (msg->magic != 0x4D4D4A52) {
            return nullptr;
        }
        
        return msg;
    }
    
    // 解析载荷
    static json ParsePayload(const MmmojoMessage* msg) {
        // 根据消息类型解析载荷
        switch (msg->type) {
            case MmmojoMessageType::DATA_REQUEST:
                return ParseDataRequest(msg->payload, msg->payload_size);
            case MmmojoMessageType::FUNCTION_CALL:
                return ParseFunctionCall(msg->payload, msg->payload_size);
            default:
                return json::object();
        }
    }
    
    // 构造消息
    static MmmojoMessage* Build(
        MmmojoMessageType type,
        uint32_t sequence,
        const json& payload
    ) {
        std::string payload_str = payload.dump();
        size_t msg_size = sizeof(MmmojoMessage) + payload_str.size();
        
        MmmojoMessage* msg = (MmmojoMessage*)malloc(msg_size);
        msg->magic = 0x4D4D4A52;
        msg->version = 1;
        msg->type = (uint32_t)type;
        msg->sequence = sequence;
        msg->payload_size = payload_str.size();
        memcpy(msg->payload, payload_str.c_str(), payload_str.size());
        
        return msg;
    }
};
```

### 3. 消息重放器

```cpp
class MmmojoReplayer {
public:
    // 发送消息到微信
    static bool SendMessageToWechat(const MmmojoMessage* msg) {
        // 创建 WriteInfo
        void* write_info = Original_CreateMMMojoWriteInfo();
        if (!write_info) return false;
        
        // 填充消息数据
        FillWriteInfo(write_info, msg);
        
        // 发送
        bool result = Original_SendMMMojoWriteInfo(
            g_mmojo_env,
            write_info,
            DEFAULT_CHANNEL
        );
        
        // 清理
        DestroyWriteInfo(write_info);
        
        return result;
    }
    
    // 发送功能调用
    static json CallFunction(const std::string& func_name, const json& args) {
        // 构造调用消息
        json payload = {
            {"function", func_name},
            {"arguments", args}
        };
        
        MmmojoMessage* msg = MmmojoParser::Build(
            MmmojoMessageType::FUNCTION_CALL,
            GetNextSequence(),
            payload
        );
        
        // 发送并等待响应
        json response = SendAndWaitResponse(msg);
        
        free(msg);
        return response;
    }
};
```

---

## 功能映射

### 消息相关

```cpp
// 发送消息
json SendMessage(const std::string& to, const std::string& content) {
    return MmmojoReplayer::CallFunction("SendMessage", {
        {"to", to},
        {"content", content},
        {"type", "text"}
    });
}

// 获取消息历史
json GetMessages(const std::string& chat, int limit) {
    return MmmojoReplayer::CallFunction("GetMessages", {
        {"chat", chat},
        {"limit", limit}
    });
}
```

### 联系人相关

```cpp
// 获取联系人列表
json GetContacts(const std::string& keyword) {
    return MmmojoReplayer::CallFunction("GetContacts", {
        {"keyword", keyword}
    });
}

// 获取群聊列表
json GetGroups(const std::string& keyword) {
    return MmmojoReplayer::CallFunction("GetGroups", {
        {"keyword", keyword}
    });
}
```

---

## 通道管理

```cpp
class ChannelManager {
public:
    // 注册通道
    void RegisterChannel(uint32_t id, const std::string& name) {
        channels_[id] = name;
    }
    
    // 获取通道名称
    std::string GetChannelName(uint32_t id) {
        auto it = channels_.find(id);
        return it != channels_.end() ? it->second : "Unknown";
    }
    
    // 监听通道
    void ListenChannel(uint32_t id, MessageCallback callback) {
        listeners_[id].push_back(callback);
    }
    
private:
    std::map<uint32_t, std::string> channels_;
    std::map<uint32_t, std::vector<MessageCallback>> listeners_;
};
```

---

## 集成到 API 层

```cpp
// 在 API 网关中暴露 Mmmojo 功能
class MmmojoAPI {
public:
    // 注册到 Fastify
    static void RegisterRoutes(Fastify& app) {
        // 发送消息
        app.post("/api/mmmojo/send", [](Request& req, Response& res) {
            json body = json::parse(req.body());
            json result = MmmojoReplayer::CallFunction(
                "SendMessage", body
            );
            res.json(result);
        });
        
        // 获取联系人
        app.get("/api/mmmojo/contacts", [](Request& req, Response& res) {
            std::string keyword = req.query("keyword", "");
            json result = MmmojoReplayer::CallFunction(
                "GetContacts", {{"keyword", keyword}}
            );
            res.json(result);
        });
    }
};
```

---

## 下一步

- [XPlugin 注入](xplugin-inject.md) — 原生 API 访问
- [Hook 内核层](hook-layer.md) — 风控规避
- [MCP 协议](mcp-protocol.md) — AI 接口
