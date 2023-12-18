#include <XRLED.h>
#include <XRSequencer.h>
#include <XRHelpers.h>
#include <XRKeyMatrix.h>
#include <map>

namespace XRLED
{
    int8_t _stepLEDPins[16] = {
        0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15, 16
    };

    std::map<char, uint8_t> charLEDMap = {
        {'m', 0},
        {'n', 1},
        {'o', 2},
        {'p', 3},
        {'q', 23},
        {'s', 4},
        {'t', 5},
        {'u', 6},
        {'v', 7},
        {'w', 22},
        {'y', 9},
        {'z', 10},
        {'1', 11},
        {'2', 12},
        {'5', 13},
        {'6', 14},
        {'7', 15},
        {'8', 16},
    };

    Adafruit_TLC5947 tlc(NUM_TLC5947, tlc5947_clock, tlc5947_data, tlc5947_latch);

    void init()
    {
        tlc.begin();

        if (tlc5947_oe >= 0) {
            pinMode(tlc5947_oe, OUTPUT);
            digitalWrite(tlc5947_oe, LOW);
        }

        for (int i=0; i<25; i++) {
            setPWM(i, 0); // sets all 24 outputs to no brightness
        }
    }

    void setPWM(uint8_t LED, uint16_t PWM)
    {
        tlc.setPWM(LED, PWM);
        tlc.write();
    }

    void setPWMDouble(uint8_t LED1, uint16_t PWM1, uint8_t LED2, uint16_t PWM2)
    {
        tlc.setPWM(LED1, PWM1);
        tlc.setPWM(LED2, PWM2);
        tlc.write();  
    }

    void showOctaveLEDs(int8_t octave)
    {
        switch (octave)
        {
        case 1:
            setPWM(17, 100);
            setPWM(18, 25);
            setPWM(19, 0);
            setPWM(20, 0);
            break;
        
        case 2:
            setPWM(17, 25);
            setPWM(18, 25);
            setPWM(19, 0);
            setPWM(20, 0);
            break;
        
        case 3:
            setPWM(17, 0);
            setPWM(18, 25);
            setPWM(19, 0);
            setPWM(20, 0);
            break;
        
        case 4:
            setPWM(17, 0);
            setPWM(18, 0);
            setPWM(19, 0);
            setPWM(20, 0);
            break;
        
        case 5:
            setPWM(17, 0);
            setPWM(18, 0);
            setPWM(19, 25);
            setPWM(20, 0);
            break;
        
        case 6:
            setPWM(17, 0);
            setPWM(18, 0);
            setPWM(19, 25);
            setPWM(20, 25);
            break;
        
        case 7:
            setPWM(17, 0);
            setPWM(18, 0);
            setPWM(19, 25);
            setPWM(20, 100);
            break;
        
        default:
            break;
        }
    }

    void displayPageLEDs(int currentBar, bool sequencerRunning, int currentStepPage, int lastStep)
    {
        // Serial.printf("enter displayPageLEDs, lastStep: %d\n", lastStep);
        
        //bool blinkCurrentPage = _seq_state.playback_state == RUNNING && currentBar != -1;
        bool blinkCurrentPage = sequencerRunning && currentBar != -1;

        // TRACK currTrack = getHeapCurrentSelectedTrack();
        // uint16_t pageOneBrightnessMin = (current_step_page == 1) ? 50 : 5;
        // uint16_t pageTwoBrightnessMin = (current_step_page == 2) ? 50 : (currTrack.last_step > 16 ? 5 : 0);
        // uint16_t pageThreeBrightnessMin = (current_step_page == 3) ? 50 : (currTrack.last_step > 32 ? 5 : 0);
        // uint16_t pageFourBrightnessMin = (current_step_page == 4) ? 50 : (currTrack.last_step > 48 ? 5 : 0);

        uint16_t pageOneBrightnessMin = (currentStepPage == 1) ? 50 : 5;
        uint16_t pageTwoBrightnessMin = (currentStepPage == 2) ? 50 : (lastStep > 16 ? 5 : 0);
        uint16_t pageThreeBrightnessMin = (currentStepPage == 3) ? 50 : (lastStep > 32 ? 5 : 0);
        uint16_t pageFourBrightnessMin = (currentStepPage == 4) ? 50 : (lastStep > 48 ? 5 : 0);

        uint16_t pageOneBrightnessMax = blinkCurrentPage && (currentBar == 1) ? pageOneBrightnessMin + 100 : pageOneBrightnessMin;
        uint16_t pageTwoBrightnessMax = blinkCurrentPage && (currentBar == 2) ? pageTwoBrightnessMin + 100 : pageTwoBrightnessMin;
        uint16_t pageThreeBrightnessMax = blinkCurrentPage && (currentBar == 3) ? pageThreeBrightnessMin + 100 : pageThreeBrightnessMin;
        uint16_t pageFourBrightnessMax = blinkCurrentPage && (currentBar == 4) ? pageFourBrightnessMin + 100 : pageFourBrightnessMin;

        setPWM(17, pageOneBrightnessMax);
        setPWM(18, pageTwoBrightnessMax);
        setPWM(19, pageThreeBrightnessMax);
        setPWM(20, pageFourBrightnessMax);
    }

    int8_t getKeyLED(char idx)
    {
        if (charLEDMap.count(idx) != 0)
        {
            return charLEDMap[idx];
        }

        return -1;
    }

    void clearAllStepLEDs()
    {
        for (int s = 0; s < 16; s++)
        {
            setPWM(_stepLEDPins[s], 0);
        }
    }

