#include "dz_db.h"
#include "dz_state.h"

DZDBControl dbControl;

DZDBControl::DZDBControl() : logger("DB") {}

void DZDBControl::begin()
{
  logger.info("Initializing DB");
  if (!SPIFFS.begin(true)) {
    logger.error("SPIFFS mount failed");
    state.error.db.hasError = true;
    state.error.db.message = "SPIFFS mount";
    return;
  }
  loadUIDs();
}

bool DZDBControl::saveUIDs()
{
  logger.info("Saving UIDs to file");
  File file = SPIFFS.open(uidsFilePath, FILE_WRITE);
  if (!file) {
    logger.error("Failed to open UIDs file for writing");
    state.error.db.hasError = true;
    state.error.db.message = "UID Save Fail";
    return false;
  }
  
  JsonDocument doc;
  for (const auto& entry : uids) {
    doc[entry.uid] = entry.name;
  }

  if (serializeJson(doc, file) == 0) {
    logger.error("Failed to serialize UIDs");
    state.error.db.hasError = true;
    state.error.db.message = "UID Save Fail";
    file.close();
    return false;
  }
  
  logger.info("UIDs saved successfully");
  state.error.db.hasError = false;
  state.error.db.message = "";
  file.close();
  return true;
}

bool DZDBControl::isAuthorized(const std::string& uid, std::string &nameOut)
{
  for (const auto& entry : uids) {
    if (entry.uid == uid) {
      nameOut = entry.name;
      return true;
    }
  }
  return false;
}

bool DZDBControl::loadUIDs()
{
  logger.info("Loading UIDs from file");
  if (!SPIFFS.exists(uidsFilePath)) {
    logger.info("UIDs file not found, creating new one");
    return saveUIDs();
  }
  uids.clear();

  File file = SPIFFS.open(uidsFilePath, FILE_READ);
  if (!file) {
    logger.error("Failed to open UIDs file for reading");
    state.error.db.hasError = true;
    state.error.db.message = "UID File Fail";
    return false;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    logger.error("Failed to parse UIDs file: %s", error.c_str());
    state.error.db.hasError = true;
    state.error.db.message = "UID Load Fail";
    return false;
  }

  uids.clear();
  JsonObject root = doc.as<JsonObject>();
  for (JsonPair kv : root) {
    std::string uid = kv.key().c_str();
    if(kv.value().is<const char*>()) {
      std::string name = kv.value().as<std::string>();
      uids.push_back({uid, name});
    }
  }

  logger.info("Loaded %d UIDs", uids.size());
  state.error.db.hasError = false;
  state.error.db.message = "";
  return true;
}

void DZDBControl::updateFromJSON(JsonArray root)
{
  logger.info("Updating UIDs from JSON");
  uids.clear();
  for (JsonVariant v : root) {
    if (!v.is<JsonObject>()) continue;
    JsonObject obj = v.as<JsonObject>();
    if (obj["uid"].is<const char*>() && obj["name"].is<const char*>()) {
      std::string uid = obj["uid"].as<std::string>();
      std::string name = obj["name"].as<std::string>();
      uids.push_back({uid, name});
    }
  }
  saveUIDs();
}