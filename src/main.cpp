#include <Arduino.h>
#include <dl_sd.h>
#include <dl_config.h>
#include <dl_lcd.h>
#include <dl_board_config.h>
#include <WiFi.h>
#include <dl_state.h>
#include <dl_rdm6300.h>
#include <dl_mqtt.h>
#include "dl_logger.h"

Logger logger("MAIN");

void die(const char* message) {
  logger.error("Fatal: %s", message);
  stateHandler.setState(AppState::ERROR);
  lcd.setCursor(0,1);
  lcd.print(message);
  delay(10000);
  esp_restart();
}

void flashBootLogo() {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("DLock v1.0");
    delay(DL_BOOT_LOGO_DURATION);
}

void setup(){
  Serial.begin(115200);
  logger.info("DLock booting...");

  lcd.begin(DL_LCD_ADDR, DL_LCD_COLS, DL_LCD_ROWS);
  stateHandler.begin(lcd);

  flashBootLogo();

  stateHandler.setState(AppState::BOOTING);

  if (!sdCard.begin()) {
    die("SD card failed!");
  }

  if(!config.load(sdCard.fs(), "/config.json")) {
    die("Config failed!");
  } else {
    config.printConfig();
  }

  logger.setDebug(config.getBool("debug", false));

  stateHandler.setState(AppState::WIFI_CONNECTING);
  WiFi.mode(WIFI_STA);
  WiFi.begin(config.getString("wifi_ssid", ""), config.getString("wifi_psk", ""));
  logger.info("Connecting to WiFi %s...", config.getString("wifi_ssid", "").c_str());
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  stateHandler.setState(AppState::WIFI_CONNECTED);
  logger.info("Connected to WiFi, got IP %s", WiFi.localIP().toString().c_str());
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP().toString().c_str());
  delay(2000);

  mqttHandler.begin(
    config.getString("mqtt_addr", ""),
    1883,
    config.getString("hostname", "dlock").c_str(),
    config.getString("api_key", "").c_str()
  );

  rdm6300Handler.begin();

  stateHandler.setState(AppState::READY);
}

void loop(){
  mqttHandler.loop();
  String tag = rdm6300Handler.readTag();
  if (tag != "") {
    logger.info("Tag scanned: %s", tag.c_str());
    mqttHandler.publishTag(tag);
  }
  delay(100); 
}
