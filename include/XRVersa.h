#ifndef XRVersa_h
#define XRVersa_h

#include <Arduino.h>
#include <Adafruit_MPR121.h>
#include <FastTouch.h>
#include <XRConfig.h>

namespace XRVersa
{
    bool init();

    void handleStates();
    void handleProjectNameInput();
    void mprUpdate();
    void mprUpdateExclusive();
    void fastBtnUpdate();

    int8_t getKeyboardNotesHeld();
    int8_t getNoteOnKeyboard();
}

#endif /* XRVersa_h */