#include <XRKeyMatrix.h>
#include <XRUX.h>
#include <XRKeyInput.h>
#include <XRDisplay.h>
#include <XRSequencer.h>
#include <XRSD.h>
#include <XRMenu.h>
#include <XRClock.h>

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

    bool _isFunctionActive = false;

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
        // auto currentUXMode = XRUX::getCurrentMode();
    }

    void handleHoldForKey(char key)
    {
        // auto currentUXMode = XRUX::getCurrentMode();
    }

    void handleReleaseForKey(char key)
    {
        XRUX::UX_MODE currentUXMode = XRUX::getCurrentMode();
        std::string projectName = XRKeyInput::get();

        if (currentUXMode == XRUX::UX_MODE::PROJECT_INITIALIZE)
        {
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
    }

    bool isFunctionActive()
    {
        return _isFunctionActive;
    }
}