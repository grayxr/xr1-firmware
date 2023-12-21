#ifndef XRSound_h
#define XRSound_h

#include <Arduino.h>
#include <Audio.h>
#include <XRConfig.h>
#include <XRAudioConfig.h>
#include <string>
#include <map>

namespace XRSound
{
    /*** BEGIN SOUNDS ***/

    enum SOUND_TYPE
    {
        T_EMPTY = 0,
        T_MONO_SAMPLE,
        T_MONO_SYNTH,
        T_DEXED_SYNTH,
        T_BRAIDS_SYNTH,
        T_FM_DRUM,
        T_MIDI,
        T_CV_GATE,
        T_CV_TRIG,
    };

    typedef struct
    {
        SOUND_TYPE type = T_EMPTY;
        char name[MAX_SOUND_NAME_LENGTH];
        char sampleName[MAX_SAMPLE_NAME_LENGTH];
        int32_t params[MAXIMUM_SOUND_PARAMS]; // divide by 100 to get real param values
        uint8_t dexedParams[MAXIMUM_DEXED_SOUND_PARAMS];
    } SOUND;

    // MONO SAMPLE
    enum MONO_SAMPLE_PARAM {
        MSMP_SAMPLEPLAYRATE = 0,
        MSMP_LOOPTYPE,
        MSMP_LOOPSTART,
        MSMP_LOOPFINISH,
        MSMP_CHROMATIC,
        MSMP_PLAYSTART,
        MSMP_NA_PARAM_A,
        MSMP_NA_PARAM_B,
        MSMP_NA_PARAM_C,
        MSMP_NA_PARAM_D,
        MSMP_AMP_ATTACK,
        MSMP_AMP_DECAY,
        MSMP_AMP_SUSTAIN,
        MSMP_AMP_RELEASE,
        MSMP_NA_PARAM_E,
        MSMP_LEVEL,
        MSMP_PAN,
    };

    // MONO SYNTH
    enum MONO_SYNTH_PARAM {
        MSYN_WAVE = 0,
        MSYN_DETUNE = 1,
        MSYN_FINE = 2,
        MSYN_OSCA_LEVEL = 3,
        MSYN_OSCB_LEVEL = 4,
        MSYN_WIDTH = 5,
        MSYN_NOISE = 6,
        MSYN_CUTOFF = 7,
        MSYN_RESONANCE = 8,
        MSYN_FILTER_ATTACK = 10,
        MSYN_FILTER_DECAY = 11,
        MSYN_FILTER_SUSTAIN = 12,
        MSYN_FILTER_RELEASE = 13,
        MSYN_FILTER_ENV_AMT = 14,
        MSYN_AMP_ATTACK = 15,
        MSYN_AMP_DECAY = 16,
        MSYN_AMP_SUSTAIN = 17,
        MSYN_AMP_RELEASE = 18,
        MSYN_LEVEL = 20,
        MSYN_PAN = 21,
    };

    // DEXED SYNTH
    enum DEXED_SYNTH_PARAM {
        DEXE_ALGO = 0,
        DEXE_LEVEL = 15,
        DEXE_PAN = 16,
    };

    // FM DRUM
    enum FM_DRUM_PARAM {
        FMD_FREQ = 0,
        FMD_FM = 1,
        FMD_DECAY = 2,
        FMD_NOISE = 3,
        FMD_OVERDRIVE = 4,
        FMD_LEVEL = 15,
        FMD_PAN = 16,
    };

    /*** END SOUNDS ***/

    /*** BEGIN SOUND MODS ***/

    /**
     * All sound step mods for all other banks and patterns are stored in SD card,
     * they are fetched when changing patterns, stored in RAM2 / DMAMEM,
     * and then they replace the step mods in RAM1 / heap once the pattern change occurs
    */

    typedef struct
    {
        int32_t mods[MAXIMUM_SOUND_PARAMS]; // divide by 100 to get real param mod values
        bool flags[MAXIMUM_SOUND_PARAMS]; // whether the param mod should apply or not
    } SOUND_STEP_MODS;

    typedef struct
    {
        SOUND_STEP_MODS steps[MAXIMUM_SEQUENCER_STEPS];
    } SOUND_MODS;
    
    typedef struct
    {
        SOUND_MODS sounds[MAXIMUM_SEQUENCER_TRACKS];
    } PATTERN_SOUND_MODS;

    /*** END SOUND MODS ***/

    class ComboVoice
    {
    public:
        AudioSynthFMDrum &fmdrum;
        AudioSynthDexed &dexed;
        AudioPlayArrayResmp &sample;
        AudioSynthWaveform &osca;
        AudioSynthWaveform &oscb;
        AudioSynthNoiseWhite &noise;
        AudioMixer4 &oscMix;
        AudioSynthWaveformDc &dc;
        AudioFilterLadder &lfilter;
        AudioEffectEnvelope &filterEnv;
        AudioMixer4 &mix;
        AudioEffectEnvelope &ampEnv;
        AudioAmplifier &leftCtrl;
        AudioAmplifier &rightCtrl;
        AudioAmplifier &dexedLeftCtrl;
        AudioAmplifier &dexedRightCtrl;
        AudioAmplifier &fmDrumLeftCtrl;
        AudioAmplifier &fmDrumRightCtrl;
        AudioMixer4 &leftSubMix;
        AudioMixer4 &rightSubMix;

