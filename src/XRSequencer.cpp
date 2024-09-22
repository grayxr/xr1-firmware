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
#include <XRVersa.h>

namespace XRSequencer
{
    // private variables
 
    SEQUENCER_STATE _seqState;
    RATCHET_STATE _ratchetState;
    QUEUED_PATTERN_STATE _queuedPatternState;

    STACK_STEP_DATA _stepStack[STEP_STACK_SIZE];
    STACK_STEP_DATA _ignoredStepStack[STEP_STACK_SIZE];
    STACK_RATCHET_DATA _ratchetStack[RATCHET_STACK_SIZE];

    int8_t _currentSelectedBank = 0;        // default to 0 (first)
    int8_t _currentSelectedPattern = 0;     // default to 0 (first)
    int8_t _currentSelectedTrack = 0;       // default to 0 (first)
    int8_t _currentSelectedTrackLayer = 0;  // default to 0 (first)
    int8_t _currentSelectedStep = -1;       // default to -1 (none)
    int8_t _currentStepPage = 1;
    int8_t _currentSelectedPage = 0;
    int8_t _ratchetTrack = 0;
    int8_t _ratchetDivision = -1;
    int8_t _ratchetPageNum = 0;
    int8_t _currentSelectedRatchetStep = -1;

    bool metronomeEnabled = false;
    bool ratchetLatched = false;

    bool _initTracks[MAXIMUM_SEQUENCER_TRACKS];

    bool _recording = false;
    bool _dequeuePattern = false;
    bool _patternQueueBlinked = false;
    bool _dequeueTrackLayer = false;
    bool _trackLayerBlinked = false;

    int _queuedTrackLayer = -1;
    int _drawPatternQueueBlink = -1;
    int _drawTrackLayerQueueBlink = -1;

    uint8_t _bpmBlinkTimer = 2;

    // extern globals

    EXTMEM PATTERN activePattern;
    EXTMEM PATTERN idlePattern;

    DMAMEM PATTERN_FX_PAGE_INDEXES patternFxPages[MAXIMUM_PATTERN_FX_PARAM_PAGES];
    DMAMEM TRACK_PERFORM_STATE trackPerformState[MAXIMUM_SEQUENCER_TRACKS];
    DMAMEM RECORDING_STATE recordingState;

    TRACK_TRIGGER_STATE trackTriggerState;

    bool init()
    {
        _currentSelectedBank = 0;
        _currentSelectedPattern = 0;
        _currentSelectedTrack = 0;
        _currentSelectedTrackLayer = 0;

        // init pattern fx pages
        patternFxPages[0] = PATTERN_FX_PAGE_INDEXES::DELAY;
        patternFxPages[1] = PATTERN_FX_PAGE_INDEXES::NA;
        patternFxPages[2] = PATTERN_FX_PAGE_INDEXES::NA;
        patternFxPages[3] = PATTERN_FX_PAGE_INDEXES::NA;

        for (int s = 0; s < STEP_STACK_SIZE; s++)
        {
            _stepStack[s].length = -1;
            _stepStack[s].trackNum = -1;
            _stepStack[s].stepNum = -1;

            _ignoredStepStack[s].length = -1;
            _ignoredStepStack[s].trackNum = -1;
            _ignoredStepStack[s].stepNum = -1;
        }

        for (int r = 0; r < RATCHET_STACK_SIZE; r++)
        {
            _ratchetStack[r].length = -1;
            _ratchetStack[r].trackNum = -1;
        }

        // init perform state
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            trackPerformState[t].muted = false;
            trackPerformState[t].soloing = false;
        }

        // init recording state
        for (int rec = 0; rec < MAXIMUM_SEQUENCER_TRACKS; rec++)
        {
            for (int rs = 0; rs < MAXIMUM_SEQUENCER_STEPS; rs++)
            {
                recordingState.tracks[rec].steps[rs].state = STEP_STATE::STATE_OFF;
                recordingState.tracks[rec].steps[rs].length = 4;
                recordingState.tracks[rec].steps[rs].note = 0;
                recordingState.tracks[rec].steps[rs].queued = false;
            }
        }

        initActivePattern();

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
        auto &track = activePattern.layers[_currentSelectedTrackLayer].tracks[t];
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
            auto &track = activePattern.layers[_currentSelectedTrackLayer].tracks[t];
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
            auto &track = activePattern.layers[_currentSelectedTrackLayer].tracks[t];

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

    bool isRatchetActive()
    {
        return _ratchetDivision > -1;
    }

