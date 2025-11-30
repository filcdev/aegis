#include "dz_db.h"
#include "dz_state.h"

DZDBControl dbControl;

DZDBControl::DZDBControl() {}

void DZDBControl::begin()
{
  if (!SPIFFS.begin(true)) {
    state.error.db.hasError = true;
    state.error.db.message = "SPIFFS mount";
    return;
  }
  loadUIDs();
}

bool DZDBControl::saveUIDs()
{
  File file = SPIFFS.open(uidsFilePath, FILE_WRITE);
  if (!file) {
    state.error.db.hasError = true;
    state.error.db.message = "UID Save Fail";
    return false;
  }
  
  JsonDocument doc;
  for (const auto& entry : uids) {
    JsonArray arr = doc[entry.uid].to<JsonArray>();
    arr.add(entry.name);
    arr.add(entry.authorized);
  }

  if (serializeJson(doc, file) == 0) {
    state.error.db.hasError = true;
    state.error.db.message = "UID Save Fail";
    file.close();
    return false;
  }
  
  state.error.db.hasError = false;
  state.error.db.message = "";
  file.close();
  return true;
}

bool DZDBControl::isAuthorized(const std::string& uid, std::string &nameOut)
{
  Serial.println(("Checking UID: " + uid).c_str());
  for (const auto& entry : uids) {
    if (entry.uid == uid) {
      Serial.println(("Authorized UID found: " + uid + ", Name: " + entry.name).c_str());
      nameOut = entry.name;
      return entry.authorized;
    }
  }
  return false;
}

bool DZDBControl::loadUIDs()
{
  if (!SPIFFS.exists(uidsFilePath)) {
    return saveUIDs();
  }
  uids.clear();

  File file = SPIFFS.open(uidsFilePath, FILE_READ);
  if (!file) {
    state.error.db.hasError = true;
    state.error.db.message = "UID File Fail";
    return false;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    state.error.db.hasError = true;
    state.error.db.message = "UID Load Fail";
    return false;
  }

  uids.clear();
  JsonObject root = doc.as<JsonObject>();
  for (JsonPair kv : root) {
    std::string uid = kv.key().c_str();
    JsonArray arr = kv.value().as<JsonArray>();
    if (arr.size() >= 2) {
        uids.push_back({uid, arr[0].as<std::string>(), arr[1].as<bool>()});
    }
  }

  state.error.db.hasError = false;
  state.error.db.message = "";
  return true;
}

void DZDBControl::updateFromJSON(JsonObject root)
{
  uids.clear();
  for (JsonPair kv : root) {
    std::string uid = kv.key().c_str();
    JsonArray arr = kv.value().as<JsonArray>();
    if (arr.size() >= 2) {
        uids.push_back({uid, arr[0].as<std::string>(), arr[1].as<bool>()});
    }
  }
  saveUIDs();
}