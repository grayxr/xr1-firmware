#include <XRVersa.h>
#include <XRUX.h>
#include <map>
#include <XRDisplay.h>
#include <XRKeyInput.h>
#include <XRSound.h>
#include <XRSequencer.h>

namespace XRVersa
{
    elapsedMillis elapsedMs;

    // You can have up to 4 on one i2c bus but one is enough for testing!
    Adafruit_MPR121 mpr121_a = Adafruit_MPR121();

    uint8_t _noteOnKeyboard = 0;
    int8_t _keyboardNotesHeld = 0;

    int32_t keyPressed = -1; // -1 means none pressed

    // Keeps track of the last pins touched
    // so we know when buttons are 'released'
    uint16_t _mprLastTouched = 0;
    uint16_t _mprCurrTouched = 0;

    uint16_t _mprExcLastTouched = 0;
    uint16_t _mprExcCurrTouched = 0;

    bool _pinsPressed[13] = {
        false, false, false, false,
        false, false, false, false,
        false, false, false, false,
        false};

    bool _fastBtnPressed = false;
    bool _playingNoteOnKeyboard = false;

    std::map<int32_t, uint8_t> invertedKeys = {
        {0, 12},
        {1, 11},
        {2, 10},
        {3, 9},
        {4, 8},
        {5, 7},
        {6, 6},
        {7, 5},
        {8, 4},
        {9, 3},
        {10, 2},
        {11, 1},
        {12, 0},
    };

    const uint8_t _backwardsNoteNumbers[13] = {
        12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
    };

    void handleNoteInput();

    bool init()
    {
        // Default address is 0x5A, if tied to 3.3V its 0x5B
        // If tied to SDA its 0x5C and if SCL then 0x5D
        if (!mpr121_a.begin(0x5B, &Wire2))
        {
            while (1)
            {
                return false;
            }
        }

        return true;
    }

    void handleStates()
    {
        // mprUpdate();
        // fastBtnUpdate();

        auto currentUXMode = XRUX::getCurrentMode();

        if (currentUXMode == XRUX::PROJECT_INITIALIZE)
        {
            mprUpdateExclusive();

            if (!(elapsedMs % 100))
            { // reduce touchiness of fast touch pin
                fastBtnUpdate();
            }

            if (keyPressed > -1)
            {
                handleProjectNameInput();
            }

            return;
        }

        bool allowedModeToPlayKeysFrom = (
            currentUXMode == XRUX::UX_MODE::PATTERN_WRITE || 
            currentUXMode == XRUX::UX_MODE::TRACK_WRITE || 
            currentUXMode == XRUX::UX_MODE::PERFORM_TAP ||
            currentUXMode == XRUX::UX_MODE::PERFORM_MUTE ||
            currentUXMode == XRUX::UX_MODE::PERFORM_SOLO ||
            currentUXMode == XRUX::UX_MODE::PERFORM_RATCHET
        );

        if (allowedModeToPlayKeysFrom) {
            mprUpdate();

            if (!(elapsedMs % 100))
            { // reduce touchiness of fast touch pin
                fastBtnUpdate();
            }

            // if (keyPressed > -1)
            // {
            //     handleNoteInput();
            // }

            return;
        }
    }

