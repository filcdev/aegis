#include "dl_fatal.h"
#include "dl_lcd.h"
#include "dl_logger.h"
#include <Arduino.h>

static Logger logger("FATAL");

void fatal_error(const char* message, int reboot_delay_ms) {
    logger.error("%s", message);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fatal Error!");
    lcd.setCursor(0, 1);
    lcd.print(message);
    delay(reboot_delay_ms);
    esp_restart();
}
