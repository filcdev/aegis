#ifndef DL_LCD_H
#define DL_LCD_H

#include <LiquidCrystal_I2C.h>

class LCDHandler {
public:
    LCDHandler();

    // Initialize the LCD
    void begin(uint8_t addr, uint8_t cols, uint8_t rows);

    // Main loop for handling scrolling text
    void loop();

    // Clear the display
    void clear();

    // Print a static message to the LCD
    void print(const char* message);

    // Display a message on a specific line, clearing the line first
    void displayMessage(const char* message, uint8_t row);

    // Display a message that can be scrolled if it's too long
    void displayScrollableMessage(const String& message, uint8_t row);

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
    void _updateScrollingText();
    void _clearScrollingMessage();

    LiquidCrystal_I2C* _lcd = nullptr;
    bool initialized = false;
    uint8_t _cols;
    uint8_t _rows;

    String _scrollingMessage;
    uint8_t _scrollingRow;
    int _scrollPos = 0;
    unsigned long _lastScrollTime = 0;
    unsigned long _scrollStartTime = 0;
    unsigned long _messageDisplayTime = 0;
    int _scrollCount = 0;
    unsigned long _clearTime = 0;
};

// Global LCD handler instance
extern LCDHandler lcd;

#endif // DL_LCD_H
