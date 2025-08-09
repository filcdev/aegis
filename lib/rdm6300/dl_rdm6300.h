#pragma once

#include <Arduino.h>
#include <rdm6300.h>

class RDM6300Handler {
public:
    void begin();
    String readTag();
private:
    Rdm6300 rdm;
};

extern RDM6300Handler rdm6300Handler;
