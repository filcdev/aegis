#ifndef DL_LOGGER_H
#define DL_LOGGER_H

#include <Arduino.h>
#include <cstdarg>

enum LogLevel {
    LOG_DEBUG,
    LOG_INFO,
    LOG_ERROR
};

class Logger {
public:
    Logger(const char* ns);
    void setDebug(bool enabled);

    void debug(const char* format, ...);
    void info(const char* format, ...);
    void error(const char* format, ...);

private:
    void log(LogLevel level, const char* format, va_list args);
    char _namespace[5];
    bool _debug_enabled = false;
};

#endif // DL_LOGGER_H
