# XPlugin 注入设计

## 概述

XPlugin 是微信的插件系统，用于加载小程序、游戏等扩展功能。通过将我们的代码注册为 XPlugin，可以合法地访问微信原生 API，而不需要 DLL 注入。

## 技术原理

```
┌─────────────────────────────────────────────────────────────┐
│                    微信主进程                                 │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                 XPlugin Manager                     │   │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐            │   │
│  │  │ Plugin1 │  │ Plugin2 │  │ PQY     │            │   │
│  │  │ (小程序) │  │ (游戏)  │  │ Plugin  │ ◄── 我们   │   │
│  │  └─────────┘  └─────────┘  └─────────┘            │   │
│  └─────────────────────────────────────────────────────┘   │
│                          │                                  │
│                          ▼                                  │
│  ┌─────────────────────────────────────────────────────┐   │
│  │              XPlugin API Layer                       │   │
│  │  - 消息发送     - 联系人访问     - 朋友圈操作        │   │
│  │  - 文件传输     - 支付接口       - 小程序接口        │   │
│  └─────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

---

## XPlugin 机制分析

### 关键字符串

| 字符串 | 地址 | 说明 |
|--------|------|------|
| `xplugin` | 0x1889b137b | 插件标识 |
| `wechat_call_xplugin_update_event` | 0x188285a25 | 更新事件 |
| `XPluginCdnDownloaderImpl::AddDownloadTask` | 0x188886b9c | CDN 下载 |
| `DebugXPluginCommandWindow` | 0x1889b0f52 | 调试窗口 |
| `xplugin_rollback_io_thread` | 0x188ac4025 | 回滚线程 |

### 插件生命周期

```
注册 → 初始化 → 运行 → 更新 → 销毁
  │        │       │       │       │
  ▼        ▼       ▼       ▼       ▼
Register  Init   Start   Update  Destroy
```

---

## 插件注册

### 1. 插件描述文件

```json
{
  "name": "pqy-automation",
  "version": "1.0.0",
  "description": "PQY 微信自动化插件",
  "author": "PQY",
  "min_wechat_version": "4.1.10.31",
  "permissions": [
    "message.read",
    "message.send",
    "contact.read",
    "moments.read",
    "moments.write"
  ],
  "entry": "pqy_plugin.dll"
}
```

### 2. 插件入口函数

```cpp
// 插件入口
extern "C" __declspec(dllexport)
int XPluginMain(XPluginContext* ctx) {
    // 初始化
    ctx->SetPluginInfo("pqy-automation", "1.0.0");
    
    // 注册回调
    ctx->RegisterCallback("onMessage", OnMessageCallback);
    ctx->RegisterCallback("onContact", OnContactCallback);
    ctx->RegisterCallback("onMoments", OnMomentsCallback);
    
    // 注册 API
    ctx->RegisterAPI("sendMessage", SendMessageAPI);
    ctx->RegisterAPI("getContacts", GetContactsAPI);
    ctx->RegisterAPI("getMoments", GetMomentsAPI);
    
    return 0;
}
```

---

## API 实现

### 消息 API

```cpp
// 发送消息
XPluginResult* SendMessageAPI(XPluginArgs* args) {
    // 获取参数
    const char* to = args->GetString("to");
    const char* content = args->GetString("content");
    const char* type = args->GetString("type", "text");
    
    // 调用微信原生 API
    bool result = WechatNativeAPI::SendMessage(to, content, type);
    
    // 返回结果
    XPluginResult* result = new XPluginResult();
    result->SetBool("success", result);
    return result;
}

// 接收消息回调
void OnMessageCallback(const char* event, XPluginData* data) {
    // 解析消息
    const char* from = data->GetString("from");
    const char* content = data->GetString("content");
    const char* type = data->GetString("type");
    
    // 发送到代理层
    ProxyLayer::GetInstance()->OnMessageReceived(from, content, type);
}
```

### 联系人 API

```cpp
// 获取联系人
XPluginResult* GetContactsAPI(XPluginArgs* args) {
    const char* keyword = args->GetString("keyword", "");
    
    // 调用微信原生 API
    json contacts = WechatNativeAPI::GetContacts(keyword);
    
    XPluginResult* result = new XPluginResult();
    result->SetJSON("contacts", contacts);
    return result;
}
```

### 朋友圈 API

```cpp
// 获取朋友圈
XPluginResult* GetMomentsAPI(XPluginArgs* args) {
    int limit = args->GetInt("limit", 20);
    
    // 调用微信原生 API
    json moments = WechatNativeAPI::GetMoments(limit);
    
    XPluginResult* result = new XPluginResult();
    result->SetJSON("moments", moments);
    return result;
}

