#include "dl_lcd.h"
#include "dl_board_config.h"

LCDHandler::LCDHandler() {}

void LCDHandler::begin(uint8_t addr, uint8_t cols, uint8_t rows) {
    _lcd = new LiquidCrystal_I2C(addr, cols, rows);
    _cols = cols;
    _rows = rows;
    _lcd->init();
    _lcd->backlight();
    initialized = true;
}

void LCDHandler::loop() {
    // Handle clearing for non-scrolling messages
    if (_scrollingMessage.isEmpty() && _clearTime > 0 && millis() >= _clearTime) {
        displayMessage("", 0); // Clear the top row
        _clearTime = 0;
        _messageDisplayTime = 0;
        return; // No more processing needed
    }

    // Handle scrolling messages
    if (!_scrollingMessage.isEmpty()) {
        _updateScrollingText();
    }
}

void LCDHandler::clear() {
    if (!initialized) return;
    _lcd->clear();
}

void LCDHandler::displayMessage(const char* message, uint8_t row) {
    if (!initialized) return;
    setCursor(0, row);
    for (int i = 0; i < _cols; i++) {
        print(" ");
    }
    setCursor(0, row);
    print(message);
}

void LCDHandler::displayScrollableMessage(const String& message, uint8_t row) {
    if (!initialized) return;

    // Clear the line first
    displayMessage("", row);
    setCursor(0, row);

    if (message.length() <= _cols) {
        print(message.c_str());
        _scrollingMessage = ""; // Stop any active scrolling
        _messageDisplayTime = millis(); // Set display time for non-scrolling messages too
        _clearTime = _messageDisplayTime + 5000; // It will be cleared after 5s
    } else {
        // Display the initial part of the message
        print(message.substring(0, _cols).c_str());
        
        _scrollingMessage = message;
        _scrollingRow = row;
        _scrollPos = 0;
        _scrollStartTime = millis(); // Record when the message was displayed
        _lastScrollTime = 0;
        _messageDisplayTime = millis();
        _scrollCount = 0;
        _clearTime = 0;
    }
}

void LCDHandler::print(const char* message) {
    if (!initialized) return;
    _lcd->print(message);
}

void LCDHandler::setCursor(uint8_t col, uint8_t row) {
    if (!initialized) return;
    _lcd->setCursor(col, row);
}

void LCDHandler::createChar(uint8_t location, uint8_t charmap[]) {
    if (!initialized) return;
    _lcd->createChar(location, charmap);
}

void LCDHandler::writeChar(uint8_t location) {
    if (!initialized) return;
    _lcd->write(location);
}

void LCDHandler::_updateScrollingText() {
    if (_scrollingMessage.isEmpty()) {
        return;
    }

    // --- Timeout and clearing logic ---
    // Condition: Scrolled twice + 2s
    if (_clearTime > 0 && millis() >= _clearTime) {
        _clearScrollingMessage();
        return;
    }

    // --- Scrolling animation logic ---
    // If _scrollStartTime is set, we are in the initial delay phase
    if (_scrollStartTime > 0) {
        if (millis() - _scrollStartTime >= SCROLL_START_DELAY_MS) {
            _lastScrollTime = millis(); // Start the animation timer
            _scrollStartTime = 0; // Clear the start time to proceed to scrolling
        } else {
            return; // Wait for the initial delay to pass
        }
    }

    if (millis() - _lastScrollTime >= SCROLL_DELAY_MS) {
        _lastScrollTime = millis();

        setCursor(0, _scrollingRow);
        String text_to_display = _scrollingMessage + "   "; // Add padding
        String frame = "";
        for (uint8_t i = 0; i < _cols; i++) {
            frame += text_to_display[(_scrollPos + i) % text_to_display.length()];
        }
        _lcd->print(frame.c_str());

        _scrollPos++;
        if (_scrollPos >= text_to_display.length()) {
            _scrollPos = 0;
            _scrollCount++;
            if (_scrollCount == 2) {
                _clearTime = millis() + 2000; // Set timer to clear after 2s
            }
        }
    }
}

void LCDHandler::_clearScrollingMessage() {
    displayMessage("", _scrollingRow);
    _scrollingMessage = "";
    _scrollPos = 0;
    _scrollCount = 0;
    _clearTime = 0;
    _scrollStartTime = 0;
    _messageDisplayTime = 0;
}

// Global LCD handler instance
LCDHandler lcd;
