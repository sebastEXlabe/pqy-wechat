#include <windows.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>
#include <string>

namespace pqy {

// 日志初始化
void InitLogger(const std::string& log_file, bool console_output) {
    try {
        std::vector<spdlog::sink_ptr> sinks;

        // 文件输出
        if (!log_file.empty()) {
            auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                log_file, 1024 * 1024 * 10, 3  // 10MB, 3个文件
            );
            sinks.push_back(file_sink);
        }

        // 控制台输出
        if (console_output) {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            sinks.push_back(console_sink);
        }

        // 创建 logger
        auto logger = std::make_shared<spdlog::logger>("pqy", sinks.begin(), sinks.end());
        logger->set_level(spdlog::level::debug);
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [thread %t] %v");

        // 注册为默认 logger
        spdlog::set_default_logger(logger);

        spdlog::info("Logger initialized");
    }
    catch (const spdlog::spdlog_ex& ex) {
        // 日志初始化失败，输出到调试器
        OutputDebugStringA(("Logger init failed: " + std::string(ex.what()) + "\n").c_str());
    }
}

// 关闭日志
void ShutdownLogger() {
    spdlog::shutdown();
}

} // namespace pqy
