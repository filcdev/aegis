#ifndef DZ_STATE_H
#define DZ_STATE_H

#include <string>
#include "dz_logger.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

enum DeviceState
{
  DEVICE_STATE_BOOTING = 0,
  DEVICE_STATE_IDLE = 1,
  DEVICE_STATE_ERROR = 2,
  DEVICE_STATE_UPDATING = 3
};

struct ErrorEntry
{
  bool hasError = false;
  std::string message = "";
};

#include <array>

struct Errors
{
  ErrorEntry nfc;
  ErrorEntry sd;
  ErrorEntry wifi;
  ErrorEntry webSocket;
  ErrorEntry db;
  ErrorEntry ota;

  std::array<std::string, 6> messages() const
  {
    return {
      nfc.message,
      sd.message,
      wifi.message,
      webSocket.message,
      db.message,
      ota.message
    };
  }
};

struct GlobalState
{
  DeviceState deviceState = DEVICE_STATE_BOOTING;
  Errors error;
  std::string header = "";
  std::string message = "";
  bool doorOpen = false;
  unsigned long doorOpenTmr = 0;
  std::string time = "--:--";
};

extern GlobalState state;

class DZStateControl
{
public:
  DZStateControl();
  void begin();
  void handle();
  void openDoor();
private:
  Logger logger;
  unsigned long messageTmr = 0;
  std::string lastMessage = "";
  TimerHandle_t doorTimer = NULL;
  static void doorTimerCallback(TimerHandle_t xTimer);
};

extern DZStateControl stateControl;

#endif