    void handleProjectNameInput()
    {
        uint8_t invertedKeyPressed = invertedKeys[keyPressed];

        switch (invertedKeyPressed)
        {
        case XRKeyInput::TEXT_KEYS::ELEM1:
        case XRKeyInput::TEXT_KEYS::ELEM2:
        case XRKeyInput::TEXT_KEYS::ELEM3:
        case XRKeyInput::TEXT_KEYS::ELEM4:
        case XRKeyInput::TEXT_KEYS::ELEM5:
        case XRKeyInput::TEXT_KEYS::ELEM6:
        case XRKeyInput::TEXT_KEYS::ELEM7:
        case XRKeyInput::TEXT_KEYS::ELEM8:
            XRKeyInput::enterForElement((XRKeyInput::TEXT_KEYS)invertedKeyPressed);
            XRDisplay::drawCreateProjectDialog();

            break;
        case XRKeyInput::TEXT_KEYS::LEFT_ARROW:
            XRKeyInput::selectPreviousKeyRow();
            XRDisplay::drawCreateProjectDialog();
            break;
        case XRKeyInput::TEXT_KEYS::RIGHT_ARROW:
            XRKeyInput::selectNextKeyRow();
            XRDisplay::drawCreateProjectDialog();
            break;
        case XRKeyInput::TEXT_KEYS::CASE_TOGGLE:
            XRKeyInput::toggleAlphaCase();
            XRDisplay::drawCreateProjectDialog();
            break;
        case XRKeyInput::TEXT_KEYS::NUMBERS_SELECT:
            XRKeyInput::selectNumericKeyset();
            XRDisplay::drawCreateProjectDialog();
            break;
        case XRKeyInput::TEXT_KEYS::SYMBOLS_SELECT:
            XRKeyInput::selectSymbolKeyset();
            XRDisplay::drawCreateProjectDialog();
            break;

        default:
            Serial.printf("invalid key press: %d\n", invertedKeyPressed);
            break;
        }

        // check input
        // Serial.printf("key press: %d, inverted key press: %d\n", keyPressed, invertedKeyPressed);
        // Serial.printf("current input string: %s\n", XRKeyInput::get().c_str());
    }

    void mprUpdateExclusive()
    {
        _mprExcCurrTouched = mpr121_a.touched();

        for (uint8_t i = 0; i < 12; i++)
        {
            // if *is* touched and *wasnt* touched before, alert!
            if ((_mprExcCurrTouched & _BV(i)) && !(_mprExcLastTouched & _BV(i)))
            {
                keyPressed = i;
                break;
            }
            else
            {
                keyPressed = -1;
            }
        }

        _mprExcLastTouched = _mprExcCurrTouched;
    }

    void mprUpdate()
    {
        _mprCurrTouched = mpr121_a.touched();

        auto currentUXMode = XRUX::getCurrentMode();
        int8_t invertedNoteNumber = -1;

        bool released = false;

        for (uint8_t i = 0; i < 12; i++)
        {
            // if *is* touched and *wasnt* touched before, alert!
            if ((_mprCurrTouched & _BV(i)) && !(_mprLastTouched & _BV(i)))
            {
                Serial.printf("%d touched\n", i);

                invertedNoteNumber = _backwardsNoteNumbers[i];
                _noteOnKeyboard = invertedNoteNumber;

                if (_keyboardNotesHeld < 6) _keyboardNotesHeld++;

                Serial.printf("_keyboardNotesHeld: %d\n", _keyboardNotesHeld);

                // noteOn
                if (currentUXMode != XRUX::SUBMITTING_STEP_VALUE) {
                    XRSound::triggerTrackManually(
                        XRSequencer::getCurrentSelectedTrackNum(), 
                        _noteOnKeyboard
                    );
                }
            }
            // if it *was* touched and now *isnt*, alert!
            if (!(_mprCurrTouched & _BV(i)) && (_mprLastTouched & _BV(i)))
            {
                Serial.printf("%d released\n", i);
                
                //_noteOnKeyboard = 0; // need this?

                if (_keyboardNotesHeld > 0) _keyboardNotesHeld--;

                Serial.printf("_keyboardNotesHeld: %d\n", _keyboardNotesHeld);
                
                released = true;

                // noteOff
                if (currentUXMode != XRUX::SUBMITTING_STEP_VALUE) {
                    if (_keyboardNotesHeld == 0) {
                        XRSound::noteOffTrackManually(_noteOnKeyboard);
                    }
                }
            }
        }

        _mprLastTouched = _mprCurrTouched;
    }

    void fastBtnUpdate()
    {
        if (!_fastBtnPressed && fastTouchRead(FAST_TOUCH_PIN) >= 64)
        {
            _fastBtnPressed = true;
            keyPressed = 12;
        }
        else if (_fastBtnPressed && fastTouchRead(FAST_TOUCH_PIN) < 64)
        {
            _fastBtnPressed = false;
            keyPressed = -1;
        }
    }

    void handleNoteInput()
    {
        
    }
}