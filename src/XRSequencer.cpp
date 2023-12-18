#include <XRSequencer.h>
#include <XRHelpers.h>
#include <XRDisplay.h>
#include <XRUX.h>
#include <XRKeyMatrix.h>
#include <XRLED.h>
#include <XRSD.h>
#include <XRClock.h>
#include <XRAudio.h>
#include <map>

namespace XRSequencer
{
    DMAMEM SEQUENCER_EXTERNAL _seqExternal;
    DMAMEM PATTERN _patternCopyBuffer;
    DMAMEM TRACK _trackCopyBuffer;
    DMAMEM STEP _stepCopyBuffer;
    DMAMEM PATTERN _recordPatternBuffer;
    // DMAMEM PATTERN_MODS _patternMods;

    // SEQUENCER _seqHeap;
    PATTERN _heapPattern;
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

    bool _dequeuePattern = false;
    bool _queueBlinked = false;
    bool _recording = false;

    int _drawQueueBlink = -1;

    uint8_t _bpmBlinkTimer = 2;

    // external globals
    TRACK_PERFORM_STATE trackPerformState[MAXIMUM_SEQUENCER_TRACKS];

    std::map<XRSound::SOUND_TYPE, int> trackPageNumMap = {
        {XRSound::T_MONO_SAMPLE, 4},
        {XRSound::T_MONO_SYNTH, 6},
        {XRSound::T_DEXED_SYNTH, 4},
        {XRSound::T_FM_DRUM, 2},
        {XRSound::T_MIDI, 1},
        {XRSound::T_CV_GATE, 1},
        {XRSound::T_EMPTY, 1},
    };