        ComboVoice(
            AudioSynthFMDrum &fmdrum,
            AudioSynthDexed &dexed,
            AudioPlayArrayResmp &sample,
            AudioSynthWaveform &osca,
            AudioSynthWaveform &oscb,
            AudioSynthNoiseWhite &noise,
            AudioMixer4 &oscMix,
            AudioSynthWaveformDc &dc,
            AudioFilterLadder &lfilter,
            AudioEffectEnvelope &filterEnv,
            AudioMixer4 &mix,
            AudioEffectEnvelope &ampEnv,
            AudioAmplifier &leftCtrl,
            AudioAmplifier &rightCtrl,
            AudioAmplifier &dexedLeftCtrl,
            AudioAmplifier &dexedRightCtrl,
            AudioAmplifier &fmDrumLeftCtrl,
            AudioAmplifier &fmDrumRightCtrl,
            AudioMixer4 &leftSubMix,
            AudioMixer4 &rightSubMix) : fmdrum{fmdrum},
                                        dexed{dexed},
                                        sample{sample},
                                        osca{osca},
                                        oscb{oscb},
                                        noise{noise},
                                        oscMix{oscMix},
                                        dc{dc},
                                        lfilter{lfilter},
                                        filterEnv{filterEnv},
                                        mix{mix},
                                        ampEnv{ampEnv},
                                        leftCtrl{leftCtrl},
                                        rightCtrl{rightCtrl},
                                        dexedLeftCtrl{dexedLeftCtrl},
                                        dexedRightCtrl{dexedRightCtrl},
                                        fmDrumLeftCtrl{fmDrumLeftCtrl},
                                        fmDrumRightCtrl{fmDrumRightCtrl},
                                        leftSubMix{leftSubMix},
                                        rightSubMix{rightSubMix}
        {
            //
        }
    };

    class SampleVoice
    {
    public:
        AudioPlayArrayResmp &sample;
        AudioEffectEnvelope &ampEnv;
        AudioAmplifier &leftCtrl;
        AudioAmplifier &rightCtrl;
        AudioMixer4 &leftSubMix;
        AudioMixer4 &rightSubMix;

        SampleVoice(
            AudioPlayArrayResmp &sample,
            AudioEffectEnvelope &ampEnv,
            AudioAmplifier &leftCtrl,
            AudioAmplifier &rightCtrl,
            AudioMixer4 &leftSubMix,
            AudioMixer4 &rightSubMix) : sample{sample},
                                        ampEnv{ampEnv},
                                        leftCtrl{leftCtrl},
                                        rightCtrl{rightCtrl},
                                        leftSubMix{leftSubMix},
                                        rightSubMix{rightSubMix}
        {
            //
        }
    };

    enum WAVEFORM_TYPE
    {
        SAW = WAVEFORM_SAWTOOTH,
        RSAW = WAVEFORM_SAWTOOTH_REVERSE,
        TRI = WAVEFORM_TRIANGLE,
        SQUARE = WAVEFORM_SQUARE,
        PULSE = WAVEFORM_PULSE,
        SINE = WAVEFORM_SINE,
    };

    enum SOUND_CONTROL_MOD_TYPE
    {
        DEFAULT = 0,
        RANGE,
        BAR,
        DIAL,
    };

    typedef struct
    {
        std::string aName;
        std::string bName;
        std::string cName;
        std::string dName;
        std::string aValue;
        std::string bValue;
        std::string cValue;
        std::string dValue;
        float aFloatValue;
        float bFloatValue;
        float cFloatValue;
        float dFloatValue;
        SOUND_CONTROL_MOD_TYPE aType;
        SOUND_CONTROL_MOD_TYPE bType;
        SOUND_CONTROL_MOD_TYPE cType;
        SOUND_CONTROL_MOD_TYPE dType;
    } SOUND_CONTROL_MODS;

    typedef struct
    {
        float left;
        float right;
    } PANNED_AMOUNTS;

    // extern globals
    extern SOUND currentPatternSounds[MAXIMUM_SEQUENCER_TRACKS];
    extern DMAMEM SOUND nextPatternSounds[MAXIMUM_SEQUENCER_TRACKS];
    extern DMAMEM PATTERN_SOUND_MODS patternSoundStepMods;
    
    extern bool soundNeedsReinit[MAXIMUM_SEQUENCER_TRACKS];

    extern ComboVoice comboVoices[COMBO_VOICE_COUNT];
    extern SampleVoice sampleVoices[SAMPLE_VOICE_COUNT];
    
