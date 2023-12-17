#ifndef XRAsyncPSRAMLoader_h
#define XRAsyncPSRAMLoader_h

#include <Arduino.h>
#include <Audio.h>
#include <TeensyAudioFlashLoader.h>
#include <string>

namespace XRAsyncPSRAMLoader
{
    void handleAsyncPSRAMLoading();
    void postPatternChange();
    void prePatternChange();
    void addSampleFileNameForNextAsyncLoadBatch(std::string filename);
    void startAsyncInitOfNextPattern();
}

#endif /* XRAsyncPSRAMLoader_h */