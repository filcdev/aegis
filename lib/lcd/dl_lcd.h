#ifndef DL_LCD_H
#define DL_LCD_H

#include <LiquidCrystal_I2C.h>

class LCDHandler {
public:
    LCDHandler();

    // Initialize the LCD
    void begin(uint8_t addr, uint8_t cols, uint8_t rows);

    // Clear the display
    void clear();

    // Print a message to the LCD
    void print(const char* message);

    // Set the cursor position
    void setCursor(uint8_t col, uint8_t row);

    // Create a custom character
    void createChar(uint8_t location, uint8_t charmap[]);

    // Write a custom character to the LCD
    void writeChar(uint8_t location);

    // Getters for LCD dimensions
    uint8_t getCols() { return _cols; }
    uint8_t getRows() { return _rows; }

private:
    LiquidCrystal_I2C* _lcd = nullptr;
    bool initialized = false;
    uint8_t _cols;
    uint8_t _rows;
};

// Global LCD handler instance
extern LCDHandler lcd;

#endif // DL_LCD_H
