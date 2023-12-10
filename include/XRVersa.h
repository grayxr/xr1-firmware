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
}

#endif /* XRVersa_h */