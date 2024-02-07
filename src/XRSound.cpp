#include <XRSound.h>
#include <XRClock.h>
#include <XRSequencer.h>
#include <XRUX.h>
#include <XRMIDI.h>
#include <XRCV.h>
#include <XRSD.h>
#include <XRKeyMatrix.h>
#include <dualheapasyncflashloader.h>
#include <map>
#include <XRAsyncPSRAMLoader.h>

namespace XRSound
{
    // private variables

    // 8MB max of samples per pattern in external PSRAM, 1 sample allowed per track for now    
    newdigate::dualheapasyncflashloader _loader;
    uint8_t _numChannels = 1;

    bool patternSoundsDirty = false;
    bool patternSoundStepModsDirty = false;

    float _noteToFreqArr[13] = {
        16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.50, 25.96, 27.50, 29.14, 30.87, 32.70
    };

    int _cvLevels[128];
    
    float delayTimeMs = 500;
    float delayFeedback = 0;

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
        100,0,0,0,0,            // level, pan, n/a, n/a, n/a
        0,0,0,0,0               // n/a, n/a, n/a, n/a, n/a
    };

    int32_t monoSynthInitParams[MAXIMUM_SOUND_PARAMS] = {
        100,-700,0,100,50,          // waveform, detune, fine, osc-a level, osc-b level
        50,0,160000,0,0,            // width, noise, cutoff, resonance, n/a
        0,100000,100,500000,100,    // f. attack, f. decay, f. sustain, f. release, f. env amount
        0,100000,100,500000,0,      // a. attack, a. decay, a. sustain, a. release, n/a
        100,0,0,0,0,                // level, pan, delay dry/wet, n/a, n/a
        0,0,0,0,0                   // n/a, n/a, n/a, n/a, n/a
    };

    int32_t dexedSynthInitParams[MAXIMUM_SOUND_PARAMS] = {
        100,0,0,0,0,    // algorithm, n/a, n/a, n/a, n/a
        0,0,0,0,0,      // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0,      // n/a, n/a, n/a, n/a, n/a
        100,0,0,0,0,    // level, pan, n/a, n/a, n/a
        0,0,0,0,0       // poly note b, poly note c, poly note d, note mode, n/a
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
        100,0,0,0,0,    // level, pan, n/a, n/a, n/a
        0,0,0,0,0       // n/a, n/a, n/a, n/a, n/a
    };

    int32_t fmDrumInitParams[MAXIMUM_SOUND_PARAMS] = {
        5000,0,75,0,0,  // frequency, fm, decay, noise, overdrive
        0,0,0,0,0,      // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0,      // n/a, n/a, n/a, n/a, n/a
        100,0,0,0,0,    // level, pan, n/a, n/a, n/a
        0,0,0,0,0       // n/a, n/a, n/a, n/a, n/a
    };

    int32_t midiInitParams[MAXIMUM_SOUND_PARAMS] = {
        1,0,0,0,0, // channel, n/a, n/a, n/a, n/a
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
        0,0,0,0,0       // n/a, n/a, n/a, n/a, n/a
    };

    int32_t cvTrigInitParams[MAXIMUM_SOUND_PARAMS] = {
        100,0,0,0,0,    // port, n/a, n/a, n/a, n/a
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
#ifndef NO_DEXED
        {T_DEXED_SYNTH, 0},
#endif
#ifndef NO_FMDRUM
        {T_FM_DRUM, 0},
#endif
        {T_CV_GATE, 0},
        {T_CV_TRIG, 0},
        {T_BRAIDS_SYNTH, 0},
    };
  
    // extern globals
    
    SOUND activePatternSounds[MAXIMUM_SEQUENCER_TRACKS];
    DMAMEM SOUND nextPatternSounds[MAXIMUM_SEQUENCER_TRACKS];
    DMAMEM PATTERN_SOUND_MOD_LAYER activePatternSoundStepModLayer;

    bool soundNeedsReinit[MAXIMUM_SEQUENCER_TRACKS] = {
        false, false, false, false,
        false, false, false, false,
        false, false, false, false,
        false, false, false, false,
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

#ifndef NO_DEXED
    DexedInstance dexedInstances[MAXIMUM_DEXED_SYNTH_SOUNDS] = {
        DexedInstance(dexed1, dexedAmpAccent1, dexedAmp1, dexedAmpDelaySend1, dexedLeft1, dexedRight1),
        DexedInstance(dexed2, dexedAmpAccent2, dexedAmp2, dexedAmpDelaySend2, dexedLeft2, dexedRight2),
        DexedInstance(dexed3, dexedAmpAccent3, dexedAmp3, dexedAmpDelaySend3, dexedLeft3, dexedRight3),
        DexedInstance(dexed4, dexedAmpAccent4, dexedAmp4, dexedAmpDelaySend4, dexedLeft4, dexedRight4),
    };
#endif

#ifndef NO_FMDRUM
    FmDrumInstance fmDrumInstances[MAXIMUM_FM_DRUM_SOUNDS] = {
        FmDrumInstance(fmDrum1, fmDrumAmpAccent1, fmDrumAmp1, fmDrumAmpDelaySend1, fmDrumLeft1, fmDrumRight1),
        FmDrumInstance(fmDrum2, fmDrumAmpAccent2, fmDrumAmp2, fmDrumAmpDelaySend2, fmDrumLeft2, fmDrumRight2),
        FmDrumInstance(fmDrum3, fmDrumAmpAccent3, fmDrumAmp3, fmDrumAmpDelaySend3, fmDrumLeft3, fmDrumRight3),
    };
#endif

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
#ifndef NO_DEXED
        { T_DEXED_SYNTH, dexedSynthInitParams },
#endif
        { T_BRAIDS_SYNTH, braidsSynthInitParams },
#ifndef NO_FMDRUM
        { T_FM_DRUM, fmDrumInitParams },
#endif
        { T_MIDI, midiInitParams },
        { T_CV_GATE, cvGateInitParams },
        { T_CV_TRIG, cvTrigInitParams },
    };

    std::map<SOUND_TYPE, int> soundPageNumMap = {
        {T_MONO_SAMPLE, 5},
        {T_MONO_SYNTH, 6},
#ifndef NO_DEXED
        {T_DEXED_SYNTH, 5},
#endif
        {T_BRAIDS_SYNTH, 2},
#ifndef NO_FMDRUM
        {T_FM_DRUM, 2},
#endif
        {T_MIDI, 1},
        {T_CV_GATE, 1},
        {T_CV_TRIG, 1},
        {T_EMPTY, 1},
    };

    std::map<SOUND_TYPE, std::map<int, std::string>> soundCurrPageNameMap = {
        {T_MONO_SAMPLE, {
                         {0, "MAIN"},
                         {1, "SAMPLE"},
                         {2, "LOOP"},
                         {3, "A.ADSR"},
                         {4, "OUTPUT"},
                     }},
        {T_MONO_SYNTH, {
                                {0, "MAIN"},
                                {1, "OSC"},
                                {2, "FILTER"},
                                {3, "F.ADSR"},
                                {4, "A.ADSR"},
                                {5, "OUTPUT"},
                            }},
#ifndef NO_DEXED
        {T_DEXED_SYNTH, {
                         {0, "MAIN"},
                         {1, "FM1"},
                         {2, "FM2"},
                         {3, "OUTPUT"},
                         {4, "POLY"},
                     }},
#endif
#ifndef NO_FMDRUM
        {T_FM_DRUM, {
                         {0, "MAIN"},
                         {1, "OUTPUT"},
                     }},
#endif
        {T_MIDI, {
                       {0, "MAIN"},
                   }},
        {T_CV_GATE, {
                      {0, "MAIN"},
                  }},
        {T_CV_TRIG, {
                      {0, "MAIN"},
                  }},
        {T_EMPTY, {
                      {0, ""},
                  }},
    };

    std::string patternPageNames[2] = {
        "MAIN",
        "SEND EFFECTS"
    };

    std::map<SOUND_TYPE, int8_t> soundTypeInstanceLimitMap = {
        {T_EMPTY, MAXIMUM_SEQUENCER_TRACKS},
        {T_MONO_SAMPLE, MAXIMUM_MONO_SAMPLE_SOUNDS},
        {T_MONO_SYNTH, MAXIMUM_MONO_SYNTH_SOUNDS},
#ifndef NO_DEXED
        {T_DEXED_SYNTH, MAXIMUM_DEXED_SYNTH_SOUNDS},
#endif
        {T_BRAIDS_SYNTH, MAXIMUM_BRAIDS_SYNTH_SOUNDS},
#ifndef NO_FMDRUM
        {T_FM_DRUM, MAXIMUM_FM_DRUM_SOUNDS},
#endif
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

        initActivePatternSounds();
        initNextPatternSounds();
        initPatternSoundStepMods();
        initVoices();
    }

    void initNextPatternSounds()
    {
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            nextPatternSounds[t].type = T_EMPTY;

            for (int p = 0; p < MAXIMUM_SOUND_PARAMS; p++) {
                nextPatternSounds[t].params[p] = 0;
            }

            for (int dp = 0; dp < MAXIMUM_DEXED_SOUND_PARAMS; dp++) {
                nextPatternSounds[t].dexedParams[dp] = dexedInitVoice[dp];
            }

            strcpy(nextPatternSounds[t].name, "NO SOUND");
            strcpy(nextPatternSounds[t].sampleName, "");
        }

        patternSoundsDirty = true;
    }

    // Since initPatternSoundStepMods lives in DMAMEM, we need to initialize its contents
    void initPatternSoundStepMods()
    {
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
            {
                for (int p = 0; p < MAXIMUM_SOUND_PARAMS; p++) {
                    activePatternSoundStepModLayer.sounds[t].steps[s].mods[p] = 0;
                    activePatternSoundStepModLayer.sounds[t].steps[s].flags[p] = false;
                }
            }
        }
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
#ifndef NO_DEXED
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
#endif
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
#ifndef NO_FMDRUM
        auto fmDrumLvl = getValueNormalizedAsFloat(fmDrumInitParams[FMD_LEVEL]);
        auto fmDrumPan = getValueNormalizedAsFloat(fmDrumInitParams[FMD_PAN]);
        auto fmDrumDly = getValueNormalizedAsFloat(dexedSynthInitParams[FMD_DELAY]);

        Serial.printf("fmDrumLvl: %f\n");

        for (int f=0; f<MAXIMUM_FM_DRUM_SOUNDS; f++)
        {
            fmDrumInstances[f].fmDrum.init();

            fmDrumInstances[f].ampAccent.gain(1.0); // used by track velocity
            fmDrumInstances[f].amp.gain(fmDrumLvl);
            fmDrumInstances[f].ampDelaySend.gain(0);

            PANNED_AMOUNTS fmDrumPannedAmounts = getStereoPanValues(fmDrumPan);
            fmDrumInstances[f].left.gain(fmDrumPannedAmounts.left);
            fmDrumInstances[f].right.gain(fmDrumPannedAmounts.right);
        }
