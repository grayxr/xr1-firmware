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

    int8_t _keyboardOctave = 4; // range 1-7 ?
    int8_t _trackHeldForSelection = -1; // default to -1 (none)
    int8_t _ptnHeldForSelection = -1;

    Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

    void handlePressForKey(char key);
    void handleHoldForKey(char key);    
    void handleReleaseForKey(char key);

    bool handleActivateFunction(char key);
    bool handlePatternReleaseActions(char key);
    bool handleTrackReleaseActions(char key);
    bool handleCreateProjectReleaseActions(char key);
    bool handleMenuReleaseActions(char key);
    bool handleFunctionReleaseActions(char key);

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
            for (int i=0; i<LIST_MAX; i++) // Scan the whole key list.
            {
                if (discard) {
                    return;
                }

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

    void handlePressForKey(char key)
    {
        // Serial.println("enter handlePressForKey!");

        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        if (_isFunctionActive) {
            return;
        }

        // start/pause or stop
        if (key == 'q' || key == 'w') {
            XRSequencer::toggleSequencerPlayback(key);
            XRDisplay::drawSequencerScreen(false);

            return;
        }

        // track select
        if ((currentUXMode != XRUX::UX_MODE::COPY_SEL) && key == 'c') {
            Serial.println("enter track select mode!");
            XRUX::setCurrentMode(XRUX::UX_MODE::TRACK_SEL);

            _patternCopyAvailable = false;
            _trackCopyAvailable = false;
            _stepCopyAvailable = false;

            XRLED::clearAllStepLEDs();
            XRLED::displayCurrentlySelectedTrack();

            return;
        }
        
        if (currentUXMode == XRUX::UX_MODE::TRACK_SEL && btnCharIsATrack(key)) {
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

            return;
        } 
        
        if (currentUXMode == XRUX::UX_MODE::TRACK_SEL && key == SOUND_BTN_CHAR) {
            Serial.println("TODO: impl track sound change!");
            auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
            auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
            auto currType = currTrack.track_type;
            
            Serial.print("changing ");
            Serial.print(currTrackNum+1);
            Serial.print(" track sound for pattern ");
            Serial.print(currTrackNum+1);
            Serial.print(" to sound type: ");
            Serial.println(currType);

            auto newType = currTrackNum > 3 ? XRSequencer::RAW_SAMPLE : XRSequencer::SUBTRACTIVE_SYNTH;
            if (currType == XRSequencer::SUBTRACTIVE_SYNTH)
            {
                newType = XRSequencer::RAW_SAMPLE;
            }
            else if (currType == XRSequencer::RAW_SAMPLE)
            {
                newType = XRSequencer::DEXED;
            }
            else if (currType == XRSequencer::DEXED)
            {
                newType = XRSequencer::WAV_SAMPLE;
            }
            else if (currType == XRSequencer::WAV_SAMPLE)
            {
                newType = XRSequencer::MIDI_OUT;
            }
            else if (currType == XRSequencer::MIDI_OUT)
            {
                newType = XRSequencer::CV_GATE;
            }
            else if (currType == XRSequencer::CV_GATE)
            {
                newType = XRSequencer::CV_TRIG;
            }

            // reset page to 0
            XRSequencer::setSelectedPage(0);

            XRSound::changeTrackSoundType(currTrackNum, (int8_t)newType);

            XRDisplay::drawSequencerScreen(false);

            return;
        }

        // track write
        else if (currentUXMode == XRUX::UX_MODE::TRACK_WRITE && key != PATTERN_BTN_CHAR) { // TODO get allowed buttons
            auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
            auto currPageSelected = XRSequencer::getCurrentSelectedPage();

            if (key == MOD_D_BTN_CHAR) {                
                if (currTrack.track_type == XRSequencer::TRACK_TYPE::RAW_SAMPLE) {
                    if (currPageSelected == 0) {
                        XRUX::setCurrentMode(XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK);
                        XRDisplay::drawSampleBrowser();
                    }
                }
            }

            return;
        }

        // pattern select
        if (!(currentUXMode == XRUX::UX_MODE::COPY_SEL) && key == PATTERN_BTN_CHAR)
        {
            Serial.println("enter pattern select mode!");

            XRUX::setCurrentMode(XRUX::UX_MODE::PATTERN_SEL);

            _patternCopyAvailable = false;
            _trackCopyAvailable = false;
            _stepCopyAvailable = false;

            XRLED::clearAllStepLEDs();
            XRLED::displayCurrentlySelectedPattern();

        } else if (currentUXMode == XRUX::UX_MODE::PATTERN_SEL && btnCharIsATrack(key)) {
            Serial.println("confirm pattern selection!");

            int selPattern = getKeyStepNum(key) - 1; // zero-based

            auto &seqState = XRSequencer::getSeqState();
            auto currBank = XRSequencer::getCurrentSelectedBankNum();

            if (seqState.playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::RUNNING) {
                Serial.printf("queueing pattern: %d\n", selPattern);

                XRSequencer::queuePattern(selPattern, currBank);

                XRUX::setCurrentMode(XRUX::UX_MODE::PATTERN_WRITE);
            } else {
                XRSequencer::swapSequencerMemoryForPattern(currBank, selPattern);

                Serial.printf("marking pressed pattern selection (zero-based): %d", selPattern);

                _ptnHeldForSelection = selPattern;

                XRUX::setCurrentMode(XRUX::UX_MODE::PATTERN_SEL);

                XRLED::clearAllStepLEDs();
                XRLED::displayCurrentlySelectedPattern();
            }
        }

        // pattern write allow tapping tracks (and recording steps in rec mode)
        else if (currentUXMode == XRUX::UX_MODE::PATTERN_WRITE && btnCharIsATrack(key))
        {
            // auto trackNum = getKeyStepNum(key) - 1;
            // auto &track = XRSequencer::getHeapTrack(trackNum);

            // TODO: fix issue with triggering and selecting patterns, and trying to trigger tracks with no sounds:
            //XRSound::triggerTrackManually(trackNum, track.note);

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
        }
    }

    void handleHoldForKey(char key)
    {
        // Serial.println("enter handleHoldForKey!");

        if (handleActivateFunction(key)) return;
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

        if (handleTrackReleaseActions(key)) return;

        if (handlePatternReleaseActions(key)) return;
    }

    bool handlePatternReleaseActions(char key)
    {
        // Serial.println("enter handlePatternReleaseActions!");

        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        // pattern select / write release
        if (currentUXMode == XRUX::PATTERN_SEL && key == 'b' && _ptnHeldForSelection == -1)
        {
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

            XRUX::setCurrentMode(XRUX::PATTERN_WRITE); // force patt write mode when leaving patt / patt select action

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
            
            XRUX::setCurrentMode(XRUX::TRACK_WRITE);
            XRUX::setPreviousMode(XRUX::TRACK_WRITE); // force track write mode when leaving track / track select action

            XRLED::displayPageLEDs(
                -1, 
                (XRSequencer::getSeqState().playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::RUNNING),
                XRSequencer::getCurrentSelectedPage(),
                XRSequencer::getHeapCurrentSelectedTrack().last_step // TODO: use pattern last step here?
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
                XRSequencer::getHeapCurrentSelectedTrack().last_step // TODO: use pattern last step here?
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
        // Serial.println("enter handleMenuReleaseActions!");

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
                        XRUX::setCurrentMode(XRUX::PATTERN_WRITE);
                    }
                }


                XRDisplay::drawSequencerScreen();

                return true;

                break;

            case SELECT_BTN_CHAR:
                XRMenu::resetCursor();

                if (currentUXMode == XRUX::UX_MODE::SOUND_MENU_MAIN) {
                    Serial.println("enter sub menu");

                    // todo: get ux mode for selected cursor position in sub menu
                    XRUX::setCurrentMode(XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK);

                    return true;
                }

                if (currentUXMode == XRUX::UX_MODE::SET_TEMPO) {
                    XRUX::setCurrentMode(prevMode);
                    XRDisplay::drawSequencerScreen();

                    return true;
                }

                if (currentUXMode == XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK) {
                    XRUX::setCurrentMode(prevMode);
                    XRSound::assignSampleToTrack();
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
        // Serial.println("enter handleCreateProjectReleaseActions!");

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

                delay(100);

                XRUX::setCurrentMode(XRUX::UX_MODE::PATTERN_WRITE);
                XRSequencer::init();
                XRSound::loadVoiceSettings();

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
        // Serial.println("enter handleFunctionReleaseActions!");

        if (key == FUNCTION_BTN_CHAR) {
            _isFunctionActive = false;

            return true;
        }
        
        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();
        XRUX::UX_MODE previousUXMode = XRUX::getPreviousMode();

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

        return false;
    }
   
    bool handleActivateFunction(char key)
    {
        // Serial.println("enter handleActivateFunction!");

        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();

        bool allowedModeToActivateFunctionFrom = (
            currentUXMode == XRUX::UX_MODE::PATTERN_WRITE || 
            currentUXMode == XRUX::UX_MODE::TRACK_WRITE
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
    
    bool isFunctionActive()
    {
        return _isFunctionActive;
    }
    
    bool btnCharIsATrack(char btnChar)
    {
        // Serial.println("enter btnCharIsATrack!");

        std::string btnCharStr;
        btnCharStr += btnChar;

        Serial.printf("btnCharStr: %s\n", btnCharStr.c_str());
        
        std::string checkStr = "mnopstuvyz125678";
        if (btnCharStr.length() > 0) {
            if (checkStr.find(btnChar) != std::string::npos) {
                Serial.println("char found!");
                Serial.println("leave btnCharIsATrack!");
                return true;
            }
        }
        

        // char *validTrackChars = "mnopstuvyz125678";
        // if (strchr(validTrackChars, btnChar) != NULL)
        // {
        //     return true;
        // }

        Serial.println("char NOT found!");
        Serial.println("leave btnCharIsATrack!");

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
}