#ifndef XRSound_h
#define XRSound_h

#include <Arduino.h>
#include <Audio.h>
#include <XRConfig.h>
#include <XRAudioConfig.h>
#include <XRDexedManager.h>
#include <string>
#include <map>

namespace XRSound
{
    /*** BEGIN SOUNDS ***/

    enum SOUND_TYPE : uint8_t
    {
        T_EMPTY = 0,
        T_MONO_SAMPLE = 1,
        T_MONO_SYNTH = 2,
        T_DEXED_SYNTH = 3,
        T_BRAIDS_SYNTH = 4,
        T_FM_DRUM = 5,
        T_MIDI = 6,
        T_CV_GATE = 7,
        T_CV_TRIG = 8,
    };

    typedef struct
    {
        SOUND_TYPE type = T_EMPTY;
        char name[MAX_SOUND_NAME_LENGTH];
        char sampleName[MAX_SAMPLE_NAME_LENGTH];
        char sampleNameB[MAX_SAMPLE_NAME_LENGTH];
        int32_t params[MAXIMUM_SOUND_PARAMS]; // divide by 100 to get real param values
        uint8_t dexedParams[MAXIMUM_DEXED_SOUND_PARAMS];
    } SOUND;
    
    typedef struct
    {
        SOUND sounds[MAXIMUM_SEQUENCER_TRACKS];
    } KIT;

    // MONO SAMPLE
    enum MONO_SAMPLE_PARAM : uint8_t {
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
        MSMP_CHOKE,
        MSMP_DELAY,
    };

    // MONO SYNTH
    enum MONO_SYNTH_PARAM  : uint8_t {
        MSYN_WAVE = 0,
        MSYN_DETUNE,
        MSYN_FINE,
        MSYN_OSCA_LEVEL,
        MSYN_OSCB_LEVEL,
        MSYN_WIDTH,
        MSYN_NOISE,
        MSYN_CUTOFF,
        MSYN_RESONANCE,
        MSYN_NA_PARAM_A,
        MSYN_FILTER_ATTACK,
        MSYN_FILTER_DECAY,
        MSYN_FILTER_SUSTAIN,
        MSYN_FILTER_RELEASE,
        MSYN_FILTER_ENV_AMT,
        MSYN_AMP_ATTACK,
        MSYN_AMP_DECAY,
        MSYN_AMP_SUSTAIN,
        MSYN_AMP_RELEASE,
        MSYN_NA_PARAM_B,
        MSYN_LEVEL,
        MSYN_PAN,
        MSYN_CHOKE,
        MSYN_DELAY,
    };

    // DEXED SYNTH
    enum DEXED_SYNTH_PARAM  : uint8_t{
        DEXE_TRANSPOSE = 0,
        DEXE_ALGO = 1,
        DEXE_LEVEL = 15,
        DEXE_PAN = 16,
        DEXE_CHOKE = 17,
        DEXE_DELAY = 18,
        DEXE_NOTE_B = 20,
        DEXE_NOTE_C = 21,
        DEXE_NOTE_D = 22,
        DEXE_NOTE_MODE = 23,
    };

    // BRAIDS SYNTH
    enum BRAIDS_SYNTH_PARAM  : uint8_t {
        BRAIDS_TIMBRE = 0,
        BRAIDS_COLOR = 1,
        BRAIDS_MOD = 2,
        BRAIDS_FM = 3,
        BRAIDS_LEVEL = 15,
        BRAIDS_PAN = 16,
        BRAIDS_CHOKE = 17,
        BRAIDS_DELAY = 18,
    };

    // FM DRUM
    enum FM_DRUM_PARAM  : uint8_t {
        FMD_FREQ = 0,
        FMD_FM = 1,
        FMD_DECAY = 2,
        FMD_NOISE = 3,
        FMD_OVERDRIVE = 4,
        FMD_LEVEL = 15,
        FMD_PAN = 16,
        FMD_CHOKE = 17,
        FMD_DELAY = 18,
    };

    /*** END SOUNDS ***/

    /*** VOICES/INSTANCES */

