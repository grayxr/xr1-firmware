#ifndef XRKeyMatrix_h
#define XRKeyMatrix_h

#include <Arduino.h>
#include <Keypad.h>
#include <XRConfig.h>

namespace XRKeyMatrix
{    
    void init();
    void handleStates(bool discard = false);
    void handlePressForKey(char key);
    void handleHoldForKey(char key);
    void handleReleaseForKey(char key);

    bool isFunctionActive();
}

#endif /* XRKeyMatrix_h */