#include "dl_lcd.h"
#include "dl_board_config.h"
#include "dl_concurrency.h"

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
    if (_scrollingMessage.isEmpty() && _clearTime > 0 && millis() >= _clearTime) {
        displayMessage("", 0);
        _clearTime = 0;
        _messageDisplayTime = 0;
        return;
    }

    if (!_scrollingMessage.isEmpty()) {
        _updateScrollingText();
    }
}

void LCDHandler::clear() {
    if (!initialized) return;
    LcdLockGuard lock; if (!lock.locked()) return;
    _lcd->clear();
}

void LCDHandler::displayMessage(const char* message, uint8_t row) {
    if (!initialized) return;
    LcdLockGuard lock; if (!lock.locked()) return;
    setCursor(0, row);
    for (int i = 0; i < _cols; i++) {
        _lcd->print(" ");
    }
    setCursor(0, row);
    _lcd->print(message);
}

void LCDHandler::displayScrollableMessage(const String& message, uint8_t row) {
    if (!initialized) return;

    LcdLockGuard lock; if (!lock.locked()) return;

    displayMessage("", row);
    setCursor(0, row);

    if (message.length() <= _cols) {
        _scrollingMessage = "";
        _messageDisplayTime = millis();
        _clearTime = _messageDisplayTime + 5000;
        _lcd->print(message.c_str());
    } else {
        _lcd->print((message.substring(0, _cols - 1) + String(">")).c_str());
        _scrollingMessage = message;
        _scrollingRow = row;
        _scrollPos = 0;
        _scrollStartTime = millis();
        _lastScrollTime = 0;
        _messageDisplayTime = millis();
        _scrollCount = 0;
        _clearTime = 0;
    }
}

void LCDHandler::print(const char* message) {
    if (!initialized) return;
    LcdLockGuard lock; if (!lock.locked()) return;
    _lcd->print(message);
}

void LCDHandler::setCursor(uint8_t col, uint8_t row) {
    if (!initialized) return;
    _lcd->setCursor(col, row);
}

void LCDHandler::createChar(uint8_t location, uint8_t charmap[]) {
    if (!initialized) return;
    LcdLockGuard lock; if (!lock.locked()) return;
    _lcd->createChar(location, charmap);
}

void LCDHandler::writeChar(uint8_t location) {
    if (!initialized) return;
    LcdLockGuard lock; if (!lock.locked()) return;
    _lcd->write(location);
}

void LCDHandler::_updateScrollingText() {
    if (_scrollingMessage.isEmpty()) {
        return;
    }

    if (_scrollStartTime > 0) {
        if (millis() - _scrollStartTime >= SCROLL_START_DELAY_MS) {
            _lastScrollTime = millis();
            _scrollStartTime = 0;
        } else {
            return;
        }
    }

    if (millis() - _lastScrollTime >= SCROLL_DELAY_MS) {
        _lastScrollTime = millis();

        if (_scrollCount >= 2) {
            _clearScrollingMessage();
            return;
        }

        LcdLockGuard lock; if (!lock.locked()) return;
        setCursor(0, _scrollingRow);
        String text_to_display = _scrollingMessage + "   ";
        String frame = "";
        for (uint8_t i = 0; i < _cols - 1; i++) {
            frame += text_to_display[(_scrollPos + i) % text_to_display.length()];
        }
        _lcd->print(frame.c_str());

        setCursor(_cols - 1, _scrollingRow);
        _lcd->print(">");

        _scrollPos++;
        if (_scrollPos >= text_to_display.length()) {
            _scrollPos = 0;
            _scrollCount++;
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

LCDHandler lcd;
