#include <XREncoder.h>
#include <XRUX.h>
#include <XRMenu.h>
#include <XRDisplay.h>
#include <XRClock.h>
#include <XRSD.h>
#include <XRSequencer.h>
#include <XRLED.h>
#include <XRVersa.h>

namespace XREncoder
{
    elapsedMillis elapsedMs;

    float _tempFm = 0;
    int _tempFreq = 50;
    float _tempDecay = 0.75;
    float _tempNoise = 0;
    float _tempOverDrive = 0;

    int addresses[5] = {
        0x36, 0x37, 0x38, 0x39, 0x40
    };

    int16_t currentValues[5] = {
        0, 0, 0, 0, 0
    };

    int16_t lastValues[5] = {
        0, 0, 0, 0, 0
    };

    void handleEncoderSetTempo();
    void handleEncoderSetPatternMods();
    void handleEncoderSetTrackMods();
    void handleEncoderTraversePages(int diff);
    void handleEncoderPatternModA(int diff);
    void handleEncoderPatternModB(int diff);
    void handleEncoderPatternModC(int diff);
    void handleEncoderPatternModD(int diff);
    void handleEncoderSubtractiveSynthModA(int diff);
    void handleEncoderSubtractiveSynthModB(int diff);
    void handleEncoderSubtractiveSynthModC(int diff);
    void handleEncoderSubtractiveSynthModD(int diff);
    void handleEncoderDexedModA(int diff);
    void handleEncoderDexedModB(int diff);
    void handleEncoderDexedModC(int diff);
    void handleEncoderDexedModD(int diff);
    void handleEncoderFmDrumModA(int diff);
    void handleEncoderFmDrumModB(int diff);
    void handleEncoderFmDrumModC(int diff);
    void handleEncoderFmDrumModD(int diff);
    void handleEncoderRawSampleModA(int diff);
    void handleEncoderRawSampleModB(int diff);
    void handleEncoderRawSampleModC(int diff);
    void handleEncoderRawSampleModD(int diff);
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
    void updateSubtractiveSynthWaveform(int diff);
    void updateSubtractiveSynthNoiseAmt(int diff);
    void updateSubtractiveSynthFilterEnvAttack(int diff);
    void updateTrackAmpEnvAttack(int diff);
    void updateComboTrackLevel(int diff);

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

        bool uxModesWithMenu = (
            currentUXMode == XRUX::UX_MODE::SOUND_MENU_MAIN || 
            currentUXMode == XRUX::UX_MODE::CHANGE_SETUP  || 
            currentUXMode == XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK
        );

        if (!(elapsedMs % 25) && uxModesWithMenu) {
            if (currentUXMode == XRUX::UX_MODE::SOUND_MENU_MAIN) {
                if (handleMenuCursor(SOUND_MENU_ITEM_MAX)) {
                    XRDisplay::drawSoundMenuMain();
                }
            } else if (currentUXMode == XRUX::UX_MODE::CHANGE_SETUP) {
                if (handleMenuCursor(SETUP_MENU_ITEM_MAX)) {
                    XRDisplay::drawSetupMenu();
                }
            } else if (currentUXMode == XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK) {
                if (handleMenuCursor(255)) {
                    XRDisplay::drawSampleBrowser();
                }
            }
            
            return;
        }

        if (!(elapsedMs % 25) && currentUXMode == XRUX::UX_MODE::SET_TEMPO) {
            handleEncoderSetTempo();

            return;
        }

        if (!(elapsedMs % 25) && currentUXMode == XRUX::UX_MODE::PATTERN_WRITE) {
            handleEncoderSetPatternMods();

            return;
        }

        if (!(elapsedMs % 25) && (currentUXMode == XRUX::UX_MODE::TRACK_WRITE || currentUXMode == XRUX::UX_MODE::SUBMITTING_STEP_VALUE))
        {
            int diff = getDiff(MAIN_ENCODER_ADDRESS);

            if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE) {
                handleEncoderTraversePages(diff);
            }

            handleEncoderSetTrackMods();

