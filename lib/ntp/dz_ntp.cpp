#include "dz_ntp.h"
#include "dz_state.h"
#include <Arduino.h>

DZNTPControl::DZNTPControl() : lastSyncAttempt(0) {}

bool isTimeSet = false;
static const int TIME_RETRY_INTERVAL = 5000;

void DZNTPControl::setup() {
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
}

void DZNTPControl::handle() {
    unsigned long currentMillis = millis();
    unsigned long interval = isTimeSet ? TIME_SYNC_INTERVAL : TIME_RETRY_INTERVAL;
    if (currentMillis - lastSyncAttempt >= interval) {
        lastSyncAttempt = currentMillis;
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo, 100)) {
            return;
        } else {
            isTimeSet = true;
        }
    }
    std::string formatted = getFormattedTime();
    if (state.time != formatted) {
        state.time = formatted;
    }
}

std::string DZNTPControl::getFormattedTime() {
    struct tm timeinfo;
    if (!isTimeSet || !getLocalTime(&timeinfo, 100)) {
        return "--:--";
    }
    char timeString[6];
    std::snprintf(timeString, sizeof(timeString), "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    return std::string(timeString);
}
