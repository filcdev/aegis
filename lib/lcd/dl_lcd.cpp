#include "dl_lcd.h"

LCDHandler::LCDHandler() {}

void LCDHandler::begin(uint8_t addr, uint8_t cols, uint8_t rows) {
    _lcd = new LiquidCrystal_I2C(addr, cols, rows);
    _cols = cols;
    _rows = rows;
    _lcd->init();
    _lcd->backlight();
    initialized = true;
}

void LCDHandler::clear() {
    if (!initialized) return;
    _lcd->clear();
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

// Global LCD handler instance
LCDHandler lcd;
