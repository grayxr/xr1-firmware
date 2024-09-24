#include <XREncoder.h>
#include <XRUX.h>
#include <XRMenu.h>
#include <XRDisplay.h>
#include <XRClock.h>
#include <XRSD.h>
#include <XRSequencer.h>
#include <XRLED.h>
#include <XRVersa.h>
#include <XRKeyMatrix.h>

using namespace XRSound;

namespace XREncoder
{
    elapsedMillis elapsedMs;

    int addresses[5] = {
        0x36, 0x37, 0x38, 0x39, 0x40
    };

    int16_t currentValues[5] = {
        0, 0, 0, 0, 0
    };

    int16_t lastValues[5] = {
        0, 0, 0, 0, 0
    };

    void handleEncoderSetTempo(int diff);
    void handleEncoderSetStepMicrotime(int diff);
    void handleEncoderSetPatternMods();
    void handleEncoderSetTrackMods();
    void handleEncoderSetRatchetMods();
    void handleEncoderTraversePages(int diff);
    void handleEncoderTraverseRatchetPages(int diff);
    void handleEncoderPatternModA(int diff);
    void handleEncoderPatternModB(int diff);
    void handleEncoderPatternModC(int diff);
    void handleEncoderPatternModD(int diff);
    void handleEncoderMonoSynthModA(int diff);
    void handleEncoderMonoSynthModB(int diff);
    void handleEncoderMonoSynthModC(int diff);
    void handleEncoderMonoSynthModD(int diff);
    void handleEncoderDexedSynthModA(int diff);
    void handleEncoderDexedSynthModB(int diff);
    void handleEncoderDexedSynthModC(int diff);
    void handleEncoderDexedSynthModD(int diff);
    void handleEncoderFmDrumModA(int diff);
    void handleEncoderFmDrumModB(int diff);
    void handleEncoderFmDrumModC(int diff);
    void handleEncoderFmDrumModD(int diff);
    void handleEncoderMonoSampleModA(int diff);
    void handleEncoderMonoSampleModB(int diff);
    void handleEncoderMonoSampleModC(int diff);
    void handleEncoderMonoSampleModD(int diff);
    void handleEncoderWavSampleModA(int diff);
    void handleEncoderWavSampleModB(int diff);
    void handleEncoderWavSampleModC(int diff);
    void handleEncoderWavSampleModD(int diff);
    void handleEncoderCvGateModA(int diff);
    void handleEncoderCvGateModB(int diff);
    void handleEncoderCvGateModC(int diff);
    void handleEncoderCvGateModD(int diff);

    void updateTrackLength(int diff);
    void updateTrackLastStep(int diff);
    void updateMonoSynthWaveform(int diff);
    void updateMonoSynthNoiseAmt(int diff);
    void updateMonoSynthFilterEnvAttack(int diff);
    void updateTrackAmpEnvAttack(int diff);
    void updateComboTrackLevel(int diff);
    void updateTrackProbability(int diff);

    bool handleMenuCursor(XRUX::UX_MODE menuMode, int diff);

    void handleDexedBrowserMod1();
    void handleDexedBrowserMod2();
    void handleDexedBrowserMod3();

    int getDiff(int address);

    void init()
    {
        Wire1.begin();

        for (int i = 0; i < 5; i++)
        {
            config(addresses[i], -3000, 3000, 1, 0, 0);
        }
    }

    void config(int addr, int16_t rmin, int16_t rmax, int16_t rstep, int16_t rval, uint8_t rloop)
    {
        Wire1.beginTransmission(addr);
        Wire1.write((uint8_t)(rval & 0xff));
        Wire1.write((uint8_t)(rval >> 8));
        Wire1.write(0);
        Wire1.write(rloop);
        Wire1.write((uint8_t)(rmin & 0xff));
        Wire1.write((uint8_t)(rmin >> 8));
        Wire1.write((uint8_t)(rmax & 0xff));
        Wire1.write((uint8_t)(rmax >> 8));
        Wire1.write((uint8_t)(rstep & 0xff));
        Wire1.write((uint8_t)(rstep >> 8));
        Wire1.endTransmission();
    }

    void set(int addr, int16_t rval)
    {
        Wire1.beginTransmission(addr);
        Wire1.write((uint8_t)(rval & 0xff));
        Wire1.write((uint8_t)(rval >> 8));
        Wire1.endTransmission();
    }

    void setCurrentValueForAddress(int address)
    {
        currentValues[address] = get(addresses[address]);
    }

    int16_t get(int address)
    {
        Wire1.requestFrom(address, 2);

        return ((uint16_t)Wire1.read() | ((uint16_t)Wire1.read() << 8));
    }

    int getDiff(int address)
    {
        // set current state
        currentValues[address] = get(addresses[address]);

        int diff = currentValues[address] - lastValues[address];

        // set latest state
        lastValues[address] = currentValues[address];

        return diff;
    }

    void handleStates()
    {
        auto currentUXMode = XRUX::getCurrentMode();

        if (currentUXMode == XRUX::UX_MODE::SOUND_MENU_DEXED_SYSEX_BROWSER) {
            handleDexedBrowserMod1();
            handleDexedBrowserMod2();
            handleDexedBrowserMod3();

            return;
        }

        // if (!(elapsedMs % 25) && currentUXMode == XRUX::UX_MODE::SET_TEMPO) {
        //     handleEncoderSetTempo();

        //     return;
        // }

        bool uxModesToSetTempoFrom = (
            currentUXMode == XRUX::UX_MODE::PATTERN_WRITE || 
            currentUXMode == XRUX::UX_MODE::TRACK_WRITE
        );

        if (uxModesToSetTempoFrom && XRKeyMatrix::isFunctionActive()) {
            int diff = getDiff(MAIN_ENCODER_ADDRESS);
            handleEncoderSetTempo(diff);

            return;
        }

        if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE) {
            int diff = getDiff(MAIN_ENCODER_ADDRESS);
            handleEncoderTraversePages(diff);
            handleEncoderSetPatternMods();

            return;
        }

        if (
            currentUXMode == XRUX::UX_MODE::PERFORM_RATCHET || 
            currentUXMode == XRUX::UX_MODE::SUBMITTING_RATCHET_STEP_VALUE
        ) {
            int diff = getDiff(MAIN_ENCODER_ADDRESS);
            handleEncoderTraverseRatchetPages(diff);
            handleEncoderSetRatchetMods();

            return;
        }

        if (
            currentUXMode == XRUX::UX_MODE::TRACK_WRITE || 
            currentUXMode == XRUX::UX_MODE::SUBMITTING_STEP_VALUE ||
            currentUXMode == XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK_SOUND
        ) {
            int diff = getDiff(MAIN_ENCODER_ADDRESS);

            if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE) {
                handleEncoderTraversePages(diff);
            } else if (currentUXMode == XRUX::UX_MODE::SUBMITTING_STEP_VALUE) {
                handleEncoderSetStepMicrotime(diff);
            }

            handleEncoderSetTrackMods();

