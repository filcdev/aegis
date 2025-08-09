#include "dl_sd.h"
#include "dl_board_config.h"
#include "dl_logger.h"

static Logger logger("CARD");

SDCardHandler::SDCardHandler(uint8_t chipSelectPin) : csPin(chipSelectPin) {}

bool SDCardHandler::begin() {
  if (!SD.begin(csPin)) {
    logger.error("Mount Failed");
    initialized = false;
    return false;
  }

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    logger.error("No SD card attached");
    initialized = false;
    return false;
  }

  initialized = true;

  logger.info("SD Card Type: %s", getCardType().c_str());
  logger.info("SD Card Size: %lluMB", SD.cardSize() / (1024 * 1024));
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
  logger.info("Listing directory: %s", dirname);

  File root = SD.open(dirname);
  if (!root) {
    logger.error("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    logger.error("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      logger.info("  DIR : %s", file.name());
      if (levels) {
        listDir(file.name(), levels - 1);
      }
    } else {
      logger.info("  FILE: %s, SIZE: %d", file.name(), file.size());
    }
    file = root.openNextFile();
  }
}

void SDCardHandler::readFile(const char* path) {
  logger.info("Reading file: %s", path);

  File file = SD.open(path);
  if (!file) {
    logger.error("Failed to open file for reading");
    return;
  }

  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

bool SDCardHandler::writeFile(const char* path, const char* message) {
  logger.info("Writing file: %s", path);

  File file = SD.open(path, FILE_WRITE);
  if (!file) {
    logger.error("Failed to open file for writing");
    return false;
  }

  bool success = file.print(message);
  file.close();

  if(success) {
    logger.info("File written");
  } else {
    logger.error("Write failed");
  }
  return success;
}

bool SDCardHandler::appendFile(const char* path, const char* message) {
  logger.info("Appending to file: %s", path);

  File file = SD.open(path, FILE_APPEND);
  if (!file) {
    logger.error("Failed to open file for appending");
    return false;
  }

  bool success = file.print(message);
  file.close();

  if(success) {
    logger.info("Message appended");
  } else {
    logger.error("Append failed");
  }
  return success;
}

bool SDCardHandler::deleteFile(const char* path) {
  logger.info("Deleting file: %s", path);
  if (SD.remove(path)) {
    logger.info("File deleted");
    return true;
  } else {
    logger.error("Delete failed");
    return false;
  }
}

bool SDCardHandler::renameFile(const char* path1, const char* path2) {
  logger.info("Renaming file %s to %s", path1, path2);
  if (SD.rename(path1, path2)) {
    logger.info("File renamed");
    return true;
  } else {
    logger.error("Rename failed");
    return false;
  }
}

bool SDCardHandler::createDir(const char* path) {
  logger.info("Creating Dir: %s", path);
  if (SD.mkdir(path)) {
    logger.info("Dir created");
    return true;
  } else {
    logger.error("mkdir failed");
    return false;
  }
}

bool SDCardHandler::removeDir(const char* path) {
  logger.info("Removing Dir: %s", path);
  if (SD.rmdir(path)) {
    logger.info("Dir removed");
    return true;
  } else {
    logger.error("rmdir failed");
    return false;
  }
}

fs::FS& SDCardHandler::fs() {
  return SD;
}

// Global instance of SDCardHandler
SDCardHandler sdCard(DL_SD_CS_PIN);