            return;
        }
    }

    void handleEncoderSetTempo()
    {
        int diff = getDiff(MAIN_ENCODER_ADDRESS);

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

                    XRDisplay::drawSetTempoOverlay();
                }
            }
        }
    }

    bool handleMenuCursor(int menuItems)
    {
        int diff = getDiff(MAIN_ENCODER_ADDRESS);

        if (menuItems > 2 && diff != 0)
        {
            uint8_t currPos = XRMenu::getCursorPosition();
            uint8_t newPos = constrain(currPos + diff, 0, menuItems - 1);

            XRMenu::setCursorPosition(newPos);

            return true;
        }

        return false;
    }

    void handleEncoderTraversePages(int diff)
    {
        auto tPage = XRSequencer::getCurrentSelectedPage();

        int newPage = tPage + diff;

        if (newPage != tPage) {
            int maxPagesForCurrTrack = XRSequencer::getCurrentTrackPageCount();

            if (maxPagesForCurrTrack == 1) {
                return;
            }
            if (newPage > (maxPagesForCurrTrack - 1)) {
                newPage = 0;
            }
            else if (newPage < 0) {
                newPage = maxPagesForCurrTrack - 1;
            }

            XRSequencer::setSelectedPage(newPage);
            
            XRDisplay::drawSequencerScreen(false);
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
        auto &currPattern = XRSequencer::getHeapCurrentSelectedPattern();

        int currLastStep = currPattern.last_step;
        int newLastStep = currPattern.last_step + diff;

        if (newLastStep < 1) {
            newLastStep = 1;
        } else if (newLastStep > MAXIMUM_SEQUENCER_STEPS) {
            newLastStep = MAXIMUM_SEQUENCER_STEPS;
        }

        if (newLastStep != currLastStep)
        {
            currPattern.last_step = newLastStep;

            for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
            {
                // set track's last_step to match pattern if track last_step is greater than pattern's
                if (currPattern.tracks[t].last_step > newLastStep)
                {
                    currPattern.tracks[t].last_step = newLastStep;
                }
            }

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void handleEncoderPatternModB(int diff)
    {
        auto &currPattern = XRSequencer::getHeapCurrentSelectedPattern();

        int currGrooveId = currPattern.groove_id;
        int newGrooveId = currPattern.groove_id + diff;

        if (newGrooveId < -1)  {
            newGrooveId = -1;
        }  else if (newGrooveId > MAXIMUM_GROOVE_CONFIGS - 1) {
            newGrooveId = MAXIMUM_GROOVE_CONFIGS - 1;
        }

        if (newGrooveId != currGrooveId)  {
            currPattern.groove_id = newGrooveId;

            auto shuffleTemplate = XRClock::getShuffleTemplateForGroove(currPattern.groove_id, currPattern.groove_amount);

            if (newGrooveId == -1) {
                XRClock::setShuffle(false);
            } else if (!uClock.isShuffled()) {
                XRClock::setShuffleTemplate(shuffleTemplate);
                XRClock::setShuffle(true);
            }

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void handleEncoderPatternModC(int diff)
    {
        auto &currPattern = XRSequencer::getHeapCurrentSelectedPattern();

        int currGrooveAmt = currPattern.groove_amount;
        int newGrooveAmt = currPattern.groove_amount + diff;

        if (newGrooveAmt < 0) {
            newGrooveAmt = 0;
        } else if (newGrooveAmt > MAXIMUM_GROOVE_OPTIONS - 1) {
            newGrooveAmt = MAXIMUM_GROOVE_OPTIONS - 1;
        }

        if (newGrooveAmt != currGrooveAmt)
        {
            currPattern.groove_amount = newGrooveAmt;

            XRClock::setShuffleTemplateForGroove(currPattern.groove_id, newGrooveAmt);

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void handleEncoderPatternModD(int diff)
    {
        Serial.println("TODO: impl patter mod D");
    }

    void handleEncoderSetTrackMods()
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();

        for (int m = 1; m < 5; m++) // starting from 1 since 1-4 are the mod encoders, 0 is main
        {
            int mDiff = getDiff(m);

            if (mDiff == 0) continue;

            // Serial.printf("m: %d, mDiff: %d\n", m, mDiff);

            if (currTrack.track_type == XRSequencer::SUBTRACTIVE_SYNTH)
            {
                if (m == 1) {
                    handleEncoderSubtractiveSynthModA(mDiff);
                } else if (m == 2) {
                    handleEncoderSubtractiveSynthModB(mDiff);
                } else if (m == 3) {
                    handleEncoderSubtractiveSynthModC(mDiff);
                } else if (m == 4) {
                    handleEncoderSubtractiveSynthModD(mDiff);
                }
            }
            else if (currTrack.track_type == XRSequencer::DEXED)
            {
                if (m == 1) {
                    handleEncoderDexedModA(mDiff);
                } else if (m == 2) {
                    handleEncoderDexedModB(mDiff);
                } else if (m == 3) {
                    handleEncoderDexedModC(mDiff);
                } else if (m == 4) {
                    handleEncoderDexedModD(mDiff);
                }
            }
            else if (currTrack.track_type == XRSequencer::FM_DRUM)
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
            else if (currTrack.track_type == XRSequencer::RAW_SAMPLE)
            {
                if (m == 1) {
                    handleEncoderRawSampleModA(mDiff);
                } else if (m == 2) {
                    handleEncoderRawSampleModB(mDiff);
                } else if (m == 3) {
                    handleEncoderRawSampleModC(mDiff);
                } else if (m == 4) {
                    handleEncoderRawSampleModD(mDiff);
                }
            }
            else if (currTrack.track_type == XRSequencer::WAV_SAMPLE)
            {
                // if (m == 1) {
                //     handleEncoderWavSampleModA(mDiff);
                // } else if (m == 2) {
                //     handleEncoderWavSampleModB(mDiff);
                // } else if (m == 3) {
                //     handleEncoderWavSampleModC(mDiff);
                // } else if (m == 4) {
                //     handleEncoderWavSampleModD(mDiff);
                // }
            }
            else if (currTrack.track_type == XRSequencer::CV_GATE)
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

    void handleEncoderRawSampleModA(int diff)
    {
        auto &currPattern = XRSequencer::getHeapCurrentSelectedPattern();
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &currStep = XRSequencer::getHeapCurrentSelectedTrackStep();
        auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto numNotesHeldOfKeyboard = XRVersa::getKeyboardNotesHeld();
        auto noteOnKeyboard = XRVersa::getNoteOnKeyboard();
        auto currUXMode = XRUX::getCurrentMode();

        switch (currSelectedPageNum)
        {
        case 0:
        {
            int currLastStep = currTrack.last_step;
            int newLastStep = currTrack.last_step + diff;

            // make sure track last step doesn't exceed pattern's

            // TODO: try to re-align current playing track step with pattern step if able
            if (newLastStep < 1)
            {
                newLastStep = 1;
            }
            else if (newLastStep > currPattern.last_step)
            {
                newLastStep = currPattern.last_step;
            }

            if (newLastStep != currLastStep)
            {
                currTrack.last_step = newLastStep;

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
            int currLoopType = currTrack.looptype;

            if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
            {
                currLoopType = patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].looptype;
            }

            int newLoopType = (currLoopType + diff);

            if (newLoopType < 0)
            {
                newLoopType = 1;
            }
            else if (newLoopType > 1)
            {
                newLoopType = 0;
            }

            if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
            {
                patternMods.tracks[currSelectedTrackNum].step_mod_flags[currSelectedStepNum].flags[XRSequencer::MOD_ATTRS::LOOPTYPE] = true;
                patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].looptype = newLoopType;
            }
            else
            {
                currTrack.looptype = newLoopType;
            }

            XRDisplay::drawSequencerScreen(false);
        }
        break;
        case 2:
        {
            float currAtt = currTrack.amp_attack;

            int mult = 1;
            if (abs(diff) > 5)
            {
                mult = 100;
            }
            else if (abs(diff) > 1 && abs(diff) < 5)
            {
                mult = 10;
            }

            float newAtt = currTrack.amp_attack + (diff * mult);

            // real max attack = 11880
            if (!(newAtt < 1 || newAtt > 1000) && newAtt != currAtt)
            {
                currTrack.amp_attack = newAtt;

                if (currSelectedTrackNum > 3)
                {
                    AudioNoInterrupts();
                    auto &sampleVoice = XRSound::getSampleVoiceForTrack(currSelectedTrackNum - 4);

                    sampleVoice.ampEnv.attack(newAtt);
                    AudioInterrupts();
                }
                else
                {
                    AudioNoInterrupts();
                    auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();

                    comboVoice.ampEnv.attack(newAtt);
                    AudioInterrupts();
                }

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;
        case 3:
        {
            float currLvl = currTrack.level;
            float newLvl = currTrack.level + (diff * 0.01);

            if (!(newLvl < 0.0 || newLvl > 1.1) && newLvl != currLvl)
            {
                currTrack.level = newLvl;

                // Serial.print("currSelectedTrackNum: ");
                // Serial.print(currSelectedTrackNum);
                // Serial.print(" newLvl: ");
                // Serial.println(newLvl);

                if (currSelectedTrackNum > 3)
                {
                    AudioNoInterrupts();
                    auto &sampleVoice = XRSound::getSampleVoiceForTrack(currSelectedTrackNum - 4);

                    sampleVoice.leftSubMix.gain(0, newLvl);
                    sampleVoice.leftSubMix.gain(1, newLvl);
                    sampleVoice.rightSubMix.gain(0, newLvl);
                    sampleVoice.rightSubMix.gain(1, newLvl);
                    AudioInterrupts();
                }
                else
                {
                    AudioNoInterrupts();
                    auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();

                    comboVoice.leftSubMix.gain(0, newLvl);
                    comboVoice.leftSubMix.gain(1, newLvl);
                    comboVoice.rightSubMix.gain(0, newLvl);
                    comboVoice.rightSubMix.gain(1, newLvl);
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

    void handleEncoderRawSampleModB(int diff)
    {
        auto &currPattern = XRSequencer::getHeapCurrentSelectedPattern();
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &currStep = XRSequencer::getHeapCurrentSelectedTrackStep();
        auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        //auto numNotesHeldOfKeyboard = XRVersa::getKeyboardNotesHeld();
        //auto noteOnKeyboard = XRVersa::getNoteOnKeyboard();
        auto currUXMode = XRUX::getCurrentMode();

        switch (currSelectedPageNum)
        {
        case 0:
            /* code */
            break;

        case 1:
        {
            uint32_t currLoopStart = currTrack.loopstart;

            if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
            {
                currLoopStart = patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].loopstart;
            }

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

                if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
                {
                    patternMods.tracks[currSelectedTrackNum].step_mod_flags[currSelectedStepNum].flags[XRSequencer::MOD_ATTRS::LOOPSTART] = true;
                    patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].loopstart = newLoopStart;
                }
                else
                {
                    currTrack.loopstart = newLoopStart;
                }

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;

        case 2:
        {
            float currDecay = currTrack.amp_decay;

            int mult = 1;
            if (abs(diff) > 5)
            {
                mult = 100;
            }
            else if (abs(diff) > 1 && abs(diff) < 5)
            {
                mult = 10;
            }

            float newDecay = currTrack.amp_decay + (diff * mult);

            // real max decay = 11880
            if (!(newDecay < 0 || newDecay > 500) && newDecay != currDecay)
            {
                currTrack.amp_decay = newDecay;

                if (currSelectedTrackNum > 3)
                {
                    auto &sampleVoice = XRSound::getSampleVoiceForTrack(currSelectedTrackNum - 4);
                    AudioNoInterrupts();
                    sampleVoice.ampEnv.decay(newDecay);
                    AudioInterrupts();
                }
                else
                {
                    AudioNoInterrupts();
                    comboVoice.ampEnv.decay(newDecay);
                    AudioInterrupts();
                }

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;

        case 3:
        {
            float currPan = currTrack.pan;
            float newPan = currTrack.pan + (diff * 0.1);

            if (!(newPan < -1.0 || newPan > 1.0) && newPan != currPan)
            {
                currTrack.pan = newPan;

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

                if (currSelectedTrackNum > 3)
                {
                    auto &sampleVoice = XRSound::getSampleVoiceForTrack(currSelectedTrackNum - 4);
                    AudioNoInterrupts();
                    // sampleVoice.leftCtrl.gain(getStereoPanValues(newPan).left * (currTrack.velocity * 0.01));
                    // sampleVoice.leftCtrl.gain(getStereoPanValues(newPan).right * (currTrack.velocity * 0.01));
                    sampleVoice.leftCtrl.gain(newGainR);
                    sampleVoice.rightCtrl.gain(newGainL);
                    AudioInterrupts();
                }
                else
                {
                    AudioNoInterrupts();
                    // comboVoice.leftCtrl.gain(getStereoPanValues(newPan).left * (currTrack.velocity * 0.01));
                    // comboVoice.leftCtrl.gain(getStereoPanValues(newPan).right * (currTrack.velocity * 0.01));
                    comboVoice.leftCtrl.gain(newGainR);
                    comboVoice.rightCtrl.gain(newGainL);
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

    void handleEncoderRawSampleModC(int diff)
    {
        auto &currPattern = XRSequencer::getHeapCurrentSelectedPattern();
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &currStep = XRSequencer::getHeapCurrentSelectedTrackStep();
        auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        //auto numNotesHeldOfKeyboard = XRVersa::getKeyboardNotesHeld();
        //auto noteOnKeyboard = XRVersa::getNoteOnKeyboard();
        auto currUXMode = XRUX::getCurrentMode();

        switch (currSelectedPageNum)
        {
        case 0:
            {
                // sample speed adj
                float currSpeed = currTrack.sample_play_rate;

                if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
                {
                    currSpeed = patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].sample_play_rate;
                }

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

                    Serial.printf("debug param lock -- currUXMode: %d, selTrack: %d, selStep: %d\n",currUXMode,currSelectedTrackNum,currSelectedStepNum);

                    if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
                    {
                        patternMods.tracks[currSelectedTrackNum].step_mod_flags[currSelectedStepNum].flags[XRSequencer::MOD_ATTRS::SAMPLE_PLAY_RATE] = true;
                        patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].sample_play_rate = newSpeed;
                    }
                    else
                    {
                        currTrack.sample_play_rate = newSpeed;
                    }

                    if (currSelectedTrackNum > 3)
                    {
                        auto &sampleVoice = XRSound::getSampleVoiceForTrack(currSelectedTrackNum - 4);
                        AudioNoInterrupts();
                        sampleVoice.rSample.setPlaybackRate(newSpeed);
                        AudioInterrupts();
                    }
                    else
                    {
                        AudioNoInterrupts();
                        comboVoice.rSample.setPlaybackRate(newSpeed);
                        AudioInterrupts();
                    }

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            break;
        case 1:
            {
                uint32_t currLoopFinish = currTrack.loopfinish;

                if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
                {
                    currLoopFinish = patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].loopfinish;
                }

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

                    if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
                    {
                        patternMods.tracks[currSelectedTrackNum].step_mod_flags[currSelectedStepNum].flags[XRSequencer::MOD_ATTRS::LOOPFINISH] = true;
                        patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].loopfinish = newLoopFinish;
                    }
                    else
                    {
                        currTrack.loopfinish = newLoopFinish;
                    }

                    XRDisplay::drawSequencerScreen(false);
                }
            }
        break;

        case 2:
            {
                float curSus = currTrack.amp_sustain;
                float newSus = currTrack.amp_sustain + (diff * 0.01);

                if (!(newSus < 0 || newSus > 1.0) && newSus != curSus)
                {
                    currTrack.amp_sustain = newSus;

                    if (currSelectedTrackNum > 3)
                    {
                        auto &sampleVoice = XRSound::getSampleVoiceForTrack(currSelectedTrackNum - 4);
                        AudioNoInterrupts();
                        sampleVoice.ampEnv.sustain(newSus);
                        AudioInterrupts();
                    }
                    else
                    {
                        AudioNoInterrupts();
                        comboVoice.ampEnv.sustain(newSus);
                        AudioInterrupts();
                    }

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            break;
        case 3:
            {
                
            }
            break;
        
        default:
            break;
        }
    }

    void handleEncoderRawSampleModD(int diff)
    {
        auto &currPattern = XRSequencer::getHeapCurrentSelectedPattern();
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &currStep = XRSequencer::getHeapCurrentSelectedTrackStep();
        auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        // auto numNotesHeldOfKeyboard = XRVersa::getKeyboardNotesHeld();
        // auto noteOnKeyboard = XRVersa::getNoteOnKeyboard();
        auto currUXMode = XRUX::getCurrentMode();

        switch (currSelectedPageNum)
        {
        case 0:
        {
            uint8_t currBitrate = currTrack.bitrate;
            uint8_t newBitrate = currBitrate + diff;

            if (!(newBitrate < 1 || newBitrate > 16) && newBitrate != currBitrate)
            {
                currTrack.bitrate = newBitrate;

                if (currSelectedTrackNum > 3)
                {
                    AudioNoInterrupts();

                    // sampleVoices[current_selected_track].leftBitCrush.bits(newBitrate);
                    // sampleVoices[current_selected_track].rightBitCrush.bits(newBitrate);

                    AudioInterrupts();
                }
                else
                {
                    AudioNoInterrupts();

                    // comboVoices[current_selected_track].leftBitCrush.bits(newBitrate);
                    // comboVoices[current_selected_track].rightBitCrush.bits(newBitrate);

                    AudioInterrupts();
                }

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;
        case 1:
        {
            play_start currPlaystart = currTrack.playstart;

            if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
            {
                currPlaystart = patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].playstart;
            }

            int newPlayStart = (XRSound::playStartFindMap[currPlaystart]) + diff;

            if (newPlayStart < 0)
            {
                newPlayStart = 1;
            }
            else if (newPlayStart > 1)
            {
                newPlayStart = 0;
            }

            play_start playStartSel = XRSound::playStartSelMap[newPlayStart];

            if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
            {
                patternMods.tracks[currSelectedTrackNum].step_mod_flags[currSelectedStepNum].flags[XRSequencer::MOD_ATTRS::PLAYSTART] = true;
                patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].playstart = playStartSel;
            }
            else
            {
                currTrack.playstart = playStartSel;
            }

            XRDisplay::drawSequencerScreen(false);
        }
        break;
        case 2:
        {
            float curRel = currTrack.amp_release;

            int mult = 1;
            if (abs(diff) > 5)
            {
                mult = 200;
            }
            else if (abs(diff) > 1 && abs(diff) < 5)
            {
                mult = 20;
            }

            float newRel = currTrack.amp_release + (diff * mult);

            // real max release = 11880
            if (!(newRel < 0 || newRel > 11880) && newRel != curRel)
            {
                currTrack.amp_release = newRel;

                if (currSelectedTrackNum > 3)
                {
                    auto &sampleVoice = XRSound::getSampleVoiceForTrack(currSelectedTrackNum - 4);
                    AudioNoInterrupts();
                    sampleVoice.ampEnv.release(newRel);
                    AudioInterrupts();
                }
                else
                {
                    AudioNoInterrupts();
                    comboVoice.ampEnv.release(newRel);
                    AudioInterrupts();
                }

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;
        case 3:
        {
            //
        }
        break;

        default:
            break;
        }
    }

    void handleEncoderDexedModA(int diff)
    {
        auto &currPattern = XRSequencer::getHeapCurrentSelectedPattern();
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &currStep = XRSequencer::getHeapCurrentSelectedTrackStep();
        auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto numNotesHeldOfKeyboard = XRVersa::getKeyboardNotesHeld();
        auto noteOnKeyboard = XRVersa::getNoteOnKeyboard();
        auto currUXMode = XRUX::getCurrentMode();

        switch (currSelectedPageNum)
        {
        case 0:
        {
            int currLastStep = currTrack.last_step;
            int newLastStep = currTrack.last_step + diff;

            // make sure track last step doesn't exceed pattern's

            // TODO: try to re-align current playing track step with pattern step if able
            if (newLastStep < 1)
            {
                newLastStep = 1;
            }
            else if (newLastStep > currPattern.last_step)
            {
                newLastStep = currPattern.last_step;
            }

            if (newLastStep != currLastStep)
            {
                currTrack.last_step = newLastStep;

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
            //
        }
        break;
        case 2:
        {
            //
        }
        break;
        case 3:
        {
            float currLvl = currTrack.level;
            float newLvl = currTrack.level + (diff * 0.01);

            if (!(newLvl < 0.0 || newLvl > 1.1) && newLvl != currLvl)
            {
                currTrack.level = newLvl;

                AudioNoInterrupts();
                auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();

                //comboVoice.leftSubMix.gain(0, newLvl);
                comboVoice.leftSubMix.gain(1, newLvl);
                //comboVoice.rightSubMix.gain(0, newLvl);
                comboVoice.rightSubMix.gain(1, newLvl);
                AudioInterrupts();

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;

        default:
            break;
        }
    }

    void handleEncoderDexedModB(int diff)
    {
        auto &currPattern = XRSequencer::getHeapCurrentSelectedPattern();
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &currStep = XRSequencer::getHeapCurrentSelectedTrackStep();
        auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto numNotesHeldOfKeyboard = XRVersa::getKeyboardNotesHeld();
        auto noteOnKeyboard = XRVersa::getNoteOnKeyboard();
        auto currUXMode = XRUX::getCurrentMode();

        switch (currSelectedPageNum)
        {
        case 0:
            updateTrackLength(diff);
            
            break;

        case 1:
            /* code */
            break;

        case 2:
            /* code */
            break;

        case 3:
        {
            float currPan = currTrack.pan;
            float newPan = currTrack.pan + (diff * 0.1);

            if (!(newPan < -1.0 || newPan > 1.0) && newPan != currPan)
            {
                currTrack.pan = newPan;

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
                comboVoice.dexedLeftCtrl.gain(newGainR);
                comboVoice.dexedRightCtrl.gain(newGainL);
                AudioInterrupts();

                XRDisplay::drawSequencerScreen(false);
            }
        }

        break;

        default:
            break;
        }
    }

    void handleEncoderDexedModC(int diff)
    {
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        if (currSelectedPageNum == 0) {
            int dif = XRSD::dexedCurrentBank + diff;
            if (dif == XRSD::dexedCurrentBank) {
                return;
            }

            XRSD::dexedCurrentBank = XRSD::dexedCurrentBank + diff;

            if (XRSD::dexedCurrentBank < 0)
            {
                XRSD::dexedCurrentBank = 98;
            }
            else if (XRSD::dexedCurrentBank > 98)
            {
                XRSD::dexedCurrentBank = 0;
            }

            Serial.print("dexed_current_bank: ");
            Serial.print(XRSD::dexedCurrentBank);

            Serial.print(" dexed_current_patch: ");
            Serial.println(XRSD::dexedCurrentPatch);

            XRSD::loadDexedVoiceToCurrentTrack();
            XRDisplay::drawSequencerScreen(false);
        }
    }

    void handleEncoderDexedModD(int diff)
    {
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        if (currSelectedPageNum == 0) {
            int dif = XRSD::dexedCurrentPatch + diff;
            if (dif == XRSD::dexedCurrentPatch) {
                return;
            }

            XRSD::dexedCurrentPatch = XRSD::dexedCurrentPatch + diff;

            if (XRSD::dexedCurrentPatch < 0)
            {
                XRSD::dexedCurrentPatch = 31;
            }
            else if (XRSD::dexedCurrentPatch > 31)
            {
                XRSD::dexedCurrentPatch = 0;
            }

            Serial.print("dexed_current_bank: ");
            Serial.print(XRSD::dexedCurrentBank);

            Serial.print(" dexed_current_patch: ");
            Serial.println(XRSD::dexedCurrentPatch);

            XRSD::loadDexedVoiceToCurrentTrack();
            XRDisplay::drawSequencerScreen(false);
        }
    }


    void handleEncoderFmDrumModA(int diff)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto currUXMode = XRUX::getCurrentMode();

        switch (currSelectedPageNum)
        {
        case 0:
        {
            _tempNoise = _tempNoise + (diff * 0.01);

            comboVoice.fmdrum.noise(_tempNoise);
        }
        break;
        case 1:
        {
            float currLvl = currTrack.level;
            float newLvl = currTrack.level + (diff * 0.01);

            if (!(newLvl < 0.0 || newLvl > 1.1) && newLvl != currLvl)
            {
                currTrack.level = newLvl;

                AudioNoInterrupts();
                auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();

                //comboVoice.leftSubMix.gain(0, newLvl);
                comboVoice.leftSubMix.gain(2, newLvl);
                //comboVoice.rightSubMix.gain(0, newLvl);
                comboVoice.rightSubMix.gain(2, newLvl);
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

        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        switch (currSelectedPageNum)
        {
        case 0:
            _tempDecay = _tempDecay + (diff * 0.01);

            comboVoice.fmdrum.decay(_tempDecay);

            XRDisplay::drawSequencerScreen(false);

            break;

        case 1:
        {
            float currPan = currTrack.pan;
            float newPan = currTrack.pan + (diff * 0.1);

            if (!(newPan < -1.0 || newPan > 1.0) && newPan != currPan)
            {
                currTrack.pan = newPan;

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
                comboVoice.fmDrumLeftCtrl.gain(newGainR);
                comboVoice.fmDrumRightCtrl.gain(newGainL);
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

        auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        if (currSelectedPageNum == 0) 
        {
            _tempFm = _tempFm + (diff * 0.01);

            comboVoice.fmdrum.fm(_tempFm);

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void handleEncoderFmDrumModD(int diff)
    {
        if (diff == 0) {
            return;
        }

        auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        if (currSelectedPageNum == 0) {
            _tempFreq = _tempFreq + diff;

            comboVoice.fmdrum.frequency(_tempFreq);

            XRDisplay::drawSequencerScreen(false);
        }
    }



    void handleEncoderSubtractiveSynthModA(int diff)
    {
        switch (XRSequencer::getCurrentSelectedPage())
        {
        case 0:
            updateTrackLastStep(diff);
            break;
        case 1:
            updateSubtractiveSynthWaveform(diff);
            break;
        case 2:
            updateSubtractiveSynthNoiseAmt(diff);
            break;
        case 3:
            updateSubtractiveSynthFilterEnvAttack(diff);
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

    void handleEncoderSubtractiveSynthModB(int diff)
    {
        //auto &seqState = XRSequencer::getSeqState();
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();
        auto currSelectedPage = XRSequencer::getCurrentSelectedPage();
        auto numNotesHeldOfKeyboard = XRVersa::getKeyboardNotesHeld();
        auto noteOnKeyboard = XRVersa::getNoteOnKeyboard();

        switch (currSelectedPage)
        {
        case 0:
            updateTrackLength(diff);

            break;
        case 1:
            {
                float currDetune = currTrack.detune;
                float newDetune = currDetune + diff;

                if (!(newDetune < -24 || newDetune > 24) && newDetune != currDetune) {
                    uint8_t noteToUse = currTrack.note;

                    if (numNotesHeldOfKeyboard != 0) {
                        noteToUse = noteOnKeyboard;
                    }

                    currTrack.detune = newDetune;

                    auto oscFreqB = XRSound::getDetunedOscFreqB(noteToUse, newDetune);

                    // TODO: allow adjust detune in realtime
                    auto &seqState = XRSequencer::getSeqState();
                    if (seqState.playbackState != XRSequencer::RUNNING) {
                    AudioNoInterrupts();
                        comboVoice.oscb.frequency(oscFreqB);
                    AudioInterrupts();
                    }

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;
        case 2:
            {
                float currCutoff = currTrack.cutoff;

                int mult = 1;
                if (abs(diff) > 5) {
                    mult = 200;
                } else if (abs(diff) > 1 && abs(diff) < 5) {
                    mult = 10;
                }

                float newCutoff = currTrack.cutoff + (diff * mult);

                if (!(newCutoff < 1 || newCutoff > 3000) && newCutoff != currCutoff) {
                    currTrack.cutoff = newCutoff;

                AudioNoInterrupts();
                    comboVoice.lfilter.frequency(newCutoff);
                AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }   
            }

            break;
        case 3:
            {
                float currDecay = currTrack.filter_decay;

                int mult = 1;
                if (abs(diff) > 5) {
                    mult = 100;
                } else if (abs(diff) > 1 && abs(diff) < 5) {
                    mult = 10;
                }

                float newDecay = currTrack.filter_decay + (diff * mult);

                if (!(newDecay < 0 || newDecay > 11880) && newDecay != currDecay)
                {
                    currTrack.filter_decay = newDecay;

                AudioNoInterrupts();
                    comboVoice.filterEnv.decay(newDecay);
                AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            
            break;

        case 4:
            {
                float currDecay = currTrack.amp_decay;

                int mult = 1;
                if (abs(diff) > 5) {
                    mult = 100;
                } else if (abs(diff) > 1 && abs(diff) < 5) {
                    mult = 10;
                }

                float newDecay = currTrack.amp_decay + (diff * mult);

                // real max decay = 11880
                if (!(newDecay < 0 || newDecay > 500) && newDecay != currDecay) {
                    currTrack.amp_decay = newDecay;

                AudioNoInterrupts();
                    comboVoice.ampEnv.decay(newDecay);
                AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            
            break;
        
        case 5:
            {
                float currPan = currTrack.pan;
                float newPan = currTrack.pan + (diff * 0.1);

                if (!(newPan < -1.0 || newPan > 1.0) && newPan != currPan) {
                    currTrack.pan = newPan;

                    float newGainL = 1.0;
                    if (newPan < 0) {
                        newGainL += newPan;
                    }

                    float newGainR = 1.0;
                    if (newPan > 0) {
                        newGainR -= newPan;
                    }

                AudioNoInterrupts();
                    // comboVoices[current_selected_track].leftCtrl.gain(getStereoPanValues(newPan).left * (currTrack.velocity * 0.01));
                    // comboVoices[current_selected_track].leftCtrl.gain(getStereoPanValues(newPan).right * (currTrack.velocity * 0.01));
                    comboVoice.leftCtrl.gain(newGainR);
                    comboVoice.rightCtrl.gain(newGainL);
                AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;

        default:
            break;
        }
    }

    void handleEncoderSubtractiveSynthModC(int diff)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &currStep = XRSequencer::getHeapCurrentSelectedTrackStep();
        auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto numNotesHeldOfKeyboard = XRVersa::getKeyboardNotesHeld();
        auto noteOnKeyboard = XRVersa::getNoteOnKeyboard();
        auto currUXMode = XRUX::getCurrentMode();

        switch (currSelectedPageNum)
        {
        case 0:
            {
                float currVel = currTrack.velocity;
                float newVel = currVel + diff;

                if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1) {
                    currVel = currStep.velocity;
                    newVel = currVel + diff;
                }

                if (!(newVel < 1 || newVel > 100) && newVel != currVel)
                {
                    if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1) {
                        patternMods.tracks[currSelectedTrackNum].step_mod_flags[currSelectedStepNum].flags[XRSequencer::MOD_ATTRS::VELOCITY] = true;
                        patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].velocity = newVel;
                    } else {
                        currTrack.velocity = newVel;
                    }

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
                float currFine = currTrack.fine;
                float newFine = currFine + diff;

                if (!(newFine < -50.0 || newFine > 50.0) && newFine != currFine)
                {
                    currTrack.fine = newFine;

                    // TODO: also use step note
                    uint8_t noteToUse = currTrack.note;
                    if (numNotesHeldOfKeyboard != 0) {
                        noteToUse = noteOnKeyboard;
                    } else  {
                        // note_to_use = currStep.note;
                    }

                    // TODO: allow adjust fine freq in realtime:
                    auto &seqState = XRSequencer::getSeqState();
                    if (seqState.playbackState != XRSequencer::RUNNING) {
                        float oscFreqA = XRSound::getOscFreqA(noteToUse, currTrack.fine);

                    AudioNoInterrupts();
                        comboVoice.osca.frequency(oscFreqA);
                    AudioInterrupts();
                    }

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;
        case 2:
            {
                float currRes = currTrack.res;

                int mult = 1;
                if (abs(diff) > 5) {
                    mult = 10;
                } else if (abs(diff) > 1 && abs(diff) < 5) {
                    mult = 5;
                }

                float newRes = currTrack.res + (diff * mult * 0.01);

                if (!(newRes < -0.01 || newRes > 1.9) && newRes != currRes) {
                    currTrack.res = newRes;

                AudioNoInterrupts();
                    comboVoice.lfilter.resonance(newRes);
                AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;
        case 3:
            {
                float curSus = currTrack.filter_sustain;
                float newSus = currTrack.filter_sustain + (diff * 0.01);

                if (!(newSus < 0 || newSus > 1.0) && newSus != curSus)
                {
                    currTrack.filter_sustain = newSus;

                AudioNoInterrupts();
                    comboVoice.filterEnv.sustain(newSus);
                AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;
        case 4:
            {
                float curSus = currTrack.amp_sustain;
                float newSus = currTrack.amp_sustain + (diff * 0.01);

                if (!(newSus < 0 || newSus > 1.0) && newSus != curSus)
                {
                    currTrack.amp_sustain = newSus;

                    AudioNoInterrupts();
                    comboVoice.ampEnv.sustain(newSus);
                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;
        case 5:
            /* code */

            break;
        
        default:
            break;
        }
    }

    void handleEncoderSubtractiveSynthModD(int diff)
    {
        //auto &seqState = XRSequencer::getSeqState();
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        //auto &currStep = XRSequencer::getHeapCurrentSelectedTrackStep();
        //auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();
        //auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        //auto currSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        //auto numNotesHeldOfKeyboard = XRVersa::getKeyboardNotesHeld();
        //auto noteOnKeyboard = XRVersa::getNoteOnKeyboard();
        //auto currUXMode = XRUX::getCurrentMode();

        switch (currSelectedPageNum)
        {
        case 0:
            /* code */
            break;

        case 1: 
            {
                float currWidth = currTrack.width;
                float newWidth = currWidth + (diff * 0.01);

                if (!(newWidth < 0.01 || newWidth > 1.0) && newWidth != currWidth)
                {
                    currTrack.width = newWidth;

                AudioNoInterrupts();
                    comboVoice.osca.pulseWidth(newWidth);
                    comboVoice.oscb.pulseWidth(newWidth);
                AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            } 

            break;

        case 2:
            {
                float currFilterAmt = currTrack.filterenvamt;

                int mult = 1;
                if (abs(diff) > 5)
                {
                    mult = 150;
                }
                else if (abs(diff) > 1 && abs(diff) < 5)
                {
                    mult = 10;
                }

                float newFilterAmt = currTrack.filterenvamt + (diff * mult * 0.005);

                if (!(newFilterAmt < -1.0 || newFilterAmt > 1.0) && newFilterAmt != currFilterAmt)
                {
                    currTrack.filterenvamt = newFilterAmt;

                AudioNoInterrupts();
                    comboVoice.dc.amplitude(newFilterAmt);
                AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;

        case 3:
            {
                float curRel = currTrack.filter_release;

                int mult = 1;
                if (abs(diff) > 5) {
                    mult = 200;
                } else if (abs(diff) > 1 && abs(diff) < 5) {
                    mult = 20;
                }

                float newRel = currTrack.filter_release + (diff * mult);

                if (!(newRel < 0 || newRel > 11880) && newRel != curRel) {
                    currTrack.filter_release = newRel;

                AudioNoInterrupts();
                    comboVoice.filterEnv.release(newRel);
                AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;

        case 4:
            {
                float curRel = currTrack.amp_release;

                int mult = 1;
                if (abs(diff) > 5) {
                    mult = 200;
                } else if (abs(diff) > 1 && abs(diff) < 5) {
                    mult = 20;
                }

                float newRel = currTrack.amp_release + (diff * mult);

                // real max release = 11880
                if (!(newRel < 0 || newRel > 11880) && newRel != curRel) {
                    currTrack.amp_release = newRel;

                AudioNoInterrupts();
                    comboVoice.ampEnv.release(newRel);
                AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;

        case 5:
            /* code */
            break;
        
        default:
            break;
        }
    }

    void updateSubtractiveSynthWaveform(int diff)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto currSelectedTrack = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedStep = XRSequencer::getCurrentSelectedStepNum();
        auto currentUXMode = XRUX::getCurrentMode();
        int currWaveform = XRSound::getWaveformNumber(currTrack.waveform);

        if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStep > -1) {
            currWaveform = XRSound::getWaveformNumber(patternMods.tracks[currSelectedTrack].steps[currSelectedStep].waveform);
        }

        int newWaveform = currWaveform + diff;

        if (newWaveform < 0) {
            newWaveform = 5;
        }  else if (newWaveform > 5) {
            newWaveform = 0;
        }

        int waveformSel = XRSound::getWaveformTypeSelection(newWaveform);

        if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE) {
            patternMods.tracks[currSelectedTrack].step_mod_flags[currSelectedStep].flags[XRSequencer::MOD_ATTRS::WAVEFORM] = true;
           patternMods.tracks[currSelectedTrack].steps[currSelectedStep].waveform = waveformSel;
        } else {
            currTrack.waveform = waveformSel;
        }

        auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();
        comboVoice.osca.begin(waveformSel);
        comboVoice.oscb.begin(waveformSel);

        XRDisplay::drawSequencerScreen(false);
    }

    void updateSubtractiveSynthNoiseAmt(int diff)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();

        float currNoise = currTrack.noise;
        float newNoise = currTrack.noise + (diff * 0.05);

        if (!(newNoise < 0.01 || newNoise > 1.0) && newNoise != currNoise)
        {
            currTrack.noise = newNoise;

        AudioNoInterrupts();
            auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();
            comboVoice.noise.amplitude(newNoise);
        AudioInterrupts();

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void updateSubtractiveSynthFilterEnvAttack(int diff)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();

        float currAtt = currTrack.filter_attack;

        int mult = 1;
        if (abs(diff) > 5)
        {
            mult = 200;
        }
        else if (abs(diff) > 1 && abs(diff) < 5)
        {
            mult = 20;
        }

        float newAtt = currTrack.filter_attack + (diff * mult);

        // real max attack = 11880
        if (!(newAtt < 0 || newAtt > 1000) && newAtt != currAtt)
        {
            currTrack.filter_attack = newAtt;

        AudioNoInterrupts();
            auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();
            comboVoice.filterEnv.attack(newAtt);
        AudioInterrupts();

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void updateTrackAmpEnvAttack(int diff)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();

        float currAtt = currTrack.amp_attack;

        int mult = 1;
        if (abs(diff) > 5)
        {
            mult = 100;
        }
        else if (abs(diff) > 1 && abs(diff) < 5)
        {
            mult = 10;
        }

        float newAtt = currTrack.amp_attack + (diff * mult);

        if (!(newAtt < 1 || newAtt > 500) && newAtt != currAtt)
        {
            currTrack.amp_attack = newAtt;

        AudioNoInterrupts();
            auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();
            comboVoice.ampEnv.attack(newAtt);
        AudioInterrupts();

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void updateComboTrackLevel(int diff)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();

        float currLvl = currTrack.level;
        float newLvl = currTrack.level + (diff * 0.01);

        if (!(newLvl < 0.0 || newLvl > 1.1) && newLvl != currLvl)
        {
            currTrack.level = newLvl;

        AudioNoInterrupts();
            auto &comboVoice = XRSound::getComboVoiceForCurrentTrack();
            comboVoice.leftSubMix.gain(0, newLvl);
            comboVoice.leftSubMix.gain(1, newLvl);
            comboVoice.rightSubMix.gain(0, newLvl);
            comboVoice.rightSubMix.gain(1, newLvl);
        AudioInterrupts();

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void updateTrackLength(int diff)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currUXMode = XRUX::getCurrentMode();

        // length adj
        int currLen = currTrack.length;
        int newLen = currLen + diff;

        if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currStepNum > -1) {
            currLen = patternMods.tracks[currTrackNum].steps[currStepNum].length;
            newLen = currLen + diff;
        }

        if (!(newLen < 0 && newLen > 64) && (newLen != currLen)) {
            if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currStepNum > -1) {
                patternMods.tracks[currTrackNum].step_mod_flags[currStepNum].flags[XRSequencer::MOD_ATTRS::LENGTH] = true;
                patternMods.tracks[currTrackNum].steps[currStepNum].length = newLen;
            } else {
                currTrack.length = newLen;
            }

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void updateTrackLastStep(int diff)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();

        int currLastStep = currTrack.last_step;
        int newLastStep = currTrack.last_step + diff;

        // make sure track last step doesn't exceed pattern's
        auto &currPattern = XRSequencer::getHeapCurrentSelectedPattern();

        // TODO: try to re-align current playing track step with pattern step if able

        if (newLastStep < 1) {
            newLastStep = 1;
        } else if (newLastStep > currPattern.last_step) {
            newLastStep = currPattern.last_step;
        }

        if (newLastStep != currLastStep) {
            currTrack.last_step = newLastStep;

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
}