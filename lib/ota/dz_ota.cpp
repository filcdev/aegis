#include "dz_ota.h"
#include "dz_state.h"
#include "dz_configMgr.h"

DZOTAControl otaControl;

DZOTAControl::DZOTAControl() : logger("OTA") {}

void DZOTAControl::begin()
{
  logger.info("Initializing OTA");
  if (cfg.cert == "") {
    logger.error("OTA Certificate missing");
    state.error.ota.hasError = true;
    state.error.ota.message = "OTA Cert Miss";
  }
}

void DZOTAControl::handle()
{
  if (state.deviceState != DEVICE_STATE_UPDATING) return;

  HttpsOTAStatus_t otaStatus = HttpsOTA.status();
  switch (otaStatus) {
    case HTTPS_OTA_SUCCESS:
      logger.info("OTA Update Successful, rebooting...");
      state.deviceState = DEVICE_STATE_IDLE;
      state.message = "Rebooting...";
      delay(1000);
      ESP.restart();
      break;
      
    case HTTPS_OTA_FAIL:
      logger.error("OTA Update Failed");
      state.deviceState = DEVICE_STATE_IDLE;
      state.error.ota.hasError = true;
      state.error.ota.message = "OTA Failed";
      break;
      
    default:
      break;
  }
}

void DZOTAControl::startUpdate(const char* url)
{
  logger.info("Starting OTA update from %s", url);
  if (state.error.wifi.hasError) {
    logger.error("Cannot start OTA: WiFi error");
    return;
  }

  if (cfg.cert == "") {
    logger.error("Cannot start OTA: Certificate missing");
    state.error.ota.hasError = true;
    state.error.ota.message = "OTA Cert Miss";
    return;
  }

  state.error.ota.hasError = false;
  state.deviceState = DEVICE_STATE_UPDATING;
  state.message = "Updating...";
  HttpsOTA.begin(url, cfg.cert.c_str(), true);
}
