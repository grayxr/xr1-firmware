#include <XRAsyncPSRAMLoader.h>
#include <TeensyAudioFlashLoader.h>
#include <string>
#include <utility>
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

    void addSampleFileNameForNextAsyncLoadBatch(std::string filename){
        fileNames[currentWriteHeap][numWriteHeapSamplesRequested] = std::move(filename);
        numWriteHeapSamplesRequested++;
    }

    void handleAsyncPSRAMLoading() {
        if (failed || writeHeapHasLoaded) return;

        if (numWriteHeapSamplesLoaded < numWriteHeapSamplesRequested) {
            if (asyncOpening) {
                const auto filename = fileNames[currentWriteHeap][numWriteHeapSamplesLoaded];
                asyncLoadFile = SD.open(filename.c_str());
                samples[currentWriteHeap][numWriteHeapSamplesLoaded] = asyncflashloader.beginAsyncLoad(asyncLoadFile);
                if (!samples[currentWriteHeap][numWriteHeapSamplesLoaded]) {
                    failed = true;
                    Serial.printf("abort!!! can not find '%s'...(heap:%d, index:%d)\n", filename.c_str(),
                                  currentWriteHeap, numWriteHeapSamplesLoaded);
                    return;
                }
                asyncOpening = false;
                Serial.printf("loading '%s'...      (heap:%d, index:%d, pointer:%x)\t\t", filename.c_str(),
                              currentWriteHeap, numWriteHeapSamplesLoaded,
                              samples[currentWriteHeap][numWriteHeapSamplesLoaded]->sampledata);
            } else if (asyncflashloader.continueAsyncLoadPartial()) {
                numWriteHeapSamplesLoaded++;
                asyncLoadFile.close();
                asyncOpening = true;
                if (numWriteHeapSamplesLoaded == numWriteHeapSamplesRequested) {
                    Serial.printf("Write heap has completed loading...      (heap:%d, index:%d, pointer:%x)\t\t",
                                  currentWriteHeap, numWriteHeapSamplesLoaded,
                                  samples[currentWriteHeap][numWriteHeapSamplesLoaded]->sampledata);
                    writeHeapHasLoaded = true;
                }
            }
        }
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
        auto newPatternData = XRSequencer::getSequencerExternal().banks[newBank].patterns[newPattern];
        for (auto track: newPatternData.tracks) {
            if (track.track_type == XRSequencer::RAW_SAMPLE) {
                if (strcmp(track.sample_name, "") != 0) {
                    auto sampleName = std::string("/audio enjoyer/xr-1/samples/") + std::string(track.sample_name);
                    Serial.printf("initializing this sample name: %s\n", sampleName.c_str());
                    XRAsyncPSRAMLoader::addSampleFileNameForNextAsyncLoadBatch(sampleName);
                }
            }
        }
    }

    newdigate::audiosample* getReadSample(const std::string &filename) {
        auto it = std::find( fileNames[currentReadHeap].begin(), fileNames[currentReadHeap].end(), filename);
        size_t index = it - fileNames[currentReadHeap].begin();
        if (it != std::end(fileNames[currentReadHeap])) {
            if (index < samples[currentReadHeap].size())
                return samples[currentReadHeap][index];
        }

        return nullptr;
    }

}