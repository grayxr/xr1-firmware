#ifndef XREncoder_h
#define XREncoder_h

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <XRConfig.h>

namespace XREncoder
{
    void init();
    void handleStates();
    bool handleMenuCursor(int menuItems);
    void config(int addr, int16_t rmin, int16_t rmax, int16_t rstep, int16_t rval, uint8_t rloop);
    void set(int addr, int16_t rval);
    int16_t get(int address);
}

#endif /* XREncoder_h */