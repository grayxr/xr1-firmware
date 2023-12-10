#ifndef XRCV_h
#define XRCV_h

#include <Arduino.h>
#include <SPI.h>
#include <XRConfig.h>

namespace XRCV
{
    void init();
    void write(int chip, int channel, int value);
}

#endif /* XRCV_h */