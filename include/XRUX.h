#ifndef XRUX_h
#define XRUX_h

#include <Arduino.h>
#include <XRConfig.h>

namespace XRUX
{
    enum UX_MODE
    {
        BANK_WRITE,
        BANK_SEL,
        PATTERN_WRITE,
        PATTERN_SEL,
        TRACK_WRITE,
        TRACK_SEL,
        SET_TEMPO,
        SUBMITTING_STEP_VALUE,
        PROJECT_INITIALIZE,
        PROJECT_BUSY, // The default mode during boot
        CHANGE_SETUP,
        PERFORM_SEL,
        PERFORM_TAP,
        PERFORM_MUTE,
        PERFORM_SOLO,
        PERFORM_RATCHET,
        COPY_SEL,
        COPY_PATTERN,
        COPY_TRACK,
        COPY_STEP,
        PASTE_SEL,
        PASTE_PATTERN,
        PASTE_TRACK,
        PASTE_STEP,
        UNDO,
        REDO,
        SOUND_MENU_MAIN,
        ASSIGN_SAMPLE_TO_TRACK_SOUND
    };

    void setCurrentMode(UX_MODE mode);
    void setPreviousMode(UX_MODE mode);

    UX_MODE getCurrentMode();
    UX_MODE getPreviousMode();
}

#endif /* XRUX_h */