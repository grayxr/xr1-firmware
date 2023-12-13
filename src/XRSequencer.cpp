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
    DMAMEM SEQUENCER_EXTERNAL _seqExternal;
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
        false, false, false, false
    };

    bool _dequeuePattern = false;
    bool _queueBlinked = false;
    bool _recording = false;

    int _drawQueueBlink = -1;

    uint8_t _bpmBlinkTimer = 2;

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

        // every 1/4 step log memory usage
        if (!(tick % 24)) {
            //logMetrics();

            // blink queued bank / pattern
            if (_queuedPattern.bank > -1 && _queuedPattern.number > -1) {
                _drawQueueBlink = 1;
            }
        }

        if (_recording) {
            XRLED::setPWM(23, 512);
            //setLEDPWM(23, 1024);
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
        // Serial.println("enter handleNoteOffForTrackStep!");

        //return;

        XRSound::handleNoteOffForTrackStep(track, step);
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

    SEQUENCER_STATE &getSeqState()
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

    QUEUED_PATTERN &getQueuedPattern()
    {
        return _queuedPattern;
    }

    SEQUENCER_HEAP &getSequencerHeap()
    {
        return _seqHeap;
    }

    SEQUENCER_EXTERNAL &getSequencerExternal()
    {
        return _seqExternal;
    }

    TRACK &getHeapTrack(int track)
    {
        return _seqHeap.pattern.tracks[track];
    }

    TRACK_STEP &getHeapStep(int track, int step)
    {
        return _seqHeap.pattern.tracks[track].steps[step];
    }

    PATTERN &getHeapCurrentSelectedPattern()
    {
        return _seqHeap.pattern;
    }

    TRACK &getHeapCurrentSelectedTrack()
    {
        return _seqHeap.pattern.tracks[_currentSelectedTrack];
    }

    TRACK_STEP &getHeapCurrentSelectedTrackStep()
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

    PATTERN_MODS &getModsForCurrentPattern()
    {
        return _patternMods;
    }

    TRACK_STEP_MODS &getModsForCurrentTrackStep()
    {
        return _patternMods.tracks[_currentSelectedTrack].steps[_currentSelectedStep];
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
        // Serial.println("enter handleNoteOnForTrackStep!");

        // return;

        auto trackToUse = getHeapTrack(track);
        if (trackToUse.muted)
        {
            return;
        }

        if (_trkNeedsInit[track])
        {
            Serial.print("init sounds for track: ");
            Serial.println(track);

            XRSound::initSoundsForTrack(track);
        }

        if (trackToUse.track_type == RAW_SAMPLE)
        {
            XRSound::handleRawSampleNoteOnForTrackStep(track, step);
        }
        else if (trackToUse.track_type == WAV_SAMPLE)
        {
            XRSound::handleWavSampleNoteOnForTrackStep(track, step);
        }
        else if (trackToUse.track_type == DEXED)
        {
            XRSound::handleDexedNoteOnForTrackStep(track, step);
        }
        else if (trackToUse.track_type == SUBTRACTIVE_SYNTH)
        {
            XRSound::handleSubtractiveSynthNoteOnForTrackStep(track, step);
        }
        else if (trackToUse.track_type == MIDI_OUT)
        {
            XRSound::handleMIDINoteOnForTrackStep(track, step);
        }
        else if (trackToUse.track_type == CV_GATE)
        {
            XRSound::handleCvGateNoteOnForTrackStep(track, step);
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
                _seqExternal.banks[b].patterns[p].last_step = DEFAULT_LAST_STEP;
                _seqExternal.banks[b].patterns[p].initialized = false;
                _seqExternal.banks[b].patterns[p].groove_amount = 0;
                _seqExternal.banks[b].patterns[p].groove_id = -1;

                if (p == 0)
                    _seqExternal.banks[b].patterns[p].initialized = true;

                for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
                {
                    _seqExternal.banks[b].patterns[p].tracks[t].track_type = RAW_SAMPLE;
                    //_seqExternal.banks[b].patterns[p].tracks[t].sample_name;
                    strcpy(_seqExternal.banks[b].patterns[p].tracks[t].sample_name, "");
                    _seqExternal.banks[b].patterns[p].tracks[t].waveform = XRSound::WAVEFORM_TYPE::SAW;
                    _seqExternal.banks[b].patterns[p].tracks[t].last_step = DEFAULT_LAST_STEP;
                    _seqExternal.banks[b].patterns[p].tracks[t].length = 4;
                    _seqExternal.banks[b].patterns[p].tracks[t].note = 0;
                    _seqExternal.banks[b].patterns[p].tracks[t].octave = 4;
                    _seqExternal.banks[b].patterns[p].tracks[t].detune = 0;
                    _seqExternal.banks[b].patterns[p].tracks[t].fine = 0;
                    _seqExternal.banks[b].patterns[p].tracks[t].velocity = 50;
                    _seqExternal.banks[b].patterns[p].tracks[t].microtiming = 0;
                    _seqExternal.banks[b].patterns[p].tracks[t].probability = 100;
                    _seqExternal.banks[b].patterns[p].tracks[t].bitrate = 16;
                    _seqExternal.banks[b].patterns[p].tracks[t].channel = 1;
                    _seqExternal.banks[b].patterns[p].tracks[t].looptype = 0;
                    _seqExternal.banks[b].patterns[p].tracks[t].loopstart = 0;
                    _seqExternal.banks[b].patterns[p].tracks[t].loopfinish = 5000;
                    _seqExternal.banks[b].patterns[p].tracks[t].playstart = play_start_sample;
                    _seqExternal.banks[b].patterns[p].tracks[t].level = 0.7;
                    _seqExternal.banks[b].patterns[p].tracks[t].pan = 0;
                    _seqExternal.banks[b].patterns[p].tracks[t].sample_play_rate = 1.0;
                    _seqExternal.banks[b].patterns[p].tracks[t].width = 0.5;
                    _seqExternal.banks[b].patterns[p].tracks[t].oscalevel = 1;
                    _seqExternal.banks[b].patterns[p].tracks[t].oscblevel = 0.5;
                    _seqExternal.banks[b].patterns[p].tracks[t].cutoff = 1600;
                    _seqExternal.banks[b].patterns[p].tracks[t].res = 0;
                    _seqExternal.banks[b].patterns[p].tracks[t].filterenvamt = 1.0;
                    _seqExternal.banks[b].patterns[p].tracks[t].filter_attack = 0;
                    _seqExternal.banks[b].patterns[p].tracks[t].filter_decay = 1000;
                    _seqExternal.banks[b].patterns[p].tracks[t].filter_sustain = 1.0;
                    _seqExternal.banks[b].patterns[p].tracks[t].filter_release = 5000;
                    _seqExternal.banks[b].patterns[p].tracks[t].amp_attack = 0;
                    _seqExternal.banks[b].patterns[p].tracks[t].amp_decay = 1000;
                    _seqExternal.banks[b].patterns[p].tracks[t].amp_sustain = 1.0;
                    _seqExternal.banks[b].patterns[p].tracks[t].amp_release = 5000;
                    _seqExternal.banks[b].patterns[p].tracks[t].noise = 0;
                    _seqExternal.banks[b].patterns[p].tracks[t].chromatic_enabled = false;
                    _seqExternal.banks[b].patterns[p].tracks[t].muted = false;
                    _seqExternal.banks[b].patterns[p].tracks[t].soloing = false;
                    _seqExternal.banks[b].patterns[p].tracks[t].initialized = false;

                    if (t == 0)
                        _seqExternal.banks[b].patterns[p].tracks[t].initialized = true;

                    // now fill in steps
                    for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
                    {
                        _seqExternal.banks[b].patterns[p].tracks[t].steps[s].length = 4;
                        _seqExternal.banks[b].patterns[p].tracks[t].steps[s].note = 0;
                        _seqExternal.banks[b].patterns[p].tracks[t].steps[s].octave = 4;
                        _seqExternal.banks[b].patterns[p].tracks[t].steps[s].velocity = 50;
                        _seqExternal.banks[b].patterns[p].tracks[t].steps[s].microtiming = 0;
                        _seqExternal.banks[b].patterns[p].tracks[t].steps[s].probability = 100;

                        _seqExternal.banks[b].patterns[p].tracks[t].steps[s].state = TRACK_STEP_STATE::STATE_OFF;
                    }
                }
            }
        }
    }

    void initExternalPatternMods()
    {
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            // now fill in mod flags
            for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
            {
                for (int f = 0; f < MAX_STEP_MOD_ATTRS; f++)
                {
                    _patternMods.tracks[t].step_mod_flags[s].flags[f] = false;
                }
            }

            // now fill in mods
            for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
            {
                //_patternMods.tracks[t].steps[s].sample_name;
                strcpy(_patternMods.tracks[t].steps[s].sample_name, "");
                _patternMods.tracks[t].steps[s].waveform = XRSound::WAVEFORM_TYPE::SAW;
                _patternMods.tracks[t].steps[s].detune = 0;
                _patternMods.tracks[t].steps[s].fine = 0;
                _patternMods.tracks[t].steps[s].looptype = 0;
                _patternMods.tracks[t].steps[s].loopstart = 0;
                _patternMods.tracks[t].steps[s].loopfinish = 5000;
                _patternMods.tracks[t].steps[s].playstart = play_start_sample;
                _patternMods.tracks[t].steps[s].level = 0.7;
                _patternMods.tracks[t].steps[s].pan = 0;
                _patternMods.tracks[t].steps[s].sample_play_rate = 1.0;
                _patternMods.tracks[t].steps[s].width = 0.5;
                _patternMods.tracks[t].steps[s].oscalevel = 1.0;
                _patternMods.tracks[t].steps[s].oscblevel = 0.5;
                _patternMods.tracks[t].steps[s].cutoff = 1600;
                _patternMods.tracks[t].steps[s].res = 0;
                _patternMods.tracks[t].steps[s].filterenvamt = 1.0;
                _patternMods.tracks[t].steps[s].filter_attack = 0;
                _patternMods.tracks[t].steps[s].filter_decay = 1000;
                _patternMods.tracks[t].steps[s].filter_sustain = 1.0;
                _patternMods.tracks[t].steps[s].filter_release = 5000;
                _patternMods.tracks[t].steps[s].amp_attack = 0;
                _patternMods.tracks[t].steps[s].amp_decay = 1000;
                _patternMods.tracks[t].steps[s].amp_sustain = 1.0;
                _patternMods.tracks[t].steps[s].amp_release = 5000;
                _patternMods.tracks[t].steps[s].noise = 0;
            }
        }
    }

    void handleQueueActions()
    {
        if (_drawQueueBlink > -1)
        {
            //if (!_queueBlinked && _drawQueueBlink == 1)
            if (_drawQueueBlink == 1)
            {
                XRDisplay::drawSequencerScreen(true);
                //_queueBlinked = true;
            }
            //else if (_queueBlinked && _drawQueueBlink == 0)
            else if (_drawQueueBlink == 0)
            {
                XRDisplay::drawSequencerScreen(false);
                //_queueBlinked = false;
            }
        }

        if (_dequeuePattern)
        {
            Serial.println("enter _dequeuePattern!");

            _dequeuePattern = false;

            swapSequencerMemoryForPattern(_queuedPattern.bank, _queuedPattern.number);
            Serial.println("finished swapping seq mem!");

            // clearing samples manually not working
            // XRSound::clearSamples();
            // Serial.println("finished clearing samples!");

            // reset queue flags
            _queuedPattern.bank = -1;
            _queuedPattern.number = -1;
            _drawQueueBlink = -1;
            //_queueBlinked = false;

            _currentStepPage = 1;
            _currentSelectedPage = 0;
            //_currentSelectedTrack = 0;

            auto currentUXMode = XRUX::getCurrentMode();
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
                    _seqHeap.pattern.last_step // TODO: use pattern OR track last step depending which UX mode is active
                );                           // TODO need?
            }

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void swapSequencerMemoryForPattern(int newBank, int newPattern)
    {
        // AudioNoInterrupts();
        auto currPatternData = _seqHeap.pattern;
        auto newPatternData = _seqExternal.banks[newBank].patterns[newPattern];

        // save any mods for current pattern to SD
        XRSD::savePatternModsToSdCard();

        // swap memory data
        _seqExternal.banks[_currentSelectedBank].patterns[_currentSelectedPattern] = currPatternData;
        _seqHeap.pattern = newPatternData;

        // initialize new pattern
        _seqHeap.pattern.initialized = true;

        // update currently selected vars
        _currentSelectedBank = newBank;
        _currentSelectedPattern = newPattern;
        _currentSelectedTrack = 0;

        // load any mods for new bank/pattern from SD
        XRSD::loadPatternModsFromSdCard();

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

        _seqExternal.banks[_currentSelectedBank].patterns[_currentSelectedPattern].tracks[_currentSelectedTrack].steps[adjStep] = _seqHeap.pattern.tracks[_currentSelectedTrack].steps[adjStep];
    }

    void toggleSequencerPlayback(char btn)
    {
        Serial.println("enter toggleSequencerPlayback!");

        auto currentUXMode = XRUX::getCurrentMode();

        int8_t currStepChar = XRHelpers::stepCharMap[_seqState.currentStep - 1]; // TODO change type to char?
        uint8_t keyLED = XRLED::getKeyLED(currStepChar);

        if (_seqState.playbackState > STOPPED) {
            if (_seqState.playbackState == RUNNING && btn == START_BTN_CHAR) {
                _seqState.playbackState = PAUSED;

                XRClock::pause();

                XRLED::setPWMDouble(23, 0, keyLED, 0);

                int8_t currentSelectedTrackCurrentBar = _seqState.currentTrackSteps[_currentSelectedTrack].currentBar;

                auto currTrack = getHeapCurrentSelectedTrack();

                XRLED::displayPageLEDs(
                    currentSelectedTrackCurrentBar,
                    (_seqState.playbackState == RUNNING),
                    _currentStepPage,
                    currTrack.last_step
                );

                if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE) {
                    XRLED::setDisplayStateForAllStepLEDs(); // TODO: wrap with "if in track view display step LED state" conditional, etc
                } else if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE) {
                    XRLED::clearAllStepLEDs();
                }
            } else if (_seqState.playbackState == PAUSED && btn == START_BTN_CHAR) {
                // Unpaused, so advance sequencer from last known step
                _seqState.playbackState = RUNNING;

                XRClock::pause();
            } else if (btn == 'w') {
                // Stopped, so reset sequencer to FIRST step in pattern
                _seqState.currentStep = 1;
                _seqState.currentBar = 1;

                rewindAllCurrentStepsForAllTracks();

                _seqState.playbackState = STOPPED;

                XRClock::stop();

                XRLED::setPWM(keyLED, 0); // turn off current step LED
                XRLED::setPWM(23, 0);     // turn start button led OFF

                if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE) {
                    XRLED::setDisplayStateForAllStepLEDs(); // TODO: wrap with "if in track view display step LED state" conditional, etc
                } else if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE) {
                    XRLED::clearAllStepLEDs();
                }
            }
        } else if (btn == START_BTN_CHAR) {
            // Started, so start sequencer from FIRST step in pattern
            //_seqState.current_step = 1;
            _seqState.playbackState = RUNNING;

            XRClock::start();
            // uClock.start();
        } else if (btn == 'w') {
            // Stopped, so reset sequencer to FIRST step in pattern
            _seqState.currentStep = 1;
            _seqState.currentBar = 1;

            rewindAllCurrentStepsForAllTracks();

            _seqState.playbackState = STOPPED;

            XRClock::stop();

            XRLED::setPWM(23, 0); // turn start button led OFF

            if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE) {
                XRLED::setDisplayStateForAllStepLEDs(); // TODO: wrap with "if in track view display step LED state" conditional, etc
            } else if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE) {
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

        if (trackToUse.track_type == RAW_SAMPLE)
        {
            XRSound::handleRawSampleNoteOnForTrack(track);
        }
        else if (trackToUse.track_type == WAV_SAMPLE)
        {
            XRSound::handleWavSampleNoteOnForTrack(track);
        }
        else if (trackToUse.track_type == DEXED)
        {
            XRSound::handleDexedNoteOnForTrack(track);
        }
        else if (trackToUse.track_type == SUBTRACTIVE_SYNTH)
        {
            XRSound::handleSubtractiveSynthNoteOnForTrack(track);
        }
        else if (trackToUse.track_type == CV_GATE)
        {
            XRSound::handleCvGateNoteOnForTrack(track);
        }
    }

    void handleNoteOffForTrack(int track)
    {
        XRSound::handleNoteOffForTrack(track);
    }

    void initializeCurrentSelectedTrack()
    {
        _seqHeap.pattern.tracks[_currentSelectedTrack].initialized = true;
    }


    void setTrackTypeForHeapTrack(int8_t track, TRACK_TYPE type)
    {
        _seqHeap.pattern.tracks[track].track_type = type;
    }

    void setTrackNeedsInit(int track, bool init)
    {
        _trkNeedsInit[track] = init;
    }
    
    void assignSampleNameToTrack(std::string sampleName)
    {
        strcpy(_seqHeap.pattern.tracks[_currentSelectedTrack].sample_name, sampleName.c_str());
    }

    void queuePattern(int pattern, int bank)
    {
        _queuedPattern.bank = bank;
        _queuedPattern.number = pattern;
    }

    void setSelectedPattern(int8_t pattern)
    {
        _currentSelectedPattern = pattern;
    }

    void saveCurrentPatternOffHeap()
    {
        // push current heap memory to RAM2/DMAMEM
        _seqExternal.banks[_currentSelectedBank].patterns[_currentSelectedPattern] = _seqHeap.pattern;
    }

    void setCurrentSelectedStep(int step)
    {
       // _seqHeap.pattern.tracks[_currentSelectedTrack].steps[step];

        _currentSelectedStep = step;
    }
}