#include <XRVersa.h>
#include <XRUX.h>
#include <map>
#include <XRDisplay.h>
#include <XRKeyInput.h>
#include <XRSound.h>
#include <XRSequencer.h>
#include <XRKeyMatrix.h>
#include <XRHelpers.h>
#include <XRLED.h>

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

    uint8_t _ratchetsHeld = 0;
    long _ratchetReleaseTime;

    bool _notesPressed[13] = {
        false, false, false, false,
        false, false, false, false,
        false, false, false, false,
        false};

    std::map<uint8_t, uint8_t> _keyedRatchetDivisions = {
        {0, 24},  // 1/4 note
        {1, 16},  // 1/6 note
        {2, 12},  // 1/8 note
        {3, 8},   // 1/12 note
        {4, 6},   // 1/16 note
        {5, 3},   // 1/32 note
        {6, 4},   // 1/24 note
        {7, 2},   // 1/48 note
        {8, 24},  // 1/24 note
        {9, 1},   // 1/96 note
        {10, -1}, // n/a
        {11, -1}, // n/a
        {12, -1}, // n/a
    };

    std::map<uint8_t, int8_t> _fillKeys = {
        {0, -1},  // n/a
        {1, 0},   // -- fill mode OFF
        {2, 2},   // 2nd measure
        {3, 1},   // -- fill mode ON
        {4, 4},   // 4th measure
        {5, 8},   // 8th measure
        {6, 100}, // -- chain mode ON
        {7, 16},  // 16th measure
        {8, -1},  // n/a
        {9, -1},  // n/a
        {10, -1}, // n/a
        {11, -1}, // n/a
        {12, -1}, // n/a
    };

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
    void handleNoteOnInput(uint8_t pin);
    void handleNoteOffInput(uint8_t pin);
    void handleKeyboardSetRatchets();
    void handleKeyboardSetFill();

    void mprUpdateForRatchets();
    void fastBtnUpdateForRatchets();

    void mprUpdateForFill();

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
        auto &queuedPattern = XRSequencer::getQueuedPatternState();
        auto currentUXMode = XRUX::getCurrentMode();

        // TODO: remove?
        if (queuedPattern.bank > -1 || queuedPattern.number > -1) {
            //Serial.println("pattern queued, don't allow versa keyboard events!");

            return;
        }
        
        if (currentUXMode == XRUX::UX_MODE::PERFORM_RATCHET || currentUXMode == XRUX::UX_MODE::SUBMITTING_RATCHET_STEP_VALUE) {
            handleKeyboardSetRatchets();

            return;
        }

        if (currentUXMode == XRUX::UX_MODE::PERFORM_FILL_CHAIN) {
            handleKeyboardSetFill();

            return;
        }

        if (currentUXMode == XRUX::UX_MODE::PROJECT_INITIALIZE)
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
            currentUXMode == XRUX::UX_MODE::TRACK_SEL || 
            currentUXMode == XRUX::UX_MODE::TRACK_WRITE || 
            currentUXMode == XRUX::UX_MODE::PERFORM_FILL_CHAIN ||
            currentUXMode == XRUX::UX_MODE::PERFORM_MUTE ||
            currentUXMode == XRUX::UX_MODE::PERFORM_SOLO ||
            currentUXMode == XRUX::UX_MODE::SUBMITTING_STEP_VALUE ||
            currentUXMode == XRUX::UX_MODE::SOUND_MENU_DEXED_SYSEX_BROWSER
        );

        if (allowedModeToPlayKeysFrom) {
            mprUpdateForNote();

            if (!(elapsedMs % 10)) { // reduce touchiness of fast touch pin
                fastBtnUpdateForNote();
            }

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

    void mprUpdateForNote()
    {
        _mprCurrTouched = mpr121_a.touched();

        // bool released = false;

        for (uint8_t i = 0; i < 12; i++)
        {
            // if *is* touched and *wasnt* touched before, alert!
            if ((_mprCurrTouched & _BV(i)) && !(_mprLastTouched & _BV(i)))
            {
                Serial.printf("%d touched\n", i);

                handleNoteOnInput(i);
            }
            // if it *was* touched and now *isnt*, alert!
            if (!(_mprCurrTouched & _BV(i)) && (_mprLastTouched & _BV(i))) {
                Serial.printf("%d released\n", i);

                handleNoteOffInput(i);
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

    void fastBtnUpdateForNote()
    {
        if (!_fastBtnPressed && fastTouchRead(FAST_TOUCH_PIN) >= 64)
        {
            _fastBtnPressed = true;
            
            handleNoteOnInput(12);
        }
        else if (_fastBtnPressed && fastTouchRead(FAST_TOUCH_PIN) < 64)
        {
            _fastBtnPressed = false;
            
            handleNoteOffInput(12);
        }
    }

    void handleNoteOnInput(uint8_t pin)
    {
        auto currentUXMode = XRUX::getCurrentMode();

        int8_t invertedNoteNumber = -1;
        invertedNoteNumber = _backwardsNoteNumbers[pin];
                
        _noteOnKeyboard = invertedNoteNumber;

        if (_keyboardNotesHeld < 6) _keyboardNotesHeld++;

        if (invertedNoteNumber > -1 && _notesPressed[invertedNoteNumber] == false) {
            _notesPressed[invertedNoteNumber] = true;

            // noteOn
            if (currentUXMode != XRUX::SUBMITTING_STEP_VALUE && currentUXMode != XRUX::TRACK_SEL) {
                XRSound::triggerTrackManually(
                    XRSequencer::getCurrentSelectedTrackNum(), 
                    _noteOnKeyboard,
                    XRKeyMatrix::getKeyboardOctave(),
                    XRKeyMatrix::isSelectBtnHeld()
                );

                if (currentUXMode == XRUX::PATTERN_WRITE) {
                    XRLED::toggleNoteOnForTrackLED(XRSequencer::getCurrentSelectedTrackNum(), true);
                }
            }

            Serial.printf("_keyboardNotesHeld: %d, invertedNoteNumber: %d\n", _keyboardNotesHeld, invertedNoteNumber);
        }
    }

    void handleNoteOffInput(uint8_t pin)
    {
        auto currSelTrackNum = XRSequencer::getCurrentSelectedTrackNum(); 
        auto currSelStepNum = XRSequencer::getCurrentSelectedStepNum(); 
        auto currentUXMode = XRUX::getCurrentMode();
        auto layer = XRSequencer::getCurrentSelectedTrackLayerNum();
        auto &currTrack = XRSequencer::getCurrentSelectedTrack();
        auto &currLayer = XRSequencer::activeTrackLayer;

        int8_t invertedNoteNumber = -1;
        invertedNoteNumber = _backwardsNoteNumbers[pin];
        
        _noteOnKeyboard = invertedNoteNumber;

        if (_keyboardNotesHeld > 0) _keyboardNotesHeld--;

        if (invertedNoteNumber > -1 && _notesPressed[invertedNoteNumber]) {
            _notesPressed[invertedNoteNumber] = false;

            // noteOff
            if (currentUXMode != XRUX::SUBMITTING_STEP_VALUE && currentUXMode != XRUX::TRACK_SEL) {
                XRSound::noteOffTrackManually(invertedNoteNumber, XRKeyMatrix::getKeyboardOctave());

                if (currentUXMode == XRUX::PATTERN_WRITE && _keyboardNotesHeld == 0) {
                    XRLED::toggleNoteOnForTrackLED(XRSequencer::getCurrentSelectedTrackNum(), false);
                }
            } 
            else if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelStepNum > -1) {
                currLayer.tracks[currSelTrackNum].steps[currSelStepNum].tMods[XRSequencer::NOTE] = invertedNoteNumber;
                currLayer.tracks[currSelTrackNum].steps[currSelStepNum].tFlags[XRSequencer::NOTE] = true;
                currLayer.tracks[currSelTrackNum].steps[currSelStepNum].tMods[XRSequencer::OCTAVE] = XRKeyMatrix::getKeyboardOctave();
                currLayer.tracks[currSelTrackNum].steps[currSelStepNum].tFlags[XRSequencer::OCTAVE] = true;

                std::string noteStr = XRHelpers::getNoteStringForBaseNoteNum(invertedNoteNumber);
                noteStr += std::to_string(XRKeyMatrix::getKeyboardOctave());

                //XRDisplay::drawSequencerScreen(false);
                XRDisplay::drawGeneralConfirmOverlay("  step note: " + noteStr);
                XRSequencer::trackLayerDirty = true;
            } 
            else if (currentUXMode == XRUX::TRACK_SEL) {
                // set track note
                currTrack.note = invertedNoteNumber;
                currTrack.octave = XRKeyMatrix::getKeyboardOctave();

                std::string noteStr = XRHelpers::getNoteStringForBaseNoteNum(invertedNoteNumber);
                noteStr += std::to_string(XRKeyMatrix::getKeyboardOctave());

                XRDisplay::drawGeneralConfirmOverlay("  track note: " + noteStr);
            }

            Serial.printf("_keyboardNotesHeld: %d, invertedNoteNumber: %d\n", _keyboardNotesHeld, invertedNoteNumber);
        }
    }

    void handleKeyboardSetRatchets()
    {
        auto ratchetTrack = XRSequencer::getRatchetTrack();

        if (ratchetTrack == -1)
        {
            XRSequencer::setRatchetDivision(-1);

            return;
        }

        if (_ratchetReleaseTime != 0 && ((elapsedMs - _ratchetReleaseTime) >= 25))
        {
            _ratchetReleaseTime = 0;

            XRSequencer::setRatchetDivision(-1);
        }

        mprUpdateForRatchets();

        if (!(elapsedMs % 10)) { // reduce touchiness of fast touch pin
            fastBtnUpdateForRatchets();
        }

        return;
    }

    void mprUpdateForRatchets()
    {
        _mprCurrTouched = mpr121_a.touched();

        int8_t invertedNoteNumber = -1;

        for (size_t i = 0; i < 12; i++)
        {
            // if *is* touched and *wasnt* touched before, alert!
            if ((_mprCurrTouched & _BV(i)) && !(_mprLastTouched & _BV(i)))
            {
                if (_ratchetsHeld < 3) {
                    ++_ratchetsHeld;
                }

                invertedNoteNumber = _backwardsNoteNumbers[i];

                auto currSelStepNum = XRSequencer::getCurrentSelectedRatchetStep();
                if (XRUX::getCurrentMode() == XRUX::SUBMITTING_RATCHET_STEP_VALUE && currSelStepNum > -1)
                {
                    auto ratchetTrack = XRSequencer::getRatchetTrack();

                    Serial.printf("RATCHET invertedNoteNumber: %d, octave: %d\n", invertedNoteNumber, XRKeyMatrix::getKeyboardOctave());

                    XRSequencer::activeRatchetLayer.tracks[ratchetTrack].steps[currSelStepNum].tMods[XRSequencer::NOTE] = invertedNoteNumber;
                    XRSequencer::activeRatchetLayer.tracks[ratchetTrack].steps[currSelStepNum].tFlags[XRSequencer::NOTE] = true;
                    XRSequencer::activeRatchetLayer.tracks[ratchetTrack].steps[currSelStepNum].tMods[XRSequencer::OCTAVE] = XRKeyMatrix::getKeyboardOctave();
                    XRSequencer::activeRatchetLayer.tracks[ratchetTrack].steps[currSelStepNum].tFlags[XRSequencer::OCTAVE] = true;

                    std::string noteStr = XRHelpers::getNoteStringForBaseNoteNum(invertedNoteNumber);
                    noteStr += std::to_string(XRKeyMatrix::getKeyboardOctave());

                    XRDisplay::drawSequencerScreen(false);
                    XRDisplay::drawGeneralConfirmOverlay("  step note: " + noteStr);

                } else {
                    XRSequencer::setRatchetDivision(_keyedRatchetDivisions[invertedNoteNumber]);
                    XRDisplay::drawSequencerScreen(false);
                }

                //break;
            }

            // if it *was* touched and now *isnt*, alert!
            if (!(_mprCurrTouched & _BV(i)) && (_mprLastTouched & _BV(i)))
            {
                if (_ratchetsHeld > 0) {
                    --_ratchetsHeld;
                }
                
                // check if all ratchets are finally unheld 
                // after decrementing above
                if (_ratchetsHeld == 0 && XRSequencer::ratchetLatched == false) {
                    _ratchetReleaseTime = elapsedMs;
                    XRSequencer::setRatchetDivision(-1);
                    XRSequencer::setCurrentRatchetStep(1);
                    XRSequencer::resetRatchetBar();
                    if (XRSequencer::getCurrentRatchetPageNum() == 1) {
                        XRLED::setDisplayStateForAllStepLEDs();
                    }
                }

                XRDisplay::drawSequencerScreen(false);
            }
        }

        _mprLastTouched = _mprCurrTouched;
    }

    void fastBtnUpdateForRatchets()
    {
        int8_t invertedNoteNumber = -1;

        if (!_fastBtnPressed && fastTouchRead(FAST_TOUCH_PIN) >= 64)
        {
            if (_ratchetsHeld < 3) {
                ++_ratchetsHeld;
            }

            _fastBtnPressed = true;

            invertedNoteNumber = _backwardsNoteNumbers[12];

            auto currSelStepNum = XRSequencer::getCurrentSelectedRatchetStep();
            if (XRUX::getCurrentMode() == XRUX::SUBMITTING_RATCHET_STEP_VALUE && currSelStepNum > -1) {
                auto ratchetTrack = XRSequencer::getRatchetTrack();

                Serial.printf("RATCHET invertedNoteNumber: %d, octave: %d\n", invertedNoteNumber, XRKeyMatrix::getKeyboardOctave());

                XRSequencer::activeRatchetLayer.tracks[ratchetTrack].steps[currSelStepNum].tMods[XRSequencer::NOTE] = invertedNoteNumber;
                XRSequencer::activeRatchetLayer.tracks[ratchetTrack].steps[currSelStepNum].tFlags[XRSequencer::NOTE] = true;
                XRSequencer::activeRatchetLayer.tracks[ratchetTrack].steps[currSelStepNum].tMods[XRSequencer::OCTAVE] = XRKeyMatrix::getKeyboardOctave();
                XRSequencer::activeRatchetLayer.tracks[ratchetTrack].steps[currSelStepNum].tFlags[XRSequencer::OCTAVE] = true;

                std::string noteStr = XRHelpers::getNoteStringForBaseNoteNum(invertedNoteNumber);
                noteStr += std::to_string(XRKeyMatrix::getKeyboardOctave());

                XRDisplay::drawSequencerScreen(false);
                XRDisplay::drawGeneralConfirmOverlay("  step note: " + noteStr);

            } else {
                XRSequencer::setRatchetDivision(_keyedRatchetDivisions[invertedNoteNumber]);
                XRDisplay::drawSequencerScreen(false);
            }
        }
        else if (_fastBtnPressed && fastTouchRead(FAST_TOUCH_PIN) < 64)
        {
            invertedNoteNumber = _backwardsNoteNumbers[12];

            if (_ratchetsHeld > 0) {
                --_ratchetsHeld;
            }

            // check if all ratchets are finally unheld 
            // after decrementing above
            if (_ratchetsHeld == 0) {
                _ratchetReleaseTime = elapsedMs;
                XRSequencer::setRatchetDivision(-1);
            }

            _fastBtnPressed = false;

            XRDisplay::drawSequencerScreen(false);
        }
    }

    void handleKeyboardSetFill()
    {
        mprUpdateForFill();

        return;
    }

    void mprUpdateForFill()
    {
        _mprCurrTouched = mpr121_a.touched();

        int8_t invertedNoteNumber = -1;

        for (size_t i = 0; i < 12; i++)
        {
            // if *is* touched and *wasnt* touched before, alert!
            if ((_mprCurrTouched & _BV(i)) && !(_mprLastTouched & _BV(i)))
            {
                invertedNoteNumber = _backwardsNoteNumbers[i];

                if (_fillKeys[invertedNoteNumber] == 100) {
                    XRSequencer::layerChainState.enabled = true;
                    Serial.println("chain state ON");
                } else if (_fillKeys[invertedNoteNumber] == 1) {
                    XRSequencer::fillState.fillType = XRSequencer::FILL_TYPE::AUTO;
                    XRSequencer::layerChainState.enabled = false;
                    Serial.println("fill state: AUTO");
                } else if (_fillKeys[invertedNoteNumber] == 0) {
                    XRSequencer::fillState.fillType = XRSequencer::FILL_TYPE::MANUAL;
                    XRSequencer::layerChainState.enabled = false;
                    Serial.println("fill state: MANUAL");
                } else if (_fillKeys[invertedNoteNumber] != -1) {
                    XRSequencer::fillState.fillMeasure = _fillKeys[invertedNoteNumber];
                    Serial.printf("fill measure: %d\n", _fillKeys[invertedNoteNumber]);
                }

                XRDisplay::drawSequencerScreen(false);

                //break;
            }

            // if it *was* touched and now *isnt*, alert!
            if (!(_mprCurrTouched & _BV(i)) && (_mprLastTouched & _BV(i)))
            {
                // no-op
            }
        }

        _mprLastTouched = _mprCurrTouched;
    }

    void handleNoteInput()
    {
        
    }

    int8_t getKeyboardNotesHeld()
    {
        return _keyboardNotesHeld;
    }

    int8_t getNoteOnKeyboard()
    {
        return _noteOnKeyboard;
    }
}