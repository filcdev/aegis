#include "dz_wifi.h"
#include "dz_state.h"
#include "dz_configMgr.h"

DZWIFIControl::DZWIFIControl() : logger("WIFI") {}

void DZWIFIControl::begin()
{
  logger.info("Connecting to WiFi SSID: %s", cfg.wifi_ssid.c_str());
  WiFi.begin(cfg.wifi_ssid.c_str(), cfg.wifi_psk.c_str());
}

void DZWIFIControl::handle()
{
  if (WiFi.status() != WL_CONNECTED) {
    if(state.error.wifi.hasError == false) {
      logger.error("WiFi Disconnected");
      state.error.wifi.hasError = true;
      state.error.wifi.message = "WiFi Disconn";
    }
  } else {
    if(state.error.wifi.hasError == true) {
      logger.info("WiFi Reconnected");
      state.error.wifi.hasError = false;
      state.error.wifi.message = "";
    }
  }
}