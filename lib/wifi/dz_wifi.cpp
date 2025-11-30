#include "dz_wifi.h"
#include "dz_state.h"
#include "dz_configMgr.h"

DZWIFIControl::DZWIFIControl() {}

void DZWIFIControl::begin()
{
  WiFi.begin(cfg.wifi_ssid.c_str(), cfg.wifi_psk.c_str());
}

void DZWIFIControl::handle()
{
  if (WiFi.status() != WL_CONNECTED) {
    if(state.error.wifi.hasError == false) {
      state.error.wifi.hasError = true;
      state.error.wifi.message = "WiFi Disconn";
    }
  } else {
    if(state.error.wifi.hasError == true) {
      state.error.wifi.hasError = false;
      state.error.wifi.message = "";
    }
  }
}