#endif

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

        // Delay mixers
        delayMix1.gain(0, 1);
        delayMix1.gain(1, 1);
        delayMix1.gain(2, 1);
        delayMix1.gain(3, 1);
        delayMix2.gain(0, 1);
        delayMix2.gain(1, 1);
        delayMix2.gain(2, 1);
        delayMix2.gain(3, 0); // unused

        // Delay instance
        delayInstances[0].delayEffect.delay(0, delayTimeMs); // TODO: init from pattern
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
        voiceSubMixLeft3.gain(3, 0); // BRAIDS DOES NOT WORK
        voiceSubMixRight3.gain(3, 0); // BRAIDS DOES NOT WORK

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
        mainMixerLeft.gain(2, 0.5); // delay L
        mainMixerRight.gain(2, 0.5); // delay R
        mainMixerLeft.gain(3, 0);
        mainMixerRight.gain(3, 0);

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
        strcpy(activePatternSounds[track].name, "");
        strcpy(activePatternSounds[track].sampleName, "");

        // init generic sound params
        auto soundType = activePatternSounds[track].type;
        auto initParams = soundTypeInitParams[soundType];
        for (size_t p=0; p<MAXIMUM_SOUND_PARAMS; p++)
        {
            activePatternSounds[track].params[p] = initParams[p];
        }

        // // dexed "init voice" params
        // for (size_t d=0; d<MAXIMUM_DEXED_SOUND_PARAMS; d++)
        // {
        //     currentPatternSounds[track].dexedParams[d] = dexedInitVoice[d];
        // }
    }

    void setSoundNeedsReinit(int sound, bool reinit)
    {
        soundNeedsReinit[sound] = reinit;
    }

    void reinitSoundForTrack(int track)
    {
        activePatternSounds[track] = nextPatternSounds[track];

#ifndef NO_DEXED
        if (track < 4 && activePatternSounds[track].type == T_DEXED_SYNTH) {
            // load any dexed voice settings for track
            dexedInstances[track].dexed.loadVoiceParameters(activePatternSounds[track].dexedParams);
            dexedInstances[track].dexed.setMonoMode(!activePatternSounds[track].params[DEXE_NOTE_MODE]);
        }
#endif
        // all done reinitializing sound
        soundNeedsReinit[track] = false;
    }

    void initActivePatternSounds()
    {
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            activePatternSounds[t].type = T_EMPTY;

            for (int p = 0; p < MAXIMUM_SOUND_PARAMS; p++) {
                activePatternSounds[t].params[p] = 0;
            }

            for (int dp = 0; dp < MAXIMUM_DEXED_SOUND_PARAMS; dp++) {
                activePatternSounds[t].dexedParams[dp] = dexedInitVoice[dp];
            }

            strcpy(activePatternSounds[t].name, "NO SOUND");
            strcpy(activePatternSounds[t].sampleName, "");
        }
    }

    void applyActivePatternSounds()
    {
        for (size_t t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++) {
#ifndef NO_DEXED
            if (t < 4 && activePatternSounds[t].type == T_DEXED_SYNTH) {
                // load any dexed voice settings for track
                dexedInstances[t].dexed.loadVoiceParameters(activePatternSounds[t].dexedParams);
                dexedInstances[t].dexed.setMonoMode(!activePatternSounds[t].params[DEXE_NOTE_MODE]);
            }
#endif
        }
    }

    void saveSoundDataForPatternChange()
    {
        if (patternSoundsDirty) {
            XRSD::saveActivePatternSounds();
        }

        if (patternSoundStepModsDirty) {
            XRSD::saveActiveSoundStepModLayerToSdCard();
        }
    }

    void loadSoundDataForPatternChange(int nextBank, int nextPattern)
    {
        if (!XRSD::loadNextPatternSounds(nextBank, nextPattern))
        {
            initNextPatternSounds();
        }

        if (!XRSD::loadPatternSoundStepModLayerFromSdCard(nextBank, nextPattern, 0)) {
            initPatternSoundStepMods();
        }

        auto &seqState = XRSequencer::getSeqState();

        if (seqState.playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::RUNNING) {
            auto &tracks = XRSequencer::nextTrackLayer.tracks;

            for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
            {
                if (
                    tracks[t].initialized
#ifndef NO_DEXED
                    && activePatternSounds[t].type != T_DEXED_SYNTH // don't load dexed changes async since it cuts out the sound
#endif
                ) {
                    setSoundNeedsReinit(t, true); // reinit sound asynchronously since the upcoming track is active
                } else {
                    reinitSoundForTrack(t); // reinit sound synchronously since the upcoming track is either empty or a dexed sound
                }
            }
        } else {
            XRAsyncPSRAMLoader::startAsyncInitOfNextSamples();
            for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
            {
                reinitSoundForTrack(t); // reinit all sounds synchronously since the sequencer isn't running
            }
        }
    }
    
    SOUND_CONTROL_MODS getControlModDataForPattern()
    {
        SOUND_CONTROL_MODS mods;

        auto &pattern = XRSequencer::getCurrentSelectedPattern();
        auto currSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        switch (currSelectedPageNum)
        {
        case 0: // MAIN
            {
               std::string grooveForPattern = pattern.groove.id > -1 ? XRClock::getGrooveString(pattern.groove.id) : "";
                std::string grooveAmountForPattern = XRClock::getGrooveAmountString(pattern.groove.id, pattern.groove.amount);

                mods.aName = "L.STEP";
                mods.bName = "GROOVE";
                mods.cName = "GR.AMT";
                mods.dName = "ACCENT";

                mods.aValue = std::to_string(pattern.lstep);
                mods.bValue = pattern.groove.id > -1 ? grooveForPattern : "OFF";
                mods.cValue = pattern.groove.id > -1 ? grooveAmountForPattern : "--";
                mods.dValue = std::to_string(pattern.accent);
            }
            break;

        case 1: // SEND EFFECTS
            {
                mods.aName = "TIME"; // delay time
                mods.bName = "FDBK"; // delay feedback
                mods.cName = "PAN";
                mods.dName = "--";

                mods.aValue = std::to_string(round(delayTimeMs * 100) / 100);
                mods.bValue = std::to_string(round(delayFeedback * 100));
                mods.cValue = "--";
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

        auto soundType = activePatternSounds[trackNum].type;

        switch (soundType)
        {
        case T_MONO_SAMPLE:
            mods = getMonoSampleControlModData();
            break;

        case T_MONO_SYNTH:
            mods = getMonoSynthControlModData();
            break;
#ifndef NO_DEXED
        case T_DEXED_SYNTH:
            mods = getDexedSynthControlModData();
            break;
#endif
#ifndef NO_FMDRUM
            case T_FM_DRUM:
            mods = getFmDrumControlModData();
            break;
#endif
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

    SOUND_CONTROL_MODS getMonoSampleControlModData()
    {
        SOUND_CONTROL_MODS mods;

        auto &currentSelectedTrack = XRSequencer::getCurrentSelectedTrack();
        auto currentSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();

            // TODO: add step mod support back in

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            {
                mods.aName = "L.STEP";
                mods.bName = "VELO";
                mods.cName = "PROB";
                mods.dName = "FILE";

                mods.aValue = std::to_string(currentSelectedTrack.lstep);
                mods.bValue = std::to_string(currentSelectedTrack.velocity);
                mods.cValue = "--";
                mods.dValue = "--";
            }

            break;
        case 1: // SAMPLE
            {
                mods.aName = "SPEED";
                mods.bName = "--";
                mods.cName = "--";
                mods.dName = "--";

                auto sampleplayrate = getValueNormalizedAsFloat(
                    activePatternSounds[currentSelectedTrackNum].params[MSMP_SAMPLEPLAYRATE]
                );

                mods.aValue = getPlaybackSpeedStr(sampleplayrate);
                mods.bValue = "--";
                mods.cValue = "--";
                mods.dValue = "--";
            }

            break;

        case 2: // LOOPING
            {
                mods.aName = "TYPE";
                mods.bName = "START";
                mods.cName = "FINISH";
                mods.dName = "PLAYST";

                mods.aValue = getLoopTypeName();

                auto loopstartToUse = getValueNormalizedAsUInt32(
                    activePatternSounds[currentSelectedTrackNum].params[MSMP_LOOPSTART]
                );

                std::string lsStr = std::to_string(loopstartToUse);
                lsStr += "ms";

                mods.bValue = lsStr;

                auto loopfinishToUse = getValueNormalizedAsUInt32(
                    activePatternSounds[currentSelectedTrackNum].params[MSMP_LOOPFINISH]
                );

                // if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStepNum > -1)
                // {
                //     loopfinishToUse = modsForCurrentTrackStep.loopfinish;
                // }

                std::string lfStr = std::to_string(loopfinishToUse);
                lfStr += "ms";

                mods.cValue = lfStr;

                auto playstartToUse = (play_start)getValueNormalizedAsInt8(
                    activePatternSounds[currentSelectedTrackNum].params[MSMP_PLAYSTART]
                );

                // if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStepNum > -1)
                // {
                //     playstartToUse = modsForCurrentTrackStep.playstart;
                // }

                mods.dValue = playstartToUse == play_start::play_start_loop ? "LOOP" : "SAMPLE";
            }

            break;
        case 3: // AMP ENV
            {
                mods.aName = "ATT";
                mods.bName = "DEC";
                mods.cName = "SUS";
                mods.dName = "REL";

                auto aatt = getValueNormalizedAsFloat(activePatternSounds[currentSelectedTrackNum].params[MSMP_AMP_ATTACK]);
                auto adec = getValueNormalizedAsFloat(activePatternSounds[currentSelectedTrackNum].params[MSMP_AMP_DECAY]);
                auto asus = getValueNormalizedAsFloat(activePatternSounds[currentSelectedTrackNum].params[MSMP_AMP_SUSTAIN]);
                auto arel = getValueNormalizedAsFloat(activePatternSounds[currentSelectedTrackNum].params[MSMP_AMP_RELEASE]);

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

        case 4: // OUTPUT
            {
                mods.aName = "LEVEL";
                mods.bName = "PAN";
                mods.cName = "--";
                mods.dName = "--"; // fx send?

                auto lvl = getValueNormalizedAsFloat(activePatternSounds[currentSelectedTrackNum].params[MSMP_LEVEL]);
                auto pan = getValueNormalizedAsFloat(activePatternSounds[currentSelectedTrackNum].params[MSMP_PAN]);

                mods.aValue = std::to_string(round(lvl * 100));
                mods.bValue = std::to_string((float)round(pan * 100) / 100);
                mods.bValue = mods.bValue.substr(0, 3);
                mods.bFloatValue = pan;
                mods.bType = RANGE;

                mods.cValue = "--";
                mods.dValue = "--";
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

        auto &currentSelectedTrack = XRSequencer::getCurrentSelectedTrack();
        auto currentSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto currSelectedStep = XRSequencer::getCurrentSelectedStepNum();
        auto currentUXMode = XRUX::getCurrentMode();

        auto currentSoundForTrack = activePatternSounds[currentSelectedTrackNum];

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            {
                mods.aName = "LSTP";
                mods.bName = "LEN";
                mods.cName = "VELO";
                mods.dName = "PROB";

                auto len = currentSelectedTrack.length;

                if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStep > -1) {
                    if (XRSequencer::activeTrackStepModLayer.tracks[currentSelectedTrackNum].steps[currSelectedStep].flags[XRSequencer::LENGTH]) {
                        len = XRSequencer::activeTrackStepModLayer.tracks[currentSelectedTrackNum].steps[currSelectedStep].mods[XRSequencer::LENGTH];
                    }
                }

                mods.aValue = std::to_string(currentSelectedTrack.lstep);
                mods.bValue = std::to_string(len);
                mods.cValue = std::to_string(currentSelectedTrack.velocity);
                mods.dValue = "100%"; // TODO: impl
            }

            break;

        case 1: // OSC
            {
                mods.aName = "WAVE";
                mods.bName = "DET";
                mods.cName = "FINE";
                mods.dName = "WID";

                auto waveform = getValueNormalizedAsUInt8(currentSoundForTrack.params[MSYN_WAVE]);
                auto detune = getValueNormalizedAsInt8(currentSoundForTrack.params[MSYN_DETUNE]);
                auto fine = getValueNormalizedAsInt8(currentSoundForTrack.params[MSYN_FINE]);
                auto width = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_WIDTH]);

                if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStep > -1) {
                    if (XRSound::activePatternSoundStepModLayer.sounds[currentSelectedTrackNum].steps[currSelectedStep].flags[XRSound::MSYN_WAVE]) {
                        waveform = getWaveformNumber(
                            getValueNormalizedAsUInt8(
                                XRSound::activePatternSoundStepModLayer.sounds[currentSelectedTrackNum].steps[currSelectedStep].mods[XRSound::MSYN_WAVE]
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
                mods.aName = "NOIS";
                mods.bName = "FREQ";
                mods.cName = "RESO";
                mods.dName = "AMT";

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
                mods.aName = "ATT";
                mods.bName = "DEC";
                mods.cName = "SUS";
                mods.dName = "REL";

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
                mods.aName = "ATT";
                mods.bName = "DEC";
                mods.cName = "SUS";
                mods.dName = "REL";

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
                mods.aName = "LVL";
                mods.bName = "PAN";
                mods.cName = "CHK";
                mods.dName = "DLY"; // fx send?

                auto lvl = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_LEVEL]);
                auto pan = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_PAN]);
                auto dly = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_DELAY]);

                mods.aValue = std::to_string(round(lvl * 100));
                mods.bValue = std::to_string((float)round(pan * 100) / 100);
                mods.bValue = mods.bValue.substr(0, 3);
                mods.cValue = "--";
                mods.dValue = std::to_string(round(dly * 100));

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

        auto &currentSelectedTrack = XRSequencer::getCurrentSelectedTrack();
        auto currentSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto currSelectedStep = XRSequencer::getCurrentSelectedStepNum();
        auto currentUXMode = XRUX::getCurrentMode();

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            {
                mods.aName = "LSTP";
                mods.bName = "LEN";
                mods.cName = "VELO";
                mods.dName = "PROB";

                auto len = currentSelectedTrack.length;

                if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStep > -1) {
                    if (XRSequencer::activeTrackStepModLayer.tracks[currentSelectedTrackNum].steps[currSelectedStep].flags[XRSequencer::LENGTH]) {
                        len = XRSequencer::activeTrackStepModLayer.tracks[currentSelectedTrackNum].steps[currSelectedStep].mods[XRSequencer::LENGTH];
                    }
                }

                mods.aValue = std::to_string(currentSelectedTrack.lstep);
                mods.bValue = std::to_string(len);
                mods.cValue = std::to_string(currentSelectedTrack.velocity);
                mods.dValue = "--";
            }

            break;

        case 1: // FM1
            {
                mods.aName = "ALGO";
                mods.bName = "--";
                mods.cName = "--";
                mods.dName = "--";

                mods.aValue = "--";
                mods.bValue = "--";
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

        case 3: // OUTPUT
            {
                mods.aName = "LEVEL";
                mods.bName = "PAN";
                mods.cName = "--";
                mods.dName = "--"; // fx send?

                auto lvl = getValueNormalizedAsFloat(activePatternSounds[currentSelectedTrackNum].params[DEXE_LEVEL]);
                auto pan = getValueNormalizedAsFloat(activePatternSounds[currentSelectedTrackNum].params[DEXE_PAN]);

                mods.aValue = std::to_string(round(lvl * 100));
                mods.bValue = std::to_string((float)round(pan * 100) / 100);
                mods.bValue = mods.bValue.substr(0, 3);
                mods.bFloatValue = pan;
                mods.bType = RANGE;

                mods.cValue = "--";
                mods.dValue = "--";
            }

            break;

        case 4: // POLY
            {
                mods.aName = "MODE";
                mods.bName = "NOTE2";
                mods.cName = "NOTE3";
                mods.dName = "NOTE4";

                auto noteMode = getValueNormalizedAsInt8(activePatternSounds[currentSelectedTrackNum].params[DEXE_NOTE_MODE]);

                auto noteB = getValueNormalizedAsInt8(activePatternSounds[currentSelectedTrackNum].params[DEXE_NOTE_B]);
                auto noteC = getValueNormalizedAsInt8(activePatternSounds[currentSelectedTrackNum].params[DEXE_NOTE_C]);
                auto noteD = getValueNormalizedAsInt8(activePatternSounds[currentSelectedTrackNum].params[DEXE_NOTE_D]);

                if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currSelectedStep > -1) {
                    if (XRSound::activePatternSoundStepModLayer.sounds[currentSelectedTrackNum].steps[currSelectedStep].flags[XRSound::DEXE_NOTE_B]) {
                        noteB = getValueNormalizedAsInt8(
                            XRSound::activePatternSoundStepModLayer.sounds[currentSelectedTrackNum].steps[currSelectedStep].mods[XRSound::DEXE_NOTE_B]
                        );
                    }
                    if (XRSound::activePatternSoundStepModLayer.sounds[currentSelectedTrackNum].steps[currSelectedStep].flags[XRSound::DEXE_NOTE_C]) {
                        noteC = getValueNormalizedAsInt8(
                            XRSound::activePatternSoundStepModLayer.sounds[currentSelectedTrackNum].steps[currSelectedStep].mods[XRSound::DEXE_NOTE_C]
                        );
                    }
                    if (XRSound::activePatternSoundStepModLayer.sounds[currentSelectedTrackNum].steps[currSelectedStep].flags[XRSound::DEXE_NOTE_D]) {
                        noteD = getValueNormalizedAsInt8(
                            XRSound::activePatternSoundStepModLayer.sounds[currentSelectedTrackNum].steps[currSelectedStep].mods[XRSound::DEXE_NOTE_D]
                        );
                    }
                }

                mods.aValue = (noteMode > 0) ? "POLY" : "MONO";
                mods.bValue = (noteB != 0) ? std::to_string(noteB) : "--";
                mods.cValue = (noteC != 0) ? std::to_string(noteC) : "--";
                mods.dValue = (noteD != 0) ? std::to_string(noteD) : "--";
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

        auto currentSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        auto freq = getValueNormalizedAsUInt32(activePatternSounds[currentSelectedTrackNum].params[FMD_FREQ]);
        auto fm = getValueNormalizedAsFloat(activePatternSounds[currentSelectedTrackNum].params[FMD_FM]);
        auto dec = getValueNormalizedAsFloat(activePatternSounds[currentSelectedTrackNum].params[FMD_DECAY]);
        auto nse = getValueNormalizedAsFloat(activePatternSounds[currentSelectedTrackNum].params[FMD_NOISE]);

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            {
                mods.aName = "TUNE";
                mods.bName = "FM";
                mods.cName = "DECAY";
                mods.dName = "NOISE";

                mods.aValue = std::to_string(freq);
                mods.aValue += "hz";
                mods.bValue = std::to_string((float)round(fm * 100) / 100);
                mods.bValue = mods.bValue.substr(0, 4);
                mods.cValue = std::to_string((float)round(dec * 100) / 100);
                mods.cValue = mods.cValue.substr(0, 4);
                mods.dValue = std::to_string((float)round(nse * 100) / 100);
                mods.dValue = mods.dValue.substr(0, 4);
            }

            break;

        case 1: // OUTPUT
            {
                mods.aName = "LEVEL";
                mods.bName = "PAN";
                mods.cName = "--";
                mods.dName = "--"; // fx send?

                auto lvl = getValueNormalizedAsFloat(activePatternSounds[currentSelectedTrackNum].params[FMD_LEVEL]);
                auto pan = getValueNormalizedAsFloat(activePatternSounds[currentSelectedTrackNum].params[FMD_PAN]);

                mods.aValue = std::to_string(round(lvl * 100));
                mods.bValue = std::to_string((float)round(pan * 100) / 100);
                mods.bValue = mods.bValue.substr(0, 3);
                mods.bFloatValue = pan;
                mods.bType = RANGE;

                mods.cValue = "--";
                mods.dValue = "--";
            }
            
            break;

        default:
            break;
        }

        return mods;
    }

    SOUND_CONTROL_MODS getMidiControlModData()
    {
        SOUND_CONTROL_MODS mods;

        auto &currentSelectedTrack = XRSequencer::getCurrentSelectedTrack();
        auto currentSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto currentSoundForTrack = activePatternSounds[currentSelectedTrackNum];

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            {
                mods.aName = "LSTP";
                mods.bName = "LEN";
                mods.cName = "CHAN";
                mods.dName = "VELO";

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

        auto currentSoundForTrack = activePatternSounds[currentSelectedTrackNum];

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
        {
            mods.aName = "LSTP";
            mods.bName = "LEN";
            mods.cName = "OUT";
            mods.dName = "PROB";

            mods.aValue = std::to_string(currentSelectedTrack.lstep);
            mods.bValue = std::to_string(currentSelectedTrack.length);

            auto port = getValueNormalizedAsInt8(currentSoundForTrack.params[0]); // TODO make enum

            std::string outputPortStr = std::to_string(port);
            outputPortStr += "AB";

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

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            {
                mods.aName = "LSTP";
                mods.bName = "OUT";
                mods.cName = "PROB";
                mods.dName = "--";

                mods.aValue = std::to_string(currentSelectedTrack.lstep);
                mods.bValue = "1AB";  // TODO: impl
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

        auto msmpLooptype = getValueNormalizedAsUInt8(activePatternSounds[track].params[MSMP_LOOPTYPE]);
        auto msmpLoopstart = getValueNormalizedAsInt32(activePatternSounds[track].params[MSMP_LOOPSTART]);
        auto msmpLoopfinish = getValueNormalizedAsInt32(activePatternSounds[track].params[MSMP_LOOPFINISH]);
        auto msmpPlaystart = (play_start)getValueNormalizedAsUInt8(activePatternSounds[track].params[MSMP_PLAYSTART]);
        auto msmpAatt = getValueNormalizedAsFloat(activePatternSounds[track].params[MSMP_AMP_ATTACK]);
        auto msmpAdec = getValueNormalizedAsFloat(activePatternSounds[track].params[MSMP_AMP_DECAY]);
        auto msmpAsus = getValueNormalizedAsFloat(activePatternSounds[track].params[MSMP_AMP_SUSTAIN]);
        auto msmpArel = getValueNormalizedAsFloat(activePatternSounds[track].params[MSMP_AMP_RELEASE]);
        auto msmpPan = getValueNormalizedAsFloat(activePatternSounds[track].params[MSMP_PAN]);
        auto msmpLvl = getValueNormalizedAsFloat(activePatternSounds[track].params[MSMP_LEVEL]);

        AudioNoInterrupts();

        monoSampleInstances[track].ampEnv.attack(msmpAatt);
        monoSampleInstances[track].ampEnv.decay(msmpAdec);
        monoSampleInstances[track].ampEnv.sustain(msmpAsus);
        monoSampleInstances[track].ampEnv.release(msmpArel);
        
        monoSampleInstances[track].ampAccent.gain(trackToUse.velocity * 0.01);

        monoSampleInstances[track].amp.gain(msmpLvl);

        monoSampleInstances[track].left.gain(getStereoPanValues(msmpPan).left);
        monoSampleInstances[track].right.gain(getStereoPanValues(msmpPan).right);

        AudioInterrupts();

        monoSampleInstances[track].ampEnv.noteOn();

        std::string trackSampleName(activePatternSounds[track].sampleName);

        // if sample has valid name, assume it is loaded in PSRAM and can be played
        if (trackSampleName.length() > 0) {
            const auto &sampleName = std::string("/audio enjoyer/xr-1/samples/") + std::string(trackSampleName);

            newdigate::audiosample *sample = XRAsyncPSRAMLoader::getReadSample(&sampleName);
            if (sample != nullptr) {
                monoSampleInstances[track].sample.playRaw(
                    sample->sampledata,
                    sample->samplesize / 2,
                    _numChannels
                );
            }
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
       
        auto msynWave = getValueNormalizedAsInt8(activePatternSounds[track].params[MSYN_WAVE]);
        auto msynFine = getValueNormalizedAsInt8(activePatternSounds[track].params[MSYN_FINE]);
        auto msynDetune = getValueNormalizedAsInt8(activePatternSounds[track].params[MSYN_DETUNE]);
        auto msynFatt = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_FILTER_ATTACK]);
        auto msynFdec = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_FILTER_DECAY]);
        auto msynFsus = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_FILTER_SUSTAIN]);
        auto msynAatt = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_AMP_ATTACK]);
        auto msynAdec = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_AMP_DECAY]);
        auto msynAsus = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_AMP_SUSTAIN]);
        auto msynArel = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_AMP_RELEASE]);
        auto msynPan = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_PAN]);
        auto msynLvl = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_LEVEL]);
        auto msynDly = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_DELAY]);

        float foundBaseFreq = _noteToFreqArr[trackToUse.note];
        float octaveFreqA = (foundBaseFreq + (msynFine * 0.01)) * (pow(2, trackToUse.octave));
        float octaveFreqB = (foundBaseFreq * pow(2.0, (float)msynDetune / 12.0)) * (pow(2, trackToUse.octave));
        
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

        monoSynthInstances[track].filterAccent.gain((trackToUse.velocity * 0.01));
        monoSynthInstances[track].ampAccent.gain((trackToUse.velocity * 0.01));
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
#ifndef NO_DEXED
    void handleDexedSynthNoteOnForTrack(int track)
    {
        if (track > 3) return;

        auto &trackToUse = XRSequencer::getTrack(track);

        auto dexeLvl = getValueNormalizedAsFloat(activePatternSounds[track].params[DEXE_LEVEL]);
        auto dexeDly = getValueNormalizedAsFloat(activePatternSounds[track].params[DEXE_DELAY]);

        uint8_t noteToUse = trackToUse.note;
        uint8_t octaveToUse = trackToUse.octave;

        int midiNote = (noteToUse + (12 * (octaveToUse)));

        dexedInstances[track].ampAccent.gain((trackToUse.velocity * 0.01));
        dexedInstances[track].amp.gain(dexeLvl);
        dexedInstances[track].ampDelaySend.gain(0);

        dexedInstances[track].dexed.keydown(midiNote, 50); // TODO: parameterize velocity
    }