    class MonoSampleInstance
    {
    public:
        AudioPlaySdResmp &sample;
        AudioEffectEnvelope &ampEnv;
        AudioAmplifier &ampAccent;
        AudioAmplifier &amp;
        AudioAmplifier &ampDelaySend;
        AudioAmplifier &left;
        AudioAmplifier &right;

        MonoSampleInstance(
            AudioPlaySdResmp &sample,
            AudioEffectEnvelope &ampEnv,
            AudioAmplifier &ampAccent,
            AudioAmplifier &amp,
            AudioAmplifier &ampDelaySend,
            AudioAmplifier &left,
            AudioAmplifier &right
        ) : sample{sample},
            ampEnv{ampEnv},
            ampAccent{ampAccent},
            amp{amp},
            ampDelaySend{ampDelaySend},
            left{left},
            right{right}
        {
            //
        }
    };

    class MonoSynthInstance
    {
    public:
        AudioSynthWaveform &oscA;
        AudioSynthWaveform &oscB;
        AudioSynthNoiseWhite &noise;
        AudioMixer4 &mix;
        AudioSynthWaveformDc &dc;
        AudioEffectEnvelope &filterEnv;
        AudioAmplifier &filterAccent;
        AudioFilterLadder &filter;
        AudioEffectEnvelope &ampEnv;
        AudioAmplifier &ampAccent;
        AudioAmplifier &amp;
        AudioAmplifier &ampDelaySend;
        AudioAmplifier &left;
        AudioAmplifier &right;

        MonoSynthInstance(
            AudioSynthWaveform &oscA,
            AudioSynthWaveform &oscB,
            AudioSynthNoiseWhite &noise,
            AudioMixer4 &mix,
            AudioSynthWaveformDc &dc,
            AudioEffectEnvelope &filterEnv,
            AudioAmplifier &filterAccent,
            AudioFilterLadder &filter,
            AudioEffectEnvelope &ampEnv,
            AudioAmplifier &ampAccent,
            AudioAmplifier &amp,
            AudioAmplifier &ampDelaySend,
            AudioAmplifier &left,
            AudioAmplifier &right
        ) : oscA{oscA},
            oscB{oscB},
            noise{noise},
            mix{mix},
            dc{dc},
            filterEnv{filterEnv},
            filterAccent{filterAccent},
            filter{filter},
            ampEnv{ampEnv},
            ampAccent{ampAccent},
            amp{amp},
            ampDelaySend{ampDelaySend},
            left{left},
            right{right}
        {
            //
        }
    };

    class DexedInstance
    {
    public:
        Dexed &dexed;
        AudioAmplifier &ampAccent;
        AudioAmplifier &amp;
        AudioAmplifier &ampDelaySend;
        AudioAmplifier &left;
        AudioAmplifier &right;

        DexedInstance(
            Dexed &dexed,
            AudioAmplifier &ampAccent,
            AudioAmplifier &amp,
            AudioAmplifier &ampDelaySend,
            AudioAmplifier &left,
            AudioAmplifier &right
        ) : dexed{dexed},
            ampAccent{ampAccent},
            amp{amp},
            ampDelaySend{ampDelaySend},
            left{left},
            right{right}
        {
            //
        }
    };

    class FmDrumInstance
    {
    public:
        AudioSynthFMDrum &fmDrum;
        AudioAmplifier &ampAccent;
        AudioAmplifier &amp;
        AudioAmplifier &ampDelaySend;
        AudioAmplifier &left;
        AudioAmplifier &right;

        FmDrumInstance(
            AudioSynthFMDrum &fmDrum,
            AudioAmplifier &ampAccent,
            AudioAmplifier &amp,
            AudioAmplifier &ampDelaySend,
            AudioAmplifier &left,
            AudioAmplifier &right
        ) : fmDrum{fmDrum},
            ampAccent{ampAccent},
            amp{amp},
            ampDelaySend{ampDelaySend},
            left{left},
            right{right}
        {
            //
        }
    };

    // class BraidsInstance
    // {
    // public:
    //     AudioSynthBraids &braids;
    //     AudioAmplifier &amp;
    //     AudioAmplifier &ampDelaySend;
    //     AudioAmplifier &left;
    //     AudioAmplifier &right;

