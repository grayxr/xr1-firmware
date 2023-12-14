#ifndef XRMain_h
#define XRMain_h

#include <Arduino.h>
#include <XRSD.h>
#include <XRAudio.h>
#include <XREncoder.h>
#include <XRCV.h>
#include <XRLED.h>
#include <XRVersa.h>
#include <XRKeyMatrix.h>
#include <XRMIDI.h>
#include <XRDisplay.h>
#include <XRClock.h>
#include <XRUX.h>
#include <SD.h>
#include <string>
#include <map>

namespace XRMain
{
    void boot();
    void update();
}

#endif /* XRMain_h */