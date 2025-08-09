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
    bool is_transient;
    AppState next_state;
    unsigned long transient_duration_ms;
};

class StateHandler {
public:
    StateHandler();
    void begin(LCDHandler& lcd_handler);
    void setState(AppState new_state);
    AppState getState();
    void showMessage(const char* message, unsigned int duration_ms);
    void loop();

private:
    void registerStateDisplays();
    void updateDisplay();

    LCDHandler* lcd = nullptr;
    AppState currentState;
    StateDisplay stateDisplays[7];
    bool initialized = false;
    uint8_t lcd_cols;
    uint8_t lcd_rows;
    unsigned long _transient_state_entry_time = 0;

    char _temp_message[17] = "";
    unsigned long _temp_message_expiry = 0;
    bool _is_showing_temp_message = false;
};

extern StateHandler stateHandler;

#endif // DL_STATE_H
