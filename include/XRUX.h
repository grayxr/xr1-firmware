#ifndef XRUX_h
#define XRUX_h

#include <Arduino.h>
#include <XRConfig.h>

namespace XRUX
{
    enum UX_MODE : uint8_t
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
        PERFORM_FILL_CHAIN,
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
        SOUND_MENU_DEXED_SYSEX_BROWSER,
        ASSIGN_SAMPLE_TO_TRACK_SOUND,
        TRACK_LAYER_SEL,
        STEP_PREVIEW,
        COPY_ERROR,
        PATTERN_CHANGE_QUEUED,
        PATTERN_CHANGE_INSTANT,
        SUBMITTING_RATCHET_STEP_VALUE,
        FILL_BASE_LAYER_CHANGE,
        TRACK_LAYER_QUEUED,
    };

    void setCurrentMode(UX_MODE mode);
    void setPreviousMode(UX_MODE mode);

    UX_MODE getCurrentMode();
    UX_MODE getPreviousMode();
}

#endif /* XRUX_h */