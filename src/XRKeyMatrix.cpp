#include <XRKeyMatrix.h>
#include <XRUX.h>
#include <XRKeyInput.h>
#include <XRDisplay.h>
#include <XRSequencer.h>
#include <XRSD.h>
#include <XRLED.h>
#include <XRMenu.h>
#include <XRClock.h>
#include <XRHelpers.h>
#include <XRAsyncIO.h>
#include <map>

namespace XRKeyMatrix
{
    const byte ROWS = 6;
    const byte COLS = 6;

    byte rowPins[ROWS] = {38, 37, 36, 35, 34, 33}; //connect to the row pinouts of the keypad
    byte colPins[COLS] = {2, 9, 12, 41, 40, 39}; //connect to the column pinouts of the keypad

    char keys[ROWS][COLS] = {
        {'a','b','c','d','e','f'},
        {'g','h','i','j','k','l'},
        {'m','n','o','p','q','r'},
        {'s','t','u','v','w','x'},
        {'y','z','1','2','3','4'},
        {'5','6','7','8','9','0'},
    };

    std::map<char, int8_t> _charStepMap = {
        {'m', 1},
        {'n', 2},
        {'o', 3},
        {'p', 4},
        {'s', 5},
        {'t', 6},
        {'u', 7},
        {'v', 8},
        {'y', 9},
        {'z', 10},
        {'1', 11},
        {'2', 12},
        {'5', 13},
        {'6', 14},
        {'7', 15},
        {'8', 16},
    };

    bool _isFunctionActive = false;
    bool _copyBtnHeld = false;

    int _patternCopyAvailable = -1;
    int _trackCopyAvailable = -1;
    int _stepCopyAvailable = -1;

    bool _recording = false;
    bool _selectBtnHeld = false;

    elapsedMillis _elapsedMs;
    long _patternSelDebounceStartMs = 0;

    int8_t _keyboardOctave = 4; // range 1-7 ?
    int8_t _trackHeldForSelection = -1; // default to -1 (none)
    int8_t _ptnHeldForSelection = -1;
    int8_t _performModeHeldForSelection = -1;
    int8_t _currentTracksSoloed = 0;

    Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

    void handlePressForKey(char key);
    void handleHoldForKey(char key);    
    void handleReleaseForKey(char key);

    void handleSoloForTrack(uint8_t track, bool undoSoloing);

    bool handleFunctionPress(char key);
    bool handleActivateFunction(char key);
    bool handleActivateParamLockStep(char key);
    bool handleActivateRatchetParamLockStep(char key);
    bool handlePatternReleaseActions(char key);
    bool handleRatchetReleaseActions(char key);
    bool handleTrackReleaseActions(char key);
    bool handleTrackLayerReleaseActions(char key);
    bool handleCreateProjectReleaseActions(char key);
    bool handleMenuReleaseActions(char key);
    bool handleFunctionReleaseActions(char key);
    bool handleParamLockStepRelease(char key);
    bool handleRatchetParamLockStepRelease(char key);
    bool handleStepPreviewRelease(char key);
    bool handleCopyRelease(char key);
    bool handleRatchets(char key);
    bool handleCopySelect(char key);

    void instantPatternChange(int nextBank, int nextPattern);
    void prepareQueuedPatternChange(int nextBank, int nextPattern);
    
    // TODO: extract to XRSound namespace method
    XRSound::SOUND_TYPE selectNewSoundTypeForTrack(int currTrackNum, XRSound::SOUND_TYPE currType);

    bool keyIsATrack(char keyChar);
    bool keyIsAStep(char keyChar);
    bool isSelectBtnHeld();

    uint8_t getKeyStepNum(char idx);

    void init()
    {
        kpd.setHoldTime(150);
    }

