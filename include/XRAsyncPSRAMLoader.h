#ifndef XRASYNCPSRAMLOADER_H
#define XRASYNCPSRAMLOADER_H

#include <Arduino.h>
#include <Audio.h>
#include <TeensyAudioFlashLoader.h>
#include <string>

namespace XRAsyncPSRAMLoader
{
    void handleAsyncPSRAMLoading();
    void postPatternChange();
    void prePatternChange();
    void addSampleFileNameForNextAsyncLoadBatch(const std::string &filename);
    void startAsyncInitOfCurrentPattern();
    void startAsyncInitOfNextPattern(int newBank, int newPattern);
    void startAsyncInitOfNextPattern();
    newdigate::audiosample *getReadSample(const std::string *filename);

    /// reads a file syncronously to the current sample heap (should only be done when not currently playing
    newdigate::audiosample *loadSampleSync(const std::string *filename);

}

#endif //XRASYNCPSRAMLOADER_H
