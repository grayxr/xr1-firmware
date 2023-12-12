#ifndef XRSequencer_h
#define XRSequencer_h

#include <Arduino.h>
#include <XRConfig.h>
#include <XRSound.h>
#include <TeensyVariablePlayback.h>

namespace XRSequencer
{
    enum TRACK_TYPE
    {
        MIDI_OUT = 0,
        CV_TRIG = 1,
        CV_GATE = 2,
        SUBTRACTIVE_SYNTH = 3,
        RAW_SAMPLE = 4,
        WAV_SAMPLE = 5,
        DEXED = 6,
    };

    enum TRACK_STEP_STATE
    {
        STATE_OFF = 0,
        STATE_ON = 1,
        STATE_ACCENTED = 2
    };

    enum SEQUENCER_PLAYBACK_STATE
    {
        STOPPED = 0,
        RUNNING = 1,
        PAUSED = 2
    };

    typedef struct
    {
        TRACK_STEP_STATE state = TRACK_STEP_STATE::STATE_OFF;
        uint8_t note = 0;        // 0 - C
        uint8_t octave = 4;      // 4 - middle C (C4)
        uint8_t length = 4;      // 4 = 1/16
        uint8_t velocity = 50;   // 1 - 100%
        uint8_t probability = 0; // 1 = 100ms
        int8_t microtiming = 0;  // 1 = 100ms
    } TRACK_STEP;

    typedef struct
    {
        TRACK_TYPE track_type = RAW_SAMPLE;
        TRACK_STEP steps[MAXIMUM_SEQUENCER_STEPS];
        char sample_name[MAX_SAMPLE_NAME_LENGTH];
        uint8_t waveform = XRSound::WAVEFORM_TYPE::SAW;
        uint8_t last_step = DEFAULT_LAST_STEP;
        uint8_t note = 0;   // 0 - C1
        uint8_t octave = 4; // 4 - middle C (C4)
        int8_t detune = -7;
        int8_t fine = 0;
        int8_t microtiming = 0;
        uint8_t length = 4; // 1 = 1/64 step len
        uint8_t bitrate = 16;
        uint8_t velocity = 50; // 1 - 100%
        uint8_t probability = 100;
        uint8_t channel = 1;
        uint8_t looptype = 0;
        uint32_t loopstart = 0;
        uint32_t loopfinish = 3000;
        play_start playstart = play_start_sample;
        float level = 0.7; // TODO: impl real default level based on default mixer settings
        float pan = 0;     // -1.0 = panned fully left, 1.0 = panned fully right
        float sample_play_rate = 1.0;
        float width = 0.5; // pulsewidth / waveshaping
        float oscalevel = 1;
        float oscblevel = 0.5;
        float cutoff = 1600;
        float res = 0;
        float filter_attack = 0;
        float filter_decay = 1000;
        float filter_sustain = 1.0;
        float filter_release = 5000;
        float filterenvamt = 1.0;
        float amp_attack = 0;
        float amp_decay = 1000;
        float amp_sustain = 1.0;
        float amp_release = 5000;
        float noise = 0;
        bool chromatic_enabled = false;
        bool muted = false;
        bool soloing = false;
        bool initialized = false;
    } TRACK;

    typedef struct
    {
        TRACK tracks[MAXIMUM_SEQUENCER_TRACKS];
        uint8_t last_step = DEFAULT_LAST_STEP;
        uint8_t groove_amount = 0;
        int8_t groove_id = -1;
        bool initialized = false;
    } PATTERN;

    typedef struct
    {
        PATTERN patterns[MAXIMUM_SEQUENCER_PATTERNS];
    } BANK;

    typedef struct
    {
        BANK banks[MAXIMUM_SEQUENCER_BANKS];
    } SEQUENCER_EXTERNAL;

    typedef struct
    {
        PATTERN pattern;
    } SEQUENCER_HEAP;

    typedef struct
    {
        int bank = -1;
        int number = -1;
    } QUEUED_PATTERN;

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

