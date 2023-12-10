#include <XRSequencer.h>
#include <XRHelpers.h>
#include <XRDisplay.h>
#include <XRUX.h>
#include <XRKeyMatrix.h>
#include <XRLED.h>
#include <XRSD.h>
#include <XRClock.h>
#include <map>

namespace XRSequencer
{
    DMAMEM SEQUENCER_EXTERNAL _seqExternall;
    DMAMEM PATTERN _patternCopyBuffer;
    DMAMEM TRACK _trackCopyBuffer;
    DMAMEM TRACK_STEP _stepCopyBuffer;
    DMAMEM PATTERN _recordPatternBuffer;
    DMAMEM PATTERN_MODS _patternMods;

    SEQUENCER_HEAP _seqHeap;
    QUEUED_PATTERN _queuedPattern;
    SEQUENCER_STATE _seqState;

    STACK_STEP_DATA _stepStack[STEP_STACK_SIZE];
    STACK_RATCHET_DATA _ratchetStack[RATCHET_STACK_SIZE];

    int8_t _currentSelectedBank = 0;    // default to 0 (first)
    int8_t _currentSelectedPattern = 0; // default to 0 (first)
    int8_t _currentSelectedTrack = 0;   // default to 0 (first)
    int8_t _currentSelectedStep = -1;   // default to -1 (none)
    int8_t _currentSelectedPage = 0;
    int8_t _currentStepPage = 1;

    int8_t _ratchetTrack = -1;
    int8_t _ratchetDivision = -1;
    int8_t _ratchetsHeld = 0;
    long _ratchetReleaseTime;

    std::map<TRACK_TYPE, int> trackPageNumMap = {
        {SUBTRACTIVE_SYNTH, 6},
        {DEXED, 1},
        {RAW_SAMPLE, 4},
        {WAV_SAMPLE, 1},
        {MIDI_OUT, 1},
        {CV_GATE, 1},
        {CV_TRIG, 1},
    };

    std::map<TRACK_TYPE, std::map<int, std::string>> trackCurrPageNameMap = {
        {SUBTRACTIVE_SYNTH, {
                                {0, "MAIN"},
                                {1, "OSC"},
                                {2, "FILTER"},
                                {3, "FILTER ENV"},
                                {4, "AMP ENV"},
                                {5, "OUTPUT"},
                            }},
        {DEXED, {
                    {0, "MAIN"},
                }},
        {RAW_SAMPLE, {
                         {0, "MAIN"},
                         {1, "LOOP"},
                         {2, "AMP ENV"},
                         {3, "OUTPUT"},
                     }},
        {WAV_SAMPLE, {
                         {0, "MAIN"},
                     }},
        {MIDI_OUT, {
                       {0, "MAIN"},
                   }},
        {CV_GATE, {
                      {0, "MAIN"},
                  }},
        {CV_TRIG, {
                      {0, "MAIN"},
                  }},
    };

    bool _trkNeedsInit[MAXIMUM_SEQUENCER_TRACKS] = {
        false, false, false, false,
        false, false, false, false,
        false, false, false, false,
        false, false, false, false};

    bool _dequeuePattern = false;
    int _drawQueueBlink = -1;

    uint8_t _bpmBlinkTimer = 1;

    bool init()
    {
        _currentSelectedBank = 0;
        _currentSelectedPattern = 0;
        _currentSelectedTrack = 0;

        XRDisplay::drawSequencerScreen(false);

        return true;
    }

    void onClockStart()
    {
        auto currentUXMode = XRUX::getCurrentMode();
        if (currentUXMode == XRUX::UX_MODE::PROJECT_INITIALIZE || currentUXMode == XRUX::UX_MODE::PROJECT_BUSY)
        {
            return;
        }

        // usbMIDI.sendRealTime(usbMIDI.Start);
    }

    void onClockStop()
    {
        auto currentUXMode = XRUX::getCurrentMode();
        if (currentUXMode == XRUX::UX_MODE::PROJECT_INITIALIZE || currentUXMode == XRUX::UX_MODE::PROJECT_BUSY)
        {
            return;
        }

        // usbMIDI.sendRealTime(usbMIDI.Stop);

        noteOffForAllSounds();
    }

    void ClockOut16PPQN(uint32_t tick)
    {
        auto currentUXMode = XRUX::getCurrentMode();
        if (currentUXMode == XRUX::UX_MODE::PROJECT_INITIALIZE || currentUXMode == XRUX::UX_MODE::PROJECT_BUSY)
        {
            return;
        }

        handle16PPQN(tick);
    }

    void ClockOut96PPQN(uint32_t tick)
    {
        auto currentUXMode = XRUX::getCurrentMode();
        if (currentUXMode == XRUX::UX_MODE::PROJECT_INITIALIZE || currentUXMode == XRUX::UX_MODE::PROJECT_BUSY)
        {
            return;
        }

        handle96PPQN(tick);

        // Send MIDI_CLOCK to external gears
        // usbMIDI.sendRealTime(usbMIDI.Clock);
    }

