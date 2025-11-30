#ifndef DZ_NTP_H
#define DZ_NTP_H

#include <WiFi.h>
#include "time.h"

#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 3600
#define DAYLIGHT_OFFSET_SEC 3600
#define TIME_SYNC_INTERVAL 3600000

class DZNTPControl {
public:
    DZNTPControl();
    void setup();
    void handle();
    std::string getFormattedTime();

private:
    unsigned long lastSyncAttempt;
};

#endif