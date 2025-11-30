#pragma once

#include <stdio.h>
#include <string>
#include <stdint.h>


enum class LogOutput
{
    CONSOLE,
    FILE_STREAM
};

class Logger
{
public:
    void debug_msg(uint32_t line, const char* file, const std::string msg, ...);
    void info_msg(uint32_t line, const char* file, const std::string msg, ...);
    void error_msg(uint32_t line, const char* file, const std::string msg, ...);

    Logger(bool debug, bool info, LogOutput output);
    ~Logger();

    bool enable_debug() const { return m_debug; }
    bool enable_info() const { return m_info; }

private:
    bool m_debug = true;
    bool m_info = true;
    LogOutput m_output;
    FILE *m_f = nullptr;

    enum class Level
    {
        DEBUG,
        INFO,
        ERROR
    };

    void log(uint32_t line, const char* file, Level level, const std::string& msg, va_list& ap);
};

const char* strip_prefix(const char* path);

#define LOG_DEBUG(logger, ...) if (logger.enable_debug()) { logger.debug_msg(__LINE__, strip_prefix(__FILE__), __VA_ARGS__); }
#define LOG_INFO(logger, ...)  if (logger.enable_info()) { logger.info_msg(__LINE__, strip_prefix(__FILE__), __VA_ARGS__); }
#define LOG_ERROR(logger, ...)  logger.error_msg(__LINE__, strip_prefix(__FILE__), __VA_ARGS__)
