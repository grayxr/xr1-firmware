#ifndef XRMIDI_h
#define XRMIDI_h

#include <Arduino.h>
//#include <MIDI.h>
#include <XRConfig.h>

namespace XRMIDI
{
    void init();

    //void sendNoteOn(midi::DataByte inNoteNumber, midi::DataByte inVelocity, midi::Channel inChannel);
    //void sendNoteOff(midi::DataByte inNoteNumber, midi::DataByte inVelocity, midi::Channel inChannel);
}

#endif /* XRMIDI_h */