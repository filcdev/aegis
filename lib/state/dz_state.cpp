#include <Arduino.h>
#include "dz_state.h"

GlobalState state;

DZStateControl::DZStateControl() : logger("STAT") {}

void DZStateControl::handle()
{
  if(state.deviceState != DEVICE_STATE_UPDATING){
    if(
      state.error.nfc.hasError ||
      state.error.sd.hasError ||
      state.error.wifi.hasError ||
      state.error.webSocket.hasError ||
      state.error.db.hasError ||
      state.error.ota.hasError
    ) {
      if (state.deviceState != DEVICE_STATE_ERROR) {
        logger.error("Entering ERROR state");
      }
      state.deviceState = DEVICE_STATE_ERROR;
    }
    else {
      if (state.deviceState == DEVICE_STATE_ERROR) {
        logger.info("Errors cleared, entering IDLE state");
      }
      state.deviceState = DEVICE_STATE_IDLE;
    }
  }
  digitalWrite(LED_BUILTIN, state.doorOpen ? HIGH : LOW);
  if(state.doorOpen) {
    if(state.doorOpenTmr == 0) state.doorOpenTmr = millis();
    if(millis() - state.doorOpenTmr > 5000) {
      logger.info("Closing door");
      state.doorOpen = false;
      state.doorOpenTmr = 0;
    }
  }
  if(state.message != "") {
    if(lastMessage != state.message) {
      lastMessage = state.message;
      messageTmr = millis();
    }
    if(messageTmr == 0) messageTmr = millis();
    if(millis() - messageTmr > 5000) {
      state.message = "";
      lastMessage = "";
      state.header = "";
      messageTmr = 0;
    }
  }
}