    void handle16PPQN(uint32_t tick)
    {
        auto currentUXMode = XRUX::getCurrentMode();
        auto seqState = getSeqState();
        auto currentSelectedTrack = getCurrentSelectedTrackNum();
        auto currentStepPage = getCurrentStepPage();

        bool functionActive = XRKeyMatrix::isFunctionActive();

        // int8_t currentSelectedPatternCurrentStep = seqState.currentStep;
        int8_t currentSelectedTrackCurrentStep = seqState.currentTrackSteps[currentSelectedTrack].currentStep;
        int8_t currentSelectedTrackCurrentBar = seqState.currentTrackSteps[currentSelectedTrack].currentBar;

        int currStepPaged = currentSelectedTrackCurrentStep;

        if (currentStepPage == 2 && currStepPaged > 16 && currStepPaged <= 32)
        {
            currStepPaged -= 16;
        }
        else if (currentStepPage == 3 && currStepPaged > 32 && currStepPaged <= 48)
        {
            currStepPaged -= 32;
        }
        else if (currentStepPage == 4 && currStepPaged > 48 && currStepPaged <= 64)
        {
            currStepPaged -= 48;
        }

        bool isOnStraightQtrNote = (currentSelectedTrackCurrentStep == 1 || !((currentSelectedTrackCurrentStep - 1) % 4));

        if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE && !functionActive && isOnStraightQtrNote)
        {
            auto currTrack = getHeapCurrentSelectedTrack();

            XRLED::displayPageLEDs(
                currentSelectedTrackCurrentBar,
                (_seqState.playbackState == RUNNING),
                currentStepPage,
                currTrack.last_step);
        }

        if (isOnStraightQtrNote)
        {
            XRLED::setPWM(23, 4095); // when recording, each straight quarter note start button led ON
        }

        // This method handles advancing the sequencer
        // and displaying the start btn and step btn BPM LEDs
        int8_t currStepChar = XRHelpers::stepCharMap[currStepPaged];
        int8_t keyLED = XRLED::getKeyLED(currStepChar);

