#include <XRSound.h>
#include <XRClock.h>
#include <XRSequencer.h>
#include <XRUX.h>
#include <XRMIDI.h>
#include <XRCV.h>
#include <XRSD.h>
#include <XRKeyMatrix.h>
#include <XRHelpers.h>
#include <map>

#define USE_WAV true

namespace XRSound
{
    // extern globals
    EXTMEM KIT activeKit;
    EXTMEM KIT idleKit;

    // used for saving to SD card
    EXTMEM KIT kitForWrite;
    EXTMEM std::string kitWriteFilename;

    // private variables

    uint8_t _numChannels = 1;

    bool kitDirty = false;

    float _noteToFreqArr[13] = {
        16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.50, 25.96, 27.50, 29.14, 30.87, 32.70
    };

    int _cvLevels[128];

    std::map<int, int> _waveformFindMap = {
        {WAVEFORM_SAWTOOTH, 0},
        {WAVEFORM_SAWTOOTH_REVERSE, 1},
        {WAVEFORM_TRIANGLE, 2},
        {WAVEFORM_SQUARE, 3},
        {WAVEFORM_PULSE, 4},
        {WAVEFORM_SINE, 5},
    };

    std::map<int, int> _waveformSelMap = {
        {0, WAVEFORM_SAWTOOTH},
        {1, WAVEFORM_SAWTOOTH_REVERSE},
        {2, WAVEFORM_TRIANGLE},
        {3, WAVEFORM_SQUARE},
        {4, WAVEFORM_PULSE},
        {5, WAVEFORM_SINE},
    };

    int32_t monoSampleInitParams[MAXIMUM_SOUND_PARAMS] = {
        100,0,0,300000,0,       // sampleplayrate, looptype, loopstart, loopfinish, chromatic
        0,0,0,0,0,              // playstart, n/a, n/a, n/a, n/a
        0,100000,100,500000,0,  // a. attack, a. decay, a. sustain, a. release, n/a
        100,0,-100,0,0,         // level, pan, choke, delay send, n/a
        0,0,0,0,0,               // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0               // n/a, n/a, n/a, n/a, n/a
    };

    int32_t monoSynthInitParams[MAXIMUM_SOUND_PARAMS] = {
        100,-700,0,100,50,          // waveform, detune, fine, osc-a level, osc-b level
        50,0,300000,0,0,            // width, noise, cutoff, resonance, n/a
        0,100000,100,500000,100,    // f. attack, f. decay, f. sustain, f. release, f. env amount
        0,100000,100,500000,0,      // a. attack, a. decay, a. sustain, a. release, n/a
        100,0,-100,0,0,             // level, pan, choke, delay send, n/a
        0,0,0,0,0                   // n/a, n/a, n/a, n/a, n/a
    };

    int32_t dexedSynthInitParams[MAXIMUM_SOUND_PARAMS] = {
        0,100,0,0,0,    // transpose, algorithm, n/a, n/a, n/a
        0,0,0,0,0,      // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0,      // n/a, n/a, n/a, n/a, n/a
        100,0,-100,0,0, // level, pan, choke, delay send, n/a
        0,0,0,0,0,       // poly note b, poly note c, poly note d, note mode, n/a
        0,0,0,0,0               // n/a, n/a, n/a, n/a, n/a
    };

    const uint8_t dexedInitVoice[MAXIMUM_DEXED_SOUND_PARAMS] = {
      99, 99, 99, 99, 99, 99, 99, 00, 33, 00, 00, 00, 00, 00, 00, 00, 00, 00, 01, 00, 00, // OP6 eg_rate_1-4, level_1-4, kbd_lev_scl_brk_pt, kbd_lev_scl_lft_depth, kbd_lev_scl_rht_depth, kbd_lev_scl_lft_curve, kbd_lev_scl_rht_curve, kbd_rate_scaling, amp_mod_sensitivity, key_vel_sensitivity, operator_output_level, osc_mode, osc_freq_coarse, osc_freq_fine, osc_detune
      99, 99, 99, 99, 99, 99, 99, 00, 33, 00, 00, 00, 00, 00, 00, 00, 00, 00, 01, 00, 00, // OP5
      99, 99, 99, 99, 99, 99, 99, 00, 33, 00, 00, 00, 00, 00, 00, 00, 00, 00, 01, 00, 00, // OP4
      99, 99, 99, 99, 99, 99, 99, 00, 33, 00, 00, 00, 00, 00, 00, 00, 00, 00, 01, 00, 00, // OP4
      99, 99, 99, 99, 99, 99, 99, 00, 33, 00, 00, 00, 00, 00, 00, 00, 00, 00, 01, 00, 00, // OP4
      99, 99, 99, 99, 99, 99, 99, 00, 33, 00, 00, 00, 00, 00, 00, 00, 99, 00, 01, 00, 00, // OP4
      99, 99, 99, 99, 50, 50, 50, 50,                                                     // 4 * pitch EG rates, 4 * pitch EG level
      01, 00, 01,                                                                         // algorithm, feedback, osc sync
      35, 00, 00, 00, 01, 00,                                                             // lfo speed, lfo delay, lfo pitch_mod_depth, lfo_amp_mod_depth, lfo_sync, lfo_waveform
      03, 48,                                                                             // pitch_mod_sensitivity, transpose
      73, 78, 73, 84, 32, 86, 79, 73, 67, 69                                              // 10 * char for name ("INIT VOICE")
    };

    int32_t braidsSynthInitParams[MAXIMUM_SOUND_PARAMS] = {
        0,0,0,0,0,      // timbre, color, modulation, fm, n/a
        0,0,0,0,0,      // coarse, fine, n/a, n/a, n/a
        0,0,0,0,0,      // n/a, n/a, n/a, n/a, n/a
        100,0,-100,0,0,    // level, pan, choke, delay send, n/a
        0,0,0,0,0,       // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0       // n/a, n/a, n/a, n/a, n/a
    };

    int32_t fmDrumInitParams[MAXIMUM_SOUND_PARAMS] = {
        5000,0,75,0,0,  // frequency, fm, decay, noise, overdrive
        0,0,0,0,0,      // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0,      // n/a, n/a, n/a, n/a, n/a
        100,0,-100,0,0, // level, pan, choke, delay send, n/a
        0,0,0,0,0,       // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0        // n/a, n/a, n/a, n/a, n/a
    };

    int32_t midiInitParams[MAXIMUM_SOUND_PARAMS] = {
        100,0,0,0,0, // channel, n/a, n/a, n/a, n/a
        0,0,0,0,0, // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0, // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0, // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0, // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0  // n/a, n/a, n/a, n/a, n/a
    };

    int32_t cvGateInitParams[MAXIMUM_SOUND_PARAMS] = {
        100,0,0,0,0,    // port, n/a, n/a, n/a, n/a
        0,0,0,0,0,      // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0,      // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0,      // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0,      // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0       // n/a, n/a, n/a, n/a, n/a
    };

    int32_t cvTrigInitParams[MAXIMUM_SOUND_PARAMS] = {
        100,0,0,0,0,    // port, n/a, n/a, n/a, n/a
        0,0,0,0,0,      // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0,      // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0,      // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0,      // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0       // n/a, n/a, n/a, n/a, n/a
    };

    std::map<SOUND_TYPE, int8_t> currentPatternSoundInstanceMap = {
        {T_EMPTY, 0},
        {T_MIDI, 0},
        {T_MONO_SAMPLE, 0},
        {T_MONO_SYNTH, 0},
        {T_DEXED_SYNTH, 0},
        {T_FM_DRUM, 0},
        {T_CV_GATE, 0},
        {T_CV_TRIG, 0},
        {T_BRAIDS_SYNTH, 0},
    };

    bool soundNeedsReinit[MAXIMUM_SEQUENCER_TRACKS] = {
        false, false, false, false,
        false, false, false, false,
        false, false, false, false,
        false, false, false, false,
    };

    bool chokeSourcesEnabled[MAXIMUM_SEQUENCER_TRACKS] = {
        false, false, false, false,
        false, false, false, false,
        false, false, false, false,
        false, false, false, false,
    };

    int chokeSourceDest[MAXIMUM_SEQUENCER_TRACKS] = {
        -1, -1, -1, -1,
        -1, -1, -1, -1,
        -1, -1, -1, -1,
        -1, -1, -1, -1,
    };

    int chokeDestSource[MAXIMUM_SEQUENCER_TRACKS] = {
        -1, -1, -1, -1,
        -1, -1, -1, -1,
        -1, -1, -1, -1,
        -1, -1, -1, -1,
    };

    MonoSampleInstance monoSampleInstances[MAXIMUM_MONO_SAMPLE_SOUNDS] = {
        MonoSampleInstance(monoSample1, monoSampleAmpEnv1, monoSampleAmpAccent1, monoSampleAmp1, monoSampleAmpDelaySend1, monoSampleLeft1, monoSampleRight1),
        MonoSampleInstance(monoSample2, monoSampleAmpEnv2, monoSampleAmpAccent2, monoSampleAmp2, monoSampleAmpDelaySend2, monoSampleLeft2, monoSampleRight2),
        MonoSampleInstance(monoSample3, monoSampleAmpEnv3, monoSampleAmpAccent3, monoSampleAmp3, monoSampleAmpDelaySend3, monoSampleLeft3, monoSampleRight3),
        MonoSampleInstance(monoSample4, monoSampleAmpEnv4, monoSampleAmpAccent4, monoSampleAmp4, monoSampleAmpDelaySend4, monoSampleLeft4, monoSampleRight4),
        MonoSampleInstance(monoSample5, monoSampleAmpEnv5, monoSampleAmpAccent5, monoSampleAmp5, monoSampleAmpDelaySend5, monoSampleLeft5, monoSampleRight5),
        MonoSampleInstance(monoSample6, monoSampleAmpEnv6, monoSampleAmpAccent6, monoSampleAmp6, monoSampleAmpDelaySend6, monoSampleLeft6, monoSampleRight6),
        MonoSampleInstance(monoSample7, monoSampleAmpEnv7, monoSampleAmpAccent7, monoSampleAmp7, monoSampleAmpDelaySend7, monoSampleLeft7, monoSampleRight7),
        MonoSampleInstance(monoSample8, monoSampleAmpEnv8, monoSampleAmpAccent8, monoSampleAmp8, monoSampleAmpDelaySend8, monoSampleLeft8, monoSampleRight8),
        MonoSampleInstance(monoSample9, monoSampleAmpEnv9, monoSampleAmpAccent9, monoSampleAmp9, monoSampleAmpDelaySend9, monoSampleLeft9, monoSampleRight9),
        MonoSampleInstance(monoSample10, monoSampleAmpEnv10, monoSampleAmpAccent10, monoSampleAmp10, monoSampleAmpDelaySend10, monoSampleLeft10, monoSampleRight10),
        MonoSampleInstance(monoSample11, monoSampleAmpEnv11, monoSampleAmpAccent11, monoSampleAmp11, monoSampleAmpDelaySend11, monoSampleLeft11, monoSampleRight11),
        MonoSampleInstance(monoSample12, monoSampleAmpEnv12, monoSampleAmpAccent12, monoSampleAmp12, monoSampleAmpDelaySend12, monoSampleLeft12, monoSampleRight12),
        MonoSampleInstance(monoSample13, monoSampleAmpEnv13, monoSampleAmpAccent13, monoSampleAmp13, monoSampleAmpDelaySend13, monoSampleLeft13, monoSampleRight13),
        MonoSampleInstance(monoSample14, monoSampleAmpEnv14, monoSampleAmpAccent14, monoSampleAmp14, monoSampleAmpDelaySend14, monoSampleLeft14, monoSampleRight14),
        MonoSampleInstance(monoSample15, monoSampleAmpEnv15, monoSampleAmpAccent15, monoSampleAmp15, monoSampleAmpDelaySend15, monoSampleLeft15, monoSampleRight15),
        MonoSampleInstance(monoSample16, monoSampleAmpEnv16, monoSampleAmpAccent16, monoSampleAmp16, monoSampleAmpDelaySend16, monoSampleLeft16, monoSampleRight16),
    };
    
    MonoSynthInstance monoSynthInstances[MAXIMUM_MONO_SYNTH_SOUNDS] = {
        MonoSynthInstance(
            monoSynthOscA1, monoSynthOscB1, monoSynthNoise1, monoSynthMix1, 
            monoSynthDc1, monoSynthFilterEnv1, monoSynthFilterAccent1, monoSynthFilter1, 
            monoSynthAmpEnv1, monoSynthAmpAccent1, monoSynthAmp1, monoSynthAmpDelaySend1, monoSynthLeft1, monoSynthRight1
        ),
        MonoSynthInstance(
            monoSynthOscA2, monoSynthOscB2, monoSynthNoise2, monoSynthMix2, 
            monoSynthDc2,  monoSynthFilterEnv2, monoSynthFilterAccent2, monoSynthFilter2,
            monoSynthAmpEnv2, monoSynthAmpAccent2, monoSynthAmp2, monoSynthAmpDelaySend2, monoSynthLeft2, monoSynthRight2
        ),
        MonoSynthInstance(
            monoSynthOscA3, monoSynthOscB3, monoSynthNoise3, monoSynthMix3, 
            monoSynthDc3, monoSynthFilterEnv3, monoSynthFilterAccent3, monoSynthFilter3, 
            monoSynthAmpEnv3, monoSynthAmpAccent3, monoSynthAmp3, monoSynthAmpDelaySend3, monoSynthLeft3, monoSynthRight3
        ),
        MonoSynthInstance(
            monoSynthOscA4, monoSynthOscB4, monoSynthNoise4, monoSynthMix4, 
            monoSynthDc4, monoSynthFilterEnv4, monoSynthFilterAccent4, monoSynthFilter4, 
            monoSynthAmpEnv4, monoSynthAmpAccent4, monoSynthAmp4, monoSynthAmpDelaySend4, monoSynthLeft4, monoSynthRight4
        ),
    };

    DexedInstance dexedInstances[MAXIMUM_DEXED_SYNTH_SOUNDS] = {
        DexedInstance(dexed1, dexedAmpAccent1, dexedAmp1, dexedAmpDelaySend1, dexedLeft1, dexedRight1),
        DexedInstance(dexed2, dexedAmpAccent2, dexedAmp2, dexedAmpDelaySend2, dexedLeft2, dexedRight2),
        DexedInstance(dexed3, dexedAmpAccent3, dexedAmp3, dexedAmpDelaySend3, dexedLeft3, dexedRight3),
        DexedInstance(dexed4, dexedAmpAccent4, dexedAmp4, dexedAmpDelaySend4, dexedLeft4, dexedRight4),
        DexedInstance(dexed5, dexedAmpAccent5, dexedAmp5, dexedAmpDelaySend5, dexedLeft5, dexedRight5),
        DexedInstance(dexed6, dexedAmpAccent6, dexedAmp6, dexedAmpDelaySend6, dexedLeft6, dexedRight6),
        DexedInstance(dexed7, dexedAmpAccent7, dexedAmp7, dexedAmpDelaySend7, dexedLeft7, dexedRight7),
        DexedInstance(dexed8, dexedAmpAccent8, dexedAmp8, dexedAmpDelaySend8, dexedLeft8, dexedRight8),
    };

    // FmDrumInstance fmDrumInstances[MAXIMUM_FM_DRUM_SOUNDS] = {
    //     FmDrumInstance(fmDrum1, fmDrumAmpAccent1, fmDrumAmp1, fmDrumAmpDelaySend1, fmDrumLeft1, fmDrumRight1),
    //     FmDrumInstance(fmDrum2, fmDrumAmpAccent2, fmDrumAmp2, fmDrumAmpDelaySend2, fmDrumLeft2, fmDrumRight2),
    //     FmDrumInstance(fmDrum3, fmDrumAmpAccent3, fmDrumAmp3, fmDrumAmpDelaySend3, fmDrumLeft3, fmDrumRight3),
    // };

    // BraidsInstance braidsInstances[MAXIMUM_BRAIDS_SYNTH_SOUNDS] = {
    //     BraidsInstance(braids1, braidsAmp1, braidsLeft1, braidsRight1),
    // };

    StereoDelayInstance delayInstances[1] = {
        StereoDelayInstance(
            delay1,
            delayFeedbackMixer1,
            delayLeftAmp1, delayRightAmp1
        )
    };

    std::map<int, loop_type> loopTypeSelMap = {
        {0, looptype_none},
        {1, looptype_repeat},
    };

    std::map<loop_type, int> loopTypeFindMap = {
        {looptype_none, 0},
        {looptype_repeat, 1},
    };

    std::map<int, play_start> playStartSelMap = {
        {0, play_start_sample},
        {1, play_start_loop},
    };

    std::map<play_start, int> playStartFindMap = {
        {play_start_sample, 0},
        {play_start_loop, 1},
    };

    std::map<SOUND_TYPE, int32_t*> soundTypeInitParams = {
        { T_MONO_SAMPLE, monoSampleInitParams },
        { T_MONO_SYNTH, monoSynthInitParams },
        { T_DEXED_SYNTH, dexedSynthInitParams },
        { T_BRAIDS_SYNTH, braidsSynthInitParams },
        { T_FM_DRUM, fmDrumInitParams },
        { T_MIDI, midiInitParams },
        { T_CV_GATE, cvGateInitParams },
        { T_CV_TRIG, cvTrigInitParams },
    };

    std::map<SOUND_TYPE, int> soundPageNumMap = {
        {T_MONO_SAMPLE, 6},
        {T_MONO_SYNTH, 6},
        {T_DEXED_SYNTH, 5},
        {T_BRAIDS_SYNTH, 3},
        {T_FM_DRUM, 3},
        {T_MIDI, 1},
        {T_CV_GATE, 1},
        {T_CV_TRIG, 1},
        {T_EMPTY, 1},
    };

    std::map<SOUND_TYPE, std::map<int, std::string>> soundCurrPageNameMap = {
        {T_MONO_SAMPLE, {
                        {0, "step"},
                        {1, "file"},
                        {2, "pitch"},
                        {3, "loop"},
                        {4, "amp env"},
                        {5, "output"},
                    }},
        {T_MONO_SYNTH, {
                        {0, "step"},
                        {1, "osc"},
                        {2, "filter"},
                        {3, "filter env"},
                        {4, "amp env"},
                        {5, "output"},
                    }},
        {T_DEXED_SYNTH, {
                        {0, "step"},
                        {1, "fm1"},
                        {2, "fm2"},
                        {3, "poly"},
                        {4, "output"},
                    }},
        {T_FM_DRUM, {
                        {0, "step"},
                        {1, "drum"},
                        {2, "output"},
                    }},
        {T_MIDI, {
                    {0, "step"},
                }},
        {T_CV_GATE, {
                    {0, "step"},
                }},
        {T_CV_TRIG, {
                    {0, "step"},
                }},
        {T_EMPTY, {
                    {0, ""},
                }},
    };

    std::string patternPageNames[MAXIMUM_PATTERN_PAGES] = {
        "main",
        "fx: stereo delay"
    };

    std::map<SOUND_TYPE, int8_t> soundTypeInstanceLimitMap = {
        {T_EMPTY, MAXIMUM_SEQUENCER_TRACKS},
        {T_MONO_SAMPLE, MAXIMUM_MONO_SAMPLE_SOUNDS},
        {T_MONO_SYNTH, MAXIMUM_MONO_SYNTH_SOUNDS},
        {T_DEXED_SYNTH, MAXIMUM_DEXED_SYNTH_SOUNDS},
        {T_BRAIDS_SYNTH, MAXIMUM_BRAIDS_SYNTH_SOUNDS},
        {T_FM_DRUM, MAXIMUM_FM_DRUM_SOUNDS},
        {T_MIDI, MAXIMUM_SEQUENCER_TRACKS},
        {T_CV_GATE, MAXIMUM_CV_GATE_SOUNDS},
        {T_CV_TRIG, MAXIMUM_CV_TRIG_SOUNDS},
    };

