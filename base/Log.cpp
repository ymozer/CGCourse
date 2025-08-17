#include "Log.hpp"
#include "PathUtils.hpp"
#include <vector>

Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
{
#if PLATFORM_ANDROID
    auto bootstrap_sink = std::make_shared<spdlog::sinks::android_sink_mt>("BOOTSTRAP");
#else
    auto bootstrap_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
#endif
    bootstrap_sink->set_level(spdlog::level::trace);
    m_logger = std::make_shared<spdlog::logger>("Bootstrap", bootstrap_sink);
    m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");
}

Logger::~Logger() {}

void Logger::shutdown()
{
    Logger &instance = getInstance();
    std::lock_guard<std::mutex> lock(instance.m_mutex);
    if (instance.m_logger)
    {
        LOG_INFO("Logger shutdown requested.");
        instance.m_logger->flush();
        instance.m_logger = nullptr;
    }
    spdlog::shutdown();
}

void Logger::initialize(const LoggerConfig &config)
{
    std::call_once(m_init_flag, [&]()
                   {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(m_logger) m_logger->info("Logger initialization started...");

        try {
            std::vector<spdlog::sink_ptr> sinks;

#if PLATFORM_ANDROID
            // On Android, the primary sink should be the logcat sink.
            if (config.enableConsoleLogging) {
                // The tag "AppLogger" will appear in logcat.
                auto android_sink = std::make_shared<spdlog::sinks::android_sink_mt>(config.loggerName);
                android_sink->set_level(config.consoleLogLevel);
                sinks.push_back(android_sink);
            }
#else
            if (config.enableConsoleLogging) {
                auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
                console_sink->set_level(config.consoleLogLevel);
                console_sink->set_color_mode(config.enableColor ? spdlog::color_mode::always : spdlog::color_mode::never);
                sinks.push_back(console_sink);
            }
            if (config.enableFileLogging) {
                std::string logFileString = getPrefPath(config.logFilename.c_str());
                std::filesystem::path logFilePath = logFileString;
                if (m_logger) m_logger->warn("Log file path: {}", logFilePath.string());

                std::filesystem::create_directories(logFilePath.parent_path());
                if (config.useRotatingFiles) {
                    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                        logFilePath.string(), config.maxFileSize, config.maxFiles);
                    file_sink->set_level(config.fileLogLevel);
                    sinks.push_back(file_sink);
                } else {
                    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
                        logFilePath.string(), true);
                    file_sink->set_level(config.fileLogLevel);
                    sinks.push_back(file_sink);
                }
            }
#endif
            if (sinks.empty()) {
                if (m_logger) m_logger->warn("No sinks configured. Logging will be ineffective.");
            }

            auto new_logger = std::make_shared<spdlog::logger>(config.loggerName, sinks.begin(), sinks.end());
            new_logger->set_level(spdlog::level::trace);
            new_logger->set_pattern(config.logPattern);
            new_logger->flush_on(config.flushLevel);
            
            m_logger = new_logger;
            spdlog::register_logger(m_logger);
            
            m_logger->info("--- Logger '{}' initialized successfully. ---", config.loggerName);

        } catch (const spdlog::spdlog_ex& ex) {
            if (m_logger) m_logger->critical("Logger initialization failed: {}", ex.what());
        } });
}

std::shared_ptr<spdlog::logger> Logger::getSpdlogLogger()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_logger;
}

void Logger::setLevel(spdlog::level::level_enum level)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_logger)
    {
        m_logger->set_level(level);
    }
}

void Logger::flush()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_logger)
    {
        m_logger->flush();
    }
}

void Logger::setFlushLevel(spdlog::level::level_enum level)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_logger)
    {
        m_logger->flush_on(level);
    }
}

spdlog::level::level_enum Logger::getLevel()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_logger)
    {
        return m_logger->level();
    }
    return spdlog::level::off;
}