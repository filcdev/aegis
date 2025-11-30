#include <Arduino.h>
#include "dz_state.h"

GlobalState state;

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
      state.deviceState = DEVICE_STATE_ERROR;
    }
    else state.deviceState = DEVICE_STATE_IDLE;
  }
  digitalWrite(LED_BUILTIN, state.doorOpen ? HIGH : LOW);
  if(state.doorOpen) {
    if(state.doorOpenTmr == 0) state.doorOpenTmr = millis();
    if(millis() - state.doorOpenTmr > 5000) {
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