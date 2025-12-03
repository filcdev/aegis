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
    stateControl.setError(ErrorSource::OTA, true, "OTA Cert Miss");
  }
}

void DZOTAControl::handle()
{
  if (stateControl.getDeviceState() != DEVICE_STATE_UPDATING) return;

  HttpsOTAStatus_t otaStatus = HttpsOTA.status();
  switch (otaStatus) {
    case HTTPS_OTA_SUCCESS:
      logger.info("OTA Update Successful, rebooting...");
      stateControl.setDeviceState(DEVICE_STATE_IDLE);
      stateControl.setMessage("Rebooting...");
      delay(1000);
      ESP.restart();
      break;
      
    case HTTPS_OTA_FAIL:
      logger.error("OTA Update Failed");
      stateControl.setDeviceState(DEVICE_STATE_IDLE);
      stateControl.setError(ErrorSource::OTA, true, "OTA Failed");
      break;
      
    default:
      break;
  }
}

void DZOTAControl::startUpdate(const char* url)
{
  logger.info("Starting OTA update from %s", url);
  if (stateControl.hasError(ErrorSource::WIFI)) {
    logger.error("Cannot start OTA: WiFi error");
    return;
  }

  if (cfg.cert == "") {
    logger.error("Cannot start OTA: Certificate missing");
    stateControl.setError(ErrorSource::OTA, true, "OTA Cert Miss");
    return;
  }

  stateControl.setError(ErrorSource::OTA, false);
  stateControl.setDeviceState(DEVICE_STATE_UPDATING);
  stateControl.setMessage("Updating...");
  HttpsOTA.begin(url, cfg.cert.c_str(), true);
}
