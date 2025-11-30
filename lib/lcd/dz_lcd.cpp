#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "dz_lcd.h"
#include "dz_state.h"
#include "dz_config.h"
#include <Arduino.h>

DZLCDControl::DZLCDControl() : lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS) {}

void DZLCDControl::begin()
{
  lcd.init();
  lcd.backlight();
  handle();
}

void DZLCDControl::printLn(const char* msg) {
  const size_t max = LCD_COLUMNS; // 16
  size_t len = strlen(msg);
  size_t toWrite = (len < max) ? len : max;
  for (size_t i = 0; i < toWrite; ++i) {
    lcd.write(msg[i]);
  }
  for (size_t i = toWrite; i < max; ++i) {
    lcd.write(' ');
  }
}

bool DZLCDControl::isUpdateNeeded() {
  return (
    state.deviceState != lastState.deviceState || 
    state.deviceState != DEVICE_STATE_IDLE ||
    state.doorOpen != lastState.doorOpen ||
    state.message != lastState.message
  );
}

void DZLCDControl::updateHeader() {
  if(
    (backlightOn && 
    state.deviceState != DEVICE_STATE_BOOTING && 
    state.deviceState != DEVICE_STATE_UPDATING &&
    lastTime != state.time) || state.header != lastState.header
  ) {
    lcd.setCursor(0, 0);
    if(state.header != "") {
      printLn(state.header.c_str());
    } else {
      printLn("Aegis");
    }
    lcd.setCursor(11, 0);
    lcd.print(state.time.c_str());
    lastTime = state.time;
  }
}

void DZLCDControl::cycleErrors() {
  const int MAX_ERRORS = 6;
  if (erIndex < 0 || erIndex >= MAX_ERRORS) erIndex = 0;
  
  if (state.error.messages()[erIndex].empty()) {
    for (int i = 0; i < MAX_ERRORS; ++i) {
      if (!state.error.messages()[i].empty()) { erIndex = i; break; }
    }
  }
  
  unsigned long now = millis();
  if (now - lastErrorSwitch >= switchInterval) {
    int next = erIndex;
    for (int i = 1; i <= MAX_ERRORS; ++i) {
      int idx = (erIndex + i) % MAX_ERRORS;
      if (!state.error.messages()[idx].empty()) { next = idx; break; }
    }
    erIndex = next;
    lastErrorSwitch = now;
    lcd.setCursor(0, 1);
    lcd.print("E: ");
    printLn(state.error.messages()[erIndex].c_str());
  }
}

void DZLCDControl::displayCurrentState() {
  if (state.message != "") {
      lcd.setCursor(0, 1);
      printLn(state.message.c_str());
      return;
  }
  
  switch (state.deviceState)
  {
    case DEVICE_STATE_BOOTING:
      lcd.clear();
      lcd.setCursor(0, 0);
      printLn("SzakiWare Aegis");
      lcd.setCursor(0, 1);
      printLn("Please wait...");
      break;
    case DEVICE_STATE_ERROR:
      cycleErrors();
      break;
    case DEVICE_STATE_UPDATING:
      lcd.setCursor(0, 0);
      printLn("OTA Updating");
      lcd.setCursor(0, 1);
      printLn("Please wait...");
      break;
    default:
      lcd.setCursor(0, 1);
      printLn("");
      break;
  }
}

void DZLCDControl::handle()
{ 
  updateHeader();

  if(isUpdateNeeded()) {
    if(!backlightOn) {
      lcd.backlight();
      lcd.display();
      backlightOn = true;
    }
    lcd.display();
    backlightTmr = millis();
    lastState = state;
  } else {
    if(millis() - backlightTmr > BACKLIGHT_TIMEOUT) {
      lcd.noBacklight();
      lcd.noDisplay();
      backlightOn = false;
    }
    return;
  }
  
  displayCurrentState();
}