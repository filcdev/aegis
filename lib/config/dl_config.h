#ifndef DL_CONFIG_H
#define DL_CONFIG_H

#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>

class Config {
private:
  JsonDocument doc;
  bool loaded = false;

public:
  // Load JSON config file from FS
  bool load(fs::FS &fs, const char *path);

  // Typed getters with default values
  String getString(const char* key, const char* defaultValue = "");
  int getInt(const char* key, int defaultValue = 0);
  bool getBool(const char* key, bool defaultValue = false);
  float getFloat(const char* key, float defaultValue = 0.0);

  // Print whole config JSON to Serial for debugging
  void printConfig();
};

// Global config instance
extern Config config;

#endif // DL_CONFIG_H
