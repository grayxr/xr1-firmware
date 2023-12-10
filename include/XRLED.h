#ifndef XRLED_h
#define XRLED_h

#include <Arduino.h>
#include <Adafruit_TLC5947.h>
#include <XRConfig.h>

namespace XRLED
{    
    void init();
    void setPWM(uint8_t LED, uint16_t PWM);
    void setPWMDouble(uint8_t LED1, uint16_t PWM1, uint8_t LED2, uint16_t PWM2);
    void showOctaveLEDs(int8_t octave);
    void displayPageLEDs(int currentBar, bool sequencerRunning, int currentStepPage, int lastStep);
    void clearAllStepLEDs();
    void displayCurrentlySelectedPattern();
    void displayCurrentlySelectedTrack();
    void setDisplayStateForAllStepLEDs();

    int8_t getKeyLED(char idx);
}

#endif /* XRLED_h */