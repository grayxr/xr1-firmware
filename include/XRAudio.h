#ifndef XRAudio_h
#define XRAudio_h

#include <Arduino.h>
#include <Audio.h>
#include <ResponsiveAnalogRead.h>
#include <XRConfig.h>
#include <XRAudioConfig.h>
#include <TeensyVariablePlayback.h>
#include <flashloader.h>

namespace XRAudio
{
    // newdigate::flashloader loader;
    // newdigate::audiosample *samples[MAX_PROJECT_RAW_SAMPLES];

    void init();
    void logMetrics();
    void resetMetrics();
    void handleHeadphones();
}

#endif /* XRAudio_h */