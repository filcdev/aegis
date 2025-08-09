# GitHub Copilot Instructions

This document provides instructions for GitHub Copilot on how to best assist with the development of this embedded project.

## Project Overview

This is an embedded application for a "dlock" device, built using the PlatformIO ecosystem with the Arduino framework for an ESP32 microcontroller. The project is structured to be modular and easily extensible.

## Technology Stack

- **Language:** C++
- **Framework:** Arduino
- **Build System:** PlatformIO

## Project Structure

The workspace is organized as follows:

- `src/`: Contains the main application logic (`main.cpp`).
- `lib/`: Contains modular libraries (handlers) for different hardware components and functionalities.
  - `lib/config/`: Handles loading configuration from a JSON file.
  - `lib/sd/`: Manages the SD card.
  - `lib/lcd/`: Controls the I2C LCD display.
- `include/`: Contains global header files.
  - `dl_board_config.h`: Defines hardware-specific constants like pin numbers and I2C addresses. This is the single source of truth for hardware configuration.
- `data/`: Contains data files to be uploaded to the device's filesystem, such as `config.json`.
- `platformio.ini`: The PlatformIO project configuration file, including library dependencies.

## Best Practices & Coding Conventions

1.  **Hardware Abstraction:** All hardware-specific values (pin numbers, I2C addresses, etc.) should be defined as macros in `include/dl_board_config.h`. Avoid hardcoding these values in `.cpp` files.

2.  **Modular Handlers:** Functionality should be encapsulated within handler classes in the `lib/` directory. For example, all SD card operations are in `SDCardHandler`, and all LCD operations are in `LCDHandler`.

3.  **Global Instances:** Each handler has a single global instance created in its respective `.cpp` file (e.g., `sdCard`, `config`, `lcd`). These instances are made available globally via an `extern` declaration in the header file.

4.  **Initialization and Error Handling:**
    - Core components like the SD card and configuration are initialized in the `setup()` function in `src/main.cpp`.
    - If a critical component fails to initialize, the `die()` function should be called. This function prints an error message to the Serial monitor and restarts the device after a delay.

5.  **Configuration:**
    - Runtime configuration (like WiFi credentials) is loaded from `/config.json` on the SD card using the `Config` handler.
    - Hardware configuration is compile-time and lives in `include/dl_board_config.h`.

## How to Add a New Feature (e.g., a new sensor)

1.  **Create a New Handler:**
    - Create a new subdirectory in `lib/` for the new component (e.g., `lib/new_sensor/`).
    - Inside, create a header (`.h`) and a source (`.cpp`) file for the new handler class (e.g., `dl_new_sensor.h`, `dl_new_sensor.cpp`).
    - Follow the existing pattern: define the class in the header, implement it in the source file, and create a global `extern` instance.

2.  **Add Hardware Configuration:**
    - If the new component requires any hardware-specific definitions (pins, addresses), add them to `include/dl_board_config.h`.

3.  **Integrate into `main.cpp`:**
    - Include the new handler's header file in `src/main.cpp`.
    - Initialize the new handler in the `setup()` function. Use `die()` if initialization is critical and fails.
    - Use the handler's methods in the `loop()` function or elsewhere as needed.

4.  **Add Dependencies:**
    - If the new handler requires an external library, add it to the `lib_deps` section of `platformio.ini`.
