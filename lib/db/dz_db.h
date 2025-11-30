#ifndef DZ_DB_H
#define DZ_DB_H

#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#include <vector>
#include <string>

struct UidEntry {
  std::string uid;
  std::string name;
};

class DZDBControl
{
public:
  DZDBControl();
  void begin();
  bool saveUIDs();
  bool loadUIDs();
  void updateFromJSON(JsonArray root);
  bool isAuthorized(const std::string& uid, std::string &nameOut);
private:
  const char* uidsFilePath = "/uids.json";
  std::vector<UidEntry> uids;
};

extern DZDBControl dbControl;

#endif