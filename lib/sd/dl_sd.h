#ifndef DL_SD_H
#define DL_SD_H

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include "dl_board_config.h"

class SDCardHandler {
private:
  uint8_t csPin;
  bool initialized = false;

public:
  SDCardHandler(uint8_t chipSelectPin = DL_SD_CS_PIN);

  // Initialize and mount the SD card
  bool begin();

  // Check if SD card is initialized
  bool isInitialized();

  // Get card type as string
  String getCardType();

  // Get card size in MB
  uint64_t getCardSizeMB();

  // List directory contents recursively
  void listDir(const char* dirname, uint8_t levels = 0);

  // Read file contents to Serial
  void readFile(const char* path);

  // Write string to file (overwrite)
  bool writeFile(const char* path, const char* message);

  // Append string to file
  bool appendFile(const char* path, const char* message);

  // Remove file
  bool deleteFile(const char* path);

  // Rename file
  bool renameFile(const char* path1, const char* path2);

  // Create directory
  bool createDir(const char* path);

  // Remove directory
  bool removeDir(const char* path);

  // Expose the underlying FS object (SD)
  fs::FS& fs();
};

// Global instance of SDCardHandler
extern SDCardHandler sdCard;

#endif // DL_SD_H
