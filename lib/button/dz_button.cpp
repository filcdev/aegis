#include <Arduino.h>
#include "dz_config.h"
#include "dz_button.h"
#include "dz_state.h"
#include "dz_ws.h"

DZButton::DZButton() {}

void DZButton::begin() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void DZButton::handle() {
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW) {
    if ((millis() - lastDebounceTime) > 500) {
      state.doorOpenTmr = millis();
      state.doorOpen = true;
      state.header = "Aegis  <<";
      state.message = "Door Open";
      lastDebounceTime = millis();
      wsControl.sendCardRead("", true, true);
    }
  }
}