    //     BraidsInstance(
    //         AudioSynthBraids &braids,
    //         AudioAmplifier &amp,
    //         AudioAmplifier &ampDelaySend,
    //         AudioAmplifier &left,
    //         AudioAmplifier &right
    //     ) : braids{braids},
    //         amp{amp},
    //         ampDelaySend{ampDelaySend},
    //         left{left},
    //         right{right}
    //     {
    //         //
    //     }
    // };

    class StereoDelayInstance
    {
    public:
        AudioEffectDelay &delayEffect;
        AudioMixer4 &feedbackMix;
        AudioAmplifier &left;
        AudioAmplifier &right;

        StereoDelayInstance(
            AudioEffectDelay &delayEffect,
            AudioMixer4 &feedbackMix,
            AudioAmplifier &left,
            AudioAmplifier &right
        ) : delayEffect{delayEffect},
            feedbackMix{feedbackMix},
            left{left},
            right{right}
        {
            //
        }
    };

    /*** END VOICES/INSTANCES */

    enum WAVEFORM_TYPE : uint8_t
    {
        SAW = WAVEFORM_SAWTOOTH,
        RSAW = WAVEFORM_SAWTOOTH_REVERSE,
        TRI = WAVEFORM_TRIANGLE,
        SQUARE = WAVEFORM_SQUARE,
        PULSE = WAVEFORM_PULSE,
        SINE = WAVEFORM_SINE,
    };

    enum SOUND_CONTROL_MOD_TYPE  : uint8_t
    {
        DEFAULT = 0,
        RANGE,
        BAR,
        DIAL,
    };

