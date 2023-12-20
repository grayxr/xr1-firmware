#ifndef XRAudio_h
#define XRAudio_h

#include <Arduino.h>
#include <Audio.h>
#include <ResponsiveAnalogRead.h>
#include <XRConfig.h>
#include <XRAudioConfig.h>

namespace XRAudio
{
    FLASHMEM void init();
    void logMetrics();
    void resetMetrics();
    void handleHeadphones();
}

#endif /* XRAudio_h */