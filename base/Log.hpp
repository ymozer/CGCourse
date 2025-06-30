#pragma once

#include <spdlog/spdlog.h>
#include <memory>
#include <mutex>

// Platform-specific sink includes
#if PLATFORM_ANDROID
    #include <spdlog/sinks/android_sink.h>
#else
    #include <spdlog/sinks/stdout_color_sinks.h>
    #include <spdlog/sinks/basic_file_sink.h>
    #include <spdlog/sinks/rotating_file_sink.h>
    #include <filesystem>
#endif

struct LoggerConfig
{
    std::string loggerName = "AppLogger";
    spdlog::level::level_enum consoleLogLevel = spdlog::level::trace;
    spdlog::level::level_enum fileLogLevel = spdlog::level::debug;
    spdlog::level::level_enum flushLevel = spdlog::level::warn;
    std::string logPattern = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v";
    bool enableConsoleLogging = true;

    // Desktop-only options
#if !PLATFORM_ANDROID
    bool enableFileLogging = true;
    bool enableColor = true;
    std::string logDirectory = "logs";
    std::string logFilename = "app.log";
    bool useRotatingFiles = true;
    size_t maxFileSize = 1024 * 1024 * 5;
    size_t maxFiles = 3;
#endif
};

class Logger
{
public:
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    static Logger &getInstance();
    static void shutdown();

    void initialize(const LoggerConfig &config = {});

    std::shared_ptr<spdlog::logger> getSpdlogLogger();

    template <typename... Args>
    void trace(fmt::format_string<Args...> fmt, Args &&...args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_logger) m_logger->trace(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void debug(fmt::format_string<Args...> fmt, Args &&...args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_logger) m_logger->debug(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void info(fmt::format_string<Args...> fmt, Args &&...args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_logger) m_logger->info(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warn(fmt::format_string<Args...> fmt, Args &&...args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_logger) m_logger->warn(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(fmt::format_string<Args...> fmt, Args &&...args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_logger) m_logger->error(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void critical(fmt::format_string<Args...> fmt, Args &&...args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_logger) m_logger->critical(fmt, std::forward<Args>(args)...);
    }

    void setLevel(spdlog::level::level_enum level);
    spdlog::level::level_enum getLevel();

    void flush();
    void setFlushLevel(spdlog::level::level_enum level);

private:
    Logger();
    ~Logger();

    std::shared_ptr<spdlog::logger> m_logger;
    std::mutex m_mutex;
    std::once_flag m_init_flag;
};

#define LOG_TRACE(...) Logger::getInstance().trace(__VA_ARGS__)
#define LOG_DEBUG(...) Logger::getInstance().debug(__VA_ARGS__)
#define LOG_INFO(...) Logger::getInstance().info(__VA_ARGS__)
#define LOG_WARN(...) Logger::getInstance().warn(__VA_ARGS__)
#define LOG_ERROR(...) Logger::getInstance().error(__VA_ARGS__)
#define LOG_CRITICAL(...) Logger::getInstance().critical(__VA_ARGS__)