#include "dl_state.h"

// Custom character definitions
byte wifi_char[8] = {
  0b00000,
  0b01110,
  0b10001,
  0b00100,
  0b01010,
  0b00000,
  0b00100,
  0b00000
};

byte check_char[8] = {
  0b00000,
  0b00001,
  0b00011,
  0b10110,
  0b11100,
  0b01000,
  0b00000,
  0b00000
};

byte cross_char[8] = {
  0b00000,
  0b10001,
  0b01010,
  0b00100,
  0b01010,
  0b10001,
  0b00000,
  0b00000
};

byte boot_char[8] = {
  0b01110,
  0b10001,
  0b10101,
  0b10101,
  0b10001,
  0b01110,
  0b00000,
  0b00000
};

byte mqtt_char[8] = {
  0b00000,
  0b01110,
  0b10001,
  0b00110,
  0b01001,
  0b10010,
  0b01100,
  0b00000
};


StateHandler::StateHandler() {}

void StateHandler::begin(LCDHandler& lcd_handler) {
    this->lcd = &lcd_handler;
    this->lcd_cols = lcd_handler.getCols();
    this->lcd_rows = lcd_handler.getRows();
    
    // Create custom characters for states
    lcd->createChar(0, boot_char);
    lcd->createChar(1, wifi_char);
    lcd->createChar(2, check_char);
    lcd->createChar(3, cross_char);
    lcd->createChar(4, mqtt_char);

    registerStateDisplays();
    initialized = true;
}

void StateHandler::registerStateDisplays() {
    stateDisplays[(int)AppState::BOOTING] = {0, 'B', "Booting..."};
    stateDisplays[(int)AppState::WIFI_CONNECTING] = {1, 'W', "Connecting WiFi"};
    stateDisplays[(int)AppState::WIFI_CONNECTED] = {2, 'W', "WiFi OK"};
    stateDisplays[(int)AppState::MQTT_CONNECTING] = {4, 'M', "Connecting MQTT"};
    stateDisplays[(int)AppState::MQTT_OK] = {2, 'M', "MQTT OK"};
    stateDisplays[(int)AppState::READY] = {2, 'R', "Ready"};
    stateDisplays[(int)AppState::ERROR] = {3, 'E', "Error!"};
}

void StateHandler::setState(AppState new_state) {
    if (!initialized) return;
    currentState = new_state;
    updateDisplay();
}

void StateHandler::showMessage(const char* message, unsigned int duration_ms) {
    if (!initialized) return;
    strncpy(_temp_message, message, 16);
    _temp_message[16] = '\0';
    _temp_message_expiry = millis() + duration_ms;
    _is_showing_temp_message = true;
    updateDisplay();
}

void StateHandler::loop() {
    if (_is_showing_temp_message && millis() >= _temp_message_expiry) {
        _is_showing_temp_message = false;
        updateDisplay();
    }

    // Transition from MQTT_OK to READY after a delay
    if (currentState == AppState::MQTT_OK) {
        static unsigned long mqtt_ok_time = 0;
        if (mqtt_ok_time == 0) {
            mqtt_ok_time = millis();
        }
        if (millis() - mqtt_ok_time > 2000) {
            setState(AppState::READY);
            mqtt_ok_time = 0; // Reset for next time
        }
    }
}

AppState StateHandler::getState() {
    return currentState;
}

void StateHandler::updateDisplay() {
    if (!initialized || !lcd) return;

    lcd->clear();

    // Display temporary message if active, otherwise display current state message
    if (_is_showing_temp_message) {
        lcd->setCursor(0, 0);
        lcd->print(_temp_message);
    } else {
        StateDisplay display = stateDisplays[(int)currentState];
        lcd->setCursor(0, 0);
        lcd->print(display.message);
    }
    
    // Display status icon and char on the bottom row, right-aligned
    StateDisplay display = stateDisplays[(int)currentState];
    lcd->setCursor(lcd_cols - 2, 1);
    lcd->writeChar(display.icon_char_code);
    lcd->print(String(display.status_char).c_str());
}

StateHandler stateHandler;
