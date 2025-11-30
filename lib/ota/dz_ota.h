#ifndef DZ_OTA_H
#define DZ_OTA_H

#include <WiFi.h>
#include "HttpsOTAUpdate.h"

class DZOTAControl {
public:
    DZOTAControl();
    void begin();
    void handle();
    void startUpdate(const char* url);
};

#endif