#endif

    void handleBraidsNoteOnForTrack(int track)
    {
        // auto &trackToUse = XRSequencer::getTrack(track);

        // uint8_t noteToUse = trackToUse.note;
        // uint8_t octaveToUse = trackToUse.octave;

        // int midiNote = (noteToUse + (12 * (octaveToUse)));

        // braidsInstances[track].braids.set_braids_pitch(midiNote << 7);
    }
#ifndef NO_FMDRUM
    void handleFmDrumNoteOnForTrack(int track)
    {
        if (track > 2) return;

        auto &trackToUse = XRSequencer::getTrack(track);

        auto fmDrumLvl = getValueNormalizedAsFloat(activePatternSounds[track].params[FMD_LEVEL]);
        auto fmDrumDly = getValueNormalizedAsFloat(activePatternSounds[track].params[FMD_DELAY]);

        fmDrumInstances[track].ampAccent.gain((trackToUse.velocity * 0.01));
        fmDrumInstances[track].amp.gain(fmDrumLvl);
        fmDrumInstances[track].ampDelaySend.gain(0);

        fmDrumInstances[track].fmDrum.noteOn();
    }
#endif

    void handleMIDINoteOnForTrack(int track)
    {
        // TODO: impl
    }

    void handleCvGateNoteOnForTrack(int track)
    {
        // TODO: impl
    }

    void handleMonoSampleNoteOnForTrackStep(int track, int step)
    {
        auto &trackToUse = XRSequencer::getTrack(track);
        auto &stepToUse = XRSequencer::getStep(track, step);

        auto msmpSamplePlayRate = getValueNormalizedAsFloat(activePatternSounds[track].params[MSMP_SAMPLEPLAYRATE]);
        auto msmpLooptype = getValueNormalizedAsUInt8(activePatternSounds[track].params[MSMP_LOOPTYPE]);
        auto msmpLoopstart = getValueNormalizedAsInt32(activePatternSounds[track].params[MSMP_LOOPSTART]);
        auto msmpLoopfinish = getValueNormalizedAsInt32(activePatternSounds[track].params[MSMP_LOOPFINISH]);
        auto msmpPlaystart = (play_start)getValueNormalizedAsUInt8(activePatternSounds[track].params[MSMP_PLAYSTART]);
        auto msmpAatt = getValueNormalizedAsFloat(activePatternSounds[track].params[MSMP_AMP_ATTACK]);
        auto msmpAdec = getValueNormalizedAsFloat(activePatternSounds[track].params[MSMP_AMP_DECAY]);
        auto msmpAsus = getValueNormalizedAsFloat(activePatternSounds[track].params[MSMP_AMP_SUSTAIN]);
        auto msmpArel = getValueNormalizedAsFloat(activePatternSounds[track].params[MSMP_AMP_RELEASE]);
        auto msmpPan = getValueNormalizedAsFloat(activePatternSounds[track].params[MSMP_PAN]);
        auto msmpLvl = getValueNormalizedAsFloat(activePatternSounds[track].params[MSMP_LEVEL]);
        auto msmpDly = getValueNormalizedAsFloat(activePatternSounds[track].params[MSMP_DELAY]);

        // TODO: allow sample chromatic note playback

        auto velocityToUse = trackToUse.velocity;
        if (stepToUse.state == XRSequencer::STATE_ACCENTED) {
            velocityToUse = max(trackToUse.velocity, XRSequencer::activePattern.accent);
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
        if (activePatternSoundStepModLayer.sounds[track].steps[step].flags[MSMP_LOOPTYPE])
        {
            looptypeToUse = getValueNormalizedAsUInt8(activePatternSoundStepModLayer.sounds[track].steps[step].mods[MSMP_LOOPTYPE]);
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

        std::string trackSampleName(activePatternSounds[track].sampleName);
        
        AudioNoInterrupts();

        monoSampleInstances[track].ampEnv.attack(msmpAatt);
        monoSampleInstances[track].ampEnv.decay(msmpAdec);
        monoSampleInstances[track].ampEnv.sustain(msmpAsus);
        monoSampleInstances[track].ampEnv.release(msmpArel);
        
        monoSampleInstances[track].ampAccent.gain(velocityToUse * 0.01);
        monoSampleInstances[track].amp.gain(msmpLvl);
        monoSampleInstances[track].ampDelaySend.gain(0);

        monoSampleInstances[track].left.gain(getStereoPanValues(msmpPan).left);
        monoSampleInstances[track].right.gain(getStereoPanValues(msmpPan).right);

        monoSampleInstances[track].sample.setPlaybackRate(speedToUse);
        
        AudioInterrupts();

        monoSampleInstances[track].ampEnv.noteOn();

        // if sample has valid name, assume it is loaded in PSRAM and can be played
        if (trackSampleName.length() > 0) {
            const auto &sampleName = std::string("/audio enjoyer/xr-1/samples/") + std::string(trackSampleName);

            newdigate::audiosample *sample = XRAsyncPSRAMLoader::getReadSample(&sampleName);
            if (sample != nullptr) {
                monoSampleInstances[track].sample.playRaw(
                    sample->sampledata,
                    sample->samplesize / 2,
                    _numChannels
                );
            }
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

        auto &trackToUse = XRSequencer::getTrack(track);
        auto &stepToUse = XRSequencer::getStep(track, step);

        auto msynWave = getValueNormalizedAsUInt8(activePatternSounds[track].params[MSYN_WAVE]);
        auto msynFine = getValueNormalizedAsInt8(activePatternSounds[track].params[MSYN_FINE]);
        auto msynDetune = getValueNormalizedAsInt8(activePatternSounds[track].params[MSYN_DETUNE]);
        auto msynFatt = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_FILTER_ATTACK]);
        auto msynFdec = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_FILTER_DECAY]);
        auto msynFsus = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_FILTER_SUSTAIN]);
        auto msynFrel = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_FILTER_RELEASE]);
        auto msynAatt = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_AMP_ATTACK]);
        auto msynAdec = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_AMP_DECAY]);
        auto msynAsus = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_AMP_SUSTAIN]);
        auto msynArel = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_AMP_RELEASE]);
        auto msynPan = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_PAN]);
        auto msynLvl = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_LEVEL]);
        auto msynDly = getValueNormalizedAsFloat(activePatternSounds[track].params[MSYN_DELAY]);

        auto currLayer = XRSequencer::getCurrentSelectedTrackLayerNum();

        uint8_t noteToUse = trackToUse.note;
        if (XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].flags[XRSequencer::NOTE])
        {
            noteToUse = XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].mods[XRSequencer::NOTE];
        }

        uint8_t octaveToUse = trackToUse.octave;
        if (XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].flags[XRSequencer::OCTAVE])
        {
            octaveToUse = XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].mods[XRSequencer::OCTAVE];
        }

        uint8_t velocityToUse = trackToUse.velocity;
        if (stepToUse.state == XRSequencer::STATE_ACCENTED) {
            if (XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].flags[XRSequencer::VELOCITY]) {
                velocityToUse = XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].mods[XRSequencer::VELOCITY];
            } else {
                velocityToUse = max(trackToUse.velocity, XRSequencer::activePattern.accent);
            }
        }

        int waveformToUse = msynWave;
        if (activePatternSoundStepModLayer.sounds[track].steps[step].flags[MSYN_WAVE])
        {
            waveformToUse = getWaveformNumber(
                getValueNormalizedAsUInt8(activePatternSoundStepModLayer.sounds[track].steps[step].mods[MSYN_WAVE])
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
#ifndef NO_DEXED
    void handleDexedSynthNoteOnForTrackStep(int track, int step)
    {
        if (track > 3) return;

        auto &trackToUse = XRSequencer::getTrack(track);
        auto &stepToUse = XRSequencer::getStep(track, step);
        auto currLayer = XRSequencer::getCurrentSelectedTrackLayerNum();

        uint8_t noteToUse = trackToUse.note;
        if (XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].flags[XRSequencer::NOTE])
        {
            noteToUse = XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].mods[XRSequencer::NOTE];
        }

        uint8_t octaveToUse = trackToUse.octave;
        if (XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].flags[XRSequencer::OCTAVE])
        {
            octaveToUse = XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].mods[XRSequencer::OCTAVE];
        }

        uint8_t velocityToUse = trackToUse.velocity;
        if (stepToUse.state == XRSequencer::STATE_ACCENTED) {
            if (XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].flags[XRSequencer::VELOCITY]) {
                velocityToUse = XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].mods[XRSequencer::VELOCITY];
            } else {
                velocityToUse = max(trackToUse.velocity, XRSequencer::activePattern.accent);
            }
        }

        auto dexePan = getValueNormalizedAsFloat(activePatternSounds[track].params[DEXE_PAN]);
        auto dexeLvl = getValueNormalizedAsFloat(activePatternSounds[track].params[DEXE_LEVEL]);
        auto dexeDly = getValueNormalizedAsFloat(activePatternSounds[track].params[DEXE_DELAY]);
        
        dexedInstances[track].ampAccent.gain((velocityToUse * 0.01));
        dexedInstances[track].amp.gain(dexeLvl);
        dexedInstances[track].ampDelaySend.gain(0);

        dexedInstances[track].left.gain(getStereoPanValues(dexePan).left);
        dexedInstances[track].right.gain(getStereoPanValues(dexePan).right);

        // handle poly note assignments
        // TODO: do check for mono mode so we're not redundantly keydowning all notes when in mono mode?
        int noteMode = getValueNormalizedAsInt8(activePatternSounds[track].params[DEXE_NOTE_MODE]);

        int noteB = getValueNormalizedAsInt8(activePatternSounds[track].params[DEXE_NOTE_B]);
        int noteC = getValueNormalizedAsInt8(activePatternSounds[track].params[DEXE_NOTE_C]);
        int noteD = getValueNormalizedAsInt8(activePatternSounds[track].params[DEXE_NOTE_D]);

        if (XRSound::activePatternSoundStepModLayer.sounds[track].steps[step].flags[XRSound::DEXE_NOTE_B]) {
            noteB = getValueNormalizedAsInt8(
                XRSound::activePatternSoundStepModLayer.sounds[track].steps[step].mods[XRSound::DEXE_NOTE_B]
            );
        }
        if (XRSound::activePatternSoundStepModLayer.sounds[track].steps[step].flags[XRSound::DEXE_NOTE_C]) {
            noteC = getValueNormalizedAsInt8(
                XRSound::activePatternSoundStepModLayer.sounds[track].steps[step].mods[XRSound::DEXE_NOTE_C]
            );
        }
        if (XRSound::activePatternSoundStepModLayer.sounds[track].steps[step].flags[XRSound::DEXE_NOTE_D]) {
            noteD = getValueNormalizedAsInt8(
                XRSound::activePatternSoundStepModLayer.sounds[track].steps[step].mods[XRSound::DEXE_NOTE_D]
            );
        }

        int midiNoteA = (noteToUse + (12 * (octaveToUse)));
        int midiNoteB = (noteToUse + noteB + (12 * (octaveToUse)));
        int midiNoteC = (noteToUse + noteC + (12 * (octaveToUse)));
        int midiNoteD = (noteToUse + noteD + (12 * (octaveToUse)));

        dexedInstances[track].dexed.keydown(midiNoteA, velocityToUse);
        if (noteMode == 1) { // 0 = mono, 1 = poly
            if (noteB != 0) dexedInstances[track].dexed.keydown(midiNoteB, velocityToUse);
            if (noteC != 0) dexedInstances[track].dexed.keydown(midiNoteC, velocityToUse);
            if (noteD != 0) dexedInstances[track].dexed.keydown(midiNoteD, velocityToUse);
        }
    }