    typedef struct
    {
        char sample_name[MAX_SAMPLE_NAME_LENGTH] = {'N','/','A'};
        uint8_t waveform = XRSound::WAVEFORM_TYPE::SAW;
        uint8_t note = 0;        // 0 - C
        uint8_t octave = 4;      // 4 - middle C (C4)
        uint8_t length = 4;      // 4 = 1/16
        uint8_t velocity = 50;   // 1 - 100%
        uint8_t probability = 0; // 1 = 100ms
        int8_t microtiming = 0;  // 1 = 100ms
        int8_t detune = 0;
        int8_t fine = 0;
        uint8_t looptype = 0;
        uint32_t loopstart = 0;
        uint32_t loopfinish = 3000;
        play_start playstart = play_start_sample;
        float level = 0.7; // TODO: impl real default level based on default mixer settings
        float pan = 0;     // -1.0 = panned fully left, 1.0 = panned fully right
        float sample_play_rate = 1.0;
        float width = 0.5; // pulsewidth / waveshaping
        float oscalevel = 1;
        float oscblevel = 0.5;
        float cutoff = 1600;
        float res = 0;
        float filter_attack = 0;
        float filter_decay = 1000;
        float filter_sustain = 1.0;
        float filter_release = 5000;
        float filterenvamt = 1.0;
        float amp_attack = 0;
        float amp_decay = 500;
        float amp_sustain = 1.0;
        float amp_release = 500;
        float noise = 0;
    } TRACK_STEP_MODS;

    enum MOD_ATTRS
    {
        SAMPLE_NAME = 0,
        WAVEFORM,
        NOTE,
        OCTAVE,
        LENGTH,
        VELOCITY,
        PROBABILITY,
        MICROTIMING,
        DETUNE,
        FINE,
        LOOPTYPE,
        LOOPSTART,
        LOOPFINISH,
        PLAYSTART,
        LEVEL,
        PAN,
        SAMPLE_PLAY_RATE,
        WIDTH,
        OSCALEVEL,
        OSCBLEVEL,
        CUTOFF,
        RES,
        F_ATTACK,
        F_SUSTAIN,
        F_DECAY,
        F_RELEASE,
        A_ATTACK,
        A_SUSTAIN,
        A_DECAY,
        A_RELEASE,
        NOISE,
    };

    typedef struct
    {
        bool flags[MAX_STEP_MOD_ATTRS];
    } TRACK_STEP_MOD_FLAGS;

    typedef struct
    {
        TRACK_STEP_MODS steps[MAXIMUM_SEQUENCER_STEPS];
        TRACK_STEP_MOD_FLAGS step_mod_flags[MAXIMUM_SEQUENCER_STEPS];
    } TRACK_MODS;

    typedef struct
    {
        TRACK_MODS tracks[MAXIMUM_SEQUENCER_TRACKS];
    } PATTERN_MODS;

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

    bool init();

    void initExternalSequencer();
    void initExternalPatternMods();
    
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
    void setTrackTypeForHeapTrack(int8_t track, TRACK_TYPE type);
    void setTrackNeedsInit(int track, bool init);

    void assignSampleNameToTrack(std::string sampleName);

    void toggleSequencerPlayback(char btn);
    void rewindAllCurrentStepsForAllTracks();

    SEQUENCER_STATE &getSeqState();

    SEQUENCER_EXTERNAL &getSequencerExternal();
    QUEUED_PATTERN &getQueuedPattern();
    SEQUENCER_HEAP &getSequencerHeap();

    TRACK &getHeapTrack(int track);
    TRACK_STEP &getHeapStep(int track, int step);
    PATTERN &getHeapCurrentSelectedPattern();
    TRACK &getHeapCurrentSelectedTrack();
    TRACK_STEP &getHeapCurrentSelectedTrackStep();

    PATTERN_MODS &getModsForCurrentPattern();
    TRACK_STEP_MODS &getModsForCurrentTrackStep();

    int8_t getCurrentSelectedBankNum();
    int8_t getCurrentSelectedPatternNum();
    int8_t getCurrentSelectedTrackNum();
    int8_t getCurrentSelectedStepNum();
    int8_t getCurrentStepPage();

    // the current page for the current track layer (default layer is "sound")
    int8_t getCurrentSelectedPage();
    uint8_t getCurrentTrackPageCount();

    std::string getTrackMetaStr(TRACK_TYPE type);
    std::string getTrackTypeNameStr(TRACK_TYPE type);
    std::string getCurrPageNameForTrack();

    int8_t getRatchetTrack();
    int8_t getRatchetDivision();
}

#endif /* XRSequencer_h */