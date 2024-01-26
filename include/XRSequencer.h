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
    } TRACK_LAYER;

    typedef struct
    {
        TRACK_LAYER layers[MAXIMUM_SEQUENCER_TRACK_LAYERS];

        bool initialized = false;
    } TRACK;

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
        PATTERN patterns[MAXIMUM_SEQUENCER_PATTERNS];
    } PATTERN_BANK;
    
    // sequencer state

    typedef struct
    {
        int8_t bank = -1;
        int8_t number = -1;
    } QUEUED_PATTERN_STATE;

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

    enum TRACK_MOD_NUM : uint8_t
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
        int8_t mods[MAXIMUM_TRACK_MODS];
        bool flags[MAXIMUM_TRACK_MODS]; // whether the param mod should apply or not
    } TRACK_STEP_MOD;

    typedef struct
    {
        TRACK_STEP_MOD steps[MAXIMUM_SEQUENCER_STEPS];
    } TRACK_MOD_LAYER;

    typedef struct
    {
        TRACK_MOD_LAYER layers[MAXIMUM_SEQUENCER_TRACK_LAYERS];
    } TRACK_MOD;

    typedef struct
    {
        TRACK_MOD tracks[MAXIMUM_SEQUENCER_TRACKS];
    } TRACK_STEP_MODS;

    // extern globals
    extern TRACK_PERFORM_STATE trackPerformState[MAXIMUM_SEQUENCER_TRACKS];
    extern PATTERN activePattern;
    extern DMAMEM PATTERN_BANK activePatternBank;
    extern DMAMEM TRACK_STEP_MODS trackStepMods;

    bool init();

    void initPatternBank();
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
    void handlePatternQueueActions();
    void handleTrackLayerQueueActions();
    void queuePattern(int pattern, int bank);

    void setSelectedPattern(int8_t pattern);
    void setSelectedTrack(int8_t track);
    void setSelectedTrackLayer(int8_t layer);
    void setSelectedPage(int8_t page);
    void initializeCurrentSelectedTrack();
    void setCurrentSelectedStep(int step);
    void setCurrentStepPage(int8_t page);
    void setRatchetTrack(int track);
    void setRatchetDivision(int track);

    void setCopyBufferForPattern(int pattern);
    void setCopyBufferForTrack(int track);
    void setCopyBufferForStep(int step);

    void toggleSequencerPlayback(char btn);
    void rewindAllCurrentStepsForAllTracks();

    PATTERN &getCopyBufferForPattern();
    TRACK &getCopyBufferForTrack();
    STEP &getCopyBufferForStep();

    SEQUENCER_STATE &getSeqState();
    QUEUED_PATTERN_STATE &getQueuedPatternState();

    PATTERN_BANK &getActivePatternBank();
    PATTERN &getActivePattern();
    TRACK &getTrack(int track);
    TRACK_LAYER &getTrackLayer(int track, int layer);
    STEP &getStep(int track, int layer, int step);

    PATTERN &getCurrentSelectedPattern();
    TRACK &getCurrentSelectedTrack();
    TRACK_LAYER &getCurrentSelectedTrackLayer();
    STEP &getCurrentSelectedTrackStep();

    int8_t getCurrentSelectedBankNum();
    int8_t getCurrentSelectedPatternNum();
    int8_t getCurrentSelectedTrackNum();
    int8_t getCurrentSelectedTrackLayerNum();
    int8_t getCurrentSelectedStepNum();
    int8_t getCurrentSelectedPage();
    int8_t getCurrentStepPage();
    int8_t getRatchetTrack();
    int8_t getRatchetDivision();

    bool * getInitializedTracksForPattern(int bank, int pattern);
}

#endif /* XRSequencer_h */