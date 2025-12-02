#ifndef DZ_CONFIGMGR_H
#define DZ_CONFIGMGR_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "dz_logger.h"

struct DeviceConfig {
  String hostname = "DokZar-ESP32";
  String api_key = "";
  String wifi_ssid = "";
  String wifi_psk = "";
  String ws_addr = "";
  uint16_t ws_port = 443;
  String ws_path = "/";
  String ota_url = "";
  String cert = "";
};

class DZConfigManager {
public:
  DZConfigManager();
  void begin();

private:
  Logger logger;
  void parseConfigFile();
  bool copyFile(const char* srcPath, const char* dstPath);
  void checkSDUpdates();
};

extern DeviceConfig cfg;

#endif