    std::map<XRSound::SOUND_TYPE, std::map<int, std::string>> trackCurrPageNameMap = {
        {XRSound::T_MONO_SAMPLE, {
                         {0, "MAIN"},
                         {1, "LOOP"},
                         {2, "AMP ENV"},
                         {3, "OUTPUT"},
                     }},
        {XRSound::T_MONO_SYNTH, {
                                {0, "MAIN"},
                                {1, "OSC"},
                                {2, "FILTER"},
                                {3, "FILTER ENV"},
                                {4, "AMP ENV"},
                                {5, "OUTPUT"},
                            }},
        {XRSound::T_DEXED_SYNTH, {
                         {0, "MAIN"},
                         {1, "FM1"},
                         {2, "FM2"},
                         {3, "OUTPUT"},
                     }},
        {XRSound::T_FM_DRUM, {
                         {0, "MAIN"},
                         {1, "OUTPUT"},
                     }},
        {XRSound::T_MIDI, {
                       {0, "MAIN"},
                   }},
        {XRSound::T_CV_GATE, {
                      {0, "MAIN"},
                  }},
        {XRSound::T_EMPTY, {
                      {0, ""},
                  }},
    };

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
                currTrack.lstep);
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
                    int currTrackLastStep = currTrack.lstep;
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

                int lastStep = (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE) ? _heapPattern.lstep : _heapPattern.tracks[_currentSelectedTrack].lstep;

                if (!XRKeyMatrix::isFunctionActive())
                {
                    XRLED::displayPageLEDs(
                        -1,
                        (_seqState.playbackState == RUNNING),
                        _currentStepPage,
                        lastStep
                    );   
                }
            }
        }

        // every 1/4 step log memory usage
        if (!(tick % 24)) {
            XRAudio::logMetrics();

            // blink queued bank / pattern
            if (_queuedPattern.bank > -1 && _queuedPattern.number > -1) {
                _drawQueueBlink = 1;
            }
        }

        if (_recording) {
            XRLED::setPWM(23, 512);
            //XRLED::setPWM(23, 1024);
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
        XRSound::handleNoteOffForTrackStep(track, step);
    }

    void noteOffForAllSounds()
    {
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            XRSound::handleNoteOffForTrack(t);
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

    PATTERN &getHeapPattern()
    {
        return _heapPattern;
    }

    SEQUENCER_EXTERNAL &getSequencerExternal()
    {
        return _seqExternal;
    }

    TRACK &getHeapTrack(int track)
    {
        return _heapPattern.tracks[track];
    }

    STEP &getHeapStep(int track, int step)
    {
        return _heapPattern.tracks[track].steps[step];
    }

    PATTERN &getHeapCurrentSelectedPattern()
    {
        return _heapPattern;
    }

    TRACK &getHeapCurrentSelectedTrack()
    {
        return _heapPattern.tracks[_currentSelectedTrack];
    }

    STEP &getHeapCurrentSelectedTrackStep()
    {
        return _heapPattern.tracks[_currentSelectedTrack].steps[_currentSelectedStep];
    }

    std::string getTrackMetaStr(XRSound::SOUND_TYPE type)
    {
        std::string outputStr;

        switch (type)
        {

        case XRSound::T_EMPTY:
            outputStr = "EMPTY";
            break;

        case XRSound::T_MONO_SAMPLE:
            outputStr = "MONOSAMPLE >";
            break;

        case XRSound::T_MONO_SYNTH:
            outputStr = "MONOSYNTH >";
            break;

        case XRSound::T_DEXED_SYNTH:
            outputStr = "DEXED >";
            break;

        case XRSound::T_FM_DRUM:
            outputStr = "FM DRUM >";
            break;

        case XRSound::T_MIDI:
            outputStr = "MIDI";
            break;

        case XRSound::T_CV_GATE:
            outputStr = "CV/GATE";
            break;

        case XRSound::T_CV_TRIG:
            outputStr = "CV/TRIG";
            break;

        default:
            break;
        }

        return outputStr;
    }

    // TODO: reuse this as a sound/patch name method?
    std::string getTrackTypeNameStr(XRSound::SOUND_TYPE type)
    {
        std::string str = "EMPTY"; // default

        switch (type)
        {
        case XRSound::T_MONO_SAMPLE:
            str = "N/A";

            break;

        case XRSound::T_MONO_SYNTH:
            str = "INIT";

            break;

        case XRSound::T_DEXED_SYNTH:
            str = "INIT";

            break;

        case XRSound::T_FM_DRUM:
            str = "INIT";

            break;

        case XRSound::T_MIDI:
            str = "MIDI";

            break;

        case XRSound::T_CV_GATE:
            str = "CV/GATE";

            break;

        case XRSound::T_CV_TRIG:
            str = "CV/TRIG";

            break;

        default:
            break;
        }

        return str;
    }

    // PATTERN_MODS &getModsForCurrentPattern()
    // {
    //     return _patternMods;
    // }

    // TRACK_STEP_MODS &getModsForCurrentTrackStep()
    // {
    //     return _patternMods.tracks[_currentSelectedTrack].steps[_currentSelectedStep];
    // }

    std::string getCurrPageNameForTrack()
    {
        auto currTrack = getHeapCurrentSelectedTrack();
        auto currTrackNum = getCurrentSelectedTrackNum();
        auto currSoundForTrack = XRSound::currentPatternSounds[currTrackNum];

        std::string outputStr = trackCurrPageNameMap[currSoundForTrack.type][_currentSelectedPage];

        return outputStr;
    }

    uint8_t getCurrentTrackPageCount()
    {
        TRACK currTrack = getHeapCurrentSelectedTrack();
        auto currTrackNum = getCurrentSelectedTrackNum();
        auto currSoundForTrack = XRSound::currentPatternSounds[currTrackNum];

        return trackPageNumMap[currSoundForTrack.type];
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

        const int MAX_PATTERN_TRACK_SIZE = 17; // plus one

        for (int t = 1; t < MAX_PATTERN_TRACK_SIZE; t++)
        {
            auto currTrack = currentPattern.tracks[t - 1];
            auto currTrackStep = _seqState.currentTrackSteps[t - 1].currentStep;
            auto currTrackStepForLED = currTrack.steps[currTrackStep - 1];
            auto currLEDChar = XRHelpers::stepCharMap[t];
            auto keyLED = XRLED::getKeyLED(currLEDChar);

            if (currTrackStepForLED.state == STEP_STATE::STATE_OFF)
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
            else if (currTrackStepForLED.state == STEP_STATE::STATE_ON || currTrackStepForLED.state == STEP_STATE::STATE_ACCENTED)
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

            // TODO: add 16 perform struct objects to store mutes & solos, etc

            if (!trackPerformState[t].muted && ((currTrackStepData.state == STEP_STATE::STATE_ON) || (currTrackStepData.state == STEP_STATE::STATE_ACCENTED)))
            {
                handleAddToStepStack(tick, t, currTrackStep);
            }
        }
    }

    void handleAddToStepStack(uint32_t tick, int track, int step)
    {
        auto trackToUse = getHeapTrack(track);

        // bool lenStepModEnabled = _patternMods.tracks[track].step_mod_flags[step].flags[MOD_ATTRS::LENGTH];
        // int lenStepMod = _patternMods.tracks[track].steps[step].length;

        // TODO: FIX
        int lenStepMod = 4;
        bool lenStepModEnabled = false;

        for (uint8_t i = 0; i < STEP_STACK_SIZE; i++)
        {
            if (!trackPerformState[track].muted && _stepStack[i].length == -1)
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
        auto &trackToUse = getHeapTrack(track);
        auto currentSoundForTrack = XRSound::currentPatternSounds[track];

        if (trackPerformState[track].muted)
        {
            return;
        }

        if (XRSound::soundNeedsReinit[track])
        {
            Serial.printf("reinit sounds for track: %d\n", track);

            XRSound::reinitSoundForTrack(track);
        }

        switch (currentSoundForTrack.type)
        {
        case XRSound::T_MONO_SAMPLE:
            XRSound::handleMonoSampleNoteOnForTrackStep(track, step);

            break;
        case XRSound::T_MONO_SYNTH:
            XRSound::handleMonoSynthNoteOnForTrackStep(track, step);
            
            break;
        case XRSound::T_DEXED_SYNTH:
            XRSound::handleDexedSynthNoteOnForTrackStep(track, step);
            
            break;
        case XRSound::T_FM_DRUM:
            XRSound::handleFmDrumNoteOnForTrackStep(track, step);
            
            break;
        case XRSound::T_MIDI:
            XRSound::handleMIDINoteOnForTrackStep(track, step);
            
            break;
        case XRSound::T_CV_GATE:
            XRSound::handleCvGateNoteOnForTrackStep(track, step);
            
            break;
        default:
            break;
        }
    }

    void updateCurrentPatternStepState()
    {
        int currPatternLastStep = _heapPattern.lstep;

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
            auto track = _heapPattern.tracks[t];
            int trackLastStep = track.lstep;
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
                _seqExternal.banks[b].patterns[p].lstep = DEFAULT_LAST_STEP;
                _seqExternal.banks[b].patterns[p].groove.amount = 0;
                _seqExternal.banks[b].patterns[p].groove.id = -1;
                _seqExternal.banks[b].patterns[p].initialized = false;

                if (p == 0)
                    _seqExternal.banks[b].patterns[p].initialized = true;

                for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
                {
                    _seqExternal.banks[b].patterns[p].tracks[t].length = 4;
                    _seqExternal.banks[b].patterns[p].tracks[t].note = 0;
                    _seqExternal.banks[b].patterns[p].tracks[t].octave = 4;
                    _seqExternal.banks[b].patterns[p].tracks[t].velocity = 50;
                    _seqExternal.banks[b].patterns[p].tracks[t].probability = 100;
                    _seqExternal.banks[b].patterns[p].tracks[t].initialized = false;

                    if (t == 0)
                        _seqExternal.banks[b].patterns[p].tracks[t].initialized = true;

                    // now fill in steps
                    for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
                    {
                        _seqExternal.banks[b].patterns[p].tracks[t].steps[s].state = STEP_STATE::STATE_OFF;
                    }
                }
            }
        }
    }

    // void initExternalPatternMods()
    // {
    //     for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
    //     {
    //         // now fill in mod flags
    //         for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
    //         {
    //             for (int f = 0; f < MAX_STEP_MOD_ATTRS; f++)
    //             {
    //                 _patternMods.tracks[t].step_mod_flags[s].flags[f] = false;
    //             }
    //         }

    //         // now fill in mods
    //         for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
    //         {
    //             //_patternMods.tracks[t].steps[s].sample_name;
    //             strcpy(_patternMods.tracks[t].steps[s].sample_name, "");
    //             _patternMods.tracks[t].steps[s].waveform = XRSound::WAVEFORM_TYPE::SAW;
    //             _patternMods.tracks[t].steps[s].detune = 0;
    //             _patternMods.tracks[t].steps[s].fine = 0;
    //             _patternMods.tracks[t].steps[s].looptype = 0;
    //             _patternMods.tracks[t].steps[s].loopstart = 0;
    //             _patternMods.tracks[t].steps[s].loopfinish = 5000;
    //             _patternMods.tracks[t].steps[s].playstart = play_start_sample;
    //             _patternMods.tracks[t].steps[s].level = 0.7;
    //             _patternMods.tracks[t].steps[s].pan = 0;
    //             _patternMods.tracks[t].steps[s].sample_play_rate = 1.0;
    //             _patternMods.tracks[t].steps[s].width = 0.5;
    //             _patternMods.tracks[t].steps[s].oscalevel = 1.0;
    //             _patternMods.tracks[t].steps[s].oscblevel = 0.5;
    //             _patternMods.tracks[t].steps[s].cutoff = 1600;
    //             _patternMods.tracks[t].steps[s].res = 0;
    //             _patternMods.tracks[t].steps[s].filterenvamt = 1.0;
    //             _patternMods.tracks[t].steps[s].filter_attack = 0;
    //             _patternMods.tracks[t].steps[s].filter_decay = 1000;
    //             _patternMods.tracks[t].steps[s].filter_sustain = 1.0;
    //             _patternMods.tracks[t].steps[s].filter_release = 5000;
    //             _patternMods.tracks[t].steps[s].amp_attack = 0;
    //             _patternMods.tracks[t].steps[s].amp_decay = 1000;
    //             _patternMods.tracks[t].steps[s].amp_sustain = 1.0;
    //             _patternMods.tracks[t].steps[s].amp_release = 5000;
    //             _patternMods.tracks[t].steps[s].noise = 0;
    //         }
    //     }
    // }

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

            // IMPORTANT: must change sounds before changing sequencer data!
            XRSound::manageSoundDataForPatternChange(_queuedPattern.bank, _queuedPattern.number);
            swapSequencerMemoryForPattern(_queuedPattern.bank, _queuedPattern.number);
            Serial.println("finished swapping seq mem!");

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
                int lastStep = (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE) ? _heapPattern.lstep : _heapPattern.tracks[_currentSelectedTrack].lstep;

                XRLED::displayPageLEDs(
                    1,
                    (_seqState.playbackState == RUNNING),
                    _currentStepPage,
                    lastStep // TODO need?
                );
            }

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void swapSequencerMemoryForPattern(int newBank, int newPattern)
    {
        // AudioNoInterrupts();
        auto currPatternData = _heapPattern;
        auto newPatternData = _seqExternal.banks[newBank].patterns[newPattern];

        // save any mods for current pattern to SD
        // XRSD::savePatternModsToSdCard();

        // swap sequencer memory data
        _seqExternal.banks[_currentSelectedBank].patterns[_currentSelectedPattern] = currPatternData;
        _heapPattern = newPatternData;

        // initialize new pattern
        _heapPattern.initialized = true;

        // update currently selected vars
        _currentSelectedBank = newBank;
        _currentSelectedPattern = newPattern;
        _currentSelectedTrack = 0;

        // load any mods for new bank/pattern from SD
        // XRSD::loadPatternModsFromSdCard();
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
        STEP_STATE currStepState = currTrack.steps[adjStep].state;

        Serial.print("currStepState: ");
        Serial.println(currStepState == STEP_STATE::STATE_ACCENTED ? "accented" : (currStepState == STEP_STATE::STATE_ON ? "on" : "off"));

        // TODO: implement accent state for MIDI, CV/Trig, Sample, Synth track types?
        if (currStepState == STEP_STATE::STATE_OFF)
        {
            _heapPattern.tracks[_currentSelectedTrack].steps[adjStep].state = STEP_STATE::STATE_ON;
            // TODO: REMOVE
            // copy track properties to steps
            // _heapPattern.tracks[_currentSelectedTrack].steps[adjStep].note = currTrack.note;
            // _heapPattern.tracks[_currentSelectedTrack].steps[adjStep].octave = currTrack.octave;
            // _heapPattern.tracks[_currentSelectedTrack].steps[adjStep].velocity = currTrack.velocity;
        }
        else if (currStepState == STEP_STATE::STATE_ON)
        {
            _heapPattern.tracks[_currentSelectedTrack].steps[adjStep].state = STEP_STATE::STATE_ACCENTED;
            // TODO: REMOVE
            // _heapPattern.tracks[_currentSelectedTrack].steps[adjStep].velocity = 100; // TODO: use a "global accent" value here
        }
        else if (currStepState == STEP_STATE::STATE_ACCENTED)
        {
            _heapPattern.tracks[_currentSelectedTrack].steps[adjStep].state = STEP_STATE::STATE_OFF;
        }

        _seqExternal.banks[_currentSelectedBank].patterns[_currentSelectedPattern].tracks[_currentSelectedTrack].steps[adjStep] = _heapPattern.tracks[_currentSelectedTrack].steps[adjStep];
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
                    currTrack.lstep
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

        // TODO: extract all of below out into XRSound method
        auto currentSoundForTrack = XRSound::currentPatternSounds[track];

        switch (currentSoundForTrack.type)
        {
        case XRSound::T_MONO_SAMPLE:
            XRSound::handleMonoSampleNoteOnForTrack(track);
            break;
        case XRSound::T_MONO_SYNTH:
            XRSound::handleDexedSynthNoteOnForTrack(track);
            break;
        case XRSound::T_DEXED_SYNTH:
            XRSound::handleFmDrumNoteOnForTrack(track);
            break;
        case XRSound::T_FM_DRUM:
            XRSound::handleFmDrumNoteOnForTrack(track);
            break;
        case XRSound::T_MIDI:
            XRSound::handleMIDINoteOnForTrack(track);
            break;
        case XRSound::T_CV_GATE:
            XRSound::handleCvGateNoteOnForTrack(track);
            break;
        
        default:
            break;
        }
    }

    void handleNoteOffForTrack(int track)
    {
        XRSound::handleNoteOffForTrack(track);
    }

    void initializeCurrentSelectedTrack()
    {
        _heapPattern.tracks[_currentSelectedTrack].initialized = true;
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
        _seqExternal.banks[_currentSelectedBank].patterns[_currentSelectedPattern] = _heapPattern;
    }

    void setCurrentSelectedStep(int step)
    {
       // _heapPattern.tracks[_currentSelectedTrack].steps[step];

        _currentSelectedStep = step;
    }

    void setCurrentStepPage(int8_t page)
    {
        _currentStepPage = page;
    }

    void setCopyBufferForStep(int step)
    {
        _stepCopyBuffer = _heapPattern.tracks[_currentSelectedTrack].steps[step];
    }

    void setCopyBufferForTrack(int track)
    {
        _trackCopyBuffer = _heapPattern.tracks[track];
    }

    void setCopyBufferForPattern(int pattern)
    {
        _patternCopyBuffer = _seqExternal.banks[_currentSelectedBank].patterns[pattern];
    }

    PATTERN &getCopyBufferForPattern()
    {
        return _patternCopyBuffer;
    }

    TRACK &getCopyBufferForTrack()
    {
        return _trackCopyBuffer;
    }

    STEP &getCopyBufferForStep()
    {
        return _stepCopyBuffer;
    }

    void setRatchetTrack(int track)
    {
        _ratchetTrack = track;
    }

    void setRatchetDivision(int track)
    {
        _ratchetDivision = track;
    }
}