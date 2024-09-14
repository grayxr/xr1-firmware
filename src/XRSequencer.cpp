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
#include <XRAsyncPSRAMLoader.h>

namespace XRSequencer
{
    // private variables
 
    SEQUENCER_STATE _seqState;
    QUEUED_PATTERN_STATE _queuedPatternState;

    STACK_STEP_DATA _stepStack[STEP_STACK_SIZE];
    STACK_STEP_DATA _ignoredStepStack[STEP_STACK_SIZE];
    STACK_RATCHET_DATA _ratchetStack[RATCHET_STACK_SIZE];

    DMAMEM PATTERN _patternRecordBuffer;

    int8_t _currentSelectedBank = 0;        // default to 0 (first)
    int8_t _currentSelectedPattern = 0;     // default to 0 (first)
    int8_t _currentSelectedTrack = 0;       // default to 0 (first)
    int8_t _currentSelectedTrackLayer = 0;  // default to 0 (first)
    int8_t _currentSelectedStep = -1;       // default to -1 (none)
    int8_t _currentStepPage = 1;
    int8_t _currentSelectedPage = 0;
    int8_t _ratchetTrack = -1;
    int8_t _ratchetDivision = -1;

    bool _initTracks[MAXIMUM_SEQUENCER_TRACKS];

    bool _recording = false;
    bool _dequeuePattern = false;
    bool _dequeueLoadNewPatternSamples = false;
    bool _patternQueueBlinked = false;
    bool _dequeueTrackLayer = false;
    bool _trackLayerBlinked = false;

    int _queuedTrackLayer = -1;
    int _drawPatternQueueBlink = -1;
    int _drawTrackLayerQueueBlink = -1;

    uint8_t _bpmBlinkTimer = 2;

    // extern globals

    DMAMEM PATTERN activePattern;
    DMAMEM PATTERN nextPattern;
    DMAMEM TRACK_LAYER activeTrackLayer;
    DMAMEM TRACK_LAYER nextTrackLayer;
    DMAMEM TRACK_STEP_MOD_LAYER activeTrackStepModLayer;
    DMAMEM PATTERN_FX_PAGE_INDEXES patternFxPages[MAXIMUM_PATTERN_FX_PARAM_PAGES];
    DMAMEM RECORDING_STATE recordingState;

    DMAMEM PATTERN patternCopyBuffer;
    DMAMEM TRACK_LAYER trackLayerCopyBuffer;
    DMAMEM TRACK_STEP_MOD_LAYER trackStepModLayerCopyBuffer;

    TRACK_PERFORM_STATE trackPerformState[MAXIMUM_SEQUENCER_TRACKS];

    QUEUE_ACTION queueAction = QUEUE_ACTION::NONE;
    bool queueErrorLogged = false;

    bool init()
    {
        _currentSelectedBank = 0;
        _currentSelectedPattern = 0;
        _currentSelectedTrack = 0;
        _currentSelectedTrackLayer = 0;

        patternFxPages[0] = PATTERN_FX_PAGE_INDEXES::DELAY;
        patternFxPages[1] = PATTERN_FX_PAGE_INDEXES::NA;
        patternFxPages[2] = PATTERN_FX_PAGE_INDEXES::NA;
        patternFxPages[3] = PATTERN_FX_PAGE_INDEXES::NA;

        // init recording state
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
            {
                recordingState.tracks[t].steps[s].state = STEP_STATE::STATE_OFF;
                recordingState.tracks[t].steps[s].length = 4;
                recordingState.tracks[t].steps[s].note = 0;
                recordingState.tracks[t].steps[s].queued = false;
            }
        }

        initActivePattern();
        initActiveTrackLayer();
        initActiveTrackStepModLayer();

