#include "dz_ws.h"
#include "dz_configMgr.h"
#include "dz_state.h"
#include "dz_db.h"
#include "dz_ota.h"
#include "dz_config.h"

DZWSControl wsControl;

DZWSControl::DZWSControl() : logger("WS") {}

void DZWSControl::begin() {
  logger.info("Initializing WebSocket");
  if (cfg.ws_addr.length() > 0 && cfg.ws_port > 0 && cfg.ws_path.length() > 0 && cfg.api_key.length() > 0) {
    webSocket.begin(cfg.ws_addr, cfg.ws_port, cfg.ws_path);
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(RECONNECT_INTERVAL);
    webSocket.setExtraHeaders(("X-Aegis-Device-Token: " + cfg.api_key).c_str());
  } else {
    logger.error("WebSocket configuration missing");
    state.error.webSocket.hasError = true;
    state.error.webSocket.message = "No WS Config";
  }
}

void DZWSControl::handle() {
  if(state.error.wifi.hasError) {
    return;
  }
  webSocket.loop();

  if (!state.error.webSocket.hasError && millis() - lastPingTime > PING_INTERVAL) {
    sendPing();
  }
}

void DZWSControl::send(String message) {
  if (!state.error.wifi.hasError || !state.error.webSocket.hasError)
  {
    webSocket.sendTXT(message);
  }
}

void DZWSControl::sendPing() {
  lastPingTime = millis();
  
  JsonDocument doc;
  doc["type"] = "ping";
  doc["fwVersion"] = FW_VERSION;
  doc["uptime"] = millis();
  doc["ram"] = ESP.getFreeHeap();
  doc["storage"]["total"] = SPIFFS.totalBytes();
  doc["storage"]["used"] = SPIFFS.usedBytes();
  
  doc["debug"]["deviceState"] = state.deviceState;
  doc["debug"]["lastResetReason"] = getResetReason();
  doc["debug"]["errors"]["nfc"] = state.error.nfc.hasError;
  doc["debug"]["errors"]["sd"] = state.error.sd.hasError;
  doc["debug"]["errors"]["wifi"] = state.error.wifi.hasError;
  doc["debug"]["errors"]["db"] = state.error.db.hasError;
  doc["debug"]["errors"]["ota"] = state.error.ota.hasError;

  String msg;
  serializeJson(doc, msg);
  send(msg);
}

void DZWSControl::sendCardRead(const std::string& uid, bool authorized, bool isButton) {
  JsonDocument doc;
  doc["type"] = "card-read";
  doc["uid"] = uid;
  doc["authorized"] = authorized;
  doc["isButton"] = isButton;
  String msg;
  serializeJson(doc, msg);
  send(msg);
}

String DZWSControl::getResetReason() {
  esp_reset_reason_t reason = esp_reset_reason();
  switch (reason) {
    case ESP_RST_UNKNOWN: return "ESP_RST_UNKNOWN";
    case ESP_RST_POWERON: return "ESP_RST_POWERON";
    case ESP_RST_EXT: return "ESP_RST_EXT";
    case ESP_RST_SW: return "ESP_RST_SW";
    case ESP_RST_PANIC: return "ESP_RST_PANIC";
    case ESP_RST_INT_WDT: return "ESP_RST_INT_WDT";
    case ESP_RST_TASK_WDT: return "ESP_RST_TASK_WDT";
    case ESP_RST_WDT: return "ESP_RST_WDT";
    case ESP_RST_DEEPSLEEP: return "ESP_RST_DEEPSLEEP";
    case ESP_RST_BROWNOUT: return "ESP_RST_BROWNOUT";
    case ESP_RST_SDIO: return "ESP_RST_SDIO";
    default: return "UNKNOWN";
  }
}

void DZWSControl::webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      wsControl.logger.error("WebSocket Disconnected");
      state.error.webSocket.hasError = true;
      state.error.webSocket.message = "WS Disconn";
      break;
    case WStype_CONNECTED:
      wsControl.logger.info("WebSocket Connected");
      state.error.webSocket.hasError = false;
      state.error.webSocket.message = "";
      break;
    case WStype_TEXT:
      wsControl.logger.info("WebSocket Message Received: %s", payload);
      handleIncomingMessage(std::string((char*)payload, length));
      break;
    case WStype_BIN:
      break;
    case WStype_ERROR:
      wsControl.logger.error("WebSocket Error");
      state.error.webSocket.hasError = true;
      state.error.webSocket.message = "WS Error";
      break;
    case WStype_PING:
    case WStype_PONG:
      break;
  }
}

void DZWSControl::handleIncomingMessage(const std::string& message) {
  JsonDocument wsDoc;
  DeserializationError error = deserializeJson(wsDoc, message);
  if (error) {
    wsControl.logger.error("Failed to parse WebSocket message");
    return;
  }
  
  const char* type = wsDoc["type"];
  if (!type) return;

  wsControl.logger.info("Processing message type: %s", type);

  if (strcmp(type, "sync-database") == 0) {
    wsControl.logger.info("Syncing database");
    if (wsDoc["db"].is<JsonArray>()) {
      dbControl.updateFromJSON(wsDoc["db"].as<JsonArray>());
    }
  } else if (strcmp(type, "open-door") == 0) {
    wsControl.logger.info("Remote open door request");
    state.header = "Welcome >>";
    state.doorOpen = true;
    state.doorOpenTmr = millis();
    if (wsDoc["name"].is<const char*>()) {
      state.message = wsDoc["name"].as<const char*>();
    } else {
      state.message = "WebUser";
    }
  } else if (strcmp(type, "update") == 0) {
    wsControl.logger.info("Remote update request");
    if (wsDoc["url"].is<const char*>())
    {
      otaControl.startUpdate(wsDoc["url"].as<const char*>());
    } else {
      otaControl.startUpdate(cfg.ota_url.c_str());
    }
  }
}
