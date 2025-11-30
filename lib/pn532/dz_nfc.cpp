#include "dz_nfc.h"
#include "dz_state.h"
#include "dz_ws.h"
#include <ArduinoJson.h>
#include <string>

DZNFCControl::DZNFCControl() {}

void DZNFCControl::begin()
{
  Serial1.begin(PN532_HSU_BAUD, SERIAL_8N1, PN532_HSU_RX_PIN, PN532_HSU_TX_PIN);
  delay(10);

  nfc.begin();
  uint32_t versiondata = 0;
  unsigned long start = millis();
  while (!versiondata && (millis() - start) < 2000) {
    versiondata = nfc.getFirmwareVersion();
    if (!versiondata) delay(200);
  }

  if (!versiondata) {
    state.error.nfc.hasError = true;
    state.error.nfc.message = "PN532 Disconn";
    return;
  }

  nfc.SAMConfig();
  nfc.setPassiveActivationRetries(0x01);
  state.error.nfc.hasError = false;
}

void DZNFCControl::handle()
{
  unsigned long now = millis();
  if (now - lastHealthCheck >= 5000) {
    lastHealthCheck = now;
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
      state.error.nfc.hasError = true;
      state.error.nfc.message = "PN532 Disconn";
    } else {
      state.error.nfc.hasError = false;
      state.error.nfc.message = "";
    }
  }
  if (state.error.nfc.hasError) return;
  if (now - lastDetectionTime < 1000) return;
  uint8_t uid[7];
  uint8_t uidLength;
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    lastDetectionTime = now;
    std::string uidStr;
    char buf[4];
    for (uint8_t i = 0; i < uidLength; i++) {
      if (i > 0) uidStr += ':';
      snprintf(buf, sizeof(buf), "%02X", uid[i]);
      uidStr += buf;
    }
    std::string name;
    bool authorized = dbControl.isAuthorized(uidStr, name);
    if (authorized) {
      state.header = "Welcome >>";
      state.message = name;
      state.doorOpen = true;
      state.doorOpenTmr = now;
    } else {
      state.message = "Access Denied";
    }
    wsControl.sendCardRead(uidStr, authorized, false);
  }
}