        return true;
    }

    void startRecording()
    {
        _recording = true;
    }

    void stopRecording()
    {
        _recording = false;
    }

    void applyRecordingToSequencer(uint8_t t, bool lastStep)
    {
        auto &track = activeTrackLayer.tracks[t];
        auto trackLastStep = track.lstep;

        if (!lastStep) {
            auto currStep = _seqState.currentTrackSteps[t].currentStep == 1 ? trackLastStep : _seqState.currentTrackSteps[t].currentStep;

            for (int zPrevStep = 0; zPrevStep < currStep; zPrevStep++) {
                if (recordingState.tracks[t].steps[zPrevStep].queued) {
                    Serial.printf("STEP applying recording for track: %d, step: %d, step queue state: %d ?\n", t+1, zPrevStep+1, recordingState.tracks[t].steps[zPrevStep].queued);

                    // then apply recording state to sequencer data
                    track.steps[zPrevStep].state = recordingState.tracks[t].steps[zPrevStep].state;

                    // TODO: impl recording based track step mod layer NOTE and LENGTH tracking
                    // e.g. if note is not -1, then set note to track step mod layer
                    // e.g. if length is not -1, then set length to track step mod layer

                    // then reinit recording state
                    recordingState.tracks[t].steps[zPrevStep].state = STEP_STATE::STATE_OFF;
                    recordingState.tracks[t].steps[zPrevStep].queued = false;
                }
            }
        } else {
            auto &track = activeTrackLayer.tracks[t];
            auto zLastStep = trackLastStep - 1;

            if (recordingState.tracks[t].steps[zLastStep].queued) { 

                Serial.printf("LAST STEP applying recording for track %d last step: %d\n", t+1, zLastStep);

                // then apply recording state to sequencer data
                track.steps[zLastStep].state = recordingState.tracks[t].steps[zLastStep].state;

                // TODO: impl recording based track step mod layer NOTE and LENGTH tracking
                // e.g. if note is not -1, then set note to track step mod layer
                // e.g. if length is not -1, then set length to track step mod layer

                // then reinit recording state
                recordingState.tracks[t].steps[zLastStep].state = STEP_STATE::STATE_OFF;
                recordingState.tracks[t].steps[zLastStep].queued = false;
            }
        }
    }

    void applyRecordingToSequencerWhenStopped()
    {
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            auto &track = activeTrackLayer.tracks[t];

            for (int s = 0; s < track.lstep; s++)
            {
                applyRecordingToSequencer(t, (s+1) == track.lstep);
            }
        }
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

        // NOTE: this function is affected by swing

        // handle16PPQN(tick);
    }

    void ClockOutRigid16PPQN(uint32_t tick)
    {
        auto currentUXMode = XRUX::getCurrentMode();
        if (currentUXMode == XRUX::UX_MODE::PROJECT_INITIALIZE || currentUXMode == XRUX::UX_MODE::PROJECT_BUSY)
        {
            return;
        }

        // NOTE: this function is NOT affected by swing

        auto seqState = getSeqState();
        bool isOnStraightQtrNote = (seqState.currentStep == 1 || !((seqState.currentStep - 1) % 4));

        if (isOnStraightQtrNote && !(tick % 4))
        {
            XRLED::setPWM(23, 4095); // when recording, each straight quarter note start button led FULL ON
        }
        
        updateCurrentPatternStepState();

        if (!(tick % 4)) {
            // blink queued pattern number every qtr note if pattern is queued
            if (_queuedPatternState.bank > -1 && _queuedPatternState.number > -1) {
                _drawPatternQueueBlink = 1;
            } else if (_queuedTrackLayer > -1) {
                _drawTrackLayerQueueBlink = 1;
            }
        } else if ((_drawPatternQueueBlink > -1) && !(tick % 2)) {
            _drawPatternQueueBlink = 0;
        } else if ((_drawTrackLayerQueueBlink > -1) && !(tick % 2)) {
            _drawTrackLayerQueueBlink = 0;
        }

        // every 1/4 step log memory usage
        if (!(tick % 4)) {
            XRAudio::logMetrics();
        }
    }

    void ClockOutTracked16PPQN(uint8_t t, uint32_t tick)
    {
        auto currentUXMode = XRUX::getCurrentMode();
        if (currentUXMode == XRUX::UX_MODE::PROJECT_INITIALIZE || currentUXMode == XRUX::UX_MODE::PROJECT_BUSY)
        {
            return;
        }

        // NOTE: this function is affected by swing

        handleCurrentTrackStepLEDs(t);

        triggerAllStepsForTrack(t, tick);

        updateTrackStepState(t, tick);
    }

    void ClockOutRigidTracked16PPQN(uint8_t t, uint32_t tick)
    {
        auto currentUXMode = XRUX::getCurrentMode();
        if (currentUXMode == XRUX::UX_MODE::PROJECT_INITIALIZE || currentUXMode == XRUX::UX_MODE::PROJECT_BUSY)
        {
            return;
        }

        // NOTE: this function is NOT affected by swing
    }

    void updateTrackStepState(uint8_t t, uint32_t tick)
    {
        auto &track = activeTrackLayer.tracks[t];
        auto trackLastStep = track.lstep;
        auto trackCurrentStep = _seqState.currentTrackSteps[t].currentStep;

        if (trackCurrentStep <= trackLastStep)
        {
            if (trackCurrentStep < trackLastStep)
            {
                if (!((trackCurrentStep + 1) % 16))
                { // TODO: make this bar division configurable?
                    ++_seqState.currentTrackSteps[t].currentBar;
                }

                ++_seqState.currentTrackSteps[t].currentStep; // advance current step for track

                applyRecordingToSequencer(t, false);
            }
            else
            {
                _seqState.currentTrackSteps[t].currentStep = 1; // reset current step for track
                _seqState.currentTrackSteps[t].currentBar = 1;  // reset current bar for track

                applyRecordingToSequencer(t, true);
            }
        }
    }

    void handleCurrentTrackStepLEDs(uint8_t t)
    {
        auto currentUXMode = XRUX::getCurrentMode();
        auto currentSelectedTrack = getCurrentSelectedTrackNum();

        if (t != currentSelectedTrack) {
            return;
        }

        auto seqState = getSeqState();
        auto currentStepPage = getCurrentStepPage();
        bool functionActive = XRKeyMatrix::isFunctionActive();

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
            auto &currTrack = getCurrentSelectedTrack();

            XRLED::displayPageLEDs(
                currentSelectedTrackCurrentBar,
                (_seqState.playbackState == RUNNING),
                currentStepPage,
                currTrack.lstep
            );
        }

        // This method handles advancing the sequencer
        // and displaying the start btn and step btn BPM LEDs
        int8_t currStepChar = XRHelpers::stepCharMap[currStepPaged];
        int8_t keyLED = XRLED::getKeyLED(currStepChar);

        if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE)
        {
            displayAllTrackNoteOnLEDs(true);
        }

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

                    auto &currTrack = activeTrackLayer.tracks[t];
                    auto prevTrackStepData = currTrack.steps[currentSelectedTrackCurrentStep-2];

                    if (prevTrackStepData.state == STEP_STATE::STATE_ACCENTED)
                    {
                        XRLED::setPWM(keyLED, 4095); // turn prev sixteenth led ACCENTED ON
                    } else if (prevTrackStepData.state == STEP_STATE::STATE_ON) {
                        XRLED::setPWM(keyLED, 512); // turn prev sixteenth led ON
                    } else {
                        XRLED::setPWM(prevKeyLED, 0); // turn prev sixteenth led OFF
                    }
                }
            }
            else
            {
                if (turnOffLastLED)
                {
                    auto &currTrack = getCurrentSelectedTrack();
                    auto currTrackLastStep = currTrack.lstep;

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
                auto &currTrack = activeTrackLayer.tracks[t];
                auto currTrackStepData = currTrack.steps[currentSelectedTrackCurrentStep-1];

                if (currTrackStepData.state == STEP_STATE::STATE_ACCENTED)
                {
                    XRLED::setPWM(keyLED, 0); // turn sixteenth led OFF
                } else {
                    XRLED::setPWM(keyLED, 4095); // turn sixteenth led ON
                }
            }
        }
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

    void handle96PPQN(uint32_t tick)
    {
        auto currentUXMode = XRUX::getCurrentMode();

        if ((tick % 6) && !(tick % _bpmBlinkTimer))
        {
            XRLED::setPWM(23, 0); // turn start button led OFF every 16th note
        }

        if (_recording) {
            XRLED::setPWM(23, 512);
            //XRLED::setPWM(23, 1024);
        }

        //if (!(tick % 8)) {
        triggerRatchetingTrack(tick);
        //}

        // step should be removed from stack at rate of 96ppqn / 6
        if (!(tick % 6)) { 
            handleRemoveFromStepStack(tick);
            handleRemoveFromIgnoredStepStack(tick);
        }

        if ((tick % (6)) && !(tick % _bpmBlinkTimer))
        {
            if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE)
            {
                displayAllTrackNoteOnLEDs(false);
            }
            else if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE || currentUXMode == XRUX::UX_MODE::SUBMITTING_STEP_VALUE)
            {
                XRLED::setDisplayStateForAllStepLEDs();

                int lastStep = (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE) ? activePattern.lstep : activeTrackLayer.tracks[_currentSelectedTrack].lstep;

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

    void handleRemoveFromIgnoredStepStack(uint32_t tick)
    {
        for (int i = 0; i < STEP_STACK_SIZE; i++)
        {
            if (_ignoredStepStack[i].length != -1)
            {
                --_ignoredStepStack[i].length;

                if (_ignoredStepStack[i].length == 0)
                {
                    // re-initialize stack entry
                    _ignoredStepStack[i].trackNum = -1;
                    _ignoredStepStack[i].stepNum = -1;
                    _ignoredStepStack[i].length = -1;
                }
            }
        }
    }
    
    bool isTrackStepBeingIgnored(int track, int step)
    {
        for (int i = 0; i < STEP_STACK_SIZE; i++)
        {
            if (
                _ignoredStepStack[i].trackNum == track && 
                _ignoredStepStack[i].stepNum == step
            ) {
                return true;
            }
        }

        return false;
    }

    void handleNoteOffForTrackStep(int track, int step)
    {
        XRSound::handleNoteOffForTrackStep(track, step);
    }

    void noteOffForAllSounds()
    {
        // clear track based notes
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {  
            handleNoteOffForTrack(t);
        }

        // clear step based notes
        for (int i = 0; i < STEP_STACK_SIZE; i++)
        {
            if (_stepStack[i].length > 0)
            {
                handleNoteOffForTrackStep(_stepStack[i].trackNum, _stepStack[i].stepNum);

                // re-initialize stack entry
                _stepStack[i].trackNum = -1;
                _stepStack[i].stepNum = -1;
                _stepStack[i].length = -1;
            }
        }

        // clear ratchet stack
        for (int r = 0; r < RATCHET_STACK_SIZE; r++)
        {
            if (_ratchetStack[r].length > 0)
            {
                // re-initialize stack entry
                _ratchetStack[r].trackNum = -1;
                _ratchetStack[r].length = -1;
            }
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

    int8_t getCurrentStepPage()
    {
        return _currentStepPage;
    }

    int8_t getCurrentSelectedPage()
    {
        return _currentSelectedPage;
    }

    int8_t getCurrentSelectedTrackLayerNum()
    {
        return _currentSelectedTrackLayer;
    }

    QUEUED_PATTERN_STATE &getQueuedPatternState()
    {
        return _queuedPatternState;
    }

    PATTERN &getActivePattern()
    {
        return activePattern;
    }

    PATTERN &getNextPattern()
    {
        return nextPattern;
    }

    TRACK &getTrack(int track)
    {
        return activeTrackLayer.tracks[track];
    }

    STEP &getStep(int track, int step)
    {
        return activeTrackLayer
            .tracks[track]
            .steps[step];
    }

    PATTERN &getCurrentSelectedPattern()
    {
        return activePattern;
    }

    TRACK &getCurrentSelectedTrack()
    {
        return activeTrackLayer
            .tracks[_currentSelectedTrack];
    }

    TRACK_LAYER &getCurrentSelectedTrackLayer()
    {
        return activeTrackLayer;
    }

    STEP &getCurrentSelectedTrackStep()
    {
        return activeTrackLayer
            .tracks[_currentSelectedTrack]
            .steps[_currentSelectedStep];
    }

    int8_t getRatchetTrack()
    {
        return _ratchetTrack;
    }

    int8_t getRatchetDivision()
    {
        return _ratchetDivision;
    }

    void displayAllTrackNoteOnLEDs(bool enable)
    {
        const int MAX_PATTERN_TRACK_SIZE = 17; // plus one

        for (int t = 1; t < MAX_PATTERN_TRACK_SIZE; t++)
        {
            auto currTrack = activeTrackLayer.tracks[t - 1];
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

    void triggerAllStepsForTrack(uint8_t t, uint32_t tick) {
        int8_t currTrackStep = _seqState.currentTrackSteps[t].currentStep - 1; // get zero-based track step number

        auto &currTrack = activeTrackLayer.tracks[t];
        auto currTrackStepData = currTrack.steps[currTrackStep];
        
        if (!trackPerformState[t].muted && ((currTrackStepData.state == STEP_STATE::STATE_ON) || (currTrackStepData.state == STEP_STATE::STATE_ACCENTED)))
        {
            if (isStepProbablyEnabled(t, currTrackStep)) {
                handleAddToStepStack(tick, t, currTrackStep);
            } else {
                handleAddToIgnoredStepStack(tick, t, currTrackStep);
            }
        }
    }

    void triggerAllStepsForAllTracks(uint32_t tick)
    {
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            triggerAllStepsForTrack(t, tick);
        }
    }

    bool isStepProbablyEnabled(int track, int step)
    {
        auto &currTrack = activeTrackLayer.tracks[track];
        auto prob = currTrack.probability;

        if (activeTrackStepModLayer.tracks[track].steps[step].flags[PROBABILITY]) {
            prob = activeTrackStepModLayer.tracks[track].steps[step].mods[PROBABILITY];
        }

        if (prob < 100) {
            uint8_t r = random(99);

            if (prob == 0 || (r > prob)) {
                return false;
            }
        }

        return true;
    }

    void handleAddToStepStack(uint32_t tick, int track, int step)
    {
        auto &trackToUse = getTrack(track);

        int lenStepMod = activeTrackStepModLayer.tracks[track].steps[step].mods[LENGTH];
        bool lenStepModEnabled = activeTrackStepModLayer.tracks[track].steps[step].flags[LENGTH];

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

    void handleAddToIgnoredStepStack(uint32_t tick, int track, int step)
    {
        auto &trackToUse = getTrack(track);

        int lenStepMod = activeTrackStepModLayer.tracks[track].steps[step].mods[LENGTH];
        bool lenStepModEnabled = activeTrackStepModLayer.tracks[track].steps[step].flags[LENGTH];

        for (size_t i = 0; i < STEP_STACK_SIZE; i++)
        {
            if (_ignoredStepStack[i].length == -1)
            {
                _ignoredStepStack[i].trackNum = track;
                _ignoredStepStack[i].stepNum = step;
                _ignoredStepStack[i].length = lenStepModEnabled ? lenStepMod : trackToUse.length;

                return;
            }
        }
    }

    void handleNoteOnForTrackStep(int track, int step)
    {
        if (trackPerformState[track].muted)
        {
            return;
        }

        // hard dequeue and reinit all tracks on first step
        if (step == 0 && _dequeuePattern) {
            if (_dequeuePattern) // still have to dequeue
            {
                Serial.println("MANUALLY DEQUEUE PATTERN BEFORE FIRST STEP!!!");

                if (queueAction != QUEUE_ACTION::DONE) {
                    // cycle through all queue actions manually here
                    forceQueueActions();
                }

                dequeuePattern();

                if (step == 0 && XRSound::soundNeedsReinit[track]) {
                    for (int d = 0; d < 4; d++)
                    {
                        if (XRSound::activePatternSounds[d].type == XRSound::T_DEXED_SYNTH) {
                            XRSound::reinitSoundForTrack(d);

                            auto a = XRDexedManager::getActiveInstanceForTrack(d);
                            auto in = XRDexedManager::getInactiveInstanceForTrack(d);

                            // enable active instance, mute inactive instance
                            AudioNoInterrupts();
                            XRSound::dexedInstances[a].amp.gain(1);
                            XRSound::dexedInstances[in].amp.gain(0);
                            XRSound::dexedInstances[in].dexed.notesOff();
                            AudioInterrupts();
                        }
                    }

                    if (track > 3) {
                        Serial.printf("reinit sound for track: %d\n", track);

                        XRSound::reinitSoundForTrack(track);
                        // TODO: apply specific track choke instead of reapplying all chokes here
                        XRSound::applyTrackChokes();
                    }
                }
            }
        } else if (step == 0 && XRSound::soundNeedsReinit[track]) {
            for (int d = 0; d < 4; d++)
            {
                if (XRSound::activePatternSounds[d].type == XRSound::T_DEXED_SYNTH) {
                    XRSound::reinitSoundForTrack(d);

                    auto a = XRDexedManager::getActiveInstanceForTrack(d);
                    auto in = XRDexedManager::getInactiveInstanceForTrack(d);

                    // enable active instance, mute inactive instance
                    AudioNoInterrupts();
                    XRSound::dexedInstances[a].amp.gain(1);
                    XRSound::dexedInstances[in].amp.gain(0);
                    XRSound::dexedInstances[in].dexed.notesOff();
                    AudioInterrupts();
                }
            }

            if (track > 3) {
                Serial.printf("reinit sound for track: %d\n", track);

                XRSound::reinitSoundForTrack(track);
                // TODO: apply specific track choke instead of reapplying all chokes here
                XRSound::applyTrackChokes();
            }
        }

        if (XRSound::soundNeedsReinit[track])
        {
            Serial.printf("reinit sounds for track: %d\n", track);

            XRSound::reinitSoundForTrack(track);

            // TODO: apply specific track choke instead of reapplying all chokes here
            XRSound::applyTrackChokes();
        }

        switch (XRSound::activePatternSounds[track].type)
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
        case XRSound::T_BRAIDS_SYNTH:
            XRSound::handleBraidsNoteOnForTrackStep(track, step);
            
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
        int currPatternLastStep = activePattern.lstep;

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

                if (_queuedPatternState.bank > -1 && _queuedPatternState.number > -1)
                {
                    _dequeuePattern = true;
                    if (!_dequeueLoadNewPatternSamples) {
                        _dequeueLoadNewPatternSamples = true;
                    }
                } else if (_queuedTrackLayer > -1)
                {
                    _dequeueTrackLayer = true;
                }
            }
        }
    }

    void updateAllTrackStepStates()
    {
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            auto &track = activeTrackLayer.tracks[t];
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

    void initActivePattern()
    {
        activePattern.initialized = true; // always initialize first pattern?
        activePattern.lstep = DEFAULT_LAST_STEP;
        activePattern.groove.amount = 0;
        activePattern.groove.id = -1;
        activePattern.accent = DEFAULT_GLOBAL_ACCENT;
        activePattern.fx = getInitActivePatternFxParams();

        // Serial.printf(
        //     "activePattern.fx delay time: %f, delay fdbk: %f, delay pan: %f\n",
        //     activePattern.fx.pages[PATTERN_FX_PAGE_INDEXES::DELAY].params[PATTERN_FX_DELAY_PARAMS::TIME],
        //     activePattern.fx.pages[PATTERN_FX_PAGE_INDEXES::DELAY].params[PATTERN_FX_DELAY_PARAMS::FEEDBACK],
        //     activePattern.fx.pages[PATTERN_FX_PAGE_INDEXES::DELAY].params[PATTERN_FX_DELAY_PARAMS::PAN]
        // );
    }

    void initNextPattern()
    {
        nextPattern.initialized = true; // always initialize first pattern?
        nextPattern.lstep = DEFAULT_LAST_STEP;
        nextPattern.groove.amount = 0;
        nextPattern.groove.id = -1;
        nextPattern.accent = DEFAULT_GLOBAL_ACCENT;
        nextPattern.fx = getInitActivePatternFxParams();

        // Serial.printf(
        //     "nextPattern.fx delay time: %f, delay fdbk: %f, delay pan: %f\n",
        //     nextPattern.fx.pages[PATTERN_FX_PAGE_INDEXES::DELAY].params[PATTERN_FX_DELAY_PARAMS::TIME],
        //     nextPattern.fx.pages[PATTERN_FX_PAGE_INDEXES::DELAY].params[PATTERN_FX_DELAY_PARAMS::FEEDBACK],
        //     nextPattern.fx.pages[PATTERN_FX_PAGE_INDEXES::DELAY].params[PATTERN_FX_DELAY_PARAMS::PAN]
        // );
    }

    void initActiveTrackLayer()
    {
        for (size_t t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++) {
            activeTrackLayer.tracks[t].length = 4;
            activeTrackLayer.tracks[t].note = 0;
            activeTrackLayer.tracks[t].octave = 4;
            activeTrackLayer.tracks[t].velocity = 50;
            activeTrackLayer.tracks[t].probability = 100;
            activeTrackLayer.tracks[t].lstep = DEFAULT_LAST_STEP;
            activeTrackLayer.tracks[t].initialized = false;

            // now fill in steps
            for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
            {
                activeTrackLayer.tracks[t].steps[s].state = STEP_STATE::STATE_OFF;
            }
        }
    }

    void initTrackLayerCopyBuffer()
    {
        // for (size_t t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++) {
        //     trackLayerCopyBuffer.tracks[t].length = 4;
        //     trackLayerCopyBuffer.tracks[t].note = 0;
        //     trackLayerCopyBuffer.tracks[t].octave = 4;
        //     trackLayerCopyBuffer.tracks[t].velocity = 50;
        //     trackLayerCopyBuffer.tracks[t].probability = 100;
        //     trackLayerCopyBuffer.tracks[t].lstep = DEFAULT_LAST_STEP;
        //     trackLayerCopyBuffer.tracks[t].initialized = false;

        //     // now fill in steps
        //     for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
        //     {
        //         trackLayerCopyBuffer.tracks[t].steps[s].state = STEP_STATE::STATE_OFF;
        //     }
        // }
    }

    void initNextTrackLayer()
    {
        for (size_t t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++) {
            nextTrackLayer.tracks[t].length = 4;
            nextTrackLayer.tracks[t].note = 0;
            nextTrackLayer.tracks[t].octave = 4;
            nextTrackLayer.tracks[t].velocity = 50;
            nextTrackLayer.tracks[t].probability = 100;
            nextTrackLayer.tracks[t].lstep = DEFAULT_LAST_STEP;
            nextTrackLayer.tracks[t].initialized = false;

            // now fill in steps
            for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
            {
                nextTrackLayer.tracks[t].steps[s].state = STEP_STATE::STATE_OFF;
            }
        }
    }

    void initActiveTrackStepModLayer()
    {
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
            {
                for (int m = 0; m < MAXIMUM_TRACK_MODS; m++)
                {
                    activeTrackStepModLayer.tracks[t].steps[s].mods[m] = 0;
                    activeTrackStepModLayer.tracks[t].steps[s].flags[m] = false;
                }
            }
        }
    }

    PATTERN_FX_PARAMS getInitActivePatternFxParams()
    {
        PATTERN_FX_PARAMS fxParams;

        for (size_t f=0; f<MAXIMUM_PATTERN_FX_PARAM_PAGES; f++) {
            if (patternFxPages[f] == PATTERN_FX_PAGE_INDEXES::DELAY) {
                fxParams.pages[f].params[PATTERN_FX_DELAY_PARAMS::TIME] = 300; // 300ms
                fxParams.pages[f].params[PATTERN_FX_DELAY_PARAMS::FEEDBACK] = 0; // 0-1.0 range
                fxParams.pages[f].params[PATTERN_FX_DELAY_PARAMS::PAN] = 0; // -1.0-1.0 range
            }
        }

        return fxParams;
    }

    void handlePatternQueueActions()
    {
        if (!queueErrorLogged && queueAction != QUEUE_ACTION::NONE && queueAction != QUEUE_ACTION::DONE && _queuedPatternState.bank == -1 && _queuedPatternState.number == -1) {
            Serial.println("QUEUE ACTION: ERROR! QUEUE ACTIONS NOT DONE! NO QUEUED PATTERN ANYMORE!");

            queueErrorLogged = true;

            return;
        }

        if (_queuedPatternState.bank > -1 && _queuedPatternState.number > -1 && queueAction != QUEUE_ACTION::DONE) {
            delay(2);

            auto newBank = _queuedPatternState.bank;
            auto newPattern = _queuedPatternState.number;

            switch (queueAction) {
                case QUEUE_ACTION::NONE:
                    // N/A
                    Serial.println("QUEUE ACTION:: starting queue actions!");

                    queueAction = QUEUE_ACTION::MANAGE_SOUNDS;

                    break;
                case QUEUE_ACTION::MANAGE_SOUNDS:
                    Serial.println("QUEUE ACTION:: manage sounds queue action!");

                    // save, load
                    
                    if (XRSound::patternSoundsDirty) {
                        XRSD::saveActivePatternSounds();
                    }

                    if (!XRSD::loadNextPatternSounds(newBank, newPattern))
                    {
                        XRSound::initNextPatternSounds();
                    }

                    Serial.println("QUEUE ACTION:: manage sound queue action DONE!");

                    queueAction = QUEUE_ACTION::MANAGE_SOUND_STEP_MODS;

                    break;
                case QUEUE_ACTION::MANAGE_SOUND_STEP_MODS:
                    Serial.println("QUEUE ACTION:: manage sound step mods queue action!");

                    // save, load
                    if (XRSound::patternSoundStepModsDirty) {
                        XRSD::saveActiveSoundStepModLayerToSdCard();
                    }

                    if (!XRSD::loadPatternSoundStepModLayerFromSdCard(newBank, newPattern, 0)) {
                        XRSound::initPatternSoundStepMods();
                    }

                    queueAction = QUEUE_ACTION::MANAGE_PATTERNS;

                    Serial.println("QUEUE ACTION:: manage sound step mods queue action DONE!");

                    break;
                case QUEUE_ACTION::MANAGE_PATTERNS:
                    Serial.println("QUEUE ACTION:: manage patterns queue action!");

                    // save, load
                    XRSD::saveActivePatternToSdCard();

                    if (!XRSD::loadNextPattern(newBank, newPattern)){
                        XRSequencer::initNextPattern();
                    }

                    queueAction = QUEUE_ACTION::MANAGE_TRACK_LAYERS;

                    Serial.println("QUEUE ACTION:: manage patterns queue action DONE!");

                    break;
                case QUEUE_ACTION::MANAGE_TRACK_LAYERS:
                    Serial.println("QUEUE ACTION:: manage track layers queue action!");

                    // save, load
                    XRSD::saveActiveTrackLayerToSdCard();

                    if (!XRSD::loadNextTrackLayer(newBank, newPattern, 0)){
                        XRSequencer::initNextTrackLayer();
                    }

                    queueAction = QUEUE_ACTION::MANAGE_TRACK_LAYER_STEP_MODS;

                    Serial.println("QUEUE ACTION:: manage track layers queue action DONE!");

                    break;
                case QUEUE_ACTION::MANAGE_TRACK_LAYER_STEP_MODS:
                    Serial.println("QUEUE ACTION:: manage track layer step mods queue action!");

                    // save, load
                    XRSD::saveActiveTrackStepModLayerToSdCard();
                    
                    if (!XRSD::loadActiveTrackStepModLayerFromSdCard(newBank, newPattern, 0)) {
                        XRSequencer::initActiveTrackStepModLayer();
                    }

                    // load next dexed instances
                    XRSound::loadNextDexedInstances();
                    XRAsyncPSRAMLoader::startAsyncInitOfNextSamples();

                    queueAction = QUEUE_ACTION::DONE;

                    Serial.println("QUEUE ACTION:: manage track layer step mods queue action DONE!");
                    Serial.println("QUEUE ACTION:: queue actions ALL DONE!");

                    break;
                default:
                    break;
            }
        }
    }

    void forceQueueActions()
    {
        auto newBank = _queuedPatternState.lastBank;
        auto newPattern = _queuedPatternState.lastNumber;

        if (newBank == -1 || newPattern == -1) {
            Serial.println("ERROR! CANNOT FORCE QUEUE ACTIONS!");

            return;
        }

        // N/A
        Serial.println("QUEUE ACTION:: starting queue actions!");

        queueAction = QUEUE_ACTION::MANAGE_SOUNDS;

        Serial.println("QUEUE ACTION:: manage sounds queue action!");

        // save, load
        
        if (XRSound::patternSoundsDirty) {
            XRSD::saveActivePatternSounds();
        }

        if (!XRSD::loadNextPatternSounds(newBank, newPattern))
        {
            XRSound::initNextPatternSounds();
        }

        Serial.println("QUEUE ACTION:: manage sound queue action DONE!");

        queueAction = QUEUE_ACTION::MANAGE_SOUND_STEP_MODS;

        Serial.println("QUEUE ACTION:: manage sound step mods queue action!");

        // save, load
        if (XRSound::patternSoundStepModsDirty) {
            XRSD::saveActiveSoundStepModLayerToSdCard();
        }

        if (!XRSD::loadPatternSoundStepModLayerFromSdCard(newBank, newPattern, 0)) {
            XRSound::initPatternSoundStepMods();
        }

        queueAction = QUEUE_ACTION::MANAGE_PATTERNS;

        Serial.println("QUEUE ACTION:: manage sound step mods queue action DONE!");

        Serial.println("QUEUE ACTION:: manage patterns queue action!");

        // save, load
        XRSD::saveActivePatternToSdCard();

        if (!XRSD::loadNextPattern(newBank, newPattern)){
            XRSequencer::initNextPattern();
        }

        queueAction = QUEUE_ACTION::MANAGE_TRACK_LAYERS;

        Serial.println("QUEUE ACTION:: manage patterns queue action DONE!");

        Serial.println("QUEUE ACTION:: manage track layers queue action!");

        // save, load
        XRSD::saveActiveTrackLayerToSdCard();

        if (!XRSD::loadNextTrackLayer(newBank, newPattern, 0)){
            XRSequencer::initNextTrackLayer();
        }

        queueAction = QUEUE_ACTION::MANAGE_TRACK_LAYER_STEP_MODS;

        Serial.println("QUEUE ACTION:: manage track layers queue action DONE!");

        Serial.println("QUEUE ACTION:: manage track layer step mods queue action!");

        // save, load
        XRSD::saveActiveTrackStepModLayerToSdCard();
        
        if (!XRSD::loadActiveTrackStepModLayerFromSdCard(newBank, newPattern, 0)) {
            XRSequencer::initActiveTrackStepModLayer();
        }

        // load next dexed instances
        XRSound::loadNextDexedInstances();
        XRAsyncPSRAMLoader::startAsyncInitOfNextSamples();

        queueAction = QUEUE_ACTION::DONE;

        Serial.println("QUEUE ACTION:: manage track layer step mods queue action DONE!");
        Serial.println("QUEUE ACTION:: queue actions ALL DONE!");

    }

    void handlePatternDequeueActions()
    {
        if (_drawPatternQueueBlink > -1)
        {
            if (_drawPatternQueueBlink == 1)
            {
                XRDisplay::drawSequencerScreen(true);
            }
            else if (_drawPatternQueueBlink == 0)
            {
                XRDisplay::drawSequencerScreen(false);
            }
        }

        if (_dequeuePattern)
        {
            Serial.println("enter _dequeuePattern!");

            if (queueAction != QUEUE_ACTION::DONE) {
                Serial.println("ERROR! CANNOT DEQUEUE PATTERN!");

                return;
            }

            dequeuePattern();
        }
    }

    void dequeuePattern()
    {
        _dequeuePattern = false;
            if (_dequeueLoadNewPatternSamples)
                XRAsyncPSRAMLoader::prePatternChange();

            // IMPORTANT: must change sounds before changing sequencer data!
            //XRSound::saveSoundDataForPatternChange(); // make sure current sounds are saved first
            XRSound::prepareSoundDataForPatternChange(_queuedPatternState.bank, _queuedPatternState.number);
            swapSequencerMemoryForPattern(_queuedPatternState.bank, _queuedPatternState.number);

            XRDexedManager::swapInstances();

            if (_dequeueLoadNewPatternSamples) {
                XRAsyncPSRAMLoader::postPatternChange();
                _dequeueLoadNewPatternSamples = false;
            }
            Serial.println("finished swapping seq mem!");

            // reset queue flags
            _queuedPatternState.lastBank = _queuedPatternState.bank;
            _queuedPatternState.lastNumber = _queuedPatternState.number;
            _queuedPatternState.bank = -1;
            _queuedPatternState.number = -1;
            _drawPatternQueueBlink = -1;
            //_patternQueueBlinked = false;

            _currentStepPage = 1;
            _currentSelectedPage = 0;
            //_currentSelectedTrack = 0;

            auto currentUXMode = XRUX::getCurrentMode();
            if (currentUXMode == XRUX::PATTERN_CHANGE_QUEUED) {
                XRLED::clearAllStepLEDs();

                XRUX::setCurrentMode(XRUX::PATTERN_WRITE);
            }
            else if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE)
            {
                int lastStep = activeTrackLayer.tracks[_currentSelectedTrack].lstep;

                XRLED::displayPageLEDs(
                    1,
                    (_seqState.playbackState == RUNNING),
                    _currentStepPage,
                    lastStep // TODO need?
                );
            }

            XRDisplay::drawSequencerScreen(false);
    }

    void handleTrackLayerDequeueActions()
    {
        if (_drawTrackLayerQueueBlink > -1)
        {
            if (_drawTrackLayerQueueBlink == 1)
            {
                Serial.println("draw track layer blink!");

                XRDisplay::drawSequencerScreen(false); // TODO: pass new arg to let display know to blink the new layer number
            }
            else if (_drawTrackLayerQueueBlink == 0)
            {
                XRDisplay::drawSequencerScreen(false);
            }
        }

        if (_dequeueTrackLayer)
        {
            Serial.println("enter _dequeueTrackLayer!");

            // reset queue flags
            _dequeueTrackLayer = false;
            _drawTrackLayerQueueBlink = -1;
            _currentSelectedTrackLayer = _queuedTrackLayer;
            _queuedTrackLayer = -1;
            // _currentSelectedPage = 0;

            auto currentUXMode = XRUX::getCurrentMode();
            if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE)
            {
                int lastStep = activeTrackLayer.tracks[_currentSelectedTrack].lstep;

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

    void swapSequencerMemoryForTrackLayerChange()
    {
        // swap data
        activeTrackLayer = nextTrackLayer;
    }

    void swapSequencerMemoryForPattern(int newBank, int newPattern)
    {
        XRSD::saveActivePatternToSdCard();
        if (!XRSD::loadNextPattern(newBank, newPattern)){
            initNextPattern();
        }
        
        // swap data
        activePattern = nextPattern;
        // always initialize next pattern?
        activePattern.initialized = true;
        
        // now that NEXT pattern is ACTIVE, save its files?

        XRSD::saveActiveTrackLayerToSdCard();
        // if (!XRSD::loadNextTrackLayer(newBank, newPattern, 0)){
        //     initNextTrackLayer();
        // }

        XRSD::saveActiveTrackStepModLayerToSdCard();
        // if (!XRSD::loadActiveTrackStepModLayerFromSdCard(newBank, newPattern, 0)) {
        //     initActiveTrackStepModLayer();
        // }

        XRSound::applyFxForActivePattern();
        XRSound::applyTrackChokes();

        // if the new pattern has a groove, set it on the clock
        if (activePattern.groove.id > -1) {
            XRClock::setShuffle(true);
            XRClock::setShuffleTemplateForGroove(activePattern.groove.id, activePattern.groove.amount);

            XRClock::setShuffleForAllTracks(true);
            XRClock::setShuffleTemplateForGrooveForAllTracks(activePattern.groove.id, activePattern.groove.amount);
        } else {
            XRClock::setShuffle(false);
            XRClock::setShuffleForAllTracks(false);
        }
        
        swapSequencerMemoryForTrackLayerChange();

        // update currently selected vars
        _currentSelectedBank = newBank;
        _currentSelectedPattern = newPattern;
        _currentSelectedTrack = 0; // when changing patterns, always select first track as default
        _currentSelectedTrackLayer = 0; // when changing patterns, always select first layer as default
    }

    void setSelectedTrack(int8_t track)
    {
        _currentSelectedTrack = track;
    }

    void setSelectedTrackLayer(int8_t layer)
    {
        _currentSelectedTrackLayer = layer;
    }

    void setSelectedPage(int8_t page)
    {
        _currentSelectedPage = page;
    }

    void toggleSelectedStep(uint8_t step)
    {
        int stepToUse = step;

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

        uint8_t stepNum = stepToUse - 1; // get zero based step num

        auto &currTrack = getCurrentSelectedTrack();
        auto currStepState = currTrack.steps[stepNum].state;

        if (currStepState == STEP_STATE::STATE_OFF)
        {
            currTrack.steps[stepNum].state = STEP_STATE::STATE_ON;
        }
        else if (currStepState == STEP_STATE::STATE_ON)
        {
            currTrack.steps[stepNum].state = STEP_STATE::STATE_ACCENTED;
        }
        else if (currStepState == STEP_STATE::STATE_ACCENTED)
        {
            currTrack.steps[stepNum].state = STEP_STATE::STATE_OFF;
        }
    }

    void toggleSequencerPlayback(char btn)
    {
        //Serial.println("enter toggleSequencerPlayback!");

        auto currentUXMode = XRUX::getCurrentMode();

        int8_t currStepChar = XRHelpers::stepCharMap[_seqState.currentStep - 1]; // TODO change type to char?
        uint8_t keyLED = XRLED::getKeyLED(currStepChar);

        if (_seqState.playbackState > STOPPED) { // either RUNNING or PAUSED
            if (_seqState.playbackState == RUNNING && btn == START_BTN_CHAR) {
                XRClock::stop();

                applyRecordingToSequencerWhenStopped();

                // Stopped, so reset sequencer to FIRST step in pattern
                _seqState.currentStep = 1;
                _seqState.currentBar = 1;

                rewindAllCurrentStepsForAllTracks();

                if (
                    currentUXMode == XRUX::UX_MODE::PERFORM_TAP ||
                    currentUXMode == XRUX::UX_MODE::PERFORM_MUTE ||
                    currentUXMode == XRUX::UX_MODE::PERFORM_SOLO ||
                    currentUXMode == XRUX::UX_MODE::PERFORM_RATCHET
                ) {
                    return;
                }

                XRLED::setPWM(keyLED, 0); // turn off current step LED
                XRLED::setPWM(23, 0);     // turn start button led OFF

                if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE) {
                    XRLED::setDisplayStateForAllStepLEDs();
                } else if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE) {
                    XRLED::clearAllStepLEDs();
                }
                
                XRClock::start();

                // _seqState.playbackState = PAUSED;

                // XRClock::pause(); // toggle pause ON

                // XRLED::setPWM(23, 0);

                // if (
                //     currentUXMode == XRUX::UX_MODE::PERFORM_TAP ||
                //     currentUXMode == XRUX::UX_MODE::PERFORM_MUTE ||
                //     currentUXMode == XRUX::UX_MODE::PERFORM_SOLO ||
                //     currentUXMode == XRUX::UX_MODE::PERFORM_RATCHET
                // ) {
                //     return;
                // }

                // XRLED::setPWM(keyLED, 0);

                // if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE) {
                //     int8_t currentSelectedTrackCurrentBar = _seqState.currentTrackSteps[_currentSelectedTrack].currentBar;
                //     auto &currTrack = getCurrentSelectedTrack();

                //     XRLED::displayPageLEDs(
                //         currentSelectedTrackCurrentBar,
                //         (_seqState.playbackState == RUNNING),
                //         _currentStepPage,
                //         currTrack.lstep
                //     );

                //     XRLED::setDisplayStateForAllStepLEDs(); // TODO: wrap with "if in track view display step LED state" conditional, etc
                // } else if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE) {
                //     XRLED::clearAllStepLEDs();
                // }
            } else if (_seqState.playbackState == PAUSED && btn == START_BTN_CHAR) {
                // // Unpaused, so advance sequencer from last known step
                // _seqState.playbackState = RUNNING;

                // XRClock::pause(); // toggle pause OFF
            } else if (btn == STOP_BTN_CHAR) {
                _seqState.playbackState = STOPPED;

                XRClock::stop();

                applyRecordingToSequencerWhenStopped();

                // Stopped, so reset sequencer to FIRST step in pattern
                _seqState.currentStep = 1;
                _seqState.currentBar = 1;

                rewindAllCurrentStepsForAllTracks();

                if (
                    currentUXMode == XRUX::UX_MODE::PERFORM_TAP ||
                    currentUXMode == XRUX::UX_MODE::PERFORM_MUTE ||
                    currentUXMode == XRUX::UX_MODE::PERFORM_SOLO ||
                    currentUXMode == XRUX::UX_MODE::PERFORM_RATCHET
                ) {
                    return;
                }

                XRLED::setPWM(keyLED, 0); // turn off current step LED
                XRLED::setPWM(23, 0);     // turn start button led OFF

                if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE) {
                    XRLED::setDisplayStateForAllStepLEDs();
                } else if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE) {
                    XRLED::clearAllStepLEDs();
                }
            }
        } else if (btn == START_BTN_CHAR) {
            // Started, so start sequencer from FIRST step in pattern
            //_seqState.current_step = 1;
            _seqState.playbackState = RUNNING;

            // make sure pattern fx are unmuted
            XRSound::applyFxForActivePattern();

            XRClock::start();
        } else if (btn == STOP_BTN_CHAR) {
           // Already stopped, so disable all sound output when stop btn is pressed again
           XRSound::turnOffAllSounds();
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

        if (_ratchetDivision > -1 && !(tick % (_ratchetDivision*4)))
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
        for (size_t i = 0; i < RATCHET_STACK_SIZE; i++)
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
        for (size_t i = 0; i < STEP_STACK_SIZE; i++)
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
        switch (XRSound::activePatternSounds[track].type)
        {
        case XRSound::T_MONO_SAMPLE:
            XRSound::handleMonoSampleNoteOnForTrack(track);

            break;
        case XRSound::T_MONO_SYNTH:
            XRSound::handleMonoSynthNoteOnForTrack(track);

            break;
        case XRSound::T_DEXED_SYNTH:
            XRSound::handleDexedSynthNoteOnForTrack(track);

            break;
        case XRSound::T_BRAIDS_SYNTH:
            XRSound::handleBraidsNoteOnForTrack(track);

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
        activeTrackLayer.tracks[_currentSelectedTrack].initialized = true;
    }

    void queuePattern(int pattern, int bank)
    {
        queueAction = QUEUE_ACTION::NONE;
        queueErrorLogged = false;

        _queuedPatternState.bank = bank;
        _queuedPatternState.number = pattern;
    }

    void setSelectedPattern(int8_t pattern)
    {
        _currentSelectedPattern = pattern;
    }

    void saveCurrentPatternOffHeap()
    {
        // push current heap memory to RAM2/DMAMEM
        // activePatternBank.patterns[_currentSelectedPattern] = activePattern;
    }

    void setCurrentSelectedStep(int step)
    {
       // activePattern.tracks[_currentSelectedTrack].steps[step];

        _currentSelectedStep = step;
    }

    void setCurrentStepPage(int8_t page)
    {
        _currentStepPage = page;
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