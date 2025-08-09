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
#include "dl_fatal.h"
#include "dl_concurrency.h"

Logger logger("MAIN");

// Task handles
static TaskHandle_t taskRFIDHandle = nullptr;
static TaskHandle_t taskCommHandle = nullptr;

// Watchdog timing
static const uint32_t WATCHDOG_TIMEOUT_MS = 15000; // 15s
static volatile uint32_t lastRFIDFeed = 0;
static volatile uint32_t lastCommFeed = 0;

void feedRFIDWatchdog() { lastRFIDFeed = millis(); }
void feedCommWatchdog() { lastCommFeed = millis(); }

void checkWatchdogs() {
  uint32_t now = millis();
  if (now - lastRFIDFeed > WATCHDOG_TIMEOUT_MS) {
    logger.error("RFID task hang detected");
    fatal_error("RFID Hang", 5000);
  }
  if (now - lastCommFeed > WATCHDOG_TIMEOUT_MS) {
    logger.error("Comm task hang detected");
    fatal_error("Comm Hang", 5000);
  }
}

void die(const char* message) {
  stateHandler.setState(AppState::ERROR);
  fatal_error(message, 10000);
}

void flashBootLogo() {
    LcdLockGuard lock; if (!lock.locked()) return;
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("DLock v1.0");
    delay(DL_BOOT_LOGO_DURATION);
}

// RFID reader task (Core 0)
void taskRFID(void* pvParameters) {
  logger.info("RFID task started on core %d", xPortGetCoreID());
  for(;;) {
    String tag = rdm6300Handler.readTag();
    if (tag.length() > 0) {
      char buf[TAG_MAX_LEN];
      strlcpy(buf, tag.c_str(), TAG_MAX_LEN);
      if (xQueueSend(tagQueue, buf, pdMS_TO_TICKS(10)) != pdPASS) { /* drop */ }
    }
    feedRFIDWatchdog();
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

// Communication task (WiFi + MQTT) (Core 1)
void taskComm(void* pvParameters) {
  logger.info("Comm task started on core %d", xPortGetCoreID());
  for(;;) {
    mqttHandler.loop();
    feedCommWatchdog();
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void setup(){
  Serial.begin(115200);
  initConcurrency();
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
  uint32_t wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - wifiStart > 20000) {
      die("WiFi timeout");
    }
    delay(500);
  }

  stateHandler.setState(AppState::WIFI_CONNECTED);
  logger.info("Connected to WiFi, got IP %s", WiFi.localIP().toString().c_str());
  {
    LcdLockGuard lock; if (lock.locked()) {
      lcd.setCursor(0, 1);
      lcd.print(WiFi.localIP().toString().c_str());
    }
  }
  delay(2000);

  mqttHandler.begin(
    config.getString("mqtt_addr", ""),
    1883,
    config.getString("hostname", "dlock").c_str(),
    config.getString("api_key", "").c_str()
  );

  rdm6300Handler.begin();

  // Initialize watchdog feeds
  lastRFIDFeed = lastCommFeed = millis();

  // Create tasks pinned to cores
  xTaskCreatePinnedToCore(taskRFID, "RFID", 4096, nullptr, 1, &taskRFIDHandle, 0); // Core 0
  xTaskCreatePinnedToCore(taskComm, "COMM", 4096, nullptr, 1, &taskCommHandle, 1); // Core 1

  stateHandler.setState(AppState::READY);
}

void loop(){
  stateHandler.loop();
  lcd.loop();

  // Process tag queue
  char tagBuf[TAG_MAX_LEN];
  while (xQueueReceive(tagQueue, &tagBuf, 0) == pdTRUE) {
    String tag(tagBuf);
    logger.info("Tag scanned: %s", tag.c_str());
    mqttHandler.publishTag(tag);
  }

  checkWatchdogs();
  delay(100); 
}
