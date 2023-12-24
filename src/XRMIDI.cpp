#include <XRMIDI.h>
#ifdef BUILD_FOR_LINUX
#include "RtMidiMIDI.h"
#include "RtMidiTransport.h"
#endif
namespace XRMIDI
{
#ifdef BUILD_FOR_LINUX
    MIDI_CREATE_RTMIDI_INSTANCE(RtMidiMIDI, rtMIDI,  MIDI);
#else
    MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
#endif
    void init()
    {
        MIDI.begin();
    }

    void sendNoteOn(midi::DataByte inNoteNumber, midi::DataByte inVelocity, midi::Channel inChannel)
    {
        MIDI.sendNoteOn(inNoteNumber, inVelocity, inChannel);
    }

    void sendNoteOff(midi::DataByte inNoteNumber, midi::DataByte inVelocity, midi::Channel inChannel)
    {
        MIDI.sendNoteOff(inNoteNumber, inVelocity, inChannel);
    }
}