    void handleStates(bool discard)
    {
        // Fills kpd.key[] array with up-to 10 active keys.
        // Returns true if there are ANY active keys.
        if (kpd.getKeys()) {
            if (discard) return;

            for (size_t i=0; i<LIST_MAX; i++) // Scan the whole key list.
            {
                if (kpd.key[i].stateChanged)
                {
                    switch (kpd.key[i].kstate) // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                    {
                        case PRESSED: 
                            {
                                //Serial.print("button pressed: ");
                                //Serial.println(kpd.key[i].kchar);

                                // if active UX mode involves project data changes, 
                                // ignore all key input until UX state changes
                                XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

                                bool isSensitiveProjectChangeUxMode = (
                                    currentUXMode == XRUX::PROJECT_BUSY ||
                                    currentUXMode == XRUX::PROJECT_INITIALIZE ||
                                    currentUXMode == XRUX::PATTERN_CHANGE_QUEUED ||
                                    currentUXMode == XRUX::PATTERN_CHANGE_INSTANT ||
                                    currentUXMode == XRUX::PASTE_PATTERN ||
                                    currentUXMode == XRUX::PASTE_TRACK ||
                                    currentUXMode == XRUX::PASTE_STEP
                                );

                                if (isSensitiveProjectChangeUxMode) {
                                    Serial.printf("BLOCKING BUTTON INPUT, UX MODE: %d\n", currentUXMode);
                                    return;
                                }

                                handlePressForKey(kpd.key[i].kchar);
                            }

                            break;
                        case HOLD:
                            //Serial.print("button held: ");
                            //Serial.println(kpd.key[i].kchar);

                            handleHoldForKey(kpd.key[i].kchar);

                            break;
                        case RELEASED:
                            //Serial.print("button released: ");
                            //Serial.println(kpd.key[i].kchar);

                            handleReleaseForKey(kpd.key[i].kchar);

                            break;
                        case IDLE:

                            break;

                        default:
                            break;
                    }
                }
            }
        }
    }

    void handlePressForKey(char key)
    {
        //Serial.println("enter handlePressForKey!");

        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        if (currentUXMode == XRUX::PROJECT_BUSY) {
            return;
        }

        if (_isFunctionActive) {
            // handleFunctionPress(key);

            return;
        }

        // start/pause or stop
        else if (key == START_BTN_CHAR || key == STOP_BTN_CHAR) {
            XRSequencer::toggleSequencerPlayback(key);

            // TODO: allowedModesToDrawSequencerFrom ?

            if (currentUXMode != XRUX::UX_MODE::SOUND_MENU_DEXED_SYSEX_BROWSER) {
                XRDisplay::drawSequencerScreen(false);
            }
            return;
        }

        // track select
        else if ((currentUXMode != XRUX::UX_MODE::COPY_SEL) && key == TRACK_BTN_CHAR) {
            Serial.println("enter track select mode!");
            XRUX::setCurrentMode(XRUX::UX_MODE::TRACK_SEL);

            _patternCopyAvailable = -1;
            _trackCopyAvailable = -1;
            _stepCopyAvailable = -1;

            XRLED::clearAllStepLEDs();
            XRLED::displayCurrentlySelectedTrack();

            return;
        }       
        else if (currentUXMode == XRUX::UX_MODE::TRACK_SEL && keyIsATrack(key)) {
            uint8_t selTrack = getKeyStepNum(key)-1; // zero-based

            XRSequencer::setSelectedTrack(selTrack);

            // initialize track
            XRSequencer::initializeCurrentSelectedTrack();

            Serial.print("marking pressed track selection (zero-based): ");
            Serial.println(selTrack);

            _trackHeldForSelection = selTrack;

            XRUX::setCurrentMode(XRUX::UX_MODE::TRACK_SEL);
            XRUX::setPreviousMode(XRUX::UX_MODE::TRACK_SEL);

            XRLED::clearAllStepLEDs();
            XRLED::displayCurrentlySelectedTrack();

            XRLED::displayPageLEDs(
                -1, 
                (XRSequencer::getSeqState().playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::RUNNING),
                XRSequencer::getCurrentStepPage(),
                XRSequencer::getCurrentSelectedTrack().lstep // TODO: use pattern last step here?
            );

            return;
        } 
        else if (currentUXMode == XRUX::UX_MODE::TRACK_SEL && key == SOUND_BTN_CHAR) {
            auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
            auto currSoundForTrack = XRSound::activeKit.sounds[currTrackNum];
            auto currType = currSoundForTrack.type;

            auto newType = selectNewSoundTypeForTrack(currTrackNum, currType);

            // reset page to 0
            XRSequencer::setSelectedPage(0);

            XRSound::changeTrackSoundType(currTrackNum, newType);

            // TODO: ALSO STOP CURRENTLY PLAYING SOUND

            XRDisplay::drawSequencerScreen(false);

            return;
        }
        
        // track write
        else if (
            currentUXMode == XRUX::UX_MODE::TRACK_WRITE && 
            key != PATTERN_BTN_CHAR && key != '9' && key != '3' && key != 'a'// TODO use allowed button list instead
        ) {
            auto currPageSelected = XRSequencer::getCurrentSelectedPage();

            if (key == MOD_A_BTN_CHAR) {            
                auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
                auto currSoundForTrack = XRSound::activeKit.sounds[currTrackNum];    
                
                if (currSoundForTrack.type == XRSound::SOUND_TYPE::T_MONO_SAMPLE) {
                    if (currPageSelected == 1) {
                        XRUX::setCurrentMode(XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK_SOUND);
                        XRDisplay::drawSampleBrowser();
                    }
                }
            }

            return;
        }

        // pattern select
        else if (!(currentUXMode == XRUX::UX_MODE::COPY_SEL) && key == PATTERN_BTN_CHAR)
        {
            XRUX::setCurrentMode(XRUX::UX_MODE::PATTERN_SEL);

            _patternCopyAvailable = -1;
            _trackCopyAvailable = -1;
            _stepCopyAvailable = -1;
            XRSequencer::setCurrentRatchetPageNum(0);
            XRSequencer::setRatchetTrack(0);
            XRSequencer::ratchetLatched = false;

            XRLED::clearAllStepLEDs();
            XRLED::displayCurrentlySelectedPattern();

            XRDisplay::drawSequencerScreen(false);

            return;

        } 
        else if (currentUXMode == XRUX::UX_MODE::PATTERN_SEL && keyIsATrack(key))
        {
            auto &seqState = XRSequencer::getSeqState();
            auto currBank = XRSequencer::getCurrentSelectedBankNum();

            int nextBank = currBank; // TODO: allow for bank selection with pattern selection?
            int nextPattern = getKeyStepNum(key) - 1; // zero-based

            if (seqState.playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::RUNNING) {
                // queued pattern change
                Serial.printf("queueing pattern: %d\n", nextPattern);

                XRUX::setCurrentMode(XRUX::UX_MODE::PATTERN_CHANGE_QUEUED);

                XRSequencer::queuePattern(nextPattern, nextBank);

                prepareQueuedPatternChange(nextBank, nextPattern);

                XRLED::clearAllStepLEDs();
                XRDisplay::drawSequencerScreen(false);

            } else {
                instantPatternChange(nextBank, nextPattern);
            }

            return;
        }

        // pattern write allow tapping tracks (and recording steps in rec mode)
        else if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE && keyIsATrack(key) && !_selectBtnHeld)
        {
            auto trackNum = getKeyStepNum(key) - 1;
            auto &trackToUse = XRSequencer::getTrack(trackNum);

            if (XRSequencer::getSeqState().playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::STOPPED)
            {
                // make sure pattern fx are unmuted before triggering a track manually
                XRSound::applyFxForActivePattern();
            }

            XRSequencer::setSelectedTrack(trackNum);

            XRSound::triggerTrackManually(trackNum, trackToUse.note, trackToUse.octave, _selectBtnHeld);

            if (_recording && XRSequencer::getSeqState().playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::RUNNING)
            {
                // TODO: make it so that we track tap length so we can set the step length

                // get zero-based current seq state track step number to see where to add the step
                auto currStep = XRSequencer::getSeqState().currentStep-1; 
                auto currTrackStep = XRSequencer::getSeqState().currentTrackSteps[trackNum].currentStep-1; 

                // get the current tick and determine the closest microtiming amount
                auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
                auto clockModStepCounter = uClock.getModTrackStepCounter(currTrackNum);
                auto &currTrack = XRSequencer::getCurrentSelectedTrack();
                
                auto targetStep = currTrackStep;

                Serial.printf("RAW STEP: for track %d at pattern step %d track targetStep %d, mtc: %d\n", currTrackNum+1, currStep+1, currTrackStep+1, clockModStepCounter);

                // if tapped step bleeds into next step timeframe, enable the previous step instead
                if (clockModStepCounter <= 4) {
                    targetStep = currTrackStep == 0 ? currTrack.lstep-1 : currTrackStep-1;

                    Serial.printf("TAKE PREV STEP: for track %d at pattern step %d track targetStep %d, mtc: %d\n", currTrackNum+1, currStep+1, targetStep+1, clockModStepCounter);
                } else {
                    //auto targetStep = currTrackStep+1 == currTrack.lstep ? 0 : currTrackStep;

                    Serial.printf("TAKE NEXT STEP: for track %d at pattern step %d track targetStep %d, mtc: %d\n", currTrackNum+1, currStep+1, currTrackStep+1, clockModStepCounter);
                }

                // then change the step state to ON
                XRSequencer::recordingState.tracks[currTrackNum].steps[targetStep].queued = true;
                XRSequencer::recordingState.tracks[currTrackNum].steps[targetStep].state = XRSequencer::STEP_STATE::STATE_ON; // TODO: make accentable
                // TODO: track step length
                // TODO: track step note number from track OR versa keys
            }

            return;
        } else if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE && keyIsATrack(key) && _selectBtnHeld) {
            // allow selecting track in pattern write mode by tapping track keyauto trackNum = getKeyStepNum(key) - 1;

            auto trackNum = getKeyStepNum(key) - 1;
            auto &trackToUse = XRSequencer::getTrack(trackNum);
            
            XRSequencer::setSelectedTrack(trackNum);
        }

        // bank sel
        else if (currentUXMode == XRUX::UX_MODE::BANK_SEL && keyIsATrack(key)) {
            auto &seqState = XRSequencer::getSeqState();

            int nextBank = getKeyStepNum(key) - 1; // zero-based
            int nextPattern = 0; // zero-based

            if (seqState.playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::RUNNING) {
                // queued pattern change
                Serial.printf("queueing bank: %d\n", nextBank);

                XRUX::setCurrentMode(XRUX::UX_MODE::PATTERN_CHANGE_QUEUED);

                XRSequencer::queuePattern(nextPattern, nextBank);

                prepareQueuedPatternChange(nextBank, nextPattern);

                XRLED::clearAllStepLEDs();
                XRDisplay::drawSequencerScreen(false);

            } else {
                instantPatternChange(nextBank, nextPattern);
            }
        }
        
        // copy
        else if (
            currentUXMode == XRUX::COPY_SEL && 
            (_stepCopyAvailable < 0 && _stepCopyAvailable < 0 && _patternCopyAvailable < 0) && 
            (keyIsATrack(key) || key == PATTERN_BTN_CHAR || key == TRACK_BTN_CHAR)
        ) {         
            auto prevMode = XRUX::getPreviousMode();

            if (prevMode == XRUX::TRACK_WRITE && _stepCopyAvailable < 0 && keyIsATrack(key))
            { 
                // copy step
                auto currStepPage = XRSequencer::getCurrentStepPage();

                XRUX::setCurrentMode(XRUX::COPY_STEP);

                uint8_t stepToUse = getKeyStepNum(key);
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

                _stepCopyAvailable = stepToUse - 1;

                Serial.println("drawing copy step confirm overlay!");

                XRDisplay::drawCopyConfirmOverlay("STEP", stepToUse);
            }
            // else if (key == PATTERN_BTN_CHAR)
            // {
            //     XRUX::setCurrentMode(XRUX::COPY_PATTERN);

            //     Serial.println("drawing copy pattern sel overlay!");

            //     XRDisplay::drawCopySelOverlay("PATTERN");

            //     //XRLED::displayInitializedPatternLEDs();
            // }
            else if (_trackCopyAvailable < 0 && key == TRACK_BTN_CHAR)
            {
                XRUX::setCurrentMode(XRUX::COPY_TRACK);

                Serial.println("drawing copy track sel overlay!");

                XRDisplay::drawCopySelOverlay("TRACK");

                XRLED::displayInitializedTrackLEDs();
            }

            return;
        }
        else if (currentUXMode == XRUX::COPY_PATTERN && _patternCopyAvailable < 0 && keyIsATrack(key))
        {
            Serial.printf("copy selected pattern %d !!!\n", getKeyStepNum(key));

            // auto curSelBank = XRSequencer::getCurrentSelectedBankNum();
            // auto currSelPattern = XRSequencer::getCurrentSelectedPatternNum();
            // auto &bank = XRSequencer::getActivePatternBank();
            // auto &activePattern = XRSequencer::getActivePattern();

            // Serial.println("try this:");

            // // make sure if copying current pattern that the latest state exists in RAM2
            // if (currSelPattern == getKeyStepNum(key) - 1)
            // {
            //     bank.patterns[currSelPattern] = activePattern;
            // }

            // Serial.println("finished that");

            // XRSequencer::setCopyBufferForPattern(getKeyStepNum(key) - 1);

            // _patternCopyAvailable = true;

            // XRDisplay::drawCopyConfirmOverlay("PATTERN", getKeyStepNum(key));

            return;
        }
        else if (currentUXMode == XRUX::COPY_TRACK && _trackCopyAvailable < 0 && keyIsATrack(key))
        {
            Serial.printf("copy selected track %d !!!\n", getKeyStepNum(key));

           _trackCopyAvailable = getKeyStepNum(key) - 1;

            XRDisplay::drawCopyConfirmOverlay("TRACK", getKeyStepNum(key));

            return;
        }

        // paste
        else if (
            currentUXMode == XRUX::COPY_STEP && _stepCopyAvailable > -1 && 
            (keyIsATrack(key) || key == ESCAPE_BTN_CHAR)
        ) { 
            // paste step

            if (key == ESCAPE_BTN_CHAR) // discard copy/paste operation
            { 
                _stepCopyAvailable = -1;

                auto prev = XRUX::getPreviousMode();
                XRUX::setCurrentMode(prev);
                XRUX::setPreviousMode(prev);

                XRLED::clearAllStepLEDs();

                if (currentUXMode == XRUX::TRACK_WRITE) {
                    XRLED::setDisplayStateForAllStepLEDs();
                }
                
                XRDisplay::drawSequencerScreen(false);

                return;
            }

            uint8_t stepToUse = getKeyStepNum(key);
            auto currStepPage = XRSequencer::getCurrentStepPage();
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

            XRUX::setCurrentMode(XRUX::PASTE_STEP);

            XRDisplay::drawPasteConfirmOverlay("STEP", stepToUse);

            XRSD::saveCopiedStep(
                XRSequencer::getCurrentSelectedTrackNum(),
                _stepCopyAvailable,
                stepToUse-1
            );

            _stepCopyAvailable = -1;

            return;
        }
        else if (currentUXMode == XRUX::COPY_PATTERN && _patternCopyAvailable > -1 && (keyIsATrack(key) || key == ESCAPE_BTN_CHAR))
        { 
            // paste pattern

            if (key == ESCAPE_BTN_CHAR) // discard copy/paste operation
            { 
                _patternCopyAvailable = -1;

                auto prev = XRUX::getPreviousMode();
                XRUX::setCurrentMode(prev);
                XRUX::setPreviousMode(prev);

                XRLED::clearAllStepLEDs();

                if (currentUXMode == XRUX::TRACK_WRITE) {
                    XRLED::setDisplayStateForAllStepLEDs();
                }
                
                XRDisplay::drawSequencerScreen(false);

                return;
            }

            // Serial.println("pasting selected pattern to target pattern!");

            // uint8_t targetPattern = getKeyStepNum(key) - 1;
            // auto &bank = XRSequencer::getActivePatternBank();
            // auto &activePattern = XRSequencer::getActivePattern();
            // auto &seqState = XRSequencer::getSeqState();
            // auto currSelBank = XRSequencer::getCurrentSelectedBankNum();
            // auto currSelPattern = XRSequencer::getCurrentSelectedPatternNum();
            // auto &patternCopyBuf = XRSequencer::getCopyBufferForPattern();

            // // save pasted pattern to RAM2 / DMAMEM
            // bank.patterns[targetPattern] = patternCopyBuf;

            // // and reload the current pattern into heap if target pattern is current selected pattern
            // if (currSelPattern == targetPattern)
            // {
            //     activePattern = bank.patterns[currSelPattern];
            // }

            // _patternCopyAvailable = false;
            // XRUX::setCurrentMode(XRUX::PASTE_PATTERN);

            // Serial.println("drawing paste pattern confirm overlay!");

            // XRDisplay::drawPasteConfirmOverlay("PATTERN", getKeyStepNum(key));

            // // transition to target pattern as well?
            // if (seqState.playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::RUNNING)
            // {
            //     XRSequencer::queuePattern(currSelBank, targetPattern);
            // }
            // else
            // {
            //     // TODO: rework this to enable pasting patterns across banks too
            //     XRSequencer::setSelectedPattern(targetPattern);

            //     XRSound::saveSoundDataForPatternChange();
            //     XRSound::loadSoundDataForPatternChange(currSelBank, targetPattern);

            //     XRSD::saveActiveTrackStepModLayerToSdCard();
            //     XRSequencer::swapSequencerMemoryForPattern(currSelBank, targetPattern);
            // }

            return;
        }
        else if (currentUXMode == XRUX::COPY_TRACK && _trackCopyAvailable > -1 && (keyIsATrack(key) || key == ESCAPE_BTN_CHAR))
        { 
            // paste track
            
            if (key == ESCAPE_BTN_CHAR) // discard copy/paste operation
            { 
                _trackCopyAvailable = -1;

                auto prev = XRUX::getPreviousMode();
                XRUX::setCurrentMode(prev);
                XRUX::setPreviousMode(prev);

                XRLED::clearAllStepLEDs();

                if (currentUXMode == XRUX::TRACK_WRITE) {
                    XRLED::setDisplayStateForAllStepLEDs();
                }
                
                XRDisplay::drawSequencerScreen(false);

                return;
            }

            auto destTrack = getKeyStepNum(key) - 1;

            // TODO: FIX THIS BY FURTHER DECOUPLING TRACK AND SOUND INSTANCES !
            // since certain sound types are only available to certain tracks,
            // make sure the user can't copy to/from tracks without the corresponding sound types
            bool trackCanBeCopied = true;

            if (_trackCopyAvailable < 4 && destTrack > 3)
            {
                if (_trackCopyAvailable < 3 && destTrack > 2)
                {
                    if (XRSound::activeKit.sounds[_trackCopyAvailable].type == XRSound::T_FM_DRUM)
                    {
                        trackCanBeCopied = false;
                    }
                }
                
                if (
                    XRSound::activeKit.sounds[_trackCopyAvailable].type == XRSound::T_MONO_SYNTH ||
                    XRSound::activeKit.sounds[_trackCopyAvailable].type == XRSound::T_DEXED_SYNTH ||
                    XRSound::activeKit.sounds[_trackCopyAvailable].type == XRSound::T_FM_DRUM ||
                    XRSound::activeKit.sounds[_trackCopyAvailable].type == XRSound::T_BRAIDS_SYNTH
                ) {
                    trackCanBeCopied = false;
                }
            }

            if (!trackCanBeCopied)
            {
                _trackCopyAvailable = -1;

                XRUX::setCurrentMode(XRUX::COPY_ERROR);

                XRDisplay::drawError("TRACK SOUND CANNOT BE COPIED!");

                delay(1000);

                return;
            }

            XRUX::setCurrentMode(XRUX::PASTE_TRACK);

            XRDisplay::drawPasteConfirmOverlay("TRACK", getKeyStepNum(key));

            XRSD::saveCopiedTrackToSameLayer(_trackCopyAvailable, destTrack);

            _trackCopyAvailable = -1;

            // transition to target track as well?
            XRSequencer::setSelectedTrack(destTrack);

            return;
        }

        // page
        else if (key == PAGE_LEFT_BTN_CHAR || key == PAGE_RIGHT_BTN_CHAR)
        {
            auto &currTrack = XRSequencer::getCurrentSelectedTrack();
            auto currStepPage = XRSequencer::getCurrentStepPage();

            int maxPages = 1;
            if (currTrack.lstep > 16 && currTrack.lstep <= 32)
            {
                maxPages = 2;
            }
            else if (currTrack.lstep > 32 && currTrack.lstep <= 48)
            {
                maxPages = 3;
            }
            else if (currTrack.lstep > 48)
            {
                maxPages = 4;
            }

            if (key == '3')
            {
                XRSequencer::setCurrentStepPage(min(maxPages, currStepPage + 1));
            }
            else if (key == '9')
            {
                XRSequencer::setCurrentStepPage(max(1, currStepPage - 1));
            }

            XRLED::displayPageLEDs(
                -1, 
                (XRSequencer::getSeqState().playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::RUNNING),
                XRSequencer::getCurrentStepPage(),
                currTrack.lstep // TODO: use pattern last step here?
            );

            XRLED::setDisplayStateForAllStepLEDs();

            return;
        }

        // perform
        else if (key == PERFORM_BTN_CHAR)
        {
            XRUX::setCurrentMode(XRUX::PERFORM_SEL);

            _patternCopyAvailable = -1;
            _trackCopyAvailable = -1;
            _stepCopyAvailable = -1;
            XRSequencer::setCurrentRatchetPageNum(0);
            XRSequencer::setRatchetTrack(0);
            XRSequencer::ratchetLatched = false;

            XRLED::clearAllStepLEDs();
            XRLED::displayPerformModeLEDs();

            return;
        }
        else if (currentUXMode == XRUX::PERFORM_SEL && keyIsATrack(key) && getKeyStepNum(key) >= 13)
        {
            _performModeHeldForSelection = getKeyStepNum(key) - 1;

            Serial.print("_performModeHeldForSelection: ");
            Serial.println(_performModeHeldForSelection);

            if (getKeyStepNum(key) == 13)
            {
                // enable mute mode
                // TODO: find better way to track UI mode before PERFORM_SEL
                XRUX::setCurrentMode(XRUX::PERFORM_TAP);
                XRUX::setPreviousMode(XRUX::PATTERN_WRITE);

                XRLED::clearAllStepLEDs();
                XRLED::displayTrackLayers();
                XRDisplay::drawSequencerScreen(false);
            }
            else if (getKeyStepNum(key) == 14)
            {
                // enable mute mode
                // TODO: find better way to track UI mode before PERFORM_SEL
                XRUX::setCurrentMode(XRUX::PERFORM_MUTE);
                XRUX::setPreviousMode(XRUX::PATTERN_WRITE);

                XRLED::clearAllStepLEDs();
                XRLED::displayMuteLEDs();
                XRDisplay::drawSequencerScreen(false);
            }
            else if (getKeyStepNum(key) == 15)
            {
                // enable solo mode
                // TODO: find better way to track UI mode before PERFORM_SEL
                XRUX::setCurrentMode(XRUX::PERFORM_SOLO);
                XRUX::setPreviousMode(XRUX::PATTERN_WRITE);

                // displayMuteLEDs();
                XRLED::clearAllStepLEDs();
                XRDisplay::drawSequencerScreen(false);
            }
            else if (getKeyStepNum(key) == 16)
            {
                // enable ratchet mode
                // TODO: find better way to track UI mode before PERFORM_SEL
                XRUX::setCurrentMode(XRUX::PERFORM_RATCHET);
                XRUX::setPreviousMode(XRUX::PATTERN_WRITE);

                // displayMuteLEDs();
                XRLED::clearAllStepLEDs();
                XRLED::displayRatchetTrackLED(XRSequencer::isRatchetAccented());
                XRDisplay::drawSequencerScreen(false);
            }

            return;
        }
        else if (currentUXMode == XRUX::PERFORM_TAP && keyIsATrack(key))
        {
            uint8_t selTrackLayer = getKeyStepNum(key)-1; // zero-based

            Serial.printf(
                "cur bank: %d cur ptn: %d cur layer: %d sel layer: %d\n",
                XRSequencer::getCurrentSelectedBankNum(),
                XRSequencer::getCurrentSelectedPatternNum(),
                XRSequencer::getCurrentSelectedTrackLayerNum(),
                selTrackLayer
            );

            if (selTrackLayer == XRSequencer::getCurrentSelectedTrackLayerNum()) {
                // don't do move to same track layer
                return;
            }

            //
            //
            // TESTING INSTANT TRACK LAYER SWITCHING LATENCY
            //
            //

            // XRSD::saveActiveSoundModLayer();
            // if (!XRSD::loadSoundModLayer(XRSequencer::getCurrentSelectedBankNum(), XRSequencer::getCurrentSelectedPatternNum(), selTrackLayer)) {
            //     XRSound::initSoundStepMods();
            // }

            XRSequencer::swapSequencerMemoryForTrackLayerChange();
            XRSequencer::setSelectedTrackLayer(selTrackLayer);

            // 
            //
            // ELSE USE QUEUEING
            //
            //

            XRLED::displayTrackLayers();

            return;
        }
        else if (currentUXMode == XRUX::PERFORM_MUTE && keyIsATrack(key))
        { 
            auto trackNum = getKeyStepNum(key) - 1;

            // handle mutes
            bool currMuteState = XRSequencer::trackPerformState[trackNum].muted;
            XRSequencer::trackPerformState[trackNum].muted = (currMuteState ? false : true);

            XRLED::displayMuteLEDs();

            return;
        }
        else if (currentUXMode == XRUX::PERFORM_SOLO && keyIsATrack(key))
        { 
            auto trackNum = getKeyStepNum(key) - 1;

            // handle solos
            bool undoSoloing = XRSequencer::trackPerformState[trackNum].soloing;

            handleSoloForTrack(getKeyStepNum(key) - 1, undoSoloing);

            return;
        }
        else if (
            currentUXMode == XRUX::PERFORM_RATCHET && 
            XRSequencer::getCurrentRatchetPageNum() == 0 &&
            keyIsATrack(key)
        ){ 
            auto trackNum = getKeyStepNum(key) - 1;
            // auto isRatchetAccented = false;

            // if (trackNum == XRSequencer::getRatchetTrack()) {
            //     isRatchetAccented = (XRSequencer::isRatchetAccented() ? false : true);
            // }

            // XRSequencer::toggleIsRatchetAccented(isRatchetAccented);
            XRSequencer::setRatchetTrack(trackNum);
            
            XRLED::clearAllStepLEDs();
            XRLED::displayRatchetTrackLED(false);
            XRDisplay::drawSequencerScreen(false);

            return;
        }

        else if (currentUXMode == XRUX::UX_MODE::SOUND_MENU_DEXED_SYSEX_BROWSER && key == SELECT_BTN_CHAR) {
            
            XRSound::applyCurrentDexedPatchToSound();

            XRSequencer::setSelectedPage(0);
            XRUX::setCurrentMode(XRUX::UX_MODE::TRACK_WRITE); // TODO fix by tracking real seq UX mode separately

            XRDisplay::drawSequencerScreen(false);

            return;
        }

        else if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && key == SELECT_BTN_CHAR)
        {
            XRUX::setCurrentMode(XRUX::STEP_PREVIEW);

            auto &currTrack = XRSequencer::getCurrentSelectedTrack();
            auto layer = XRSequencer::getCurrentSelectedTrackLayerNum();
            auto track = XRSequencer::getCurrentSelectedTrackNum();
            auto step = XRSequencer::getCurrentSelectedStepNum();
            auto &currStep = XRSequencer::activeTrackLayer.tracks[track].steps[step];

            auto noteToUse = currTrack.note;
            if (currStep.tFlags[XRSequencer::NOTE])
            {
                noteToUse = currStep.tMods[XRSequencer::NOTE];
            }

            auto octaveToUse = currTrack.octave;
            if (currStep.tFlags[XRSequencer::OCTAVE])
            {
                octaveToUse = currStep.tMods[XRSequencer::OCTAVE];
            }

            // TODO: account for mono sample track accent peculiarity

            //uint8_t velocityToUse = currTrack.velocity;
            bool accented = false;
            if (currStep.state == XRSequencer::STATE_ACCENTED) {
                // if (XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].flags[XRSequencer::VELOCITY]) {
                //     velocityToUse = XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].mods[XRSequencer::VELOCITY];
                // } else {
                //     velocityToUse = max(currTrack.velocity, XRSequencer::activePattern.accent);
                // }
                accented = true;
            }

            Serial.printf("step preview: octaveToUse: %d\n", octaveToUse);

            XRSound::triggerTrackManually(track, noteToUse, octaveToUse, accented);
        }
    }

    void handleHoldForKey(char key)
    {
        // Serial.println("enter handleHoldForKey!");

        if (handleActivateFunction(key)) return;

        if (handleActivateParamLockStep(key)) return;

        if (handleActivateRatchetParamLockStep(key)) return;

        if (handleRatchets(key)) return;

        if (handleCopySelect(key)) return;

        if (key == SELECT_BTN_CHAR) {
            _selectBtnHeld = true;
        }
    }

    void handleReleaseForKey(char key)
    {
       //  Serial.println("enter handleReleaseForKey!");

        if (_isFunctionActive)
        {
            handleFunctionReleaseActions(key);

            return;
        }

        if (handleCreateProjectReleaseActions(key)) return;

        if (handleMenuReleaseActions(key)) return;

        if (handleRatchetReleaseActions(key)) return;

        if (handleTrackLayerReleaseActions(key)) return;

        if (handleTrackReleaseActions(key)) return;

        if (handlePatternReleaseActions(key)) return;

        if (handleParamLockStepRelease(key)) return;

        if (handleRatchetParamLockStepRelease(key)) return;

        if (handleStepPreviewRelease(key)) return;

        if (handleCopyRelease(key)) return;

        if (key == SELECT_BTN_CHAR) {
            _selectBtnHeld = false;
        }
    }

    bool handleCopyRelease(char key)
    {
        auto currentUXMode = XRUX::getCurrentMode();

        if (currentUXMode == XRUX::COPY_SEL && key == COPY_BTN_CHAR)
        {
            Serial.println("COPY SEL RELEASE");

            auto prevMode = XRUX::getPreviousMode();

            XRUX::setCurrentMode(prevMode);
            XRUX::setPreviousMode(prevMode);
            
            _patternCopyAvailable = -1;
            _trackCopyAvailable = -1;
            _stepCopyAvailable = -1;

            return true;
        } 
        else if (currentUXMode == XRUX::PASTE_STEP && (keyIsATrack(key)))
        {
            Serial.println("PASTE STEP RELEASE");

            XRUX::setCurrentMode(XRUX::TRACK_WRITE);
            XRUX::setPreviousMode(XRUX::TRACK_WRITE);
            
            _patternCopyAvailable = -1;
            _trackCopyAvailable = -1;
            _stepCopyAvailable = -1;

            XRLED::setDisplayStateForAllStepLEDs();

            // delay(250);

            XRDisplay::drawSequencerScreen(false);

            return true;
        }
        else if (currentUXMode == XRUX::PASTE_TRACK && (keyIsATrack(key)))
        {
            Serial.println("PASTE TRACK RELEASE");

            XRUX::setCurrentMode(XRUX::TRACK_WRITE);
            XRUX::setPreviousMode(XRUX::TRACK_WRITE);
            
            _patternCopyAvailable = -1;
            _trackCopyAvailable = -1;
            _stepCopyAvailable = -1;

            XRLED::clearAllStepLEDs();

            // delay(250);

            XRDisplay::drawSequencerScreen(false);

            return true;
        }
        else if (currentUXMode == XRUX::COPY_ERROR && (keyIsATrack(key)))
        {
            Serial.println("COPY ERROR RELEASE");

            if (XRUX::getPreviousMode() == XRUX::COPY_TRACK) {
                XRUX::setCurrentMode(XRUX::TRACK_WRITE);
                XRUX::setPreviousMode(XRUX::TRACK_WRITE);
            } else {
                XRUX::setCurrentMode(XRUX::PATTERN_WRITE);
                XRUX::setPreviousMode(XRUX::PATTERN_WRITE);
            }
            
            _patternCopyAvailable = -1;
            _trackCopyAvailable = -1;
            _stepCopyAvailable = -1;

            XRDisplay::drawSequencerScreen(false);

            return true;
        }

        return false;
    }

    bool handleStepPreviewRelease(char key)
    {
        auto currentUXMode = XRUX::getCurrentMode();

        if (currentUXMode == XRUX::STEP_PREVIEW && (key == SELECT_BTN_CHAR || keyIsATrack(key)))
        {
            auto layer = XRSequencer::getCurrentSelectedTrackLayerNum();
            auto track = XRSequencer::getCurrentSelectedTrackNum();
            auto step = XRSequencer::getCurrentSelectedStepNum();

            auto &currTrack = XRSequencer::getCurrentSelectedTrack();
            auto &currStep = XRSequencer::activeTrackLayer.tracks[track].steps[step];

            auto noteToUse = currTrack.note;
            if (currStep.tFlags[XRSequencer::NOTE])
            {
                noteToUse = currStep.tMods[XRSequencer::NOTE];
            }

            auto octaveToUse = currTrack.octave;
            if (currStep.tFlags[XRSequencer::OCTAVE])
            {
                octaveToUse = currStep.tMods[XRSequencer::OCTAVE];
            }

            XRSound::noteOffTrackManually(noteToUse, octaveToUse);
        }

        if (currentUXMode == XRUX::STEP_PREVIEW && key == SELECT_BTN_CHAR)
        {
            XRUX::setCurrentMode(XRUX::SUBMITTING_STEP_VALUE);

            _selectBtnHeld = false;

            return true;
        } else if (currentUXMode == XRUX::STEP_PREVIEW && keyIsATrack(key))
        {
            XRUX::setCurrentMode(XRUX::TRACK_WRITE);

            XRDisplay::drawSequencerScreen(false);

            return true;
        }

        return false;
    }

    bool handlePatternReleaseActions(char key)
    {
        // Serial.println("enter handlePatternReleaseActions!");

        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        // pattern select / write release
        if (currentUXMode == XRUX::PATTERN_SEL && key == PATTERN_BTN_CHAR && _ptnHeldForSelection == -1)
        {
            XRSequencer::setSelectedPage(0);
            XRSequencer::setCurrentRatchetPageNum(0);
            XRSequencer::setRatchetTrack(0);
            XRSequencer::ratchetLatched = false;
            XRUX::setCurrentMode(XRUX::PATTERN_WRITE); // force patt write mode when leaving patt / patt select action

            XRLED::clearPageLEDs();
            XRLED::clearAllStepLEDs();
            XRDisplay::drawSequencerScreen(false);

            return true;
        }

        return false;
    }

    bool handleTrackLayerReleaseActions(char key)
    {
        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        // track layer select
        if (currentUXMode == XRUX::UX_MODE::TRACK_LAYER_SEL && keyIsATrack(key)) {
            uint8_t selTrackLayer = getKeyStepNum(key)-1; // zero-based

            Serial.printf(
                "cur bank: %d cur ptn: %d cur layer: %d sel layer: %d\n",
                XRSequencer::getCurrentSelectedBankNum(),
                XRSequencer::getCurrentSelectedPatternNum(),
                XRSequencer::getCurrentSelectedTrackLayerNum(),
                selTrackLayer
            );

            if (selTrackLayer == XRSequencer::getCurrentSelectedTrackLayerNum()) {
                // don't do move to same track layer
                return true;
            }

            //
            //
            // TESTING INSTANT TRACK LAYER SWITCHING LATENCY
            //
            //

            // XRSD::saveActiveSoundModLayer();
            // if (!XRSD::loadSoundModLayer(XRSequencer::getCurrentSelectedBankNum(), XRSequencer::getCurrentSelectedPatternNum(), selTrackLayer)) {
            //     XRSound::initSoundStepMods();
            // }

            XRSequencer::swapSequencerMemoryForTrackLayerChange();
            XRSequencer::setSelectedTrackLayer(selTrackLayer);

            // 
            //
            // ELSE USE QUEUEING
            //
            //

            XRSequencer::setSelectedPage(0);
            XRUX::setCurrentMode(XRUX::TRACK_WRITE);

            XRLED::displayPageLEDs(
                -1, 
                (XRSequencer::getSeqState().playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::RUNNING),
                XRSequencer::getCurrentSelectedPage(),
                XRSequencer::getCurrentSelectedTrack().lstep // TODO: use pattern last step here?
            );

            XRLED::setDisplayStateForAllStepLEDs();

            XRDisplay::drawSequencerScreen(false);

            return true;
        }

        return false;
    }

    bool handleRatchetReleaseActions(char key)
    {
        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        if (
            !_isFunctionActive && 
            currentUXMode == XRUX::PERFORM_RATCHET && 
            XRSequencer::getCurrentRatchetPageNum() == 1 &&
            keyIsAStep(key)
        ) {
            uint8_t stepToToggle = getKeyStepNum(key);

            XRSequencer::toggleSelectedStep(stepToToggle);
            XRLED::setDisplayStateForAllStepLEDs();

            Serial.println("toggling ratchet step!");

            return true;
        }

        return false;
    }

    bool handleTrackReleaseActions(char key)
    {
        // Serial.println("enter handleTrackReleaseActions!");

        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        // track select / write release
        if (currentUXMode == XRUX::TRACK_SEL && key == TRACK_BTN_CHAR && _trackHeldForSelection == -1)
        {
            XRSD::unloadSampleFileListPaged();
            
            XRSequencer::setSelectedPage(0);
            XRSequencer::setCurrentRatchetPageNum(0);
            XRSequencer::setRatchetTrack(0);
            XRSequencer::ratchetLatched = false;
            XRSequencer::setCurrentStepPage(1);
            XRUX::setCurrentMode(XRUX::TRACK_WRITE);
            XRUX::setPreviousMode(XRUX::TRACK_WRITE); // force track write mode when leaving track / track select action

            XRLED::displayPageLEDs(
                -1, 
                (XRSequencer::getSeqState().playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::RUNNING),
                XRSequencer::getCurrentSelectedPage(),
                XRSequencer::getCurrentSelectedTrack().lstep // TODO: use pattern last step here?
            );

            //XRLED::clearAllStepLEDs(); // need?
            XRLED::setDisplayStateForAllStepLEDs();

            XRDisplay::drawSequencerScreen(false);

            return true;
        }
        else if (currentUXMode == XRUX::TRACK_SEL && keyIsATrack(key) && ((getKeyStepNum(key) - 1) == _trackHeldForSelection))
        {
            Serial.println("unmarking track as held for selection!");

            XRSequencer::setSelectedTrack(getKeyStepNum(key) - 1);
            _trackHeldForSelection = -1;

            XRSequencer::setSelectedPage(0);
            XRSequencer::setCurrentStepPage(1);
            XRUX::setCurrentMode(XRUX::TRACK_WRITE);
            XRUX::setPreviousMode(XRUX::TRACK_WRITE); // force track write mode when leaving track / track select action

            XRLED::displayPageLEDs(
                -1, 
                (XRSequencer::getSeqState().playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::RUNNING),
                XRSequencer::getCurrentSelectedPage(),
                XRSequencer::getCurrentSelectedTrack().lstep // TODO: use pattern last step here?
            );

            //XRLED::clearAllStepLEDs();
            XRLED::setDisplayStateForAllStepLEDs();
            
            XRDisplay::drawSequencerScreen(false);

            return true;
        }
        else if (
            !_isFunctionActive && 
            !_copyBtnHeld && 
            currentUXMode == XRUX::TRACK_WRITE && 
            keyIsAStep(key) && 
            _trackHeldForSelection == -1
        ) {
            uint8_t stepToToggle = getKeyStepNum(key);

            XRSequencer::toggleSelectedStep(stepToToggle);
            XRLED::setDisplayStateForAllStepLEDs();

            Serial.println("toggling step!");

            return true;
        }
        else if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE && keyIsATrack(key))
        {
            auto trackNum = getKeyStepNum(key) - 1;
            auto layer = XRSequencer::getCurrentSelectedTrackLayerNum();
            auto &track = XRSequencer::activeTrackLayer.tracks[trackNum];
            auto trackNote = track.note;

            // if track sound type is synth, trigger note off
            if (XRSound::activeKit.sounds[trackNum].type == XRSound::T_MONO_SYNTH ||
                XRSound::activeKit.sounds[trackNum].type == XRSound::T_DEXED_SYNTH ||
                XRSound::activeKit.sounds[trackNum].type == XRSound::T_BRAIDS_SYNTH)
            {
                XRSound::noteOffTrackManually(trackNote, XRKeyMatrix::getKeyboardOctave());

                // TODO: capture note length here and dequeue the step?
            }
        }

        return false;
    }

    bool handleMenuReleaseActions(char key)
    {
        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        bool allowedModeToLaunchMenuFrom = (
            currentUXMode == XRUX::UX_MODE::PATTERN_WRITE || 
            currentUXMode == XRUX::UX_MODE::TRACK_WRITE || 
            currentUXMode == XRUX::UX_MODE::PERFORM_TAP ||
            currentUXMode == XRUX::UX_MODE::PERFORM_MUTE ||
            currentUXMode == XRUX::UX_MODE::PERFORM_SOLO ||
            currentUXMode == XRUX::UX_MODE::PERFORM_RATCHET
        );

        bool mainMainProhibitedModes = (
            currentUXMode != XRUX::UX_MODE::PROJECT_INITIALIZE &&
            currentUXMode != XRUX::UX_MODE::PROJECT_BUSY &&
            currentUXMode != XRUX::UX_MODE::STEP_PREVIEW
        );

        if (mainMainProhibitedModes)
        {
            auto prevMode = XRUX::getPreviousMode();

            switch (key)
            {
            case SOUND_BTN_CHAR:
                if (currentUXMode == XRUX::UX_MODE::SOUND_MENU_MAIN) {
                    XRMenu::resetCursor();
                    XRUX::setCurrentMode(prevMode);
                    XRDisplay::drawSequencerScreen(false);

                    return true;
                }

                if (allowedModeToLaunchMenuFrom) {
                    XRUX::setCurrentMode(XRUX::UX_MODE::SOUND_MENU_MAIN);
                    XRDisplay::drawSoundMenuMain();

                    return true;
                }

                // if setup is currently active, revert
                if (currentUXMode == XRUX::UX_MODE::CHANGE_SETUP) {
                    XRMenu::resetCursor();
                    XRUX::setCurrentMode(prevMode);

                    // force pattern write mode
                    if (prevMode != XRUX::TRACK_WRITE && prevMode != XRUX::PATTERN_WRITE) {
                        XRSequencer::setSelectedPage(0);
                        XRUX::setCurrentMode(XRUX::PATTERN_WRITE);
                    }

                    XRDisplay::drawSequencerScreen(false);

                    return true;
                }

                break;

            case ESCAPE_BTN_CHAR:
                XRMenu::resetCursor();
                XRUX::setCurrentMode(prevMode);

                // if setup is currently active, revert
                if (currentUXMode == XRUX::UX_MODE::CHANGE_SETUP) {
                    // force pattern write mode
                    if (prevMode != XRUX::TRACK_WRITE && prevMode != XRUX::PATTERN_WRITE) {
                        XRSequencer::setSelectedPage(0);
                        XRUX::setCurrentMode(XRUX::PATTERN_WRITE);
                    }
                }


                XRDisplay::drawSequencerScreen(false);

                return true;

                break;

            case SELECT_BTN_CHAR:
                if (currentUXMode == XRUX::UX_MODE::SOUND_MENU_MAIN) {
                    Serial.println("entering sound sub menu");

                    auto cursorPos = XRMenu::getCursorPosition();
                    auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
                    if (XRSound::activeKit.sounds[currTrackNum].type == XRSound::T_DEXED_SYNTH) {
                        if (XRMenu::getDexedSoundMenuItems()[cursorPos] == "browse dexed sysex") {
                            Serial.println("in browse dexed sysex sub menu");
                            
                            XRUX::setCurrentMode(XRUX::UX_MODE::SOUND_MENU_DEXED_SYSEX_BROWSER);

                            XRSD::dexedCurrentBank = 0;
                            XRSD::dexedCurrentPatch = 0;
                            XRSD::loadDexedVoiceToCurrentTrack();
                            
                            XRDisplay::drawDexedSysexBrowser();
                            XRMenu::resetCursor();

                            return true;
                        }
                    }

                    return true;
                }

                if (currentUXMode == XRUX::UX_MODE::CHANGE_SETUP) {
                    Serial.println("enter setup sub menu");

                    auto cursorPos = XRMenu::getCursorPosition();

                    if (XRMenu::getSetupMenuItems()[cursorPos] == "save project") { // TODO: something better than a str compare
                        XRUX::setCurrentMode(XRUX::PROJECT_BUSY);
                        XRDisplay::drawSaveProject();
                        XRSD::saveProject();
                        XRMenu::resetCursor();
                        XRSequencer::setSelectedPage(0);
                        XRUX::setCurrentMode(XRUX::PATTERN_WRITE); 

                        // TODO: impl UX mode "categories", so instead of forcing pattern write mode here,
                        // leaving project modes with several transitions
                        // can happen and the user is placed back at the last known sequencer UX mode, etc

                        XRDisplay::drawSequencerScreen(false);

                        return true;
                    }
                }

                // if (currentUXMode == XRUX::UX_MODE::SET_TEMPO) {
                //     XRUX::setCurrentMode(prevMode);
                //     XRDisplay::drawSequencerScreen(false);

                //     return true;
                // }

                if (currentUXMode == XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK_SOUND) {
                    XRMenu::resetCursor();
                    XRUX::setCurrentMode(prevMode);
                    XRSound::assignSampleToTrackSound();
                    XRDisplay::drawSequencerScreen(false);
                }

                break;

            // case TEMPO_BTN_CHAR:
            //     if (currentUXMode == XRUX::UX_MODE::SET_TEMPO) {
            //         XRUX::setCurrentMode(prevMode);
            //         XRDisplay::drawSequencerScreen(false);

            //         return true;
            //     }

            //     if (allowedModeToLaunchMenuFrom) {
            //         XRUX::setCurrentMode(XRUX::UX_MODE::SET_TEMPO);
            //         XRDisplay::drawSetTempoOverlay();
            //         XRLED::clearAllStepLEDs();

            //         return true;
            //     }
                
            //     break;
            
            default:
                break;
            }
        }

        return false;
    }

    bool handleCreateProjectReleaseActions(char key)
    {
        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        if (currentUXMode == XRUX::UX_MODE::PROJECT_INITIALIZE)
        {
            std::string projectName = XRKeyInput::get();

            switch (key)
            {
            case ESCAPE_BTN_CHAR:
                XRKeyInput::remove();
                XRDisplay::drawCreateProjectDialog();

                return true;

                break;

            case SELECT_BTN_CHAR:
                if (projectName.length() < 1) {
                    return true;
                }
                
                XRUX::setCurrentMode(XRUX::UX_MODE::PROJECT_BUSY);
                XRDisplay::drawSaveProject();

                delay(100);

                XRSD::createNewProject();

                return true;

                break;
            
            default:
                break;
            }
        }
            
         return false;
    }

    bool handleFunctionReleaseActions(char key)
    {
        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        Serial.println("enter handleFunctionReleaseActions!");

        if (key == FUNCTION_BTN_CHAR) { 
            // leaving function
            _isFunctionActive = false;

            if (currentUXMode == XRUX::PATTERN_WRITE || currentUXMode == XRUX::TRACK_WRITE) {
                XRLED::displayPageLEDs(
                    -1,
                    (XRSequencer::getSeqState().playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::RUNNING),
                    XRSequencer::getCurrentSelectedPage(),
                    XRSequencer::getCurrentSelectedTrack().lstep // TODO: use pattern last step here?
                );
            }

            return true;
        }

        if (key == START_BTN_CHAR) {
            _recording = _recording ? false : true;

            if (_recording) {
                XRSequencer::startRecording();
            } else {
                XRSequencer::stopRecording();
            }

            Serial.printf("start button pressed! value: %d\n", _recording);

            return true;
        }

        // octave
        if (key == '9' || key == '3') {
            // Serial.print("btn: ");
            // Serial.println(key);

            // 1-7 octave range e.g. [1,2,3,4,5,6,7]
            // LEDs 17-20
            if (key == '3') {
                _keyboardOctave = min(7, _keyboardOctave+1);
            } else if (key == '9') {
                _keyboardOctave = max(1, _keyboardOctave-1);
            }

            XRLED::displayCurrentOctaveLEDs(_keyboardOctave);

            // Serial.print("Updated octave to: ");
            // Serial.println(_keyboardOctave);

            return true;
        }

        // metronome toggle
        if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE && key == DATA_BTN_CHAR) {
            XRSequencer::metronomeEnabled = (XRSequencer::metronomeEnabled) ? false : true;

            return true;
        }

        bool allowedModeToLaunchMenuFrom = (
            currentUXMode == XRUX::UX_MODE::PATTERN_WRITE || 
            currentUXMode == XRUX::UX_MODE::TRACK_WRITE || 
            currentUXMode == XRUX::UX_MODE::PERFORM_TAP ||
            currentUXMode == XRUX::UX_MODE::PERFORM_MUTE ||
            currentUXMode == XRUX::UX_MODE::PERFORM_SOLO ||
            currentUXMode == XRUX::UX_MODE::PERFORM_RATCHET
        );

        if (key == SOUND_BTN_CHAR) {
            if (allowedModeToLaunchMenuFrom) {
                XRUX::setCurrentMode(XRUX::UX_MODE::CHANGE_SETUP);
                XRDisplay::drawSetupMenu();
                
                return true;
            }
        }

        bool allowedModeToSelectTrackLayerFrom = (
            currentUXMode == XRUX::UX_MODE::PATTERN_WRITE || 
            currentUXMode == XRUX::UX_MODE::TRACK_WRITE
        );

        if (key == TRACK_BTN_CHAR) {
            if (allowedModeToSelectTrackLayerFrom) {
                XRUX::setCurrentMode(XRUX::UX_MODE::TRACK_LAYER_SEL);

                XRLED::clearAllStepLEDs();
                XRLED::displayTrackLayers();

                XRDisplay::drawGeneralConfirmOverlay("select track layer");

                return true;
            }
        }

        bool allowedModeToSelectBankFrom = (
            currentUXMode == XRUX::UX_MODE::PATTERN_WRITE || 
            currentUXMode == XRUX::UX_MODE::TRACK_WRITE
        );

        if (allowedModeToSelectBankFrom && key == PATTERN_BTN_CHAR)
        {
            XRUX::setCurrentMode(XRUX::BANK_SEL);

            XRLED::clearAllStepLEDs();
            XRLED::displayCurrentlySelectedBank();

            XRDisplay::drawGeneralConfirmOverlay("   select bank");

            return true;
        }

        return false;
    }
   
    bool handleParamLockStepRelease(char key)
    {
        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        // param lock release
        if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && keyIsAStep(key)) {
            // revert
            XRUX::setCurrentMode(XRUX::TRACK_WRITE);

            XRSequencer::setCurrentSelectedStep(-1); // unselect the step

            XRDisplay::drawSequencerScreen(false);

            return true;
        }

        return false;
    }

    bool handleRatchetParamLockStepRelease(char key)
    {
        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        // param lock release
        if (currentUXMode == XRUX::SUBMITTING_RATCHET_STEP_VALUE && keyIsAStep(key)) {
            // revert
            XRUX::setCurrentMode(XRUX::PERFORM_RATCHET);

            Serial.println("releasing ratchet param lock!");

            XRSequencer::setCurrentSelectedRatchetStep(-1); // unselect the step

            XRDisplay::drawSequencerScreen(false);

            return true;
        }

        return false;
    }

    bool handleActivateFunction(char key)
    {
        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        bool allowedModeToActivateFunctionFrom = (
            currentUXMode == XRUX::UX_MODE::PATTERN_WRITE || 
            currentUXMode == XRUX::UX_MODE::TRACK_WRITE || 
            currentUXMode == XRUX::UX_MODE::SOUND_MENU_DEXED_SYSEX_BROWSER || 
            currentUXMode == XRUX::UX_MODE::PERFORM_RATCHET
        );

        if (allowedModeToActivateFunctionFrom) {
            if (key == FUNCTION_BTN_CHAR) {
                Serial.println("starting function!");

                _isFunctionActive = true;

                XRLED::displayCurrentOctaveLEDs(_keyboardOctave);

                return true;
            }
        }

        return false;
    }

    bool handleActivateParamLockStep(char key)
    {
        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        // param lock step
        bool allowedModeToParamLockFrom = (currentUXMode == XRUX::TRACK_WRITE);

        if (allowedModeToParamLockFrom && keyIsAStep(key))
        {
            // editing a step value / parameter locking this step
            XRUX::setCurrentMode(XRUX::SUBMITTING_STEP_VALUE);

            uint8_t stepToUse = getKeyStepNum(key);
            auto currStepPage = XRSequencer::getCurrentStepPage();

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

            int selectedStepNum = stepToUse - 1;
            XRSequencer::setCurrentSelectedStep(selectedStepNum);

            auto &currTrack = XRSequencer::getCurrentSelectedTrack();
            auto &heldStep = currTrack.steps[selectedStepNum];

            // only toggle held step ON if initially in the OFF position,
            // so that holding / param locking doesn't turn the step off
            if (heldStep.state == XRSequencer::STEP_STATE::STATE_OFF)
            {
                Serial.println("toggling held step!");

                uint8_t stepToToggle = getKeyStepNum(key);

                XRSequencer::toggleSelectedStep(stepToToggle);
                XRLED::setDisplayStateForAllStepLEDs();
            }

            XRDisplay::drawSequencerScreen(false);

            return true;
        }

        return false;
    }

    bool handleActivateRatchetParamLockStep(char key)
    {
        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        // param lock step
        bool allowedModeToParamLockFrom = (
            currentUXMode == XRUX::PERFORM_RATCHET && 
            XRSequencer::getCurrentRatchetPageNum() == 1
        );

        if (allowedModeToParamLockFrom && keyIsAStep(key))
        {
            // editing a step value / parameter locking this step
            XRUX::setCurrentMode(XRUX::SUBMITTING_RATCHET_STEP_VALUE);

            auto stepToUse = getKeyStepNum(key);
            auto selectedStepNum = stepToUse - 1;

            Serial.printf("ratchet param lock step: %d\n", selectedStepNum);

            XRSequencer::setCurrentSelectedRatchetStep(selectedStepNum);

            auto &currTrack = XRSequencer::activeRatchetLayer.tracks[XRSequencer::getRatchetTrack()];
            auto &heldStep = currTrack.steps[selectedStepNum];

            // only toggle held step ON if initially in the OFF position,
            // so that holding / param locking doesn't turn the step off
            if (heldStep.state == XRSequencer::STEP_STATE::STATE_OFF)
            {
                Serial.println("toggling held step!");

                uint8_t stepToToggle = getKeyStepNum(key);

                XRSequencer::toggleSelectedStep(stepToToggle);
                XRLED::setDisplayStateForAllStepLEDs();
            }

            XRDisplay::drawSequencerScreen(false);

            return true;
        }

        return false;
    }

    bool isFunctionActive()
    {
        return _isFunctionActive;
    }

    bool handleCopySelect(char key)
    {
        auto currUXMode = XRUX::getCurrentMode();
        bool allowedModeToCopySelectFrom = (currUXMode == XRUX::PATTERN_WRITE || currUXMode == XRUX::TRACK_WRITE);

        if (allowedModeToCopySelectFrom && key == COPY_BTN_CHAR)
        {
            Serial.print("current mode: ");
            Serial.println(currUXMode);

            Serial.println("enter copy select mode!");

            XRUX::setCurrentMode(XRUX::COPY_SEL);

            return true;
        }

        return false;
    }

    bool handleRatchets(char key)
    {
        auto currentUXMode = XRUX::getCurrentMode();
        auto &seqState = XRSequencer::getSeqState();

        // perform ratchet
        if (currentUXMode == XRUX::PERFORM_RATCHET && keyIsATrack(key))
        { 
            // handle ratchets
            if (seqState.playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::RUNNING)
            {
                XRSequencer::setRatchetTrack(getKeyStepNum(key) - 1);

                XRDisplay::drawSequencerScreen(false);
                
                return true;
            }
        }

        return false;
    }

    void handleSoloForTrack(uint8_t track, bool undoSoloing)
    {
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            if (undoSoloing && _currentTracksSoloed > 0 && t == track)
            {
                --_currentTracksSoloed;

                XRSequencer::trackPerformState[t].soloing = false;
                XRSequencer::trackPerformState[t].muted = true;
                
                XRLED::setPWM(XRLED::_stepLEDPins[t], 0);
            }

            if (undoSoloing && _currentTracksSoloed == 0)
            {
                XRSequencer::trackPerformState[t].soloing = false;
                XRSequencer::trackPerformState[t].muted = false;

                XRLED::setPWM(XRLED::_stepLEDPins[t], 0);
            }

            if (undoSoloing)
                continue;

            if (t == track)
            {
                ++_currentTracksSoloed;

                XRSequencer::trackPerformState[t].soloing = true;
                XRSequencer::trackPerformState[t].muted = false;

                XRLED::setPWM(XRLED::_stepLEDPins[t], 4095);
            }
            else if (!XRSequencer::trackPerformState[t].soloing)
            {
                XRSequencer::trackPerformState[t].soloing = false;
                XRSequencer::trackPerformState[t].muted = true;

                XRLED::setPWM(XRLED::_stepLEDPins[t], 0);
            }
        }
    }

    bool keyIsATrack(char keyChar)
    {
        std::string keyCharStr;
        keyCharStr += keyChar;
        
        std::string checkStr = "mnopstuvyz125678";
        if (keyCharStr.length() > 0) {
            if (checkStr.find(keyChar) != std::string::npos) {
                return true;
            }
        }

        return false;
    }

    bool keyIsAStep(char keyChar)
    {
        return keyIsATrack(keyChar);
    }

    uint8_t getKeyStepNum(char idx)
    {
        if (_charStepMap.count(idx) != 0)
        {
            return _charStepMap[idx];
        }

        return 1; // default first step num
    }

    int8_t getKeyboardOctave()
    {
        return _keyboardOctave;
    }

    XRSound::SOUND_TYPE selectNewSoundTypeForTrack(int currTrackNum, XRSound::SOUND_TYPE currType)
    {
        auto newType = XRSound::T_EMPTY;

        if (currTrackNum < 4) {
            if (currTrackNum < 3) {
                if (currType == XRSound::T_EMPTY)
                {
                    newType = XRSound::T_MONO_SAMPLE;
                }
                else if (currType == XRSound::T_MONO_SAMPLE)
                {
                    newType = XRSound::T_MONO_SYNTH;
                }
                else if (currType == XRSound::T_MONO_SYNTH)
                {
                    newType = XRSound::T_DEXED_SYNTH;
                }
                else if (currType == XRSound::T_DEXED_SYNTH)
                {
                    newType = XRSound::T_FM_DRUM;
                }
                else if (currType == XRSound::T_FM_DRUM)
                {
                    newType = XRSound::T_MIDI;
                }
                else if (currType == XRSound::T_MIDI)
                {
                    newType = XRSound::T_MONO_SAMPLE;
                }
            } else {
                if (currType == XRSound::T_EMPTY)
                {
                    newType = XRSound::T_MONO_SAMPLE;
                }
                else if (currType == XRSound::T_MONO_SAMPLE)
                {
                    newType = XRSound::T_MONO_SYNTH;
                }
                else if (currType == XRSound::T_MONO_SYNTH)
                {
                    newType = XRSound::T_DEXED_SYNTH;
                }
                else if (currType == XRSound::T_DEXED_SYNTH)
                {
                    newType = XRSound::T_MIDI;
                }
                // else if (currType == XRSound::T_BRAIDS_SYNTH)
                // {
                //     newType = XRSound::T_MIDI;
                // } 
                else if (currType == XRSound::T_MIDI)
                {
                    newType = XRSound::T_MONO_SAMPLE;
                }
            }
        } else {
            if (currTrackNum < 8) {
                if (currType == XRSound::T_EMPTY)
                {
                    newType = XRSound::T_MONO_SAMPLE;
                }
                else if (currType == XRSound::T_MONO_SAMPLE)
                {
                    newType = XRSound::T_CV_GATE;
                } 
                else if (currType == XRSound::T_CV_GATE)
                {
                    newType = XRSound::T_CV_TRIG;
                } 
                else if (currType == XRSound::T_CV_TRIG)
                {
                    newType = XRSound::T_MIDI;
                } 
                else if (currType == XRSound::T_MIDI)
                {
                    newType = XRSound::T_MONO_SAMPLE;
                }
            } else {
                if (currType == XRSound::T_EMPTY)
                {
                    newType = XRSound::T_MONO_SAMPLE;
                }
                else if (currType == XRSound::T_MONO_SAMPLE)
                {
                    newType = XRSound::T_MIDI;
                } 
                else if (currType == XRSound::T_MIDI)
                {
                    newType = XRSound::T_MONO_SAMPLE;
                }
            }
        }

        return newType;
    }

    bool isSelectBtnHeld()
    {
        return _selectBtnHeld;
    }

    void instantPatternChange(int nextBank, int nextPattern)
    {
        // instant pattern change
        XRUX::setCurrentMode(XRUX::UX_MODE::PATTERN_CHANGE_INSTANT);

        XRSD::saveCurrentProjectDataSync();

        if (!XRSD::loadNextPatternSettings(nextBank, nextPattern)) 
        {
            Serial.println("failed ot load next pattern settings, init it instead!");
            XRSequencer::initPatternSettings(XRSequencer::idlePatternSettings);
        }

        if (!XRSD::loadNextTrackLayer(nextBank, nextPattern, 0)) 
        {
            Serial.println("failed ot load next track layer, init it instead!");
            XRSequencer::initTrackLayer(XRSequencer::idleTrackLayer);
        }

        if (!XRSD::loadNextRatchetLayer(nextBank, nextPattern)) 
        {
            Serial.println("failed ot load next ratchet layer, init it instead!");
            XRSequencer::initRatchetLayer(XRSequencer::idleRatchetLayer);
        }

        if (!XRSD::loadNextKit(nextBank, nextPattern)) 
        {
            Serial.println("failed ot load next kit, init it instead!");
            XRSound::initKit(XRSound::idleKit);
        }

        // load next dexed instances
        XRSound::loadNextDexedInstances();

        // IMPORTANT: must change sound data before sequencer data!
        XRSound::swapSoundDataForPatternChange(nextBank, nextPattern);
        XRSequencer::swapSequencerDataForPatternChange(nextBank, nextPattern);

        // swap dexed instances so inactive = active and vice versa
        XRDexedManager::swapInstances();

        //_ptnHeldForSelection = nextPattern; // TODO: need?

        XRUX::setCurrentMode(XRUX::PATTERN_WRITE);

        XRLED::clearAllStepLEDs();
        XRDisplay::drawSequencerScreen(false);
    }

    void patternQueueCallback(const XRAsyncIO::IO_CONTEXT& item)
    {
        switch (item.fileType)
        {
        case XRAsyncIO::FILE_TYPE::PATTERN_SETTINGS:
            XRSequencer::initPatternSettings(XRSequencer::idlePatternSettings);
            Serial.println("pattern settings initialized!");
            break;
        case XRAsyncIO::FILE_TYPE::RATCHET_LAYER:
            XRSequencer::initRatchetLayer(XRSequencer::idleRatchetLayer);
            Serial.println("ratchet layer initialized!");
            break;
        case XRAsyncIO::FILE_TYPE::TRACK_LAYER:
            XRSequencer::initTrackLayer(XRSequencer::idleTrackLayer);
            Serial.println("track layer initialized!");
            break;
        case XRAsyncIO::FILE_TYPE::KIT:
            XRSound::initKit(XRSound::idleKit);
            Serial.println("kit initialized!");
            break;
        
        default:
            break;
        }

        XRAsyncIO::setCallback(nullptr); // clear callback
    }

    void prepareQueuedPatternChange(int nextBank, int nextPattern)
    {
        XRAsyncIO::setCallback(patternQueueCallback);

        // TODO: swap dexed instances?

        XRAsyncIO::addItem({
            XRAsyncIO::FILE_TYPE::PATTERN_SETTINGS,
            XRAsyncIO::FILE_IO_TYPE::READ,
            XRSD::getPatternSettingsFilename(nextBank, nextPattern),
            sizeof(XRSequencer::idlePatternSettings),
        });

        XRAsyncIO::addItem({
            XRAsyncIO::FILE_TYPE::RATCHET_LAYER,
            XRAsyncIO::FILE_IO_TYPE::READ,
            XRSD::getRatchetLayerFilename(nextBank, nextPattern),
            sizeof(XRSequencer::idleRatchetLayer),
        });

        XRAsyncIO::addItem({
            XRAsyncIO::FILE_TYPE::TRACK_LAYER,
            XRAsyncIO::FILE_IO_TYPE::READ,
            XRSD::getTrackLayerFilename(nextBank, nextPattern, 0),
            sizeof(XRSequencer::idleTrackLayer),
        });

        XRAsyncIO::addItem({
            XRAsyncIO::FILE_TYPE::KIT,
            XRAsyncIO::FILE_IO_TYPE::READ,
            XRSD::getKitFilename(nextBank, nextPattern),
            sizeof(XRSound::idleKit),
        });

        // yield();
    }
}