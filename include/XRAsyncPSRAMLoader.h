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
    newdigate::audiosample *getReadSample(const std::string &filename);
}
#endif /* XRAsyncPSRAMLoader_h */