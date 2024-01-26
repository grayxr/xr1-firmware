#ifndef XRSequencer_h
#define XRSequencer_h

#include <Arduino.h>
#include <XRConfig.h>
#include <XRSound.h>
#include <TeensyVariablePlayback.h>

namespace XRSequencer
{
    enum STEP_STATE : uint8_t
    {
        STATE_OFF = 0,
        STATE_ON = 1,
        STATE_ACCENTED = 2
    };

    typedef struct
    {
        STEP_STATE state = STEP_STATE::STATE_OFF;
    } STEP;

    typedef struct
    {
        bool muted = false;
        bool soloing = false;
    } TRACK_PERFORM_STATE;

    typedef struct
    {
        STEP steps[MAXIMUM_SEQUENCER_STEPS];

        uint8_t lstep = DEFAULT_LAST_STEP;
        uint8_t note = 0;   // 0 = C1
        uint8_t octave = 4; // 4 = middle C (C4)
        uint8_t length = 4; // 1 = 1/64 step len
        uint8_t velocity = 50; // 50 = 50%
        uint8_t probability = 100;

        bool initialized = false;
    } TRACK;

    typedef struct
    {
        TRACK tracks[MAXIMUM_SEQUENCER_TRACKS];
    } TRACK_LAYER;

    typedef struct
    {
        int8_t id = -1;
        uint8_t amount = 0;
    } PATTERN_GROOVE;

    typedef struct
    {
        TRACK tracks[MAXIMUM_SEQUENCER_TRACKS];

        PATTERN_GROOVE groove;
        uint8_t lstep = DEFAULT_LAST_STEP;
        uint8_t accent = DEFAULT_GLOBAL_ACCENT;
        float tempo;

        bool initialized = false;
    } PATTERN;

    typedef struct
    {
        int8_t bank = -1;
        int8_t number = -1;
    } QUEUED_PATTERN;

    typedef struct
    {
        PATTERN patterns[MAXIMUM_SEQUENCER_PATTERNS];
    } SEQUENCER;
    
    // sequencer state

    enum SEQUENCER_PLAYBACK_STATE : uint8_t
    {
        STOPPED = 0,
        RUNNING = 1,
        PAUSED = 2
    };

    typedef struct
    {
        int8_t currentStep = 1;
        int8_t currentBar = 1;
    } TRACK_CURRENT_STEP_STATE;

    typedef struct
    {
        SEQUENCER_PLAYBACK_STATE playbackState = STOPPED;
        TRACK_CURRENT_STEP_STATE currentTrackSteps[MAXIMUM_SEQUENCER_TRACKS];
        int8_t currentStep = 1;
        int8_t currentBar = 1;
    } SEQUENCER_STATE;

    // noteOn / noteOff stacks

    typedef struct
    {
        int8_t trackNum = -1;
        int8_t stepNum = -1;
        int8_t length = -1; // need -1 to know if active/not active
        // int8_t microtiming;
    } STACK_STEP_DATA;

    typedef struct
    {
        int8_t trackNum = -1;
        int8_t length = -1;
    } STACK_RATCHET_DATA;

    // mods

    enum TRACK_MOD : uint8_t
    {
        NOTE = 0,
        OCTAVE = 1,
        LENGTH = 2,
        VELOCITY = 3,
        PROBABILITY = 4,
        MICROTIMING = 5
    };

    typedef struct
    {
        int8_t mods[MAXIMUM_TRACK_MODS]; // divide by 100 to get real param mod values
        bool flags[MAXIMUM_TRACK_MODS]; // whether the param mod should apply or not
    } TRACK_STEP_MODS; // ~11b

    typedef struct
    {
        TRACK_STEP_MODS steps[MAXIMUM_SEQUENCER_STEPS];
    } TRACK_MODS;

    typedef struct
    {
        TRACK_MODS tracks[MAXIMUM_SEQUENCER_TRACKS];
    } TRACK_MOD_LAYER;

