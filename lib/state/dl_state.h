#ifndef DL_STATE_H
#define DL_STATE_H

#include <Arduino.h>
#include <dl_lcd.h>

enum class AppState {
    BOOTING,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    MQTT_CONNECTING,
    MQTT_OK,
    READY,
    ERROR
};

struct StateDisplay {
    uint8_t icon_char_code;
    char status_char;
    const char* message;
};

class StateHandler {
public:
    StateHandler();
    void begin(LCDHandler& lcd_handler);
    void setState(AppState new_state);
    AppState getState();

private:
    void registerStateDisplays();
    void updateDisplay();

    LCDHandler* lcd = nullptr;
    AppState currentState;
    StateDisplay stateDisplays[7]; // One for each AppState
    bool initialized = false;
    uint8_t lcd_cols;
    uint8_t lcd_rows;
};

extern StateHandler stateHandler;

#endif // DL_STATE_H
