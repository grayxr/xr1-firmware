#include <XRMIDI.h>

namespace XRMIDI
{
    MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

    void init()
    {
        MIDI.begin();
    }
}