#endif

    void handleBraidsNoteOnForTrackStep(int track, int step)
    {
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
        //         velocityToUse = max(trackToUse.velocity, XRSequencer::activePattern.accent);
        //     }
        // }

        // int midiNote = (noteToUse + (12 * (octaveToUse)));

        // braidsInstances[track].braids.set_braids_pitch(midiNote << 7);
    }
#ifndef NO_FMDRUM
    void handleFmDrumNoteOnForTrackStep(int track, int step)
    {
        if (track > 2) return;

        auto &trackToUse = XRSequencer::getTrack(track);
        auto &stepToUse = XRSequencer::getStep(track, step);

        auto fmdNoise = getValueNormalizedAsFloat(activePatternSounds[track].params[FMD_NOISE]);
        auto fmdDecay = getValueNormalizedAsFloat(activePatternSounds[track].params[FMD_DECAY]);
        auto fmdFm = getValueNormalizedAsFloat(activePatternSounds[track].params[FMD_FM]);
        auto fmdFreq = getValueNormalizedAsUInt8(activePatternSounds[track].params[FMD_FREQ]);
        auto fmdPan = getValueNormalizedAsFloat(activePatternSounds[track].params[FMD_PAN]);
        auto fmdLvl = getValueNormalizedAsFloat(activePatternSounds[track].params[FMD_LEVEL]);
        auto fmdDly = getValueNormalizedAsFloat(activePatternSounds[track].params[FMD_DELAY]);

        auto currLayer = XRSequencer::getCurrentSelectedTrackLayerNum();

        uint8_t velocityToUse = trackToUse.velocity;
        if (stepToUse.state == XRSequencer::STATE_ACCENTED) {
            if (XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].flags[XRSequencer::VELOCITY]) {
                velocityToUse = XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].mods[XRSequencer::VELOCITY];
            } else {
                velocityToUse = max(trackToUse.velocity, XRSequencer::activePattern.accent);
            }
        }

        AudioNoInterrupts();

        fmDrumInstances[track].fmDrum.noise(fmdNoise);
        fmDrumInstances[track].fmDrum.decay(fmdDecay);
        fmDrumInstances[track].fmDrum.fm(fmdFm);
        fmDrumInstances[track].fmDrum.frequency(fmdFreq);

        fmDrumInstances[track].ampAccent.gain((velocityToUse * 0.01));
        fmDrumInstances[track].amp.gain(fmdLvl);
        fmDrumInstances[track].ampDelaySend.gain(0);

        fmDrumInstances[track].left.gain(getStereoPanValues(fmdPan).left);
        fmDrumInstances[track].right.gain(getStereoPanValues(fmdPan).right);
        
        AudioInterrupts();

        fmDrumInstances[track].fmDrum.noteOn();
    }
