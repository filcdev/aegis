#include "dz_ota.h"
#include "dz_state.h"
#include "dz_configMgr.h"

DZOTAControl::DZOTAControl() {}

void DZOTAControl::begin()
{
  if (cfg.cert == "") {
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
      state.deviceState = DEVICE_STATE_IDLE;
      state.message = "Rebooting...";
      delay(1000);
      ESP.restart();
      break;
      
    case HTTPS_OTA_FAIL:
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
  if (state.error.wifi.hasError) return;

  if (cfg.cert == "") {
    state.error.ota.hasError = true;
    state.error.ota.message = "OTA Cert Miss";
    return;
  }

  state.error.ota.hasError = false;
  state.deviceState = DEVICE_STATE_UPDATING;
  state.message = "Updating...";
  HttpsOTA.begin(url, cfg.cert.c_str(), true);
}
