#ifndef XRMenu_h
#define XRMenu_h

#include <Arduino.h>
#include <XRConfig.h>
#include <string>

namespace XRMenu
{
    std::string *getSoundMenuItems();
    uint8_t getCursorPosition();
    void setCursorPosition(uint8_t idx);
    void resetCursor();
}

#endif /* XRMenu_h */