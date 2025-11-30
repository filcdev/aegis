#ifndef DZ_LED_H
#define DZ_LED_H
#include <Adafruit_NeoPixel.h>
#include "dz_config.h"

class DZLEDControl
{
public:
    DZLEDControl();
    void begin();
    void handle();
private:
    Adafruit_NeoPixel pixels;
    void testSequence();

    unsigned long lastMillis;
    float breathePhase;
    int doorSeqState;
    unsigned long doorSeqStart;
    float doorPulsePhase;
    int doorPulsesDone;
    int errSeqState;
    unsigned long errSeqStart;

    void handleErrorState(unsigned long now);
    void handleDoorState(unsigned long now, unsigned long dt);
    void handleIdleState(unsigned long dt);
};

#endif