            if (currentUXMode != XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK_SOUND) {
                return;
            }
        }

        bool uxModeWithMenu = (
            currentUXMode == XRUX::UX_MODE::SOUND_MENU_MAIN || 
            currentUXMode == XRUX::UX_MODE::CHANGE_SETUP  || 
            currentUXMode == XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK_SOUND
        );

        if (uxModeWithMenu) {
            int diff = getDiff(MAIN_ENCODER_ADDRESS);
            handleMenuCursor(currentUXMode, diff);
            
            return;
        }
    }

    void handleEncoderSetTempo(int diff)
    {
        if (diff != 0)
        {
            float currTempo = XRClock::getTempo();
            float newTempo = currTempo + diff;

            if (!(newTempo < 30 || newTempo > 300))
            {
                if ((newTempo - currTempo >= 1) || (currTempo - newTempo) >= 1)
                {
                    uClock.setTempo(newTempo);

                    XRSD::_current_project.tempo = newTempo;

                    XRDisplay::drawSequencerScreen(false);
                }
            }
        }
    }

    void handleEncoderSetStepMicrotime(int diff)
    {
        if (diff != 0)
        {
            auto &currPattern = XRSequencer::getCurrentSelectedPattern();
            auto &currTrack = XRSequencer::getCurrentSelectedTrack();
            auto &currLayer = XRSequencer::getCurrentSelectedTrackLayer();
            auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
            auto currStep = XRSequencer::getCurrentSelectedStepNum();

            // initialize a shuffle template for the track
            int8_t trackShuffleTemplate[MAXIMUM_SEQUENCER_STEPS];
            
            // try to build the template for the track based on the pattern first
            int currGrooveId = currPattern.groove.id;
            int currGrooveAmt = currPattern.groove.amount;
            if (currGrooveId != -1) { // pattern has a groove template
                auto currPatternTemplate = XRClock::getShuffleTemplateForGroove(currGrooveId, currGrooveAmt);
                size_t currPatternTemplateLength = sizeof(currPatternTemplate);

                for (size_t i = 0; i < MAXIMUM_SEQUENCER_STEPS; i++)
                {
                    trackShuffleTemplate[i] = currPatternTemplate[i % currPatternTemplateLength];
                }
            } else {
                // pattern doesn't have groove but we need ot enable shuffle on the clock
                // so we can apply microtiming to the track
                XRClock::setShuffle(true);
            }

            // then get any existing step mods for the entire track and apply them to the template
            for (size_t i = 0; i < MAXIMUM_SEQUENCER_STEPS; i++)
            {
                if (currLayer.tracks[currTrackNum].steps[i].flags[XRSequencer::MICROTIMING]) {
                    trackShuffleTemplate[i] = currLayer.tracks[currTrackNum].steps[i].mods[XRSequencer::MICROTIMING];
                }
            }

            // then compare the current microtiming value with the new one to see if it needs to be updated
            auto currStepMicrotiming = trackShuffleTemplate[currStep];

            // step 1 is not allowed negative microtiming (TODO: fix?)
            // because when negative microtiming is applied to step 1, 
            // it doesn't advance the seqState.trackSteps[t].currentStep
            int8_t leftLimit = currStep == 0 ? 0 : -12;
            int8_t rightLimit = 12;

            int8_t newStepMicrotiming = constrain(currStepMicrotiming + diff, leftLimit, rightLimit);

            if (newStepMicrotiming != currStepMicrotiming)
            {
                // set the mod
                currLayer.tracks[currTrackNum].steps[currStep].flags[XRSequencer::MICROTIMING] = true;
                currLayer.tracks[currTrackNum].steps[currStep].mods[XRSequencer::MICROTIMING] = newStepMicrotiming;

                // set the new shuffle template for the track
                trackShuffleTemplate[currStep] = newStepMicrotiming;
                XRClock::setShuffleTemplateForTrack(currTrackNum, trackShuffleTemplate, currTrack.lstep);

                // // print tmpl as one string
                Serial.printf("enc track template: ");
                for (size_t i = 0; i < currTrack.lstep; i++)
                {
                    Serial.printf("%d, ", trackShuffleTemplate[i]);
                }
                Serial.printf("\n");
            }

            // draw the microtiming value on the screen
            std::string microtimingStr = std::to_string(newStepMicrotiming);
            
            if (newStepMicrotiming > 0)
            {
                microtimingStr = "+" + microtimingStr;
            }

            XRDisplay::drawStepMicrotimingOverlay(microtimingStr);
        }
    }


    bool handleMenuCursor(XRUX::UX_MODE menuMode, int diff)
    {
        if (diff != 0)
        {
            auto menuItems = 0;

            if (menuMode == XRUX::UX_MODE::SOUND_MENU_MAIN) {
                menuItems = SOUND_MENU_ITEM_MAX;

                auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();

                if (activeSounds[currTrackNum].type == T_DEXED_SYNTH) {
                    menuItems = DEXED_SOUND_MENU_ITEM_MAX;
                }
            } else if (menuMode == XRUX::UX_MODE::CHANGE_SETUP) {
                menuItems = SETUP_MENU_ITEM_MAX;
            } else if (menuMode == XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK_SOUND) {
                menuItems = 255;
            } else {
                return false;
            }

            if (menuItems > 2) {
                uint8_t currPos = XRMenu::getCursorPosition();
                uint8_t newPos = constrain(currPos + diff, 0, menuItems - 1);

                XRMenu::setCursorPosition(newPos);
            }

            if (menuMode == XRUX::UX_MODE::SOUND_MENU_MAIN) {
                XRDisplay::drawSoundMenuMain();
            } else if (menuMode == XRUX::UX_MODE::CHANGE_SETUP) {
                XRDisplay::drawSetupMenu();
            } else if (menuMode == XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK_SOUND) {
                XRDisplay::drawSampleBrowser();
            }

            return true;
        }

        return false;
    }

    void handleEncoderTraversePages(int diff)
    {
        auto tPage = XRSequencer::getCurrentSelectedPage();

        int newPage = tPage + diff;

        if (newPage != tPage) {
            auto currUXMode = XRUX::getCurrentMode();

            int maxPages = currUXMode == XRUX::PATTERN_WRITE ? 2 : XRSound::getPageCountForCurrentTrack();

            if (maxPages == 1) {
                return;
            }

            if (newPage > (maxPages - 1)) {
                newPage = 0;
            } else if (newPage < 0) {
                newPage = maxPages - 1;
            }

            XRSequencer::setSelectedPage(newPage);
            
            XRDisplay::drawSequencerScreen(false);
        }
    }

    void handleEncoderTraverseRatchetPages(int diff)
    {
        auto rPage = XRSequencer::getCurrentRatchetPageNum();

        int newPage = rPage + diff;

        if (newPage != rPage) {
            int maxPages = 2;

            if (newPage > (maxPages - 1)) {
                newPage = 0;
            } else if (newPage < 0) {
                newPage = maxPages - 1;
            }

            XRSequencer::setCurrentRatchetPageNum(newPage);

            if (newPage == 1) {
                XRLED::setDisplayStateForAllStepLEDs();
            } else {
                XRLED::clearAllStepLEDs();
                XRLED::displayRatchetTrackLED(false);
            }
            
            XRDisplay::drawSequencerScreen(false);
        }
    }

    void handleEncoderSetRatchetMods()
    {
        const int modCount = 4;
        for (int m = 0; m < modCount; m++)
        {
            int diff = getDiff(m + 1);

            if (diff != 0)
            {
                if (m == 0)
                {
                    auto ratchetTrackNum = XRSequencer::getRatchetTrack();
                    auto &ratchetTrack = XRSequencer::activePattern.ratchetLayer.tracks[ratchetTrackNum];

                    // last step
                    int currLastStep = ratchetTrack.lstep;
                    int newLastStep = constrain(ratchetTrack.lstep + diff, 1, 16);

                    if (newLastStep != currLastStep)
                    {
                        ratchetTrack.lstep = newLastStep;

                        XRLED::setDisplayStateForAllStepLEDs();
                        XRDisplay::drawSequencerScreen(false);
                    }
                }
                else if (m == 1)
                {
                    auto ratchetTrackNum = XRSequencer::getRatchetTrack();
                    auto &ratchetTrack = XRSequencer::activePattern.ratchetLayer.tracks[ratchetTrackNum];

                    // note
                    int currNote = ratchetTrack.note;
                    int newNote = ratchetTrack.note + diff;

                    if (newNote != currNote)
                    {
                        auto newOctave = ratchetTrack.octave;

                        if (newNote < 0) {
                            newNote = 12 - newNote;
                            --newOctave;
                        } else if (newNote > 12) {
                            newNote = newNote - 12;
                            ++newOctave;
                        }

                        if (newOctave < 1 || newOctave > 7) {
                            return;
                        }

                        ratchetTrack.note = newNote;
                        ratchetTrack.octave = newOctave;

                        XRDisplay::drawSequencerScreen(false);
                    }
                }
                else if (m == 2)
                {
                    auto ratchetTrackNum = XRSequencer::getRatchetTrack();
                    auto &ratchetTrack = XRSequencer::activePattern.ratchetLayer.tracks[ratchetTrackNum];

                    // velocity
                    int currVelo = ratchetTrack.velocity;
                    int newVelo = constrain(ratchetTrack.velocity + diff, 0, 100);

                    if (newVelo != currVelo)
                    {
                        ratchetTrack.velocity = newVelo;

                        XRDisplay::drawSequencerScreen(false);
                    }
                }
                else if (m == 3)
                {
                    auto ratchetTrackNum = XRSequencer::getRatchetTrack();
                    auto &ratchetTrack = XRSequencer::activePattern.ratchetLayer.tracks[ratchetTrackNum];

                    // ratchet latch
                    bool ratchetLatched = XRSequencer::ratchetLatched;

                    if (diff < 0)
                    {
                        XRSequencer::ratchetLatched = false;
                        
                        XRSequencer::setRatchetDivision(-1);
                        XRSequencer::setCurrentRatchetStep(1);
                        XRSequencer::resetRatchetBar();
                        XRLED::setDisplayStateForAllStepLEDs();
                    } else {
                        XRSequencer::ratchetLatched = true;
                    }
                    
                    XRDisplay::drawSequencerScreen(false);
                }
            }
        }
    }
    
    void handleEncoderSetPatternMods()
    {
        const int modCount = 4;
        for (int m = 0; m < modCount; m++)
        {
            int diff = getDiff(m + 1);

            if (diff != 0)
            {
                if (m == 0)
                {
                    handleEncoderPatternModA(diff);
                }
                else if (m == 1)
                {
                    handleEncoderPatternModB(diff);
                }
                else if (m == 2)
                {
                    handleEncoderPatternModC(diff);
                }
                else if (m == 3)
                {
                    handleEncoderPatternModD(diff);
                }
            }
        }
    }

    void handleEncoderPatternModA(int diff)
    {
        auto &currPattern = XRSequencer::getCurrentSelectedPattern();
        auto &currTrack = XRSequencer::getCurrentSelectedTrack();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        switch (currSelectedPageNum)
        {
        case 0:
            {
                int currLastStep = currPattern.lstep;
                int newLastStep = currPattern.lstep + diff;

                if (newLastStep < 1) {
                    newLastStep = 1;
                } else if (newLastStep > MAXIMUM_SEQUENCER_STEPS) {
                    newLastStep = MAXIMUM_SEQUENCER_STEPS;
                }

                if (newLastStep != currLastStep)
                {
                    currPattern.lstep = newLastStep;

                    for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
                    {
                        // set track's last_step to match pattern if track last_step is greater than pattern's
                        if (currTrack.lstep > newLastStep)
                        {
                            currTrack.lstep = newLastStep;
                        }
                    }

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            break;
        case 1:
            {
                auto delayParams = currPattern.fx.pages[XRSequencer::PATTERN_FX_PAGE_INDEXES::DELAY]; 

                float currDelayTimeMs = delayParams.params[XRSequencer::PATTERN_FX_DELAY_PARAMS::TIME];
                float newDelayTimeMs = currDelayTimeMs + diff;

                newDelayTimeMs = constrain(newDelayTimeMs, 10, 1000);

                if (newDelayTimeMs != currDelayTimeMs)
                {
                    currPattern.fx.pages[XRSequencer::PATTERN_FX_PAGE_INDEXES::DELAY].params[XRSequencer::PATTERN_FX_DELAY_PARAMS::TIME] = newDelayTimeMs;
                    
                    AudioNoInterrupts();

                    XRSound::delayInstances[0].delayEffect.delay(0, newDelayTimeMs);

                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            break;
        
        default:
            break;
        }
    }

    void handleEncoderPatternModB(int diff)
    {
        auto &currPattern = XRSequencer::getCurrentSelectedPattern();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        switch (currSelectedPageNum)
        {
        case 0:
            {
                int currGrooveId = currPattern.groove.id;
                int newGrooveId = currPattern.groove.id + diff;

                if (newGrooveId < -1)  {
                    newGrooveId = -1;
                } else if (newGrooveId > MAXIMUM_GROOVE_CONFIGS - 1) {
                    newGrooveId = MAXIMUM_GROOVE_CONFIGS - 1;
                }

                if (newGrooveId != currGrooveId)  {
                    currPattern.groove.id = newGrooveId;
                    currPattern.groove.amount = 0;

                    Serial.printf("currPattern.groove.id: %d, currPattern.groove.amount: %d\n", currPattern.groove.id, currPattern.groove.amount);

                    if (newGrooveId == -1) {
                        XRClock::setShuffle(false);
                        XRClock::setShuffleForAllTracks(false);
                    } else {
                        XRClock::setShuffleTemplateForGroove(currPattern.groove.id, currPattern.groove.amount);
                        XRClock::setShuffle(true);

                        XRClock::setShuffleTemplateForGrooveForAllTracks(currPattern.groove.id, currPattern.groove.amount);
                        XRClock::setShuffleForAllTracks(true);
                    }

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            break;
        case 1:
            {
                auto delayParams = currPattern.fx.pages[XRSequencer::PATTERN_FX_PAGE_INDEXES::DELAY]; 

                float currDelayFeedback = delayParams.params[XRSequencer::PATTERN_FX_DELAY_PARAMS::FEEDBACK];
                float newDelayFeedback = currDelayFeedback + (diff * 0.01);

                newDelayFeedback = constrain(newDelayFeedback, 0, 1.0);

                if (newDelayFeedback != currDelayFeedback)
                {
                    currPattern.fx.pages[XRSequencer::PATTERN_FX_PAGE_INDEXES::DELAY].params[XRSequencer::PATTERN_FX_DELAY_PARAMS::FEEDBACK] = newDelayFeedback;

                    AudioNoInterrupts();

                    XRSound::delayInstances[0].feedbackMix.gain(0, newDelayFeedback);

                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            break;
        
        default:
            break;
        }
    }

    void handleEncoderPatternModC(int diff)
    {
        auto &currPattern = XRSequencer::getCurrentSelectedPattern();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        switch (currSelectedPageNum)
        {
        case 0:
            {
                int currGrooveAmt = currPattern.groove.amount;
                int newGrooveAmt = currPattern.groove.amount + diff;

                auto g = XRClock::getShuffleTemplateCountForGroove(currPattern.groove.id);
                Serial.printf("getShuffleTemplateCountForGroove: %d\n", g);

                if (newGrooveAmt < 0) {
                    newGrooveAmt = 0;
                } else if (newGrooveAmt > g - 1) {
                    newGrooveAmt = g - 1;
                }

                Serial.printf("currPattern.groove.id: %d\n", currPattern.groove.id);
                Serial.printf("newGrooveAmt: %d\n", newGrooveAmt);

                if (newGrooveAmt != currGrooveAmt)
                {
                    currPattern.groove.amount = newGrooveAmt;

                    XRClock::setShuffleTemplateForGroove(currPattern.groove.id, newGrooveAmt);
                    XRClock::setShuffleTemplateForGrooveForAllTracks(currPattern.groove.id, newGrooveAmt);

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            break;
        
        case 1:
            {
                auto delayParams = currPattern.fx.pages[XRSequencer::PATTERN_FX_PAGE_INDEXES::DELAY]; 

                float currDelayPan = delayParams.params[XRSequencer::PATTERN_FX_DELAY_PARAMS::PAN];
                float newDelayPan = currDelayPan + (diff * 0.01);

                newDelayPan = constrain(newDelayPan, -1.0, 1.0);

                Serial.printf("currDelayPan: %f newDelayPan: %f\n",currDelayPan,newDelayPan);

                if (newDelayPan != currDelayPan)
                {
                    currPattern.fx.pages[XRSequencer::PATTERN_FX_PAGE_INDEXES::DELAY].params[XRSequencer::PATTERN_FX_DELAY_PARAMS::PAN] = newDelayPan;

                    float newGainL = 1.0;
                    if (newDelayPan < 0)
                    {
                        newGainL += newDelayPan;
                    }

                    float newGainR = 1.0;
                    if (newDelayPan > 0)
                    {
                        newGainR -= newDelayPan;
                    }
                    
                    AudioNoInterrupts();

                    XRSound::delayInstances[0].left.gain(newGainR);
                    XRSound::delayInstances[0].right.gain(newGainL);

                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            break;
        
        default:
            break;
        }
    }

    void handleEncoderPatternModD(int diff)
    {
        auto &currPattern = XRSequencer::getCurrentSelectedPattern();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        switch (currSelectedPageNum)
        {
        case 0:
            {
                int currAccent = currPattern.accent;
                int newAccent = currPattern.accent + diff;

                // TODO: allow global accent to go to 0 or 1?
                if (newAccent < 1) {
                    newAccent = 1;
                } else if (newAccent > 100) {
                    newAccent = 100;
                }

                if (newAccent != currAccent)
                {
                    currPattern.accent = newAccent;

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            break;
        case 1:
            //
            break;
        
        default:
            break;
        }
    }

    void handleEncoderSetTrackMods()
    {
        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();

        for (int m = 1; m < 5; m++) // starting from 1 since 1-4 are the mod encoders, 0 is main
        {
            int mDiff = getDiff(m);

            if (mDiff == 0) continue;

            // Serial.printf("m: %d, mDiff: %d\n", m, mDiff);

            if (XRSound::activeSounds[currTrackNum].type == T_MONO_SAMPLE)
            {
                if (m == 1) {
                    handleEncoderMonoSampleModA(mDiff);
                } else if (m == 2) {
                    handleEncoderMonoSampleModB(mDiff);
                } else if (m == 3) {
                    handleEncoderMonoSampleModC(mDiff);
                } else if (m == 4) {
                    handleEncoderMonoSampleModD(mDiff);
                }
            }
            else if (XRSound::activeSounds[currTrackNum].type == T_MONO_SYNTH)
            {
                if (m == 1) {
                    handleEncoderMonoSynthModA(mDiff);
                } else if (m == 2) {
                    handleEncoderMonoSynthModB(mDiff);
                } else if (m == 3) {
                    handleEncoderMonoSynthModC(mDiff);
                } else if (m == 4) {
                    handleEncoderMonoSynthModD(mDiff);
                }
            }
            else if (XRSound::activeSounds[currTrackNum].type == T_DEXED_SYNTH)
            {
                if (m == 1) {
                    handleEncoderDexedSynthModA(mDiff);
                } else if (m == 2) {
                    handleEncoderDexedSynthModB(mDiff);
                } else if (m == 3) {
                    handleEncoderDexedSynthModC(mDiff);
                } else if (m == 4) {
                    handleEncoderDexedSynthModD(mDiff);
                }
            }
            else if (XRSound::activeSounds[currTrackNum].type == T_FM_DRUM)
            {
                if (m == 1) {
                    handleEncoderFmDrumModA(mDiff);
                } else if (m == 2) {
                    handleEncoderFmDrumModB(mDiff);
                } else if (m == 3) {
                    handleEncoderFmDrumModC(mDiff);
                } else if (m == 4) {
                    handleEncoderFmDrumModD(mDiff);
                }
            }
            else if (XRSound::activeSounds[currTrackNum].type == T_CV_GATE)
            {
                // if (m == 1) {
                //     handleEncoderCvGateModA(mDiff);
                // } else if (m == 2) {
                //     handleEncoderCvGateModB(mDiff);
                // } else if (m == 3) {
                //     handleEncoderCvGateModC(mDiff);
                // } else if (m == 4) {
                //     handleEncoderCvGateModD(mDiff);
                // }
            }
        }
    }

    void handleEncoderMonoSampleModA(int diff)
    {
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        auto &currPattern = XRSequencer::getCurrentSelectedPattern();
        auto &currTrack = XRSequencer::getCurrentSelectedTrack();
        
        auto &monoSampleInstance = XRSound::monoSampleInstances[currSelectedTrackNum];
        auto currentUXMode = XRUX::getCurrentMode();

        switch (currSelectedPageNum)
        {
        case 0:
        {
            int currLastStep = currTrack.lstep;
            int newLastStep = currTrack.lstep + diff;

            // make sure track last step doesn't exceed pattern's

            // TODO: try to re-align current playing track step with pattern step if able
            if (newLastStep < 1)
            {
                newLastStep = 1;
            }
            else if (newLastStep > currPattern.lstep)
            {
                newLastStep = currPattern.lstep;
            }

            if (newLastStep != currLastStep)
            {
                currTrack.lstep = newLastStep;

                XRLED::displayPageLEDs(
                    -1,
                    (XRSequencer::getSeqState().playbackState == XRSequencer::RUNNING),
                    XRSequencer::getCurrentStepPage(),
                    newLastStep);

                XRLED::setDisplayStateForAllStepLEDs();
                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;

        case 1:
        {
            auto currUXMode = XRUX::getCurrentMode();

            if (currUXMode == XRUX::UX_MODE::TRACK_WRITE) {            
                auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
                auto currSoundForTrack = XRSound::activeSounds[currTrackNum];    
                
                if (currSoundForTrack.type == XRSound::SOUND_TYPE::T_MONO_SAMPLE) {
                    XRUX::setCurrentMode(XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK_SOUND);
                    XRSD::setActiveSampleSlot(0); // browsing for first sample slot, based on this encoder
                    XRDisplay::drawSampleBrowser();
                }
            } else if (currUXMode == XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK_SOUND) {
                handleMenuCursor(currUXMode, diff);
            }
        }
        break;

        case 2:
            {
                float currSpeed = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[MSMP_SAMPLEPLAYRATE]);

                // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
                // {
                //     currSpeed = patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].sample_play_rate;
                // }

                float newSpeed = currSpeed + (diff * 0.1);

                if (!(newSpeed < -1.1 || newSpeed > 10.1) && newSpeed != currSpeed)
                {
                    if ((currSpeed > 0.0 && newSpeed < 0.1) || (currSpeed == -1.0 && newSpeed < -1.0))
                    {
                        newSpeed = -1.0;
                    }
                    else if (currSpeed <= -1.0 && newSpeed > -1.0)
                    {
                        newSpeed = 0.1;
                    }

                    // Serial.printf("debug param lock -- currUXMode: %d, selTrack: %d, selStep: %d\n",currUXMode,currSelectedTrackNum,currSelectedStepNum);

                    // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
                    // {
                    //     patternMods.tracks[currSelectedTrackNum].step_mod_flags[currSelectedStepNum].flags[XRSequencer::MOD_ATTRS::SAMPLE_PLAY_RATE] = true;
                    //     patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].sample_play_rate = newSpeed;
                    // }
                    // else
                    // {
                        XRSound::activeSounds[currSelectedTrackNum].params[MSMP_SAMPLEPLAYRATE] = getFloatValuePaddedAsInt32(newSpeed);
                        XRSound::patternSoundsDirty = true;
                    // }

                    AudioNoInterrupts();
                    monoSampleInstance.sample.setPlaybackRate(newSpeed);
                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            
            break;

        case 3:
        {
            int currLoopType = getValueNormalizedAsUInt8(XRSound::activeSounds[currSelectedTrackNum].params[MSMP_LOOPTYPE]);

            // when param locking, use the existing param lock value as the current value
            if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1) {
                if (XRSound::activeSoundModLayer.sounds[currSelectedTrackNum].steps[currSelectedStepNum].flags[XRSound::MSMP_LOOPTYPE]) {
                    currLoopType = getValueNormalizedAsUInt8(
                        XRSound::activeSoundModLayer.sounds[currSelectedTrackNum].steps[currSelectedStepNum].mods[XRSound::MSMP_LOOPTYPE]
                    );
                }
            }

            int newLoopType = constrain(currLoopType + diff, 0, 1);

            if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE) {
                XRSound::activeSoundModLayer.sounds[currSelectedTrackNum].steps[currSelectedStepNum].flags[XRSound::MSMP_LOOPTYPE] = true;
                XRSound::activeSoundModLayer.sounds[currSelectedTrackNum].steps[currSelectedStepNum].mods[XRSound::MSMP_LOOPTYPE] = getInt32ValuePaddedAsInt32(newLoopType);

                XRSound::patternSoundStepModsDirty = true;
            } else {
                XRSound::activeSounds[currSelectedTrackNum].params[MSMP_LOOPTYPE] = getInt32ValuePaddedAsInt32(newLoopType);
            }

            XRSound::patternSoundsDirty = true;

            XRDisplay::drawSequencerScreen(false);
        }
        case 4:
        {
            float currAtt = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[MSMP_AMP_ATTACK]);

            int mult = 1;
            if (abs(diff) > 5)
            {
                mult = 100;
            }
            else if (abs(diff) > 1 && abs(diff) < 5)
            {
                mult = 10;
            }

            float newAtt = currAtt + (diff * mult);

            // real max attack = 11880
            if (!(newAtt < 1 || newAtt > 1000) && newAtt != currAtt)
            {
                XRSound::activeSounds[currSelectedTrackNum].params[MSMP_AMP_ATTACK] = getFloatValuePaddedAsInt32(newAtt);
                XRSound::patternSoundsDirty = true;

                AudioNoInterrupts();

                monoSampleInstance.ampEnv.attack(newAtt);
                
                AudioInterrupts();

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;
        case 5:
        {
            float currLvl = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[MSMP_LEVEL]);
            float newLvl = currLvl + (diff * 0.01);

            if (!(newLvl < 0.0 || newLvl > 1.1) && newLvl != currLvl)
            {
                XRSound::activeSounds[currSelectedTrackNum].params[MSMP_LEVEL] = getFloatValuePaddedAsInt32(newLvl);
                XRSound::patternSoundsDirty = true;

                // if current track is an active choke destination track,
                // don't apply amp settings in real time since it will unchoke the sound
                if (!isTrackAnActiveChokeDestination(currSelectedTrackNum)) {
                    AudioNoInterrupts();

                    monoSampleInstance.amp.gain(newLvl);
                    
                    AudioInterrupts();
                }

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;

        default:
            break;
        }
    }

    void handleEncoderMonoSampleModB(int diff)
    {
        auto &currTrack = XRSequencer::getCurrentSelectedTrack();
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        auto &monoSampleInstance = XRSound::monoSampleInstances[currSelectedTrackNum];

        switch (currSelectedPageNum)
        {
        case 0:
            //
            break;
        case 1:
        {
            auto currUXMode = XRUX::getCurrentMode();

            if (currUXMode == XRUX::UX_MODE::TRACK_WRITE) {            
                auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
                auto currSoundForTrack = XRSound::activeSounds[currTrackNum];    
                
                if (currSoundForTrack.type == XRSound::SOUND_TYPE::T_MONO_SAMPLE) {
                    Serial.println("here 123");
                    XRUX::setCurrentMode(XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK_SOUND);
                    XRSD::setActiveSampleSlot(1); // browsing for first sample slot, based on this encoder
                    XRDisplay::drawSampleBrowser();
                }
            } else if (currUXMode == XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK_SOUND) {
                Serial.println("here 234");
                handleMenuCursor(currUXMode, diff);
            }
        }
        break;
        case 2:
        {}
        break;

        case 3:
        {
            uint32_t currLoopStart = getValueNormalizedAsUInt32(XRSound::activeSounds[currSelectedTrackNum].params[MSMP_LOOPSTART]);

            // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
            // {
            //     currLoopStart = patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].loopstart;
            // }

            int mult = 1;
            if (abs(diff) > 5)
            {
                mult = 100;
            }
            else if (abs(diff) > 1 && abs(diff) < 5)
            {
                mult = 10;
            }

            uint32_t newLoopStart = currLoopStart + (diff * mult);

            if (!(newLoopStart < 0 || newLoopStart > 10000) && newLoopStart != currLoopStart)
            {

                // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
                // {
                //     patternMods.tracks[currSelectedTrackNum].step_mod_flags[currSelectedStepNum].flags[XRSequencer::MOD_ATTRS::LOOPSTART] = true;
                //     patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].loopstart = newLoopStart;
                // }
                // else
                // {
                    XRSound::activeSounds[currSelectedTrackNum].params[MSMP_LOOPSTART] = getUInt32ValuePaddedAsInt32(newLoopStart);
                    XRSound::patternSoundsDirty = true;
                // }

                XRDisplay::drawSequencerScreen(false);
            }
        }
            break;

        case 4:
        {
            float currDecay = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[MSMP_AMP_DECAY]);

            int mult = 1;
            if (abs(diff) > 5)
            {
                mult = 100;
            }
            else if (abs(diff) > 1 && abs(diff) < 5)
            {
                mult = 10;
            }

            float newDecay = currDecay + (diff * mult);

            // real max decay = 11880
            if (!(newDecay < 0 || newDecay > 11880) && newDecay != currDecay)
            {
                XRSound::activeSounds[currSelectedTrackNum].params[MSMP_AMP_DECAY] = getFloatValuePaddedAsInt32(newDecay);
                XRSound::patternSoundsDirty = true;

                AudioNoInterrupts();

                monoSampleInstance.ampEnv.decay(newDecay);

                AudioInterrupts();

                XRDisplay::drawSequencerScreen(false);
            }
        }
            break;

        case 5:
        {
            float currPan = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[MSMP_PAN]);
            float newPan = currPan + (diff * 0.1);

            if (!(newPan < -1.0 || newPan > 1.0) && newPan != currPan)
            {
                XRSound::activeSounds[currSelectedTrackNum].params[MSMP_PAN] = getFloatValuePaddedAsInt32(newPan);
                XRSound::patternSoundsDirty = true;

                // if current track is an active choke destination track,
                // don't apply amp settings in real time since it will unchoke the sound
                if (!isTrackAnActiveChokeDestination(currSelectedTrackNum)) {
                    float newGainL = 1.0;
                    if (newPan < 0)
                    {
                        newGainL += newPan;
                    }

                    float newGainR = 1.0;
                    if (newPan > 0)
                    {
                        newGainR -= newPan;
                    }
                    
                    AudioNoInterrupts();

                    monoSampleInstance.left.gain(newGainR);
                    monoSampleInstance.right.gain(newGainL);

                    AudioInterrupts();
                }

                XRDisplay::drawSequencerScreen(false);
            }
        }
            break;

        default:
            break;
        }
    }

    void handleEncoderMonoSampleModC(int diff)
    {
        auto &currTrack = XRSequencer::getCurrentSelectedTrack();
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto &monoSampleInstance = XRSound::monoSampleInstances[currSelectedTrackNum];
        auto currUXMode = XRUX::getCurrentMode();

        switch (currSelectedPageNum)
        {
        case 0:
            {
                float currVel = currTrack.velocity;
                float newVel = currVel + diff;

                // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1) {
                //     currVel = currStep.velocity;
                //     newVel = currVel + diff;
                // }

                if (!(newVel < 1 || newVel > 100) && newVel != currVel)
                {
                    // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1) {
                    //     patternMods.tracks[currSelectedTrackNum].step_mod_flags[currSelectedStepNum].flags[XRSequencer::MOD_ATTRS::VELOCITY] = true;
                    //     patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].velocity = newVel;
                    // } else {
                        currTrack.velocity = newVel;
                    // }

                    // AudioNoInterrupts();
                    // voices[current_selected_track].leftCtrl.gain(newVel * 0.01);
                    // voices[current_selected_track].rightCtrl.gain(newVel * 0.01);
                    // AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            break;
        case 1:
            {
                
            }
            break;
        case 2:
            {

            }
            break;
        
        case 3:
            {
                uint32_t currLoopFinish = getValueNormalizedAsUInt32(XRSound::activeSounds[currSelectedTrackNum].params[MSMP_LOOPFINISH]);

                // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
                // {
                //     currLoopFinish = patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].loopfinish;
                // }

                // Serial.print("currLoopFinish: ");
                // Serial.println(currLoopFinish);

                int mult = 1;
                if (abs(diff) > 5)
                {
                    mult = 100;
                }
                else if (abs(diff) > 1 && abs(diff) < 5)
                {
                    mult = 10;
                }

                uint32_t newLoopFinish = currLoopFinish + (diff * mult);

                // Serial.print("newLoopFinish: ");
                // Serial.println(newLoopFinish);

                if (!(newLoopFinish < 0 || newLoopFinish > 10000) && newLoopFinish != currLoopFinish)
                {

                    // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
                    // {
                    //     patternMods.tracks[currSelectedTrackNum].step_mod_flags[currSelectedStepNum].flags[XRSequencer::MOD_ATTRS::LOOPFINISH] = true;
                    //     patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].loopfinish = newLoopFinish;
                    // }
                    // else
                    // {
                        XRSound::activeSounds[currSelectedTrackNum].params[MSMP_LOOPFINISH] = getUInt32ValuePaddedAsInt32(newLoopFinish);
                        XRSound::patternSoundsDirty = true;
                    // }

                    XRDisplay::drawSequencerScreen(false);
                }
            }
        break;

        case 4:
            {
                float curSus = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[MSMP_AMP_SUSTAIN]);
                float newSus = curSus + (diff * 0.01);

                if (!(newSus < 0 || newSus > 1.0) && newSus != curSus)
                {
                    XRSound::activeSounds[currSelectedTrackNum].params[MSMP_AMP_SUSTAIN] = getFloatValuePaddedAsInt32(newSus);
                    XRSound::patternSoundsDirty = true;

                    AudioNoInterrupts();

                    monoSampleInstance.ampEnv.sustain(newSus);
                    
                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            break;

        case 5:
        {
            // if this track is already choking and choke tracks are all used up, don't allow more chokes
            if (!chokeSourcesEnabled[currSelectedTrackNum] && getChokeSourcesEnabledCount() == 8) {
                return;
            }

            // if this track is already being choked, don't allow this track to choke other tracks
            if (chokeDestSource[currSelectedTrackNum] > -1) {
                return;
            }

            int currChkDest = getValueNormalizedAsInt8(XRSound::activeSounds[currSelectedTrackNum].params[MSMP_CHOKE]);
            int newChkDest = currChkDest + diff;

            if (newChkDest != currChkDest)
            {
                // make sure choke destination track is never the same as the current track
                if (newChkDest == currSelectedTrackNum && newChkDest < currChkDest) {
                    --newChkDest;
                } else if (newChkDest == currSelectedTrackNum && newChkDest > currChkDest) {
                    ++newChkDest;
                }

                newChkDest = constrain(newChkDest, -1, 15);

                if (chokeSourcesEnabled[currSelectedTrackNum] && newChkDest == -1) {
                    // if this track is a choke source and is being disabled, disable it

                    chokeSourcesEnabled[currSelectedTrackNum] = false;

                    auto currChokeDest = chokeSourceDest[currSelectedTrackNum];
                    chokeSourceDest[currSelectedTrackNum] = -1;
                    chokeDestSource[currChokeDest] = -1;
                } else {
                    chokeSourcesEnabled[currSelectedTrackNum] = true;
                    chokeSourceDest[currSelectedTrackNum] = newChkDest;
                    chokeDestSource[newChkDest] = currSelectedTrackNum;
                }

                XRSound::activeSounds[currSelectedTrackNum].params[MSMP_CHOKE] = getInt32ValuePaddedAsInt32(newChkDest);
                XRSound::patternSoundsDirty = true;

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;
        
        default:
            break;
        }
    }

    void handleEncoderMonoSampleModD(int diff)
    {
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        
        auto &monoSampleInstance = XRSound::monoSampleInstances[currSelectedTrackNum];

        switch (currSelectedPageNum)
        {
        case 0:
            {
                updateTrackProbability(diff);
            }
            break;
        case 1:
            {
                //
            }
            break;
        case 2:
            {

            }
            break;

        case 3:
        {
            play_start currPlaystart = (play_start)getValueNormalizedAsUInt8(XRSound::activeSounds[currSelectedTrackNum].params[MSMP_PLAYSTART]);

            // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
            // {
            //     currPlaystart = patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].playstart;
            // }

            int newPlayStart = (playStartFindMap[currPlaystart]) + diff;

            if (newPlayStart < 0)
            {
                newPlayStart = 1;
            }
            else if (newPlayStart > 1)
            {
                newPlayStart = 0;
            }

            // play_start playStartSel = playStartSelMap[newPlayStart];

            // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
            // {
            //     patternMods.tracks[currSelectedTrackNum].step_mod_flags[currSelectedStepNum].flags[XRSequencer::MOD_ATTRS::PLAYSTART] = true;
            //     patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].playstart = playStartSel;
            // }
            // else
            // {
                XRSound::activeSounds[currSelectedTrackNum].params[MSMP_PLAYSTART] = getInt32ValuePaddedAsInt32(newPlayStart);
                XRSound::patternSoundsDirty = true;
            // }

            XRDisplay::drawSequencerScreen(false);
        }
        break;

        case 4:
        {
            float curRel = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[MSMP_AMP_RELEASE]);

            int mult = 1;
            if (abs(diff) > 5)
            {
                mult = 200;
            }
            else if (abs(diff) > 1 && abs(diff) < 5)
            {
                mult = 20;
            }

            float newRel = curRel + (diff * mult);

            // real max release = 11880
            if (!(newRel < 0 || newRel > 11880) && newRel != curRel)
            {
                XRSound::activeSounds[currSelectedTrackNum].params[MSMP_AMP_RELEASE] = getFloatValuePaddedAsInt32(newRel);
                XRSound::patternSoundsDirty = true;

                AudioNoInterrupts();

                monoSampleInstance.ampEnv.release(newRel);

                AudioInterrupts();

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;

        case 5:
        {
            float currDly = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[MSMP_DELAY]);
            float newDly = currDly + (diff * 0.01);

            if (newDly != currDly)
            {
                newDly = constrain(newDly, 0, 1.0);

                XRSound::activeSounds[currSelectedTrackNum].params[MSMP_DELAY] = getFloatValuePaddedAsInt32(newDly);
                XRSound::patternSoundsDirty = true;

                AudioNoInterrupts();

                monoSampleInstance.ampDelaySend.gain(newDly);

                AudioInterrupts();

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;

        default:
            break;
        }
    }

    void handleEncoderDexedSynthModA(int diff)
    {
        auto &currPattern = XRSequencer::getCurrentSelectedPattern();
        auto &currTrack= XRSequencer::getCurrentSelectedTrack();
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        
        auto &dexedInstance = XRSound::dexedInstances[currSelectedTrackNum];

        switch (currSelectedPageNum)
        {
        case 0:
        {
            int currLastStep = currTrack.lstep;
            int newLastStep = currTrack.lstep + diff;

            // make sure track last step doesn't exceed pattern's

            // TODO: try to re-align current playing track step with pattern step if able
            if (newLastStep < 1)
            {
                newLastStep = 1;
            }
            else if (newLastStep > currPattern.lstep)
            {
                newLastStep = currPattern.lstep;
            }

            if (newLastStep != currLastStep)
            {
                currTrack.lstep = newLastStep;

                XRLED::displayPageLEDs(
                    -1,
                    (XRSequencer::getSeqState().playbackState == XRSequencer::RUNNING),
                    XRSequencer::getCurrentStepPage(),
                    newLastStep);

                XRLED::setDisplayStateForAllStepLEDs();
                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;
        case 1:
        {
            auto currTranspose = getValueNormalizedAsInt32(activeSounds[currSelectedTrackNum].params[DEXE_TRANSPOSE]);
            auto newTranspose = constrain(currTranspose + diff, 0, 48);

            if (newTranspose != currTranspose)
            {
                XRSound::activeSounds[currSelectedTrackNum].params[DEXE_TRANSPOSE] = getInt32ValuePaddedAsInt32(newTranspose);
                XRSound::patternSoundsDirty = true;

                AudioNoInterrupts();

                dexedInstance.dexed.setTranspose(newTranspose);

                AudioInterrupts();

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;
        case 2:
        {
            //
        }
        break;
        case 3:
        {
            int currNoteMode = getValueNormalizedAsInt8(XRSound::activeSounds[currSelectedTrackNum].params[DEXE_NOTE_MODE]);
            int newNoteMode = currNoteMode + diff;
            
            if (newNoteMode != currNoteMode) {
                if (newNoteMode > 1) {
                    newNoteMode = 1;
                } else if (newNoteMode < 0) {
                    newNoteMode = 0;
                }

                if (newNoteMode == currNoteMode) {
                    return;
                }

                XRSound::activeSounds[currSelectedTrackNum].params[DEXE_NOTE_MODE] = getInt32ValuePaddedAsInt32(newNoteMode);
                XRSound::patternSoundsDirty = true;

                AudioNoInterrupts();

                dexedInstance.dexed.setMonoMode(!(bool)newNoteMode); // since 0 = mono and 1 = poly

                AudioInterrupts();

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;

        case 4:
        {
            float currLvl = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[DEXE_LEVEL]);
            float newLvl = currLvl + (diff * 0.01);

            if (!(newLvl < 0.0 || newLvl > 1.1) && newLvl != currLvl)
            {
                XRSound::activeSounds[currSelectedTrackNum].params[DEXE_LEVEL] = getFloatValuePaddedAsInt32(newLvl);
                XRSound::patternSoundsDirty = true;

                AudioNoInterrupts();

                dexedInstance.amp.gain(newLvl);

                AudioInterrupts();

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;

        default:
            break;
        }
    }

    void handleEncoderDexedSynthModB(int diff)
    {
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto currSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currentUXMode = XRUX::getCurrentMode();
        
        auto &dexedInstance = XRSound::dexedInstances[currSelectedTrackNum];

        switch (currSelectedPageNum)
        {
        case 0:
            updateTrackLength(diff);
            
            break;

        case 1:
        {
            auto currAlgo = getValueNormalizedAsInt32(activeSounds[currSelectedTrackNum].params[DEXE_ALGO]);
            auto newAlgo = constrain(currAlgo + diff, 0, 31);

            if (newAlgo != currAlgo)
            {
                XRSound::activeSounds[currSelectedTrackNum].params[DEXE_ALGO] = getInt32ValuePaddedAsInt32(newAlgo);
                XRSound::patternSoundsDirty = true;

                AudioNoInterrupts();

                dexedInstance.dexed.setAlgorithm(newAlgo);

                AudioInterrupts();

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;

        case 2:
            /* code */
            break;

        case 3:
        {
            // poly mode note B assign
            int currNoteB = getValueNormalizedAsInt8(XRSound::activeSounds[currSelectedTrackNum].params[DEXE_NOTE_B]);

            // when param locking, use the existing param lock value as the current value
            if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1) {
                if (XRSound::activeSoundModLayer.sounds[currSelectedTrackNum].steps[currSelectedStepNum].flags[XRSound::DEXE_NOTE_B]) {
                    currNoteB = getValueNormalizedAsUInt8(
                        XRSound::activeSoundModLayer.sounds[currSelectedTrackNum].steps[currSelectedStepNum].mods[XRSound::DEXE_NOTE_B]
                    );
                }
            }

            int newNoteB = currNoteB + diff;

            if (newNoteB != currNoteB)
            {
                if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE) {
                    XRSound::activeSoundModLayer.sounds[currSelectedTrackNum].steps[currSelectedStepNum].flags[XRSound::DEXE_NOTE_B] = true;
                    XRSound::activeSoundModLayer.sounds[currSelectedTrackNum].steps[currSelectedStepNum].mods[XRSound::DEXE_NOTE_B] = getInt32ValuePaddedAsInt32(newNoteB);
                    XRSound::patternSoundStepModsDirty = true;
                } else {
                    XRSound::activeSounds[currSelectedTrackNum].params[DEXE_NOTE_B] = getInt32ValuePaddedAsInt32(newNoteB);
                }
                XRSound::patternSoundsDirty = true;

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;

        case 4:
        {
            float currPan = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[DEXE_PAN]);
            float newPan = currPan + (diff * 0.1);

            if (newPan != currPan)
            {
                newPan = constrain(newPan, -1.0, 1.0);

                XRSound::activeSounds[currSelectedTrackNum].params[DEXE_PAN] = getFloatValuePaddedAsInt32(newPan);
                XRSound::patternSoundsDirty = true;

                float newGainL = 1.0;
                if (newPan < 0)
                {
                    newGainL += newPan;
                }

                float newGainR = 1.0;
                if (newPan > 0)
                {
                    newGainR -= newPan;
                }
                
                AudioNoInterrupts();

                dexedInstance.left.gain(newGainR);
                dexedInstance.right.gain(newGainL);

                AudioInterrupts();

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;

        default:
            break;
        }
    }

    void handleEncoderDexedSynthModC(int diff)
    {
        auto &currTrack = XRSequencer::getCurrentSelectedTrack();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currentUXMode = XRUX::getCurrentMode();
        
        //auto &dexedInstance = XRSound::dexedInstances[currTrackNum];
        
        switch (currSelectedPageNum)
        {
        case 0:
        {
            float currVel = currTrack.velocity;
            float newVel = currVel + diff;

            // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1) {
            //     currVel = currStep.velocity;
            //     newVel = currVel + diff;
            // }

            if (!(newVel < 1 || newVel > 100) && newVel != currVel)
            {
                // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1) {
                //     patternMods.tracks[currSelectedTrackNum].step_mod_flags[currSelectedStepNum].flags[XRSequencer::MOD_ATTRS::VELOCITY] = true;
                //     patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].velocity = newVel;
                // } else {
                    currTrack.velocity = newVel;
                // }

                // AudioNoInterrupts();
                // voices[current_selected_track].leftCtrl.gain(newVel * 0.01);
                // voices[current_selected_track].rightCtrl.gain(newVel * 0.01);
                // AudioInterrupts();

                XRDisplay::drawSequencerScreen(false);
            }
        }
            
            break;

        case 1:
            /* code */
            break;

        case 2:
            /* code */
            break;

        case 3:
        {
            // poly mode note C assign
            int currNoteC = getValueNormalizedAsInt8(XRSound::activeSounds[currSelectedTrackNum].params[DEXE_NOTE_C]);

            // when param locking, use the existing param lock value as the current value
            if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1) {
                if (XRSound::activeSoundModLayer.sounds[currSelectedTrackNum].steps[currSelectedStepNum].flags[XRSound::DEXE_NOTE_C]) {
                    currNoteC = getValueNormalizedAsUInt8(
                        XRSound::activeSoundModLayer.sounds[currSelectedTrackNum].steps[currSelectedStepNum].mods[XRSound::DEXE_NOTE_C]
                    );
                }
            }

            int newNoteC = currNoteC + diff;

            if (newNoteC != currNoteC)
            {
                if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE) {
                    XRSound::activeSoundModLayer.sounds[currSelectedTrackNum].steps[currSelectedStepNum].flags[XRSound::DEXE_NOTE_C] = true;
                    XRSound::activeSoundModLayer.sounds[currSelectedTrackNum].steps[currSelectedStepNum].mods[XRSound::DEXE_NOTE_C] = getInt32ValuePaddedAsInt32(newNoteC);
                    XRSound::patternSoundStepModsDirty = true;
                } else {
                    XRSound::activeSounds[currSelectedTrackNum].params[DEXE_NOTE_C] = getInt32ValuePaddedAsInt32(newNoteC);
                }
                XRSound::patternSoundsDirty = true;

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;

        case 4:
            //
            break;

        default:
            break;
        }
    }

    void handleEncoderDexedSynthModD(int diff)
    {
        auto &currTrack = XRSequencer::getCurrentSelectedTrack();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currentUXMode = XRUX::getCurrentMode();
        
        auto &dexedInstance = XRSound::dexedInstances[currSelectedTrackNum];
        
        switch (currSelectedPageNum)
        {
        case 0:
            updateTrackProbability(diff);

            break;

        case 1:
            /* code */
            break;

        case 2:
            /* code */
            break;

        case 3:
        {
            // poly mode note D assign
            int currNoteD = getValueNormalizedAsInt8(XRSound::activeSounds[currSelectedTrackNum].params[DEXE_NOTE_D]);

            // when param locking, use the existing param lock value as the current value
            if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1) {
                if (XRSound::activeSoundModLayer.sounds[currSelectedTrackNum].steps[currSelectedStepNum].flags[XRSound::DEXE_NOTE_D]) {
                    currNoteD = getValueNormalizedAsUInt8(
                        XRSound::activeSoundModLayer.sounds[currSelectedTrackNum].steps[currSelectedStepNum].mods[XRSound::DEXE_NOTE_D]
                    );
                }
            }

            int newNoteD = currNoteD + diff;

            if (newNoteD != currNoteD)
            {
                if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE) {
                    XRSound::activeSoundModLayer.sounds[currSelectedTrackNum].steps[currSelectedStepNum].flags[XRSound::DEXE_NOTE_D] = true;
                    XRSound::activeSoundModLayer.sounds[currSelectedTrackNum].steps[currSelectedStepNum].mods[XRSound::DEXE_NOTE_D] = getInt32ValuePaddedAsInt32(newNoteD);
                    XRSound::patternSoundStepModsDirty = true;
                } else {
                    XRSound::activeSounds[currSelectedTrackNum].params[DEXE_NOTE_D] = getInt32ValuePaddedAsInt32(newNoteD);
                }
                XRSound::patternSoundsDirty = true;

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;

        case 4:
        {
            float currDly = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[DEXE_DELAY]);
            float newDly = currDly + (diff * 0.01);

            if (newDly != currDly)
            {
                newDly = constrain(newDly, 0, 1.0);

                XRSound::activeSounds[currSelectedTrackNum].params[DEXE_DELAY] = getFloatValuePaddedAsInt32(newDly);
                XRSound::patternSoundsDirty = true;

                AudioNoInterrupts();

                dexedInstance.ampDelaySend.gain(newDly);

                AudioInterrupts();

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;

        default:
            break;
        }
    }

    void handleEncoderFmDrumModA(int diff)
    {
        if (diff == 0) {
            return;
        }

        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto &trackToUse = XRSequencer::getTrack(currTrackNum);
        auto &fmDrumInstance = XRSound::fmDrumInstances[currTrackNum];

        switch (currSelectedPageNum)
        {
        case 0:
            updateTrackLastStep(diff);
            break;
        case 1:
        {
            uint16_t currFreq = getValueNormalizedAsUInt32(XRSound::activeSounds[currTrackNum].params[FMD_FREQ]);
            uint16_t newFreq = currFreq + diff;

            if (newFreq != currFreq) {
                newFreq = constrain(newFreq, 0, 255);

                XRSound::activeSounds[currTrackNum].params[FMD_FREQ] = getUInt32ValuePaddedAsInt32(newFreq);
                XRSound::patternSoundsDirty = true;

                fmDrumInstance.fmDrum.frequency(newFreq);
            }

            XRDisplay::drawSequencerScreen(false);
        }
        break;
        case 2:
        {
            float currLvl = getValueNormalizedAsFloat(XRSound::activeSounds[currTrackNum].params[FMD_LEVEL]);
            float newLvl = currLvl + (diff * 0.01);

            if (newLvl != currLvl)
            {
                newLvl = constrain(newLvl, 0, 1.0);

                XRSound::activeSounds[currTrackNum].params[FMD_LEVEL] = getFloatValuePaddedAsInt32(newLvl);
                XRSound::patternSoundsDirty = true;

                AudioNoInterrupts();

                fmDrumInstance.amp.gain(newLvl);

                AudioInterrupts();

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;

        default:
            break;
        }
    }

    void handleEncoderFmDrumModB(int diff)
    {
        if (diff == 0) {
            return;
        }

        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto &fmDrumInstance = XRSound::fmDrumInstances[currTrackNum];

        switch (currSelectedPageNum)
        {
        case 0:
            //

            break;
        case 1:
            {
                float currFm = getValueNormalizedAsFloat(XRSound::activeSounds[currTrackNum].params[FMD_FM]);
                float newFm = currFm + (diff * 0.01);

                if (newFm != currFm) {
                    newFm = constrain(newFm, 0.0, 1.0);

                    XRSound::activeSounds[currTrackNum].params[FMD_FM] = getFloatValuePaddedAsInt32(newFm);
                    XRSound::patternSoundsDirty = true;

                    AudioNoInterrupts();

                    fmDrumInstance.fmDrum.fm(newFm);

                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;

        case 2:
        {
            float currPan = getValueNormalizedAsFloat(XRSound::activeSounds[currTrackNum].params[FMD_PAN]);
            float newPan = currPan + (diff * 0.1);

            if (!(newPan < -1.0 || newPan > 1.0) && newPan != currPan)
            {
                XRSound::activeSounds[currTrackNum].params[FMD_PAN] = getFloatValuePaddedAsInt32(newPan);
                XRSound::patternSoundsDirty = true;

                float newGainL = 1.0;
                if (newPan < 0)
                {
                    newGainL += newPan;
                }

                float newGainR = 1.0;
                if (newPan > 0)
                {
                    newGainR -= newPan;
                }
                
                AudioNoInterrupts();

                fmDrumInstance.left.gain(newGainR);
                fmDrumInstance.right.gain(newGainL);

                AudioInterrupts();

                XRDisplay::drawSequencerScreen(false);
            }
        }

        break;

        default:
            break;
        }
    }

    void handleEncoderFmDrumModC(int diff)
    {
        if (diff == 0) {
            return;
        }

        auto &currTrack = XRSequencer::getCurrentSelectedTrack();
        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto &fmDrumInstance = XRSound::fmDrumInstances[currTrackNum];

        switch (currSelectedPageNum)
        {
        case 0:
            {
                float currVel = currTrack.velocity;
                float newVel = currVel + diff;

                // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1) {
                //     currVel = currStep.velocity;
                //     newVel = currVel + diff;
                // }

                if (!(newVel < 1 || newVel > 100) && newVel != currVel)
                {
                    // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1) {
                    //     patternMods.tracks[currSelectedTrackNum].step_mod_flags[currSelectedStepNum].flags[XRSequencer::MOD_ATTRS::VELOCITY] = true;
                    //     patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].velocity = newVel;
                    // } else {
                        currTrack.velocity = newVel;
                    // }

                    // AudioNoInterrupts();
                    // voices[current_selected_track].leftCtrl.gain(newVel * 0.01);
                    // voices[current_selected_track].rightCtrl.gain(newVel * 0.01);
                    // AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            break;
        case 1:
            {
                auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
                auto &fmDrumInstance = XRSound::fmDrumInstances[currTrackNum];

                float currDecay = getValueNormalizedAsFloat(XRSound::activeSounds[currTrackNum].params[FMD_DECAY]);
                float newDecay  = currDecay + (diff * 0.01);

                if (newDecay != currDecay) {
                    newDecay = constrain(newDecay, 0.0, 1.0);

                    XRSound::activeSounds[currTrackNum].params[FMD_DECAY] = getFloatValuePaddedAsInt32(newDecay);
                    XRSound::patternSoundsDirty = true;

                    AudioNoInterrupts();

                    fmDrumInstance.fmDrum.decay(newDecay);

                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            break;
        case 2:
            {

            }
            break;
        
        default:
            break;
        }
    }

    void handleEncoderFmDrumModD(int diff)
    {
        if (diff == 0) {
            return;
        }

        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto &fmDrumInstance = XRSound::fmDrumInstances[currTrackNum];

        switch (currSelectedPageNum)
        {
        case 0:
            updateTrackProbability(diff);

            break;
        case 1:
            {
                float currNoise = getValueNormalizedAsFloat(XRSound::activeSounds[currTrackNum].params[FMD_NOISE]);
                float newNoise = currNoise + (diff * 0.01);

                if (newNoise != currNoise) {
                    newNoise = constrain(newNoise, 0.0, 1.0);

                    XRSound::activeSounds[currTrackNum].params[FMD_NOISE] = getFloatValuePaddedAsInt32(newNoise);
                    XRSound::patternSoundsDirty = true;

                    AudioNoInterrupts();

                    fmDrumInstance.fmDrum.noise(newNoise);

                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            break;
        case 2:
        {
            float currDly = getValueNormalizedAsFloat(XRSound::activeSounds[currTrackNum].params[FMD_DELAY]);
            float newDly = currDly + (diff * 0.01);

            if (newDly != currDly)
            {
                newDly = constrain(newDly, 0, 1.0);

                XRSound::activeSounds[currTrackNum].params[FMD_DELAY] = getFloatValuePaddedAsInt32(newDly);
                XRSound::patternSoundsDirty = true;

                AudioNoInterrupts();

                fmDrumInstance.ampDelaySend.gain(newDly);

                AudioInterrupts();

                XRDisplay::drawSequencerScreen(false);
            }
        }
            break;
        
        default:
            break;
        }
    }

    void handleEncoderMonoSynthModA(int diff)
    {
        switch (XRSequencer::getCurrentSelectedPage())
        {
        case 0:
            updateTrackLastStep(diff);
            break;
        case 1:
            updateMonoSynthWaveform(diff);
            break;
        case 2:
            updateMonoSynthNoiseAmt(diff);
            break;
        case 3:
            updateMonoSynthFilterEnvAttack(diff);
            break;
        case 4:
            updateTrackAmpEnvAttack(diff);
            break;
        case 5:
            updateComboTrackLevel(diff);
            break;
        default:
            break;
        }
    }

    void handleEncoderMonoSynthModB(int diff)
    {
        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedPage = XRSequencer::getCurrentSelectedPage();
        
        auto &monoSynthInstance = XRSound::monoSynthInstances[currTrackNum];

        switch (currSelectedPage)
        {
        case 0:
            updateTrackLength(diff);

            break;
        case 1:
            {
                float currDetune = getValueNormalizedAsFloat(XRSound::activeSounds[currTrackNum].params[MSYN_DETUNE]);
                float newDetune = currDetune + diff;

                Serial.printf(
                    "raw detune: %d, normal detune: %f, new detune: %f\n",
                    XRSound::activeSounds[currTrackNum].params[MSYN_DETUNE],
                    currDetune,
                    newDetune
                );

                if (!(newDetune < -24 || newDetune > 24) && newDetune != currDetune) {
                    // uint8_t noteToUse = currTrack.note;

                    // if (numNotesHeldOfKeyboard != 0) {
                    //     noteToUse = noteOnKeyboard;
                    // }

                    XRSound::activeSounds[currTrackNum].params[MSYN_DETUNE] = getFloatValuePaddedAsInt32(newDetune);
                    XRSound::patternSoundsDirty = true;

                    Serial.printf(
                        "new raw detune: %d\n",
                        XRSound::activeSounds[currTrackNum].params[MSYN_DETUNE]
                    );

                    // auto oscFreqB = getDetunedOscFreqB(noteToUse, newDetune);

                    // TODO: allow adjust detune in realtime
                    // auto &seqState = XRSequencer::getSeqState();
                    // if (seqState.playbackState != XRSequencer::RUNNING) {
                    //     AudioNoInterrupts();
                    //     comboVoice.oscb.frequency(oscFreqB);
                    //     AudioInterrupts();
                    // }

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;
        case 2:
            {
                float currCutoff = getValueNormalizedAsFloat(XRSound::activeSounds[currTrackNum].params[MSYN_CUTOFF]);

                int mult = 1;
                if (abs(diff) > 5) {
                    mult = 200;
                } else if (abs(diff) > 1 && abs(diff) < 5) {
                    mult = 10;
                }

                float newCutoff = currCutoff + (diff * mult);

                if (!(newCutoff < 1 || newCutoff > 3000) && newCutoff != currCutoff) {
                    XRSound::activeSounds[currTrackNum].params[MSYN_CUTOFF] = getFloatValuePaddedAsInt32(newCutoff);
                    XRSound::patternSoundsDirty = true;

                    AudioNoInterrupts();

                    monoSynthInstance.filter.frequency(newCutoff);

                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }   
            }

            break;
        case 3:
            {
                float currDecay = getValueNormalizedAsFloat(XRSound::activeSounds[currTrackNum].params[MSYN_FILTER_DECAY]);

                int mult = 1;
                if (abs(diff) > 5) {
                    mult = 100;
                } else if (abs(diff) > 1 && abs(diff) < 5) {
                    mult = 10;
                }

                float newDecay = currDecay + (diff * mult);

                if (!(newDecay < 0 || newDecay > 11880) && newDecay != currDecay)
                {
                    XRSound::activeSounds[currTrackNum].params[MSYN_FILTER_DECAY] = getFloatValuePaddedAsInt32(newDecay);
                    XRSound::patternSoundsDirty = true;

                    AudioNoInterrupts();

                    monoSynthInstance.filterEnv.decay(newDecay);

                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            
            break;

        case 4:
            {
                float currDecay = getValueNormalizedAsFloat(XRSound::activeSounds[currTrackNum].params[MSYN_AMP_DECAY]);

                int mult = 1;
                if (abs(diff) > 5) {
                    mult = 100;
                } else if (abs(diff) > 1 && abs(diff) < 5) {
                    mult = 10;
                }

                float newDecay = currDecay + (diff * mult);

                // real max decay = 11880
                if (!(newDecay < 0 || newDecay > 11880) && newDecay != currDecay) {
                    XRSound::activeSounds[currTrackNum].params[MSYN_AMP_DECAY] = getFloatValuePaddedAsInt32(newDecay);
                    XRSound::patternSoundsDirty = true;

                    AudioNoInterrupts();

                    monoSynthInstance.ampEnv.decay(newDecay);

                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            
            break;
        
        case 5:
            {
                float currPan = getValueNormalizedAsFloat(XRSound::activeSounds[currTrackNum].params[MSYN_PAN]);
                float newPan = currPan + (diff * 0.1);

                if (!(newPan < -1.0 || newPan > 1.0) && newPan != currPan) {
                    XRSound::activeSounds[currTrackNum].params[MSYN_PAN] = getFloatValuePaddedAsInt32(newPan);
                    XRSound::patternSoundsDirty = true;

                    float newGainL = 1.0;
                    if (newPan < 0) {
                        newGainL += newPan;
                    }

                    float newGainR = 1.0;
                    if (newPan > 0) {
                        newGainR -= newPan;
                    }

                    AudioNoInterrupts();

                    monoSynthInstance.left.gain(newGainR);
                    monoSynthInstance.right.gain(newGainL);

                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;

        default:
            break;
        }
    }

    void handleEncoderMonoSynthModC(int diff)
    {
        auto &currTrack = XRSequencer::getCurrentSelectedTrack();
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        
        auto &monoSynthInstance = XRSound::monoSynthInstances[currSelectedTrackNum];

        switch (currSelectedPageNum)
        {
        case 0:
            {
                float currVel = currTrack.velocity;
                float newVel = currVel + diff;

                // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1) {
                //     currVel = currStep.velocity;
                //     newVel = currVel + diff;
                // }

                if (!(newVel < 1 || newVel > 100) && newVel != currVel)
                {
                    // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1) {
                    //     patternMods.tracks[currSelectedTrackNum].step_mod_flags[currSelectedStepNum].flags[XRSequencer::MOD_ATTRS::VELOCITY] = true;
                    //     patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].velocity = newVel;
                    // } else {
                        currTrack.velocity = newVel;
                    // }

                    // AudioNoInterrupts();
                    // voices[current_selected_track].leftCtrl.gain(newVel * 0.01);
                    // voices[current_selected_track].rightCtrl.gain(newVel * 0.01);
                    // AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;
        case 1:
            {
                float currFine = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[MSYN_FINE]);
                float newFine = currFine + diff;

                if (!(newFine < -50.0 || newFine > 50.0) && newFine != currFine)
                {
                    XRSound::activeSounds[currSelectedTrackNum].params[MSYN_FINE] = getFloatValuePaddedAsInt32(newFine);
                    XRSound::patternSoundsDirty = true;

                    // TODO: also use step note
                    // uint8_t noteToUse = currTrack.note;
                    // if (numNotesHeldOfKeyboard != 0) {
                    //     noteToUse = noteOnKeyboard;
                    // } else  {
                    //     // note_to_use = currStep.note;
                    // }

                    // TODO: allow adjust fine freq in realtime?
                    // auto &seqState = XRSequencer::getSeqState();
                    // if (seqState.playbackState != XRSequencer::RUNNING) {
                    //     float oscFreqA = getOscFreqA(noteToUse, newFine);

                    //     AudioNoInterrupts();
                    //     comboVoice.osca.frequency(oscFreqA);
                    //     AudioInterrupts();
                    // }

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;
        case 2:
            {
                float currRes = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[MSYN_RESONANCE]);

                int mult = 1;
                if (abs(diff) > 5) {
                    mult = 10;
                } else if (abs(diff) > 1 && abs(diff) < 5) {
                    mult = 5;
                }

                float newRes = currRes + (diff * mult * 0.01);

                if (!(newRes < -0.01 || newRes > 1.9) && newRes != currRes) {
                    XRSound::activeSounds[currSelectedTrackNum].params[MSYN_RESONANCE] = getFloatValuePaddedAsInt32(newRes);
                    XRSound::patternSoundsDirty = true;

                    AudioNoInterrupts();

                    monoSynthInstance.filter.resonance(newRes);

                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;
        case 3:
            {
                float curSus = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[MSYN_FILTER_SUSTAIN]);
                float newSus = curSus + (diff * 0.01);

                if (!(newSus < 0 || newSus > 1.0) && newSus != curSus)
                {
                    XRSound::activeSounds[currSelectedTrackNum].params[MSYN_FILTER_SUSTAIN] = getFloatValuePaddedAsInt32(newSus);
                    XRSound::patternSoundsDirty = true;

                    AudioNoInterrupts();

                    monoSynthInstance.filterEnv.sustain(newSus);

                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;
        case 4:
            {
                float curSus = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[MSYN_AMP_SUSTAIN]);
                float newSus = curSus + (diff * 0.01);

                if (!(newSus < 0 || newSus > 1.0) && newSus != curSus)
                {
                    XRSound::activeSounds[currSelectedTrackNum].params[MSYN_AMP_SUSTAIN] = getFloatValuePaddedAsInt32(newSus);
                    XRSound::patternSoundsDirty = true;

                    AudioNoInterrupts();

                    monoSynthInstance.ampEnv.sustain(newSus);

                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;
        case 5:
            {
                // TODO: choke?
            }

            break;
        
        default:
            break;
        }
    }

    void handleEncoderMonoSynthModD(int diff)
    {
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        
        auto &monoSynthInstance = XRSound::monoSynthInstances[currSelectedTrackNum];

        switch (currSelectedPageNum)
        {
        case 0:
            updateTrackProbability(diff);

            break;

        case 1: 
            {
                float currWidth = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[MSYN_WIDTH]);
                float newWidth = currWidth + (diff * 0.01);

                if (!(newWidth < 0.01 || newWidth > 1.0) && newWidth != currWidth)
                {
                    XRSound::activeSounds[currSelectedTrackNum].params[MSYN_WIDTH] = getFloatValuePaddedAsInt32(newWidth);
                    XRSound::patternSoundsDirty = true;

                    AudioNoInterrupts();

                    monoSynthInstance.oscA.pulseWidth(newWidth);
                    monoSynthInstance.oscB.pulseWidth(newWidth);

                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            } 

            break;

        case 2:
            {
                float currFilterAmt = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[MSYN_FILTER_ENV_AMT]);

                int mult = 1;
                if (abs(diff) > 5)
                {
                    mult = 150;
                }
                else if (abs(diff) > 1 && abs(diff) < 5)
                {
                    mult = 10;
                }

                float newFilterAmt = currFilterAmt + (diff * mult * 0.005);

                if (!(newFilterAmt < -1.0 || newFilterAmt > 1.0) && newFilterAmt != currFilterAmt)
                {
                    XRSound::activeSounds[currSelectedTrackNum].params[MSYN_FILTER_ENV_AMT] = getFloatValuePaddedAsInt32(newFilterAmt);
                    XRSound::patternSoundsDirty = true;

                    AudioNoInterrupts();

                    monoSynthInstance.dc.amplitude(newFilterAmt);

                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;

        case 3:
            {
                float curRel = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[MSYN_FILTER_RELEASE]);

                int mult = 1;
                if (abs(diff) > 5) {
                    mult = 200;
                } else if (abs(diff) > 1 && abs(diff) < 5) {
                    mult = 20;
                }

                float newRel = curRel + (diff * mult);

                if (!(newRel < 0 || newRel > 11880) && newRel != curRel) {
                    XRSound::activeSounds[currSelectedTrackNum].params[MSYN_FILTER_RELEASE] = getFloatValuePaddedAsInt32(newRel);
                    XRSound::patternSoundsDirty = true;

                    AudioNoInterrupts();

                    monoSynthInstance.filterEnv.release(newRel);

                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;

        case 4:
            {
                float curRel = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[MSYN_AMP_RELEASE]);

                int mult = 1;
                if (abs(diff) > 5) {
                    mult = 200;
                } else if (abs(diff) > 1 && abs(diff) < 5) {
                    mult = 20;
                }

                float newRel = curRel + (diff * mult);

                // real max release = 11880
                if (!(newRel < 0 || newRel > 11880) && newRel != curRel) {
                    XRSound::activeSounds[currSelectedTrackNum].params[MSYN_AMP_RELEASE] = getFloatValuePaddedAsInt32(newRel);
                    XRSound::patternSoundsDirty = true;

                    AudioNoInterrupts();

                    monoSynthInstance.ampEnv.release(newRel);

                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;

        case 5:
            {
                float currDly = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrackNum].params[MSYN_DELAY]);
                float newDly = currDly + (diff * 0.01);

                if (newDly != currDly)
                {
                    newDly = constrain(newDly, 0, 1.0);

                    XRSound::activeSounds[currSelectedTrackNum].params[MSYN_DELAY] = getFloatValuePaddedAsInt32(newDly);
                    XRSound::patternSoundsDirty = true;

                    AudioNoInterrupts();

                    monoSynthInstance.ampDelaySend.gain(newDly);

                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            break;
        
        default:
            break;
        }
    }

    void updateMonoSynthWaveform(int diff)
    {
        auto currSelectedTrack = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedStep = XRSequencer::getCurrentSelectedStepNum();
        auto currentUXMode = XRUX::getCurrentMode();
        
        auto &monoSynthInstance = XRSound::monoSynthInstances[currSelectedTrack];

        int currWaveform = getWaveformNumber(
            getValueNormalizedAsUInt8(XRSound::activeSounds[currSelectedTrack].params[MSYN_WAVE])
        );

        // when param locking, use the existing param lock value as the current value
        if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStep > -1) {
            if (XRSound::activeSoundModLayer.sounds[currSelectedTrack].steps[currSelectedStep].flags[XRSound::MSYN_WAVE]) {
                currWaveform = getWaveformNumber(
                    getValueNormalizedAsUInt8(
                        XRSound::activeSoundModLayer.sounds[currSelectedTrack].steps[currSelectedStep].mods[XRSound::MSYN_WAVE]
                    )
                );
            }
        }

        int newWaveform = currWaveform + diff;

        if (newWaveform < 0) {
            newWaveform = 5;
        }  else if (newWaveform > 5) {
            newWaveform = 0;
        }

        if (newWaveform != currWaveform) {
            int waveformSel = getWaveformTypeSelection(newWaveform);

            if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE) {
                XRSound::activeSoundModLayer.sounds[currSelectedTrack].steps[currSelectedStep].flags[XRSound::MSYN_WAVE] = true;
                XRSound::activeSoundModLayer.sounds[currSelectedTrack].steps[currSelectedStep].mods[XRSound::MSYN_WAVE] = getInt32ValuePaddedAsInt32(waveformSel);
                XRSound::patternSoundStepModsDirty = true;
            } else {
                XRSound::activeSounds[currSelectedTrack].params[MSYN_WAVE] = getInt32ValuePaddedAsInt32(waveformSel);
            }
            XRSound::patternSoundsDirty = true;

            monoSynthInstance.oscA.begin(waveformSel);
            monoSynthInstance.oscB.begin(waveformSel);

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void updateMonoSynthNoiseAmt(int diff)
    {
        auto currSelectedTrack = XRSequencer::getCurrentSelectedTrackNum();
        auto &monoSynthInstance = XRSound::monoSynthInstances[currSelectedTrack];

        float currNoise = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrack].params[MSYN_NOISE]);
        float newNoise = currNoise + (diff * 0.05);

        if (!(newNoise < 0.01 || newNoise > 1.0) && newNoise != currNoise)
        {
            XRSound::activeSounds[currSelectedTrack].params[MSYN_NOISE] = getFloatValuePaddedAsInt32(newNoise);
            XRSound::patternSoundsDirty = true;

            AudioNoInterrupts();

            monoSynthInstance.noise.amplitude(newNoise);

            AudioInterrupts();

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void updateMonoSynthFilterEnvAttack(int diff)
    {
        auto currSelectedTrack = XRSequencer::getCurrentSelectedTrackNum();
        auto &monoSynthInstance = XRSound::monoSynthInstances[currSelectedTrack];

        float currAtt = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrack].params[MSYN_FILTER_ATTACK]);

        int mult = 1;
        if (abs(diff) > 5)
        {
            mult = 200;
        }
        else if (abs(diff) > 1 && abs(diff) < 5)
        {
            mult = 20;
        }

        float newAtt = currAtt + (diff * mult);

        // real max attack = 11880
        if (!(newAtt < 0 || newAtt > 1000) && newAtt != currAtt)
        {
            XRSound::activeSounds[currSelectedTrack].params[MSYN_FILTER_ATTACK] = getFloatValuePaddedAsInt32(newAtt);
            XRSound::patternSoundsDirty = true;

            AudioNoInterrupts();

            monoSynthInstance.filterEnv.attack(newAtt);

            AudioInterrupts();

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void updateTrackAmpEnvAttack(int diff)
    {
        auto currSelectedTrack = XRSequencer::getCurrentSelectedTrackNum();
        auto &monoSynthInstance = XRSound::monoSynthInstances[currSelectedTrack];

        float currAtt = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrack].params[MSYN_AMP_ATTACK]);

        int mult = 1;
        if (abs(diff) > 5)
        {
            mult = 100;
        }
        else if (abs(diff) > 1 && abs(diff) < 5)
        {
            mult = 10;
        }

        float newAtt = currAtt + (diff * mult);

        if (!(newAtt < 1 || newAtt > 500) && newAtt != currAtt)
        {
            XRSound::activeSounds[currSelectedTrack].params[MSYN_AMP_ATTACK] = getFloatValuePaddedAsInt32(newAtt);
            XRSound::patternSoundsDirty = true;

            AudioNoInterrupts();

            monoSynthInstance.ampEnv.attack(newAtt);

            AudioInterrupts();

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void updateComboTrackLevel(int diff)
    {
        auto currSelectedTrack = XRSequencer::getCurrentSelectedTrackNum();
        auto &monoSynthInstance = XRSound::monoSynthInstances[currSelectedTrack];

        float currLvl = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrack].params[MSYN_LEVEL]);

        if (XRSound::activeSounds[currSelectedTrack].type == T_MONO_SAMPLE) {
            currLvl = getValueNormalizedAsFloat(XRSound::activeSounds[currSelectedTrack].params[MSMP_LEVEL]);
        }

        float newLvl = currLvl + (diff * 0.01);

        if (newLvl != currLvl)
        {
            newLvl = constrain(newLvl, 0, 1.0);
            
            if (XRSound::activeSounds[currSelectedTrack].type == T_MONO_SAMPLE) {
                XRSound::activeSounds[currSelectedTrack].params[MSMP_LEVEL] = getFloatValuePaddedAsInt32(newLvl);
            } else {
                XRSound::activeSounds[currSelectedTrack].params[MSYN_LEVEL] = getFloatValuePaddedAsInt32(newLvl);
            }

            XRSound::patternSoundsDirty = true;

            // if current track is an active choke destination track,
            // don't apply amp settings in real time since it will unchoke the sound
            if (!isTrackAnActiveChokeDestination(currSelectedTrack)) {
                AudioNoInterrupts();

                monoSynthInstance.amp.gain(newLvl);

                AudioInterrupts();
            }

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void updateTrackLength(int diff)
    {
        auto &currTrack = XRSequencer::getCurrentSelectedTrack();
        auto &currLayer = XRSequencer::getCurrentSelectedTrackLayer();
        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currUXMode = XRUX::getCurrentMode();

        int currLen = currTrack.length;

        if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currStepNum > -1) {
            if (currLayer.tracks[currTrackNum].steps[currStepNum].flags[XRSequencer::LENGTH]) {
                currLen = currLayer.tracks[currTrackNum].steps[currStepNum].mods[XRSequencer::LENGTH];
            }
        }

        int newLen = currLen + diff;

        if (!(newLen < 0 && newLen > 64) && (newLen != currLen)) {
            if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currStepNum > -1) {
                currLayer.tracks[currTrackNum].steps[currStepNum].flags[XRSequencer::LENGTH] = true;
                currLayer.tracks[currTrackNum].steps[currStepNum].mods[XRSequencer::LENGTH] = newLen;
            } else {
                currTrack.length = newLen;
            }

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void updateTrackLastStep(int diff)
    {
        auto &currTrack = XRSequencer::getCurrentSelectedTrack();

        int currLastStep = currTrack.lstep;
        int newLastStep = currTrack.lstep + diff;

        // make sure track last step doesn't exceed pattern's
        auto &currPattern = XRSequencer::getCurrentSelectedPattern();

        // TODO: try to re-align current playing track step with pattern step if able

        if (newLastStep < 1) {
            newLastStep = 1;
        } else if (newLastStep > currPattern.lstep) {
            newLastStep = currPattern.lstep;
        }

        if (newLastStep != currLastStep) {
            currTrack.lstep = newLastStep;

            XRLED::displayPageLEDs(
                -1,
                (XRSequencer::getSeqState().playbackState == XRSequencer::RUNNING),
                XRSequencer::getCurrentStepPage(),
                newLastStep
            );
            
            XRLED::setDisplayStateForAllStepLEDs();

            XRDisplay::drawSequencerScreen(false);
        }
    }
    
    void handleDexedBrowserMod1()
    {
        int diff = getDiff(MOD1_ENCODER_ADDRESS);

        if (diff != 0) {
            int dif = constrain(XRSD::dexedCurrentPool + diff, 0, 1);
            if (dif == XRSD::dexedCurrentPool)
            {
                return;
            }

            XRSD::dexedCurrentPool = constrain(XRSD::dexedCurrentPool + diff, 0, 1);
            XRSD::dexedCurrentBank = 0; // always reset bank to first when changing pools
            XRSD::dexedCurrentPatch = 0; // always reset patch to first when changing pools

            XRSD::loadDexedVoiceToCurrentTrack();
            XRDisplay::drawDexedSysexBrowser();
        }
    }
    void handleDexedBrowserMod2()
    {
        int diff = getDiff(MOD2_ENCODER_ADDRESS);

        if (diff != 0) {
            int dif = constrain(XRSD::dexedCurrentBank + diff, 0, 98);
            if (dif == XRSD::dexedCurrentBank)
            {
                return;
            }

            XRSD::dexedCurrentBank = constrain(XRSD::dexedCurrentBank + diff, 0, 98);
            XRSD::dexedCurrentPatch = 0; // always reset patch to first when changing banks

            XRSD::loadDexedVoiceToCurrentTrack();
            XRDisplay::drawDexedSysexBrowser();
        }
    }
    void handleDexedBrowserMod3()
    {
        int diff = getDiff(MOD3_ENCODER_ADDRESS);

        if (diff != 0) {
            int dif = constrain(XRSD::dexedCurrentPatch + diff, 0, 31);
            if (dif == XRSD::dexedCurrentPatch)
            {
                return;
            }

            XRSD::dexedCurrentPatch = constrain(XRSD::dexedCurrentPatch + diff, 0, 31);

            XRSD::loadDexedVoiceToCurrentTrack();
            XRDisplay::drawDexedSysexBrowser();
        }
    }

    void updateTrackProbability(int diff)
    {
        auto &currTrack = XRSequencer::getCurrentSelectedTrack();
        auto &currLayer = XRSequencer::getCurrentSelectedTrackLayer();
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currUXMode = XRUX::getCurrentMode();

        uint8_t currProb = currTrack.probability;

        if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1) {
            if (currLayer.tracks[currSelectedTrackNum].steps[currSelectedStepNum].flags[XRSequencer::PROBABILITY]) {
                currProb = currLayer.tracks[currSelectedTrackNum].steps[currSelectedStepNum].mods[XRSequencer::PROBABILITY];
            }
        }

        uint8_t newProb = constrain(currProb + diff, 0, 100);

        if (newProb != currProb)
        {
            if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1) {
                currLayer.tracks[currSelectedTrackNum].steps[currSelectedStepNum].flags[XRSequencer::PROBABILITY] = true;
                currLayer.tracks[currSelectedTrackNum].steps[currSelectedStepNum].mods[XRSequencer::PROBABILITY] = newProb;
            } else {
                currTrack.probability = newProb;
            }

            XRDisplay::drawSequencerScreen(false);
        }
    }

}