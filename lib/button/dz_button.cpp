#include <Arduino.h>
#include "dz_config.h"
#include "dz_button.h"
#include "dz_state.h"
#include "dz_ws.h"

DZButton::DZButton() : logger("BTN") {}

void DZButton::begin() {
  logger.info("Initializing Button on pin %d", BUTTON_PIN);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void DZButton::handle() {
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW) {
    if ((millis() - lastDebounceTime) > 500) {
      logger.info("Button pressed, opening door");
      stateControl.openDoor();
      stateControl.setHeader("Aegis  <<");
      stateControl.setMessage("Door Open");
      lastDebounceTime = millis();
      wsControl.sendCardRead("", true, true);
    }
  }
}