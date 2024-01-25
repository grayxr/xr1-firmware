#ifndef XRKeyMatrix_h
#define XRKeyMatrix_h

#include <Arduino.h>
#include <Keypad.h>
#include <XRConfig.h>

namespace XRKeyMatrix
{
    void init();
    void handleStates(bool discard = false);
    bool isFunctionActive();
    int8_t getKeyboardOctave();
    extern Keypad kpd;
}

#endif /* XRKeyMatrix_h */