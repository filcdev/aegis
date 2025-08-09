#include "dl_sd.h"
#include "dl_board_config.h"

SDCardHandler::SDCardHandler(uint8_t chipSelectPin) : csPin(chipSelectPin) {}

bool SDCardHandler::begin() {
  if (!SD.begin(csPin)) {
    Serial.println("Card Mount Failed");
    initialized = false;
    return false;
  }

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    initialized = false;
    return false;
  }

  initialized = true;

  Serial.print("SD Card Type: ");
  switch (cardType) {
    case CARD_MMC: Serial.println("MMC"); break;
    case CARD_SD: Serial.println("SDSC"); break;
    case CARD_SDHC: Serial.println("SDHC"); break;
    default: Serial.println("UNKNOWN"); break;
  }

  Serial.printf("SD Card Size: %lluMB\n", SD.cardSize() / (1024 * 1024));
  return true;
}

bool SDCardHandler::isInitialized() {
  return initialized;
}

String SDCardHandler::getCardType() {
  uint8_t cardType = SD.cardType();
  switch (cardType) {
    case CARD_MMC: return "MMC";
    case CARD_SD: return "SDSC";
    case CARD_SDHC: return "SDHC";
    default: return "UNKNOWN";
  }
}

uint64_t SDCardHandler::getCardSizeMB() {
  return SD.cardSize() / (1024 * 1024);
}

void SDCardHandler::listDir(const char* dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = SD.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void SDCardHandler::readFile(const char* path) {
  Serial.printf("Reading file: %s\n", path);

  File file = SD.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

bool SDCardHandler::writeFile(const char* path, const char* message) {
  Serial.printf("Writing file: %s\n", path);

  File file = SD.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return false;
  }

  bool success = file.print(message);
  file.close();

  Serial.println(success ? "File written" : "Write failed");
  return success;
}

bool SDCardHandler::appendFile(const char* path, const char* message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = SD.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return false;
  }

  bool success = file.print(message);
  file.close();

  Serial.println(success ? "Message appended" : "Append failed");
  return success;
}

bool SDCardHandler::deleteFile(const char* path) {
  Serial.printf("Deleting file: %s\n", path);
  if (SD.remove(path)) {
    Serial.println("File deleted");
    return true;
  } else {
    Serial.println("Delete failed");
    return false;
  }
}

bool SDCardHandler::renameFile(const char* path1, const char* path2) {
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (SD.rename(path1, path2)) {
    Serial.println("File renamed");
    return true;
  } else {
    Serial.println("Rename failed");
    return false;
  }
}

bool SDCardHandler::createDir(const char* path) {
  Serial.printf("Creating Dir: %s\n", path);
  if (SD.mkdir(path)) {
    Serial.println("Dir created");
    return true;
  } else {
    Serial.println("mkdir failed");
    return false;
  }
}

bool SDCardHandler::removeDir(const char* path) {
  Serial.printf("Removing Dir: %s\n", path);
  if (SD.rmdir(path)) {
    Serial.println("Dir removed");
    return true;
  } else {
    Serial.println("rmdir failed");
    return false;
  }
}

fs::FS& SDCardHandler::fs() {
  return SD;
}

// Global instance of SDCardHandler
SDCardHandler sdCard(DL_SD_CS_PIN);