#ifndef DZ_CONFIGMGR_H
#define DZ_CONFIGMGR_H

#include <Arduino.h>
#include <ArduinoJson.h>

struct DeviceConfig {
  String hostname = "DokZar-ESP32";
  String api_key = "";
  String wifi_ssid = "";
  String wifi_psk = "";
  String mqtt_addr = "";
  String mqtt_user = "";
  String mqtt_psk = "";
  uint16_t mqtt_port = 1883;
  String cert = "";
};

class DZConfigManager {
public:
  void begin();

private:
  void parseConfigFile();
  bool copyFile(const char* srcPath, const char* dstPath);
  void checkSDUpdates();
};

extern DeviceConfig cfg;

#endif
