#ifndef XRSound_h
#define XRSound_h

#include <Arduino.h>
#include <Audio.h>
#include <XRConfig.h>
#include <XRAudioConfig.h>
#include <string>

namespace XRSound
{
    class ComboVoice
    {
    public:
        AudioSynthDexed &dexed;
        AudioPlayArrayResmp &rSample;
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
        AudioMixer4 &leftSubMix;
        AudioMixer4 &rightSubMix;

        ComboVoice(
            AudioSynthDexed &dexed,
            AudioPlayArrayResmp &rSample,
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
            AudioMixer4 &leftSubMix,
            AudioMixer4 &rightSubMix) : dexed{dexed},
                                        rSample{rSample},
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
                                        leftSubMix{leftSubMix},
                                        rightSubMix{rightSubMix}
        {
            //
        }
    };

    class SampleVoice
    {
    public:
        AudioPlayArrayResmp &rSample;
        AudioEffectEnvelope &ampEnv;
        AudioAmplifier &leftCtrl;
        AudioAmplifier &rightCtrl;
        AudioMixer4 &leftSubMix;
        AudioMixer4 &rightSubMix;

        SampleVoice(
            AudioPlayArrayResmp &rSample,
            AudioEffectEnvelope &ampEnv,
            AudioAmplifier &leftCtrl,
            AudioAmplifier &rightCtrl,
            AudioMixer4 &leftSubMix,
            AudioMixer4 &rightSubMix) : rSample{rSample},
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

    extern ComboVoice comboVoices[COMBO_VOICE_COUNT];
    extern SampleVoice sampleVoices[SAMPLE_VOICE_COUNT];

    SOUND_CONTROL_MODS getControlModDataForPattern();
    SOUND_CONTROL_MODS getControlModDataForTrack();
    SOUND_CONTROL_MODS getSubtractiveSynthControlModData();
    SOUND_CONTROL_MODS getRawSampleControlModData();
    SOUND_CONTROL_MODS getWavSampleControlModData();
    SOUND_CONTROL_MODS getDexedControlModData();
    SOUND_CONTROL_MODS getMidiControlModData();
    SOUND_CONTROL_MODS getCvGateControlModData();
    SOUND_CONTROL_MODS getCvTrigControlModData();

    PANNED_AMOUNTS getStereoPanValues(float pan);

    std::string getWaveformName(uint8_t waveform);
    std::string getPlaybackSpeedStr(float rate);
    std::string getLoopTypeName();

    void init();
    void loadVoiceSettings();
    void initSoundsForTrack(int t);
    void initTrackSounds();
    void configureVoiceSettingsForTrack(int t);
    void configureSampleVoiceSettingsOnLoad(int t);
    void assignSampleToTrack();
    void clearSamples();

    void changeTrackSoundType(int8_t t, int8_t newType);
    void changeSampleTrackSoundType(uint8_t t, int8_t newType);

    void handleRawSampleNoteOnForTrack(int track);
    void handleWavSampleNoteOnForTrack(int track);
    void handleDexedNoteOnForTrack(int track);
    void handleSubtractiveSynthNoteOnForTrack(int track);
    void handleMIDINoteOnForTrack(int track);
    void handleCvGateNoteOnForTrack(int track);

    void handleNoteOffForTrack(int track);
    void handleNoteOffForTrackStep(int track, int step);
    void noteOffTrackManually(int noteOnKeyboard);

    void triggerTrackManually(uint8_t t, uint8_t note);
    void triggerRawSampleNoteOn(uint8_t t, uint8_t note);
    void triggerDexedNoteOn(uint8_t t, uint8_t note);
    void triggerSubtractiveSynthNoteOn(uint8_t t, uint8_t note);
    void triggerCvGateNoteOn(uint8_t t, uint8_t note);

    void handleRawSampleNoteOnForTrackStep(int track, int step);
    void handleWavSampleNoteOnForTrackStep(int track, int step);
    void handleDexedNoteOnForTrackStep(int track, int step);
    void handleSubtractiveSynthNoteOnForTrackStep(int track, int step);
    void handleMIDINoteOnForTrackStep(int track, int step);
    void handleCvGateNoteOnForTrackStep(int track, int step);
}

#endif /* XRSound_h */