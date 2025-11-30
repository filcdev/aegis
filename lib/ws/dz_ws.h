#ifndef DZ_WS_H
#define DZ_WS_H

#include <WebSocketsClient.h>
#include <Arduino.h>
#include "dz_db.h"

class DZWSControl {
public:
  DZWSControl();
  void begin();
  void handle();
  void send(String message);
  void sendCardRead(const std::string& uid, bool granted, bool isButton = false);

private:
  WebSocketsClient webSocket;
  static void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
  static void handleIncomingMessage(const std::string& message);
  static String getResetReason();
  
  void sendPing();

  bool helloMessageSent = false;
  unsigned long lastPingTime = 0;
  
  static const unsigned long PING_INTERVAL = 60000;
  static const unsigned long RECONNECT_INTERVAL = 5000;
};

extern DZWSControl wsControl;

#endif