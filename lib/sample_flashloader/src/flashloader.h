/* Audio Library for Teensy
 * Copyright (c) 2021, Nic Newdigate
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef TEENSY_SAMPLE_FLASHLOADER_FLASHLOADER_H
#define TEENSY_SAMPLE_FLASHLOADER_FLASHLOADER_H

#include <Arduino.h>
#include <SD.h>
#include "audiosample.h"

extern "C" uint8_t external_psram_size;

namespace newdigate {

    const uint32_t flashloader_default_sd_buffersize = 4 * 1024;

    class flashloader {
    public:

        flashloader() : _samples() {
            _bytes_available = external_psram_size * 1048576;
            _lastPointer = 0x0A;
        }

        std::vector<audiosample*> _samples;
        uint32_t _bytes_available;
        uint32_t _lastPointer;
        audiosample * loadSample(const char *filename );

        // load array to play via AudioPlayMemory object
        audiosample * loadAudioPlayMemorySample(const char *filename );

        // free all existing samples
        void clearSamples() {
            AudioNoInterrupts();
            for (auto && sample : _samples){
#ifdef BUILD_FOR_LINUX
                delete [] sample->sampledata;
                delete sample;
#else
                extmem_free(sample->sampledata);
#endif
                _bytes_available += sample->samplesize;
            }
            _samples.clear();
            AudioInterrupts();
        }
    };
};
#endif