#ifndef XRLED_h
#define XRLED_h

#include <Arduino.h>
#include <Adafruit_TLC5947.h>
#include <XRConfig.h>

namespace XRLED
{
    extern int8_t _stepLEDPins[16];

    void init();
    void setPWM(uint8_t LED, uint16_t PWM);
    void setPWMDouble(uint8_t LED1, uint16_t PWM1, uint8_t LED2, uint16_t PWM2);
    void showOctaveLEDs(int8_t octave);
    void displayPageLEDs(int currentBar, bool sequencerRunning, int currentStepPage, int lastStep);
    void clearPageLEDs();
    void clearAllStepLEDs();
    void displayCurrentlySelectedBank();
    void displayCurrentlySelectedPattern();
    void displayCurrentlySelectedTrack();
    void setDisplayStateForAllStepLEDs();
    void displayCurrentOctaveLEDs(int8_t octave);
    void displayPerformModeLEDs();
    void displayMuteLEDs();
    void displayInitializedPatternLEDs();
    void displayInitializedTrackLEDs();
    void displayTrackLayers();
    void toggleNoteOnForTrackLED(uint8_t t, bool enable);
    void displayRatchetTrackLED(bool accented);

    int8_t getKeyLED(char idx);
    extern Adafruit_TLC5947 tlc;
}

#endif /* XRLED_h */