#endif

    void handleMIDINoteOnForTrackStep(int track, int step)
    {
        //auto &trackToUse = XRSequencer::getTrack(track);
        //auto &stepToUse = XRSequencer::getStep(track, step);

        XRMIDI::sendNoteOn(64, 100, 1);
    }

    void handleCvGateNoteOnForTrackStep(int track, int step)
    {
        auto &trackToUse = XRSequencer::getTrack(track);

        auto currentSoundForTrack = activePatternSounds[track];

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
        for (size_t t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++) {
            if (t < 4) { // dexed voice instances are only available to tracks 0-3
                dexedInstances[t].dexed.notesOff();
            }
        }
    }

    void handleNoteOffForTrack(int track)
    {
        auto &trackToUse = XRSequencer::getTrack(track);
        auto currentSoundForTrack = activePatternSounds[track];

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
#ifndef NO_DEXED
        case T_DEXED_SYNTH:
            {
                if (track < 4)
                {
                    uint8_t noteToUse = trackToUse.note;
                    uint8_t octaveToUse = trackToUse.octave;
                    int midiNote = (noteToUse + (12 * (octaveToUse)));

                    dexedInstances[track].dexed.keyup(midiNote);
                }
            }
            break;
#endif
#ifndef NO_FMDRUM
        case T_FM_DRUM:
            // n/a
            break;
#endif
        case T_MIDI:
            {
                XRMIDI::sendNoteOff(64, 100, 1);
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
        auto currentSoundForTrack = activePatternSounds[track];

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
#ifndef NO_DEXED
        case T_DEXED_SYNTH:
            {
                uint8_t noteToUse = trackToUse.note;
                if (XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].flags[XRSequencer::NOTE])
                {
                    noteToUse = XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].mods[XRSequencer::NOTE];
                }

                uint8_t octaveToUse = trackToUse.octave;
                if (XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].flags[XRSequencer::OCTAVE])
                {
                    octaveToUse = XRSequencer::activeTrackStepModLayer.tracks[track].steps[step].mods[XRSequencer::OCTAVE];
                }

                // handle poly note assignments
                // TODO: do check for mono mode so we're not redundantly keydowning all notes when in mono mode?

                int noteB = getValueNormalizedAsInt8(activePatternSounds[track].params[DEXE_NOTE_B]);
                int noteC = getValueNormalizedAsInt8(activePatternSounds[track].params[DEXE_NOTE_C]);
                int noteD = getValueNormalizedAsInt8(activePatternSounds[track].params[DEXE_NOTE_D]);

                if (XRSound::activePatternSoundStepModLayer.sounds[track].steps[step].flags[XRSound::DEXE_NOTE_B]) {
                    noteB = getValueNormalizedAsInt8(
                        XRSound::activePatternSoundStepModLayer.sounds[track].steps[step].mods[XRSound::DEXE_NOTE_B]
                    );
                }
                if (XRSound::activePatternSoundStepModLayer.sounds[track].steps[step].flags[XRSound::DEXE_NOTE_C]) {
                    noteC = getValueNormalizedAsInt8(
                        XRSound::activePatternSoundStepModLayer.sounds[track].steps[step].mods[XRSound::DEXE_NOTE_C]
                    );
                }
                if (XRSound::activePatternSoundStepModLayer.sounds[track].steps[step].flags[XRSound::DEXE_NOTE_D]) {
                    noteD = getValueNormalizedAsInt8(
                        XRSound::activePatternSoundStepModLayer.sounds[track].steps[step].mods[XRSound::DEXE_NOTE_D]
                    );
                }

                int midiNoteA = (noteToUse + (12 * (octaveToUse)));
                int midiNoteB = (noteToUse + noteB + (12 * (octaveToUse)));
                int midiNoteC = (noteToUse + noteC + (12 * (octaveToUse)));
                int midiNoteD = (noteToUse + noteD + (12 * (octaveToUse)));

                dexedInstances[track].dexed.keyup(midiNoteA);
                if (noteB != 0) dexedInstances[track].dexed.keyup(midiNoteB);
                if (noteC != 0) dexedInstances[track].dexed.keyup(midiNoteC);
                if (noteD != 0) dexedInstances[track].dexed.keyup(midiNoteD);
            }
            break;
