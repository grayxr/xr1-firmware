#ifndef XRMenu_h
#define XRMenu_h

#include <Arduino.h>
#include <XRConfig.h>
#include <string>

namespace XRMenu
{
    std::string *getSoundMenuItems();
    std::string *getSetupMenuItems();
    int16_t getCursorPosition();
    void setCursorPosition(int16_t idx);
    void resetCursor();
    void incCursorPosition();
    void decCursorPosition();
}

#endif /* XRMenu_h */