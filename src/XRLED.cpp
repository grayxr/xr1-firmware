#include <XRLED.h>
#include <XRSequencer.h>
#include <XRHelpers.h>
#include <XRKeyMatrix.h>
#include <XRUX.h>
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
        Serial.println("calling showOctaveLEDs");

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
        //Serial.println("calling displayPageLEDs");

        //Serial.printf("enter displayPageLEDs, lastStep: %d\n", lastStep);
        
        //bool blinkCurrentPage = _seq_state.playback_state == RUNNING && currentBar != -1;
        bool blinkCurrentPage = sequencerRunning && currentBar != -1;

        // TRACK currTrack = getCurrentSelectedTrack();
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

    void clearAllStepLEDs()
    {
        Serial.println("calling clearAllStepLEDs");

        for (int s = 0; s < 16; s++)
        {
            setPWM(_stepLEDPins[s], 0);
        }
    }

    void displayCurrentlySelectedBank()
    {
        auto currentSelectedBank = XRSequencer::getCurrentSelectedBankNum();

        for (size_t b = 0; b < MAXIMUM_SEQUENCER_BANKS; b++)
        {
            if (b == currentSelectedBank)
            {
                setPWM(_stepLEDPins[b], 4095);
            }
            else
            {
                setPWM(_stepLEDPins[b], 512);
            }
        }
    }

    void displayCurrentlySelectedPattern()
    {
        Serial.println("calling displayCurrentlySelectedPattern");

        //auto &bank = XRSequencer::getActivePatternBank();
        //auto currentSelectedBank = XRSequencer::getCurrentSelectedBankNum();
        auto currentSelectedPattern = XRSequencer::getCurrentSelectedPatternNum();

        for (int8_t p = 0; p < MAXIMUM_SEQUENCER_PATTERNS; p++)
        {
            // TODO: impl pattern, track, bank initialize properly

            //if (bank.banks[currentSelectedBank].patterns[p].initialized)
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
        Serial.println("calling displayCurrentlySelectedTrack");
        //Serial.println("fix displayCurrentlySelectedTrack");
        //return;

        //auto &seqHeap = XRSequencer::getActivePattern();
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

    void displayTrackLayers()
    {
        auto currentSelectedTrackLayer = XRSequencer::getCurrentSelectedTrackLayerNum();

        for (int l = 0; l < MAXIMUM_SEQUENCER_TRACK_LAYERS; l++)
        {
            if (l == currentSelectedTrackLayer)
            {
                setPWM(_stepLEDPins[l], 4095);
            }
            else
            {
                setPWM(_stepLEDPins[l], 512);
            }
        }
    }

    void displaySelectedTrackLayer(bool blinkFillLayer)
    {
        Serial.println("calling displaySelectedTrackLayer");

        auto currentSelectedTrackLayer = XRSequencer::getCurrentSelectedTrackLayerNum();
        auto currentSelectedFillTrackLayer = XRSequencer::getCurrentSelectedFillTrackLayerNum();
        auto queuedFillTrackLayer = XRSequencer::getQueuedFillTrackLayerNum();
        auto queuedTrackLayer = XRSequencer::getQueuedTrackLayer();

        for (int l = 0; l < MAXIMUM_SEQUENCER_TRACK_LAYERS; l++)
        {
            if (l == currentSelectedTrackLayer)
            {
                setPWM(_stepLEDPins[l], 4095);
            }
            else if (l == currentSelectedFillTrackLayer || l == queuedFillTrackLayer || l == queuedTrackLayer)
            {
                //Serial.printf("blinkFillLayer: %d, queuedFillTrackLayer: %d, currentSelectedFillTrackLayer: %d, queuedTrackLayer: %d\n", (int)blinkFillLayer, queuedFillTrackLayer, currentSelectedFillTrackLayer, queuedTrackLayer);

                if (l == queuedFillTrackLayer || l == queuedTrackLayer) {
                    setPWM(_stepLEDPins[l], blinkFillLayer ? 4095 : 512);
                } else {
                    setPWM(_stepLEDPins[l], 512);
                }
            } 
            else {
                setPWM(_stepLEDPins[l], 0);
            }
        }
    }

    void displayChainLEDs(bool blinkChainLayer)
    {
        auto currentSelectedTrackLayer = XRSequencer::getCurrentSelectedTrackLayerNum();
        auto queuedTrackLayer = XRSequencer::getQueuedTrackLayer();

        // Serial.printf("currentSelectedTrackLayer: %d, queuedTrackLayer: %d\n", currentSelectedTrackLayer, queuedTrackLayer);

        // Serial.printf("LayerChain elements: ");
        // for (int i = 0; i < MAXIMUM_SEQUENCER_TRACK_LAYERS; i++) {
        //     Serial.printf("%d ", XRSequencer::layerChainState.chain[i]);
        // }
        // Serial.printf("\n");

        for (int l = 0; l < MAXIMUM_SEQUENCER_TRACK_LAYERS; l++)
        {
            if (XRSequencer::layerChainState.chain[l] > -1)
            {
                auto currChainLayer = XRSequencer::layerChainState.chain[l];

                if (currChainLayer == currentSelectedTrackLayer)
                {
                    setPWM(_stepLEDPins[currChainLayer], 4095);
                }
                else
                {
                    setPWM(_stepLEDPins[currChainLayer], (currChainLayer == queuedTrackLayer && blinkChainLayer) ? 512 : 0);
                }
            }
        }
    }

    void setDisplayStateForAllStepLEDs()
    {
        //Serial.println("calling setDisplayStateForAllStepLEDs");

        auto currUXMode = XRUX::getCurrentMode();
        auto &currTrack = (currUXMode == XRUX::PERFORM_RATCHET || currUXMode == XRUX::SUBMITTING_RATCHET_STEP_VALUE) ? 
            XRSequencer::activeRatchetLayer.tracks[XRSequencer::getRatchetTrack()] : 
            XRSequencer::getCurrentSelectedTrack();

        auto currStepPage = (currUXMode == XRUX::PERFORM_RATCHET || currUXMode == XRUX::SUBMITTING_RATCHET_STEP_VALUE) ? 
            1 : // ratchet track always has only 16 steps max
            XRSequencer::getCurrentStepPage();

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
        Serial.println("calling displayCurrentOctaveLEDs");

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

    void toggleNoteOnForTrackLED(uint8_t t, bool enable)
    {
        auto currLEDChar = XRHelpers::stepCharMap[t+1];
        auto keyLED = XRLED::getKeyLED(currLEDChar);

        Serial.printf("toggleNoteOnForTrackLED: %d, %d\n", t, keyLED);

        XRLED::setPWM(keyLED, enable ? 4095 : 0);
    }

    void displayRatchetTrackLED(bool accented)
    {
        auto currLEDChar = XRHelpers::stepCharMap[XRSequencer::getRatchetTrack() + 1];
        auto keyLED = XRLED::getKeyLED(currLEDChar);

        XRLED::setPWM(keyLED, accented ? 4095 : 512);
    }

    void displayInitializedPatternLEDs()
    {
        // auto &activePatternBank = XRSequencer::getActivePatternBank();

        // for (int p = 0; p < MAXIMUM_SEQUENCER_PATTERNS; p++)
        // {
        //     if (activePatternBank.patterns[p].initialized)
        //     {
        //         setPWM(_stepLEDPins[p], 4095);
        //     }
        //     else
        //     {
        //         setPWM(_stepLEDPins[p], 0);
        //     }
        // }
    }

    void displayInitializedTrackLEDs()
    {
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            if (XRSequencer::activeTrackLayer.tracks[t].initialized)
            {
                setPWM(_stepLEDPins[t], 4095);
            }
            else
            {
                setPWM(_stepLEDPins[t], 0);
            }
        }
    }

    int8_t getKeyLED(char idx)
    {
        if (charLEDMap.count(idx) != 0)
        {
            return charLEDMap[idx];
        }

        return -1;
    }
}