#endif
#ifndef NO_FMDRUM
        case T_FM_DRUM:
            // n/a
            break;
#endif
        case T_MIDI:
            {
                XRMIDI::sendNoteOff(64, 100, 1);
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
        // TODO: impl async loading so there's no audible SPI noise from reading the SD card
        // also, impl freeing any existing sample from the track and loading the new sample (if changing sample)

        auto track = XRSequencer::getCurrentSelectedTrackNum();

        std::string sampleNameStr = "/audio enjoyer/xr-1/samples/";
        std::string selected = XRSD::getCurrSampleFileHighlighted();
        sampleNameStr += selected;
        
        strcpy(activePatternSounds[track].sampleName, selected.c_str());
        // TODO: Nic: when playing, we want to do it async - code below is fine when sequencer is not playing
        if (XRSequencer::getSeqState().playbackState == XRSequencer::RUNNING) {
            XRAsyncPSRAMLoader::addSampleFileNameForCurrentReadHeap(sampleNameStr);
        } else
            XRAsyncPSRAMLoader::loadSampleSync(&sampleNameStr);
        patternSoundsDirty = true;
    }

    void changeTrackSoundType(uint8_t t, SOUND_TYPE newType)
    {
        auto currType = activePatternSounds[t].type;
        if (currType == newType) return;

        activePatternSounds[t].type = newType;

        AudioNoInterrupts();

        initTrackSound(t);

        AudioInterrupts();
    }

    void triggerTrackManually(uint8_t t, uint8_t note)
    {
        auto currentSoundForTrack = activePatternSounds[t];

        switch (currentSoundForTrack.type)
        {
        case T_MONO_SAMPLE:
            triggerMonoSampleNoteOn(t, note);

            break;
        case T_MONO_SYNTH:
            triggerMonoSynthNoteOn(t, note);
            
            break;
#ifndef NO_DEXED
        case T_DEXED_SYNTH:
            triggerDexedSynthNoteOn(t, note);
            
            break;
#endif
        case T_BRAIDS_SYNTH:
            triggerBraidsNoteOn(t, note);

            break;
#ifndef NO_FMDRUM
        case T_FM_DRUM:
            triggerFmDrumNoteOn(t, note);
            
            break;
#endif
        case T_CV_GATE:
            triggerCvGateNoteOn(t, note);
            
            break;
        
        default:
            break;
        }
    }

    void triggerMonoSampleNoteOn(uint8_t t, uint8_t note)
    {
        auto &trackToUse = XRSequencer::getTrack(t);

        auto msmpLooptype = getValueNormalizedAsUInt8(activePatternSounds[t].params[MSMP_LOOPTYPE]);
        auto msmpLoopstart = getValueNormalizedAsInt32(activePatternSounds[t].params[MSMP_LOOPSTART]);
        auto msmpLoopfinish = getValueNormalizedAsInt32(activePatternSounds[t].params[MSMP_LOOPFINISH]);
        auto msmpPlaystart = (play_start)getValueNormalizedAsUInt8(activePatternSounds[t].params[MSMP_PLAYSTART]);
        auto msmpAatt = getValueNormalizedAsFloat(activePatternSounds[t].params[MSMP_AMP_ATTACK]);
        auto msmpAdec = getValueNormalizedAsFloat(activePatternSounds[t].params[MSMP_AMP_DECAY]);
        auto msmpAsus = getValueNormalizedAsFloat(activePatternSounds[t].params[MSMP_AMP_SUSTAIN]);
        auto msmpArel = getValueNormalizedAsFloat(activePatternSounds[t].params[MSMP_AMP_RELEASE]);
        auto msmpPan = getValueNormalizedAsFloat(activePatternSounds[t].params[MSMP_PAN]);
        auto msmpLvl = getValueNormalizedAsFloat(activePatternSounds[t].params[MSMP_LEVEL]);

        std::string trackSampleName(activePatternSounds[t].sampleName);

        AudioNoInterrupts();

        monoSampleInstances[t].ampEnv.attack(msmpAatt);
        monoSampleInstances[t].ampEnv.decay(msmpAdec);
        monoSampleInstances[t].ampEnv.sustain(msmpAsus);
        monoSampleInstances[t].ampEnv.release(msmpArel);

        monoSampleInstances[t].ampAccent.gain(trackToUse.velocity * 0.01);
        monoSampleInstances[t].amp.gain(msmpLvl);

        monoSampleInstances[t].left.gain(getStereoPanValues(msmpPan).left);
        monoSampleInstances[t].right.gain(getStereoPanValues(msmpPan).right);

        AudioInterrupts();

        monoSampleInstances[t].ampEnv.noteOn();

        if (trackSampleName.length() > 0) {
            const auto &sampleName = std::string("/audio enjoyer/xr-1/samples/") + std::string(trackSampleName);

            newdigate::audiosample *sample = XRAsyncPSRAMLoader::getReadSample(&sampleName);
            if (sample != nullptr) {
                monoSampleInstances[t].sample.playRaw(
                    sample->sampledata,
                    sample->samplesize / 2,
                    _numChannels
                );
            }
        }

        AudioNoInterrupts();

        // always re-initialize loop type
        monoSampleInstances[t].sample.setLoopType(loopTypeSelMap[msmpLooptype]);

        if (loopTypeSelMap[msmpLooptype] == looptype_none)
        {
            monoSampleInstances[t].sample.setPlayStart(play_start::play_start_sample);
            monoSampleInstances[t].sample.setLoopType(loop_type::looptype_none);
        }
        else if (loopTypeSelMap[msmpLooptype] == looptype_repeat)
        {
            monoSampleInstances[t].sample.setPlayStart(msmpPlaystart == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
            monoSampleInstances[t].sample.setLoopStart(msmpLoopstart);
            monoSampleInstances[t].sample.setLoopFinish(msmpLoopfinish);
        }

        AudioInterrupts();
    }

    void triggerMonoSynthNoteOn(uint8_t t, uint8_t note)
    {
        if (t > 3) return;

        auto &trackToUse = XRSequencer::getTrack(t);
       
        auto msynFine = getValueNormalizedAsInt8(activePatternSounds[t].params[MSYN_FINE]);
        auto msynDetune = getValueNormalizedAsInt8(activePatternSounds[t].params[MSYN_DETUNE]);
        auto msynFatt = getValueNormalizedAsFloat(activePatternSounds[t].params[MSYN_FILTER_ATTACK]);
        auto msynFdec = getValueNormalizedAsFloat(activePatternSounds[t].params[MSYN_FILTER_DECAY]);
        auto msynFsus = getValueNormalizedAsFloat(activePatternSounds[t].params[MSYN_FILTER_SUSTAIN]);
        auto msynFrel = getValueNormalizedAsFloat(activePatternSounds[t].params[MSYN_FILTER_RELEASE]);
        auto msynAatt = getValueNormalizedAsFloat(activePatternSounds[t].params[MSYN_AMP_ATTACK]);
        auto msynAdec = getValueNormalizedAsFloat(activePatternSounds[t].params[MSYN_AMP_DECAY]);
        auto msynAsus = getValueNormalizedAsFloat(activePatternSounds[t].params[MSYN_AMP_SUSTAIN]);
        auto msynArel = getValueNormalizedAsFloat(activePatternSounds[t].params[MSYN_AMP_RELEASE]);
        auto msynPan = getValueNormalizedAsFloat(activePatternSounds[t].params[MSYN_PAN]);
        auto msynLvl = getValueNormalizedAsFloat(activePatternSounds[t].params[MSYN_LEVEL]);

        AudioNoInterrupts();

        float foundBaseFreq = _noteToFreqArr[note];
        float octaveFreqA = (foundBaseFreq + (msynFine * 0.01)) * (pow(2, XRKeyMatrix::getKeyboardOctave()));
        float octaveFreqB = (foundBaseFreq * pow(2.0, (float)msynDetune / 12.0)) * (pow(2, XRKeyMatrix::getKeyboardOctave()));

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

        monoSynthInstances[t].filterAccent.gain(trackToUse.velocity * 0.01);
        monoSynthInstances[t].ampAccent.gain(trackToUse.velocity * 0.01);
        monoSynthInstances[t].amp.gain(msynLvl);

        Serial.printf("msynLvl: %f msynRel: %f\n", 
            msynLvl * (trackToUse.velocity * 0.01), 
            msynArel * (trackToUse.velocity * 0.01)
        );

        monoSynthInstances[t].left.gain(getStereoPanValues(msynPan).left);
        monoSynthInstances[t].right.gain(getStereoPanValues(msynPan).right);

        AudioInterrupts();

        // now trigger envs
        monoSynthInstances[t].ampEnv.noteOn();
        monoSynthInstances[t].filterEnv.noteOn();
    }
#ifndef NO_DEXED
    void triggerDexedSynthNoteOn(uint8_t t, uint8_t note)
    {
        if (t > 3) return;

        auto &trackToUse = XRSequencer::getTrack(t);

        int midiNote = (note + (12 * (XRKeyMatrix::getKeyboardOctave())));

        dexedInstances[t].ampAccent.gain(trackToUse.velocity * 0.01);
        dexedInstances[t].dexed.keydown(midiNote, 50);
    }
#endif
    void triggerBraidsNoteOn(uint8_t t, uint8_t note)
    {
        int midiNote = (note + (12 * (XRKeyMatrix::getKeyboardOctave())));

        // braidsInstances[t].braids.set_braids_pitch(midiNote << 7);
    }
#ifndef NO_FMDRUM
    void triggerFmDrumNoteOn(uint8_t t, uint8_t note)
    {
        if (t > 2) return;

        auto &trackToUse = XRSequencer::getTrack(t);

        fmDrumInstances[t].ampAccent.gain(trackToUse.velocity * 0.01);
        fmDrumInstances[t].fmDrum.noteOn();
    }
#endif

    void triggerCvGateNoteOn(uint8_t t, uint8_t note)
    {
        auto currentSoundForTrack = activePatternSounds[t];
        auto cvgaPort = getValueNormalizedAsInt8(currentSoundForTrack.params[0]); // TODO: use enum

        uint8_t noteToUse = note;
        uint8_t octaveToUse = XRKeyMatrix::getKeyboardOctave(); // +1 ?

        int midiNote = (noteToUse + (12 * (octaveToUse))); // C0 = 12

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

    void noteOffTrackManually(int noteOnKeyboard)
    {
        auto currSelTrackNum = XRSequencer::getCurrentSelectedTrackNum();

        switch (activePatternSounds[currSelTrackNum].type)
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
#ifndef NO_DEXED
        case T_DEXED_SYNTH:
            {
                int midiNote = (noteOnKeyboard + (12 * (XRKeyMatrix::getKeyboardOctave())));

                dexedInstances[currSelTrackNum].dexed.keyup(midiNote);
            }
            break;
#endif
        case T_BRAIDS_SYNTH:
            {
                // int midiNote = (noteOnKeyboard + (12 * (XRKeyMatrix::getKeyboardOctave())));

                // braidsInstances[currSelTrackNum].braids.set_braids_pitch(midiNote << 7);
            }
            break;
        case T_MIDI:
            {
                XRMIDI::sendNoteOff(64, 100, 1);
            }
            break;
        case T_CV_GATE:
            {
                auto cvgaPort = getValueNormalizedAsInt8(activePatternSounds[currSelTrackNum].params[0]); // TODO: use enum
                
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
            outputStr = "EMPTY";
            break;

        case T_MONO_SAMPLE:
            outputStr = "SAMPLE >";
            break;

        case T_MONO_SYNTH:
            outputStr = "MONO-SYNTH >";
            break;
#ifndef NO_DEXED
        case T_DEXED_SYNTH:
            outputStr = "FM-DEXED >";
            break;
#endif
        case T_BRAIDS_SYNTH:
            outputStr = "BRAIDS >";
            break;
#ifndef NO_FMDRUM
        case T_FM_DRUM:
            outputStr = "FM-DRUM >";
            break;
#endif
        case T_MIDI:
            outputStr = "MIDI-OUT";
            break;

        case T_CV_GATE:
            outputStr = "CV-GATE";
            break;

        case T_CV_TRIG:
            outputStr = "CV-TRIG";
            break;

        default:
            break;
        }

        return outputStr;
    }

    std::string getSoundTypeNameStr(SOUND_TYPE type)
    {
        std::string str = "EMPTY"; // default

        switch (type)
        {
        case T_MONO_SAMPLE:
            str = "INIT";

            break;

        case T_MONO_SYNTH:
            str = "INIT";

            break;
#ifndef NO_DEXED
        case T_DEXED_SYNTH:
            str = "INIT";

            break;
#endif
        case T_BRAIDS_SYNTH:
            str = "INIT";

            break;
#ifndef NO_FMDRUM
        case T_FM_DRUM:
            str = "INIT";

            break;
#endif
        case T_MIDI:
            str = "MIDI";

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

        std::string outputStr = soundCurrPageNameMap[activePatternSounds[currTrack].type][currPage];

        return outputStr;
    }

    uint8_t getPageCountForCurrentTrack()
    {
        auto currTrack = XRSequencer::getCurrentSelectedTrackNum();

        return soundPageNumMap[activePatternSounds[currTrack].type];
    }

    uint8_t getPageCountForTrack(int track)
    {
        return soundPageNumMap[activePatternSounds[track].type];
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

        auto looptype = getValueNormalizedAsUInt8(activePatternSounds[currentSelectedTrackNum].params[MSMP_LOOPTYPE]);
        auto chromatic = getValueNormalizedAsBool(activePatternSounds[currentSelectedTrackNum].params[MSMP_CHROMATIC]);

        uint8_t looptypeToUse = looptype;

        if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStepNum > -1)
        {
            if (activePatternSoundStepModLayer.sounds[currentSelectedTrackNum].steps[currentSelectedStepNum].flags[MSMP_LOOPTYPE])
            {
                looptypeToUse = getValueNormalizedAsUInt8(
                    activePatternSoundStepModLayer.sounds[currentSelectedTrackNum].steps[currentSelectedStepNum].mods[MSMP_LOOPTYPE]
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
        std::string sampleName = "/audio enjoyer/xr-1/samples/";
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
#ifndef NO_DEXED
    void applyCurrentDexedPatchToSound()
    {
        auto track = XRSequencer::getCurrentSelectedTrackNum();

        uint8_t dexedParamData[MAXIMUM_DEXED_SOUND_PARAMS];

        dexedInstances[track].dexed.getVoiceData(dexedParamData);

        for (int dp=0; dp<MAXIMUM_DEXED_SOUND_PARAMS; dp++)
        {
            activePatternSounds[track].dexedParams[dp] = dexedParamData[dp];
        }
    }
#endif

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
        return (int32_t)(value * 100);
    }

    int32_t getBoolValuePaddedAsInt32(bool value)
    {
        return (int32_t)(value * 100);
    }
}
