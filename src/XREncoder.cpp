#include <XREncoder.h>
#include <XRUX.h>
#include <XRMenu.h>
#include <XRDisplay.h>
#include <XRClock.h>
#include <XRSD.h>
#include <XRSequencer.h>
#include <XRLED.h>
#include <XRVersa.h>

using namespace XRSound;

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
            currentUXMode == XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK_SOUND
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
            } else if (currentUXMode == XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK_SOUND) {
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
                if (currPattern.tracks[t].lstep > newLastStep)
                {
                    currPattern.tracks[t].lstep = newLastStep;
                }
            }

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void handleEncoderPatternModB(int diff)
    {
        auto &currPattern = XRSequencer::getHeapCurrentSelectedPattern();

        int currGrooveId = currPattern.groove.id;
        int newGrooveId = currPattern.groove.id + diff;

        if (newGrooveId < -1)  {
            newGrooveId = -1;
        }  else if (newGrooveId > MAXIMUM_GROOVE_CONFIGS - 1) {
            newGrooveId = MAXIMUM_GROOVE_CONFIGS - 1;
        }

        if (newGrooveId != currGrooveId)  {
            currPattern.groove.id = newGrooveId;

            auto shuffleTemplate = XRClock::getShuffleTemplateForGroove(currPattern.groove.id, currPattern.groove.amount);

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

        int currGrooveAmt = currPattern.groove.amount;
        int newGrooveAmt = currPattern.groove.amount + diff;

        if (newGrooveAmt < 0) {
            newGrooveAmt = 0;
        } else if (newGrooveAmt > MAXIMUM_GROOVE_OPTIONS - 1) {
            newGrooveAmt = MAXIMUM_GROOVE_OPTIONS - 1;
        }

        if (newGrooveAmt != currGrooveAmt)
        {
            currPattern.groove.amount = newGrooveAmt;

            XRClock::setShuffleTemplateForGroove(currPattern.groove.id, newGrooveAmt);

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void handleEncoderPatternModD(int diff)
    {
        Serial.println("TODO: impl patter mod D");
    }

    void handleEncoderSetTrackMods()
    {
        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();

        for (int m = 1; m < 5; m++) // starting from 1 since 1-4 are the mod encoders, 0 is main
        {
            int mDiff = getDiff(m);

            if (mDiff == 0) continue;

            // Serial.printf("m: %d, mDiff: %d\n", m, mDiff);

            if (XRSound::currentPatternSounds[currTrackNum].type == T_MONO_SAMPLE)
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
            else if (XRSound::currentPatternSounds[currTrackNum].type == T_MONO_SYNTH)
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
            else if (XRSound::currentPatternSounds[currTrackNum].type == T_DEXED_SYNTH)
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
            else if (XRSound::currentPatternSounds[currTrackNum].type == T_FM_DRUM)
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
            else if (XRSound::currentPatternSounds[currTrackNum].type == T_CV_GATE)
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
        auto &currPattern = XRSequencer::getHeapCurrentSelectedPattern();
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &currStep = XRSequencer::getHeapCurrentSelectedTrackStep();
        //auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto numNotesHeldOfKeyboard = XRVersa::getKeyboardNotesHeld();
        auto noteOnKeyboard = XRVersa::getNoteOnKeyboard();
        auto currUXMode = XRUX::getCurrentMode();

        auto &comboVoice = getComboVoiceForCurrentTrack();

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
            int currLoopType = getValueNormalizedAsUInt8(XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_LOOPTYPE]);

            // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
            // {
            //     currLoopType = patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].looptype;
            // }

            int32_t newLoopType = (currLoopType + diff);

            if (newLoopType < 0)
            {
                newLoopType = 1;
            }
            else if (newLoopType > 1)
            {
                newLoopType = 0;
            }

            // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStepNum > -1)
            // {
            //     patternMods.tracks[currSelectedTrackNum].step_mod_flags[currSelectedStepNum].flags[XRSequencer::MOD_ATTRS::LOOPTYPE] = true;
            //     patternMods.tracks[currSelectedTrackNum].steps[currSelectedStepNum].looptype = newLoopType;
            // }
            // else
            // {
                // currTrack.looptype = newLoopType;
                
                XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_LOOPTYPE] = getInt32ValuePaddedAsInt32(newLoopType);

            // }

            XRDisplay::drawSequencerScreen(false);
        }
        break;
        case 2:
        {
            float currAtt = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_AMP_ATTACK]);

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
                XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_AMP_ATTACK] = getFloatValuePaddedAsInt32(newAtt);

                if (currSelectedTrackNum > 3)
                {
                    AudioNoInterrupts();
                    auto &sampleVoice = getSampleVoiceForTrack(currSelectedTrackNum - 4);

                    sampleVoice.ampEnv.attack(newAtt);
                    AudioInterrupts();
                }
                else
                {
                    AudioNoInterrupts();
                    auto &comboVoice = getComboVoiceForCurrentTrack();

                    comboVoice.ampEnv.attack(newAtt);
                    AudioInterrupts();
                }

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;
        case 3:
        {
            float currLvl = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_LEVEL]);
            float newLvl = currLvl + (diff * 0.01);

            if (!(newLvl < 0.0 || newLvl > 1.1) && newLvl != currLvl)
            {
                XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_LEVEL] = getFloatValuePaddedAsInt32(newLvl);

                if (currSelectedTrackNum > 3)
                {
                    AudioNoInterrupts();
                    auto &sampleVoice = getSampleVoiceForTrack(currSelectedTrackNum - 4);

                    sampleVoice.leftSubMix.gain(0, newLvl);
                    sampleVoice.rightSubMix.gain(0, newLvl);
                    AudioInterrupts();
                }
                else
                {
                    AudioNoInterrupts();
                    auto &comboVoice = getComboVoiceForCurrentTrack();

                    comboVoice.leftSubMix.gain(0, newLvl);
                    comboVoice.rightSubMix.gain(0, newLvl);
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
        auto &currPattern = XRSequencer::getHeapCurrentSelectedPattern();
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &currStep = XRSequencer::getHeapCurrentSelectedTrackStep();
        // auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto &comboVoice = getComboVoiceForCurrentTrack();
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
            uint32_t currLoopStart = getValueNormalizedAsUInt32(XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_LOOPSTART]);

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
                    XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_LOOPSTART] = getUInt32ValuePaddedAsInt32(newLoopStart);
                // }

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;

        case 2:
        {
            float currDecay = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_AMP_DECAY]);

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
            if (!(newDecay < 0 || newDecay > 500) && newDecay != currDecay)
            {
                XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_AMP_DECAY] = getFloatValuePaddedAsInt32(newDecay);

                if (currSelectedTrackNum > 3)
                {
                    auto &sampleVoice = getSampleVoiceForTrack(currSelectedTrackNum - 4);
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
            float currPan = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_PAN]);
            float newPan = currPan + (diff * 0.1);

            if (!(newPan < -1.0 || newPan > 1.0) && newPan != currPan)
            {
                XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_PAN] = getFloatValuePaddedAsInt32(newPan);

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
                    auto &sampleVoice = getSampleVoiceForTrack(currSelectedTrackNum - 4);
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

    void handleEncoderMonoSampleModC(int diff)
    {
        auto &currPattern = XRSequencer::getHeapCurrentSelectedPattern();
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &currStep = XRSequencer::getHeapCurrentSelectedTrackStep();
        //auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto &comboVoice = getComboVoiceForCurrentTrack();
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
                float currSpeed = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_SAMPLEPLAYRATE]);

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
                        XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_SAMPLEPLAYRATE] = getFloatValuePaddedAsInt32(newSpeed);
                    // }

                    if (currSelectedTrackNum > 3)
                    {
                        auto &sampleVoice = getSampleVoiceForTrack(currSelectedTrackNum - 4);
                        AudioNoInterrupts();
                        sampleVoice.sample.setPlaybackRate(newSpeed);
                        AudioInterrupts();
                    }
                    else
                    {
                        AudioNoInterrupts();
                        comboVoice.sample.setPlaybackRate(newSpeed);
                        AudioInterrupts();
                    }

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            break;
        case 1:
            {
                uint32_t currLoopFinish = getValueNormalizedAsUInt32(XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_LOOPFINISH]);

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
                        XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_LOOPFINISH] = getUInt32ValuePaddedAsInt32(newLoopFinish);
                    // }

                    XRDisplay::drawSequencerScreen(false);
                }
            }
        break;

        case 2:
            {
                float curSus = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_AMP_SUSTAIN]);
                float newSus = curSus + (diff * 0.01);

                if (!(newSus < 0 || newSus > 1.0) && newSus != curSus)
                {
                    XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_AMP_SUSTAIN] = getFloatValuePaddedAsInt32(newSus);

                    if (currSelectedTrackNum > 3)
                    {
                        auto &sampleVoice = getSampleVoiceForTrack(currSelectedTrackNum - 4);
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

    void handleEncoderMonoSampleModD(int diff)
    {
        auto &currPattern = XRSequencer::getHeapCurrentSelectedPattern();
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &currStep = XRSequencer::getHeapCurrentSelectedTrackStep();
        //auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto &comboVoice = getComboVoiceForCurrentTrack();
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
            //
        }
        break;
        case 1:
        {
            play_start currPlaystart = (play_start)getValueNormalizedAsUInt8(XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_PLAYSTART]);

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
                XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_PLAYSTART] = getInt32ValuePaddedAsInt32(newPlayStart);
            // }

            XRDisplay::drawSequencerScreen(false);
        }
        break;
        case 2:
        {
            float curRel = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_AMP_RELEASE]);

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
                XRSound::currentPatternSounds[currSelectedTrackNum].params[MSMP_AMP_RELEASE] = getFloatValuePaddedAsInt32(newRel);

                if (currSelectedTrackNum > 3)
                {
                    auto &sampleVoice = getSampleVoiceForTrack(currSelectedTrackNum - 4);
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

    void handleEncoderDexedSynthModA(int diff)
    {
        auto &currPattern = XRSequencer::getHeapCurrentSelectedPattern();
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &currStep = XRSequencer::getHeapCurrentSelectedTrackStep();
        //auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto &comboVoice = getComboVoiceForCurrentTrack();
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
            float currLvl = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrackNum].params[DEXE_LEVEL]);
            float newLvl = currLvl + (diff * 0.01);

            if (!(newLvl < 0.0 || newLvl > 1.1) && newLvl != currLvl)
            {
                XRSound::currentPatternSounds[currSelectedTrackNum].params[DEXE_LEVEL] = getFloatValuePaddedAsInt32(newLvl);

                AudioNoInterrupts();
                auto &comboVoice = getComboVoiceForCurrentTrack();

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

    void handleEncoderDexedSynthModB(int diff)
    {
        auto &currPattern = XRSequencer::getHeapCurrentSelectedPattern();
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &currStep = XRSequencer::getHeapCurrentSelectedTrackStep();
        //auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto &comboVoice = getComboVoiceForCurrentTrack();
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
            float currPan = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrackNum].params[DEXE_PAN]);
            float newPan = currPan + (diff * 0.1);

            if (!(newPan < -1.0 || newPan > 1.0) && newPan != currPan)
            {
                XRSound::currentPatternSounds[currSelectedTrackNum].params[DEXE_PAN] = getFloatValuePaddedAsInt32(newPan);

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

    void handleEncoderDexedSynthModC(int diff)
    {
        // auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        // if (currSelectedPageNum == 0) {
        //     int dif = XRSD::dexedCurrentBank + diff;
        //     if (dif == XRSD::dexedCurrentBank) {
        //         return;
        //     }

        //     XRSD::dexedCurrentBank = XRSD::dexedCurrentBank + diff;

        //     if (XRSD::dexedCurrentBank < 0)
        //     {
        //         XRSD::dexedCurrentBank = 98;
        //     }
        //     else if (XRSD::dexedCurrentBank > 98)
        //     {
        //         XRSD::dexedCurrentBank = 0;
        //     }

        //     Serial.print("dexed_current_bank: ");
        //     Serial.print(XRSD::dexedCurrentBank);

        //     Serial.print(" dexed_current_patch: ");
        //     Serial.println(XRSD::dexedCurrentPatch);

        //     XRSD::loadDexedVoiceToCurrentTrack();
        //     XRDisplay::drawSequencerScreen(false);
        // }
    }

    void handleEncoderDexedSynthModD(int diff)
    {
        // auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        // if (currSelectedPageNum == 0) {
        //     int dif = XRSD::dexedCurrentPatch + diff;
        //     if (dif == XRSD::dexedCurrentPatch) {
        //         return;
        //     }

        //     XRSD::dexedCurrentPatch = XRSD::dexedCurrentPatch + diff;

        //     if (XRSD::dexedCurrentPatch < 0)
        //     {
        //         XRSD::dexedCurrentPatch = 31;
        //     }
        //     else if (XRSD::dexedCurrentPatch > 31)
        //     {
        //         XRSD::dexedCurrentPatch = 0;
        //     }

        //     Serial.print("dexed_current_bank: ");
        //     Serial.print(XRSD::dexedCurrentBank);

        //     Serial.print(" dexed_current_patch: ");
        //     Serial.println(XRSD::dexedCurrentPatch);

        //     XRSD::loadDexedVoiceToCurrentTrack();
        //     XRDisplay::drawSequencerScreen(false);
        // }
    }

    void handleEncoderFmDrumModA(int diff)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto &comboVoice = getComboVoiceForCurrentTrack();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto currUXMode = XRUX::getCurrentMode();

        switch (currSelectedPageNum)
        {
        case 0:
        {
            float currNoise = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currTrackNum].params[FMD_NOISE]);
            float newNoise = currNoise + (diff * 0.01);

            if (newNoise != currNoise) {
                XRSound::currentPatternSounds[currTrackNum].params[FMD_NOISE] = getFloatValuePaddedAsInt32(newNoise);

                comboVoice.fmdrum.noise(newNoise);

                XRDisplay::drawSequencerScreen(false);
            }
        }
        break;
        case 1:
        {
            float currLvl = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currTrackNum].params[FMD_LEVEL]);
            float newLvl = currLvl + (diff * 0.01);

            if (!(newLvl < 0.0 || newLvl > 1.1) && newLvl != currLvl)
            {
                XRSound::currentPatternSounds[currTrackNum].params[FMD_LEVEL] = getFloatValuePaddedAsInt32(newLvl);

                auto &comboVoice = getComboVoiceForCurrentTrack();

                AudioNoInterrupts();
                comboVoice.leftSubMix.gain(2, newLvl);
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
        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto &comboVoice = getComboVoiceForCurrentTrack();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        switch (currSelectedPageNum)
        {
        case 0:
            {
                float currDecay = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currTrackNum].params[FMD_DECAY]);
                float newDecay  = currDecay + (diff * 0.01);

                if (newDecay != currDecay) {
                    XRSound::currentPatternSounds[currTrackNum].params[FMD_DECAY] = getFloatValuePaddedAsInt32(newDecay);

                    comboVoice.fmdrum.decay(newDecay);

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;

        case 1:
        {
            float currPan = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currTrackNum].params[FMD_PAN]);
            float newPan = currPan + (diff * 0.1);

            if (!(newPan < -1.0 || newPan > 1.0) && newPan != currPan)
            {
                XRSound::currentPatternSounds[currTrackNum].params[FMD_PAN] = getFloatValuePaddedAsInt32(newPan);

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

        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto &comboVoice = getComboVoiceForCurrentTrack();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        if (currSelectedPageNum == 0) 
        {
            float currFm = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currTrackNum].params[FMD_FM]);
            float newFm = currFm + (diff * 0.01);

            if (newFm != currFm) {
                XRSound::currentPatternSounds[currTrackNum].params[FMD_FM] = getFloatValuePaddedAsInt32(newFm);

                comboVoice.fmdrum.fm(newFm);

                XRDisplay::drawSequencerScreen(false);
            }
        }
    }

    void handleEncoderFmDrumModD(int diff)
    {
        if (diff == 0) {
            return;
        }

        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto &comboVoice = getComboVoiceForCurrentTrack();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        if (currSelectedPageNum == 0) {
            uint8_t currFreq = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currTrackNum].params[FMD_FREQ]);
            int8_t newFreq = currFreq + diff;

            if (newFreq != currFreq && newFreq >= 0) {
                XRSound::currentPatternSounds[currTrackNum].params[FMD_FREQ] = getUInt32ValuePaddedAsInt32(newFreq);

                comboVoice.fmdrum.frequency(newFreq);
            }

            XRDisplay::drawSequencerScreen(false);
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
        //auto &seqState = XRSequencer::getSeqState();
        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &comboVoice = getComboVoiceForCurrentTrack();
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
                float currDetune = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currTrackNum].params[MSYN_DETUNE]);
                float newDetune = currDetune + diff;

                Serial.printf(
                    "raw detune: %d, normal detune: %f, new detune: %f\n",
                    XRSound::currentPatternSounds[currTrackNum].params[MSYN_DETUNE],
                    currDetune,
                    newDetune
                );

                if (!(newDetune < -24 || newDetune > 24) && newDetune != currDetune) {
                    uint8_t noteToUse = currTrack.note;

                    if (numNotesHeldOfKeyboard != 0) {
                        noteToUse = noteOnKeyboard;
                    }

                    XRSound::currentPatternSounds[currTrackNum].params[MSYN_DETUNE] = getFloatValuePaddedAsInt32(newDetune);

                    Serial.printf(
                        "new raw detune: %d\n",
                        XRSound::currentPatternSounds[currTrackNum].params[MSYN_DETUNE]
                    );

                    auto oscFreqB = getDetunedOscFreqB(noteToUse, newDetune);

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
                float currCutoff = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currTrackNum].params[MSYN_CUTOFF]);

                int mult = 1;
                if (abs(diff) > 5) {
                    mult = 200;
                } else if (abs(diff) > 1 && abs(diff) < 5) {
                    mult = 10;
                }

                float newCutoff = currCutoff + (diff * mult);

                if (!(newCutoff < 1 || newCutoff > 3000) && newCutoff != currCutoff) {
                    XRSound::currentPatternSounds[currTrackNum].params[MSYN_CUTOFF] = getFloatValuePaddedAsInt32(newCutoff);

                    AudioNoInterrupts();
                    comboVoice.lfilter.frequency(newCutoff);
                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }   
            }

            break;
        case 3:
            {
                float currDecay = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currTrackNum].params[MSYN_FILTER_DECAY]);

                int mult = 1;
                if (abs(diff) > 5) {
                    mult = 100;
                } else if (abs(diff) > 1 && abs(diff) < 5) {
                    mult = 10;
                }

                float newDecay = currDecay + (diff * mult);

                if (!(newDecay < 0 || newDecay > 11880) && newDecay != currDecay)
                {
                    XRSound::currentPatternSounds[currTrackNum].params[MSYN_FILTER_DECAY] = getFloatValuePaddedAsInt32(newDecay);

                    AudioNoInterrupts();
                    comboVoice.filterEnv.decay(newDecay);
                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            
            break;

        case 4:
            {
                float currDecay = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currTrackNum].params[MSYN_AMP_DECAY]);

                int mult = 1;
                if (abs(diff) > 5) {
                    mult = 100;
                } else if (abs(diff) > 1 && abs(diff) < 5) {
                    mult = 10;
                }

                float newDecay = currDecay + (diff * mult);

                // real max decay = 11880
                if (!(newDecay < 0 || newDecay > 500) && newDecay != currDecay) {
                    XRSound::currentPatternSounds[currTrackNum].params[MSYN_AMP_DECAY] = getFloatValuePaddedAsInt32(newDecay);

                    AudioNoInterrupts();
                    comboVoice.ampEnv.decay(newDecay);
                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }
            
            break;
        
        case 5:
            {
                float currPan = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currTrackNum].params[MSYN_PAN]);
                float newPan = currPan + (diff * 0.1);

                if (!(newPan < -1.0 || newPan > 1.0) && newPan != currPan) {
                    XRSound::currentPatternSounds[currTrackNum].params[MSYN_PAN] = getFloatValuePaddedAsInt32(newPan);

                    float newGainL = 1.0;
                    if (newPan < 0) {
                        newGainL += newPan;
                    }

                    float newGainR = 1.0;
                    if (newPan > 0) {
                        newGainR -= newPan;
                    }

                    AudioNoInterrupts();
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

    void handleEncoderMonoSynthModC(int diff)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto &currStep = XRSequencer::getHeapCurrentSelectedTrackStep();
        //auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto &comboVoice = getComboVoiceForCurrentTrack();
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
                float currFine = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrackNum].params[MSYN_FINE]);
                float newFine = currFine + diff;

                if (!(newFine < -50.0 || newFine > 50.0) && newFine != currFine)
                {
                    XRSound::currentPatternSounds[currSelectedTrackNum].params[MSYN_FINE] = getFloatValuePaddedAsInt32(newFine);

                    // TODO: also use step note
                    uint8_t noteToUse = currTrack.note;
                    if (numNotesHeldOfKeyboard != 0) {
                        noteToUse = noteOnKeyboard;
                    } else  {
                        // note_to_use = currStep.note;
                    }

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
                float currRes = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrackNum].params[MSYN_RESONANCE]);

                int mult = 1;
                if (abs(diff) > 5) {
                    mult = 10;
                } else if (abs(diff) > 1 && abs(diff) < 5) {
                    mult = 5;
                }

                float newRes = currRes + (diff * mult * 0.01);

                if (!(newRes < -0.01 || newRes > 1.9) && newRes != currRes) {
                    XRSound::currentPatternSounds[currSelectedTrackNum].params[MSYN_RESONANCE] = getFloatValuePaddedAsInt32(newRes);

                    AudioNoInterrupts();
                    comboVoice.lfilter.resonance(newRes);
                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;
        case 3:
            {
                float curSus = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrackNum].params[MSYN_FILTER_SUSTAIN]);
                float newSus = curSus + (diff * 0.01);

                if (!(newSus < 0 || newSus > 1.0) && newSus != curSus)
                {
                    XRSound::currentPatternSounds[currSelectedTrackNum].params[MSYN_FILTER_SUSTAIN] = getFloatValuePaddedAsInt32(newSus);

                    AudioNoInterrupts();
                    comboVoice.filterEnv.sustain(newSus);
                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;
        case 4:
            {
                float curSus = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrackNum].params[MSYN_AMP_SUSTAIN]);
                float newSus = curSus + (diff * 0.01);

                if (!(newSus < 0 || newSus > 1.0) && newSus != curSus)
                {
                    XRSound::currentPatternSounds[currSelectedTrackNum].params[MSYN_AMP_SUSTAIN] = getFloatValuePaddedAsInt32(newSus);

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

    void handleEncoderMonoSynthModD(int diff)
    {
        //auto &seqState = XRSequencer::getSeqState();
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        //auto &currStep = XRSequencer::getHeapCurrentSelectedTrackStep();
        //auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto &comboVoice = getComboVoiceForCurrentTrack();
        auto currSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
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
                float currWidth = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrackNum].params[MSYN_WIDTH]);
                float newWidth = currWidth + (diff * 0.01);

                if (!(newWidth < 0.01 || newWidth > 1.0) && newWidth != currWidth)
                {
                    XRSound::currentPatternSounds[currSelectedTrackNum].params[MSYN_WIDTH] = getFloatValuePaddedAsInt32(newWidth);

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
                float currFilterAmt = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrackNum].params[MSYN_FILTER_ENV_AMT]);

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
                    XRSound::currentPatternSounds[currSelectedTrackNum].params[MSYN_FILTER_ENV_AMT] = getFloatValuePaddedAsInt32(newFilterAmt);

                    AudioNoInterrupts();
                    comboVoice.dc.amplitude(newFilterAmt);
                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;

        case 3:
            {
                float curRel = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrackNum].params[MSYN_FILTER_RELEASE]);

                int mult = 1;
                if (abs(diff) > 5) {
                    mult = 200;
                } else if (abs(diff) > 1 && abs(diff) < 5) {
                    mult = 20;
                }

                float newRel = curRel + (diff * mult);

                if (!(newRel < 0 || newRel > 11880) && newRel != curRel) {
                    XRSound::currentPatternSounds[currSelectedTrackNum].params[MSYN_FILTER_RELEASE] = getFloatValuePaddedAsInt32(newRel);

                    AudioNoInterrupts();
                    comboVoice.filterEnv.release(newRel);
                    AudioInterrupts();

                    XRDisplay::drawSequencerScreen(false);
                }
            }

            break;

        case 4:
            {
                float curRel = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrackNum].params[MSYN_AMP_RELEASE]);

                int mult = 1;
                if (abs(diff) > 5) {
                    mult = 200;
                } else if (abs(diff) > 1 && abs(diff) < 5) {
                    mult = 20;
                }

                float newRel = curRel + (diff * mult);

                // real max release = 11880
                if (!(newRel < 0 || newRel > 11880) && newRel != curRel) {
                    XRSound::currentPatternSounds[currSelectedTrackNum].params[MSYN_AMP_RELEASE] = newRel;

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

    void updateMonoSynthWaveform(int diff)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        //auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto currSelectedTrack = XRSequencer::getCurrentSelectedTrackNum();
        auto currSelectedStep = XRSequencer::getCurrentSelectedStepNum();
        auto currentUXMode = XRUX::getCurrentMode();
        int currWaveform = getWaveformNumber(
            getValueNormalizedAsUInt8(XRSound::currentPatternSounds[currSelectedTrack].params[MSYN_WAVE])
        );

        // if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStep > -1) {
        //     currWaveform = getWaveformNumber(patternMods.tracks[currSelectedTrack].steps[currSelectedStep].waveform);
        // }

        int newWaveform = currWaveform + diff;

        if (newWaveform < 0) {
            newWaveform = 5;
        }  else if (newWaveform > 5) {
            newWaveform = 0;
        }

        int waveformSel = getWaveformTypeSelection(newWaveform);

        // if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE) {
        //     patternMods.tracks[currSelectedTrack].step_mod_flags[currSelectedStep].flags[XRSequencer::MOD_ATTRS::WAVEFORM] = true;
        //    patternMods.tracks[currSelectedTrack].steps[currSelectedStep].waveform = waveformSel;
        // } else {
            XRSound::currentPatternSounds[currSelectedTrack].params[MSYN_WAVE] = getInt32ValuePaddedAsInt32(waveformSel);
        // }

        auto &comboVoice = getComboVoiceForCurrentTrack();
        comboVoice.osca.begin(waveformSel);
        comboVoice.oscb.begin(waveformSel);

        XRDisplay::drawSequencerScreen(false);
    }

    void updateMonoSynthNoiseAmt(int diff)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto currSelectedTrack = XRSequencer::getCurrentSelectedTrackNum();

        float currNoise = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrack].params[MSYN_NOISE]);
        float newNoise = currNoise + (diff * 0.05);

        if (!(newNoise < 0.01 || newNoise > 1.0) && newNoise != currNoise)
        {
            XRSound::currentPatternSounds[currSelectedTrack].params[MSYN_NOISE] = getFloatValuePaddedAsInt32(newNoise);

        AudioNoInterrupts();
            auto &comboVoice = getComboVoiceForCurrentTrack();
            comboVoice.noise.amplitude(newNoise);
        AudioInterrupts();

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void updateMonoSynthFilterEnvAttack(int diff)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto currSelectedTrack = XRSequencer::getCurrentSelectedTrackNum();

        float currAtt = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrack].params[MSYN_FILTER_ATTACK]);

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
            XRSound::currentPatternSounds[currSelectedTrack].params[MSYN_FILTER_ATTACK] = getFloatValuePaddedAsInt32(newAtt);

            auto &comboVoice = getComboVoiceForCurrentTrack();

            AudioNoInterrupts();
            comboVoice.filterEnv.attack(newAtt);
            AudioInterrupts();

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void updateTrackAmpEnvAttack(int diff)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto currSelectedTrack = XRSequencer::getCurrentSelectedTrackNum();

        float currAtt = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrack].params[MSYN_AMP_ATTACK]);

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
            XRSound::currentPatternSounds[currSelectedTrack].params[MSYN_AMP_ATTACK] = getFloatValuePaddedAsInt32(newAtt);

            auto &comboVoice = getComboVoiceForCurrentTrack();

            AudioNoInterrupts();
            comboVoice.ampEnv.attack(newAtt);
            AudioInterrupts();

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void updateComboTrackLevel(int diff)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto currSelectedTrack = XRSequencer::getCurrentSelectedTrackNum();

        float currLvl = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrack].params[MSYN_LEVEL]);

        if (XRSound::currentPatternSounds[currSelectedTrack].type == T_MONO_SAMPLE) {
            currLvl = getValueNormalizedAsFloat(XRSound::currentPatternSounds[currSelectedTrack].params[MSMP_LEVEL]);
        }

        float newLvl = currLvl + (diff * 0.01);

        if (!(newLvl < 0.0 || newLvl > 1.1) && newLvl != currLvl)
        {
            if (XRSound::currentPatternSounds[currSelectedTrack].type == T_MONO_SAMPLE) {
                XRSound::currentPatternSounds[currSelectedTrack].params[MSMP_LEVEL] = getFloatValuePaddedAsInt32(newLvl);
            } else {
                XRSound::currentPatternSounds[currSelectedTrack].params[MSYN_LEVEL] = getFloatValuePaddedAsInt32(newLvl);
            }

            auto &comboVoice = getComboVoiceForCurrentTrack();

            AudioNoInterrupts();
            comboVoice.leftSubMix.gain(0, newLvl);
            comboVoice.rightSubMix.gain(0, newLvl);
            AudioInterrupts();

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void updateTrackLength(int diff)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        // auto &patternMods = XRSequencer::getModsForCurrentPattern();
        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currUXMode = XRUX::getCurrentMode();

        // length adj
        int currLen = currTrack.length;
        int newLen = currLen + diff;

        // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currStepNum > -1) {
        //     currLen = patternMods.tracks[currTrackNum].steps[currStepNum].length;
        //     newLen = currLen + diff;
        // }

        if (!(newLen < 0 && newLen > 64) && (newLen != currLen)) {
            // if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currStepNum > -1) {
            //     patternMods.tracks[currTrackNum].step_mod_flags[currStepNum].flags[XRSequencer::MOD_ATTRS::LENGTH] = true;
            //     patternMods.tracks[currTrackNum].steps[currStepNum].length = newLen;
            // } else {
                currTrack.length = newLen;
            // }

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void updateTrackLastStep(int diff)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();

        int currLastStep = currTrack.lstep;
        int newLastStep = currTrack.lstep + diff;

        // make sure track last step doesn't exceed pattern's
        auto &currPattern = XRSequencer::getHeapCurrentSelectedPattern();

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
}