    void handle96PPQN(uint32_t tick)
    {
        auto currentUXMode = XRUX::getCurrentMode();
        
        if (onRatchetStepPage())
        {
            if (isRatchetActive() && !(tick % (_ratchetDivision*4))) // while ratcheting
            {
                XRLED::setDisplayStateForAllStepLEDs();
            }

            if (!(tick % _bpmBlinkTimer)) {
                if (isRatchetActive() && !(tick % (_ratchetDivision*4))) // while ratcheting
                {
                    handleRatchetTrackStepLEDs(_ratchetTrack);
                }
            }
        }

        triggerRatchetingTrack(tick);

        // step should be removed from stack at rate of 96ppqn / 6
        if (!(tick % 6)) { 
            handleRemoveFromStepStack(tick);
            handleRemoveFromIgnoredStepStack(tick);
        }

        if (!(tick % 2) && currentUXMode == XRUX::UX_MODE::PATTERN_WRITE)
        {
            displayAllTrackNoteOnLEDs(false);
        }

        if (_recording) {
            XRLED::setPWM(23, 512);
            //XRLED::setPWM(23, 1024);
        }

        if (!(tick % 6))
        {
            XRLED::setPWM(23, 0); // turn start button led OFF every 16th note

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

            if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE)
            {
                int lastStep = (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE) ? activePattern.lstep : activePattern.layers[_currentSelectedTrackLayer].tracks[_currentSelectedTrack].lstep;

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

    void ClockOut16PPQN(uint32_t tick)
    {
        auto currentUXMode = XRUX::getCurrentMode();
        if (currentUXMode == XRUX::UX_MODE::PROJECT_INITIALIZE || currentUXMode == XRUX::UX_MODE::PROJECT_BUSY)
        {
            return;
        }

        // NOTE: this function is affected by swing

        if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE)
        {
            displayAllTrackNoteOnLEDs(true);
        }

        // if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE || currentUXMode == XRUX::UX_MODE::SUBMITTING_STEP_VALUE)
        // {
        //     XRLED::setDisplayStateForAllStepLEDs();

        //     int lastStep = (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE) ? activePattern.lstep : activePattern.layers[_currentSelectedTrackLayer].tracks[_currentSelectedTrack].lstep;

        //     if (!XRKeyMatrix::isFunctionActive())
        //     {
        //         XRLED::displayPageLEDs(
        //             -1,
        //             (_seqState.playbackState == RUNNING),
        //             _currentStepPage,
        //             lastStep
        //         );   
        //     }
        // }

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

            if (metronomeEnabled) {
                auto plstp = activePattern.lstep;
                auto lsDivBy16 = (plstp >= 16 && !(plstp % 16));
                //auto lsDivBy12 = (plstp >= 12 && !(plstp % 12));
                
                bool accented = false;
                if ( _seqState.currentStep == 1 || (lsDivBy16 && !((_seqState.currentStep-1)%16)))
                {
                    accented = true;
                }

                XRSound::handleNoteOnForMetronome(accented);
                //XRDisplay::toggleMetronomeDirection();
                //XRDisplay::drawSequencerScreen(false);
            }
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
        auto &track = activePattern.layers[_currentSelectedTrackLayer].tracks[t];
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

    void handleRatchetTrackStepLEDs(uint8_t t)
    {
        auto currentUXMode = XRUX::getCurrentMode();
        auto currStepChar = XRHelpers::stepCharMap[_ratchetState.currentStep];
        auto keyLED = XRLED::getKeyLED(currStepChar);

        if (onRatchetStepPage())
        {
            bool transitionStepLEDs = (_ratchetState.currentStep <= 16);
            bool turnOffLastLED = (_ratchetState.currentStep == 1);

            if (_ratchetState.currentStep > 1)
            {
                if (transitionStepLEDs)
                {
                    uint8_t prevKeyLED = XRLED::getKeyLED(XRHelpers::stepCharMap[_ratchetState.currentStep - 1]);

                    auto &currTrack = activePattern.ratchetLayer.tracks[t];
                    auto prevTrackStepData = currTrack.steps[_ratchetState.currentStep-2];

                    if (prevTrackStepData.state == STEP_STATE::STATE_ACCENTED)
                    {
                        XRLED::setPWM(prevKeyLED, 4095); // turn prev sixteenth led ACCENTED ON
                    } else if (prevTrackStepData.state == STEP_STATE::STATE_ON) {
                        XRLED::setPWM(prevKeyLED, 512); // turn prev sixteenth led ON
                    } else {
                        XRLED::setPWM(prevKeyLED, 0); // turn prev sixteenth led OFF
                    }
                }
            } 
            else if (turnOffLastLED) 
            {
                XRLED::setDisplayStateForAllStepLEDs();
                // auto &currTrack = activePattern.ratchetLayer.tracks[t];
                // auto currTrackLastStep = currTrack.lstep;

                // uint8_t prevKeyLED = XRLED::getKeyLED(XRHelpers::stepCharMap[currTrackLastStep]);

                // XRLED::setPWM(prevKeyLED, 0); // turn prev sixteenth led OFF
            }

            if (transitionStepLEDs)
            {
                auto &currTrack = activePattern.ratchetLayer.tracks[t];
                auto currTrackStepData = currTrack.steps[_ratchetState.currentStep-1];

                if (currTrackStepData.state != STEP_STATE::STATE_OFF)
                {
                    XRLED::setPWM(keyLED, currTrackStepData.state == STEP_STATE::STATE_ON ? 4095 : 0); // turn sixteenth led OFF
                } else {
                    XRLED::setPWM(keyLED, 4095); // turn sixteenth led ON
                }
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

        if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE)
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

                    auto &currTrack = activePattern.layers[_currentSelectedTrackLayer].tracks[t];
                    auto prevTrackStepData = currTrack.steps[currentSelectedTrackCurrentStep-2];

                    if (prevTrackStepData.state == STEP_STATE::STATE_ACCENTED)
                    {
                        XRLED::setPWM(prevKeyLED, 4095); // turn prev sixteenth led ACCENTED ON
                    } else if (prevTrackStepData.state == STEP_STATE::STATE_ON) {
                        XRLED::setPWM(prevKeyLED, 512); // turn prev sixteenth led ON
                    } else {
                        XRLED::setPWM(prevKeyLED, 0); // turn prev sixteenth led OFF
                    }
                }
            }
            else
            {
                if (turnOffLastLED)
                {
                    XRLED::setDisplayStateForAllStepLEDs();
                    // Serial.println("turning off last LED");

                    // auto &currTrack = getCurrentSelectedTrack();
                    // auto currTrackLastStep = currTrack.lstep;

                    // if (currTrackLastStep > 16)
                    // {
                    //     currTrackLastStep -= 16;
                    // }

                    // uint8_t prevKeyLED = XRLED::getKeyLED(XRHelpers::stepCharMap[currTrackLastStep]);

                    // XRLED::setPWM(prevKeyLED, 0); // turn prev sixteenth led OFF
                }
            }

            if (transitionStepLEDs)
            {
                auto &currTrack = activePattern.layers[_currentSelectedTrackLayer].tracks[t];
                auto currTrackStepData = currTrack.steps[currentSelectedTrackCurrentStep-1];

                if (currTrackStepData.state != STEP_STATE::STATE_OFF)
                {
                    XRLED::setPWM(keyLED, currTrackStepData.state == STEP_STATE::STATE_ON ? 4095 : 0); // turn sixteenth led OFF
                } else {
                    XRLED::setPWM(keyLED, 4095); // turn sixteenth led ON
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
                    auto track = _stepStack[i].trackNum;

                    // handleNoteOffForTrackStep(_stepStack[i].trackNum, _stepStack[i].stepNum);
                    trackTriggerState.trackTriggers[track].currentState = 0;
                    trackTriggerState.trackTriggers[track].pattern = _currentSelectedPattern;
                    trackTriggerState.trackTriggers[track].layer = _currentSelectedTrackLayer;
                    trackTriggerState.trackTriggers[track].step = _stepStack[i].stepNum;

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
            XRSound::handleNoteOffForTrack(t);
        }

        // clear step based notes
        for (int i = 0; i < STEP_STACK_SIZE; i++)
        {
            if (_stepStack[i].length > 0)
            {
                // handleNoteOffForTrackStep(_stepStack[i].trackNum, _stepStack[i].stepNum);

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
        return idlePattern;
    }

    TRACK &getTrack(int track)
    {
        return activePattern
            .layers[_currentSelectedTrackLayer]
            .tracks[track];
    }

    STEP &getStep(int track, int step)
    {
        return activePattern
            .layers[_currentSelectedTrackLayer]
            .tracks[track]
            .steps[step];
    }

    PATTERN &getCurrentSelectedPattern()
    {
        return activePattern;
    }

    TRACK &getCurrentSelectedTrack()
    {
        return activePattern
            .layers[_currentSelectedTrackLayer]
            .tracks[_currentSelectedTrack];
    }

    TRACK_LAYER &getCurrentSelectedTrackLayer()
    {
        return activePattern.layers[_currentSelectedTrackLayer];
    }

    STEP &getCurrentSelectedTrackStep()
    {
        return activePattern
            .layers[_currentSelectedTrackLayer]
            .tracks[_currentSelectedTrack]
            .steps[_currentSelectedStep];
    }

    int8_t getRatchetTrack()
    {
        return _ratchetTrack;
    }

    bool isRatchetAccented()
    {
        return false; // _isRatchetAccented
    }

    void toggleIsRatchetAccented(bool enable)
    {
        //_isRatchetAccented = enable;
    }

    int8_t getCurrentRatchetPageNum()
    {
        return _ratchetPageNum;
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
            // ignore this track's LED if currently playing a note on the keys
            if (t-1 == _currentSelectedTrack && XRVersa::getKeyboardNotesHeld() > 0) continue;

            auto &currTrack = activePattern.layers[_currentSelectedTrackLayer].tracks[t - 1];
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
            else if (currTrackStepForLED.state == STEP_STATE::STATE_ON)
            {
                XRLED::setPWM(keyLED, enable ? 512 : 0);
            }
            else if (currTrackStepForLED.state == STEP_STATE::STATE_ACCENTED)
            {
                XRLED::setPWM(keyLED, enable ? 4095 : 0);
            }
        }
    }

    void triggerAllStepsForTrack(uint8_t t, uint32_t tick) {
        int8_t currTrackStep = _seqState.currentTrackSteps[t].currentStep - 1; // get zero-based track step number

        auto &currTrack = activePattern.layers[_currentSelectedTrackLayer].tracks[t];
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
        auto &currTrack = activePattern.layers[_currentSelectedTrackLayer].tracks[track];
        auto prob = currTrack.probability;

        if (currTrack.steps[step].flags[PROBABILITY]) {
            prob = currTrack.steps[step].mods[PROBABILITY];
        }

        if (prob < 100) {
            Serial.println("step probability: " + String(prob));

            uint8_t r = random(99);

            if (prob == 0 || (r > prob)) {
                return false;
            }
        }

        return true;
    }

    void handleAddToStepStack(uint32_t tick, int track, int step)
    {
        // TODO handle concurrent ratcheting and step stacking
        // by using separate trigger trackers
        if (_ratchetTrack == track && isRatchetActive())  // if ratcheting is enabled
        {
            return;
        }

        auto &trackToUse = getTrack(track);

        int lenStepMod = trackToUse.steps[step].mods[LENGTH];
        bool lenStepModEnabled = trackToUse.steps[step].flags[LENGTH];

        for (uint8_t i = 0; i < STEP_STACK_SIZE; i++)
        {
            if (!trackPerformState[track].muted && _stepStack[i].length == -1)
            {
                _stepStack[i].trackNum = track;
                _stepStack[i].stepNum = step;
                _stepStack[i].length = lenStepModEnabled ? lenStepMod : trackToUse.length;

                trackTriggerState.trackTriggers[track].lastState = 0;
                trackTriggerState.trackTriggers[track].currentState = 1; // <-- this is important for triggering
                trackTriggerState.trackTriggers[track].pattern = _currentSelectedPattern;
                trackTriggerState.trackTriggers[track].layer = _currentSelectedTrackLayer;
                trackTriggerState.trackTriggers[track].step = step;
                trackTriggerState.trackTriggers[track].soundType = XRSound::activeSounds[track].type;

                // Serial.printf("added to step stack: track: %d, step: %d, length: %d\n", track, step, _stepStack[i].length);

                // handleNoteOnForTrackStep(_stepStack[i].trackNum, _stepStack[i].stepNum, trackTriggerState.trackTriggers[0]);

                return;
            }
        }
    }

    void handleAddToIgnoredStepStack(uint32_t tick, int track, int step)
    {
        auto &trackToUse = getTrack(track);

        int lenStepMod = trackToUse.steps[step].mods[LENGTH];
        bool lenStepModEnabled = trackToUse.steps[step].flags[LENGTH];

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

    void handleNoteOnForTrackStep(int track, int step, TRACK_TRIGGER trigger)
    {
        // if (_currentSelectedPattern != trigger.pattern) {
        //     Serial.println("FIX: DO NOT TRIGGER FOR WRONG PATTERN!");
        //     return;
        // }

        // if (_currentSelectedTrackLayer != trigger.layer) {
        //     Serial.println("FIX: DO NOT TRIGGER FOR WRONG TRACK LAYER!");
        //     return;
        // }

        // if (XRSound::activeSounds[track].type != trigger.soundType) {
        //     Serial.println("FIX: DO NOT TRIGGER FOR WRONG SOUND TYPE!");
        //     return;
        // }

        if (trackPerformState[track].muted){
            return;
        }

        if (step == 0 && XRSound::soundNeedsReinit[track]) {
            for (int d = 0; d < 4; d++)
            {
                if (XRSound::activeSounds[d].type == XRSound::T_DEXED_SYNTH) {
                    XRSound::reinitSoundForTrack(track);

                    auto a = XRDexedManager::getActiveInstanceForTrack(d);
                    auto in = XRDexedManager::getInactiveInstanceForTrack(d);

                    // enable active instance, mute inactive instance
                    // TODO: fade out quick instead?
                    AudioNoInterrupts();
                    XRSound::dexedInstances[a].amp.gain(1);
                    XRSound::dexedInstances[in].amp.gain(0);
                    XRSound::dexedInstances[in].dexed.notesOff();
                    AudioInterrupts();
                }
            }

            // if (track == 0) {
            //     Serial.printf("reinit sound for track: %d\n", track);

            //     XRSound::reinitSoundForTrack(track);
            //     // TODO: apply specific track choke instead of reapplying all chokes here
            //     XRSound::applyTrackChokes();
            // }

            Serial.printf("reinit sound for track: %d\n", track);

            XRSound::reinitSoundForTrack(track);

            // TODO: apply specific track choke instead of reapplying all chokes here
            XRSound::applyTrackChokes();
        }

        if (XRSound::soundNeedsReinit[track])
        {
            Serial.printf("reinit sounds for track: %d\n", track);

            XRSound::reinitSoundForTrack(track);

            // TODO: apply specific track choke instead of reapplying all chokes here
            XRSound::applyTrackChokes();
        }

        switch (XRSound::activeSounds[track].type)
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
            auto l = getCurrentSelectedTrackLayerNum();
            auto &track = activePattern.layers[l].tracks[t];
            auto trackLastStep = track.lstep;
            auto trackCurrentStep = _seqState.currentTrackSteps[t].currentStep;

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
        activePattern.fx = getInitPatternFxParams();

        for (size_t l = 0; l < MAXIMUM_SEQUENCER_TRACK_LAYERS; l++) {

            for (size_t t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
            {
                activePattern.layers[l].tracks[t].length = 4;
                activePattern.layers[l].tracks[t].note = 0;
                activePattern.layers[l].tracks[t].octave = 4;
                activePattern.layers[l].tracks[t].velocity = 50;
                activePattern.layers[l].tracks[t].probability = 100;
                activePattern.layers[l].tracks[t].lstep = DEFAULT_LAST_STEP;
                activePattern.layers[l].tracks[t].initialized = false;

                // now fill in steps
                for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
                {
                    activePattern.layers[l].tracks[t].steps[s].state = STEP_STATE::STATE_OFF;

                    for (int m = 0; m < MAXIMUM_TRACK_MODS; m++)
                    {
                        activePattern.layers[l].tracks[t].steps[s].mods[m] = 0;
                        activePattern.layers[l].tracks[t].steps[s].flags[m] = false;
                    }
                }
            }
        }

        // ratchet layer stuff
        for (size_t rt = 0; rt < MAXIMUM_SEQUENCER_TRACKS; rt++)
        {
            activePattern.ratchetLayer.tracks[rt].length = 2;
            activePattern.ratchetLayer.tracks[rt].note = 0;
            activePattern.ratchetLayer.tracks[rt].octave = 4;
            activePattern.ratchetLayer.tracks[rt].velocity = 50;
            activePattern.ratchetLayer.tracks[rt].probability = 100;
            activePattern.ratchetLayer.tracks[rt].lstep = DEFAULT_LAST_STEP;
            activePattern.ratchetLayer.tracks[rt].initialized = false;

            // now fill in steps
            for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
            {
                activePattern.ratchetLayer.tracks[rt].steps[s].state = STEP_STATE::STATE_ON;

                for (int m = 0; m < MAXIMUM_TRACK_MODS; m++)
                {
                    activePattern.ratchetLayer.tracks[rt].steps[s].mods[m] = 0;
                    activePattern.ratchetLayer.tracks[rt].steps[s].flags[m] = false;
                }
            }
        }
    }

    void initIdlePattern()
    {
        idlePattern.initialized = true; // always initialize first pattern?
        idlePattern.lstep = DEFAULT_LAST_STEP;
        idlePattern.groove.amount = 0;
        idlePattern.groove.id = -1;
        idlePattern.accent = DEFAULT_GLOBAL_ACCENT;
        idlePattern.fx = getInitPatternFxParams();

        for (size_t l = 0; l < MAXIMUM_SEQUENCER_TRACK_LAYERS; l++)
        {
            for (size_t t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
            {
                idlePattern.layers[l].tracks[t].length = 4;
                idlePattern.layers[l].tracks[t].note = 0;
                idlePattern.layers[l].tracks[t].octave = 4;
                idlePattern.layers[l].tracks[t].velocity = 50;
                idlePattern.layers[l].tracks[t].probability = 100;
                idlePattern.layers[l].tracks[t].lstep = DEFAULT_LAST_STEP;
                idlePattern.layers[l].tracks[t].initialized = false;

                // now fill in steps
                for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
                {
                    idlePattern.layers[l].tracks[t].steps[s].state = STEP_STATE::STATE_OFF;

                    for (int m = 0; m < MAXIMUM_TRACK_MODS; m++)
                    {
                        idlePattern.layers[l].tracks[t].steps[s].mods[m] = 0;
                        idlePattern.layers[l].tracks[t].steps[s].flags[m] = false;
                    }
                }
            }
        }

        // ratchet layer stuff
        for (size_t rt = 0; rt < MAXIMUM_SEQUENCER_TRACKS; rt++)
        {
            idlePattern.ratchetLayer.tracks[rt].length = 2;
            idlePattern.ratchetLayer.tracks[rt].note = 0;
            idlePattern.ratchetLayer.tracks[rt].octave = 4;
            idlePattern.ratchetLayer.tracks[rt].velocity = 50;
            idlePattern.ratchetLayer.tracks[rt].probability = 100;
            idlePattern.ratchetLayer.tracks[rt].lstep = DEFAULT_LAST_STEP;
            idlePattern.ratchetLayer.tracks[rt].initialized = false;

            // now fill in steps
            for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
            {
                idlePattern.ratchetLayer.tracks[rt].steps[s].state = STEP_STATE::STATE_ON;

                for (int m = 0; m < MAXIMUM_TRACK_MODS; m++)
                {
                    idlePattern.ratchetLayer.tracks[rt].steps[s].mods[m] = 0;
                    idlePattern.ratchetLayer.tracks[rt].steps[s].flags[m] = false;
                }
            }
        }
    }

    void initActiveTrackLayer()
    {
        // for (size_t t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++) {
        //     activeTrackLayer.tracks[t].length = 4;
        //     activeTrackLayer.tracks[t].note = 0;
        //     activeTrackLayer.tracks[t].octave = 4;
        //     activeTrackLayer.tracks[t].velocity = 50;
        //     activeTrackLayer.tracks[t].probability = 100;
        //     activeTrackLayer.tracks[t].lstep = DEFAULT_LAST_STEP;
        //     activeTrackLayer.tracks[t].initialized = false;

        //     // now fill in steps
        //     for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
        //     {
        //         activeTrackLayer.tracks[t].steps[s].state = STEP_STATE::STATE_OFF;
        //     }
        // }
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
        // for (size_t t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++) {
        //     nextTrackLayer.tracks[t].length = 4;
        //     nextTrackLayer.tracks[t].note = 0;
        //     nextTrackLayer.tracks[t].octave = 4;
        //     nextTrackLayer.tracks[t].velocity = 50;
        //     nextTrackLayer.tracks[t].probability = 100;
        //     nextTrackLayer.tracks[t].lstep = DEFAULT_LAST_STEP;
        //     nextTrackLayer.tracks[t].initialized = false;

        //     // now fill in steps
        //     for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
        //     {
        //         nextTrackLayer.tracks[t].steps[s].state = STEP_STATE::STATE_OFF;
        //     }
        // }
    }

    void initActiveTrackStepModLayer()
    {
        // for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        // {
        //     for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
        //     {
        //         for (int m = 0; m < MAXIMUM_TRACK_MODS; m++)
        //         {
        //             activeTrackStepModLayer.tracks[t].steps[s].mods[m] = 0;
        //             activeTrackStepModLayer.tracks[t].steps[s].flags[m] = false;
        //         }
        //     }
        // }
    }

    PATTERN_FX_PARAMS getInitPatternFxParams()
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

            _dequeuePattern = false;

            // TODO: fix pattern dequeueing
            _queuedPatternState.bank = -1;
            _queuedPatternState.number = -1;
            _drawPatternQueueBlink = -1;
            XRUX::setCurrentMode(XRUX::UX_MODE::PATTERN_WRITE);
            return;

            // IMPORTANT: must change sounds before changing sequencer data!
            XRSound::saveSoundDataForPatternChange(); // make sure current sounds are saved first
            XRSound::loadSoundDataForPatternChange(_queuedPatternState.bank, _queuedPatternState.number);

            swapSequencerMemoryForPattern(_queuedPatternState.bank, _queuedPatternState.number);

            XRDexedManager::swapInstances();

            Serial.println("finished swapping seq mem!");

            // reset queue flags
            _queuedPatternState.bank = -1;
            _queuedPatternState.number = -1;
            _drawPatternQueueBlink = -1;
            //_patternQueueBlinked = false;

            _currentStepPage = 1;
            _currentSelectedPage = 0;
            _ratchetTrack = 0;
            _ratchetPageNum = 0;
            //_currentSelectedTrack = 0;

            auto currentUXMode = XRUX::getCurrentMode();
            if (currentUXMode == XRUX::PATTERN_CHANGE_QUEUED) {
                XRLED::clearAllStepLEDs();

                XRUX::setCurrentMode(XRUX::PATTERN_WRITE);
            }
            else if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE)
            {
                int lastStep = activePattern.layers[_currentSelectedTrackLayer].tracks[_currentSelectedTrack].lstep;

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

    void handleTrackLayerQueueActions()
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
                int lastStep = activePattern.layers[_currentSelectedTrackLayer].tracks[_currentSelectedTrack].lstep;

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
        // activeTrackLayer = nextTrackLayer;
    }

    void swapSequencerMemoryForPattern(int newBank, int newPattern)
    {
        XRSD::saveActivePatternToSdCard();

        // swap data
        activePattern = idlePattern;

        // always initialize next pattern?
        activePattern.initialized = true;

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

        auto &currTrack = onRatchetStepPage() ? 
            activePattern.ratchetLayer.tracks[_ratchetTrack] : 
            getCurrentSelectedTrack();

        auto currStepState = onRatchetStepPage() ? 
            activePattern.ratchetLayer.tracks[_ratchetTrack].steps[stepNum].state : 
            currTrack.steps[stepNum].state;

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
                _ratchetState.firstBar = true;

                rewindAllCurrentStepsForAllTracks();

                // if (
                //     currentUXMode == XRUX::UX_MODE::PERFORM_TAP ||
                //     currentUXMode == XRUX::UX_MODE::PERFORM_MUTE ||
                //     currentUXMode == XRUX::UX_MODE::PERFORM_SOLO ||
                //     currentUXMode == XRUX::UX_MODE::PERFORM_RATCHET
                // ) {
                //     return;
                // }

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
                _ratchetState.firstBar = true;

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

    void setCurrentRatchetStep(int8_t step)
    {
        _ratchetState.currentStep = step;
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
        handleRemoveFromRatchetStack();

        if (XRUX::getCurrentMode() != XRUX::PERFORM_RATCHET && XRUX::getCurrentMode() != XRUX::SUBMITTING_RATCHET_STEP_VALUE) {
            return;
        }

        if (isRatchetActive() && !(tick % (_ratchetDivision*4))) // while ratcheting
        {
            // Serial.print("in ratchet division! tick: ");
            // Serial.print(tick);
            // Serial.print(" ratchet_division: ");
            // Serial.println(_ratchetDivision);

            int8_t currRatchetStep = _ratchetState.currentStep - 1; // get zero-based ratchet step number

            auto &ratchetTrack = activePattern.ratchetLayer.tracks[_ratchetTrack];
            auto &currTrackStepData = ratchetTrack.steps[currRatchetStep];
            
            if ((currTrackStepData.state == STEP_STATE::STATE_ON) || (currTrackStepData.state == STEP_STATE::STATE_ACCENTED))
            {
                //if (isStepProbablyEnabled(t, currRatchetStep)) {
                    handleAddToRatchetStack(currRatchetStep);
                // } else {
                //     handleAddToIgnoredStepStack(tick, t, currTrackStep);
                // }
            }

            auto trackLastStep = ratchetTrack.lstep;
            auto trackCurrentStep = _ratchetState.currentStep;

            if (trackCurrentStep <= trackLastStep)
            {
                if (trackCurrentStep < trackLastStep)
                {
                    ++_ratchetState.currentStep; // advance current step for ratchet track
                }
                else
                {
                    _ratchetState.currentStep = 1; // reset current step for ratchet track
                    _ratchetState.firstBar = false;
                }
            }
        }
    }

    void handleRemoveFromRatchetStack()
    {
        for (size_t i = 0; i < RATCHET_STACK_SIZE; i++)
        {
            auto trk = _ratchetStack[i].trackNum;

            if (_ratchetStack[i].length != -1)
            {
                --_ratchetStack[i].length;

                if (_ratchetStack[i].length == 0)
                {
                    // handleNoteOffForTrack(_ratchetStack[i].trackNum);
                    trackTriggerState.trackTriggers[trk].currentState = 0;
                    trackTriggerState.trackTriggers[trk].pattern = _currentSelectedPattern;
                    trackTriggerState.trackTriggers[trk].layer = _currentSelectedTrackLayer;
                    trackTriggerState.trackTriggers[trk].step = -1;
                    trackTriggerState.trackTriggers[trk].soundType = XRSound::activeSounds[trk].type;

                    // re-initialize stack entry
                    _ratchetStack[i].trackNum = -1;
                    _ratchetStack[i].length = -1;
                }
            }
        }
    }

    void handleAddToRatchetStack(int step)
    {
        auto &currRatchetTrack = activePattern.ratchetLayer.tracks[_ratchetTrack];

        for (size_t i = 0; i < RATCHET_STACK_SIZE; i++)
        {
            if (_ratchetStack[i].length == -1)
            {
                _ratchetStack[i].trackNum = _ratchetTrack;
                _ratchetStack[i].length = currRatchetTrack.length;

                // handleNoteOnForTrack(_ratchetStack[i].trackNum, trackTriggerState.trackTriggers[0]);
                trackTriggerState.trackTriggers[_ratchetStack[i].trackNum].currentState = 1;
                trackTriggerState.trackTriggers[_ratchetStack[i].trackNum].pattern = _currentSelectedPattern;
                trackTriggerState.trackTriggers[_ratchetStack[i].trackNum].layer = _currentSelectedTrackLayer;
                trackTriggerState.trackTriggers[_ratchetStack[i].trackNum].step = step;
                trackTriggerState.trackTriggers[_ratchetStack[i].trackNum].soundType = XRSound::activeSounds[_ratchetStack[i].trackNum].type;

                return;
            }
        }
    }

    void handleNoteOnForTrack(int track, TRACK_TRIGGER trigger)
    {
        // if (_currentSelectedPattern != trigger.pattern) {
        //     Serial.println("FIX: DO NOT TRIGGER FOR WRONG PATTERN!");
        //     return;
        // }

        // if (_currentSelectedTrackLayer != trigger.layer) {
        //     Serial.println("FIX: DO NOT TRIGGER FOR WRONG TRACK LAYER!");
        //     return;
        // }

        // if (XRSound::activeSounds[track].type != trigger.soundType) {
        //     Serial.println("FIX: DO NOT TRIGGER FOR WRONG SOUND TYPE!");
        //     return;
        // }

        switch (XRSound::activeSounds[track].type)
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

    void handleTriggerStates()
    {
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            if (trackTriggerState.trackTriggers[t].currentState != trackTriggerState.trackTriggers[t].lastState)
            {
                //Serial.println("trigger state changed!");

                if (trackTriggerState.trackTriggers[t].step > -1) {
                    if (trackTriggerState.trackTriggers[t].currentState == 1)
                    {
                        //Serial.printf("triggering ON step %d for track %d\n", trackTriggerState.trackTriggers[t].step+1, t+1);

                        handleNoteOnForTrackStep(t, trackTriggerState.trackTriggers[t].step, trackTriggerState.trackTriggers[t]);

                        trackTriggerState.trackTriggers[t].lastState = trackTriggerState.trackTriggers[t].currentState;
                        trackTriggerState.trackTriggers[t].currentState = 2;
                    }
                    else if (trackTriggerState.trackTriggers[t].currentState == 0)
                    {
                        //Serial.printf("triggering OFF step %d for track %d\n", trackTriggerState.trackTriggers[t].step+1, t+1);

                        handleNoteOffForTrackStep(t, trackTriggerState.trackTriggers[t].step);

                        trackTriggerState.trackTriggers[t].pattern = -1;
                        trackTriggerState.trackTriggers[t].layer = -1;
                        trackTriggerState.trackTriggers[t].step = -1;
                        trackTriggerState.trackTriggers[t].soundType = XRSound::T_EMPTY;

                        trackTriggerState.trackTriggers[t].lastState = trackTriggerState.trackTriggers[t].currentState;
                    }
                } else {
                    if (trackTriggerState.trackTriggers[t].currentState == 1)
                    {
                        //Serial.printf("triggering ON track %d\n", t+1);

                        handleNoteOnForTrack(t, trackTriggerState.trackTriggers[t]);

                        trackTriggerState.trackTriggers[t].lastState = trackTriggerState.trackTriggers[t].currentState;
                        trackTriggerState.trackTriggers[t].currentState = 2;
                    }
                    else if (trackTriggerState.trackTriggers[t].currentState == 0)
                    {
                        //Serial.printf("triggering OFF track %d\n", t+1);

                        handleNoteOffForTrack(t);

                        trackTriggerState.trackTriggers[t].pattern = -1;
                        trackTriggerState.trackTriggers[t].layer = -1;
                        trackTriggerState.trackTriggers[t].step = -1;
                        trackTriggerState.trackTriggers[t].soundType = XRSound::T_EMPTY;

                        trackTriggerState.trackTriggers[t].lastState = trackTriggerState.trackTriggers[t].currentState;
                    }
                }
            }
        }
    }

    void initializeCurrentSelectedTrack()
    {
        activePattern.layers[_currentSelectedTrackLayer].tracks[_currentSelectedTrack].initialized = true;
    }

    void queuePattern(int pattern, int bank)
    {
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

    void setCurrentRatchetPageNum(int8_t page)
    {
        _ratchetPageNum = page;
    }

    void resetRatchetBar()
    {
        _ratchetState.firstBar = true;
    }

    void setCurrentSelectedRatchetStep(int step)
    {
        _currentSelectedRatchetStep = step;
    }

    int8_t getCurrentSelectedRatchetStep()
    {
        return _currentSelectedRatchetStep;
    }

    bool onRatchetStepPage()
    {
        return XRUX::getCurrentMode() == XRUX::PERFORM_RATCHET && _ratchetPageNum == 1;
    }
}