    void muteAllOutput()
    {
        // Voice/instance sub mixers
        voiceSubMixLeft1.gain(0, 0);
        voiceSubMixRight1.gain(0, 0);
        voiceSubMixLeft1.gain(1, 0);
        voiceSubMixRight1.gain(1, 0);
        voiceSubMixLeft1.gain(2, 0);
        voiceSubMixRight1.gain(2, 0);
        voiceSubMixLeft1.gain(3, 0);
        voiceSubMixRight1.gain(3, 0);

        voiceSubMixLeft2.gain(0, 0);
        voiceSubMixRight2.gain(0, 0);
        voiceSubMixLeft2.gain(1, 0);
        voiceSubMixRight2.gain(1, 0);
        voiceSubMixLeft2.gain(2, 0);
        voiceSubMixRight2.gain(2, 0);
        voiceSubMixLeft2.gain(3, 0);
        voiceSubMixRight2.gain(3, 0);

        voiceSubMixLeft3.gain(0, 0);
        voiceSubMixRight3.gain(0, 0);
        voiceSubMixLeft3.gain(1, 0);
        voiceSubMixRight3.gain(1, 0);
        voiceSubMixLeft3.gain(2, 0);
        voiceSubMixRight3.gain(2, 0);
        voiceSubMixLeft3.gain(3, 0);
        voiceSubMixRight3.gain(3, 0);

        voiceSubMixLeft4.gain(0, 0);
        voiceSubMixRight4.gain(0, 0);
        voiceSubMixLeft4.gain(1, 0);
        voiceSubMixRight4.gain(1, 0);
        voiceSubMixLeft4.gain(2, 0);
        voiceSubMixRight4.gain(2, 0);
        voiceSubMixLeft4.gain(3, 0);
        voiceSubMixRight4.gain(3, 0);

        voiceSubMixLeft5.gain(0, 0);
        voiceSubMixRight5.gain(0, 0);
        voiceSubMixLeft5.gain(1, 0);
        voiceSubMixRight5.gain(1, 0);
        voiceSubMixLeft5.gain(2, 0);
        voiceSubMixRight5.gain(2, 0);
        voiceSubMixLeft5.gain(3, 0);
        voiceSubMixRight5.gain(3, 0);

        voiceSubMixLeft6.gain(0, 0);
        voiceSubMixRight6.gain(0, 0);
        voiceSubMixLeft6.gain(1, 0);
        voiceSubMixRight6.gain(1, 0);
        voiceSubMixLeft6.gain(2, 0);
        voiceSubMixRight6.gain(2, 0);
        voiceSubMixLeft6.gain(3, 0);
        voiceSubMixRight6.gain(3, 0);

        voiceSubMixLeft7.gain(0, 0);
        voiceSubMixRight7.gain(0, 0);
        voiceSubMixLeft7.gain(1, 0);
        voiceSubMixRight7.gain(1, 0);
        voiceSubMixLeft7.gain(2, 0);
        voiceSubMixRight7.gain(2, 0);
        voiceSubMixLeft7.gain(3, 0);
        voiceSubMixRight7.gain(3, 0);
        
        // Voice/instance mixers
        voiceMixLeft1.gain(0, 0);
        voiceMixRight1.gain(0, 0);
        voiceMixLeft1.gain(1, 0);
        voiceMixRight1.gain(1, 0);
        voiceMixLeft1.gain(2, 0);
        voiceMixRight1.gain(2, 0);

        voiceMixLeft2.gain(0, 0);
        voiceMixRight2.gain(0, 0);
        voiceMixLeft2.gain(1, 0);
        voiceMixRight2.gain(1, 0);
        voiceMixLeft2.gain(2, 0);
        voiceMixRight2.gain(2, 0);
        voiceMixLeft2.gain(3, 0);
        voiceMixRight2.gain(3, 0);

        // Main L&R output mixer
        mainMixerLeft.gain(0, 0);
        mainMixerRight.gain(0, 0);
        mainMixerLeft.gain(1, 0);
        mainMixerRight.gain(1, 0);
        // todo need delay chans here?
        mainMixerLeft.gain(3, 0);
        mainMixerRight.gain(3, 0);

        // L&R input mixer
        inputMixerLeft.gain(0, 0);
        inputMixerRight.gain(0, 0);

        // Main L&R output mixer
        OutputMixerLeft.gain(0, 0);
        OutputMixerRight.gain(0, 0);
        OutputMixerLeft.gain(1, 0);
        OutputMixerRight.gain(1, 0);
    }

    void init()
    {
        // initialize CV level array
        for (int i = 0; i < 128; i++)
        {
            _cvLevels[i] = i * 26;
        }

        initKit(activeKit);
        initKit(idleKit);
        initVoices();

        XRDexedManager::init();
    }

