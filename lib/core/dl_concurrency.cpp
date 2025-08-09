#include "dl_concurrency.h"

SemaphoreHandle_t lcdMutex = nullptr;
SemaphoreHandle_t loggerMutex = nullptr;
SemaphoreHandle_t stateMutex = nullptr;
QueueHandle_t tagQueue = nullptr;

void initConcurrency() {
    if (!lcdMutex) lcdMutex = xSemaphoreCreateRecursiveMutex();
    if (!loggerMutex) loggerMutex = xSemaphoreCreateMutex();
    if (!stateMutex) stateMutex = xSemaphoreCreateMutex();
    if (!tagQueue) tagQueue = xQueueCreate(10, TAG_MAX_LEN); // queue for up to 10 tags
}