    typedef struct
    {
        bool isAbleToStepModA;
        bool isAbleToStepModB;
        bool isAbleToStepModC;
        bool isAbleToStepModD;
        bool isActiveStepModA;
        bool isActiveStepModB;
        bool isActiveStepModC;
        bool isActiveStepModD;
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

    extern EXTMEM KIT activeKit;
    extern EXTMEM KIT idleKit;

    // new data structure testing

    extern MonoSampleInstance monoSampleInstances[MAXIMUM_MONO_SAMPLE_SOUNDS];
    extern MonoSynthInstance monoSynthInstances[MAXIMUM_MONO_SYNTH_SOUNDS];
    extern DexedInstance dexedInstances[MAXIMUM_DEXED_SYNTH_SOUNDS];
    // extern BraidsInstance braidsInstances[MAXIMUM_BRAIDS_SYNTH_SOUNDS];
    extern FmDrumInstance fmDrumInstances[MAXIMUM_FM_DRUM_SOUNDS];
    extern StereoDelayInstance delayInstances[MAXIMUM_DELAY_INSTANCES];

    extern std::string patternPageNames[MAXIMUM_PATTERN_PAGES];
    extern std::map<SOUND_TYPE, int8_t> soundTypeInstanceLimitMap;
    extern std::map<int, loop_type> loopTypeSelMap;
    extern std::map<loop_type, int> loopTypeFindMap;
    extern std::map<int, play_start> playStartSelMap;
    extern std::map<play_start, int> playStartFindMap;
    extern std::map<SOUND_TYPE, int32_t*> soundTypeInitParams;
    extern std::map<SOUND_TYPE, int> soundPageNumMap;
    extern std::map<SOUND_TYPE, std::map<int, std::string>> soundCurrPageNameMap;

    extern int chokeSourceDest[MAXIMUM_SEQUENCER_TRACKS];
    extern int chokeDestSource[MAXIMUM_SEQUENCER_TRACKS];
    extern bool chokeSourcesEnabled[MAXIMUM_SEQUENCER_TRACKS];
    extern bool soundNeedsReinit[MAXIMUM_SEQUENCER_TRACKS];
    extern bool patternSoundsDirty;
    extern bool patternSoundStepModsDirty;

    SOUND_CONTROL_MODS getControlModDataForPattern();
    SOUND_CONTROL_MODS getControlModDataForTrack();
    SOUND_CONTROL_MODS getMonoSynthControlModData();
    SOUND_CONTROL_MODS getMonoSampleControlModData();
    SOUND_CONTROL_MODS getDexedSynthControlModData();
    SOUND_CONTROL_MODS getBraidsControlModData();
    SOUND_CONTROL_MODS getFmDrumControlModData();
    SOUND_CONTROL_MODS getMidiControlModData();
    SOUND_CONTROL_MODS getCvGateControlModData();
    SOUND_CONTROL_MODS getCvTrigControlModData();
    SOUND_CONTROL_MODS getEmptyControlModData();
    SOUND_CONTROL_MODS getRatchetControlModData();

    PANNED_AMOUNTS getStereoPanValues(float pan);

    int getWaveformNumber(uint8_t waveformType);
    int getWaveformTypeSelection(uint8_t waveformNumber);

    float getOscFreqA(uint8_t note, int8_t fine);
    float getDetunedOscFreqB(uint8_t note, float detuneAmount);

    std::string getSoundMetaStr(XRSound::SOUND_TYPE type);
    std::string getSoundTypeNameStr(XRSound::SOUND_TYPE type);
    std::string getPageNameForCurrentTrack();
    std::string getWaveformName(uint8_t waveform);
    std::string getPlaybackSpeedStr(float rate);
    std::string getLoopTypeName();

    uint8_t getPageCountForCurrentTrack();
    uint8_t getPageCountForTrack(int track);

    void init();
    void initKit(KIT &kit);
    void initVoices();

    void setSoundNeedsReinit(int sound, bool reinit);
    void reinitSoundForTrack(int track);
    void applyActiveSounds();
    void applyFxForActivePattern();
    void applyTrackChokes();
    void loadNextDexedInstances();
    void swapSoundDataForPatternChange(int nextBank, int nextPattern);
    void assignSampleToTrackSound();
    void changeTrackSoundType(uint8_t t, SOUND_TYPE newType);
    void initTrackSound(int8_t t);
    void muteAllOutput();
    void turnOffAllSounds();
    void handleMonoSampleNoteOnForTrack(int track);
    void handleMonoSynthNoteOnForTrack(int track);
    void handleDexedSynthNoteOnForTrack(int track);
    void handleBraidsNoteOnForTrack(int track);
    void handleFmDrumNoteOnForTrack(int track);
    void handleMIDINoteOnForTrack(int track);
    void handleCvGateNoteOnForTrack(int track);
    void handleNoteOffForTrack(int track);
    void handleNoteOffForTrackStep(int track, int step);
    void noteOffTrackManually(int noteOnKeyboard, uint8_t octave);
    void handleMonoSampleNoteOnForTrackStep(int track, int step);
    void handleMonoSynthNoteOnForTrackStep(int track, int step);
    void handleDexedSynthNoteOnForTrackStep(int track, int step);
    void handleBraidsNoteOnForTrackStep(int track, int step);
    void handleFmDrumNoteOnForTrackStep(int track, int step);
    void handleMIDINoteOnForTrackStep(int track, int step);
    void handleCvGateNoteOnForTrackStep(int track, int step);
    void triggerTrackManually(uint8_t t, uint8_t note, uint8_t octave, bool accented);
    void triggerMonoSampleNoteOn(uint8_t t, uint8_t note, uint8_t octave, bool accented);
    void triggerMonoSynthNoteOn(uint8_t t, uint8_t note, uint8_t octave, bool accented);
    void triggerDexedSynthNoteOn(uint8_t t, uint8_t note, uint8_t octave, bool accented);
    void triggerBraidsNoteOn(uint8_t t, uint8_t note, uint8_t octave, bool accented);
    void triggerFmDrumNoteOn(uint8_t t, uint8_t note, uint8_t octave, bool accented);
    void triggerCvGateNoteOn(uint8_t t, uint8_t note, uint8_t octave, bool accented);
    int getChokeSourcesEnabledCount();
    bool isTrackAnActiveChokeDestination(int track);
    void applyChokeForSourceTrack(int track);
    bool applyChokeForDestinationTrackStep(int track, int step);
    void applyCurrentDexedPatchToSound();
    void handleNoteOnForMetronome(bool accented);

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