    void initKit(KIT &kit)
    {
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            kit.sounds[t].type = T_EMPTY;

            for (int p = 0; p < MAXIMUM_SOUND_PARAMS; p++) {
                kit.sounds[t].params[p] = 0;
            }

            for (int dp = 0; dp < MAXIMUM_DEXED_SOUND_PARAMS; dp++) {
                kit.sounds[t].dexedParams[dp] = dexedInitVoice[dp];
            }

            strcpy(kit.sounds[t].name, "NO SOUND");
            strcpy(kit.sounds[t].sampleName, "");
            strcpy(kit.sounds[t].sampleNameB, "");
        }
    }

    void initIdleKit()
    {
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            idleKit.sounds[t].type = T_EMPTY;

            for (int p = 0; p < MAXIMUM_SOUND_PARAMS; p++) {
                idleKit.sounds[t].params[p] = 0;
            }

            for (int dp = 0; dp < MAXIMUM_DEXED_SOUND_PARAMS; dp++) {
                idleKit.sounds[t].dexedParams[dp] = dexedInitVoice[dp];
            }

            strcpy(idleKit.sounds[t].name, "NO SOUND");
            strcpy(idleKit.sounds[t].sampleName, "");
            strcpy(idleKit.sounds[t].sampleNameB, "");
        }
    }

    void initSoundForIdleKit(uint8_t t)
    {
        idleKit.sounds[t].type = T_EMPTY;

        for (int p = 0; p < MAXIMUM_SOUND_PARAMS; p++) {
            idleKit.sounds[t].params[p] = 0;
        }

        for (int dp = 0; dp < MAXIMUM_DEXED_SOUND_PARAMS; dp++) {
            idleKit.sounds[t].dexedParams[dp] = dexedInitVoice[dp];
        }

        strcpy(idleKit.sounds[t].name, "NO SOUND");
        strcpy(idleKit.sounds[t].sampleName, "");
        strcpy(idleKit.sounds[t].sampleNameB, "");
    }

    void initVoices()
    {
        auto msmpSamplePlayRate = getValueNormalizedAsFloat(monoSampleInitParams[MSMP_SAMPLEPLAYRATE]);
        auto msmpAatt = getValueNormalizedAsFloat(monoSampleInitParams[MSMP_AMP_ATTACK]);
        auto msmpAdec = getValueNormalizedAsFloat(monoSampleInitParams[MSMP_AMP_DECAY]);
        auto msmpAsus = getValueNormalizedAsFloat(monoSampleInitParams[MSMP_AMP_SUSTAIN]);
        auto msmpArel = getValueNormalizedAsFloat(monoSampleInitParams[MSMP_AMP_RELEASE]);
        auto msmpLvl = getValueNormalizedAsFloat(monoSampleInitParams[MSMP_LEVEL]);
        auto msmpPan = getValueNormalizedAsFloat(monoSampleInitParams[MSMP_PAN]);
        auto msmpDly = getValueNormalizedAsFloat(monoSampleInitParams[MSMP_DELAY]);

        // init MonoSampleInstance objects
        for (int i=0; i<MAXIMUM_MONO_SAMPLE_SOUNDS; i++)
        {
            monoSampleInstances[i].sample.setPlaybackRate(msmpSamplePlayRate);
            monoSampleInstances[i].sample.enableInterpolation(true);
            monoSampleInstances[i].sample.setBufferInPSRAM(true);

            monoSampleInstances[i].ampEnv.attack(msmpAatt);
            monoSampleInstances[i].ampEnv.decay(msmpAdec);
            monoSampleInstances[i].ampEnv.sustain(msmpAsus);
            monoSampleInstances[i].ampEnv.release(msmpArel);

            monoSampleInstances[i].ampAccent.gain(1.0); // used by track velocity
            monoSampleInstances[i].amp.gain(msmpLvl); // used by sound volume
            monoSampleInstances[i].ampDelaySend.gain(0);

            PANNED_AMOUNTS monoSamplePannedAmounts = getStereoPanValues(msmpPan);
            monoSampleInstances[i].left.gain(monoSamplePannedAmounts.left);
            monoSampleInstances[i].right.gain(monoSamplePannedAmounts.right);
        }

        auto msynWave = getValueNormalizedAsUInt8(monoSynthInitParams[MSYN_WAVE]);
        auto msynAmpLvlA = getValueNormalizedAsFloat(monoSynthInitParams[MSYN_OSCA_LEVEL]);
        auto msynAmpLvlB = getValueNormalizedAsFloat(monoSynthInitParams[MSYN_OSCB_LEVEL]);
        auto msynWidth = getValueNormalizedAsFloat(monoSynthInitParams[MSYN_WIDTH]);
        auto msynNoise = getValueNormalizedAsFloat(monoSynthInitParams[MSYN_NOISE]);
        auto msynFenv = getValueNormalizedAsFloat(monoSynthInitParams[MSYN_FILTER_ENV_AMT]);
        auto msynCutoff = getValueNormalizedAsInt32(monoSynthInitParams[MSYN_CUTOFF]);
        auto msynRes = getValueNormalizedAsFloat(monoSynthInitParams[MSYN_RESONANCE]);
        auto msynFatt = getValueNormalizedAsFloat(monoSynthInitParams[MSYN_FILTER_ATTACK]);
        auto msynFdec = getValueNormalizedAsFloat(monoSynthInitParams[MSYN_FILTER_DECAY]);
        auto msynFsus = getValueNormalizedAsFloat(monoSynthInitParams[MSYN_FILTER_SUSTAIN]);
        auto msynFrel = getValueNormalizedAsFloat(monoSynthInitParams[MSYN_FILTER_RELEASE]);
        auto msynAatt = getValueNormalizedAsFloat(monoSynthInitParams[MSYN_AMP_ATTACK]);
        auto msynAdec = getValueNormalizedAsFloat(monoSynthInitParams[MSYN_AMP_DECAY]);
        auto msynAsus = getValueNormalizedAsFloat(monoSynthInitParams[MSYN_AMP_SUSTAIN]);
        auto msynArel = getValueNormalizedAsFloat(monoSynthInitParams[MSYN_AMP_RELEASE]);
        auto msynPan = getValueNormalizedAsFloat(monoSynthInitParams[MSYN_PAN]);
        auto msynLvl = getValueNormalizedAsFloat(monoSynthInitParams[MSYN_LEVEL]);
        auto msynDly = getValueNormalizedAsFloat(monoSynthInitParams[MSYN_DELAY]);

        for (int s=0; s<MAXIMUM_MONO_SYNTH_SOUNDS; s++)
        {
            monoSynthInstances[s].oscA.begin(msynWave);
            monoSynthInstances[s].oscA.frequency(NOTE_FREQ_C4);
            monoSynthInstances[s].oscA.amplitude(msynAmpLvlA);
            monoSynthInstances[s].oscA.pulseWidth(msynWidth);
            monoSynthInstances[s].oscB.begin(msynWave);
            monoSynthInstances[s].oscB.frequency(NOTE_FREQ_C4);
            monoSynthInstances[s].oscB.amplitude(msynAmpLvlB);
            monoSynthInstances[s].oscB.pulseWidth(msynWidth);
            monoSynthInstances[s].noise.amplitude(msynNoise);
            monoSynthInstances[s].mix.gain(0, 0.33); // osc A
            monoSynthInstances[s].mix.gain(1, 0.33); // osc B
            monoSynthInstances[s].mix.gain(2, 0.33); // noise

            monoSynthInstances[s].dc.amplitude(msynFenv);
            monoSynthInstances[s].filterEnv.attack(msynFatt);
            monoSynthInstances[s].filterEnv.decay(msynFdec);
            monoSynthInstances[s].filterEnv.sustain(msynFsus);
            monoSynthInstances[s].filterEnv.release(msynFrel);
            monoSynthInstances[s].filterAccent.gain(1.0); // used by track velocity
            monoSynthInstances[s].filter.frequency(msynCutoff);
            monoSynthInstances[s].filter.resonance(msynRes);
            monoSynthInstances[s].filter.octaveControl(4); // TODO: use 7 ?
            
            monoSynthInstances[s].ampEnv.attack(msynAatt);
            monoSynthInstances[s].ampEnv.decay(msynAdec);
            monoSynthInstances[s].ampEnv.sustain(msynAsus);
            monoSynthInstances[s].ampEnv.release(msynArel);

            monoSynthInstances[s].ampAccent.gain(1.0); // used by track velocity
            monoSynthInstances[s].amp.gain(msynLvl); // used by sound volume
            monoSynthInstances[s].ampDelaySend.gain(0);

            PANNED_AMOUNTS monoSynthPannedAmounts = getStereoPanValues(msynPan);
            monoSynthInstances[s].left.gain(monoSynthPannedAmounts.left);
            monoSynthInstances[s].right.gain(monoSynthPannedAmounts.right);
        }

        auto dexeLvl = getValueNormalizedAsFloat(dexedSynthInitParams[DEXE_LEVEL]);
        auto dexePan = getValueNormalizedAsFloat(dexedSynthInitParams[DEXE_PAN]);
        auto dexeDly = getValueNormalizedAsFloat(dexedSynthInitParams[DEXE_DELAY]);

        for (int d=0; d<MAXIMUM_DEXED_SYNTH_SOUNDS; d++)
        {
            dexedInstances[d].dexed.loadInitVoice();

            dexedInstances[d].ampAccent.gain(1.0); // used by track velocity
            dexedInstances[d].amp.gain(dexeLvl);
            dexedInstances[d].ampDelaySend.gain(0);

            PANNED_AMOUNTS dexedSynthPannedAmounts = getStereoPanValues(dexePan);
            dexedInstances[d].left.gain(dexedSynthPannedAmounts.left);
            dexedInstances[d].right.gain(dexedSynthPannedAmounts.right);
        }

        // auto braidsLvl = getValueNormalizedAsFloat(braidsSynthInitParams[BRAIDS_LEVEL]);
        // auto braidsPan = getValueNormalizedAsFloat(braidsSynthInitParams[BRAIDS_PAN]);

        // for (int b=0; b<MAXIMUM_BRAIDS_SYNTH_SOUNDS; b++)
        // {
        //     braidsInstances[b].braids.init_braids();

        //     braidsInstances[b].ampAccent.gain(1.0); // used by track velocity
        //     braidsInstances[b].amp.gain(braidsLvl);

        //     PANNED_AMOUNTS braidsPannedAmounts = getStereoPanValues(braidsPan);
        //     dexedInstances[b].left.gain(braidsPannedAmounts.left);
        //     dexedInstances[b].right.gain(braidsPannedAmounts.right);
        // }

        auto fmDrumLvl = getValueNormalizedAsFloat(fmDrumInitParams[FMD_LEVEL]);
        auto fmDrumPan = getValueNormalizedAsFloat(fmDrumInitParams[FMD_PAN]);
        auto fmDrumDly = getValueNormalizedAsFloat(dexedSynthInitParams[FMD_DELAY]);

        Serial.printf("fmDrumLvl: %f\n");

        // for (int f=0; f<MAXIMUM_FM_DRUM_SOUNDS; f++)
        // {
        //     fmDrumInstances[f].fmDrum.init();

        //     fmDrumInstances[f].ampAccent.gain(1.0); // used by track velocity
        //     fmDrumInstances[f].amp.gain(fmDrumLvl);
        //     fmDrumInstances[f].ampDelaySend.gain(0);

        //     PANNED_AMOUNTS fmDrumPannedAmounts = getStereoPanValues(fmDrumPan);
        //     fmDrumInstances[f].left.gain(fmDrumPannedAmounts.left);
        //     fmDrumInstances[f].right.gain(fmDrumPannedAmounts.right);
        // }

        // Delay sub mixers
        delaySubMix1.gain(0, 1);
        delaySubMix1.gain(1, 1);
        delaySubMix1.gain(2, 1);
        delaySubMix1.gain(3, 1);
        delaySubMix2.gain(0, 1);
        delaySubMix2.gain(1, 1);
        delaySubMix2.gain(2, 1);
        delaySubMix2.gain(3, 1);
        delaySubMix3.gain(0, 1);
        delaySubMix3.gain(1, 1);
        delaySubMix3.gain(2, 1);
        delaySubMix3.gain(3, 1);
        delaySubMix4.gain(0, 1);
        delaySubMix4.gain(1, 1);
        delaySubMix4.gain(2, 1);
        delaySubMix4.gain(3, 1);
        delaySubMix5.gain(0, 1);
        delaySubMix5.gain(1, 1);
        delaySubMix5.gain(2, 1);
        delaySubMix5.gain(3, 1);
        delaySubMix6.gain(0, 1);
        delaySubMix6.gain(1, 1);
        delaySubMix6.gain(2, 1);
        delaySubMix6.gain(3, 1);
        delaySubMix7.gain(0, 1);
        delaySubMix7.gain(1, 1);
        delaySubMix7.gain(2, 1);
        delaySubMix7.gain(3, 1);
        delaySubMix8.gain(0, 1);
        delaySubMix8.gain(1, 1);
        delaySubMix8.gain(2, 1);
        delaySubMix8.gain(3, 1);

        // Delay mixers
        delayMix1.gain(0, 1);
        delayMix1.gain(1, 1);
        delayMix1.gain(2, 1);
        delayMix1.gain(3, 1);
        delayMix2.gain(0, 1);
        delayMix2.gain(1, 1);
        delayMix2.gain(2, 1);
        delayMix2.gain(3, 1);

        // Delay instance
        delayInstances[0].delayEffect.delay(0, 300); // TODO: init from pattern
        //delayInstances[0].delayEffect.disable(0);
        delayInstances[0].delayEffect.disable(1);
        delayInstances[0].delayEffect.disable(2);
        delayInstances[0].delayEffect.disable(3);
        delayInstances[0].delayEffect.disable(4);
        delayInstances[0].delayEffect.disable(5);
        delayInstances[0].delayEffect.disable(6);
        delayInstances[0].feedbackMix.gain(0, 0); // feedback line
        delayInstances[0].feedbackMix.gain(1, 1); // mix1 signal line
        delayInstances[0].feedbackMix.gain(2, 1); // mix2 signal line
        delayInstances[0].left.gain(1); // TODO: init from pattern pan left amount
        delayInstances[0].right.gain(1); // TODO: init from pattern pan right amount

        // Voice/instance sub mixers
        voiceSubMixLeft1.gain(0, 1);
        voiceSubMixRight1.gain(0, 1);
        voiceSubMixLeft1.gain(1, 1);
        voiceSubMixRight1.gain(1, 1);
        voiceSubMixLeft1.gain(2, 1);
        voiceSubMixRight1.gain(2, 1);
        voiceSubMixLeft1.gain(3, 1);
        voiceSubMixRight1.gain(3, 1);

        voiceSubMixLeft2.gain(0, 1);
        voiceSubMixRight2.gain(0, 1);
        voiceSubMixLeft2.gain(1, 1);
        voiceSubMixRight2.gain(1, 1);
        voiceSubMixLeft2.gain(2, 1);
        voiceSubMixRight2.gain(2, 1);
        voiceSubMixLeft2.gain(3, 1);
        voiceSubMixRight2.gain(3, 1);

        voiceSubMixLeft3.gain(0, 1);
        voiceSubMixRight3.gain(0, 1);
        voiceSubMixLeft3.gain(1, 1);
        voiceSubMixRight3.gain(1, 1);
        voiceSubMixLeft3.gain(2, 1);
        voiceSubMixRight3.gain(2, 1);
        voiceSubMixLeft3.gain(3, 1);
        voiceSubMixRight3.gain(3, 1);

        voiceSubMixLeft4.gain(0, 1);
        voiceSubMixRight4.gain(0, 1);
        voiceSubMixLeft4.gain(1, 1);
        voiceSubMixRight4.gain(1, 1);
        voiceSubMixLeft4.gain(2, 1);
        voiceSubMixRight4.gain(2, 1);
        voiceSubMixLeft4.gain(3, 1);
        voiceSubMixRight4.gain(3, 1);

        voiceSubMixLeft5.gain(0, 1);
        voiceSubMixRight5.gain(0, 1);
        voiceSubMixLeft5.gain(1, 1);
        voiceSubMixRight5.gain(1, 1);
        voiceSubMixLeft5.gain(2, 1);
        voiceSubMixRight5.gain(2, 1);
        voiceSubMixLeft5.gain(3, 1);
        voiceSubMixRight5.gain(3, 1);

        voiceSubMixLeft6.gain(0, 1);
        voiceSubMixRight6.gain(0, 1);
        voiceSubMixLeft6.gain(1, 1);
        voiceSubMixRight6.gain(1, 1);
        voiceSubMixLeft6.gain(2, 1);
        voiceSubMixRight6.gain(2, 1);
        voiceSubMixLeft6.gain(3, 1);
        voiceSubMixRight6.gain(3, 1);

        voiceSubMixLeft7.gain(0, 1);
        voiceSubMixRight7.gain(0, 1);
        voiceSubMixLeft7.gain(1, 1);
        voiceSubMixRight7.gain(1, 1);
        voiceSubMixLeft7.gain(2, 1);
        voiceSubMixRight7.gain(2, 1);
        voiceSubMixLeft7.gain(3, 1);
        voiceSubMixRight7.gain(3, 1);
        
        // Voice/instance mixers
        voiceMixLeft1.gain(0, 1);
        voiceMixRight1.gain(0, 1);
        voiceMixLeft1.gain(1, 1);
        voiceMixRight1.gain(1, 1);
        voiceMixLeft1.gain(2, 1);
        voiceMixRight1.gain(2, 1);
        voiceMixLeft1.gain(3, 1);
        voiceMixRight1.gain(3, 1);

        voiceMixLeft2.gain(0, 1);
        voiceMixRight2.gain(0, 1);
        voiceMixLeft2.gain(1, 1);
        voiceMixRight2.gain(1, 1);
        voiceMixLeft2.gain(2, 1);
        voiceMixRight2.gain(2, 1);
        voiceMixLeft2.gain(3, 1);
        voiceMixRight2.gain(3, 1);

        // Main L&R output mixer
        mainMixerLeft.gain(0, 1); // voice mix 1 L 
        mainMixerRight.gain(0, 1); // voice mix 1 R 
        mainMixerLeft.gain(1, 1); // voice mix 2 L 
        mainMixerRight.gain(1, 1); // voice mix 2 R 
        mainMixerLeft.gain(2, 1); // delay L
        mainMixerRight.gain(2, 1); // delay R
        mainMixerLeft.gain(3, 0.15); // metronome L
        mainMixerRight.gain(3, 0.15); // metronome R

        // L&R input mixer
        inputMixerLeft.gain(0, 0);
        inputMixerRight.gain(0, 0);
        // inputMixerLeft.gain(0, 0.25);
        // inputMixerRight.gain(0, 0.25);

        // Main L&R output mixer
        OutputMixerLeft.gain(0, 1);
        OutputMixerRight.gain(0, 1);
        OutputMixerLeft.gain(1, 1);
        OutputMixerRight.gain(1, 1);
        OutputMixerLeft.gain(2, 0);
        OutputMixerRight.gain(2, 0);
        OutputMixerLeft.gain(3, 0);
        OutputMixerRight.gain(3, 0);
    }

    void initTrackSound(int8_t track)
    {
        // init sound and sample names
        strcpy(activeKit.sounds[track].name, "");
        strcpy(activeKit.sounds[track].sampleName, "");
        strcpy(activeKit.sounds[track].sampleNameB, "");

        // init generic sound params
        auto soundType = activeKit.sounds[track].type;
        auto initParams = soundTypeInitParams[soundType];

        for (size_t p=0; p<MAXIMUM_SOUND_PARAMS; p++)
        {
            activeKit.sounds[track].params[p] = initParams[p];
        }

        // // dexed "init voice" params
        // for (size_t d=0; d<MAXIMUM_DEXED_SOUND_PARAMS; d++)
        // {
        //     currentPatternSounds[track].dexedParams[d] = dexedInitVoice[d];
        // }
    }

    void loadNextDexedInstances()
    {
        // first 4 tracks are dexed synth capable
        for (int t = 0; t < 4; t++)
        {
            //Serial.printf("Trying to load dexed instances, track type is %d\n", idleKit.sounds[t].type);

            if (idleKit.sounds[t].type == T_DEXED_SYNTH) {
                auto i = XRDexedManager::inactiveInstances[t];

                //Serial.printf("LOADING DEXED FOR TRACK %d USING INSTANCE %d\n", t, i);

                // print loading next pattern's track's dexed voice settings into the inactive instance
                //Serial.printf("LOADING DEXED FOR NEXT TRACK %d USING INSTANCE %d\n", t, i);

                // load the next pattern's track's dexed voice settings into the inactive instance
                dexedInstances[i].dexed.loadVoiceParameters(idleKit.sounds[t].dexedParams);
                dexedInstances[i].dexed.setMonoMode(!idleKit.sounds[t].params[DEXE_NOTE_MODE]);
                dexedInstances[i].dexed.setTranspose(getValueNormalizedAsInt32(idleKit.sounds[t].params[DEXE_TRANSPOSE]));
                dexedInstances[i].dexed.setAlgorithm(getValueNormalizedAsInt32(idleKit.sounds[t].params[DEXE_ALGO]));
            }
        }
    }

    void setSoundNeedsReinit(int sound, bool reinit)
    {
        soundNeedsReinit[sound] = reinit;
    }

    void reinitSoundForTrack(int track)
    {
        // reinit active sound from next/idle
        activeKit.sounds[track] = idleKit.sounds[track];

        // all done reinitializing sound
        soundNeedsReinit[track] = false;

        // reset idle sound
        initSoundForIdleKit(track);
    }

    void applyFxForActivePattern()
    {
        auto &activePatternSettings = XRSequencer::activePatternSettings;

        for (size_t fxp = 0; fxp < MAXIMUM_PATTERN_FX_PARAM_PAGES; fxp++)
        {
            if (XRSequencer::patternFxPages[fxp] == XRSequencer::PATTERN_FX_PAGE_INDEXES::DELAY) {
                auto delayParams = activePatternSettings.fx.pages[fxp];
                auto delayPan = delayParams.params[XRSequencer::PATTERN_FX_DELAY_PARAMS::PAN];

                // Serial.printf(
                //     "APPLY PTN FX -- delay time: %f delay fdbk: %f delay pan: %f\n",
                //     delayParams.params[XRSequencer::PATTERN_FX_DELAY_PARAMS::TIME],
                //     delayParams.params[XRSequencer::PATTERN_FX_DELAY_PARAMS::FEEDBACK],
                //     delayParams.params[XRSequencer::PATTERN_FX_DELAY_PARAMS::PAN]
                // );

                float gainL = 1.0;
                if (delayPan < 0)
                {
                    gainL += delayPan;
                }

                float gainR = 1.0;
                if (delayPan > 0)
                {
                    gainR -= delayPan;
                }
                
                AudioNoInterrupts();

                delayInstances[0].delayEffect.delay(0, delayParams.params[XRSequencer::PATTERN_FX_DELAY_PARAMS::TIME]);
                delayInstances[0].feedbackMix.gain(0, delayParams.params[XRSequencer::PATTERN_FX_DELAY_PARAMS::FEEDBACK]);
                delayInstances[0].left.gain(gainR);
                delayInstances[0].right.gain(gainL);

                AudioInterrupts();
            }
        }
    }

    void applyActiveSounds()
    {
        for (size_t t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            if (t < 4 && activeKit.sounds[t].type == T_DEXED_SYNTH) {
                auto di = XRDexedManager::getActiveInstanceForTrack(t);

                Serial.printf("APPLYING DEXED FOR TRACK %d USING INSTANCE %d\n", t, di);

                // load any dexed voice settings for track
                dexedInstances[di].dexed.loadVoiceParameters(activeKit.sounds[t].dexedParams);
                dexedInstances[di].dexed.setMonoMode(!activeKit.sounds[t].params[DEXE_NOTE_MODE]);
                dexedInstances[di].dexed.setTranspose(getValueNormalizedAsInt32(activeKit.sounds[t].params[DEXE_TRANSPOSE]));
                dexedInstances[di].dexed.setAlgorithm(getValueNormalizedAsInt32(activeKit.sounds[t].params[DEXE_ALGO]));
            }
        }

        applyFxForActivePattern();
        applyTrackChokes();
    }

    int getChokeSourcesEnabledCount()
    {
        int enabled = 0;

        for (size_t c = 0; c < MAXIMUM_CHOKE_SOURCES; c++)
        {
            if (chokeSourcesEnabled[c]) {
                ++enabled;
            }
        }

        return enabled;
    }

    bool isTrackAnActiveChokeDestination(int track)
    {
        return chokeDestSource[track] > -1;
    }
    
    void applyTrackChokes()
    {
        // init track chokes
        for (size_t s = 0; s < MAXIMUM_SEQUENCER_TRACKS; s++) {
            chokeSourcesEnabled[s] = false;
            chokeSourceDest[s] = -1;
            chokeDestSource[s] = -1;
        }

        // apply active track chokes
        for (size_t s = 0; s < MAXIMUM_SEQUENCER_TRACKS; s++) {
            if (activeKit.sounds[s].type == T_MONO_SAMPLE) {
                auto chokeDest = getValueNormalizedAsInt8(activeKit.sounds[s].params[MSMP_CHOKE]);

                if (chokeDest > -1 && getChokeSourcesEnabledCount() < 8) {
                    chokeSourcesEnabled[s] = true;
                    chokeSourceDest[s] = chokeDest;
                    chokeDestSource[chokeDest] = s;
                }
            }
        }
    }

    void handleNoteOnForMetronome(bool accented)
    {
        metronome.length(25);
        metronome.frequency(accented ? 1500 : 1000);
        metronome.pitchMod(0.5);
        metronome.noteOn();
    }

    void swapSoundDataForPatternChange(int nextBank, int nextPattern)
    {
        auto &seqState = XRSequencer::getSeqState();

        if (seqState.playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::RUNNING) {
            auto &tracks = XRSequencer::idleTrackLayer.tracks;

            for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
            {
                if (tracks[t].initialized) {
                    setSoundNeedsReinit(t, true); // reinit sound asynchronously since the upcoming track is active
                } else {
                    reinitSoundForTrack(t); // reinit sound synchronously since the upcoming track is inactive?
                }
            }
        } else {
            for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
            {
                reinitSoundForTrack(t); // reinit all sounds synchronously since the sequencer isn't running
            }
        }
    }
    
    SOUND_CONTROL_MODS getControlModDataForPattern()
    {
        SOUND_CONTROL_MODS mods;

        auto &pattern = XRSequencer::activePatternSettings;
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        switch (currSelectedPageNum)
        {
        case 0: // MAIN
            {
                std::string grooveForPattern = pattern.groove.id > -1 ? XRClock::getGrooveString(pattern.groove.id) : "";
                std::string grooveAmountForPattern = XRClock::getGrooveAmountString(pattern.groove.id, pattern.groove.amount);

                mods.aName = "lstep";
                mods.bName = "groove";
                mods.cName = "gr.amt";
                mods.dName = "accent";

                mods.aValue = std::to_string(pattern.lstep);
                mods.bValue = pattern.groove.id > -1 ? grooveForPattern : "off";
                mods.cValue = pattern.groove.id > -1 ? grooveAmountForPattern : "--";
                mods.dValue = std::to_string(pattern.accent);
            }
            break;

        case 1: // FX: DELAY
            {
                auto delayParams = pattern.fx.pages[XRSequencer::PATTERN_FX_PAGE_INDEXES::DELAY]; 

                mods.aName = "time"; // delay time
                mods.bName = "fdbk"; // delay feedback
                mods.cName = "pan";
                mods.dName = "--";

                mods.aValue = std::to_string(round(delayParams.params[XRSequencer::PATTERN_FX_DELAY_PARAMS::TIME] * 100) / 100);
                mods.bValue = std::to_string(round(delayParams.params[XRSequencer::PATTERN_FX_DELAY_PARAMS::FEEDBACK] * 100));

                mods.cValue = std::to_string((float)round(delayParams.params[XRSequencer::PATTERN_FX_DELAY_PARAMS::PAN] * 100) / 100);
                mods.cValue = mods.cValue.substr(0, 3);
                mods.cFloatValue = delayParams.params[XRSequencer::PATTERN_FX_DELAY_PARAMS::PAN];
                mods.cType = RANGE;

                mods.dValue = "--";
            }

            break;
        default:
            break;
        }

        return mods;
    }

    SOUND_CONTROL_MODS getControlModDataForTrack()
    {
        SOUND_CONTROL_MODS mods;

        auto trackNum = XRSequencer::getCurrentSelectedTrackNum();

        auto soundType = activeKit.sounds[trackNum].type;

        switch (soundType)
        {
        case T_MONO_SAMPLE:
            mods = getMonoSampleControlModData();
            break;
        case T_MONO_SYNTH:
            mods = getMonoSynthControlModData();
            break;
        case T_DEXED_SYNTH:
            mods = getDexedSynthControlModData();
            break;
            case T_FM_DRUM:
            mods = getFmDrumControlModData();
            break;
        case T_MIDI:
            mods = getMidiControlModData();
            break;
        case T_CV_GATE:
            mods = getCvGateControlModData();
            break;
        case T_CV_TRIG:
            mods = getCvTrigControlModData();
            break;
        case T_EMPTY:
            mods = getEmptyControlModData();
            break;
        default:
            break;
        }

        return mods;
    }

    SOUND_CONTROL_MODS getRatchetControlModData()
    {
        SOUND_CONTROL_MODS mods;

        auto &ratchetTrack = XRSequencer::activeRatchetLayer
            .tracks[XRSequencer::getRatchetTrack()];

        auto currUXMode = XRUX::getCurrentMode();

        mods.isAbleToStepModA = false;
        mods.isAbleToStepModB = true;
        mods.isAbleToStepModC = true;
        mods.isAbleToStepModD = false;

        mods.isActiveStepModA = false;
        mods.isActiveStepModB = false;
        mods.isActiveStepModC = false;
        mods.isActiveStepModD = false;

        auto noteToUse = XRHelpers::getNoteStringForBaseNoteNum(ratchetTrack.note);
        noteToUse += std::to_string(ratchetTrack.octave);

        mods.aName = "lstep";
        mods.bName = "note";
        mods.cName = "velo";
        mods.dName = "latch";

        mods.aValue = std::to_string(ratchetTrack.lstep);
        mods.bValue = noteToUse;
        mods.cValue = std::to_string(ratchetTrack.velocity);
        mods.dValue = XRSequencer::ratchetLatched ? "ON" : "OFF";

        return mods;
    }

    SOUND_CONTROL_MODS getMonoSampleControlModData()
    {
        SOUND_CONTROL_MODS mods;

        auto currUXMode = XRUX::getCurrentMode();
        auto &currentSelectedTrackLayer = XRSequencer::activeTrackLayer;
        auto &currentSelectedTrack = XRSequencer::getCurrentSelectedTrack();
        auto currentSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currentSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        mods.isAbleToStepModA = false;
        mods.isAbleToStepModB = false;
        mods.isAbleToStepModC = false;
        mods.isAbleToStepModD = false;

        mods.isActiveStepModA = false;
        mods.isActiveStepModB = false;
        mods.isActiveStepModC = false;
        mods.isActiveStepModD = false;

        switch (currentSelectedPageNum)
        {
        case 0: // STEP
            {
                mods.isAbleToStepModC = true;
                mods.isAbleToStepModD = true;

                auto prob = currentSelectedTrack.probability;
                if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStepNum > -1) {
                    if (currentSelectedTrackLayer.tracks[currentSelectedTrackNum].steps[currentSelectedStepNum].tFlags[XRSequencer::PROBABILITY]) {
                        mods.isActiveStepModD = true;
                        prob = currentSelectedTrackLayer.tracks[currentSelectedTrackNum].steps[currentSelectedStepNum].tMods[XRSequencer::PROBABILITY];
                    }
                }

                mods.aName = "lstep";
                mods.bName = "--";
                mods.cName = "velo";
                mods.dName = "prob";

                mods.aValue = std::to_string(currentSelectedTrack.lstep);
                mods.bValue = "--";
                mods.cValue = std::to_string(currentSelectedTrack.velocity);
                mods.dValue = std::to_string(prob) + "%";
            }

            break;
        case 1: // FILE
            {
                mods.isAbleToStepModB = true;

                mods.aName = "file 1";
                mods.bName = "file 2";
                mods.cName = "--";
                mods.dName = "--";

                mods.aValue = "--";
                mods.bValue = "--";
                mods.cValue = "--";
                mods.dValue = "--";
            }

            break;
        case 2: // PITCH
            {
                mods.isAbleToStepModB = true;

                mods.aName = "speed";
                mods.bName = "--";
                mods.cName = "--";
                mods.dName = "--";

                auto sampleplayrate = getValueNormalizedAsFloat(
                    activeKit.sounds[currentSelectedTrackNum].params[MSMP_SAMPLEPLAYRATE]
                );

                mods.aValue = getPlaybackSpeedStr(sampleplayrate);
                mods.bValue = "--";
                mods.cValue = "--";
                mods.dValue = "--";
            }

            break;

        case 3: // LOOPING
            {
                mods.isAbleToStepModA = true;

                if (currUXMode == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStepNum > -1)
                {
                    if (currentSelectedTrack.steps[currentSelectedStepNum].sFlags[MSMP_LOOPTYPE])
                    {
                        mods.isActiveStepModA = true;
                    }
                }

                mods.aName = "type";
                mods.bName = "start";
                mods.cName = "finish";
                mods.dName = "plyst";

                mods.aValue = getLoopTypeName();

                auto loopstartToUse = getValueNormalizedAsUInt32(
                    activeKit.sounds[currentSelectedTrackNum].params[MSMP_LOOPSTART]
                );

                std::string lsStr = std::to_string(loopstartToUse);
                lsStr += "ms";

                mods.bValue = lsStr;

                auto loopfinishToUse = getValueNormalizedAsUInt32(
                    activeKit.sounds[currentSelectedTrackNum].params[MSMP_LOOPFINISH]
                );

                // if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStepNum > -1)
                // {
                //     loopfinishToUse = modsForCurrentTrackStep.loopfinish;
                // }

                std::string lfStr = std::to_string(loopfinishToUse);
                lfStr += "ms";

                mods.cValue = lfStr;

                auto playstartToUse = (play_start)getValueNormalizedAsInt8(
                    activeKit.sounds[currentSelectedTrackNum].params[MSMP_PLAYSTART]
                );

                // if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStepNum > -1)
                // {
                //     playstartToUse = modsForCurrentTrackStep.playstart;
                // }

                mods.dValue = playstartToUse == play_start::play_start_loop ? "loop" : "sample";
            }

            break;
        case 4: // AMP ENV
            {
                mods.aName = "att";
                mods.bName = "dec";
                mods.cName = "sus";
                mods.dName = "rel";

                auto aatt = getValueNormalizedAsFloat(activeKit.sounds[currentSelectedTrackNum].params[MSMP_AMP_ATTACK]);
                auto adec = getValueNormalizedAsFloat(activeKit.sounds[currentSelectedTrackNum].params[MSMP_AMP_DECAY]);
                auto asus = getValueNormalizedAsFloat(activeKit.sounds[currentSelectedTrackNum].params[MSMP_AMP_SUSTAIN]);
                auto arel = getValueNormalizedAsFloat(activeKit.sounds[currentSelectedTrackNum].params[MSMP_AMP_RELEASE]);

                mods.aValue = std::to_string((float)round(aatt * 100) / 100);
                mods.aValue = mods.aValue.substr(0, 3);
                mods.bValue = std::to_string((float)round(adec * 100) / 100);
                mods.bValue = mods.bValue.substr(0, 3);
                mods.cValue = std::to_string((float)round(asus * 100) / 100);
                mods.cValue = mods.cValue.substr(0, 3);
                mods.dValue = std::to_string((float)round(arel * 100) / 100);
                mods.dValue = mods.dValue.substr(0, 3);
            }

            break;

        case 5: // OUTPUT
            {
                mods.isAbleToStepModB = true;
                mods.isAbleToStepModD = true;
                
                mods.aName = "level";
                mods.bName = "pan";
                mods.cName = "choke";
                mods.dName = "delay"; // fx send?

                auto lvl = getValueNormalizedAsFloat(activeKit.sounds[currentSelectedTrackNum].params[MSMP_LEVEL]);
                auto pan = getValueNormalizedAsFloat(activeKit.sounds[currentSelectedTrackNum].params[MSMP_PAN]);
                auto chk = getValueNormalizedAsInt8(activeKit.sounds[currentSelectedTrackNum].params[MSMP_CHOKE]);
                auto dly = getValueNormalizedAsFloat(activeKit.sounds[currentSelectedTrackNum].params[MSMP_DELAY]);

                mods.aValue = std::to_string((int16_t)round(lvl * 100));
                mods.bValue = std::to_string((float)round(pan * 100) / 100);
                mods.bValue = mods.bValue.substr(0, 3);
                mods.bFloatValue = pan;
                mods.bType = RANGE;

                mods.cValue = chk > -1 ? std::to_string(chk+1) : "--";
                mods.dValue = std::to_string((int16_t)round(dly * 100));
            }

            break;

        default:
            break;
        }

        return mods;
    }

    SOUND_CONTROL_MODS getMonoSynthControlModData()
    {
        SOUND_CONTROL_MODS mods;

        auto &currentSelectedTrackLayer = XRSequencer::activeTrackLayer;
        auto &currentSelectedTrack = XRSequencer::getCurrentSelectedTrack();
        auto currentSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto currSelectedStep = XRSequencer::getCurrentSelectedStepNum();
        auto currentUXMode = XRUX::getCurrentMode();

        auto currentSoundForTrack = activeKit.sounds[currentSelectedTrackNum];

        mods.isAbleToStepModA = false;
        mods.isAbleToStepModB = false;
        mods.isAbleToStepModC = false;
        mods.isAbleToStepModD = false;

        mods.isActiveStepModA = false;
        mods.isActiveStepModB = false;
        mods.isActiveStepModC = false;
        mods.isActiveStepModD = false;

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            {
                mods.isAbleToStepModB = true;
                mods.isAbleToStepModC = true;
                mods.isAbleToStepModD = true;

                mods.aName = "lstep";
                mods.bName = "len";
                mods.cName = "velo";
                mods.dName = "prob";

                auto len = currentSelectedTrack.length;
                auto prob = currentSelectedTrack.probability;

                if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStep > -1) {
                    if (currentSelectedTrackLayer.tracks[currentSelectedTrackNum].steps[currSelectedStep].tFlags[XRSequencer::LENGTH]) {
                        mods.isAbleToStepModB = true;
                        len = currentSelectedTrackLayer.tracks[currentSelectedTrackNum].steps[currSelectedStep].tMods[XRSequencer::LENGTH];
                    }
                    if (currentSelectedTrackLayer.tracks[currentSelectedTrackNum].steps[currSelectedStep].tFlags[XRSequencer::PROBABILITY]) {
                        mods.isAbleToStepModD = true;
                        prob = currentSelectedTrackLayer.tracks[currentSelectedTrackNum].steps[currSelectedStep].tMods[XRSequencer::PROBABILITY];
                    }
                }

                mods.aValue = std::to_string(currentSelectedTrack.lstep);
                mods.bValue = std::to_string(len);
                mods.cValue = std::to_string(currentSelectedTrack.velocity);
                mods.dValue = std::to_string(prob) + "%";
            }

            break;

        case 1: // OSC
            {
                mods.isAbleToStepModA = true;
                mods.isAbleToStepModB = true;
                mods.isAbleToStepModC = true;
                mods.isAbleToStepModD = true;

                mods.aName = "wave";
                mods.bName = "det";
                mods.cName = "fine";
                mods.dName = "wid";

                auto waveform = getValueNormalizedAsUInt8(currentSoundForTrack.params[MSYN_WAVE]);
                auto detune = getValueNormalizedAsInt8(currentSoundForTrack.params[MSYN_DETUNE]);
                auto fine = getValueNormalizedAsInt8(currentSoundForTrack.params[MSYN_FINE]);
                auto width = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_WIDTH]);

                if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStep > -1) {
                    if (currentSelectedTrack.steps[currSelectedStep].sFlags[XRSound::MSYN_WAVE]) {
                        mods.isActiveStepModA = true;
                        waveform = getWaveformNumber(
                            getValueNormalizedAsUInt8(
                                currentSelectedTrack.steps[currSelectedStep].sMods[XRSound::MSYN_WAVE]
                            )
                        );
                    }
                }

                mods.aValue = getWaveformName(waveform);
                mods.bValue = std::to_string(detune);
                mods.cValue = std::to_string(fine);
                mods.dValue = std::to_string((float)round(width * 100) / 100);
                mods.dValue = mods.dValue.substr(0, 4);
            }

            break;

        case 2: // FILTER
            {
                mods.isAbleToStepModA = true;
                mods.isAbleToStepModB = true;
                mods.isAbleToStepModC = true;
                mods.isAbleToStepModD = true;

                mods.aName = "noise";
                mods.bName = "freq";
                mods.cName = "reso";
                mods.dName = "amt";

                auto noise = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_NOISE]);
                auto cutoff = getValueNormalizedAsInt32(currentSoundForTrack.params[MSYN_CUTOFF]);
                auto res = getValueNormalizedAsInt8(currentSoundForTrack.params[MSYN_RESONANCE]);
                auto fenv = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_FILTER_ENV_AMT]);

                mods.aValue = std::to_string((float)round(noise * 100) / 100);
                mods.aValue = mods.aValue.substr(0, 3);

                mods.bValue = std::to_string(round(cutoff));
                mods.bValue = mods.bValue.substr(0, 5);

                mods.cValue = std::to_string((float)round(res * 100) / 100);
                mods.cValue = mods.cValue.substr(0, 4);

                mods.dValue = std::to_string((float)round(fenv * 100) / 100);
                mods.dValue = mods.dValue.substr(0, 4);
            }

            break;

        case 3: // FILTER ENV
            {
                mods.aName = "att";
                mods.bName = "dec";
                mods.cName = "sus";
                mods.dName = "rel";

                auto fatt = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_FILTER_ATTACK]);
                auto fdec = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_FILTER_DECAY]);
                auto fsus = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_FILTER_SUSTAIN]);
                auto frel = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_FILTER_RELEASE]);

                mods.aValue = std::to_string((float)round(fatt * 100) / 100);
                mods.aValue = mods.aValue.substr(0, 3);

                mods.bValue = std::to_string((float)round(fdec * 100) / 100);
                mods.bValue = mods.bValue.substr(0, 5);

                mods.cValue = std::to_string((float)round(fsus * 100) / 100);
                mods.cValue = mods.cValue.substr(0, 3);

                mods.dValue = std::to_string((float)round(frel * 100) / 100);
                mods.dValue = mods.dValue.substr(0, 5);
            }

            break;

        case 4: // AMP ENV
            {
                mods.aName = "att";
                mods.bName = "dec";
                mods.cName = "sus";
                mods.dName = "rel";

                auto aatt = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_AMP_ATTACK]);
                auto adec = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_AMP_DECAY]);
                auto asus = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_AMP_SUSTAIN]);
                auto arel = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_AMP_RELEASE]);

                mods.aValue = std::to_string((float)round(aatt * 100) / 100);
                mods.aValue = mods.aValue.substr(0, 3);

                mods.bValue = std::to_string((float)round(adec * 100) / 100);
                mods.bValue = mods.bValue.substr(0, 5);

                mods.cValue = std::to_string((float)round(asus * 100) / 100);
                mods.cValue = mods.cValue.substr(0, 3);

                mods.dValue = std::to_string((float)round(arel * 100) / 100);
                mods.dValue = mods.dValue.substr(0, 5);
            }

            break;

        case 5: // OUTPUT
            {
                mods.isAbleToStepModB = true;
                mods.isAbleToStepModD = true;

                mods.aName = "lvl";
                mods.bName = "pan";
                mods.cName = "--"; // choke is unused on synth tracks
                mods.dName = "dly";

                auto lvl = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_LEVEL]);
                auto pan = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_PAN]);
                auto dly = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_DELAY]);

                mods.aValue = std::to_string((int16_t)round(lvl * 100));
                mods.bValue = std::to_string((float)round(pan * 100) / 100);
                mods.bValue = mods.bValue.substr(0, 3);
                mods.cValue = "--";
                mods.dValue = std::to_string((int16_t)round(dly * 100));

                mods.bFloatValue = pan;
                mods.bType = RANGE;
            }

            break;

        default:
            break;
        }

        return mods;
    }

    SOUND_CONTROL_MODS getDexedSynthControlModData()
    {
        SOUND_CONTROL_MODS mods;

        auto &currentSelectedTrackLayer = XRSequencer::activeTrackLayer;
        auto &currentSelectedTrack = XRSequencer::getCurrentSelectedTrack();
        auto currentSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto currSelectedStep = XRSequencer::getCurrentSelectedStepNum();
        auto currentUXMode = XRUX::getCurrentMode();

        mods.isAbleToStepModA = false;
        mods.isAbleToStepModB = false;
        mods.isAbleToStepModC = false;
        mods.isAbleToStepModD = false;

        mods.isActiveStepModA = false;
        mods.isActiveStepModB = false;
        mods.isActiveStepModC = false;
        mods.isActiveStepModD = false;

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            {
                mods.isAbleToStepModB = true;
                mods.isAbleToStepModC = true;
                mods.isAbleToStepModD = true;

                mods.aName = "lstep";
                mods.bName = "len";
                mods.cName = "velo";
                mods.dName = "prob";

                auto len = currentSelectedTrack.length;
                auto prob = currentSelectedTrack.probability;

                if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStep > -1) {
                    if (currentSelectedTrack.steps[currSelectedStep].tFlags[XRSequencer::LENGTH]) {
                        mods.isActiveStepModB = true;
                        len = currentSelectedTrack.steps[currSelectedStep].tMods[XRSequencer::LENGTH];
                    }
                    if (currentSelectedTrack.steps[currSelectedStep].tFlags[XRSequencer::PROBABILITY]) {
                        mods.isActiveStepModD = true;
                        prob = currentSelectedTrack.steps[currSelectedStep].tMods[XRSequencer::PROBABILITY];
                    }
                }

                mods.aValue = std::to_string(currentSelectedTrack.lstep);
                mods.bValue = std::to_string(len);
                mods.cValue = std::to_string(currentSelectedTrack.velocity);
                mods.dValue = std::to_string(prob) + "%";
            }

            break;

        case 1: // FM1
            {
                mods.isAbleToStepModA = true;
                mods.isAbleToStepModB = true;

                auto transpose = getValueNormalizedAsInt32(activeKit.sounds[currentSelectedTrackNum].params[DEXE_TRANSPOSE]);
                auto algo = getValueNormalizedAsInt32(activeKit.sounds[currentSelectedTrackNum].params[DEXE_ALGO]);

                // dexedInstances[0].dexed.setModWheel();

                // dexedInstances[0].dexed.setPitchRate(0, 99); // increase all PITCH EG rates (R1-R4) at once?
                // dexedInstances[0].dexed.setPitchRate(1, 99); // increase all PITCH EG rates (R1-R4) at once?
                // dexedInstances[0].dexed.setPitchRate(2, 99); // increase all PITCH EG rates (R1-R4) at once?
                // dexedInstances[0].dexed.setPitchRate(3, 99); // increase all PITCH EG rates (R1-R4) at once?

                // dexedInstances[0].dexed.setPitchLevel(0, 99); // increase all PITCH EG levels (L1-L4) at once?
                // dexedInstances[0].dexed.setPitchLevel(1, 99); // increase all PITCH EG levels (L1-L4) at once?
                // dexedInstances[0].dexed.setPitchLevel(2, 99); // increase all PITCH EG levels (L1-L4) at once?
                // dexedInstances[0].dexed.setPitchLevel(3, 99); // increase all PITCH EG levels (L1-L4) at once?

                // dexedInstances[0].dexed.setOPLevel(0,0,99); // increase all OP EG levels (L1-L4) at once?
                // dexedInstances[0].dexed.setOPLevel(0,1,99); // increase all OP EG levels (L1-L4) at once?
                // dexedInstances[0].dexed.setOPLevel(0,2,99); // increase all OP EG levels (L1-L4) at once?
                // dexedInstances[0].dexed.setOPLevel(0,3,99); // increase all OP EG levels (L1-L4) at once?

                // dexedInstances[0].dexed.setOPRate(0,0,99); // increase all OP EG rates (R1-R4) at once?
                // dexedInstances[0].dexed.setOPRate(0,1,99); // increase all OP EG rates (R1-R4) at once?
                // dexedInstances[0].dexed.setOPRate(0,2,99); // increase all OP EG rates (R1-R4) at once?
                // dexedInstances[0].dexed.setOPRate(0,3,99); // increase all OP EG rates (R1-R4) at once?

                mods.aName = "trns";
                mods.bName = "algo";
                mods.cName = "--";
                mods.dName = "--";

                mods.aValue = std::to_string(transpose);
                mods.bValue = std::to_string(algo + 1);
                mods.cValue = "--";
                mods.dValue = "--";
            }
            
            break;

        case 2: // FM2
            {
                mods.aName = "--";
                mods.bName = "--";
                mods.cName = "--";
                mods.dName = "--";

                mods.aValue = "--";
                mods.bValue = "--";
                mods.cValue = "--";
                mods.dValue = "--";
            }

            break;

        case 3: // POLY
            {
                mods.isAbleToStepModB = true;
                mods.isAbleToStepModC = true;
                mods.isAbleToStepModD = true;

                mods.aName = "mode";
                mods.bName = "note2";
                mods.cName = "note3";
                mods.dName = "note4";

                auto noteMode = getValueNormalizedAsInt8(activeKit.sounds[currentSelectedTrackNum].params[DEXE_NOTE_MODE]);

                auto noteB = getValueNormalizedAsInt8(activeKit.sounds[currentSelectedTrackNum].params[DEXE_NOTE_B]);
                auto noteC = getValueNormalizedAsInt8(activeKit.sounds[currentSelectedTrackNum].params[DEXE_NOTE_C]);
                auto noteD = getValueNormalizedAsInt8(activeKit.sounds[currentSelectedTrackNum].params[DEXE_NOTE_D]);

                if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStep > -1) {
                    if (currentSelectedTrack.steps[currSelectedStep].sFlags[XRSound::DEXE_NOTE_B]) {
                        mods.isActiveStepModB = true;
                        noteB = getValueNormalizedAsInt8(
                            currentSelectedTrack.steps[currSelectedStep].sMods[XRSound::DEXE_NOTE_B]
                        );
                    }
                    if (currentSelectedTrack.steps[currSelectedStep].sFlags[XRSound::DEXE_NOTE_C]) {
                        mods.isActiveStepModC = true;
                        noteC = getValueNormalizedAsInt8(
                            currentSelectedTrack.steps[currSelectedStep].sMods[XRSound::DEXE_NOTE_C]
                        );
                    }
                    if (currentSelectedTrack.steps[currSelectedStep].sFlags[XRSound::DEXE_NOTE_D]) {
                        mods.isActiveStepModD = true;
                        noteD = getValueNormalizedAsInt8(
                            currentSelectedTrack.steps[currSelectedStep].sMods[XRSound::DEXE_NOTE_D]
                        );
                    }
                }

                mods.aValue = (noteMode > 0) ? "poly" : "mono";
                mods.bValue = (noteB != 0) ? std::to_string(noteB) : "--";
                mods.cValue = (noteC != 0) ? std::to_string(noteC) : "--";
                mods.dValue = (noteD != 0) ? std::to_string(noteD) : "--";
            }

            break;

        case 4: // OUTPUT
            {
                mods.isAbleToStepModB = true;
                mods.isAbleToStepModD = true;

                mods.aName = "level";
                mods.bName = "pan";
                mods.cName = "--"; // choke is unused on synth tracks
                mods.dName = "dly";

                auto lvl = getValueNormalizedAsFloat(activeKit.sounds[currentSelectedTrackNum].params[DEXE_LEVEL]);
                auto pan = getValueNormalizedAsFloat(activeKit.sounds[currentSelectedTrackNum].params[DEXE_PAN]);
                auto dly = getValueNormalizedAsFloat(activeKit.sounds[currentSelectedTrackNum].params[DEXE_DELAY]);

                mods.aValue = std::to_string((int16_t)round(lvl * 100));
                mods.bValue = std::to_string((float)round(pan * 100) / 100);
                mods.bValue = mods.bValue.substr(0, 3);
                mods.bFloatValue = pan;
                mods.bType = RANGE;

                mods.cValue = "--";
                mods.dValue = std::to_string((int16_t)round(dly * 100));
            }

            break;

        default:
            break;
        }

        return mods;
    }

    SOUND_CONTROL_MODS getFmDrumControlModData()
    {
        SOUND_CONTROL_MODS mods;

        // auto &currentSelectedTrackLayer = XRSequencer::activeTrackLayer;
        // auto &currentSelectedTrack = XRSequencer::getCurrentSelectedTrack();
        // auto currentSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        // auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        // auto currSelectedStep = XRSequencer::getCurrentSelectedStepNum();
        // auto currentUXMode = XRUX::getCurrentMode();

        // mods.isAbleToStepModA = false;
        // mods.isAbleToStepModB = false;
        // mods.isAbleToStepModC = false;
        // mods.isAbleToStepModD = false;

        // mods.isActiveStepModA = false;
        // mods.isActiveStepModB = false;
        // mods.isActiveStepModC = false;
        // mods.isActiveStepModD = false;

        // auto freq = getValueNormalizedAsUInt32(activeKit.sounds[currentSelectedTrackNum].params[FMD_FREQ]);
        // auto fm = getValueNormalizedAsFloat(activeKit.sounds[currentSelectedTrackNum].params[FMD_FM]);
        // auto dec = getValueNormalizedAsFloat(activeKit.sounds[currentSelectedTrackNum].params[FMD_DECAY]);
        // auto nse = getValueNormalizedAsFloat(activeKit.sounds[currentSelectedTrackNum].params[FMD_NOISE]);

        // switch (currentSelectedPageNum)
        // {
        // case 0: // MAIN
        //     {
        //         mods.isAbleToStepModC = true;
        //         mods.isAbleToStepModD = true;

        //         mods.aName = "lstep";
        //         mods.bName = "--";
        //         mods.cName = "velo";
        //         mods.dName = "prob";

        //         auto prob = currentSelectedTrack.probability;

        //         if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStep > -1) {
        //             if (currentSelectedTrackLayer.tracks[currentSelectedTrackNum].steps[currSelectedStep].tFlags[XRSequencer::PROBABILITY]) {
        //                 mods.isActiveStepModD = true;
        //                 prob = currentSelectedTrackLayer.tracks[currentSelectedTrackNum].steps[currSelectedStep].tMods[XRSequencer::PROBABILITY];
        //             }
        //         }

        //         mods.aValue = std::to_string(currentSelectedTrack.lstep);
        //         mods.bValue = "--";
        //         mods.cValue = std::to_string(currentSelectedTrack.velocity);
        //         mods.dValue = std::to_string(prob) + "%";
        //     }

        //     break;

        // case 1: // DRUM
        //     {
        //         mods.isAbleToStepModA = true;
        //         mods.isAbleToStepModB = true;
        //         mods.isAbleToStepModC = true;
        //         mods.isAbleToStepModD = true;

        //         mods.aName = "tune";
        //         mods.bName = "fm";
        //         mods.cName = "decay";
        //         mods.dName = "noise";

        //         mods.aValue = std::to_string(freq);
        //         mods.aValue += "hz";
        //         mods.bValue = std::to_string((float)round(fm * 100) / 100);
        //         mods.bValue = mods.bValue.substr(0, 4);
        //         mods.cValue = std::to_string((float)round(dec * 100) / 100);
        //         mods.cValue = mods.cValue.substr(0, 4);
        //         mods.dValue = std::to_string((float)round(nse * 100) / 100);
        //         mods.dValue = mods.dValue.substr(0, 4);
        //     }

        //     break;

        // case 2: // OUTPUT
        //     {
        //         mods.isAbleToStepModB = true;
        //         mods.isAbleToStepModD = true;

        //         mods.aName = "level";
        //         mods.bName = "pan";
        //         mods.cName = "choke";
        //         mods.dName = "delay";

        //         auto lvl = getValueNormalizedAsFloat(activeKit.sounds[currentSelectedTrackNum].params[FMD_LEVEL]);
        //         auto pan = getValueNormalizedAsFloat(activeKit.sounds[currentSelectedTrackNum].params[FMD_PAN]);
        //         auto chk = getValueNormalizedAsInt8(activeKit.sounds[currentSelectedTrackNum].params[FMD_CHOKE]);
        //         auto dly = getValueNormalizedAsFloat(activeKit.sounds[currentSelectedTrackNum].params[FMD_DELAY]);

        //         mods.aValue = std::to_string(round(lvl * 100));
        //         mods.bValue = std::to_string((float)round(pan * 100) / 100);
        //         mods.bValue = mods.bValue.substr(0, 3);
        //         mods.bFloatValue = pan;
        //         mods.bType = RANGE;

        //         mods.cValue = chk > -1 ? std::to_string(chk+1) : "--";
        //         mods.dValue = std::to_string(round(dly * 100));
        //     }
            
        //     break;

        // default:
        //     break;
        // }

        return mods;
    }

    SOUND_CONTROL_MODS getMidiControlModData()
    {
        SOUND_CONTROL_MODS mods;

        auto &currentSelectedTrack = XRSequencer::getCurrentSelectedTrack();
        auto currentSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto currentSoundForTrack = activeKit.sounds[currentSelectedTrackNum];

        mods.isAbleToStepModA = false;
        mods.isAbleToStepModB = false;
        mods.isAbleToStepModC = false;
        mods.isAbleToStepModD = false;

        mods.isActiveStepModA = false;
        mods.isActiveStepModB = false;
        mods.isActiveStepModC = false;
        mods.isActiveStepModD = false;

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            {
                mods.aName = "lstep";
                mods.bName = "len";
                mods.cName = "chan";
                mods.dName = "velo";

                auto chan = getValueNormalizedAsInt8(currentSoundForTrack.params[0]); // TODO make enum

                mods.aValue = std::to_string(currentSelectedTrack.lstep);
                mods.bValue = std::to_string(currentSelectedTrack.length);    // TODO: impl
                mods.cValue = std::to_string(chan);                           // TODO: impl
                mods.dValue = std::to_string(currentSelectedTrack.velocity);  // TODO: impl
            }
            
            break;

        default:
            break;
        }

        return mods;
    }

    SOUND_CONTROL_MODS getCvGateControlModData()
    {
        SOUND_CONTROL_MODS mods;

        auto &currentSelectedTrack = XRSequencer::getCurrentSelectedTrack();
        auto currentSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto currentSoundForTrack = activeKit.sounds[currentSelectedTrackNum];

        mods.isAbleToStepModA = false;
        mods.isAbleToStepModB = false;
        mods.isAbleToStepModC = false;
        mods.isAbleToStepModD = false;

        mods.isActiveStepModA = false;
        mods.isActiveStepModB = false;
        mods.isActiveStepModC = false;
        mods.isActiveStepModD = false;

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
        {
            mods.aName = "lstep";
            mods.bName = "len";
            mods.cName = "out";
            mods.dName = "prob";

            mods.aValue = std::to_string(currentSelectedTrack.lstep);
            mods.bValue = std::to_string(currentSelectedTrack.length);

            auto port = getValueNormalizedAsInt8(currentSoundForTrack.params[0]); // TODO make enum

            std::string outputPortStr = std::to_string(port);
            outputPortStr += "ab";

            mods.cValue = outputPortStr;
            mods.dValue = "100%";
        }
        break;

        default:
            break;
        }

        return mods;
    }

    SOUND_CONTROL_MODS getCvTrigControlModData()
    {
        SOUND_CONTROL_MODS mods;

        auto &currentSelectedTrack = XRSequencer::getCurrentSelectedTrack();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        mods.isAbleToStepModA = false;
        mods.isAbleToStepModB = false;
        mods.isAbleToStepModC = false;
        mods.isAbleToStepModD = false;

        mods.isActiveStepModA = false;
        mods.isActiveStepModB = false;
        mods.isActiveStepModC = false;
        mods.isActiveStepModD = false;

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            {
                mods.aName = "lstep";
                mods.bName = "out";
                mods.cName = "prob";
                mods.dName = "--";

                mods.aValue = std::to_string(currentSelectedTrack.lstep);
                mods.bValue = "1ab";  // TODO: impl
                mods.cValue = "100%"; // TODO: impl
                mods.dValue = "--";   // TODO: impl
            }
            
            break;

        default:
            break;
        }

        return mods;
    }
    
    SOUND_CONTROL_MODS getEmptyControlModData()
    {
        SOUND_CONTROL_MODS mods;

        mods.isAbleToStepModA = false;
        mods.isAbleToStepModB = false;
        mods.isAbleToStepModC = false;
        mods.isAbleToStepModD = false;

        mods.isActiveStepModA = false;
        mods.isActiveStepModB = false;
        mods.isActiveStepModC = false;
        mods.isActiveStepModD = false;

        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            {
                mods.aName = "--";
                mods.bName = "--";
                mods.cName = "--";
                mods.dName = "--";

                mods.aValue = "--";
                mods.bValue = "--";
                mods.cValue = "--";
                mods.dValue = "--";
            }
            
            break;

        default:
            break;
        }

        return mods;
    }

    void handleMonoSampleNoteOnForTrack(int track)
    {
        auto &trackToUse = XRSequencer::getTrack(track);

        applyChokeForSourceTrack(track);

        auto msmpLooptype = getValueNormalizedAsUInt8(activeKit.sounds[track].params[MSMP_LOOPTYPE]);
        auto msmpLoopstart = getValueNormalizedAsInt32(activeKit.sounds[track].params[MSMP_LOOPSTART]);
        auto msmpLoopfinish = getValueNormalizedAsInt32(activeKit.sounds[track].params[MSMP_LOOPFINISH]);
        auto msmpPlaystart = (play_start)getValueNormalizedAsUInt8(activeKit.sounds[track].params[MSMP_PLAYSTART]);
        auto msmpAatt = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSMP_AMP_ATTACK]);
        auto msmpAdec = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSMP_AMP_DECAY]);
        auto msmpAsus = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSMP_AMP_SUSTAIN]);
        auto msmpArel = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSMP_AMP_RELEASE]);
        auto msmpPan = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSMP_PAN]);
        auto msmpLvl = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSMP_LEVEL]);
        auto msmpDly = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSMP_DELAY]);

        auto velocityToUse = trackToUse.velocity;
        auto octaveToUse = trackToUse.octave;
        auto noteToUse = trackToUse.note;

        AudioNoInterrupts();

        monoSampleInstances[track].ampEnv.attack(msmpAatt);
        monoSampleInstances[track].ampEnv.decay(msmpAdec);
        monoSampleInstances[track].ampEnv.sustain(msmpAsus);
        monoSampleInstances[track].ampEnv.release(msmpArel);
        monoSampleInstances[track].ampAccent.gain(velocityToUse * 0.01);
        monoSampleInstances[track].amp.gain(msmpLvl);
        monoSampleInstances[track].ampDelaySend.gain(msmpDly);
        monoSampleInstances[track].left.gain(getStereoPanValues(msmpPan).left);
        monoSampleInstances[track].right.gain(getStereoPanValues(msmpPan).right);

        const float semi = powf(2.0f, 1.0f/12);
        float freq = powf(semi, 12*(octaveToUse - 4) + noteToUse);
        
        monoSampleInstances[track].sample.setPlaybackRate(freq);

        AudioInterrupts();

        monoSampleInstances[track].ampEnv.noteOn();

        std::string trackSampleName(activeKit.sounds[track].sampleName);

        if (trackSampleName.length() > 0) {
            const auto sampleName = std::string("/samples/") + std::string(trackSampleName);

#ifdef USE_WAV
            monoSampleInstances[track].sample.playWav(sampleName.c_str());
#else
            monoSampleInstances[track].sample.playRaw(sampleName.c_str(), 1);
#endif
        }

        AudioNoInterrupts();

        // always re-initialize loop type
        monoSampleInstances[track].sample.setLoopType(loopTypeSelMap[msmpLooptype]);

        if (loopTypeSelMap[msmpLooptype] == looptype_none)
        {
            monoSampleInstances[track].sample.setPlayStart(play_start::play_start_sample);
            monoSampleInstances[track].sample.setLoopType(loop_type::looptype_none);
        }
        else if (loopTypeSelMap[msmpLooptype] == looptype_repeat)
        {
            monoSampleInstances[track].sample.setPlayStart(msmpPlaystart == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
            monoSampleInstances[track].sample.setLoopStart(msmpLoopstart);
            monoSampleInstances[track].sample.setLoopFinish(msmpLoopfinish);
        }

        AudioInterrupts();
    }

    void handleMonoSynthNoteOnForTrack(int track)
    {
        if (track > 3) return;

        auto &trackToUse = XRSequencer::getTrack(track);
       
        auto msynWave = getValueNormalizedAsInt8(activeKit.sounds[track].params[MSYN_WAVE]);
        auto msynFine = getValueNormalizedAsInt8(activeKit.sounds[track].params[MSYN_FINE]);
        auto msynDetune = getValueNormalizedAsInt8(activeKit.sounds[track].params[MSYN_DETUNE]);
        auto msynFatt = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_FILTER_ATTACK]);
        auto msynFdec = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_FILTER_DECAY]);
        auto msynFsus = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_FILTER_SUSTAIN]);
        auto msynAatt = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_AMP_ATTACK]);
        auto msynAdec = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_AMP_DECAY]);
        auto msynAsus = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_AMP_SUSTAIN]);
        auto msynArel = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_AMP_RELEASE]);
        auto msynPan = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_PAN]);
        auto msynLvl = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_LEVEL]);
        auto msynDly = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_DELAY]);

        auto velocityToUse = trackToUse.velocity;
        auto octaveToUse = trackToUse.octave;
        auto noteToUse = trackToUse.note;

        float foundBaseFreq = _noteToFreqArr[noteToUse];
        float octaveFreqA = (foundBaseFreq + (msynFine * 0.01)) * (pow(2, velocityToUse));
        float octaveFreqB = (foundBaseFreq * pow(2.0, (float)msynDetune / 12.0)) * (pow(2, octaveToUse));
        
        AudioNoInterrupts();

        monoSynthInstances[track].oscA.begin(msynWave);
        monoSynthInstances[track].oscB.begin(msynWave);
        monoSynthInstances[track].oscA.frequency(octaveFreqA);
        monoSynthInstances[track].oscB.frequency(octaveFreqB);

        monoSynthInstances[track].filterEnv.attack(msynFatt);
        monoSynthInstances[track].filterEnv.decay(msynFdec);
        monoSynthInstances[track].filterEnv.sustain(msynFsus);
        monoSynthInstances[track].filterEnv.release(msynArel);
        monoSynthInstances[track].ampEnv.attack(msynAatt);
        monoSynthInstances[track].ampEnv.decay(msynAdec);
        monoSynthInstances[track].ampEnv.sustain(msynAsus);
        monoSynthInstances[track].ampEnv.release(msynArel);

        monoSynthInstances[track].filterAccent.gain((velocityToUse * 0.01));
        monoSynthInstances[track].ampAccent.gain((velocityToUse * 0.01));
        //monoSynthInstances[track].amp.gain(msynLvl * (trackToUse.velocity * 0.01));
        monoSynthInstances[track].amp.gain(msynLvl);
        monoSynthInstances[track].ampDelaySend.gain(msynDly);

        monoSynthInstances[track].left.gain(getStereoPanValues(msynPan).left);
        monoSynthInstances[track].right.gain(getStereoPanValues(msynPan).right);
        
        AudioInterrupts();

        // now trigger envs
        monoSynthInstances[track].ampEnv.noteOn();
        monoSynthInstances[track].filterEnv.noteOn();
    }

    void handleDexedSynthNoteOnForTrack(int track)
    {
        if (track > 3) return;

        auto &trackToUse = XRSequencer::getTrack(track);

        auto dexeLvl = getValueNormalizedAsFloat(activeKit.sounds[track].params[DEXE_LEVEL]);
        auto dexeDly = getValueNormalizedAsFloat(activeKit.sounds[track].params[DEXE_DELAY]);

        auto velocityToUse = trackToUse.velocity;
        auto octaveToUse = trackToUse.octave;
        auto noteToUse = trackToUse.note;

        int midiNote = (noteToUse + (12 * (octaveToUse)));

        auto di = XRDexedManager::getActiveInstanceForTrack(track);

        dexedInstances[di].ampAccent.gain((velocityToUse * 0.01));
        dexedInstances[di].amp.gain(dexeLvl);
        dexedInstances[di].ampDelaySend.gain(0);

        dexedInstances[di].dexed.keydown(midiNote, 50); // TODO: parameterize velocity
    }

    void handleBraidsNoteOnForTrack(int track)
    {
        // auto &trackToUse = XRSequencer::getTrack(track);

        // uint8_t noteToUse = trackToUse.note;
        // uint8_t octaveToUse = trackToUse.octave;

        // int midiNote = (noteToUse + (12 * (octaveToUse)));

        // braidsInstances[track].braids.set_braids_pitch(midiNote << 7);
    }

    void handleFmDrumNoteOnForTrack(int track)
    {
        // if (track > 2) return;

        // auto &trackToUse = XRSequencer::getTrack(track);

        // auto fmDrumLvl = getValueNormalizedAsFloat(activeKit.sounds[track].params[FMD_LEVEL]);
        // auto fmDrumDly = getValueNormalizedAsFloat(activeKit.sounds[track].params[FMD_DELAY]);

        // auto velocityToUse = trackToUse.velocity;

        // fmDrumInstances[track].ampAccent.gain((velocityToUse * 0.01));
        // fmDrumInstances[track].amp.gain(fmDrumLvl);
        // fmDrumInstances[track].ampDelaySend.gain(0);

        // fmDrumInstances[track].fmDrum.noteOn();
    }

    void handleMIDINoteOnForTrack(int track)
    {
        // TODO: impl
    }

    void handleCvGateNoteOnForTrack(int track)
    {
        // TODO: impl
    }

    bool applyChokeForDestinationTrackStep(int track, int step)
    {
        // if a choke source track is mapped to this destination track,
        // and if the choke source track is ON / ACCENTED for current step, 
        // and also not being ignored/disabled due to probability
        // then return true so it cna be used to choke the destination track sound

        auto &activeTrackLayer = XRSequencer::activeTrackLayer; 

        auto chokeDestSrcTrk = chokeDestSource[track] > -1 ? chokeDestSource[track] : -1;
        auto chokeSrcDestTrk = chokeSourceDest[chokeDestSrcTrk] > -1 ? chokeSourceDest[chokeDestSrcTrk] : -1;

        auto chokeDestSrcIgnored = XRSequencer::isTrackStepBeingIgnored(chokeDestSrcTrk, step);
        if (chokeDestSrcIgnored) {
            return false;
        }

        if (
            chokeDestSrcTrk > -1 && chokeSrcDestTrk == track &&
            (activeTrackLayer.tracks[chokeDestSrcTrk].steps[step].state == XRSequencer::STEP_STATE::STATE_ON ||
            activeTrackLayer.tracks[chokeDestSrcTrk].steps[step].state == XRSequencer::STEP_STATE::STATE_ACCENTED)
        ) {
            if (activeKit.sounds[track].type == T_MONO_SAMPLE) {
                monoSampleInstances[track].amp.gain(0);
                monoSampleInstances[track].ampDelaySend.gain(0);
            } else if (activeKit.sounds[track].type == T_MONO_SYNTH) {
                monoSynthInstances[track].amp.gain(0);
                monoSynthInstances[track].ampDelaySend.gain(0);
            } else if (activeKit.sounds[track].type == T_DEXED_SYNTH) {
                auto di = XRDexedManager::getActiveInstanceForTrack(track);

                dexedInstances[di].amp.gain(0);
                dexedInstances[di].ampDelaySend.gain(0);
            } 
            // else if (activeKit.sounds[track].type == T_FM_DRUM) {
            //     fmDrumInstances[track].amp.gain(0);
            //     fmDrumInstances[track].ampDelaySend.gain(0);
            // } 
            // else if (activeKit.sounds[track].type == T_BRAIDS_SYNTH) {
            //     braidsInstances[track].amp.gain(0);
            //     braidsInstances[track].ampDelaySend.gain(0);
            // }

            return true;
        }

        return false;
    }

    void applyChokeForSourceTrack(int track)
    {
        // if this track is a choke source track, then choke its destination track
        auto chokeEnabled = chokeSourcesEnabled[track] ? chokeSourcesEnabled[track] : false;
        auto chokeDestTrk = chokeSourceDest[track] > -1 ? chokeSourceDest[track] : -1;

        // TODO: extract to function
        if (chokeEnabled) {
            if (activeKit.sounds[chokeDestTrk].type == T_MONO_SAMPLE) {
                monoSampleInstances[chokeDestTrk].amp.gain(0);
                monoSampleInstances[chokeDestTrk].ampDelaySend.gain(0);
            } else if (activeKit.sounds[chokeDestTrk].type == T_MONO_SYNTH) {
                monoSynthInstances[chokeDestTrk].amp.gain(0);
                monoSynthInstances[chokeDestTrk].ampDelaySend.gain(0);
            } else if (activeKit.sounds[chokeDestTrk].type == T_DEXED_SYNTH) {
                auto di = XRDexedManager::getActiveInstanceForTrack(chokeDestTrk);

                dexedInstances[di].amp.gain(0);
                dexedInstances[di].ampDelaySend.gain(0);
            } 
            // else if (activeKit.sounds[chokeDestTrk].type == T_FM_DRUM) {
            //     fmDrumInstances[chokeDestTrk].amp.gain(0);
            //     fmDrumInstances[chokeDestTrk].ampDelaySend.gain(0);
            // } 
            // else if (activeKit.sounds[chokeDestTrk].type == T_BRAIDS_SYNTH) {
            //     braidsInstances[chokeDestTrk].amp.gain(0);
            //     braidsInstances[chokeDestTrk].ampDelaySend.gain(0);
            // }
        }
    }

    void handleMonoSampleNoteOnForTrackStep(int track, int step)
    {
        if (applyChokeForDestinationTrackStep(track, step)) return;

        // Serial.println("Mono Sample Note On");

        applyChokeForSourceTrack(track);

        auto &trackToUse = XRSequencer::getTrack(track);
        auto &stepToUse = XRSequencer::getStep(track, step);

        auto msmpSamplePlayRate = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSMP_SAMPLEPLAYRATE]);
        auto msmpLooptype = getValueNormalizedAsUInt8(activeKit.sounds[track].params[MSMP_LOOPTYPE]);
        auto msmpLoopstart = getValueNormalizedAsInt32(activeKit.sounds[track].params[MSMP_LOOPSTART]);
        auto msmpLoopfinish = getValueNormalizedAsInt32(activeKit.sounds[track].params[MSMP_LOOPFINISH]);
        auto msmpPlaystart = (play_start)getValueNormalizedAsUInt8(activeKit.sounds[track].params[MSMP_PLAYSTART]);
        auto msmpAatt = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSMP_AMP_ATTACK]);
        auto msmpAdec = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSMP_AMP_DECAY]);
        auto msmpAsus = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSMP_AMP_SUSTAIN]);
        auto msmpArel = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSMP_AMP_RELEASE]);
        auto msmpPan = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSMP_PAN]);
        auto msmpLvl = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSMP_LEVEL]);
        auto msmpDly = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSMP_DELAY]);

        // TODO: allow sample chromatic note playback

        std::string trackSampleName(activeKit.sounds[track].sampleName);
        std::string trackSampleNameB(activeKit.sounds[track].sampleNameB);
        bool hasFirstSample = trackSampleName.length() > 0;
        bool hasSecondSample = trackSampleNameB.length() > 0;

        auto velocityToUse = trackToUse.velocity;
        auto octaveToUse = trackToUse.octave;
        auto noteToUse = trackToUse.note;

        if (hasSecondSample) 
        {
            velocityToUse = 100;
        } else if (stepToUse.state == XRSequencer::STATE_ACCENTED) {
            velocityToUse = max(trackToUse.velocity, XRSequencer::activePatternSettings.accent);
        }

        if (stepToUse.tFlags[XRSequencer::NOTE])
        {
            noteToUse = stepToUse.tMods[XRSequencer::NOTE];
        }

        if (stepToUse.tFlags[XRSequencer::OCTAVE])
        {
            octaveToUse = stepToUse.tMods[XRSequencer::OCTAVE];
        }

        // is currently ratcheting and this is the ratcheting track
        if (track == XRSequencer::getRatchetTrack() && XRSequencer::getRatchetDivision() > -1){
            auto &ratchetTrack = XRSequencer::activeRatchetLayer.tracks[track];
            auto &ratchetTrackStep = XRSequencer::activeRatchetLayer.tracks[track].steps[step];

            velocityToUse = ratchetTrack.velocity;

            if (hasSecondSample) 
            {
                velocityToUse = 100;
            } else if (ratchetTrackStep.state == XRSequencer::STATE_ACCENTED) {
                velocityToUse = max(ratchetTrack.velocity, XRSequencer::activePatternSettings.accent);
            }

            octaveToUse = ratchetTrack.octave;
            noteToUse = ratchetTrack.note;

            if (ratchetTrackStep.tFlags[XRSequencer::NOTE])
            {
                noteToUse = ratchetTrackStep.tMods[XRSequencer::NOTE];
            }

            if (ratchetTrackStep.tFlags[XRSequencer::OCTAVE])
            {
                octaveToUse = ratchetTrackStep.tMods[XRSequencer::OCTAVE];
            }

            Serial.printf("Ratchet noteToUse: %d, octaveToUse: %d\n", noteToUse, octaveToUse);
        }

        // if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::VELOCITY])
        // {
        //     velocityToUse = patternMods.tracks[track].steps[step].velocity;
        // }
        // else
        // {
        //     velocityToUse = stepToUse.velocity;
        // }

        auto looptypeToUse = msmpLooptype;
        if (trackToUse.steps[step].sFlags[MSMP_LOOPTYPE])
        {
            looptypeToUse = getValueNormalizedAsUInt8(trackToUse.steps[step].sMods[MSMP_LOOPTYPE]);
        }

        auto loopstartToUse = msmpLoopstart;
        // if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::LOOPSTART])
        // {
        //     loopstartToUse = patternMods.tracks[track].steps[step].loopstart;
        // }

        auto loopfinishToUse = msmpLoopfinish;
        // if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::LOOPFINISH])
        // {
        //     loopfinishToUse = patternMods.tracks[track].steps[step].loopfinish;
        // }

        auto playstartToUse = msmpPlaystart;
        // if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::PLAYSTART])
        // {
        //     playstartToUse = patternMods.tracks[track].steps[step].playstart;
        // }

        float speedToUse = msmpSamplePlayRate;
        // if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::SAMPLE_PLAY_RATE])
        // {
        //     speedToUse = patternMods.tracks[track].steps[step].sample_play_rate;
        // }
        
        AudioNoInterrupts();

        //auto &sampleVoice = getNextFreeSampleVoiceForTrack(track, sampleToUse);
        //auto &sampleVoice = monoSampleInstances[track];

        monoSampleInstances[track].ampEnv.attack(msmpAatt);
        monoSampleInstances[track].ampEnv.decay(msmpAdec);
        monoSampleInstances[track].ampEnv.sustain(msmpAsus);
        monoSampleInstances[track].ampEnv.release(msmpArel);
        
        monoSampleInstances[track].ampAccent.gain(velocityToUse * 0.01);
        monoSampleInstances[track].amp.gain(msmpLvl);
        monoSampleInstances[track].ampDelaySend.gain(msmpDly);

        monoSampleInstances[track].left.gain(getStereoPanValues(msmpPan).left);
        monoSampleInstances[track].right.gain(getStereoPanValues(msmpPan).right);

        const float semi = powf(2.0f, 1.0f/12);
        float freq = powf(semi, 12*(octaveToUse - 4) + noteToUse);
        monoSampleInstances[track].sample.setPlaybackRate(freq * speedToUse);

        //monoSampleInstances[track].sample.setPlaybackRate(speedToUse);
        
        AudioInterrupts();

        std::string sampleToUse = hasFirstSample ? trackSampleName : "";
        if (hasSecondSample && stepToUse.state == XRSequencer::STATE_ACCENTED) 
        {
            sampleToUse = trackSampleNameB;
        }

        monoSampleInstances[track].ampEnv.noteOn();

        if (sampleToUse.length() > 0) {
            const auto sampleName = std::string("/samples/") + std::string(sampleToUse);

            // if (track == 4) Serial.printf("Sample: %s\n", sampleName.c_str());
            // if (track == 4) Serial.printf("Velocity: %d\n", velocityToUse);
            // if (track == 4) Serial.printf("LoopType: %d\n", looptypeToUse);
            // if (track == 4) Serial.printf("LoopStart: %d\n", loopstartToUse);
            // if (track == 4) Serial.printf("LoopFinish: %d\n", loopfinishToUse);
            // if (track == 4) Serial.printf("PlayStart: %d\n", playstartToUse);
            // if (track == 4) Serial.printf("Speed: %f\n", speedToUse);
            // if (track == 4) Serial.printf("Pan L: %f\n", getStereoPanValues(msmpPan).left);
            // if (track == 4) Serial.printf("Pan R: %f\n", getStereoPanValues(msmpPan).right);
            // if (track == 4) Serial.printf("Level: %f\n", msmpLvl);
            // if (track == 4) Serial.printf("Delay: %f\n", msmpDly);
            // if (track == 4) Serial.printf("Attack: %f\n", msmpAatt);
            // if (track == 4) Serial.printf("Decay: %f\n", msmpAdec);
            // if (track == 4) Serial.printf("Sustain: %f\n", msmpAsus);
            // if (track == 4) Serial.printf("Release: %f\n", msmpArel);

#ifdef USE_WAV
            monoSampleInstances[track].sample.playWav(sampleName.c_str());
#else
            monoSampleInstances[track].sample.playRaw(sampleName.c_str(), 1);
#endif
        }

        AudioNoInterrupts();

        // always re-initialize loop type
        monoSampleInstances[track].sample.setLoopType(loopTypeSelMap[looptypeToUse]);

        if (loopTypeSelMap[looptypeToUse] == looptype_none)
        {
            monoSampleInstances[track].sample.setPlayStart(play_start::play_start_sample);
            monoSampleInstances[track].sample.setLoopType(loop_type::looptype_none);
        }
        else if (loopTypeSelMap[looptypeToUse] == looptype_repeat)
        {
            monoSampleInstances[track].sample.setPlayStart(playstartToUse == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
            monoSampleInstances[track].sample.setLoopStart(loopstartToUse);
            monoSampleInstances[track].sample.setLoopFinish(loopfinishToUse);
        }
        
        AudioInterrupts();
    }

    void handleMonoSynthNoteOnForTrackStep(int track, int step)
    {
        if (track > 3) return;

        if (applyChokeForDestinationTrackStep(track, step)) return;

        applyChokeForSourceTrack(track);

        auto &trackToUse = XRSequencer::getTrack(track);
        auto &stepToUse = XRSequencer::getStep(track, step);

        auto msynWave = getValueNormalizedAsUInt8(activeKit.sounds[track].params[MSYN_WAVE]);
        auto msynFine = getValueNormalizedAsInt8(activeKit.sounds[track].params[MSYN_FINE]);
        auto msynDetune = getValueNormalizedAsInt8(activeKit.sounds[track].params[MSYN_DETUNE]);
        auto msynFatt = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_FILTER_ATTACK]);
        auto msynFdec = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_FILTER_DECAY]);
        auto msynFsus = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_FILTER_SUSTAIN]);
        auto msynFrel = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_FILTER_RELEASE]);
        auto msynAatt = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_AMP_ATTACK]);
        auto msynAdec = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_AMP_DECAY]);
        auto msynAsus = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_AMP_SUSTAIN]);
        auto msynArel = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_AMP_RELEASE]);
        auto msynPan = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_PAN]);
        auto msynLvl = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_LEVEL]);
        auto msynDly = getValueNormalizedAsFloat(activeKit.sounds[track].params[MSYN_DELAY]);

        auto velocityToUse = trackToUse.velocity;
        auto octaveToUse = trackToUse.octave;
        auto noteToUse = trackToUse.note;

        if (stepToUse.tFlags[XRSequencer::VELOCITY])
        {
            velocityToUse = stepToUse.tMods[XRSequencer::VELOCITY];
        }

        if (stepToUse.tFlags[XRSequencer::NOTE])
        {
            noteToUse = stepToUse.tMods[XRSequencer::NOTE];
        }

        if (stepToUse.tFlags[XRSequencer::OCTAVE])
        {
            octaveToUse = stepToUse.tMods[XRSequencer::OCTAVE];
        }

        if (stepToUse.state == XRSequencer::STATE_ACCENTED) {
            if (stepToUse.tFlags[XRSequencer::VELOCITY]) {
                velocityToUse = stepToUse.tMods[XRSequencer::VELOCITY];
            } else {
                velocityToUse = max(trackToUse.velocity, XRSequencer::activePatternSettings.accent);
            }
        }

        // is currently ratcheting and this is the ratcheting track
        if (track == XRSequencer::getRatchetTrack() && XRSequencer::getRatchetDivision() > -1){
            auto &ratchetTrack = XRSequencer::activeRatchetLayer.tracks[track];
            auto &ratchetTrackStep = XRSequencer::activeRatchetLayer.tracks[track].steps[step];

            velocityToUse = ratchetTrack.velocity;
            if (ratchetTrackStep.state == XRSequencer::STATE_ACCENTED) {
                if (ratchetTrackStep.tFlags[XRSequencer::VELOCITY]) {
                    velocityToUse = ratchetTrackStep.tMods[XRSequencer::VELOCITY];
                } else {
                    velocityToUse = max(ratchetTrack.velocity, XRSequencer::activePatternSettings.accent);
                }
            }

            octaveToUse = ratchetTrack.octave;
            noteToUse = ratchetTrack.note;

            if (ratchetTrackStep.tFlags[XRSequencer::NOTE])
            {
                noteToUse = ratchetTrackStep.tMods[XRSequencer::NOTE];
            }

            if (ratchetTrackStep.tFlags[XRSequencer::OCTAVE])
            {
                octaveToUse = ratchetTrackStep.tMods[XRSequencer::OCTAVE];
            }
        }

        int waveformToUse = msynWave;
        if (trackToUse.steps[step].sFlags[MSYN_WAVE])
        {
            waveformToUse = getWaveformNumber(
                getValueNormalizedAsUInt8(trackToUse.steps[step].sMods[MSYN_WAVE])
            );
        }

        float foundBaseFreq = _noteToFreqArr[noteToUse];
        float octaveFreqA = (foundBaseFreq + (msynFine * 0.01)) * (pow(2, octaveToUse));
        float octaveFreqB = (foundBaseFreq * pow(2.0, (float)msynDetune / 12.0)) * (pow(2, octaveToUse));

        AudioNoInterrupts();

        monoSynthInstances[track].oscA.begin(waveformToUse);
        monoSynthInstances[track].oscB.begin(waveformToUse);

        monoSynthInstances[track].oscA.frequency(octaveFreqA);
        monoSynthInstances[track].oscB.frequency(octaveFreqB);

        monoSynthInstances[track].filterEnv.attack(msynFatt);
        monoSynthInstances[track].filterEnv.decay(msynFdec);
        monoSynthInstances[track].filterEnv.sustain(msynFsus);
        monoSynthInstances[track].filterEnv.release(msynFrel);

        monoSynthInstances[track].ampEnv.attack(msynAatt);
        monoSynthInstances[track].ampEnv.decay(msynAdec);
        monoSynthInstances[track].ampEnv.sustain(msynAsus);
        monoSynthInstances[track].ampEnv.release(msynArel);
        
        monoSynthInstances[track].filterAccent.gain((velocityToUse * 0.01));
        monoSynthInstances[track].ampAccent.gain((velocityToUse * 0.01));
        monoSynthInstances[track].amp.gain(msynLvl);
        monoSynthInstances[track].ampDelaySend.gain(msynDly);

        monoSynthInstances[track].left.gain(getStereoPanValues(msynPan).left);
        monoSynthInstances[track].right.gain(getStereoPanValues(msynPan).right);

        AudioInterrupts();

        // now trigger envs
        monoSynthInstances[track].ampEnv.noteOn();
        monoSynthInstances[track].filterEnv.noteOn();
    }

    void handleDexedSynthNoteOnForTrackStep(int track, int step)
    {
        if (track > 3) return;

        if (applyChokeForDestinationTrackStep(track, step)) return;

        applyChokeForSourceTrack(track);

        //Serial.printf("handling dexed synth note on for track: %d, step: %d\n", track, step);

        auto &trackToUse = XRSequencer::getTrack(track);
        auto &stepToUse = XRSequencer::getStep(track, step);
        auto currLayer = XRSequencer::getCurrentSelectedTrackLayerNum();

        auto velocityToUse = trackToUse.velocity;
        auto octaveToUse = trackToUse.octave;
        auto noteToUse = trackToUse.note;

        if (stepToUse.tFlags[XRSequencer::NOTE])
        {
            noteToUse = stepToUse.tMods[XRSequencer::NOTE];
        }

        if (stepToUse.tFlags[XRSequencer::OCTAVE])
        {
            octaveToUse = stepToUse.tMods[XRSequencer::OCTAVE];
        }

        if (stepToUse.state == XRSequencer::STATE_ACCENTED) {
            if (stepToUse.tFlags[XRSequencer::VELOCITY]) {
                velocityToUse = stepToUse.tMods[XRSequencer::VELOCITY];
            } else {
                velocityToUse = max(trackToUse.velocity, XRSequencer::activePatternSettings.accent);
            }
        }

        // is currently ratcheting and this is the ratcheting track
        if (track == XRSequencer::getRatchetTrack() && XRSequencer::getRatchetDivision() > -1){
            auto &ratchetTrack = XRSequencer::activeRatchetLayer.tracks[track];
            auto &ratchetTrackStep = XRSequencer::activeRatchetLayer.tracks[track].steps[step];

            velocityToUse = ratchetTrack.velocity;
            if (ratchetTrackStep.state == XRSequencer::STATE_ACCENTED) {
                if (ratchetTrackStep.tFlags[XRSequencer::VELOCITY]) {
                    velocityToUse = ratchetTrackStep.tMods[XRSequencer::VELOCITY];
                } else {
                    velocityToUse = max(ratchetTrack.velocity, XRSequencer::activePatternSettings.accent);
                }
            }

            octaveToUse = ratchetTrack.octave;
            noteToUse = ratchetTrack.note;

            if (ratchetTrackStep.tFlags[XRSequencer::NOTE])
            {
                noteToUse = ratchetTrackStep.tMods[XRSequencer::NOTE];
            }

            if (ratchetTrackStep.tFlags[XRSequencer::OCTAVE])
            {
                octaveToUse = ratchetTrackStep.tMods[XRSequencer::OCTAVE];
            }
        }

        auto dexePan = getValueNormalizedAsFloat(activeKit.sounds[track].params[DEXE_PAN]);
        auto dexeLvl = getValueNormalizedAsFloat(activeKit.sounds[track].params[DEXE_LEVEL]);
        auto dexeDly = getValueNormalizedAsFloat(activeKit.sounds[track].params[DEXE_DELAY]);
        
        auto di = XRDexedManager::getActiveInstanceForTrack(track);

        dexedInstances[di].ampAccent.gain((velocityToUse * 0.01));
        dexedInstances[di].amp.gain(dexeLvl);
        dexedInstances[di].ampDelaySend.gain(dexeDly);

        dexedInstances[di].left.gain(getStereoPanValues(dexePan).left);
        dexedInstances[di].right.gain(getStereoPanValues(dexePan).right);

        // handle poly note assignments
        // TODO: do check for mono mode so we're not redundantly keydowning all notes when in mono mode?
        int noteMode = getValueNormalizedAsInt8(activeKit.sounds[track].params[DEXE_NOTE_MODE]);

        int noteB = getValueNormalizedAsInt8(activeKit.sounds[track].params[DEXE_NOTE_B]);
        int noteC = getValueNormalizedAsInt8(activeKit.sounds[track].params[DEXE_NOTE_C]);
        int noteD = getValueNormalizedAsInt8(activeKit.sounds[track].params[DEXE_NOTE_D]);

        if (trackToUse.steps[step].sFlags[XRSound::DEXE_NOTE_B]) {
            noteB = getValueNormalizedAsInt8(
                trackToUse.steps[step].sMods[XRSound::DEXE_NOTE_B]
            );
        }
        if (trackToUse.steps[step].sFlags[XRSound::DEXE_NOTE_C]) {
            noteC = getValueNormalizedAsInt8(
                trackToUse.steps[step].sMods[XRSound::DEXE_NOTE_C]
            );
        }
        if (trackToUse.steps[step].sFlags[XRSound::DEXE_NOTE_D]) {
            noteD = getValueNormalizedAsInt8(
                trackToUse.steps[step].sMods[XRSound::DEXE_NOTE_D]
            );
        }

        int midiNoteA = (noteToUse + (12 * (octaveToUse)));
        int midiNoteB = (noteToUse + noteB + (12 * (octaveToUse)));
        int midiNoteC = (noteToUse + noteC + (12 * (octaveToUse)));
        int midiNoteD = (noteToUse + noteD + (12 * (octaveToUse)));

        dexedInstances[di].dexed.keydown(midiNoteA, velocityToUse);
        if (noteMode == 1) { // 0 = mono, 1 = poly
            if (noteB != 0) dexedInstances[di].dexed.keydown(midiNoteB, velocityToUse);
            if (noteC != 0) dexedInstances[di].dexed.keydown(midiNoteC, velocityToUse);
            if (noteD != 0) dexedInstances[di].dexed.keydown(midiNoteD, velocityToUse);
        }
    }

    void handleBraidsNoteOnForTrackStep(int track, int step)
    {
        // if (applyChokeForDestinationTrackStep(track, step)) return;

        // applyChokeForSourceTrack(track);

        // auto &trackToUse = XRSequencer::getTrack(track);
        // auto &stepToUse = XRSequencer::getStep(track, step);

        // uint8_t noteToUse = trackToUse.note;
        // if (XRSequencer::patternTrackStepMods.tracks[track].steps[step].flags[XRSequencer::NOTE])
        // {
        //     noteToUse = XRSequencer::patternTrackStepMods.tracks[track].steps[step].mods[XRSequencer::NOTE];
        // }

        // uint8_t octaveToUse = trackToUse.octave;
        // if (XRSequencer::patternTrackStepMods.tracks[track].steps[step].flags[XRSequencer::OCTAVE])
        // {
        //     octaveToUse = XRSequencer::patternTrackStepMods.tracks[track].steps[step].mods[XRSequencer::OCTAVE];
        // }

        // uint8_t velocityToUse = trackToUse.velocity;
        // if (stepToUse.state == XRSequencer::STATE_ACCENTED) {
        //     if (XRSequencer::patternTrackStepMods.tracks[track].steps[step].flags[XRSequencer::VELOCITY]) {
        //         velocityToUse = XRSequencer::patternTrackStepMods.tracks[track].steps[step].mods[XRSequencer::VELOCITY];
        //     } else {
        //         velocityToUse = max(trackToUse.velocity, XRSequencer::activePatternSettings.accent);
        //     }
        // }

        // int midiNote = (noteToUse + (12 * (octaveToUse)));

        // braidsInstances[track].braids.set_braids_pitch(midiNote << 7);
    }

    void handleFmDrumNoteOnForTrackStep(int track, int step)
    {
        // if (track > 2) return;

        // if (applyChokeForDestinationTrackStep(track, step)) return;

        // applyChokeForSourceTrack(track);

        // auto &trackToUse = XRSequencer::getTrack(track);
        // auto &stepToUse = XRSequencer::getStep(track, step);

        // auto fmdNoise = getValueNormalizedAsFloat(activeKit.sounds[track].params[FMD_NOISE]);
        // auto fmdDecay = getValueNormalizedAsFloat(activeKit.sounds[track].params[FMD_DECAY]);
        // auto fmdFm = getValueNormalizedAsFloat(activeKit.sounds[track].params[FMD_FM]);
        // auto fmdFreq = getValueNormalizedAsUInt8(activeKit.sounds[track].params[FMD_FREQ]);
        // auto fmdPan = getValueNormalizedAsFloat(activeKit.sounds[track].params[FMD_PAN]);
        // auto fmdLvl = getValueNormalizedAsFloat(activeKit.sounds[track].params[FMD_LEVEL]);
        // auto fmdDly = getValueNormalizedAsFloat(activeKit.sounds[track].params[FMD_DELAY]);

        // auto velocityToUse = trackToUse.velocity;

        // if (stepToUse.state == XRSequencer::STATE_ACCENTED) {
        //     if (stepToUse.tFlags[XRSequencer::VELOCITY]) {
        //         velocityToUse = stepToUse.tMods[XRSequencer::VELOCITY];
        //     } else {
        //         velocityToUse = max(trackToUse.velocity, XRSequencer::activePatternSettings.accent);
        //     }
        // }

        // // is currently ratcheting and this is the ratcheting track
        // if (track == XRSequencer::getRatchetTrack() && XRSequencer::getRatchetDivision() > -1){
        //     auto &ratchetTrack = XRSequencer::activeRatchetLayer.tracks[track];
        //     auto &ratchetTrackStep = XRSequencer::activeRatchetLayer.tracks[track].steps[step];

        //     velocityToUse = ratchetTrack.velocity;
        //     if (ratchetTrackStep.state == XRSequencer::STATE_ACCENTED) {
        //         if (ratchetTrackStep.tFlags[XRSequencer::VELOCITY]) {
        //             velocityToUse = ratchetTrackStep.tMods[XRSequencer::VELOCITY];
        //         } else {
        //             velocityToUse = max(ratchetTrack.velocity, XRSequencer::activePatternSettings.accent);
        //         }
        //     }
        // }

        // AudioNoInterrupts();

        // fmDrumInstances[track].fmDrum.noise(fmdNoise);
        // fmDrumInstances[track].fmDrum.decay(fmdDecay);
        // fmDrumInstances[track].fmDrum.fm(fmdFm);
        // fmDrumInstances[track].fmDrum.frequency(fmdFreq);

        // fmDrumInstances[track].ampAccent.gain((velocityToUse * 0.01));
        // fmDrumInstances[track].amp.gain(fmdLvl);
        // fmDrumInstances[track].ampDelaySend.gain(fmdDly);

        // fmDrumInstances[track].left.gain(getStereoPanValues(fmdPan).left);
        // fmDrumInstances[track].right.gain(getStereoPanValues(fmdPan).right);
        
        // AudioInterrupts();

        // fmDrumInstances[track].fmDrum.noteOn();
    }

    void handleMIDINoteOnForTrackStep(int track, int step)
    {
        //auto &trackToUse = XRSequencer::getTrack(track);
        //auto &stepToUse = XRSequencer::getStep(track, step);

        //XRMIDI::sendNoteOn(64, 100, 1);
    }

    void handleCvGateNoteOnForTrackStep(int track, int step)
    {
        auto &trackToUse = XRSequencer::getTrack(track);

        auto currentSoundForTrack = activeKit.sounds[track];

        auto cvgaPort = getValueNormalizedAsInt8(currentSoundForTrack.params[0]); // TODO: use enum

        uint8_t noteToUse = trackToUse.note;
        // if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::NOTE])
        // {
        //     noteToUse = patternMods.tracks[track].steps[step].note;
        //     // Serial.println(noteToUse);
        // }

        uint8_t octaveToUse = trackToUse.octave;
        // if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::OCTAVE])
        // {
        //     octaveToUse = patternMods.tracks[track].steps[step].octave;
        //     // Serial.println(noteToUse);
        // }

        int midiNote = (noteToUse + (12 * (octaveToUse)));

        Serial.print("midiNote: ");
        Serial.print(midiNote);
        Serial.print(" cvLevels[midiNote]: ");
        Serial.println(_cvLevels[midiNote]);

        if (cvgaPort == 1)
        {
            XRCV::write(CS1, 0, _cvLevels[midiNote]); // cv
            XRCV::write(CS1, 1, 4095);               // gate
        }
        else if (cvgaPort == 2)
        {
            XRCV::write(CS2, 0, _cvLevels[midiNote]); // cv
            XRCV::write(CS2, 1, 4095);               // gate
        }
        else if (cvgaPort == 3)
        {
            XRCV::write(CS3, 0, _cvLevels[midiNote]); // cv
            XRCV::write(CS3, 1, 4095);               // gate
        }
        else if (cvgaPort == 4)
        {
            XRCV::write(CS4, 0, _cvLevels[midiNote]); // cv
            XRCV::write(CS4, 1, 4095);               // gate
        }
    }

    void turnOffAllSounds()
    {
        for (size_t d = 0; d < MAXIMUM_DEXED_SYNTH_SOUNDS; d++) {
            dexedInstances[d].dexed.notesOff();
        }

        for (size_t t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++) {
            if (activeKit.sounds[t].type == T_MONO_SAMPLE) {
                monoSampleInstances[t].amp.gain(0);
                monoSampleInstances[t].ampDelaySend.gain(0);
            } else if (activeKit.sounds[t].type == T_MONO_SYNTH) {
                monoSynthInstances[t].amp.gain(0);
                monoSynthInstances[t].ampDelaySend.gain(0);
            } 
            
            // else if (activeKit.sounds[t].type == T_FM_DRUM) {
            //     fmDrumInstances[t].amp.gain(0);
            //     fmDrumInstances[t].ampDelaySend.gain(0);
            // }

            // else if (activeKit.sounds[t].type == T_BRAIDS_SYNTH) {
            //     braidsInstances[t].amp.gain(0);
            //     braidsInstances[t].ampDelaySend.gain(0);
            // }
        }

        delayInstances[0].left.gain(0);
        delayInstances[0].right.gain(0);
    }

    void handleNoteOffForTrack(int track)
    {
        auto &trackToUse = XRSequencer::getTrack(track);
        auto currentSoundForTrack = activeKit.sounds[track];

        switch (currentSoundForTrack.type)
        {
        case T_MONO_SAMPLE:
            {
                monoSampleInstances[track].ampEnv.noteOff();
            }
            break;
        case T_MONO_SYNTH:
            {
                if (track < 4)
                {
                    monoSynthInstances[track].ampEnv.noteOff();
                    monoSynthInstances[track].filterEnv.noteOff();
                }
            }
            break;
        case T_DEXED_SYNTH:
            {
                if (track < 4)
                {
                    uint8_t noteToUse = trackToUse.note;
                    uint8_t octaveToUse = trackToUse.octave;
                    int midiNote = (noteToUse + (12 * (octaveToUse)));

                    auto di = XRDexedManager::getActiveInstanceForTrack(track);

                    dexedInstances[di].dexed.keyup(midiNote);
                }
            }
            break;
        case T_FM_DRUM:
            // n/a
            break;
        case T_MIDI:
            {
                //XRMIDI::sendNoteOff(64, 100, 1);
            }
            break;
        case T_CV_GATE:
            {
                auto cvgaPort = getValueNormalizedAsUInt8(currentSoundForTrack.params[0]); // TODO: use enum

                if (cvgaPort == 1)
                {
                    // writeToDAC(CS1, 0, cvLevels[midiNote]); // cv
                    XRCV::write(CS1, 1, 0); // gate
                }
                else if (cvgaPort == 2)
                {
                    // writeToDAC(CS2, 0, cvLevels[midiNote]); // cv
                    XRCV::write(CS2, 1, 0); // gate
                }
                else if (cvgaPort == 3)
                {
                    // writeToDAC(CS3, 0, cvLevels[midiNote]); // cv
                    XRCV::write(CS3, 1, 0); // gate
                }
                else if (cvgaPort == 4)
                {
                    // writeToDAC(CS4, 0, cvLevels[midiNote]); // cv
                    XRCV::write(CS4, 1, 0); // gate
                }
            }
            break;
        
        default:
            break;
        }
    }

    void handleNoteOffForTrackStep(int track, int step)
    {
        auto &trackToUse = XRSequencer::getTrack(track);
        auto &stepToUse = XRSequencer::getStep(track, step);
        auto currentSoundForTrack = activeKit.sounds[track];

        // TODO: get track step mods for note, octave, etc

        switch (currentSoundForTrack.type)
        {
        case T_MONO_SAMPLE:
            {
                monoSampleInstances[track].ampEnv.noteOff();
            }
            break;
        case T_MONO_SYNTH:
            {
                if (track < 4)
                {
                    monoSynthInstances[track].ampEnv.noteOff();
                    monoSynthInstances[track].filterEnv.noteOff();
                }
            }
            break;
        case T_DEXED_SYNTH:
            {
                uint8_t noteToUse = trackToUse.note;
                if (stepToUse.tFlags[XRSequencer::NOTE])
                {
                    noteToUse = stepToUse.tMods[XRSequencer::NOTE];
                }

                uint8_t octaveToUse = trackToUse.octave;
                if (stepToUse.tFlags[XRSequencer::OCTAVE])
                {
                    octaveToUse = stepToUse.tMods[XRSequencer::OCTAVE];
                }

                // handle poly note assignments
                // TODO: do check for mono mode so we're not redundantly keydowning all notes when in mono mode?

                int noteB = getValueNormalizedAsInt8(activeKit.sounds[track].params[DEXE_NOTE_B]);
                int noteC = getValueNormalizedAsInt8(activeKit.sounds[track].params[DEXE_NOTE_C]);
                int noteD = getValueNormalizedAsInt8(activeKit.sounds[track].params[DEXE_NOTE_D]);

                if (stepToUse.sFlags[XRSound::DEXE_NOTE_B]) {
                    noteB = getValueNormalizedAsInt8(
                        stepToUse.sMods[XRSound::DEXE_NOTE_B]
                    );
                }
                if (stepToUse.sFlags[XRSound::DEXE_NOTE_C]) {
                    noteC = getValueNormalizedAsInt8(
                        stepToUse.sMods[XRSound::DEXE_NOTE_C]
                    );
                }
                if (stepToUse.sFlags[XRSound::DEXE_NOTE_D]) {
                    noteD = getValueNormalizedAsInt8(
                        stepToUse.sMods[XRSound::DEXE_NOTE_D]
                    );
                }

                int midiNoteA = (noteToUse + (12 * (octaveToUse)));
                int midiNoteB = (noteToUse + noteB + (12 * (octaveToUse)));
                int midiNoteC = (noteToUse + noteC + (12 * (octaveToUse)));
                int midiNoteD = (noteToUse + noteD + (12 * (octaveToUse)));

                auto di = XRDexedManager::getActiveInstanceForTrack(track);

                dexedInstances[di].dexed.keyup(midiNoteA);
                if (noteB != 0) dexedInstances[di].dexed.keyup(midiNoteB);
                if (noteC != 0) dexedInstances[di].dexed.keyup(midiNoteC);
                if (noteD != 0) dexedInstances[di].dexed.keyup(midiNoteD);
            }
            break;
        case T_FM_DRUM:
            // n/a
            break;
        case T_MIDI:
            {
                //XRMIDI::sendNoteOff(64, 100, 1);
            }
            break;
        case T_CV_GATE:
            {
                auto cvgaPort = getValueNormalizedAsUInt8(currentSoundForTrack.params[0]); // TODO: use enum

                if (cvgaPort == 1)
                {
                    // writeToDAC(CS1, 0, cvLevels[midiNote]); // cv
                    XRCV::write(CS1, 1, 0); // gate
                }
                else if (cvgaPort == 2)
                {
                    // writeToDAC(CS2, 0, cvLevels[midiNote]); // cv
                    XRCV::write(CS2, 1, 0); // gate
                }
                else if (cvgaPort == 3)
                {
                    // writeToDAC(CS3, 0, cvLevels[midiNote]); // cv
                    XRCV::write(CS3, 1, 0); // gate
                }
                else if (cvgaPort == 4)
                {
                    // writeToDAC(CS4, 0, cvLevels[midiNote]); // cv
                    XRCV::write(CS4, 1, 0); // gate
                }
            }
            break;
        
        default:
            break;
        }
    }

    void assignSampleToTrackSound()
    {
        auto track = XRSequencer::getCurrentSelectedTrackNum();
        auto activeSampleSlot = XRSD::getActiveSampleSlot();
        std::string selected = XRSD::getCurrSampleFileHighlighted();

        if (selected == "--NONE--")
        {
            if (activeSampleSlot == 0) {
                strcpy(activeKit.sounds[track].sampleName, "");
            } else {
                strcpy(activeKit.sounds[track].sampleNameB, "");
            }

            kitDirty = true;

            return;
        }

        std::string sampleNameStr = "/samples/";
        sampleNameStr += selected;

        if (activeSampleSlot == 0) {
            strcpy(activeKit.sounds[track].sampleName, selected.c_str());
        } else {
            strcpy(activeKit.sounds[track].sampleNameB, selected.c_str());
        }

        kitDirty = true;
    }

    void changeTrackSoundType(uint8_t t, SOUND_TYPE newType)
    {
        auto currType = activeKit.sounds[t].type;
        if (currType == newType) return;

        activeKit.sounds[t].type = newType;

        AudioNoInterrupts();

        initTrackSound(t);

        AudioInterrupts();
        
        kitDirty = true;
    }

    void triggerTrackManually(uint8_t t, uint8_t note, uint8_t octave, bool accented)
    {
        auto currentSoundForTrack = activeKit.sounds[t];

        switch (currentSoundForTrack.type)
        {
        case T_MONO_SAMPLE:
            triggerMonoSampleNoteOn(t, note, octave, accented);

            break;
        case T_MONO_SYNTH:
            triggerMonoSynthNoteOn(t, note, octave, accented);
            
            break;
        case T_DEXED_SYNTH:
            triggerDexedSynthNoteOn(t, note, octave, accented);
            
            break;
        case T_BRAIDS_SYNTH:
            triggerBraidsNoteOn(t, note, octave, accented);

            break;
        case T_FM_DRUM:
            triggerFmDrumNoteOn(t, note, octave, accented);
            
            break;
        case T_CV_GATE:
            triggerCvGateNoteOn(t, note, octave, accented);
            
            break;
        
        default:
            break;
        }
    }

    void triggerMonoSampleNoteOn(uint8_t t, uint8_t note, uint8_t octave, bool accented)
    {
        auto &trackToUse = XRSequencer::getTrack(t);

        applyChokeForSourceTrack(t);

        auto msmpLooptype = getValueNormalizedAsUInt8(activeKit.sounds[t].params[MSMP_LOOPTYPE]);
        auto msmpLoopstart = getValueNormalizedAsInt32(activeKit.sounds[t].params[MSMP_LOOPSTART]);
        auto msmpLoopfinish = getValueNormalizedAsInt32(activeKit.sounds[t].params[MSMP_LOOPFINISH]);
        auto msmpPlaystart = (play_start)getValueNormalizedAsUInt8(activeKit.sounds[t].params[MSMP_PLAYSTART]);
        auto msmpAatt = getValueNormalizedAsFloat(activeKit.sounds[t].params[MSMP_AMP_ATTACK]);
        auto msmpAdec = getValueNormalizedAsFloat(activeKit.sounds[t].params[MSMP_AMP_DECAY]);
        auto msmpAsus = getValueNormalizedAsFloat(activeKit.sounds[t].params[MSMP_AMP_SUSTAIN]);
        auto msmpArel = getValueNormalizedAsFloat(activeKit.sounds[t].params[MSMP_AMP_RELEASE]);
        auto msmpPan = getValueNormalizedAsFloat(activeKit.sounds[t].params[MSMP_PAN]);
        auto msmpLvl = getValueNormalizedAsFloat(activeKit.sounds[t].params[MSMP_LEVEL]);
        auto msmpDly = getValueNormalizedAsFloat(monoSampleInitParams[MSMP_DELAY]);

        auto looptypeToUse = msmpLooptype;
        auto step = XRSequencer::getCurrentSelectedStepNum();
        if (step != -1) {
            auto layer = XRSequencer::getCurrentSelectedTrackLayerNum();

            if (trackToUse.steps[step].sFlags[MSMP_LOOPTYPE])
            {
                looptypeToUse = getValueNormalizedAsUInt8(trackToUse.steps[step].sMods[MSMP_LOOPTYPE]);
            }
        }

        std::string trackSampleName(activeKit.sounds[t].sampleName);
        std::string trackSampleNameB(activeKit.sounds[t].sampleNameB);
        bool hasFirstSample = trackSampleName.length() > 0;
        bool hasSecondSample = trackSampleNameB.length() > 0;

        auto velocityToUse = trackToUse.velocity;
        if (!hasSecondSample && accented) {
            velocityToUse = max(trackToUse.velocity, XRSequencer::activePatternSettings.accent);
        }

        AudioNoInterrupts();

        monoSampleInstances[t].ampEnv.attack(msmpAatt);
        monoSampleInstances[t].ampEnv.decay(msmpAdec);
        monoSampleInstances[t].ampEnv.sustain(msmpAsus);
        monoSampleInstances[t].ampEnv.release(msmpArel);

        monoSampleInstances[t].ampAccent.gain(velocityToUse * 0.01);
        monoSampleInstances[t].amp.gain(msmpLvl);
        monoSampleInstances[t].ampDelaySend.gain(msmpDly);

        monoSampleInstances[t].left.gain(getStereoPanValues(msmpPan).left);
        monoSampleInstances[t].right.gain(getStereoPanValues(msmpPan).right);

        const float semi = powf(2.0f, 1.0f/12);
        float freq = powf(semi, 12*(octave - 4) + note);
        monoSampleInstances[t].sample.setPlaybackRate(freq);

        AudioInterrupts();

        std::string sampleToUse = hasFirstSample ? trackSampleName : "";
        if (hasSecondSample && accented) 
        {
            sampleToUse = trackSampleNameB;
        }

        monoSampleInstances[t].ampEnv.noteOn();

        if (sampleToUse.length() > 0) {
            const auto sampleName = std::string("/samples/") + std::string(sampleToUse);

#ifdef USE_WAV
            monoSampleInstances[t].sample.playWav(sampleName.c_str());
#else
            monoSampleInstances[t].sample.playRaw(sampleName.c_str(), 1);
#endif
        }

        AudioNoInterrupts();

        // always re-initialize loop type
        monoSampleInstances[t].sample.setLoopType(loopTypeSelMap[looptypeToUse]);

        if (loopTypeSelMap[looptypeToUse] == looptype_none)
        {
            monoSampleInstances[t].sample.setPlayStart(play_start::play_start_sample);
            monoSampleInstances[t].sample.setLoopType(loop_type::looptype_none);
        }
        else if (loopTypeSelMap[looptypeToUse] == looptype_repeat)
        {
            monoSampleInstances[t].sample.setPlayStart(msmpPlaystart == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
            monoSampleInstances[t].sample.setLoopStart(msmpLoopstart);
            monoSampleInstances[t].sample.setLoopFinish(msmpLoopfinish);
        }

        AudioInterrupts();
    }

    void triggerMonoSynthNoteOn(uint8_t t, uint8_t note, uint8_t octave, bool accented)
    {
        if (t > 3) return;

        auto &trackToUse = XRSequencer::getTrack(t);
       
        auto msynFine = getValueNormalizedAsInt8(activeKit.sounds[t].params[MSYN_FINE]);
        auto msynDetune = getValueNormalizedAsInt8(activeKit.sounds[t].params[MSYN_DETUNE]);
        auto msynFatt = getValueNormalizedAsFloat(activeKit.sounds[t].params[MSYN_FILTER_ATTACK]);
        auto msynFdec = getValueNormalizedAsFloat(activeKit.sounds[t].params[MSYN_FILTER_DECAY]);
        auto msynFsus = getValueNormalizedAsFloat(activeKit.sounds[t].params[MSYN_FILTER_SUSTAIN]);
        auto msynFrel = getValueNormalizedAsFloat(activeKit.sounds[t].params[MSYN_FILTER_RELEASE]);
        auto msynAatt = getValueNormalizedAsFloat(activeKit.sounds[t].params[MSYN_AMP_ATTACK]);
        auto msynAdec = getValueNormalizedAsFloat(activeKit.sounds[t].params[MSYN_AMP_DECAY]);
        auto msynAsus = getValueNormalizedAsFloat(activeKit.sounds[t].params[MSYN_AMP_SUSTAIN]);
        auto msynArel = getValueNormalizedAsFloat(activeKit.sounds[t].params[MSYN_AMP_RELEASE]);
        auto msynPan = getValueNormalizedAsFloat(activeKit.sounds[t].params[MSYN_PAN]);
        auto msynLvl = getValueNormalizedAsFloat(activeKit.sounds[t].params[MSYN_LEVEL]);
        auto msynDly = getValueNormalizedAsFloat(monoSynthInitParams[MSYN_DELAY]);

        float foundBaseFreq = _noteToFreqArr[note];
        float octaveFreqA = (foundBaseFreq + (msynFine * 0.01)) * (pow(2, octave));
        float octaveFreqB = (foundBaseFreq * pow(2.0, (float)msynDetune / 12.0)) * (pow(2, octave));

        uint8_t velocityToUse = trackToUse.velocity;
        if (accented) {
            velocityToUse = max(trackToUse.velocity, XRSequencer::activePatternSettings.accent);
        }

        AudioNoInterrupts();

        monoSynthInstances[t].oscA.frequency(octaveFreqA);
        monoSynthInstances[t].oscB.frequency(octaveFreqB);
        monoSynthInstances[t].filterEnv.attack(msynFatt);
        monoSynthInstances[t].filterEnv.decay(msynFdec);
        monoSynthInstances[t].filterEnv.sustain(msynFsus);
        monoSynthInstances[t].filterEnv.release(msynFrel);
        monoSynthInstances[t].ampEnv.attack(msynAatt);
        monoSynthInstances[t].ampEnv.decay(msynAdec);
        monoSynthInstances[t].ampEnv.sustain(msynAsus);
        monoSynthInstances[t].ampEnv.release(msynArel);

        monoSynthInstances[t].filterAccent.gain(velocityToUse * 0.01);
        monoSynthInstances[t].ampAccent.gain(velocityToUse * 0.01);
        monoSynthInstances[t].amp.gain(msynLvl);
        monoSynthInstances[t].ampDelaySend.gain(msynDly);

        monoSynthInstances[t].left.gain(getStereoPanValues(msynPan).left);
        monoSynthInstances[t].right.gain(getStereoPanValues(msynPan).right);

        AudioInterrupts();

        // now trigger envs
        monoSynthInstances[t].ampEnv.noteOn();
        monoSynthInstances[t].filterEnv.noteOn();
    }

    void triggerDexedSynthNoteOn(uint8_t t, uint8_t note, uint8_t octave, bool accented)
    {
        if (t > 3) return;

        auto &trackToUse = XRSequencer::getTrack(t);

        int midiNote = (note + (12 * (octave)));

        uint8_t velocityToUse = trackToUse.velocity;
        if (accented) {
            velocityToUse = max(trackToUse.velocity, XRSequencer::activePatternSettings.accent);
        }

        auto di = XRDexedManager::getActiveInstanceForTrack(t);

        dexedInstances[di].ampAccent.gain(velocityToUse * 0.01);
        dexedInstances[di].dexed.keydown(midiNote, 50);
    }

    void triggerBraidsNoteOn(uint8_t t, uint8_t note, uint8_t octave, bool accented)
    {
        int midiNote = (note + (12 * (octave)));

        // braidsInstances[t].braids.set_braids_pitch(midiNote << 7);
    }

    void triggerFmDrumNoteOn(uint8_t t, uint8_t note, uint8_t octave, bool accented)
    {
        // if (t > 2) return;

        // auto &trackToUse = XRSequencer::getTrack(t);

        // auto fmdLvl = getValueNormalizedAsFloat(activeKit.sounds[t].params[FMD_LEVEL]);
        // auto fmdDly = getValueNormalizedAsFloat(activeKit.sounds[t].params[FMD_DELAY]);
        // auto fmdPan = getValueNormalizedAsFloat(activeKit.sounds[t].params[FMD_PAN]);

        // uint8_t velocityToUse = trackToUse.velocity;
        // if (accented) {
        //     velocityToUse = max(trackToUse.velocity, XRSequencer::activePatternSettings.accent);
        // }

        // AudioNoInterrupts();

        // fmDrumInstances[t].ampAccent.gain((velocityToUse * 0.01));
        // fmDrumInstances[t].amp.gain(fmdLvl);
        // fmDrumInstances[t].ampDelaySend.gain(fmdDly);

        // fmDrumInstances[t].left.gain(getStereoPanValues(fmdPan).left);
        // fmDrumInstances[t].right.gain(getStereoPanValues(fmdPan).right);

        // AudioInterrupts();

        // fmDrumInstances[t].fmDrum.noteOn();
    }

    void triggerCvGateNoteOn(uint8_t t, uint8_t note, uint8_t octave, bool accented)
    {
        auto currentSoundForTrack = activeKit.sounds[t];
        auto cvgaPort = getValueNormalizedAsInt8(currentSoundForTrack.params[0]); // TODO: use enum

        uint8_t noteToUse = note;

        int midiNote = (noteToUse + (12 * (octave))); // C0 = 12

        if (cvgaPort == 1)
        {
            XRCV::write(CS1, 0, _cvLevels[midiNote]); // cv
            XRCV::write(CS1, 1, 4095);               // gate
        }
        else if (cvgaPort == 2)
        {
            XRCV::write(CS2, 0, _cvLevels[midiNote]); // cv
            XRCV::write(CS2, 1, 4095);               // gate
        }
        else if (cvgaPort == 3)
        {
            XRCV::write(CS3, 0, _cvLevels[midiNote]); // cv
            XRCV::write(CS3, 1, 4095);               // gate
        }
        else if (cvgaPort == 4)
        {
            XRCV::write(CS4, 0, _cvLevels[midiNote]); // cv
            XRCV::write(CS4, 1, 4095);               // gate
        }
    }

    void noteOffTrackManually(int noteOnKeyboard, uint8_t octave)
    {
        auto currSelTrackNum = XRSequencer::getCurrentSelectedTrackNum();

        switch (activeKit.sounds[currSelTrackNum].type)
        {
        case T_MONO_SAMPLE:
            {
                monoSampleInstances[currSelTrackNum].ampEnv.noteOff();
            }
            break;
        case T_MONO_SYNTH:
            {
                monoSynthInstances[currSelTrackNum].ampEnv.noteOff();
                monoSynthInstances[currSelTrackNum].filterEnv.noteOff();
            }
            break;
        case T_DEXED_SYNTH:
            {
                int midiNote = (noteOnKeyboard + (12 * (octave)));

                auto di = XRDexedManager::getActiveInstanceForTrack(currSelTrackNum);

                dexedInstances[di].dexed.keyup(midiNote);
            }
            break;
        case T_BRAIDS_SYNTH:
            {
                // int midiNote = (noteOnKeyboard + (12 * (XRKeyMatrix::getKeyboardOctave())));

                // braidsInstances[currSelTrackNum].braids.set_braids_pitch(midiNote << 7);
            }
            break;
        case T_MIDI:
            {
                //XRMIDI::sendNoteOff(64, 100, 1);
            }
            break;
        case T_CV_GATE:
            {
                auto cvgaPort = getValueNormalizedAsInt8(activeKit.sounds[currSelTrackNum].params[0]); // TODO: use enum
                
                if (cvgaPort == 1)
                {
                    // writeToDAC(CS1, 0, cvLevels[midiNote]); // cv
                    XRCV::write(CS1, 1, 0); // gate
                }
                else if (cvgaPort == 2)
                {
                    // writeToDAC(CS2, 0, cvLevels[midiNote]); // cv
                    XRCV::write(CS2, 1, 0); // gate
                }
                else if (cvgaPort == 3)
                {
                    // writeToDAC(CS3, 0, cvLevels[midiNote]); // cv
                    XRCV::write(CS3, 1, 0); // gate
                }
                else if (cvgaPort == 4)
                {
                    // writeToDAC(CS4, 0, cvLevels[midiNote]); // cv
                    XRCV::write(CS4, 1, 0); // gate
                }
            }
            break;
        
        default:
            break;
        }
    }

    PANNED_AMOUNTS getStereoPanValues(float pan)
    {
        PANNED_AMOUNTS amounts;

        amounts.left = 1.0;
        if (pan > 0) {
            amounts.left -= pan;
        }

        amounts.right = 1.0;
        if (pan < 0) {
            amounts.right += pan;
        }

        return amounts;
    }
    
    std::string getSoundMetaStr(SOUND_TYPE type)
    {
        std::string outputStr;

        switch (type)
        {

        case T_EMPTY:
            outputStr = "nil";
            break;

        case T_MONO_SAMPLE:
            outputStr = "smp";
            break;

        case T_MONO_SYNTH:
            outputStr = "ms1";
            break;
        case T_DEXED_SYNTH:
            outputStr = "dex";
            break;
        case T_BRAIDS_SYNTH:
            outputStr = "brd";
            break;
        case T_FM_DRUM:
            outputStr = "fmd";
            break;
        case T_MIDI:
            outputStr = "mid";
            break;

        case T_CV_GATE:
            outputStr = "cvg";
            break;

        case T_CV_TRIG:
            outputStr = "cvt";
            break;

        default:
            break;
        }

        return outputStr;
    }

    std::string getSoundTypeNameStr(SOUND_TYPE type)
    {
        std::string str = "empty"; // default

        switch (type)
        {
        case T_MONO_SAMPLE:
            str = "init";

            break;

        case T_MONO_SYNTH:
            str = "init";

            break;
        case T_DEXED_SYNTH:
            str = "init";

            break;
        case T_BRAIDS_SYNTH:
            str = "init";

            break;
        case T_FM_DRUM:
            str = "init";

            break;
        case T_MIDI:
            str = "init";

            break;

        case T_CV_GATE:
            str = "";

            break;

        case T_CV_TRIG:
            str = "";

            break;

        default:
            break;
        }

        return str;
    }

    std::string getPageNameForCurrentTrack()
    {
        auto currTrack = XRSequencer::getCurrentSelectedTrackNum();
        auto currPage = XRSequencer::getCurrentSelectedPage();

        std::string outputStr = soundCurrPageNameMap[activeKit.sounds[currTrack].type][currPage];

        return outputStr;
    }

    uint8_t getPageCountForCurrentTrack()
    {
        auto currTrack = XRSequencer::getCurrentSelectedTrackNum();

        return soundPageNumMap[activeKit.sounds[currTrack].type];
    }

    uint8_t getPageCountForTrack(int track)
    {
        return soundPageNumMap[activeKit.sounds[track].type];
    }

    std::string getWaveformName(uint8_t waveform)
    {
        std::string outputStr;

        switch (waveform)
        {
        case WAVEFORM_SAWTOOTH:
            outputStr = "SAW";
            break;

        case WAVEFORM_SAWTOOTH_REVERSE:
            outputStr = "RSAW";
            break;

        case WAVEFORM_TRIANGLE:
            outputStr = "TRI";
            break;

        case WAVEFORM_SQUARE:
            outputStr = "SQR";
            break;

        case WAVEFORM_PULSE:
            outputStr = "PUL";
            break;

        case WAVEFORM_SINE:
            outputStr = "SIN";
            break;

        default:
            break;
        }

        return outputStr;
    }

    std::string getPlaybackSpeedStr(float rate)
    {
        std::string rateStr = std::to_string((float)round(rate * 100) / 100);

        int strLen = (int)rate >= 10 ? 4 : 3;

        std::string outputStr = rateStr.substr(0, strLen);

        bool t = rate < 0.1;
        if (t)
        {
            outputStr = "REV";
        }

        return outputStr;
    }

    std::string getLoopTypeName()
    {
        std::string outputStr;

        auto currentSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currentSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currentUXMode = XRUX::getCurrentMode();

        auto looptype = getValueNormalizedAsUInt8(activeKit.sounds[currentSelectedTrackNum].params[MSMP_LOOPTYPE]);
        auto chromatic = getValueNormalizedAsBool(activeKit.sounds[currentSelectedTrackNum].params[MSMP_CHROMATIC]);

        uint8_t looptypeToUse = looptype;

        if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStepNum > -1)
        {
            if (XRSequencer::activeTrackLayer.tracks[currentSelectedTrackNum].steps[currentSelectedStepNum].sFlags[MSMP_LOOPTYPE])
            {
                looptypeToUse = getValueNormalizedAsUInt8(
                    XRSequencer::activeTrackLayer.tracks[currentSelectedTrackNum].steps[currentSelectedStepNum].sMods[MSMP_LOOPTYPE]
                );
            }
        }

        if (loopTypeSelMap[looptypeToUse] == loop_type::looptype_none)
        {
            outputStr += "OFF";
        }
        else if (loopTypeSelMap[looptypeToUse] == loop_type::looptype_repeat)
        {
            if (chromatic)
            {
                outputStr += "CHR";
            }
            else
            {
                outputStr += "REP";
            }
        }

        return outputStr;
    }

    std::string getSampleName(const char* name)
    {
        std::string sampleName = "/samples/";
        sampleName += name;

        return sampleName;
    }

    int getWaveformNumber(uint8_t waveformType)
    {
        return _waveformFindMap[(int)waveformType];
    }

    int getWaveformTypeSelection(uint8_t waveformNumber)
    {
        return _waveformSelMap[(int)waveformNumber];
    }

    float getDetunedOscFreqB(uint8_t note, float detuneAmount)
    {
        auto foundBaseFreq = _noteToFreqArr[note];
        auto keyboardOctave = XRKeyMatrix::getKeyboardOctave();
        
        return (foundBaseFreq * pow(2.0, (float)detuneAmount / 12.0)) * (pow(2, keyboardOctave));
    }

    float getOscFreqA(uint8_t note, int8_t fine)
    {
        float foundBaseFreq = _noteToFreqArr[note];
        auto keyboardOctave = XRKeyMatrix::getKeyboardOctave();

        return (foundBaseFreq + (fine * 0.01)) * (pow(2, keyboardOctave));
    }

    void applyCurrentDexedPatchToSound()
    {
        auto track = XRSequencer::getCurrentSelectedTrackNum();

        uint8_t dexedParamData[MAXIMUM_DEXED_SOUND_PARAMS];

        auto di = XRDexedManager::getActiveInstanceForTrack(track);

        dexedInstances[di].dexed.getVoiceData(dexedParamData);

        for (int dp=0; dp<MAXIMUM_DEXED_SOUND_PARAMS; dp++)
        {
            activeKit.sounds[track].dexedParams[dp] = dexedParamData[dp];
        }
        
        activeKit.sounds[track].params[DEXE_TRANSPOSE] = getInt32ValuePaddedAsInt32(dexedInstances[di].dexed.getTranspose());
        activeKit.sounds[track].params[DEXE_ALGO] = getInt32ValuePaddedAsInt32(dexedInstances[di].dexed.getAlgorithm());
    }

    int8_t getValueNormalizedAsInt8(int32_t param)
    {
        return (int8_t)(param != 0 ? param / 100 : 0);
    }

    int32_t getValueNormalizedAsInt32(int32_t param)
    {
        return (int32_t)(param != 0 ? param / 100 : 0);
    }

    uint32_t getValueNormalizedAsUInt32(int32_t param)
    {
        return (uint32_t)(param != 0 ? param / 100 : 0);
    }

    uint8_t getValueNormalizedAsUInt8(int32_t param)
    {
        return (uint8_t)(param != 0 ? param / 100 : 0);
    }

    float getValueNormalizedAsFloat(int32_t param)
    {
        return (float)(param != 0 ? (float)param / 100 : 0);
    }

    bool getValueNormalizedAsBool(int32_t param)
    {
        return (bool)(param != 0 ? param / 100 : 0);
    }

    int32_t getInt32ValuePaddedAsInt32(int32_t value)
    {
        return (int32_t)(value * 100);
    }

    int32_t getUInt32ValuePaddedAsInt32(uint32_t value)
    {
        return (int32_t)(value * 100);
    }

    int32_t getFloatValuePaddedAsInt32(float value)
    {
        return (int32_t)(round(value * 100));
    }

    int32_t getBoolValuePaddedAsInt32(bool value)
    {
        return (int32_t)(value * 100);
    }
}