    typedef struct
    {
        TRACK_MOD_LAYER layers[MAXIMUM_SEQUENCER_TRACK_LAYERS];
    } TRACK_MOD_LAYER_COLLECTION;

    // extern globals
    extern PATTERN heapPattern;
    extern TRACK_PERFORM_STATE trackPerformState[MAXIMUM_SEQUENCER_TRACKS];
    extern DMAMEM SEQUENCER sequencer;

    extern DMAMEM TRACK_LAYER trackLayers[MAXIMUM_SEQUENCER_TRACK_LAYERS];

    // we only keep the current pattern's track mods in memory,
    // when a pattern change occurs, the next pattern's track mods are loaded from the SD card
    extern DMAMEM TRACK_MOD_LAYER_COLLECTION layeredTrackStepMods;

    bool init();

    void initSequencer();
    void initTrackLayers();
    void initTrackStepMods();
    
    void swapSequencerMemoryForPattern(int newBank, int newPattern);
    void saveCurrentPatternOffHeap();

    void toggleSelectedStep(uint8_t step);

    void onClockStart();
    void onClockStop();
    void ClockOut16PPQN(uint32_t tick);
    void ClockOut96PPQN(uint32_t tick);

    void handle16PPQN(uint32_t tick);
    void handle96PPQN(uint32_t tick);

    void triggerAllStepsForAllTracks(uint32_t tick);
    void triggerRatchetingTrack(uint32_t tick);
    void handleAddToStepStack(uint32_t tick, int track, int step);
    void handleAddToRatchetStack();
    void handleRemoveFromStepStack(uint32_t tick);
    void handleRemoveFromRatchetStack();

    void handleNoteOnForTrack(int track);
    void handleNoteOffForTrack(int track);
    void handleNoteOnForTrackStep(int track, int step);
    void handleNoteOffForTrackStep(int track, int step);
    
    void updateAllTrackStepStates();
    void updateCurrentPatternStepState();
    void setDisplayStateForPatternActiveTracksLEDs(bool enable);
    void noteOffForAllSounds();
    void handleQueueActions();
    void queuePattern(int pattern, int bank);

    void setSelectedPattern(int8_t pattern);
    void setSelectedTrack(int8_t track);
    void setSelectedPage(int8_t page);
    void initializeCurrentSelectedTrack();
    void setCurrentSelectedStep(int step);
    void setCurrentStepPage(int8_t page);
    void setRatchetTrack(int track);
    void setRatchetDivision(int track);

    void setCopyBufferForPattern(int pattern);
    void setCopyBufferForTrack(int track);
    void setCopyBufferForStep(int step);

    PATTERN &getCopyBufferForPattern();
    TRACK &getCopyBufferForTrack();
    STEP &getCopyBufferForStep();

    void toggleSequencerPlayback(char btn);
    void rewindAllCurrentStepsForAllTracks();

    SEQUENCER_STATE &getSeqState();

    SEQUENCER &getSequencer();
    QUEUED_PATTERN &getQueuedPattern();
    PATTERN &getHeapPattern();

    TRACK &getHeapTrack(int track);
    STEP &getHeapStep(int track, int step);
    PATTERN &getHeapCurrentSelectedPattern();
    TRACK &getHeapCurrentSelectedTrack();
    STEP &getHeapCurrentSelectedTrackStep();

    // PATTERN_MODS &getModsForCurrentPattern();
    TRACK_STEP_MODS &getModsForCurrentTrackStep();

    int8_t getCurrentSelectedBankNum();
    int8_t getCurrentSelectedPatternNum();
    int8_t getCurrentSelectedTrackNum();
    int8_t getCurrentSelectedStepNum();
    int8_t getCurrentStepPage();

    // the current page for the current track layer (default layer is "sound")
    int8_t getCurrentSelectedPage();
    uint8_t getCurrentSelectedTrackLayer();

    int8_t getRatchetTrack();
    int8_t getRatchetDivision();

    bool * getInitializedTracksForPattern(int bank, int pattern);
}

#endif /* XRSequencer_h */