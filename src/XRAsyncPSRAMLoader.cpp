#include <XRAsyncPSRAMLoader.h>
#include <TeensyAudioFlashLoader.h>
#include <string>
#include <XRSequencer.h>
#include <algorithm>

namespace XRAsyncPSRAMLoader
{
    std::vector<std::vector<std::string>> fileNames = { {}, {} };
    std::vector<std::vector<newdigate::audiosample*>>  samples = { {}, {} };
    newdigate::dualheapasyncflashloader asyncflashloader;
    File asyncLoadFile;
    bool writeHeapHasLoaded = false, readHeapHasLoaded = false, asyncOpening = true, asyncChanging = false;
    uint32_t currentWriteHeap = 0, currentReadHeap = 1, numWriteHeapSamplesRequested = 0, numWriteHeapSamplesLoaded = 0;

    bool failed = false;
    bool hasInitialized = false;

    void addSampleFileNameForNextAsyncLoadBatch(const std::string &filename){
        if (numWriteHeapSamplesRequested >= fileNames[currentWriteHeap].size() ) {
            fileNames[currentWriteHeap].push_back(filename);
        } else
            fileNames[currentWriteHeap][numWriteHeapSamplesRequested] = filename;
        numWriteHeapSamplesRequested++;
    }

    inline void heap_switch() {
        //Serial.printf("Heap switch init: currentWriteHeap=%d currentReadHeap=%d !\n", currentWriteHeap, currentReadHeap);
        currentWriteHeap ++;
        currentWriteHeap %= 2;
        currentReadHeap = 1 - currentWriteHeap;
        numWriteHeapSamplesLoaded = 0;
        numWriteHeapSamplesRequested = 0;
        readHeapHasLoaded = writeHeapHasLoaded;
        writeHeapHasLoaded = false;

        std::vector<newdigate::audiosample*> &samples_write = samples[currentWriteHeap];
        samples_write.clear();

        std::vector<std::string> &filenames_write = fileNames[currentWriteHeap];
        filenames_write.clear();

        Serial.printf("Heap switch complete: currentWriteHeap=%d currentReadHeap=%d !\n", currentWriteHeap, currentReadHeap);
    }

    void handleAsyncPSRAMLoading() {
        if (failed || writeHeapHasLoaded) return;

        if (numWriteHeapSamplesLoaded < numWriteHeapSamplesRequested) {
            if (asyncOpening) {
                const auto &filename = fileNames[currentWriteHeap][numWriteHeapSamplesLoaded];

                asyncLoadFile = SD.open(filename.c_str());
                auto *sample = asyncflashloader.beginAsyncLoad(asyncLoadFile);
                if (numWriteHeapSamplesLoaded >= samples[currentWriteHeap].size() ) {
                    samples[currentWriteHeap].push_back(sample);
                } else
                    samples[currentWriteHeap][numWriteHeapSamplesLoaded] = sample;
                if (!samples[currentWriteHeap][numWriteHeapSamplesLoaded]) {
                    failed = true;
                    Serial.printf("abort!!! can not find '%s'...(heap:%d, index:%d)\n", filename.c_str(), currentWriteHeap, numWriteHeapSamplesLoaded);
                    return;
                }
                asyncOpening = false;
                Serial.printf("loading '%s'...      (heap:%d, index:%d)\t\t", filename.c_str(), currentWriteHeap, numWriteHeapSamplesLoaded);
            } else if (asyncflashloader.continueAsyncLoadPartial()) {
                numWriteHeapSamplesLoaded++;
                asyncLoadFile.close();
                asyncOpening = true;
                if (numWriteHeapSamplesLoaded == numWriteHeapSamplesRequested) {
                    Serial.printf("Write heap has completed loading...      (heap:%d, loaded:%d)\n",  currentWriteHeap, numWriteHeapSamplesLoaded);
                    writeHeapHasLoaded = true;
                    if (!hasInitialized) {
                        hasInitialized = true;
                        prePatternChange();
                        postPatternChange();
                    }
                }
            }
        }
    }


    void postPatternChange() {
        if (asyncChanging) {
            Serial.println("Pattern switch (all voices from previous pattern should be stopped if necessary before calling postPatternChange switch)");
            asyncChanging = false;

            if (numWriteHeapSamplesLoaded < numWriteHeapSamplesRequested) {
                Serial.println("WARN: not all the samples managed to load in time!");
            }

            asyncflashloader.toggle_afterNewPatternStarts();
            heap_switch();
        }
    }

    void prePatternChange() {
        if (!asyncChanging) {
            // just before pattern change...
            // after this call, the audiosample will be deallocated, so don't play any new samples from the heap.
            // Existing samples will continue to play because the psram will still be intact
            asyncflashloader.toggle_beforeNewPatternVoicesStart();
            asyncChanging = true;
        }
    }

    void startAsyncInitOfNextPattern() {
        auto queued = XRSequencer::getQueuedPattern();
        int newBank = queued.bank;
        int newPattern = queued.number;
        auto &newPatternData = XRSequencer::getSequencer().banks[newBank].patterns[newPattern];
        unsigned i = 0;
        for (auto &track: newPatternData.tracks) {
            auto &sound = XRSound::nextPatternSounds[i];
            if (sound.type == XRSound::T_MONO_SAMPLE) {
                if (strcmp(sound.sampleName, "") != 0) {
                    auto sampleName = std::string("/audio enjoyer/xr-1/samples/") + std::string(sound.sampleName);
                    Serial.printf("initializing this sample name: %s\n", sampleName.c_str());
                    addSampleFileNameForNextAsyncLoadBatch(sampleName);
                }
            }
            i++;
        }
    }

    void startAsyncInitOfCurrentPattern() {
        auto &newPatternData = XRSequencer::getHeapPattern();
        unsigned i = 0;
        for (auto &track: newPatternData.tracks) {
            auto &sound = XRSound::nextPatternSounds[i];
            if (sound.type == XRSound::T_MONO_SAMPLE) {
                if (strcmp(sound.sampleName, "") != 0) {
                    const auto &sampleName = std::string("/audio enjoyer/xr-1/samples/") + std::string(sound.sampleName);
                    Serial.printf("initializing this sample name: %s\n", sampleName.c_str());
                    addSampleFileNameForNextAsyncLoadBatch(sampleName);
                }
            }
            i++;
        }
    }


    newdigate::audiosample* getReadSample(const std::string *filename) {
        if (!filename) return nullptr;
        auto it = std::find( fileNames[currentReadHeap].begin(), fileNames[currentReadHeap].end(), *filename);
        size_t index = it - fileNames[currentReadHeap].begin();
        if (it != std::end(fileNames[currentReadHeap])) {
            if (index < samples[currentReadHeap].size())
                return samples[currentReadHeap][index];
        }

        return nullptr;
    }

    newdigate::audiosample *loadSampleSync(const std::string *filename) {
        auto result = getReadSample(filename);
        if (result == nullptr) {
            result = asyncflashloader.loadSample(filename->c_str());
            if (result) {
                samples[currentReadHeap].push_back(result);
                fileNames[currentReadHeap].push_back(*filename);
            }
        }
        return result;
    }

}