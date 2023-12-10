#include <XRSequencer.h>
#include <XRDisplay.h>
#include <XRUX.h>
#include <XRKeyMatrix.h>
#include <XRLED.h>
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

    std::map<int8_t, char> _stepCharMap = {
        {1, 'm'},
        {2, 'n'},
        {3, 'o'},
        {4, 'p'},
        {5, 's'},
        {6, 't'},
        {7, 'u'},
        {8, 'v'},
        {9, 'y'},
        {10, 'z'},
        {11, '1'},
        {12, '2'},
        {13, '5'},
        {14, '6'},
        {15, '7'},
        {16, '8'},
    };

    std::map<TRACK_TYPE, int> trackPageNumMap = {
        { SUBTRACTIVE_SYNTH, 6},
        { DEXED, 1},
        { RAW_SAMPLE, 4},
        { WAV_SAMPLE, 1},
        { MIDI_OUT, 1},
        { CV_GATE, 1},
        { CV_TRIG, 1},
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
    int drawQueueBlink = -1;

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

        //usbMIDI.sendRealTime(usbMIDI.Stop);

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
        //usbMIDI.sendRealTime(usbMIDI.Clock);
    }

    void handle16PPQN(uint32_t tick)
    {
        auto currentUXMode = XRUX::getCurrentMode();
        auto seqState = getSeqState();
        auto currentSelectedTrack = getCurrentSelectedTrackNum();
        auto currentStepPage = getCurrentStepPageNum();

        bool functionActive = XRKeyMatrix::isFunctionActive();

        //int8_t currentSelectedPatternCurrentStep = seqState.currentStep;
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
                currTrack.last_step
            );
        }

        if (isOnStraightQtrNote)
        {
            XRLED::setPWM(23, 4095); // when recording, each straight quarter note start button led ON
        }

        // This method handles advancing the sequencer
        // and displaying the start btn and step btn BPM LEDs
        int8_t currStepChar = _stepCharMap[currStepPaged];
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
                    uint8_t prevKeyLED = XRLED::getKeyLED(_stepCharMap[currStepPaged - 1]);

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

                    uint8_t prevKeyLED = XRLED::getKeyLED(_stepCharMap[currTrackLastStep]);
                    
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
            auto currLEDChar = _stepCharMap[t];
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

            //XRSound::initSoundsForTrack(track);
        }

        if (trackToUse.track_type == RAW_SAMPLE)
        {
            //XRSound::handleRawSampleNoteOnForTrackStep(track, step);
        }
        else if (trackToUse.track_type == WAV_SAMPLE)
        {
            //XRSound::handleWavSampleNoteOnForTrackStep(track, step);
        }
        else if (trackToUse.track_type == DEXED)
        {
            //XRSound::handleDexedNoteOnForTrackStep(track, step);
        }
        else if (trackToUse.track_type == SUBTRACTIVE_SYNTH)
        {
            //XRSound::handleSubtractiveSynthNoteOnForTrackStep(track, step);
        }
        else if (trackToUse.track_type == MIDI_OUT)
        {
            //XRSound::handleMIDINoteOnForTrackStep(track, step);
        }
        else if (trackToUse.track_type == CV_GATE)
        {
            //XRSound::handleCvGateNoteOnForTrackStep(track, step);
        }
    }

    void updateCurrentPatternStepState()
    {
        auto currPattern = getHeapCurrentSelectedPattern();

        int currPatternLastStep = currPattern.last_step;

        if (_seqState.currentStep <= currPatternLastStep) {
            if (_seqState.currentStep < currPatternLastStep) {
                if (!((_seqState.currentStep + 1) % 16)) { // TODO: make this bar division configurable
                    ++_seqState.currentBar;
                }

                ++_seqState.currentStep; // advance current step for sequencer
            } else {
                _seqState.currentStep = 1; // reset current step
                _seqState.currentBar = 1;  // reset current bar

                if (_queuedPattern.bank > -1 && _queuedPattern.number > -1) {
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
        if (drawQueueBlink > -1) {
            if (drawQueueBlink == 1) {
                XRDisplay::drawSequencerScreen(true);
            } else if (drawQueueBlink == 0) {
                XRDisplay::drawSequencerScreen(false);
            }
        }

        if (_dequeuePattern) {
            _dequeuePattern = false;

            swapSequencerMemoryForPattern(_queuedPattern.bank, _queuedPattern.number);

            // reset queue flags
            _queuedPattern.bank = -1;
            _queuedPattern.number = -1;
            drawQueueBlink = -1;

            current_step_page = 1;
            current_page_selected = 1;

            if (current_UI_mode == PATTERN_WRITE) {
                XRLED::clearAllStepLEDs();
            } else if (current_UI_mode == TRACK_WRITE) {
                XRLED::displayPageLEDs(1); // TODO need?
            }

            XRDisplay::drawSequencerScreen(false);
        }
    }
}