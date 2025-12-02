#ifndef DZ_LCD_H
#define DZ_LCD_H

#include <LiquidCrystal_I2C.h>
#include <string>
#include "dz_state.h"
#include "dz_logger.h"

class DZLCDControl
{
public:
  DZLCDControl();
  void begin();
  void handle();
  void printLn(const char *msg);
  void clear()
  {
    lcd.clear();
  }

private:
  Logger logger;
  bool isUpdateNeeded();
  void updateHeader();
  void manageBacklight();
  void displayCurrentState();
  void cycleErrors();

  LiquidCrystal_I2C lcd;
  GlobalState lastState;
  std::string lastTime;
  
  int erIndex = 0;
  unsigned long lastErrorSwitch = 0;
  unsigned long backlightTmr = 0;
  bool backlightOn = true;
  
  static const int switchInterval = 3000;
  static const unsigned long BACKLIGHT_TIMEOUT = 30000;
};

#endif