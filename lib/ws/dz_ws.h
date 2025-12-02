#ifndef DZ_WS_H
#define DZ_WS_H

#include <WebSocketsClient.h>
#include <Arduino.h>
#include "dz_db.h"
#include "dz_logger.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

enum WSEventType {
  WS_EVT_CARD_READ
};

struct WSEvent {
  WSEventType type;
  char uid[32];
  bool authorized;
  bool isButton;
  time_t timestamp;
};

class DZWSControl {
public:
  DZWSControl();
  void begin();
  void handle();
  void send(String message);
  void sendCardRead(const std::string& uid, bool granted, bool isButton = false);
  Logger logger;

private:
  WebSocketsClient webSocket;
  QueueHandle_t _wsQueue;
  static void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
  static void handleIncomingMessage(const std::string& message);
  static String getResetReason();
  void sendPing();

  unsigned long lastPingTime = 0;
  
  static const unsigned long PING_INTERVAL = 60000;
  static const unsigned long RECONNECT_INTERVAL = 5000;
};

extern DZWSControl wsControl;

#endif