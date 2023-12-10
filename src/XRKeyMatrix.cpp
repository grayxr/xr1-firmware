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

    int8_t _trackHeldForSelection = -1; // default to -1 (none)

    Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

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
        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();
        
        // bool allowedModeToToggleSequencerFrom = (
        //     currentUXMode == XRUX::UX_MODE::PATTERN_WRITE || 
        //     currentUXMode == XRUX::UX_MODE::TRACK_WRITE || 
        //     currentUXMode == XRUX::UX_MODE::PERFORM_TAP ||
        //     currentUXMode == XRUX::UX_MODE::PERFORM_MUTE ||
        //     currentUXMode == XRUX::UX_MODE::PERFORM_SOLO ||
        //     currentUXMode == XRUX::UX_MODE::PERFORM_RATCHET
        // );

        if (!_isFunctionActive) {
            // start/pause or stop
            if (key == 'q' || key == 'w') {
                XRSequencer::toggleSequencerPlayback(key);
                XRDisplay::drawSequencerScreen(false);
            }

             // track select
            else if ((currentUXMode != XRUX::UX_MODE::COPY_SEL) && key == 'c') {
                Serial.println("enter track select mode!");
                XRUX::setCurrentMode(XRUX::UX_MODE::TRACK_SEL);

                // Serial.printf("pattern_copy_available %d track_copy_available %d step_copy_available %d", pattern_copy_available, track_copy_available, step_copy_available);
                // Serial.println(" -- Resetting copy flags if we've left a copy mode!");

                _patternCopyAvailable = false;
                _trackCopyAvailable = false;
                _stepCopyAvailable = false;

                XRLED::clearAllStepLEDs();
                XRLED::displayCurrentlySelectedTrack();
            } else if (currentUXMode == XRUX::UX_MODE::TRACK_SEL && btnCharIsATrack(key)) {
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
            } else if (currentUXMode == XRUX::UX_MODE::TRACK_SEL && key == SOUND_BTN_CHAR) {
                Serial.println("TODO: impl track sound change!");
                // TRACK_TYPE currType = _seq_heap.pattern.tracks[current_selected_track].track_type;
                
                // Serial.print("changing ");
                // Serial.print(current_selected_track+1);
                // Serial.print(" track sound for pattern ");
                // Serial.print(current_selected_pattern+1);
                // Serial.print(" to sound type: ");
                // Serial.println(currType);

                // TRACK_TYPE newType = current_selected_track > 3 ? RAW_SAMPLE : SUBTRACTIVE_SYNTH;
                // if (currType == SUBTRACTIVE_SYNTH) {
                // newType = RAW_SAMPLE;
                // } else if (currType == RAW_SAMPLE) {
                // newType = DEXED;
                // } else if (currType == DEXED) {
                // newType = WAV_SAMPLE;
                // } else if (currType == WAV_SAMPLE) {
                // newType = MIDI_OUT;
                // } else if (currType == MIDI_OUT) {
                // newType = CV_GATE;
                // } else if (currType == CV_GATE) {
                // newType = CV_TRIG;
                // }

                // // reset page to 0
                // current_page_selected = 0;

                // changeTrackSoundType(current_selected_track, newType);

                // XRDisplay::drawSequencerScreen(false);
            }
        }
    }

    void handleHoldForKey(char key)
    {
        // auto currentUXMode = XRUX::getCurrentMode();
    }

    void handleReleaseForKey(char key)
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

                break;

            case SELECT_BTN_CHAR:
                if (projectName.length() < 1) {
                    return;
                }
                
                XRUX::setCurrentMode(XRUX::UX_MODE::PROJECT_BUSY);
                XRDisplay::drawSaveProject();

                delay(100);

                XRSD::createNewProject();

                delay(100);

                XRUX::setCurrentMode(XRUX::UX_MODE::PATTERN_WRITE);
                XRSequencer::init();
                //XRSound::loadVoiceSettings();

                break;
            
            default:
                break;
            }

            return;
        }

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
                    return;
                }

                if (allowedModeToLaunchMenuFrom) {
                    XRUX::setCurrentMode(XRUX::UX_MODE::SOUND_MENU_MAIN);
                    XRDisplay::drawSoundMenuMain();
                }

                break;

            case ESCAPE_BTN_CHAR:
                XRMenu::resetCursor();
                XRUX::setCurrentMode(prevMode);
                XRDisplay::drawSequencerScreen();

                break;

            case SELECT_BTN_CHAR:
                XRMenu::resetCursor();

                if (currentUXMode == XRUX::UX_MODE::SOUND_MENU_MAIN) {
                    Serial.println("enter sub menu");

                    return;
                }

                if (currentUXMode == XRUX::UX_MODE::SET_TEMPO) {
                    XRUX::setCurrentMode(prevMode);
                    XRDisplay::drawSequencerScreen();
                    return;
                }

                break;

            case TEMPO_BTN_CHAR:
                if (currentUXMode == XRUX::UX_MODE::SET_TEMPO) {
                    XRUX::setCurrentMode(prevMode);
                    XRDisplay::drawSequencerScreen();
                    return;
                }

                if (allowedModeToLaunchMenuFrom) {
                    XRUX::setCurrentMode(XRUX::UX_MODE::SET_TEMPO);
                    XRDisplay::drawSetTempoOverlay();
                    return;
                }
                
                break;
            
            default:
                break;
            }
        }

        // track select / write release
        if (currentUXMode == XRUX::TRACK_SEL && key == 'c' && _trackHeldForSelection == -1)
        {
            XRUX::setCurrentMode(XRUX::TRACK_WRITE);
            XRUX::setPreviousMode(XRUX::TRACK_WRITE); // force track write mode when leaving track / track select action

            XRLED::displayPageLEDs(
                -1, 
                (XRSequencer::getSeqState().playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::RUNNING),
                XRSequencer::getCurrentSelectedPage(),
                XRSequencer::getHeapCurrentSelectedTrack().last_step // TODO: use pattern last step here?
            );
            XRLED::clearAllStepLEDs(); // need?
            XRLED::setDisplayStateForAllStepLEDs();

            XRDisplay::drawSequencerScreen(false);
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
            XRLED::clearAllStepLEDs();
            XRLED::setDisplayStateForAllStepLEDs();
            
            XRDisplay::drawSequencerScreen(false);
        }
        else if (!_isFunctionActive && !_copyBtnHeld && currentUXMode == XRUX::TRACK_WRITE && btnCharIsATrack(key) && _trackHeldForSelection == -1)
        {
            uint8_t stepToToggle = getKeyStepNum(key);

            XRSequencer::toggleSelectedStep(stepToToggle);
            XRLED::setDisplayStateForAllStepLEDs();

            Serial.println("toggling step!");
        }
    }

    bool isFunctionActive()
    {
        return _isFunctionActive;
    }

    bool btnCharIsATrack(char btnChar)
    {
        char *validTrackChars = "mnopstuvyz125678";
        if (strchr(validTrackChars, btnChar) != NULL)
        {
            return true;
        }

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
}