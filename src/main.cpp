#include <Arduino.h>
#include <dl_sd.h>
#include <dl_config.h>
#include <dl_lcd.h>
#include <dl_board_config.h>
#include <WiFi.h>
#include <dl_state.h>
#include <dl_rdm6300.h>


void die(const char* message) {
  Serial.printf("Error: %s\nRestarting in 10 seconds...", message);
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

  lcd.begin(DL_LCD_ADDR, DL_LCD_COLS, DL_LCD_ROWS);
  stateHandler.begin(lcd);

  flashBootLogo();

  stateHandler.setState(AppState::BOOTING);

  if (!sdCard.begin()) {
    die("SD card failed!");
  }

  if(!config.load(sdCard.fs(), "/config.json")) {
    die("Config failed!");
  }

  stateHandler.setState(AppState::WIFI_CONNECTING);
  WiFi.mode(WIFI_STA);
  WiFi.begin(config.getString("wifi_ssid", ""), config.getString("wifi_psk", ""));
  Serial.printf("\nConnecting to WiFi %s... ", config.getString("wifi_ssid", "").c_str());
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  stateHandler.setState(AppState::WIFI_CONNECTED);
  Serial.printf("\nConnected to WiFi, got IP %s", WiFi.localIP().toString().c_str());
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP().toString().c_str());
  delay(2000);

  rdm6300Handler.begin();

  stateHandler.setState(AppState::READY);
}

void loop(){
  String tag = rdm6300Handler.readTag();
  if (tag != "") {
    Serial.printf("Tag scanned: %s\n", tag.c_str());
  }
  delay(100); 
}