// 发布朋友圈
XPluginResult* PostMomentsAPI(XPluginArgs* args) {
    const char* content = args->GetString("content");
    json images = args->GetJSON("images", json::array());
    
    // 调用微信原生 API
    bool result = WechatNativeAPI::PostMoments(content, images);
    
    XPluginResult* result = new XPluginResult();
    result->SetBool("success", result);
    return result;
}
```

---

## 插件加载器

```cpp
class XPluginLoader {
public:
    // 加载插件
    bool LoadPlugin(const std::string& path) {
        // 加载 DLL
        HMODULE hModule = LoadLibraryA(path.c_str());
        if (!hModule) return false;
        
        // 获取入口函数
        XPluginMainFunc entry = (XPluginMainFunc)GetProcAddress(
            hModule, "XPluginMain"
        );
        if (!entry) {
            FreeLibrary(hModule);
            return false;
        }
        
        // 创建上下文
        XPluginContext* ctx = new XPluginContext();
        
        // 调用入口
        int result = entry(ctx);
        if (result != 0) {
            delete ctx;
            FreeLibrary(hModule);
            return false;
        }
        
        // 注册插件
        plugins_[ctx->GetName()] = {
            hModule,
            ctx,
            path
        };
        
        return true;
    }
    
    // 卸载插件
    void UnloadPlugin(const std::string& name) {
        auto it = plugins_.find(name);
        if (it == plugins_.end()) return;
        
        // 调用销毁
        it->second.ctx->OnDestroy();
        
        // 清理
        delete it->second.ctx;
        FreeLibrary(it->second.handle);
        
        plugins_.erase(it);
    }
    
private:
    struct PluginInfo {
        HMODULE handle;
        XPluginContext* ctx;
        std::string path;
    };
    
    std::map<std::string, PluginInfo> plugins_;
};
```

---

## 与 API 网关集成

```cpp
// 在 API 网关中暴露 XPlugin 功能
class XPluginAPI {
public:
    static void RegisterRoutes(Fastify& app) {
        // 发送消息
        app.post("/api/xplugin/send", [](Request& req, Response& res) {
            json body = json::parse(req.body());
            
            XPluginArgs* args = new XPluginArgs();
            args->SetString("to", body["to"]);
            args->SetString("content", body["content"]);
            
            XPluginResult* result = g_plugin->CallAPI("sendMessage", args);
            
            res.json(result->ToJSON());
            
            delete args;
            delete result;
        });
        
        // 获取联系人
        app.get("/api/xplugin/contacts", [](Request& req, Response& res) {
            std::string keyword = req.query("keyword", "");
            
            XPluginArgs* args = new XPluginArgs();
            args->SetString("keyword", keyword);
            
            XPluginResult* result = g_plugin->CallAPI("getContacts", args);
            
            res.json(result->ToJSON());
            
            delete args;
            delete result;
        });
    }
};
```

---

## 调试支持

### DebugXPluginCommandWindow

微信提供了调试用的插件命令窗口，可以用来测试插件功能。

```cpp
// 打开调试窗口
void OpenDebugWindow() {
    // 查找调试窗口函数
    auto func = GetProcAddress(g_wechat_module, "DebugXPluginCommandWindow");
    if (func) {
        ((void(*)())func)();
    }
}
```

### 日志输出

```cpp
// 插件日志
void PluginLog(int level, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format, args);
    
    // 输出到微信日志
    XLogger::Log(level, "PQY", buffer);
    
    // 输出到我们的日志
    OurLogger::Log(level, "Plugin", buffer);
    
    va_end(args);
}
```

---

## 安全考虑

1. **权限最小化** — 只申请必要的权限
2. **签名验证** — 验证插件签名
3. **沙箱隔离** — 限制插件访问范围
4. **异常处理** — 捕获所有异常，避免崩溃

---

## 下一步

- [Hook 内核层](hook-layer.md) — 风控规避
- [Mmmojo 代理](mmmojo-proxy.md) — IPC 消息代理
- [MCP 协议](mcp-protocol.md) — AI 接口
