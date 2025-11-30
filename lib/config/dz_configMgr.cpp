#include "dz_configMgr.h"
#include <SPI.h>
#include <SD.h>
#include "dz_config.h"
#include "dz_state.h"
#include <SPIFFS.h>

DeviceConfig cfg;

void DZConfigManager::begin() {
  if (!SPIFFS.begin(true)) {
    state.error.sd.hasError = true;
    state.error.sd.message = "SPIFFS Init";
    return;
  }

  if (SD.begin(5)) {
    checkSDUpdates();
    SD.end();
  }
  
  parseConfigFile();
}

void DZConfigManager::checkSDUpdates() {
  if (SD.exists("/config.new.json")) {
    if (copyFile("/config.new.json", "/config.json")) {
      SD.remove("/config.json");
      SD.rename("/config.new.json", "/config.json");
    }
  }
  
  if (SD.exists("/uids.json")) {
    if (copyFile("/uids.json", "/uids.json")) {
      SD.remove("/uids.json");
    }
  }
}

bool DZConfigManager::copyFile(const char* srcPath, const char* dstPath) {
  File src = SD.open(srcPath, FILE_READ);
  if (!src) return false;

  File dst = SPIFFS.open(dstPath, FILE_WRITE);
  if (!dst) {
    src.close();
    return false;
  }

  uint8_t buf[512];
  while (src.available()) {
    size_t len = src.read(buf, sizeof(buf));
    dst.write(buf, len);
  }

  src.close();
  dst.close();
  return true;
}

void DZConfigManager::parseConfigFile() {
  if (!SPIFFS.exists("/config.json")) {
    state.error.sd.hasError = true;
    state.error.sd.message = "Cfg Miss";
    return;
  }

  File f = SPIFFS.open("/config.json", FILE_READ);
  if (!f) {
    state.error.sd.hasError = true;
    state.error.sd.message = "Cfg Open";
    return;
  }

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, f);
  f.close();

  if (err) {
    state.error.sd.hasError = true;
    state.error.sd.message = "Cfg Parse";
    return;
  }
  
  if (doc["hostname"]) cfg.hostname = doc["hostname"].as<String>();
  if (doc["api_key"]) cfg.api_key = doc["api_key"].as<String>();
  if (doc["wifi_ssid"]) cfg.wifi_ssid = doc["wifi_ssid"].as<String>();
  if (doc["wifi_psk"]) cfg.wifi_psk = doc["wifi_psk"].as<String>();
  if (doc["mqtt_addr"]) cfg.mqtt_addr = doc["mqtt_addr"].as<String>();
  if (doc["mqtt_port"]) cfg.mqtt_port = doc["mqtt_port"].as<uint16_t>();
  if (doc["mqtt_user"]) cfg.mqtt_user = doc["mqtt_user"].as<String>();
  if (doc["mqtt_psk"]) cfg.mqtt_psk = doc["mqtt_psk"].as<String>();
  if (doc["cert"]) cfg.cert = doc["cert"].as<String>();

  state.error.sd.hasError = false;
  state.error.sd.message = "";
}