    extern std::map<int, loop_type> loopTypeSelMap;
    extern std::map<loop_type, int> loopTypeFindMap;
    extern std::map<int, play_start> playStartSelMap;
    extern std::map<play_start, int> playStartFindMap;
    extern std::map<SOUND_TYPE, int32_t*> soundTypeInitParams;
    extern std::map<SOUND_TYPE, int> soundPageNumMap;
    extern std::map<SOUND_TYPE, std::map<int, std::string>> soundCurrPageNameMap;

    SOUND_CONTROL_MODS getControlModDataForPattern();
    SOUND_CONTROL_MODS getControlModDataForTrack();
    SOUND_CONTROL_MODS getMonoSynthControlModData();
    SOUND_CONTROL_MODS getMonoSampleControlModData();
    SOUND_CONTROL_MODS getDexedSynthControlModData();
    SOUND_CONTROL_MODS getFmDrumControlModData();
    SOUND_CONTROL_MODS getMidiControlModData();
    SOUND_CONTROL_MODS getCvGateControlModData();
    SOUND_CONTROL_MODS getCvTrigControlModData();
    SOUND_CONTROL_MODS getEmptyControlModData();

    PANNED_AMOUNTS getStereoPanValues(float pan);

    ComboVoice &getComboVoiceForCurrentTrack();
    ComboVoice &getComboVoiceForTrack(int t);
    SampleVoice &getSampleVoiceForTrack(int t);

    int getWaveformNumber(uint8_t waveformType);
    int getWaveformTypeSelection(uint8_t waveformNumber);
    float getOscFreqA(uint8_t note, int8_t fine);
    float getDetunedOscFreqB(uint8_t note, float detuneAmount);

    std::string getSoundMetaStr(XRSound::SOUND_TYPE type);
    std::string getSoundTypeNameStr(XRSound::SOUND_TYPE type);
    std::string getPageNameForCurrentTrack();
    uint8_t getPageCountForCurrentTrack();
    uint8_t getPageCountForTrack(int track);

    std::string getWaveformName(uint8_t waveform);
    std::string getPlaybackSpeedStr(float rate);
    std::string getLoopTypeName();

    void init();
    void initNextPatternSounds();
    void initPatternSoundStepMods();
    void initVoices();
    void setSoundNeedsReinit(int sound, bool reinit);
    void reinitSoundForTrack(int track);
    
    void manageSoundDataForPatternChange(int nextBank, int nextPattern);

    void assignSampleToTrackSound();

    void initComboVoiceForTrack(int t);
    void setComboVoiceMixSettingsForTrack(int8_t t);

    void changeTrackSoundType(uint8_t t, SOUND_TYPE newType);
    void changeComboTrackSoundType(uint8_t t, SOUND_TYPE newType);
    void initTrackSound(int8_t t);

    void handleMonoSampleNoteOnForTrack(int track);
    void handleMonoSynthNoteOnForTrack(int track);
    void handleDexedSynthNoteOnForTrack(int track);
    void handleFmDrumNoteOnForTrack(int track);
    void handleMIDINoteOnForTrack(int track);
    void handleCvGateNoteOnForTrack(int track);

    void handleNoteOffForTrack(int track);
    void handleNoteOffForTrackStep(int track, int step);
    void noteOffTrackManually(int noteOnKeyboard);

    void handleMonoSampleNoteOnForTrackStep(int track, int step);
    void handleMonoSynthNoteOnForTrackStep(int track, int step);
    void handleDexedSynthNoteOnForTrackStep(int track, int step);
    void handleFmDrumNoteOnForTrackStep(int track, int step);
    void handleMIDINoteOnForTrackStep(int track, int step);
    void handleCvGateNoteOnForTrackStep(int track, int step);

    void triggerTrackManually(uint8_t t, uint8_t note);
    void triggerMonoSampleNoteOn(uint8_t t, uint8_t note);
    void triggerMonoSynthNoteOn(uint8_t t, uint8_t note);
    void triggerDexedSynthNoteOn(uint8_t t, uint8_t note);
    void triggerFmDrumNoteOn(uint8_t t, uint8_t note);
    void triggerCvGateNoteOn(uint8_t t, uint8_t note);

    void applyCurrentDexedPatchToSound();

    int8_t getValueNormalizedAsInt8(int32_t param);
    uint8_t getValueNormalizedAsUInt8(int32_t param);
    int32_t getValueNormalizedAsInt32(int32_t param);
    uint32_t getValueNormalizedAsUInt32(int32_t param);
    float getValueNormalizedAsFloat(int32_t param);
    bool getValueNormalizedAsBool(int32_t param);

    int32_t getInt32ValuePaddedAsInt32(int32_t value);
    int32_t getUInt32ValuePaddedAsInt32(uint32_t value);
    int32_t getFloatValuePaddedAsInt32(float value);
    int32_t getBoolValuePaddedAsInt32(bool value);
}

#endif /* XRSound_h */