#ifndef XRSequencer_h
#define XRSequencer_h

#include <Arduino.h>
#include <XRConfig.h>
#include <XRSound.h>
#include <Entropy.h>

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
        int8_t tMods[MAXIMUM_TRACK_MODS];
        bool tFlags[MAXIMUM_TRACK_MODS]; // whether the param mod should apply or not
        int32_t sMods[MAXIMUM_SOUND_PARAMS];
        bool sFlags[MAXIMUM_SOUND_PARAMS]; // whether the param mod should apply or not
    } STEP;

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
        STEP steps[MAXIMUM_SEQUENCER_STEPS];

        uint8_t lstep = DEFAULT_LAST_STEP;
        uint8_t note = 0;   // 0 = C1
        uint8_t octave = 4; // 4 = middle C (C4)
        uint8_t length = 4; // 1 = 1/64 step len
        uint8_t velocity = 50; // 50 = 50%
        uint8_t probability = 100;

        bool initialized = true;
    } TRACK;

    typedef struct
    {
        TRACK tracks[MAXIMUM_SEQUENCER_TRACKS];
    } TRACK_LAYER;

    typedef struct
    {
        TRACK tracks[MAXIMUM_SEQUENCER_TRACKS];
    } RATCHET_LAYER;

    typedef struct
    {
        int8_t id = -1;
        uint8_t amount = 0;
    } PATTERN_GROOVE;

    enum PATTERN_FX_DELAY_PARAMS : uint8_t
    {
        TIME = 0,
        FEEDBACK = 1,
        PAN = 2
    };

    enum PATTERN_FX_PAGE_INDEXES : uint8_t
    {
        DELAY = 0,
        NA = 1,
    };

    typedef struct
    {
        float32_t params[MAXIMUM_PATTERN_FX_PARAMS];
    } PATTERN_FX_PARAM_PAGE;

    typedef struct
    {
        PATTERN_FX_PARAM_PAGE pages[MAXIMUM_PATTERN_FX_PARAM_PAGES];
    } PATTERN_FX_PARAMS;

    typedef struct
    {
        PATTERN_FX_PARAMS fx;
        PATTERN_GROOVE groove;
        uint8_t lstep = DEFAULT_LAST_STEP;
        uint8_t accent = DEFAULT_GLOBAL_ACCENT;
        float tempo;
        bool initialized = false;
    } PATTERN_SETTINGS;

    // sequencer state

    typedef struct
    {
        bool muted = false;
        bool soloing = false;
    } TRACK_PERFORM_STATE;

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

    typedef struct {
        XRSound::SOUND_TYPE soundType;
        int8_t pattern = -1;
        int8_t layer = -1;
        int8_t step = -1;
        uint8_t lastState = 0; // 0 = not triggered, 1 = triggered
        uint8_t currentState = 0; // 0 = trigger reset, 1 = trigger init, 2 = trigger on
    } TRACK_TRIGGER;

    typedef struct {
        TRACK_TRIGGER trackTriggers[MAXIMUM_SEQUENCER_TRACKS];
    } TRACK_TRIGGER_STATE;

    typedef struct
    {
        SEQUENCER_PLAYBACK_STATE playbackState = STOPPED;
        TRACK_CURRENT_STEP_STATE currentTrackSteps[MAXIMUM_SEQUENCER_TRACKS];
        int8_t currentStep = 1;
        int8_t currentBar = 1;
    } SEQUENCER_STATE;

    typedef struct {
        bool firstBar = true;
        int8_t currentStep = 1;
    } RATCHET_STATE;

    // noteOn / noteOff stacks

    typedef struct
    {
        int8_t trackNum = -1;
        int8_t stepNum = -1;
        int8_t length = -1; // need -1 to know if active/not active
    } STACK_STEP_DATA;

    typedef struct
    {
        int8_t trackNum = -1;
        int8_t length = -1;
    } STACK_RATCHET_DATA;

    // recording

    typedef struct
    {
        STEP_STATE state = STATE_OFF;
        int16_t length = -1;
        int16_t note = -1;
        bool queued = false;
    } RECORDING_STEP;

    typedef struct
    {
        RECORDING_STEP steps[MAXIMUM_SEQUENCER_STEPS];
    } RECORDING_TRACK;
    
    typedef struct
    {
        RECORDING_TRACK tracks[MAXIMUM_SEQUENCER_TRACKS];
    } RECORDING_STATE;

    enum FILL_TYPE : uint8_t
    {
        MANUAL = 0,
        AUTO = 1
    };

    typedef struct
    {
        int8_t currentStep = 1;
        int8_t currentMeasure = 1; // measure = 16 steps, bar = last step of track
        int8_t fillMeasure = 4; // by default, the 4th measure is used for fill
        FILL_TYPE fillType = FILL_TYPE::MANUAL;
    } FILL_STATE;

    typedef struct
    {
        int8_t layers[MAXIMUM_SEQUENCER_TRACK_LAYERS];
    } LAYER_CHAIN;

    typedef struct
    {
        int8_t chain[MAXIMUM_SEQUENCER_TRACK_LAYERS];
        int8_t currentStep = 1;
        int8_t nextLayerIndex = 1; // always default to 2nd layer as next
        int8_t currLayerIndex = 0;
        bool enabled = false;
    } LAYER_CHAIN_STATE;

    // extern globals

    // active = current (used for playback), idle = next (read from SD card during pattern change)
    extern EXTMEM TRACK_LAYER activeTrackLayer;
    extern EXTMEM TRACK_LAYER idleTrackLayer;
    extern EXTMEM RATCHET_LAYER activeRatchetLayer;
    extern EXTMEM RATCHET_LAYER idleRatchetLayer;
    extern EXTMEM PATTERN_SETTINGS activePatternSettings;
    extern EXTMEM PATTERN_SETTINGS idlePatternSettings;

    // used for saving to SD card
    extern EXTMEM TRACK_LAYER trackLayerForWrite;
    extern EXTMEM RATCHET_LAYER ratchetLayerForWrite;
    extern EXTMEM PATTERN_SETTINGS patternSettingsForWrite;

    extern DMAMEM PATTERN_FX_PAGE_INDEXES patternFxPages[MAXIMUM_PATTERN_FX_PARAM_PAGES];
    extern DMAMEM TRACK_PERFORM_STATE trackPerformState[MAXIMUM_SEQUENCER_TRACKS];
    extern DMAMEM RECORDING_STATE recordingState;
    extern TRACK_TRIGGER_STATE trackTriggerState;
    extern FILL_STATE fillState;

    extern LAYER_CHAIN_STATE layerChainState;

    extern bool fillLayerTriggerd;
    extern bool returnToBaseLayer;

    extern bool metronomeEnabled;
    extern bool ratchetLatched;

    extern bool patternSettingsDirty;
    extern bool ratchetLayerDirty;
    extern bool trackLayerDirty;

    bool init();

    void initPatternSettings(PATTERN_SETTINGS &patternSettings);
    void initIdlePatternSettings();
    void initTrackLayer(TRACK_LAYER &trackLayer);
    void initIdleTrackLayer();
    void initRatchetLayer(RATCHET_LAYER &ratchetLayer);
    void initIdleRatchetLayer();

    bool isStepProbablyEnabled(int track, int step);
    bool isRatchetAccented();
    bool onRatchetStepPage();
    
    void swapSequencerDataForPatternChange(int newBank, int newPattern);
    void swapSequencerMemoryForTrackLayerChange();
    void saveCurrentPatternOffHeap();
    void toggleSelectedStep(uint8_t step);
    void toggleSelectedFillLayer(uint8_t layer);

    void onClockStart();
    void onClockStop();
    void ClockOutTracked16PPQN(uint8_t track, uint32_t tick);
    void ClockOutRigidTracked16PPQN(uint8_t t, uint32_t tick);
    void ClockOut16PPQN(uint32_t tick);
    void ClockOutRigid16PPQN(uint32_t tick);
    void ClockOut96PPQN(uint32_t tick);

    void handle16PPQN(uint32_t tick);
    void handle96PPQN(uint32_t tick);

    void startRecording();
    void stopRecording();
    void applyRecordingToSequencer(uint8_t t, bool lastStep);
    void applyRecordingToSequencerWhenStopped();

    void triggerAllStepsForTrack(uint8_t t, uint32_t tick);
    void triggerAllStepsForAllTracks(uint32_t tick);
    void triggerRatchetingTrack(uint32_t tick);
    void handleAddToStepStack(uint32_t tick, int track, int step);
    void handleAddToRatchetStack(int step);
    void handleRemoveFromStepStack(uint32_t tick);
    void handleRemoveFromRatchetStack();

    void handleAddToIgnoredStepStack(uint32_t tick, int track, int step);
    void handleRemoveFromIgnoredStepStack(uint32_t tick);
    bool isTrackStepBeingIgnored(int track, int step);

    void handleTriggerStates();

    void handleNoteOnForTrackStep(int track, int step, TRACK_TRIGGER trigger);
    void handleNoteOffForTrackStep(int track, int step);

    void handleNoteOnForTrack(int track, TRACK_TRIGGER trigger);
    void handleNoteOffForTrack(int track);
    
    void handleCurrentTrackStepLEDs(uint8_t t);
    void updateTrackStepState(uint8_t t, uint32_t tick);
    void updateCurrentPatternStepState();
    void updateFillStepState();
    void updateChainStepState();
    void displayAllTrackNoteOnLEDs(bool enable);
    void noteOffForAllSounds();
    void handlePatternQueueActions();
    void handleTrackLayerQueueActions();
    void queuePattern(int pattern, int bank);
    void queueTrackLayer(int layer);

    void setSelectedPattern(int8_t pattern);
    void setSelectedTrack(int8_t track);
    void setSelectedTrackLayer(int8_t layer);
    void setSelectedPage(int8_t page);
    void initializeCurrentSelectedTrack();
    void setCurrentSelectedStep(int step);
    void setCurrentSelectedRatchetStep(int step);
    void setCurrentStepPage(int8_t page);
    void setRatchetTrack(int track);
    void setRatchetDivision(int track);
    void setCurrentRatchetPageNum(int8_t page);
    void setCurrentFillChainPageNum(int8_t page);
    void handleRatchetTrackStepLEDs(uint8_t t);
    void setCurrentRatchetStep(int8_t step);
    void resetRatchetBar();
    void toggleIsRatchetAccented(bool enable);

    void toggleSequencerPlayback(char btn);
    void rewindAllCurrentStepsForAllTracks();

    void prepareQueuedTrackLayerChange(int nextBank, int nextPattern, int nextLayer);
    void instantTrackLayerChange(int nextBank, int nextPattern, int nextLayer, bool shouldWrite = true);

    PATTERN_FX_PARAMS getInitPatternFxParams();

    SEQUENCER_STATE &getSeqState();
    QUEUED_PATTERN_STATE &getQueuedPatternState();
    int getQueuedTrackLayer();

    TRACK &getTrack(int track);
    STEP &getStep(int track, int step);

    TRACK &getCurrentSelectedTrack();
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
    int8_t getCurrentRatchetPageNum();
    int8_t getCurrentSelectedRatchetStep();
    int8_t getCurrentSelectedFillTrackLayerNum();
    int8_t getQueuedFillTrackLayerNum();
    int8_t getCurrentFillChainPageNum();
}

#endif /* XRSequencer_h */