#include <XRKeyMatrix.h>
#include <XRUX.h>
#include <XRKeyInput.h>
#include <XRDisplay.h>
#include <XRSequencer.h>
#include <XRSD.h>
#include <XRLED.h>
#include <XRMenu.h>
#include <XRClock.h>
#include <map>
#include <XRAsyncPSRAMLoader.h>

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
    bool _bankCopyAvailable = false;
    bool _patternCopyAvailable = false;
    bool _trackCopyAvailable = false;
    bool _stepCopyAvailable = false;
    bool _recording = false;

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
    bool handlePatternReleaseActions(char key);
    bool handleTrackReleaseActions(char key);
    bool handleTrackLayerReleaseActions(char key);
    bool handleCreateProjectReleaseActions(char key);
    bool handleMenuReleaseActions(char key);
    bool handleFunctionReleaseActions(char key);
    bool handleParamLockStepRelease(char key);
    bool handleRatchetRelease(char key);
    bool handleRatchets(char key);
    bool handleCopySelect(char key);
    
    // TODO: extract to XRSound namespace method
    XRSound::SOUND_TYPE selectNewSoundTypeForTrack(int currTrackNum, XRSound::SOUND_TYPE currType);

    bool btnCharIsATrack(char btnChar);

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
                            Serial.print("button pressed: ");
                            Serial.println(kpd.key[i].kchar);

                            handlePressForKey(kpd.key[i].kchar);

                            break;
                        case HOLD:
                            Serial.print("button held: ");
                            Serial.println(kpd.key[i].kchar);

                            handleHoldForKey(kpd.key[i].kchar);

                            break;
                        case RELEASED:
                            Serial.print("button released: ");
                            Serial.println(kpd.key[i].kchar);

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

        if (_isFunctionActive) {
            // handleFunctionPress(key);

            return;
        }

        // start/pause or stop
        else if (key == START_BTN_CHAR || key == STOP_BTN_CHAR) {
            XRSequencer::toggleSequencerPlayback(key);

            // TODO: allowedModesToDrawSequencerFrom ?
#ifndef NO_DEXED
            if (currentUXMode != XRUX::UX_MODE::SOUND_MENU_DEXED_SYSEX_BROWSER) {
                XRDisplay::drawSequencerScreen(false);
            }
#endif
            return;
        }

        // track select
        else if ((currentUXMode != XRUX::UX_MODE::COPY_SEL) && key == 'c') {
            Serial.println("enter track select mode!");
            XRUX::setCurrentMode(XRUX::UX_MODE::TRACK_SEL);

            _patternCopyAvailable = false;
            _trackCopyAvailable = false;
            _stepCopyAvailable = false;

            XRLED::clearAllStepLEDs();
            XRLED::displayCurrentlySelectedTrack();

            return;
        }       
        else if (currentUXMode == XRUX::UX_MODE::TRACK_SEL && btnCharIsATrack(key)) {
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
            auto currSoundForTrack = XRSound::activePatternSounds[currTrackNum];
            auto currType = currSoundForTrack.type;

            auto newType = selectNewSoundTypeForTrack(currTrackNum, currType);

            // reset page to 0
            XRSequencer::setSelectedPage(0);

            XRSound::changeTrackSoundType(currTrackNum, newType);

            XRDisplay::drawSequencerScreen(false);

            return;
        }
        
        // track write
        else if (
            currentUXMode == XRUX::UX_MODE::TRACK_WRITE && 
            key != PATTERN_BTN_CHAR && key != '9' && key != '3' && key != 'a'// TODO use allowed button list instead
        ) {
            auto currPageSelected = XRSequencer::getCurrentSelectedPage();

            if (key == MOD_D_BTN_CHAR) {            
                auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
                auto currSoundForTrack = XRSound::activePatternSounds[currTrackNum];    
                
                if (currSoundForTrack.type == XRSound::SOUND_TYPE::T_MONO_SAMPLE) {
                    if (currPageSelected == 0) {
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

            _patternCopyAvailable = false;
            _trackCopyAvailable = false;
            _stepCopyAvailable = false;

            XRLED::clearAllStepLEDs();
            XRLED::displayCurrentlySelectedPattern();

            XRDisplay::drawSequencerScreen();

            return;

        } 
        else if (currentUXMode == XRUX::UX_MODE::PATTERN_SEL && btnCharIsATrack(key))
        {
            auto &seqState = XRSequencer::getSeqState();
            auto currBank = XRSequencer::getCurrentSelectedBankNum();

            int nextBank = currBank; // TODO: allow for bank selection with pattern selection?
            int nextPattern = getKeyStepNum(key) - 1; // zero-based

            if (seqState.playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::RUNNING) {
                // queued pattern change
                Serial.printf("queueing pattern: %d\n", nextPattern);

                XRSequencer::queuePattern(nextPattern, nextBank);

                if (!XRSD::loadNextPattern(nextBank, nextPattern)) 
                {
                    XRSequencer::initNextPattern();
                }

                if (!XRSD::loadNextTrackLayer(nextBank, nextPattern, 0)) 
                {
                    XRSequencer::initNextTrackLayer();
                }

                XRAsyncPSRAMLoader::startAsyncInitOfNextSamples();

                XRUX::setCurrentMode(XRUX::UX_MODE::PATTERN_SEL);

                XRLED::clearAllStepLEDs();
                XRDisplay::drawSequencerScreen();
            } else {
                // instant pattern change

                // IMPORTANT: must change sound data before sequencer data!
                XRSound::saveSoundDataForPatternChange(); // save current sound data first
                XRSound::loadSoundDataForPatternChange(nextBank, nextPattern);

                // save any track step mods for current pattern to SD
                XRSD::saveCurrentSequencerData();
                XRSequencer::swapSequencerMemoryForPattern(nextBank, nextPattern);
                XRAsyncPSRAMLoader::prePatternChange();

                _ptnHeldForSelection = nextPattern; // TODO: need?

                XRUX::setCurrentMode(XRUX::UX_MODE::PATTERN_SEL); // GETS CHANGED TO PATTERN_WRITE IN RELEASE ACTIONS HANDLER

                XRLED::clearAllStepLEDs();
                XRDisplay::drawSequencerScreen();
            }

            return;
        }

        // pattern write allow tapping tracks (and recording steps in rec mode)
        else if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE && btnCharIsATrack(key))
        {
            // if (_patternSelDebounceStartMs > 0 && (_elapsedMs - _patternSelDebounceStartMs) < 100) {
            //     Serial.println("not triggering track, pattern button debouncing first!");

            //     return;
            // } else {
            //     _patternSelDebounceStartMs = 0;
            // }

            auto trackNum = getKeyStepNum(key) - 1;
            auto &trackToUse = XRSequencer::getTrack(trackNum);

            // TODO: fix issue with triggering and selecting patterns, and trying to trigger tracks with no sounds:
            XRSound::triggerTrackManually(trackNum, trackToUse.note);

            if (_recording)
            {
                // add tapped steps to rec seq buffer @ 96ppqn scale?
                // e.g.
                // _rec_seq_heap.pattern.tracks[getKeyStepNum(kpd.key[i].kchar)-1].steps[_current_96ppqn].state = ON/ACCENTED
                // rec buffer struct looks like:
                // { int state, int note }

                // in rec mode, page buttons toggle between accent on/off?
                // or main encoder can be twisted right/left to engage/disengage accent?

                // if rec enabled, show minimap of tracks in pattern mode to show which one is currently selected for keyboard note input?
            }

            return;
        }
        
        // copy
        else if (
            currentUXMode == XRUX::COPY_SEL && 
            (!_stepCopyAvailable && !_stepCopyAvailable && !_patternCopyAvailable) && 
            (btnCharIsATrack(key) || key == PATTERN_BTN_CHAR || key == TRACK_BTN_CHAR)
        ) {            
            // if (!_stepCopyAvailable && btnCharIsATrack(key))
            // { 
            //     // copy step
            //     auto currStepPage = XRSequencer::getCurrentStepPage();

            //     XRUX::setCurrentMode(XRUX::COPY_STEP);

            //     uint8_t stepToUse = getKeyStepNum(key);
            //     if (currStepPage == 2)
            //     {
            //         stepToUse += 16;
            //     }
            //     else if (currStepPage == 3)
            //     {
            //         stepToUse += 32;
            //     }
            //     else if (currStepPage == 4)
            //     {
            //         stepToUse += 48;
            //     }

            //     XRSequencer::setCopyBufferForStep(stepToUse - 1);

            //     _stepCopyAvailable = true;

            //     Serial.println("drawing copy step confirm overlay!");

            //     XRDisplay::drawCopyConfirmOverlay("STEP", stepToUse);
            // }
            // else if (key == PATTERN_BTN_CHAR)
            // {
            //     XRUX::setCurrentMode(XRUX::COPY_PATTERN);

            //     Serial.println("drawing copy pattern sel overlay!");

            //     XRDisplay::drawCopySelOverlay("PATTERN");

            //     //XRLED::displayInitializedPatternLEDs();
            // }
            // else if (key == TRACK_BTN_CHAR)
            // {
            //     XRUX::setCurrentMode(XRUX::COPY_TRACK);

            //     Serial.println("drawing copy track sel overlay!");

            //     XRDisplay::drawCopySelOverlay("TRACK");

            //     XRLED::displayInitializedTrackLEDs();
            // }

            return;
        }
        else if (currentUXMode == XRUX::COPY_PATTERN && !_patternCopyAvailable && btnCharIsATrack(key))
        {
            Serial.printf("write selected pattern %d to copy buffer\n", getKeyStepNum(key));

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
        else if (currentUXMode == XRUX::COPY_TRACK && !_trackCopyAvailable && btnCharIsATrack(key))
        {
            Serial.printf("write selected track %s to copy buffer\n", getKeyStepNum(key));

            XRSequencer::setCopyBufferForTrack(getKeyStepNum(key) - 1);

            _trackCopyAvailable = true;

            XRDisplay::drawCopyConfirmOverlay("TRACK", getKeyStepNum(key));

            return;
        }

        // paste
        else if (
            currentUXMode == XRUX::COPY_STEP && _stepCopyAvailable && 
            (btnCharIsATrack(key) || key == ESCAPE_BTN_CHAR)
        ) { 
            // paste step

            if (key == ESCAPE_BTN_CHAR) // discard copy/paste operation
            { 
                _stepCopyAvailable = false;
                XRUX::setCurrentMode(XRUX::getPreviousMode());
                XRLED::clearAllStepLEDs();

                if (currentUXMode == XRUX::TRACK_WRITE) {
                    XRLED::setDisplayStateForAllStepLEDs();
                }
                
                XRDisplay::drawSequencerScreen();

                return;
            }

            // uint8_t stepToUse = getKeyStepNum(key);
            // auto currStepPage = XRSequencer::getCurrentStepPage();

            // if (currStepPage == 2)
            // {
            //     stepToUse += 16;
            // }
            // else if (currStepPage == 3)
            // {
            //     stepToUse += 32;
            // }
            // else if (currStepPage == 4)
            // {
            //     stepToUse += 48;
            // }

            // // save pasted step
            // auto &activePattern = XRSequencer::getActivePattern();
            // auto currSelTrack = XRSequencer::getCurrentSelectedTrackNum();
            // auto currSelTrackLayer = XRSequencer::getCurrentSelectedTrackLayerNum();

            // activePattern.tracks[currSelTrack].layers[currSelTrackLayer].steps[stepToUse - 1] = XRSequencer::getCopyBufferForStep();

            // _stepCopyAvailable = false;

            // XRUX::setCurrentMode(XRUX::PASTE_STEP);

            // Serial.println("drawing paste step confirm overlay!");

            // XRDisplay::drawPasteConfirmOverlay("STEP", stepToUse);

            return;
        }
        else if (currentUXMode == XRUX::COPY_PATTERN && _patternCopyAvailable && (btnCharIsATrack(key) || key == ESCAPE_BTN_CHAR))
        { 
            // paste pattern

            Serial.println("here9");

            if (key == ESCAPE_BTN_CHAR) // discard copy/paste operation
            {
                _patternCopyAvailable = false;

                XRUX::setCurrentMode(XRUX::getPreviousMode());
                XRLED::clearAllStepLEDs();
                XRDisplay::drawSequencerScreen();

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
        else if (currentUXMode == XRUX::COPY_TRACK && _trackCopyAvailable && (btnCharIsATrack(key) || key == ESCAPE_BTN_CHAR))
        { 
            // paste track
            
            if (key == ESCAPE_BTN_CHAR) // discard copy/paste operation
            {
                _trackCopyAvailable = false;
                XRUX::setCurrentMode(XRUX::getPreviousMode());
                XRLED::clearAllStepLEDs();
                if (currentUXMode == XRUX::TRACK_WRITE) {
                    XRLED::setDisplayStateForAllStepLEDs();
                }
                
                XRDisplay::drawSequencerScreen();
                return;
            }

            // Serial.println("pasting selected track to target track!");

            // uint8_t targetTrack = getKeyStepNum(key) - 1;
            // auto &activePattern = XRSequencer::getActivePattern();
            // auto &trackCopyBuf = XRSequencer::getCopyBufferForTrack();
            // auto currSelTrackLayer = XRSequencer::getCurrentSelectedTrackLayerNum();

            // activePattern.tracks[targetTrack] = trackCopyBuf;

            // _trackCopyAvailable = false;

            // XRUX::setCurrentMode(XRUX::PASTE_TRACK);

            // Serial.println("drawing paste track confirm overlay!");

            // XRDisplay::drawPasteConfirmOverlay("TRACK", getKeyStepNum(key));

            // // transition to target track as well?
            // XRSequencer::setSelectedTrack(targetTrack);

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

            _patternCopyAvailable = false;
            _trackCopyAvailable = false;
            _stepCopyAvailable = false;

            XRLED::clearAllStepLEDs();
            XRLED::displayPerformModeLEDs();

            return;
        }
        else if (currentUXMode == XRUX::PERFORM_SEL && btnCharIsATrack(key) && getKeyStepNum(key) >= 13)
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
                XRDisplay::drawSequencerScreen(false);
            }

            return;
        }
        else if (currentUXMode == XRUX::PERFORM_TAP && btnCharIsATrack(key))
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

            XRSD::saveActiveTrackLayerToSdCard();
            if (!XRSD::loadNextTrackLayer(XRSequencer::getCurrentSelectedBankNum(), XRSequencer::getCurrentSelectedPatternNum(), selTrackLayer)) {
                XRSequencer::initNextTrackLayer();
            }

            XRSD::saveActiveTrackStepModLayerToSdCard();
            if (!XRSD::loadActiveTrackStepModLayerFromSdCard(XRSequencer::getCurrentSelectedBankNum(), XRSequencer::getCurrentSelectedPatternNum(), selTrackLayer)) {
                XRSequencer::initActiveTrackStepModLayer();
            }

            XRSD::saveActiveSoundStepModLayerToSdCard();
            if (!XRSD::loadPatternSoundStepModLayerFromSdCard(XRSequencer::getCurrentSelectedBankNum(), XRSequencer::getCurrentSelectedPatternNum(), selTrackLayer)) {
                XRSound::initPatternSoundStepMods();
            }

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
        else if (currentUXMode == XRUX::PERFORM_MUTE && btnCharIsATrack(key))
        { 
            auto trackNum = getKeyStepNum(key) - 1;

            // handle mutes
            bool currMuteState = XRSequencer::trackPerformState[trackNum].muted;
            XRSequencer::trackPerformState[trackNum].muted = (currMuteState ? false : true);

            XRLED::displayMuteLEDs();

            return;
        }
        else if (currentUXMode == XRUX::PERFORM_SOLO && btnCharIsATrack(key))
        { 
            auto trackNum = getKeyStepNum(key) - 1;

            // handle solos
            bool undoSoloing = XRSequencer::trackPerformState[trackNum].soloing;

            handleSoloForTrack(getKeyStepNum(key) - 1, undoSoloing);

            return;
        }
#ifndef NO_DEXED
        else if (currentUXMode == XRUX::UX_MODE::SOUND_MENU_DEXED_SYSEX_BROWSER && key == SELECT_BTN_CHAR) {
            auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();

            XRSound::applyCurrentDexedPatchToSound();

            XRSequencer::setSelectedPage(0);
            XRUX::setCurrentMode(XRUX::UX_MODE::TRACK_WRITE); // TODO fix by tracking real seq UX mode separately

            XRDisplay::drawSequencerScreen(false);

            return;
        }
#endif
    }

    void handleHoldForKey(char key)
    {
        // Serial.println("enter handleHoldForKey!");

        if (handleActivateFunction(key)) return;

        if (handleActivateParamLockStep(key)) return;

        if (handleRatchets(key)) return;

        if (handleCopySelect(key)) return;
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

        if (handleTrackLayerReleaseActions(key)) return;

        if (handleTrackReleaseActions(key)) return;

        if (handlePatternReleaseActions(key)) return;

        if (handleParamLockStepRelease(key)) return;

        if (handleRatchetRelease(key)) return;
    }

    bool handlePatternReleaseActions(char key)
    {
        // Serial.println("enter handlePatternReleaseActions!");

        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        // pattern select / write release
        if (currentUXMode == XRUX::PATTERN_SEL && key == PATTERN_BTN_CHAR && _ptnHeldForSelection == -1)
        {
            XRSequencer::setSelectedPage(0);
            XRUX::setCurrentMode(XRUX::PATTERN_WRITE); // force patt write mode when leaving patt / patt select action

            XRLED::clearPageLEDs();
            XRLED::clearAllStepLEDs();
            XRDisplay::drawSequencerScreen(false);

            return true;
        }
        else if (currentUXMode == XRUX::PATTERN_SEL && btnCharIsATrack(key) && ((getKeyStepNum(key) - 1) == _ptnHeldForSelection))
        {
            // Serial.println("unmarking pattern as held for selection!");

            auto selectedPattern = getKeyStepNum(key) - 1;

            XRSequencer::setSelectedPattern(selectedPattern);

            _ptnHeldForSelection = -1;

            XRLED::clearAllStepLEDs();

            XRSequencer::setSelectedPage(0);
            XRUX::setCurrentMode(XRUX::PATTERN_WRITE); // force patt write mode when leaving patt / patt select action

            XRDisplay::drawSequencerScreen(false);

            return true;
        }

        return false;
    }

    bool handleTrackLayerReleaseActions(char key)
    {
        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        // track layer select
        if (currentUXMode == XRUX::UX_MODE::TRACK_LAYER_SEL && btnCharIsATrack(key)) {
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

            XRSD::saveActiveTrackLayerToSdCard();
            if (!XRSD::loadNextTrackLayer(XRSequencer::getCurrentSelectedBankNum(), XRSequencer::getCurrentSelectedPatternNum(), selTrackLayer)) {
                XRSequencer::initNextTrackLayer();
            }

            XRSD::saveActiveTrackStepModLayerToSdCard();
            if (!XRSD::loadActiveTrackStepModLayerFromSdCard(XRSequencer::getCurrentSelectedBankNum(), XRSequencer::getCurrentSelectedPatternNum(), selTrackLayer)) {
                XRSequencer::initActiveTrackStepModLayer();
            }

            XRSD::saveActiveSoundStepModLayerToSdCard();
            if (!XRSD::loadPatternSoundStepModLayerFromSdCard(XRSequencer::getCurrentSelectedBankNum(), XRSequencer::getCurrentSelectedPatternNum(), selTrackLayer)) {
                XRSound::initPatternSoundStepMods();
            }

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

    bool handleTrackReleaseActions(char key)
    {
        // Serial.println("enter handleTrackReleaseActions!");

        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        // track select / write release
        if (currentUXMode == XRUX::TRACK_SEL && key == TRACK_BTN_CHAR && _trackHeldForSelection == -1)
        {
            XRSD::unloadSampleFileListPaged();
            
            XRSequencer::setSelectedPage(0);
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
        else if (currentUXMode == XRUX::TRACK_SEL && btnCharIsATrack(key) && ((getKeyStepNum(key) - 1) == _trackHeldForSelection))
        {
            Serial.println("unmarking track as held for selection!");

            XRSequencer::setSelectedTrack(getKeyStepNum(key) - 1);
            _trackHeldForSelection = -1;

            XRSequencer::setSelectedPage(0);
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
        else if (!_isFunctionActive && !_copyBtnHeld && currentUXMode == XRUX::TRACK_WRITE && btnCharIsATrack(key) && _trackHeldForSelection == -1)
        {
            uint8_t stepToToggle = getKeyStepNum(key);

            XRSequencer::toggleSelectedStep(stepToToggle);
            XRLED::setDisplayStateForAllStepLEDs();

            Serial.println("toggling step!");

            return true;
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
            currentUXMode != XRUX::UX_MODE::PROJECT_BUSY
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
                    XRDisplay::drawSequencerScreen();

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

                    XRDisplay::drawSequencerScreen();

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


                XRDisplay::drawSequencerScreen();

                return true;

                break;

            case SELECT_BTN_CHAR:
                if (currentUXMode == XRUX::UX_MODE::SOUND_MENU_MAIN) {
                    Serial.println("entering sound sub menu");

                    auto cursorPos = XRMenu::getCursorPosition();
                    auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
#ifndef NO_DEXED
                    if (XRSound::activePatternSounds[currTrackNum].type == XRSound::T_DEXED_SYNTH) {
                        if (XRMenu::getDexedSoundMenuItems()[cursorPos] == "BROWSE DEXED SYSEX") {
                            Serial.println("in browse dexed sysex sub menu");
                            
                            XRUX::setCurrentMode(XRUX::UX_MODE::SOUND_MENU_DEXED_SYSEX_BROWSER);

                            XRSD::dexedCurrentBank = 0;
                            XRSD::dexedCurrentPatch = 0;
                            XRSD::loadDexedVoiceToCurrentTrack();
                            
                            XRDisplay::drawDexedSysexBrowser();

                            return true;
                        }
                    }
#endif
                    return true;
                }

                if (currentUXMode == XRUX::UX_MODE::CHANGE_SETUP) {
                    Serial.println("enter setup sub menu");

                    auto cursorPos = XRMenu::getCursorPosition();

                    if (XRMenu::getSetupMenuItems()[cursorPos] == "SAVE PROJECT") { // TODO: something better than a str compare
                        XRUX::setCurrentMode(XRUX::PROJECT_BUSY);
                        XRDisplay::drawSaveProject();
                        XRSD::saveProject();
                        XRMenu::resetCursor();
                        XRSequencer::setSelectedPage(0);
                        XRUX::setCurrentMode(XRUX::PATTERN_WRITE); 

                        // TODO: impl UX mode "categories", so instead of forcing pattern write mode here,
                        // leaving project modes with several transitions
                        // can happen and the user is placed back at the last known sequencer UX mode, etc

                        XRDisplay::drawSequencerScreen();

                        return true;
                    }
                }

                if (currentUXMode == XRUX::UX_MODE::SET_TEMPO) {
                    XRUX::setCurrentMode(prevMode);
                    XRDisplay::drawSequencerScreen();

                    return true;
                }

                if (currentUXMode == XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK_SOUND) {
                    XRMenu::resetCursor();
                    XRUX::setCurrentMode(prevMode);
                    XRSound::assignSampleToTrackSound();
                    XRDisplay::drawSequencerScreen();
                }

                break;

            case TEMPO_BTN_CHAR:
                if (currentUXMode == XRUX::UX_MODE::SET_TEMPO) {
                    XRUX::setCurrentMode(prevMode);
                    XRDisplay::drawSequencerScreen();

                    return true;
                }

                if (allowedModeToLaunchMenuFrom) {
                    XRUX::setCurrentMode(XRUX::UX_MODE::SET_TEMPO);
                    XRDisplay::drawSetTempoOverlay();
                    XRLED::clearAllStepLEDs();

                    return true;
                }
                
                break;
            
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

                XRDisplay::drawGeneralConfirmOverlay("SELECT TRACK LAYER");

                return true;
            }
        }

        return false;
    }
   
    bool handleParamLockStepRelease(char key)
    {
        XRUX::UX_MODE previousUXMode = XRUX::getPreviousMode();
        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        // param lock release
        if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && btnCharIsATrack(key)) {
            // revert
            XRUX::setCurrentMode(previousUXMode);

            XRSequencer::setCurrentSelectedStep(-1); // unselect the step

            XRDisplay::drawSequencerScreen(false);
        }

        return true;
    }

    bool handleActivateFunction(char key)
    {
        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        bool allowedModeToActivateFunctionFrom = (
            currentUXMode == XRUX::UX_MODE::PATTERN_WRITE || 
            currentUXMode == XRUX::UX_MODE::TRACK_WRITE || 
            currentUXMode == XRUX::UX_MODE::SOUND_MENU_DEXED_SYSEX_BROWSER
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

        if (allowedModeToParamLockFrom && btnCharIsATrack(key))
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

    bool handleRatchetRelease(char key)
    {
        auto currentUXMode = XRUX::getCurrentMode();

        if (currentUXMode == XRUX::PERFORM_RATCHET && btnCharIsATrack(key))
        {
            Serial.println("release track ratchet");

            XRSequencer::setRatchetTrack(-1);
            XRSequencer::setRatchetDivision(-1);

            XRDisplay::drawSequencerScreen(false);

            return true;
        }

        return false;
    }

    bool handleRatchets(char key)
    {
        auto currentUXMode = XRUX::getCurrentMode();
        auto &seqState = XRSequencer::getSeqState();

        // perform ratchet
        if (currentUXMode == XRUX::PERFORM_RATCHET && btnCharIsATrack(key))
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

    bool btnCharIsATrack(char btnChar)
    {
        // Serial.println("enter btnCharIsATrack!");

        std::string btnCharStr;
        btnCharStr += btnChar;

        // Serial.printf("btnCharStr: %s\n", btnCharStr.c_str());
        
        std::string checkStr = "mnopstuvyz125678";
        if (btnCharStr.length() > 0) {
            if (checkStr.find(btnChar) != std::string::npos) {
                // Serial.println("char found!");
                // Serial.println("leave btnCharIsATrack!");
                return true;
            }
        }
        

        // char *validTrackChars = "mnopstuvyz125678";
        // if (strchr(validTrackChars, btnChar) != NULL)
        // {
        //     return true;
        // }

        // Serial.println("char NOT found!");
        // Serial.println("leave btnCharIsATrack!");

        return false;
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
#ifndef NO_DEXED
                    newType = XRSound::T_DEXED_SYNTH;
#elif not defined(NO_FMDRUM)
                    newType = XRSound::T_FM_DRUM;
#else
                    newType = XRSound::T_MIDI;
#endif
                }
#ifndef NO_DEXED
                else if (currType == XRSound::T_DEXED_SYNTH)
                {
                    newType = XRSound::T_FM_DRUM;
                }
#endif
#ifndef NO_FMDRUM
                else if (currType == XRSound::T_FM_DRUM)
                {
                    newType = XRSound::T_MIDI;
                }
#endif
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
#ifndef NO_DEXED
                    newType = XRSound::T_DEXED_SYNTH;
#else
                    newType = XRSound::T_MIDI;
#endif
                }
#ifndef NO_DEXED
                else if (currType == XRSound::T_DEXED_SYNTH)
                {
                    newType = XRSound::T_MIDI;
                }
#endif
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
}