    void displayCurrentlySelectedPattern()
    {
        //auto &sequencer = XRSequencer::getExternalSequencer();
        //auto currentSelectedBank = XRSequencer::getCurrentSelectedBankNum();
        auto currentSelectedPattern = XRSequencer::getCurrentSelectedPatternNum();

        for (int8_t p = 0; p < MAXIMUM_SEQUENCER_PATTERNS; p++)
        {
            // TODO: impl pattern, track, bank initialize properly

            //if (sequencer.banks[currentSelectedBank].patterns[p].initialized)
            //{
                if (p == currentSelectedPattern)
                {
                    setPWM(_stepLEDPins[p], 4095);
                }
                else
                {
                    setPWM(_stepLEDPins[p], 512);
                }
            // }
            // else
            // {
            //     setPWM(_stepLEDPins[p], 0);
            // }
        }
    }

    void displayCurrentlySelectedTrack()
    {
        //Serial.println("fix displayCurrentlySelectedTrack");
        //return;

        //auto &seqHeap = XRSequencer::getHeapPattern();
        auto currentSelectedTrack = XRSequencer::getCurrentSelectedTrackNum();

        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            // TODO: impl pattern, track, bank initialize properly

            // if (seqHeap.pattern.tracks[t].initialized)
            // {
                if (t == currentSelectedTrack)
                {
                    setPWM(_stepLEDPins[t], 4095);
                }
                else
                {
                    setPWM(_stepLEDPins[t], 512);
                }
            // }
            // else
            // {
            //     setPWM(_stepLEDPins[t], 0);
            // }
        }
    }

    void setDisplayStateForAllStepLEDs()
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto currStepPage = XRSequencer::getCurrentStepPage();

        const int MAX_TRACK_LEDS_SIZE = 17;

        for (int l = 1; l < MAX_TRACK_LEDS_SIZE; l++)
        {
            int stepToUse = l;

            // todo: check if current track has last_step > 16
            // if so, use proper offset to get correct step state for current page
            if (currStepPage == 2)
            {
                stepToUse += 16;
            }
            else if (currStepPage == 3)
            {
                stepToUse += 32;
            }
            else if (currStepPage == 4)
            {
                stepToUse += 48;
            }

            auto currTrackStepForLED = currTrack.steps[stepToUse - 1];
            int8_t curr_led_char = XRHelpers::stepCharMap[l];
            int8_t keyLED = getKeyLED(curr_led_char);

            if (stepToUse > currTrack.lstep)
            {
                setPWM(keyLED, 0);
                continue;
            }

            if (currTrackStepForLED.state == XRSequencer::STEP_STATE::STATE_OFF)
            {
                if (keyLED < 0)
                {
                    Serial.println("could not find key LED!");
                }
                else
                {
                    setPWM(keyLED, 0);
                }
            }
            else if (currTrackStepForLED.state == XRSequencer::STEP_STATE::STATE_ON)
            {
                setPWM(keyLED, 512); // 256 might be better
            }
            else if (currTrackStepForLED.state == XRSequencer::STEP_STATE::STATE_ACCENTED)
            {
                setPWM(keyLED, 4095);
            }
        }
    }

    void displayCurrentOctaveLEDs(int8_t octave)
    {
        switch (octave)
        {
        case 1:
            setPWM(17, 100);
            setPWM(18, 25);
            setPWM(19, 0);
            setPWM(20, 0);
            break;

        case 2:
            setPWM(17, 25);
            setPWM(18, 25);
            setPWM(19, 0);
            setPWM(20, 0);
            break;

        case 3:
            setPWM(17, 0);
            setPWM(18, 25);
            setPWM(19, 0);
            setPWM(20, 0);
            break;

        case 4:
            setPWM(17, 0);
            setPWM(18, 0);
            setPWM(19, 0);
            setPWM(20, 0);
            break;

        case 5:
            setPWM(17, 0);
            setPWM(18, 0);
            setPWM(19, 25);
            setPWM(20, 0);
            break;

        case 6:
            setPWM(17, 0);
            setPWM(18, 0);
            setPWM(19, 25);
            setPWM(20, 25);
            break;

        case 7:
            setPWM(17, 0);
            setPWM(18, 0);
            setPWM(19, 25);
            setPWM(20, 100);
            break;

        default:
            break;
        }
    }

    void clearPageLEDs()
    {
        setPWM(17, 0);
        setPWM(18, 0);
        setPWM(19, 0);
        setPWM(20, 0);
    }

    void displayPerformModeLEDs(void)
    {
        for (int s = 12; s < 16; s++)
        {
            setPWM(_stepLEDPins[s], 4095);
        }
    }

    void displayMuteLEDs(void)
    {
        auto &currPattern = XRSequencer::getHeapCurrentSelectedPattern();

        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            if (!XRSequencer::trackPerformState[t].muted)
            {
                setPWM(_stepLEDPins[t], 0);
            }
            else
            {
                setPWM(_stepLEDPins[t], 4095);
            }
        }
    }

    void displayInitializedPatternLEDs()
    {
        auto &sequencer = XRSequencer::getExternalSequencer();
        auto currSelBank = XRSequencer::getCurrentSelectedBankNum();

        for (int p = 0; p < MAXIMUM_SEQUENCER_PATTERNS; p++)
        {
            if (sequencer.banks[currSelBank].patterns[p].initialized)
            {
                setPWM(_stepLEDPins[p], 4095);
            }
            else
            {
                setPWM(_stepLEDPins[p], 0);
            }
        }
    }

    void displayInitializedTrackLEDs()
    {
        auto &pattern = XRSequencer::getHeapCurrentSelectedPattern();

        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            if (pattern.tracks[t].initialized)
            {
                setPWM(_stepLEDPins[t], 4095);
            }
            else
            {
                setPWM(_stepLEDPins[t], 0);
            }
        }
    }
}