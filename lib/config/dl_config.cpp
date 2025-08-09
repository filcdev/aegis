#include "dl_config.h"
#include "dl_logger.h"

static Logger logger("CONF");

bool Config::load(fs::FS &fs, const char *path) {
  File file = fs.open(path, "r");
  if (!file) {
    logger.error("Failed to open config file");
    return false;
  }

  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    logger.error("Failed to parse config file: %s", error.c_str());
    return false;
  }

  logger.info("Config loaded from %s", path);
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
  String output;
  serializeJsonPretty(doc, output);
  logger.info("Current configuration:\n%s", output.c_str());
}

// Global config instance
Config config;