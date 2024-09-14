#ifndef XRDexedManager_h
#define XRDexedManager_h

#include <Arduino.h>

namespace XRDexedManager
{
    void init();

    void swapInstances(); // swap active and inactive instances right before/when pattern starts
    uint8_t getActiveInstanceForTrack(uint8_t track); // get the active instance for the track in the current pattern
    uint8_t getInactiveInstanceForTrack(uint8_t track); // get the inactive instance for the track in the current pattern

    extern uint8_t activeInstances[4];
    extern uint8_t inactiveInstances[4];
}

#endif /* XRDexedManager_h */