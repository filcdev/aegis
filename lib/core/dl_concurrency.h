#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>

// Maximum length of an RFID tag string (including null terminator)
#define TAG_MAX_LEN 32

extern SemaphoreHandle_t lcdMutex; // created as recursive mutex
extern SemaphoreHandle_t loggerMutex;
extern SemaphoreHandle_t stateMutex;
extern QueueHandle_t tagQueue;

// Non-recursive lock guard
class LockGuard {
  public:
    LockGuard(SemaphoreHandle_t m, TickType_t timeout = pdMS_TO_TICKS(50)) : _m(m) {
        if (_m) {
            _locked = (xSemaphoreTake(_m, timeout) == pdTRUE);
        }
    }
    ~LockGuard() {
        if (_locked && _m) xSemaphoreGive(_m);
    }
    bool locked() const { return _locked; }
  private:
    SemaphoreHandle_t _m;
    bool _locked = false;
};

// Recursive lock guard specifically for lcdMutex
class LcdLockGuard {
  public:
    LcdLockGuard(SemaphoreHandle_t m = lcdMutex, TickType_t timeout = pdMS_TO_TICKS(50)) : _m(m) {
        if (_m) {
            _locked = (xSemaphoreTakeRecursive(_m, timeout) == pdTRUE);
        }
    }
    ~LcdLockGuard() {
        if (_locked && _m) xSemaphoreGiveRecursive(_m);
    }
    bool locked() const { return _locked; }
  private:
    SemaphoreHandle_t _m;
    bool _locked = false;
};

// Initialize mutexes, queues, etc.
void initConcurrency();
