#include "dz_logger.h"

SemaphoreHandle_t loggerMutex = NULL;

Logger::Logger(const char* ns) {
    strncpy(_namespace, ns, 4);
    _namespace[4] = '\0';
}

void Logger::setDebug(bool enabled) {
    _debug_enabled = enabled;
}

void Logger::log(LogLevel level, const char* format, va_list args) {
    if (level == LOG_DEBUG && !_debug_enabled) {
        return;
    }

    char level_char;
    switch (level) {
        case LOG_DEBUG: level_char = 'D'; break;
        case LOG_INFO:  level_char = 'I'; break;
        case LOG_ERROR: level_char = 'E'; break;
        default:        level_char = ' '; break;
    }

    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);

    if (loggerMutex && xSemaphoreTake(loggerMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        Serial.printf("%s | %c | %s\n", _namespace, level_char, buffer);
        xSemaphoreGive(loggerMutex);
    } else {
        // Fallback without mutex if not initialized yet
        Serial.printf("%s | %c | %s\n", _namespace, level_char, buffer);
    }
}

void Logger::debug(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LOG_DEBUG, format, args);
    va_end(args);
}

void Logger::info(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LOG_INFO, format, args);
    va_end(args);
}

void Logger::error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log(LOG_ERROR, format, args);
    va_end(args);
}