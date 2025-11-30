#include <Logger.hpp>
#include <cassert>
#include <cstdint>
#include <ctime>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <cstring>

static int count_loggers = 0;


Logger::Logger(bool debug, bool info, LogOutput output)
    : m_debug(debug)
    , m_info(info)
    , m_output(output)
{
    count_loggers++;
    assert(count_loggers == 1);

    switch (output)
    {
    case LogOutput::CONSOLE:
        m_f = stdout;
        break;

    case LogOutput::FILE_STREAM: {
        const char* filename = "/var/log/flexaudio.log";
        m_f = fopen(filename, "w");
        assert(m_f != nullptr);
        fprintf(stderr, "logging to %s\n", filename);
        break;
    }
    }
}


Logger::~Logger()
{
    count_loggers--;

    switch (m_output)
    {
    case LogOutput::CONSOLE:
        break;
    case LogOutput::FILE_STREAM:
        fclose(m_f);
        break;
    }
}


void Logger::debug_msg(
    uint32_t line, const char* file, const std::string msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    log(line, file, Level::DEBUG, msg, ap);
    va_end(ap);
}

void Logger::info_msg(
    uint32_t line, const char* file, const std::string msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    log(line, file, Level::INFO, msg, ap);
    va_end(ap);
}

void Logger::error_msg(
    uint32_t line, const char* file, const std::string msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    log(line, file, Level::ERROR, msg, ap);
    va_end(ap);
}

void Logger::log(uint32_t line, const char* file, Level level,
    const std::string& msg, va_list& ap)
{
    const char* level_str = "";
    switch (level)
    {
    case Level::DEBUG:
        level_str = "DEBUG";
        break;
    case Level::INFO:
        level_str = "INFO";
        break;
    case Level::ERROR:
        level_str = "ERROR";
        break;
    }

    std::time_t time = std::time({});
    char timeString[128];
    std::strftime(std::data(timeString), std::size(timeString), "%T",
        std::gmtime(&time));

    char str[1024];
    vsnprintf(str, sizeof(str), msg.c_str(), ap);
    fprintf(
        m_f, "%s:%d: [%s] %s - %s\n", file, line, timeString, level_str, str);

    if (level == Level::ERROR)
    {
        fflush(m_f);
    }
}


const char* strip_prefix(const char* path)
{
    if (const char *src = strstr(path, "src/"))
    {
        return src;
    }

    if (const char *src = strstr(path, "include/"))
    {
        return src;
    }

    return path;
}