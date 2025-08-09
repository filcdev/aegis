#include "dl_config.h"

bool Config::load(fs::FS &fs, const char *path) {
  File file = fs.open(path, "r");
  if (!file) {
    Serial.println("Failed to open config file");
    return false;
  }

  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.print("Failed to parse config file: ");
    Serial.println(error.c_str());
    return false;
  }

  loaded = true;
  return true;
}

String Config::getString(const char* key, const char* defaultValue) {
  if (!loaded || !doc[key].is<const char*>()) return String(defaultValue);
  return String(doc[key].as<const char*>());
}

int Config::getInt(const char* key, int defaultValue) {
  if (!loaded || !doc[key].is<int>()) return defaultValue;
  return doc[key].as<int>();
}

bool Config::getBool(const char* key, bool defaultValue) {
  if (!loaded || !doc[key].is<bool>()) return defaultValue;
  return doc[key].as<bool>();
}

float Config::getFloat(const char* key, float defaultValue) {
  if (!loaded || !doc[key].is<float>()) return defaultValue;
  return doc[key].as<float>();
}

void Config::printConfig() {
  serializeJsonPretty(doc, Serial);
  Serial.println();
}

// Global config instance
Config config;