        if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE)
        {
            setDisplayStateForPatternActiveTracksLEDs(true);
        }

        // TODO: move out of 16th step !(tick % (6)) condition
        // so we can check for microtiming adjustments at the 96ppqn scale
        triggerAllStepsForAllTracks(tick);

        if (currentUXMode == XRUX::UX_MODE::PATTERN_SEL)
        {
            XRLED::clearAllStepLEDs();
            XRLED::displayCurrentlySelectedPattern();
        }
        else if (currentUXMode == XRUX::UX_MODE::TRACK_SEL)
        {
            XRLED::clearAllStepLEDs();
            XRLED::displayCurrentlySelectedTrack();
        }
        else if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE)
        {
            bool transitionStepLEDs = ((currentStepPage == 1 && currentSelectedTrackCurrentStep <= 16) ||
                                       (currentStepPage == 2 && currentSelectedTrackCurrentStep > 16 && currentSelectedTrackCurrentStep <= 32) ||
                                       (currentStepPage == 3 && currentSelectedTrackCurrentStep > 32 && currentSelectedTrackCurrentStep <= 48) ||
                                       (currentStepPage == 4 && currentSelectedTrackCurrentStep > 48 && currentSelectedTrackCurrentStep <= 64));

            bool turnOffLastLED = ((currentStepPage == 1 && currentSelectedTrackCurrentStep == 1) ||
                                   (currentStepPage == 2 && currentSelectedTrackCurrentStep == 16) ||
                                   (currentStepPage == 3 && currentSelectedTrackCurrentStep == 32) ||
                                   (currentStepPage == 4 && currentSelectedTrackCurrentStep == 48));

            if (currentSelectedTrackCurrentStep > 1)
            {
                if (transitionStepLEDs)
                {
                    uint8_t prevKeyLED = XRLED::getKeyLED(XRHelpers::stepCharMap[currStepPaged - 1]);

                    XRLED::setPWM(prevKeyLED, 0); // turn prev sixteenth led OFF
                }
            }
            else
            {
                if (turnOffLastLED)
                {
                    TRACK currTrack = getHeapCurrentSelectedTrack();
                    int currTrackLastStep = currTrack.last_step;
                    if (currTrackLastStep > 16)
                    {
                        currTrackLastStep -= 16;
                    }

                    uint8_t prevKeyLED = XRLED::getKeyLED(XRHelpers::stepCharMap[currTrackLastStep]);

                    XRLED::setPWM(prevKeyLED, 0); // turn prev sixteenth led OFF
                }
            }

            if (transitionStepLEDs)
            {
                XRLED::setPWM(keyLED, 4095); // turn sixteenth led ON
            }
        }

        updateCurrentPatternStepState();
        updateAllTrackStepStates();
    }

    void handle96PPQN(uint32_t tick)
    {
        auto currentUXMode = XRUX::getCurrentMode();

        if ((tick % 6) && !(tick % _bpmBlinkTimer))
        {
            // if (_recording) {
            XRLED::setPWM(23, 0); // turn start button led OFF every 16th note
            // }
        }

        if (!(tick % 6))
        {
            if (_queuedPattern.bank > -1 && _queuedPattern.number > -1)
            {
                _drawQueueBlink = 0;
            }
        }

        triggerRatchetingTrack(tick);
        handleRemoveFromStepStack(tick);

        if ((tick % (6)) && !(tick % _bpmBlinkTimer))
        {
            if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE)
            {
                setDisplayStateForPatternActiveTracksLEDs(false);
            }
            else if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE || currentUXMode == XRUX::UX_MODE::SUBMITTING_STEP_VALUE)
            {
                XRLED::setDisplayStateForAllStepLEDs();

                if (!XRKeyMatrix::isFunctionActive())
                {
                    XRLED::displayPageLEDs(
                        -1,
                        (_seqState.playbackState == RUNNING),
                        _currentStepPage,
                        _seqHeap.pattern.last_step // TODO: use pattern OR track last step depending which UX mode is active
                    );   
                }
            }
        }
    }

    void handleRemoveFromStepStack(uint32_t tick)
    {
        for (int i = 0; i < STEP_STACK_SIZE; i++)
        {
            if (_stepStack[i].length != -1)
            {
                --_stepStack[i].length;

                if (_stepStack[i].length == 0)
                {
                    handleNoteOffForTrackStep(_stepStack[i].trackNum, _stepStack[i].stepNum);

                    // re-initialize stack entry
                    _stepStack[i].trackNum = -1;
                    _stepStack[i].stepNum = -1;
                    _stepStack[i].length = -1;
                }
            }
        }
    }

    void handleNoteOffForTrackStep(int track, int step)
    {
        auto currTrack = getHeapTrack(track);
        auto currTrackStep = getHeapStep(track, step);

        Serial.println("TODO: impl noteOff for track step");

        return;

        // if (currTrack.track_type == SUBTRACTIVE_SYNTH)
        // {
        //     comboVoices[track].ampEnv.noteOff();
        //     comboVoices[track].filterEnv.noteOff();
        // }

        // else if (currTrack.track_type == DEXED)
        // {
        //     // comboVoices[track].dexed.notesOff();

        //     uint8_t noteToUse = currTrackStep.note;
        //     if (_pattern_mods_mem.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::NOTE])
        //     {
        //         noteToUse = _pattern_mods_mem.tracks[track].steps[step].note;
        //         // Serial.println(noteToUse);
        //     }

        //     uint8_t octaveToUse = currTrackStep.octave;
        //     if (_pattern_mods_mem.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::OCTAVE])
        //     {
        //         octaveToUse = _pattern_mods_mem.tracks[track].steps[step].octave;
        //         // Serial.println(octaveToUse);
        //     }

        //     int midiNote = (noteToUse + (12 * (octaveToUse)));

        //     comboVoices[track].dexed.keyup(midiNote);
        // }

        // // fix
        // else if (currTrack.track_type == MIDI_OUT)
        // {
        //     MIDI.sendNoteOff(64, 100, 1);
        // }
        // else if (currTrack.track_type == CV_GATE)
        // {
        //     if (currTrack.channel == 1)
        //     {
        //         // writeToDAC(CS1, 0, cvLevels[midiNote]); // cv
        //         writeToDAC(CS1, 1, 0); // gate
        //     }
        //     else if (currTrack.channel == 2)
        //     {
        //         // writeToDAC(CS2, 0, cvLevels[midiNote]); // cv
        //         writeToDAC(CS2, 1, 0); // gate
        //     }
        //     else if (currTrack.channel == 3)
        //     {
        //         // writeToDAC(CS3, 0, cvLevels[midiNote]); // cv
        //         writeToDAC(CS3, 1, 0); // gate
        //     }
        //     else if (currTrack.channel == 4)
        //     {
        //         // writeToDAC(CS4, 0, cvLevels[midiNote]); // cv
        //         writeToDAC(CS4, 1, 0); // gate
        //     }
        // }

        // else
        // {
        //     if (track > 3)
        //     {
        //         int tOffset = track - 4;
        //         sampleVoices[tOffset].ampEnv.noteOff();
        //     }
        //     else
        //     {
        //         comboVoices[track].ampEnv.noteOff();
        //     }
        // }
    }

    void noteOffForAllSounds()
    {
        for (int t = 0; t < 4; t++)
        {
            TRACK currTrack = getHeapTrack(t);

            if (currTrack.track_type == SUBTRACTIVE_SYNTH)
            {
                XRSound::comboVoices[t].ampEnv.noteOff();
                XRSound::comboVoices[t].filterEnv.noteOff();
            }
            else
            {
                XRSound::comboVoices[t].ampEnv.noteOff();
            }
        }

        for (int s = 0; s < 12; s++)
        {
            XRSound::sampleVoices[s].ampEnv.noteOff();
        }
    }

    SEQUENCER_STATE getSeqState()
    {
        return _seqState;
    }

    int8_t getCurrentSelectedBankNum()
    {
        return _currentSelectedBank;
    }

    int8_t getCurrentSelectedPatternNum()
    {
        return _currentSelectedPattern;
    }

    int8_t getCurrentSelectedTrackNum()
    {
        return _currentSelectedTrack;
    }

    int8_t getCurrentSelectedStepNum()
    {
        return _currentSelectedStep;
    }

    int8_t getCurrentSelectedPage()
    {
        return _currentSelectedPage;
    }

    int8_t getCurrentStepPage()
    {
        return _currentStepPage;
    }

    QUEUED_PATTERN getQueuedPattern()
    {
        return _queuedPattern;
    }

    SEQUENCER_HEAP getSequencerHeap()
    {
        return _seqHeap;
    }

    SEQUENCER_EXTERNAL getSequencerExternal()
    {
        return _seqExternall;
    }

    TRACK getHeapTrack(int track)
    {
        return _seqHeap.pattern.tracks[track];
    }

    TRACK_STEP getHeapStep(int track, int step)
    {
        return _seqHeap.pattern.tracks[track].steps[step];
    }

    PATTERN getHeapCurrentSelectedPattern()
    {
        return _seqHeap.pattern;
    }

    TRACK getHeapCurrentSelectedTrack()
    {
        return _seqHeap.pattern.tracks[_currentSelectedTrack];
    }

    TRACK_STEP getHeapCurrentSelectedTrackStep()
    {
        return _seqHeap.pattern.tracks[_currentSelectedTrack].steps[_currentSelectedStep];
    }

    std::string getTrackMetaStr(TRACK_TYPE type)
    {
        std::string outputStr;

        switch (type)
        {
        case SUBTRACTIVE_SYNTH:
            outputStr = "SYNTH:";
            break;

        case RAW_SAMPLE:
            outputStr = "RSAMPLE:";
            break;

        case WAV_SAMPLE:
            outputStr = "WSAMPLE:";
            break;

        case DEXED:
            outputStr = "DEXED:";
            break;

        case MIDI_OUT:
            outputStr = "MIDI";
            break;

        case CV_GATE:
            outputStr = "CV/GATE";
            break;

        case CV_TRIG:
            outputStr = "CV/TRIG";
            break;

        default:
            break;
        }

        return outputStr;
    }

    std::string getTrackTypeNameStr(TRACK_TYPE type)
    {
        std::string str = "MIDI"; // default

        switch (type)
        {
        case TRACK_TYPE::CV_GATE:
            str = "CV/GATE";

            break;

        case TRACK_TYPE::CV_TRIG:
            str = "CV/TRIG";

            break;

        case TRACK_TYPE::RAW_SAMPLE:
            str = "MONO SAMPLE";

            break;

        case TRACK_TYPE::WAV_SAMPLE:
            str = "STEREO SAMPLE";

            break;

        case TRACK_TYPE::SUBTRACTIVE_SYNTH:
            str = "2-OSC SUBTRACTIVE";

            break;

        case TRACK_TYPE::DEXED:
            str = "DEXED";

            break;

        default:
            break;
        }

        return str;
    }

    PATTERN_MODS getModsForCurrentPattern()
    {
        return _patternMods;
    }

    TRACK_STEP_MODS getModsForCurrentTrackStep()
    {
        return _patternMods.tracks[_currentSelectedTrack].steps[_currentSelectedTrack];
    }

    std::string getCurrPageNameForTrack()
    {
        TRACK currTrack = getHeapCurrentSelectedTrack();

        std::string outputStr = trackCurrPageNameMap[currTrack.track_type][_currentSelectedPage];

        return outputStr;
    }

    uint8_t getCurrentTrackPageCount()
    {
        TRACK currTrack = getHeapCurrentSelectedTrack();

        return trackPageNumMap[currTrack.track_type];
    }

    int8_t getRatchetTrack()
    {
        return _ratchetTrack;
    }

    int8_t getRatchetDivision()
    {
        return _ratchetDivision;
    }

    void setDisplayStateForPatternActiveTracksLEDs(bool enable)
    {
        auto currentPattern = getHeapCurrentSelectedPattern();

        const int MAX_PATTERN_TRACK_SIZE = 17;
        for (int t = 1; t < MAX_PATTERN_TRACK_SIZE; t++)
        {
            auto currTrack = currentPattern.tracks[t - 1];
            auto currTrackStep = _seqState.currentTrackSteps[t - 1].currentStep;
            auto currTrackStepForLED = currTrack.steps[currTrackStep - 1];
            auto currLEDChar = XRHelpers::stepCharMap[t];
            auto keyLED = XRLED::getKeyLED(currLEDChar);

            if (currTrackStepForLED.state == TRACK_STEP_STATE::STATE_OFF)
            {
                if (keyLED < 0)
                {
                    Serial.println("could not find key LED!");
                }
                else
                {
                    XRLED::setPWM(keyLED, 0);
                }
            }
            else if (currTrackStepForLED.state == TRACK_STEP_STATE::STATE_ON || currTrackStepForLED.state == TRACK_STEP_STATE::STATE_ACCENTED)
            {
                XRLED::setPWM(keyLED, enable ? 4095 : 0);
            }
        }
    }

    void triggerAllStepsForAllTracks(uint32_t tick)
    {
        auto currentPattern = getHeapCurrentSelectedPattern();

        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            int8_t currTrackStep = _seqState.currentTrackSteps[t].currentStep - 1; // get zero-based track step number

            auto currTrack = currentPattern.tracks[t];
            auto currTrackStepData = currTrack.steps[currTrackStep];

            if (!currTrack.muted && ((currTrackStepData.state == TRACK_STEP_STATE::STATE_ON) || (currTrackStepData.state == TRACK_STEP_STATE::STATE_ACCENTED)))
            {
                handleAddToStepStack(tick, t, currTrackStep);
            }
        }
    }

    void handleAddToStepStack(uint32_t tick, int track, int step)
    {
        auto trackToUse = getHeapTrack(track);

        bool lenStepModEnabled = _patternMods.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::LENGTH];
        int lenStepMod = _patternMods.tracks[track].steps[step].length;

        for (uint8_t i = 0; i < STEP_STACK_SIZE; i++)
        {
            if (!trackToUse.muted && _stepStack[i].length == -1)
            {
                _stepStack[i].trackNum = track;
                _stepStack[i].stepNum = step;
                _stepStack[i].length = lenStepModEnabled ? lenStepMod : trackToUse.length;

                handleNoteOnForTrackStep(_stepStack[i].trackNum, _stepStack[i].stepNum);

                return;
            }
        }
    }

    void handleNoteOnForTrackStep(int track, int step)
    {
        auto trackToUse = getHeapTrack(track);
        if (trackToUse.muted)
        {
            return;
        }

        if (_trkNeedsInit[track])
        {
            Serial.print("init sounds for track: ");
            Serial.println(track);

            // XRSound::initSoundsForTrack(track);
        }

        Serial.println("TODO: impl noteOn for track step!");

        if (trackToUse.track_type == RAW_SAMPLE)
        {
            // XRSound::handleRawSampleNoteOnForTrackStep(track, step);
        }
        else if (trackToUse.track_type == WAV_SAMPLE)
        {
            // XRSound::handleWavSampleNoteOnForTrackStep(track, step);
        }
        else if (trackToUse.track_type == DEXED)
        {
            // XRSound::handleDexedNoteOnForTrackStep(track, step);
        }
        else if (trackToUse.track_type == SUBTRACTIVE_SYNTH)
        {
            // XRSound::handleSubtractiveSynthNoteOnForTrackStep(track, step);
        }
        else if (trackToUse.track_type == MIDI_OUT)
        {
            // XRSound::handleMIDINoteOnForTrackStep(track, step);
        }
        else if (trackToUse.track_type == CV_GATE)
        {
            // XRSound::handleCvGateNoteOnForTrackStep(track, step);
        }
    }

    void updateCurrentPatternStepState()
    {
        auto currPattern = getHeapCurrentSelectedPattern();

        int currPatternLastStep = currPattern.last_step;

        if (_seqState.currentStep <= currPatternLastStep)
        {
            if (_seqState.currentStep < currPatternLastStep)
            {
                if (!((_seqState.currentStep + 1) % 16))
                { // TODO: make this bar division configurable
                    ++_seqState.currentBar;
                }

                ++_seqState.currentStep; // advance current step for sequencer
            }
            else
            {
                _seqState.currentStep = 1; // reset current step
                _seqState.currentBar = 1;  // reset current bar

                if (_queuedPattern.bank > -1 && _queuedPattern.number > -1)
                {
                    // nullify pattern queue
                    _dequeuePattern = true;
                }
            }
        }
    }

    void updateAllTrackStepStates()
    {
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            auto track = _seqHeap.pattern.tracks[t];
            int trackLastStep = track.last_step;
            int8_t trackCurrentStep = _seqState.currentTrackSteps[t].currentStep;

            if (trackCurrentStep <= trackLastStep)
            {
                if (trackCurrentStep < trackLastStep)
                {
                    if (!((trackCurrentStep + 1) % 16))
                    { // TODO: make this bar division configurable
                        ++_seqState.currentTrackSteps[t].currentBar;
                    }

                    ++_seqState.currentTrackSteps[t].currentStep; // advance current step for track
                }
                else
                {
                    _seqState.currentTrackSteps[t].currentStep = 1; // reset current step for track
                    _seqState.currentTrackSteps[t].currentBar = 1;  // reset current bar for track
                }
            }
        }
    }

    void initExternalSequencer()
    {
        for (int b = 0; b < MAXIMUM_SEQUENCER_BANKS; b++)
        {
            for (int p = 0; p < MAXIMUM_SEQUENCER_PATTERNS; p++)
            {
                _seqExternall.banks[b].patterns[p].last_step = DEFAULT_LAST_STEP;
                _seqExternall.banks[b].patterns[p].initialized = false;
                _seqExternall.banks[b].patterns[p].groove_amount = 0;
                _seqExternall.banks[b].patterns[p].groove_id = -1;

                if (p == 0)
                    _seqExternall.banks[b].patterns[p].initialized = true;

                for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
                {
                    _seqExternall.banks[b].patterns[p].tracks[t].track_type = RAW_SAMPLE;
                    _seqExternall.banks[b].patterns[p].tracks[t].raw_sample_id = 0;
                    _seqExternall.banks[b].patterns[p].tracks[t].wav_sample_id = 0;
                    _seqExternall.banks[b].patterns[p].tracks[t].waveform = XRSound::WAVEFORM_TYPE::SAW;
                    _seqExternall.banks[b].patterns[p].tracks[t].last_step = DEFAULT_LAST_STEP;
                    _seqExternall.banks[b].patterns[p].tracks[t].length = 4;
                    _seqExternall.banks[b].patterns[p].tracks[t].note = 0;
                    _seqExternall.banks[b].patterns[p].tracks[t].octave = 4;
                    _seqExternall.banks[b].patterns[p].tracks[t].detune = 0;
                    _seqExternall.banks[b].patterns[p].tracks[t].fine = 0;
                    _seqExternall.banks[b].patterns[p].tracks[t].velocity = 50;
                    _seqExternall.banks[b].patterns[p].tracks[t].microtiming = 0;
                    _seqExternall.banks[b].patterns[p].tracks[t].probability = 100;
                    _seqExternall.banks[b].patterns[p].tracks[t].bitrate = 16;
                    _seqExternall.banks[b].patterns[p].tracks[t].channel = 1;
                    _seqExternall.banks[b].patterns[p].tracks[t].looptype = 0;
                    _seqExternall.banks[b].patterns[p].tracks[t].loopstart = 0;
                    _seqExternall.banks[b].patterns[p].tracks[t].loopfinish = 5000;
                    _seqExternall.banks[b].patterns[p].tracks[t].playstart = play_start_sample;
                    _seqExternall.banks[b].patterns[p].tracks[t].level = 0.7;
                    _seqExternall.banks[b].patterns[p].tracks[t].pan = 0;
                    _seqExternall.banks[b].patterns[p].tracks[t].sample_play_rate = 1.0;
                    _seqExternall.banks[b].patterns[p].tracks[t].width = 0.5;
                    _seqExternall.banks[b].patterns[p].tracks[t].oscalevel = 1;
                    _seqExternall.banks[b].patterns[p].tracks[t].oscblevel = 0.5;
                    _seqExternall.banks[b].patterns[p].tracks[t].cutoff = 1600;
                    _seqExternall.banks[b].patterns[p].tracks[t].res = 0;
                    _seqExternall.banks[b].patterns[p].tracks[t].filterenvamt = 1.0;
                    _seqExternall.banks[b].patterns[p].tracks[t].filter_attack = 0;
                    _seqExternall.banks[b].patterns[p].tracks[t].filter_decay = 1000;
                    _seqExternall.banks[b].patterns[p].tracks[t].filter_sustain = 1.0;
                    _seqExternall.banks[b].patterns[p].tracks[t].filter_release = 5000;
                    _seqExternall.banks[b].patterns[p].tracks[t].amp_attack = 0;
                    _seqExternall.banks[b].patterns[p].tracks[t].amp_decay = 1000;
                    _seqExternall.banks[b].patterns[p].tracks[t].amp_sustain = 1.0;
                    _seqExternall.banks[b].patterns[p].tracks[t].amp_release = 5000;
                    _seqExternall.banks[b].patterns[p].tracks[t].noise = 0;
                    _seqExternall.banks[b].patterns[p].tracks[t].chromatic_enabled = false;
                    _seqExternall.banks[b].patterns[p].tracks[t].muted = false;
                    _seqExternall.banks[b].patterns[p].tracks[t].soloing = false;
                    _seqExternall.banks[b].patterns[p].tracks[t].initialized = false;

                    if (t == 0)
                        _seqExternall.banks[b].patterns[p].tracks[t].initialized = true;

                    // now fill in steps
                    for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
                    {
                        _seqExternall.banks[b].patterns[p].tracks[t].steps[s].length = 4;
                        _seqExternall.banks[b].patterns[p].tracks[t].steps[s].note = 0;
                        _seqExternall.banks[b].patterns[p].tracks[t].steps[s].octave = 4;
                        _seqExternall.banks[b].patterns[p].tracks[t].steps[s].velocity = 50;
                        _seqExternall.banks[b].patterns[p].tracks[t].steps[s].microtiming = 0;
                        _seqExternall.banks[b].patterns[p].tracks[t].steps[s].probability = 100;

                        _seqExternall.banks[b].patterns[p].tracks[t].steps[s].state = TRACK_STEP_STATE::STATE_OFF;
                    }
                }
            }
        }
    }

    void handleQueueActions()
    {
        if (_drawQueueBlink > -1)
        {
            if (_drawQueueBlink == 1)
            {
                XRDisplay::drawSequencerScreen(true);
            }
            else if (_drawQueueBlink == 0)
            {
                XRDisplay::drawSequencerScreen(false);
            }
        }

        if (_dequeuePattern)
        {
            _dequeuePattern = false;

            swapSequencerMemoryForPattern(_queuedPattern.bank, _queuedPattern.number);

            // reset queue flags
            _queuedPattern.bank = -1;
            _queuedPattern.number = -1;
            _drawQueueBlink = -1;

            _currentStepPage = 1;
            _currentSelectedPage = 1;

            auto currentUXMode = XRUX::getCurrentMode();
            auto currentPattern = getHeapCurrentSelectedPattern();

            if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE)
            {
                XRLED::clearAllStepLEDs();
            }
            else if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE)
            {
                XRLED::displayPageLEDs(
                    1,
                    (_seqState.playbackState == RUNNING),
                    _currentStepPage,
                    currentPattern.last_step // TODO: use pattern OR track last step depending which UX mode is active
                );                           // TODO need?
            }

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void swapSequencerMemoryForPattern(int newBank, int newPattern)
    {
        // AudioNoInterrupts();
        auto currPatternData = _seqHeap.pattern;
        auto newPatternData = _seqExternall.banks[newBank].patterns[newPattern];

        // save any mods for current pattern to SD
        XRSD::savePatternModsToSdCard();

        // swap memory data
        _seqExternall.banks[_currentSelectedBank].patterns[_currentSelectedPattern] = currPatternData;
        _seqHeap.pattern = newPatternData;

        // initialize new pattern
        _seqHeap.pattern.initialized = true;

        // update currently selected vars
        _currentSelectedBank = newBank;
        _currentSelectedPattern = newPattern;

        // load any mods for new bank/pattern to SD
        XRSD::loadPatternModsFromSdCard();

        // temp: trying this approach ^
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            _trkNeedsInit[t] = true;
        }
    }

    void setSelectedTrack(int8_t track)
    {
        _currentSelectedTrack = track;
    }

    void setSelectedPage(int8_t page)
    {
        _currentSelectedPage = page;
    }

    void toggleSelectedStep(uint8_t step)
    {
        int stepToUse = step;

        // todo: check if current track has last_step > 16
        // if so, use proper offset to get correct step state for current page
        if (_currentStepPage == 2)
        {
            stepToUse += 16;
        }
        else if (_currentStepPage == 3)
        {
            stepToUse += 32;
        }
        else if (_currentStepPage == 4)
        {
            stepToUse += 48;
        }

        uint8_t adjStep = stepToUse - 1; // get zero based step num

        Serial.print("adjStep: ");
        Serial.println(adjStep);

        TRACK currTrack = getHeapCurrentSelectedTrack();
        TRACK_STEP_STATE currStepState = currTrack.steps[adjStep].state;

        Serial.print("currStepState: ");
        Serial.println(currStepState == TRACK_STEP_STATE::STATE_ACCENTED ? "accented" : (currStepState == TRACK_STEP_STATE::STATE_ON ? "on" : "off"));

        // TODO: implement accent state for MIDI, CV/Trig, Sample, Synth track types?
        if (currStepState == TRACK_STEP_STATE::STATE_OFF)
        {
            _seqHeap.pattern.tracks[_currentSelectedTrack].steps[adjStep].state = TRACK_STEP_STATE::STATE_ON;
            // copy track properties to steps
            _seqHeap.pattern.tracks[_currentSelectedTrack].steps[adjStep].note = currTrack.note;
            _seqHeap.pattern.tracks[_currentSelectedTrack].steps[adjStep].octave = currTrack.octave;
            _seqHeap.pattern.tracks[_currentSelectedTrack].steps[adjStep].velocity = currTrack.velocity;
        }
        else if (currStepState == TRACK_STEP_STATE::STATE_ON)
        {
            _seqHeap.pattern.tracks[_currentSelectedTrack].steps[adjStep].state = TRACK_STEP_STATE::STATE_ACCENTED;
            _seqHeap.pattern.tracks[_currentSelectedTrack].steps[adjStep].velocity = 100; // TODO: use a "global accent" value here
        }
        else if (currStepState == TRACK_STEP_STATE::STATE_ACCENTED)
        {
            _seqHeap.pattern.tracks[_currentSelectedTrack].steps[adjStep].state = TRACK_STEP_STATE::STATE_OFF;
        }

        _seqExternall.banks[_currentSelectedBank].patterns[_currentSelectedPattern].tracks[_currentSelectedTrack].steps[adjStep] = _seqHeap.pattern.tracks[_currentSelectedTrack].steps[adjStep];
    }

    void toggleSequencerPlayback(char btn)
    {
        auto currentUXMode = XRUX::getCurrentMode();

        int8_t currStepChar = XRHelpers::stepCharMap[_seqState.currentStep - 1]; // TODO change type to char?
        uint8_t keyLED = XRLED::getKeyLED(currStepChar);

        if (_seqState.playbackState > STOPPED)
        {
            if (_seqState.playbackState == RUNNING && btn == 'q')
            {
                _seqState.playbackState = PAUSED;

                XRClock::pause();
                // uClock.pause();

                XRLED::setPWMDouble(23, 0, keyLED, 0);

                int8_t currentSelectedTrackCurrentBar = _seqState.currentTrackSteps[_currentSelectedTrack].currentBar;

                auto currTrack = getHeapCurrentSelectedTrack();

                XRLED::displayPageLEDs(
                    currentSelectedTrackCurrentBar,
                    (_seqState.playbackState == RUNNING),
                    _currentStepPage,
                    currTrack.last_step);

                if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE)
                {
                    XRLED::setDisplayStateForAllStepLEDs(); // TODO: wrap with "if in track view display step LED state" conditional, etc
                }
                else if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE)
                {
                    XRLED::clearAllStepLEDs();
                }
            }
            else if (_seqState.playbackState == PAUSED && btn == 'q')
            {
                // Unpaused, so advance sequencer from last known step
                _seqState.playbackState = RUNNING;

                XRClock::pause();
                // uClock.pause();
            }
            else if (btn == 'w')
            {
                // Stopped, so reset sequencer to FIRST step in pattern
                _seqState.currentStep = 1;
                _seqState.currentBar = 1;

                rewindAllCurrentStepsForAllTracks();

                _seqState.playbackState = STOPPED;

                XRClock::stop();
                // uClock.stop();

                XRLED::setPWM(keyLED, 0); // turn off current step LED
                XRLED::setPWM(23, 0);     // turn start button led OFF

                if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE)
                {
                    XRLED::setDisplayStateForAllStepLEDs(); // TODO: wrap with "if in track view display step LED state" conditional, etc
                }
                else if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE)
                {
                    XRLED::clearAllStepLEDs();
                }
            }
        }
        else if (btn == 'q')
        {
            // Started, so start sequencer from FIRST step in pattern
            //_seqState.current_step = 1;
            _seqState.playbackState = RUNNING;

            XRClock::start();
            // uClock.start();
        }
        else if (btn == 'w')
        {
            // Stopped, so reset sequencer to FIRST step in pattern
            _seqState.currentStep = 1;
            _seqState.currentBar = 1;

            rewindAllCurrentStepsForAllTracks();

            _seqState.playbackState = STOPPED;

            XRClock::stop();
            // uClock.stop();

            // XRLED::setPWM(keyLED, 0); // turn off current step LED
            XRLED::setPWM(23, 0); // turn start button led OFF

            if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE)
            {
                XRLED::setDisplayStateForAllStepLEDs(); // TODO: wrap with "if in track view display step LED state" conditional, etc
            }
            else if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE)
            {
                XRLED::clearAllStepLEDs();
            }
        }
    }

    void rewindAllCurrentStepsForAllTracks()
    {
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            _seqState.currentTrackSteps[t].currentStep = 1;
        }
    }

    void triggerRatchetingTrack(uint32_t tick)
    {
        if (_ratchetTrack == -1)
        {
            return;
        }

        handleRemoveFromRatchetStack();

        if (_ratchetDivision > -1 && !(tick % _ratchetDivision))
        {
            Serial.print("in ratchet division! tick: ");
            Serial.print(tick);
            Serial.print(" ratchet_division: ");
            Serial.println(_ratchetDivision);

            handleAddToRatchetStack();
        }
    }

    void handleRemoveFromRatchetStack()
    {
        for (int i = 0; i < STEP_STACK_SIZE; i++)
        {
            if (_ratchetStack[i].length != -1)
            {
                --_ratchetStack[i].length;

                if (_ratchetStack[i].length == 0)
                {
                    handleNoteOffForTrack(_ratchetStack[i].trackNum);

                    // re-initialize stack entry
                    _ratchetStack[i].trackNum = -1;
                    _ratchetStack[i].length = -1;
                }
            }
        }
    }

    void handleAddToRatchetStack()
    {
        for (uint8_t i = 0; i < STEP_STACK_SIZE; i++)
        {
            if (_ratchetStack[i].length == -1)
            {
                _ratchetStack[i].trackNum = _ratchetTrack;
                _ratchetStack[i].length = 2; // TODO: shorten?

                handleNoteOnForTrack(_ratchetStack[i].trackNum);

                return;
            }
        }
    }

    void handleNoteOnForTrack(int track)
    {
        auto trackToUse = getHeapTrack(track);

        Serial.println("TODO: impl handleNoteOnForTrack() !");

        // if (trackToUse.track_type == RAW_SAMPLE)
        // {
        //     handleRawSampleNoteOnForTrack(track);
        // }
        // else if (trackToUse.track_type == WAV_SAMPLE)
        // {
        //     handleWavSampleNoteOnForTrack(track);
        // }
        // else if (trackToUse.track_type == DEXED)
        // {
        //     handleDexedNoteOnForTrack(track);
        // }
        // else if (trackToUse.track_type == SUBTRACTIVE_SYNTH)
        // {
        //     handleSubtractiveSynthNoteOnForTrack(track);
        // }
        // else if (trackToUse.track_type == CV_GATE)
        // {
        //     handleCvGateNoteOnForTrack(track);
        // }
    }

    void handleNoteOffForTrack(int track)
    {
        auto currTrack = getHeapTrack(track);

        Serial.println("TODO: impl handleNoteOffForTrack() !");

        // if (currTrack.track_type == SUBTRACTIVE_SYNTH)
        // {
        //     comboVoices[track].ampEnv.noteOff();
        //     comboVoices[track].filterEnv.noteOff();
        // }

        // else if (currTrack.track_type == DEXED)
        // {
        //     uint8_t noteToUse = currTrack.note;
        //     uint8_t octaveToUse = currTrack.octave;

        //     int midiNote = (noteToUse + (12 * (octaveToUse))); // use offset of 32 instead?

        //     if (track < 4)
        //     {
        //         comboVoices[track].dexed.keyup(midiNote);
        //     }
        // }

        // // fix
        // else if (currTrack.track_type == MIDI_OUT)
        // {
        //     MIDI.sendNoteOff(64, 100, 1);
        // }
        // else if (currTrack.track_type == CV_GATE)
        // {
        //     if (currTrack.channel == 1)
        //     {
        //         // writeToDAC(CS1, 0, cvLevels[midiNote]); // cv
        //         writeToDAC(CS1, 1, 0); // gate
        //     }
        //     else if (currTrack.channel == 2)
        //     {
        //         // writeToDAC(CS2, 0, cvLevels[midiNote]); // cv
        //         writeToDAC(CS2, 1, 0); // gate
        //     }
        //     else if (currTrack.channel == 3)
        //     {
        //         // writeToDAC(CS3, 0, cvLevels[midiNote]); // cv
        //         writeToDAC(CS3, 1, 0); // gate
        //     }
        //     else if (currTrack.channel == 4)
        //     {
        //         // writeToDAC(CS4, 0, cvLevels[midiNote]); // cv
        //         writeToDAC(CS4, 1, 0); // gate
        //     }
        // }

        // else
        // {
        //     if (track > 3)
        //     {
        //         int tOffset = track - 4;
        //         sampleVoices[tOffset].ampEnv.noteOff();
        //     }
        //     else
        //     {
        //         comboVoices[track].ampEnv.noteOff();
        //     }
        // }
    }

    void initializeCurrentSelectedTrack()
    {
        _seqHeap.pattern.tracks[_currentSelectedTrack].initialized = true;
    }
}