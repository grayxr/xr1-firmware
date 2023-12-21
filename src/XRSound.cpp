#include <XRSound.h>
#include <XRClock.h>
#include <XRSequencer.h>
#include <XRUX.h>
#include <XRMIDI.h>
#include <XRCV.h>
#include <XRSD.h>
#include <XRKeyMatrix.h>
#include <flashloader.h>
#include <map>

namespace XRSound
{
    // private variables

    // 8MB max of samples per pattern in external PSRAM, 1 sample allowed per track for now    
    newdigate::audiosample *_extPatternSamples[MAXIMUM_SEQUENCER_TRACKS];
    newdigate::flashloader _loader;
    uint8_t _numChannels = 1;

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
        100,0,300000,100,0,       // sampleplayrate, looptype, loopstart, loopfinish, chromatic
        0,0,0,0,0,              // playstart, n/a, n/a, n/a, n/a
        0,100000,100,500000,0,  // a. attack, a. decay, a. sustain, a. release, n/a
        70,0,0,0,0,             // level, pan, n/a, n/a, n/a
        0,0,0,0,0               // n/a, n/a, n/a, n/a, n/a
    };

    int32_t monoSynthInitParams[MAXIMUM_SOUND_PARAMS] = {
        100,-700,0,100,50,          // waveform, detune, fine, osc-a level, osc-b level
        50,0,160000,0,0,            // width, noise, cutoff, resonance, n/a
        0,100000,100,500000,100,    // f. attack, f. decay, f. sustain, f. release, f. env amount
        0,100000,100,500000,0,      // a. attack, a. decay, a. sustain, a. release, n/a
        70,0,0,0,0,                 // level, pan, n/a, n/a, n/a
        0,0,0,0,0                   // n/a, n/a, n/a, n/a, n/a
    };

    int32_t dexedSynthInitParams[MAXIMUM_SOUND_PARAMS] = {
        1,0,0,0,0, // algorithm, n/a, n/a, n/a, n/a
        0,0,0,0,0, // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0, // n/a, n/a, n/a, n/a, n/a
        70,0,0,0,0, // level, pan, n/a, n/a, n/a
        0,0,0,0,0  // n/a, n/a, n/a, n/a, n/a
    };

    int32_t braidsSynthInitParams[MAXIMUM_SOUND_PARAMS] = {
        0,0,0,0,0, // timbre, color, modulation, fm, n/a
        0,0,0,0,0, // coarse, fine, n/a, n/a, n/a
        0,0,0,0,0, // n/a, n/a, n/a, n/a, n/a
        70,0,0,0,0, // level, pan, n/a, n/a, n/a
        0,0,0,0,0  // n/a, n/a, n/a, n/a, n/a
    };

    int32_t fmDrumInitParams[MAXIMUM_SOUND_PARAMS] = {
        5000,0,75,0,0, // frequency, fm, decay, noise, overdrive
        0,0,0,0,0, // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0, // n/a, n/a, n/a, n/a, n/a
        70,0,0,0,0, // level, pan, n/a, n/a, n/a
        0,0,0,0,0  // n/a, n/a, n/a, n/a, n/a
    };

    int32_t midiInitParams[MAXIMUM_SOUND_PARAMS] = {
        1,0,0,0,0, // channel, n/a, n/a, n/a, n/a
        0,0,0,0,0, // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0, // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0, // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0  // n/a, n/a, n/a, n/a, n/a
    };

    int32_t cvGateInitParams[MAXIMUM_SOUND_PARAMS] = {
        100,0,0,0,0, // port, n/a, n/a, n/a, n/a
        0,0,0,0,0, // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0, // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0, // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0  // n/a, n/a, n/a, n/a, n/a
    };

    int32_t cvTrigInitParams[MAXIMUM_SOUND_PARAMS] = {
        100,0,0,0,0, // port, n/a, n/a, n/a, n/a
        0,0,0,0,0, // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0, // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0, // n/a, n/a, n/a, n/a, n/a
        0,0,0,0,0  // n/a, n/a, n/a, n/a, n/a
    };
  
    // extern globals
    
    SOUND currentPatternSounds[MAXIMUM_SEQUENCER_TRACKS];
    DMAMEM SOUND nextPatternSounds[MAXIMUM_SEQUENCER_TRACKS];
    DMAMEM PATTERN_SOUND_MODS patternSoundStepMods;

    bool soundNeedsReinit[MAXIMUM_SEQUENCER_TRACKS] = {
        false, false, false, false,
        false, false, false, false,
        false, false, false, false,
        false, false, false, false,
    };

    ComboVoice comboVoices[COMBO_VOICE_COUNT] = {
        ComboVoice(
            fmdrum1, dexed1, vmsample1, vosca1, voscb1, vnoise1, voscmix1, vdc1, vlfilter1, vfilterenv1, vmix1, venv1, vleft1, vright1, dleft1, dright1, fdleft1, fdright1, vsubmixl1, vsubmixr1),
        ComboVoice(
            fmdrum2, dexed2, vmsample2, vosca2, voscb2, vnoise2, voscmix2, vdc2, vlfilter2, vfilterenv2, vmix2, venv2, vleft2, vright2, dleft2, dright2, fdleft2, fdright2, vsubmixl2, vsubmixr2),
        ComboVoice(
            fmdrum3, dexed3, vmsample3, vosca3, voscb3, vnoise3, voscmix3, vdc3, vlfilter3, vfilterenv3, vmix3, venv3, vleft3, vright3, dleft3, dright3, fdleft3, fdright3, vsubmixl3, vsubmixr3),
        ComboVoice(
            fmdrum4, dexed4, vmsample4, vosca4, voscb4, vnoise4, voscmix4, vdc4, vlfilter4, vfilterenv4, vmix4, venv4, vleft4, vright4, dleft4, dright4, fdleft4, fdright4, vsubmixl4, vsubmixr4),
    };

    SampleVoice sampleVoices[SAMPLE_VOICE_COUNT] = {
        SampleVoice(
            vmsample5, venv5, vleft5, vright5, vsubmixl5, vsubmixr5),
        SampleVoice(
            vmsample6, venv6, vleft6, vright6, vsubmixl6, vsubmixr6),
        SampleVoice(
            vmsample7, venv7, vleft7, vright7, vsubmixl7, vsubmixr7),
        SampleVoice(
            vmsample8, venv8, vleft8, vright8, vsubmixl8, vsubmixr8),
        SampleVoice(
            vmsample9, venv9, vleft9, vright9, vsubmixl9, vsubmixr9),
        SampleVoice(
            vmsample10, venv10, vleft10, vright10, vsubmixl10, vsubmixr10),
        SampleVoice(
            vmsample11, venv11, vleft11, vright11, vsubmixl11, vsubmixr11),
        SampleVoice(
            vmsample12, venv12, vleft12, vright12, vsubmixl12, vsubmixr12),
        SampleVoice(
            vmsample13, venv13, vleft13, vright13, vsubmixl13, vsubmixr13),
        SampleVoice(
            vmsample14, venv14, vleft14, vright14, vsubmixl14, vsubmixr14),
        SampleVoice(
            vmsample15, venv15, vleft15, vright15, vsubmixl15, vsubmixr15),
        SampleVoice(
            vmsample16, venv16, vleft16, vright16, vsubmixl16, vsubmixr16),
    };

    std::map<int, loop_type> loopTypeSelMap = {
        {0, looptype_none},
        {1, looptype_repeat},
        {2, looptype_repeat}, // used for chromatic repeat
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
        { T_FM_DRUM, fmDrumInitParams },
        { T_MIDI, midiInitParams },
        { T_CV_GATE, cvGateInitParams },
        { T_CV_TRIG, cvTrigInitParams },
    };

    std::map<SOUND_TYPE, int> soundPageNumMap = {
        {T_MONO_SAMPLE, 5},
        {T_MONO_SYNTH, 6},
        {T_DEXED_SYNTH, 4},
        {T_FM_DRUM, 2},
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
        {T_DEXED_SYNTH, {
                         {0, "MAIN"},
                         {1, "FM1"},
                         {2, "FM2"},
                         {3, "OUTPUT"},
                     }},
        {T_FM_DRUM, {
                         {0, "MAIN"},
                         {1, "OUTPUT"},
                     }},
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

    void init()
    {
        // initialize CV level array
        for (int i = 0; i < 128; i++)
        {
            _cvLevels[i] = i * 26;
        }

        initNextPatternSounds();
        initPatternSoundStepMods();
        initVoices();
    }

    // Since nextPatternSounds lives in DMAMEM, we need to initialize its contents
    void initNextPatternSounds()
    {
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            nextPatternSounds[t].type = T_EMPTY;

            for (int p = 0; p < MAXIMUM_SOUND_PARAMS; p++) {
                nextPatternSounds[t].params[p] = 0;
            }

            for (int dp = 0; dp < MAXIMUM_DEXED_SOUND_PARAMS; dp++) {
                nextPatternSounds[t].dexedParams[dp] = 0;
            }

            strcpy(nextPatternSounds[t].name, "NO SOUND");
            strcpy(nextPatternSounds[t].sampleName, "");
        }
    }

    // Since initPatternSoundStepMods lives in DMAMEM, we need to initialize its contents
    void initPatternSoundStepMods()
    {
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
            {
                for (int p = 0; p < MAXIMUM_SOUND_PARAMS; p++) {
                    patternSoundStepMods.sounds[t].steps[s].mods[p] = 0;
                    patternSoundStepMods.sounds[t].steps[s].flags[p] = false;
                }
            }
        }
    }

    void initVoices()
    {
        // init ComboVoice audio objects
        for (int v = 0; v < COMBO_VOICE_COUNT; v++)
        {
            initComboVoiceForTrack(v);
        }

        auto msmpSamplePlayRate = getValueNormalizedAsFloat(monoSampleInitParams[MSMP_SAMPLEPLAYRATE]);
        auto msmpAatt = getValueNormalizedAsFloat(monoSampleInitParams[MSMP_AMP_ATTACK]);
        auto msmpAdec = getValueNormalizedAsFloat(monoSampleInitParams[MSMP_AMP_DECAY]);
        auto msmpAsus = getValueNormalizedAsFloat(monoSampleInitParams[MSMP_AMP_SUSTAIN]);
        auto msmpArel = getValueNormalizedAsFloat(monoSampleInitParams[MSMP_AMP_RELEASE]);
        auto msmpLvl = getValueNormalizedAsFloat(monoSampleInitParams[MSMP_LEVEL]);
        auto msmpPan = getValueNormalizedAsFloat(monoSampleInitParams[MSMP_PAN]);

        AudioNoInterrupts();

        // init SampleVoice audio objects
        for (int v = 0; v < SAMPLE_VOICE_COUNT; v++)
        {
            // init MONO_SAMPLE
            sampleVoices[v].sample.setPlaybackRate(msmpSamplePlayRate);
            sampleVoices[v].sample.enableInterpolation(true);

            sampleVoices[v].ampEnv.attack(msmpAatt * 0.5); // 0.5 = default non-accented velocity
            sampleVoices[v].ampEnv.decay(msmpAdec * 0.5);
            sampleVoices[v].ampEnv.sustain(msmpAsus * 0.5);
            sampleVoices[v].ampEnv.release(msmpArel * 0.5);

            // init MONO_SAMPLE mono to L&R splitter
            PANNED_AMOUNTS monoSamplePannedAmounts = getStereoPanValues(msmpPan);
            sampleVoices[v].leftCtrl.gain(monoSamplePannedAmounts.right * 0.5); // 0.5 = default non-accented velocity
            sampleVoices[v].rightCtrl.gain(monoSamplePannedAmounts.left * 0.5);

            // Sub L&R mixers
            sampleVoices[v].leftSubMix.gain(0, msmpLvl);  // MONO_SAMPLE left
            sampleVoices[v].rightSubMix.gain(0, msmpLvl); // MONO_SAMPLE right
        }

        mixerLeft1.gain(0, 1);
        mixerRight1.gain(0, 1);
        mixerLeft1.gain(1, 1);
        mixerRight1.gain(1, 1);
        mixerLeft1.gain(2, 1);
        mixerRight1.gain(2, 1);
        mixerLeft1.gain(3, 1);
        mixerRight1.gain(3, 1);

        mixerLeft2.gain(0, 1);
        mixerRight2.gain(0, 1);
        mixerLeft2.gain(1, 1);
        mixerRight2.gain(1, 1);
        mixerLeft2.gain(2, 1);
        mixerRight2.gain(2, 1);
        mixerLeft2.gain(3, 1);
        mixerRight2.gain(3, 1);

        mixerLeft3.gain(0, 1);
        mixerRight3.gain(0, 1);
        mixerLeft3.gain(1, 1);
        mixerRight3.gain(1, 1);
        mixerLeft3.gain(2, 1);
        mixerRight3.gain(2, 1);
        mixerLeft3.gain(3, 1);
        mixerRight3.gain(3, 1);

        mixerLeft4.gain(0, 1);
        mixerRight4.gain(0, 1);
        mixerLeft4.gain(1, 1);
        mixerRight4.gain(1, 1);
        mixerLeft4.gain(2, 1);
        mixerRight4.gain(2, 1);
        mixerLeft4.gain(3, 1);
        mixerRight4.gain(3, 1);

        // Main L&R output mixer
        mainMixerLeft.gain(0, 1);
        mainMixerRight.gain(0, 1);
        mainMixerLeft.gain(1, 1);
        mainMixerRight.gain(1, 1);
        mainMixerLeft.gain(2, 1);
        mainMixerRight.gain(2, 1);
        mainMixerLeft.gain(3, 1);
        mainMixerRight.gain(3, 1);

        // L&R input mixer
        inputMixerLeft.gain(0, 0.25);
        inputMixerRight.gain(0, 0.25);

        // Main L&R output mixer
        OutputMixerLeft.gain(0, 1);
        OutputMixerRight.gain(0, 1);
        OutputMixerLeft.gain(1, 1);
        OutputMixerRight.gain(1, 1);

        AudioInterrupts();
    }

    void initComboVoiceForTrack(int t)
    {
        if (t > 3) {
            return;
        }

        auto msmpSamplePlayRate = getValueNormalizedAsFloat(monoSampleInitParams[MSMP_SAMPLEPLAYRATE]);
        auto msmpLvl = getValueNormalizedAsFloat(monoSampleInitParams[MSMP_LEVEL]);
        auto msmpPan = getValueNormalizedAsFloat(monoSampleInitParams[MSMP_PAN]);

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

        auto dexeLvl = getValueNormalizedAsFloat(dexedSynthInitParams[DEXE_LEVEL]);
        auto dexePan = getValueNormalizedAsFloat(dexedSynthInitParams[DEXE_PAN]);

        auto fmdLvl = getValueNormalizedAsFloat(fmDrumInitParams[FMD_LEVEL]);
        auto fmdPan = getValueNormalizedAsFloat(fmDrumInitParams[FMD_PAN]);

        AudioNoInterrupts();

        // init MONO_SAMPLE
        comboVoices[t].sample.setPlaybackRate(msmpSamplePlayRate);
        comboVoices[t].sample.enableInterpolation(true);

        // init MONO_SYNTH
        comboVoices[t].osca.begin(msynWave);
        comboVoices[t].osca.frequency(NOTE_FREQ_C4);
        comboVoices[t].osca.amplitude(msynAmpLvlA);
        comboVoices[t].osca.pulseWidth(msynWidth);

        comboVoices[t].oscb.begin(msynWave);
        comboVoices[t].oscb.frequency(NOTE_FREQ_C4);
        comboVoices[t].oscb.amplitude(msynAmpLvlB);
        comboVoices[t].oscb.pulseWidth(msynWidth);

        comboVoices[t].noise.amplitude(msynNoise);

        comboVoices[t].oscMix.gain(0, 0.33); // osc A
        comboVoices[t].oscMix.gain(1, 0.33); // osc B
        comboVoices[t].oscMix.gain(2, 0.33); // noise

        comboVoices[t].dc.amplitude(msynFenv);
        comboVoices[t].lfilter.frequency(msynCutoff);
        comboVoices[t].lfilter.resonance(msynRes);
        comboVoices[t].lfilter.octaveControl(7); // TODO: use 4 ?

        comboVoices[t].filterEnv.attack(msynFatt);
        comboVoices[t].filterEnv.decay(msynFdec);
        comboVoices[t].filterEnv.sustain(msynFsus);
        comboVoices[t].filterEnv.release(msynFrel);
        
        comboVoices[t].ampEnv.attack(msynAatt * 0.5); // 0.5 = default non-accented velocity
        comboVoices[t].ampEnv.decay(msynAdec * 0.5);
        comboVoices[t].ampEnv.sustain(msynAsus * 0.5);
        comboVoices[t].ampEnv.release(msynArel * 0.5);

        // init DEXED object
        comboVoices[t].dexed.loadInitVoice();

        // init MONO_SAMPLE & MONO_SYNTH submix
        comboVoices[t].mix.gain(0, 1); // MONO_SAMPLE ON
        comboVoices[t].mix.gain(1, 0); // MONO_SYNTH  OFF

        // init MONO_SAMPLE & MONO_SYNTH mono to L&R splitter
        PANNED_AMOUNTS monoSamplePannedAmounts = getStereoPanValues(msmpPan);
        comboVoices[t].leftCtrl.gain(monoSamplePannedAmounts.right * 0.5); // 0.5 = default non-accented velocity
        comboVoices[t].rightCtrl.gain(monoSamplePannedAmounts.left * 0.5);

        // init DEXED mono to L&R splitter
        PANNED_AMOUNTS dexedSynthPannedAmounts = getStereoPanValues(dexePan);
        comboVoices[t].dexedLeftCtrl.gain(dexedSynthPannedAmounts.right * 0.5);
        comboVoices[t].dexedRightCtrl.gain(dexedSynthPannedAmounts.left * 0.5);

        // init FM_DRUM mono to L&R splitter
        PANNED_AMOUNTS fmDrumPannedAmounts = getStereoPanValues(fmdPan);
        comboVoices[t].fmdrum.init();
        comboVoices[t].fmDrumLeftCtrl.gain(fmDrumPannedAmounts.right * 0.5);
        comboVoices[t].fmDrumRightCtrl.gain(fmDrumPannedAmounts.left * 0.5);

        // Sub L&R mixers
        comboVoices[t].leftSubMix.gain(0, msmpLvl);  // MONO_SAMPLE / MONO_SYNTH left
        comboVoices[t].leftSubMix.gain(1, dexeLvl);  // DEXED left
        comboVoices[t].leftSubMix.gain(2, fmdLvl);  // FM_DRUM left
        comboVoices[t].rightSubMix.gain(0, msmpLvl); // MONO_SAMPLE / MONO_SYNTH right
        comboVoices[t].rightSubMix.gain(1, dexeLvl); // DEXED right
        comboVoices[t].rightSubMix.gain(2, fmdLvl); // FM_DRUM right

        AudioInterrupts();
    }

    void initTrackSound(int8_t track)
    {
        // init sound and sample names
        strcpy(currentPatternSounds[track].name, "NO SOUND");
        strcpy(currentPatternSounds[track].sampleName, "");

        // init generic sound params
        auto soundType = currentPatternSounds[track].type;
        auto initParams = soundTypeInitParams[soundType];
        for (int p=0; p<MAXIMUM_SOUND_PARAMS; p++)
        {
            currentPatternSounds[track].params[p] = initParams[p];
        }

        // init dexed params
        uint8_t dexedParamData[MAXIMUM_DEXED_SOUND_PARAMS];
        comboVoices[track].dexed.loadInitVoice();
        comboVoices[track].dexed.getVoiceData(dexedParamData);
        for (int dp=0; dp<MAXIMUM_DEXED_SOUND_PARAMS; dp++)
        {
            currentPatternSounds[track].dexedParams[dp] = dexedParamData[dp];
        }
    }

    void setSoundNeedsReinit(int sound, bool reinit)
    {
        soundNeedsReinit[sound] = reinit;
    }

    void reinitSoundForTrack(int track)
    {
        currentPatternSounds[track] = nextPatternSounds[track];

        // load any samples for track
        std::string newSoundSampleName(currentPatternSounds[track].sampleName);
        if (newSoundSampleName.length() > 0) {
            // load any samples into PSRAM
            std::string sampleNameStr = "/audio enjoyer/xr-1/samples/";
            sampleNameStr += newSoundSampleName;
            _extPatternSamples[track] = _loader.loadSample(sampleNameStr.c_str());
        }

        // adjust voice settings for this track sound if using a combo voice
        if (track < 4) {
            // load any dexed voice settings for track
            comboVoices[track].dexed.loadVoiceParameters(currentPatternSounds[track].dexedParams);

            setComboVoiceMixSettingsForTrack(track);
        }

        // all done reinitializing sound
        soundNeedsReinit[track] = false;
    }

    void manageSoundDataForPatternChange(int nextBank, int nextPattern)
    {
        // TODO: check if sounds are "dirty" and require SD card save,
        // otherwise skip SD card writes to save time / perf hit / etc
        
        XRSD::savePatternSounds();
        XRSD::savePatternSoundStepModsToSdCard();

        if (!XRSD::loadPatternSounds(nextBank, nextPattern))
        {
            initNextPatternSounds();
        }

        if (!XRSD::loadPatternSoundStepModsFromSdCard(nextBank, nextPattern)) {
            initPatternSoundStepMods();
        }

        auto &seqState = XRSequencer::getSeqState();

        if (seqState.playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::RUNNING) {
            auto &tracks = XRSequencer::sequencer.banks[nextBank].patterns[nextPattern].tracks;

            for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
            {
                if (
                    tracks[t].initialized &&
                     currentPatternSounds[t].type != T_DEXED_SYNTH // don't load dexed changes async since it cuts out the sound
                ) {
                    setSoundNeedsReinit(t, true); // reinit sound asynchronously since the upcoming track is active
                } else {
                    reinitSoundForTrack(t); // reinit sound synchronously since the upcoming track is either empty or a dexed sound
                }
            }
        } else {
            for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
            {
                reinitSoundForTrack(t); // reinit all sounds synchronously since the sequencer isn't running
            }
        }
    }

    ComboVoice &getComboVoiceForCurrentTrack()
    {
        return comboVoices[XRSequencer::getCurrentSelectedTrackNum()];
    }

    ComboVoice &getComboVoiceForTrack(int t)
    {
        return comboVoices[t];
    }

    SampleVoice &getSampleVoiceForTrack(int t)
    {
        return sampleVoices[t];
    }
    
    SOUND_CONTROL_MODS getControlModDataForPattern()
    {
        SOUND_CONTROL_MODS mods;

        auto &pattern = XRSequencer::getHeapCurrentSelectedPattern();

        std::string grooveForPattern = pattern.groove.id > -1 ? XRClock::getGrooveString(pattern.groove.id) : "";
        std::string grooveAmountForPattern = XRClock::getGrooveAmountString(pattern.groove.amount);

        mods.aName = "L.STEP";
        mods.bName = "GROOVE";
        mods.cName = "GR.AMT";
        mods.dName = "ACCENT";

        mods.aValue = std::to_string(pattern.lstep);
        mods.bValue = pattern.groove.id > -1 ? grooveForPattern : "OFF";
        mods.cValue = pattern.groove.id > -1 ? grooveAmountForPattern : "--";
        mods.dValue = std::to_string(pattern.accent);

        return mods;
    }

    SOUND_CONTROL_MODS getControlModDataForTrack()
    {
        SOUND_CONTROL_MODS mods;

        auto trackNum = XRSequencer::getCurrentSelectedTrackNum();

        auto soundType = currentPatternSounds[trackNum].type;

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

    SOUND_CONTROL_MODS getMonoSampleControlModData()
    {
        SOUND_CONTROL_MODS mods;

        auto &currentSelectedTrack = XRSequencer::getHeapCurrentSelectedTrack();
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
                    currentPatternSounds[currentSelectedTrackNum].params[MSMP_SAMPLEPLAYRATE]
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
                    currentPatternSounds[currentSelectedTrackNum].params[MSMP_LOOPSTART]
                );

                // if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStepNum > -1)
                // {
                //     loopstartToUse = modsForCurrentTrackStep.loopstart;
                // }

                std::string lsStr = std::to_string(loopstartToUse);
                lsStr += "ms";

                mods.bValue = lsStr;

                auto loopfinishToUse = getValueNormalizedAsUInt32(
                    currentPatternSounds[currentSelectedTrackNum].params[MSMP_LOOPFINISH]
                );

                // if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStepNum > -1)
                // {
                //     loopfinishToUse = modsForCurrentTrackStep.loopfinish;
                // }

                std::string lfStr = std::to_string(loopfinishToUse);
                lfStr += "ms";

                mods.cValue = lfStr;

                auto playstartToUse = (play_start)getValueNormalizedAsInt8(
                    currentPatternSounds[currentSelectedTrackNum].params[MSMP_LOOPSTART]
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

                auto aatt = getValueNormalizedAsFloat(currentPatternSounds[currentSelectedTrackNum].params[MSMP_AMP_ATTACK]);
                auto adec = getValueNormalizedAsFloat(currentPatternSounds[currentSelectedTrackNum].params[MSMP_AMP_DECAY]);
                auto asus = getValueNormalizedAsFloat(currentPatternSounds[currentSelectedTrackNum].params[MSMP_AMP_SUSTAIN]);
                auto arel = getValueNormalizedAsFloat(currentPatternSounds[currentSelectedTrackNum].params[MSMP_AMP_RELEASE]);

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

                auto lvl = getValueNormalizedAsFloat(currentPatternSounds[currentSelectedTrackNum].params[MSMP_LEVEL]);
                auto pan = getValueNormalizedAsFloat(currentPatternSounds[currentSelectedTrackNum].params[MSMP_PAN]);

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

        auto &currentSelectedTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto currentSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        auto currentSoundForTrack = currentPatternSounds[currentSelectedTrackNum];

            // TODO: add step mod support back in

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            {
                mods.aName = "LSTP";
                mods.bName = "LEN";
                mods.cName = "VELO";
                mods.dName = "PROB";

                mods.aValue = std::to_string(currentSelectedTrack.lstep);
                mods.bValue = std::to_string(currentSelectedTrack.length);
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
                mods.cName = "--";
                mods.dName = "--"; // fx send?

                auto lvl = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_LEVEL]);
                auto pan = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_PAN]);

                mods.aValue = std::to_string(round(lvl * 100));
                mods.bValue = std::to_string((float)round(pan * 100) / 100);
                mods.bValue = mods.bValue.substr(0, 3);
                mods.cValue = "--";
                mods.dValue = "--";

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

        auto &currentSelectedTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto currentSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            {
                mods.aName = "LSTP";
                mods.bName = "LEN";
                mods.cName = "VELO";
                mods.dName = "PROB";

                mods.aValue = std::to_string(currentSelectedTrack.lstep);
                mods.bValue = std::to_string(currentSelectedTrack.length);
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

                auto lvl = getValueNormalizedAsFloat(currentPatternSounds[currentSelectedTrackNum].params[DEXE_LEVEL]);
                auto pan = getValueNormalizedAsFloat(currentPatternSounds[currentSelectedTrackNum].params[DEXE_PAN]);

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

    SOUND_CONTROL_MODS getFmDrumControlModData()
    {
        SOUND_CONTROL_MODS mods;

        auto currentSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        auto currentSoundForTrack = currentPatternSounds[currentSelectedTrackNum];

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

        case 1: // OUTPUT
            {
                mods.aName = "LEVEL";
                mods.bName = "PAN";
                mods.cName = "--";
                mods.dName = "--"; // fx send?

                auto lvl = getValueNormalizedAsFloat(currentSoundForTrack.params[FMD_LEVEL]);
                auto pan = getValueNormalizedAsFloat(currentSoundForTrack.params[FMD_PAN]);

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

        auto &currentSelectedTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto currentSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto currentSoundForTrack = currentPatternSounds[currentSelectedTrackNum];

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

        auto &currentSelectedTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto currentSelectedTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();

        auto currentSoundForTrack = currentPatternSounds[currentSelectedTrackNum];

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

        auto &currentSelectedTrack = XRSequencer::getHeapCurrentSelectedTrack();
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
        auto &trackToUse = XRSequencer::getHeapTrack(track);

        auto msmpLooptype = getValueNormalizedAsUInt8(currentPatternSounds[track].params[MSMP_LOOPTYPE]);
        auto msmpLoopstart = getValueNormalizedAsInt32(currentPatternSounds[track].params[MSMP_LOOPSTART]);
        auto msmpLoopfinish = getValueNormalizedAsInt32(currentPatternSounds[track].params[MSMP_LOOPFINISH]);
        auto msmpPlaystart = (play_start)getValueNormalizedAsUInt8(currentPatternSounds[track].params[MSMP_PLAYSTART]);
        auto msmpAatt = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSMP_AMP_ATTACK]);
        auto msmpAdec = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSMP_AMP_DECAY]);
        auto msmpAsus = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSMP_AMP_SUSTAIN]);
        auto msmpArel = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSMP_AMP_RELEASE]);
        auto msmpPan = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSMP_PAN]);

        if (track > 3) // sample-only voices
        {
            int tOffset = track - 4;

            AudioNoInterrupts();

            sampleVoices[tOffset].leftCtrl.gain(getStereoPanValues(msmpPan).right * (trackToUse.velocity * 0.01));
            sampleVoices[tOffset].rightCtrl.gain(getStereoPanValues(msmpPan).left * (trackToUse.velocity * 0.01));

            sampleVoices[tOffset].ampEnv.attack(msmpAatt * (trackToUse.velocity * 0.01));
            sampleVoices[tOffset].ampEnv.decay(msmpAdec * (trackToUse.velocity * 0.01));
            sampleVoices[tOffset].ampEnv.sustain(msmpAsus * (trackToUse.velocity * 0.01));
            sampleVoices[tOffset].ampEnv.release(msmpArel * (trackToUse.velocity * 0.01));

            AudioInterrupts();

            sampleVoices[tOffset].ampEnv.noteOn();

            std::string trackSampleName(currentPatternSounds[track].sampleName);

            // if sample has valid name, assume it is loaded in PSRAM and can be played
            if (trackSampleName.length() > 0) {
                sampleVoices[tOffset].sample.playRaw(
                    _extPatternSamples[track]->sampledata, 
                    _extPatternSamples[track]->samplesize / 2, 
                    _numChannels
                );
            }

            // always re-initialize loop type
            sampleVoices[tOffset].sample.setLoopType(loopTypeSelMap[msmpLooptype]);

            if (loopTypeSelMap[msmpLooptype] == looptype_none)
            {
                sampleVoices[tOffset].sample.setPlayStart(play_start::play_start_sample);
                sampleVoices[tOffset].sample.setLoopType(loop_type::looptype_none);
            }
            else if (loopTypeSelMap[msmpLooptype] == looptype_repeat)
            {
                float loopFinishToUse = msmpLoopfinish;

                // if (trackToUse.chromatic_enabled) {
                //   float foundBaseFreq = noteToFreqArr[trackToUse.note];
                //   float octaveFreq = foundBaseFreq * (pow(2, keyboardOctave));
                //   //float freq = 440.0 * powf(2.0, (12-69) / 12.0);
                //   uint32_t numSamples = 44100 / octaveFreq;
                //   loopFinishToUse = numSamples;
                // }

                sampleVoices[tOffset].sample.setPlayStart(msmpPlaystart == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
                sampleVoices[tOffset].sample.setLoopStart(msmpLoopstart);
                sampleVoices[tOffset].sample.setLoopFinish(loopFinishToUse);
            }
        }
        else // combo voices
        {
            AudioNoInterrupts();

            comboVoices[track].leftCtrl.gain(getStereoPanValues(msmpPan).right * (trackToUse.velocity * 0.01));
            comboVoices[track].rightCtrl.gain(getStereoPanValues(msmpPan).left * (trackToUse.velocity * 0.01));

            comboVoices[track].ampEnv.attack(msmpAatt * (trackToUse.velocity * 0.01));
            comboVoices[track].ampEnv.decay(msmpAdec * (trackToUse.velocity * 0.01));
            comboVoices[track].ampEnv.sustain(msmpAsus * (trackToUse.velocity * 0.01));
            comboVoices[track].ampEnv.release(msmpArel * (trackToUse.velocity * 0.01));

            AudioInterrupts();

            comboVoices[track].ampEnv.noteOn();

            std::string trackSampleName(currentPatternSounds[track].sampleName);

            // if sample has valid name, assume it is loaded in PSRAM and can be played
            if (trackSampleName.length() > 0) {
                comboVoices[track].sample.playRaw(
                    _extPatternSamples[track]->sampledata, 
                    _extPatternSamples[track]->samplesize / 2, 
                    _numChannels
                );
            }

            // always re-initialize loop type
            comboVoices[track].sample.setLoopType(loopTypeSelMap[msmpLooptype]);

            if (loopTypeSelMap[msmpLooptype] == looptype_none)
            {
                comboVoices[track].sample.setPlayStart(play_start::play_start_sample);
                comboVoices[track].sample.setLoopType(loop_type::looptype_none);
            }
            else if (loopTypeSelMap[msmpLooptype] == looptype_repeat)
            {
                float loopFinishToUse = msmpLoopfinish;

                // if (trackToUse.chromatic_enabled) {
                //   float foundBaseFreq = noteToFreqArr[trackToUse.note];
                //   float octaveFreq = foundBaseFreq * (pow(2, keyboardOctave));
                //   //float freq = 440.0 * powf(2.0, (12-69) / 12.0);
                //   uint32_t numSamples = 44100 / octaveFreq;
                //   loopFinishToUse = numSamples;
                // }

                comboVoices[track].sample.setPlayStart(msmpPlaystart == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
                comboVoices[track].sample.setLoopStart(msmpLoopstart);
                comboVoices[track].sample.setLoopFinish(loopFinishToUse);
            }
        }
    }

    void handleMonoSynthNoteOnForTrack(int track)
    {
        auto &trackToUse = XRSequencer::getHeapTrack(track);
       
        auto msynPan = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSYN_PAN]);
        auto msynFine = getValueNormalizedAsInt8(currentPatternSounds[track].params[MSYN_FINE]);
        auto msynDetune = getValueNormalizedAsInt8(currentPatternSounds[track].params[MSYN_DETUNE]);

        auto msynFatt = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSYN_FILTER_ATTACK]);
        auto msynFdec = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSYN_FILTER_DECAY]);
        auto msynFsus = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSYN_FILTER_SUSTAIN]);

        auto msynAatt = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSYN_AMP_ATTACK]);
        auto msynAdec = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSYN_AMP_DECAY]);
        auto msynAsus = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSYN_AMP_SUSTAIN]);
        auto msynArel = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSYN_AMP_RELEASE]);

        float foundBaseFreq = _noteToFreqArr[trackToUse.note];
        float octaveFreqA = (foundBaseFreq + (msynFine * 0.01)) * (pow(2, trackToUse.octave));
        float octaveFreqB = (foundBaseFreq * pow(2.0, (float)msynDetune / 12.0)) * (pow(2, trackToUse.octave));
        
        AudioNoInterrupts();

        comboVoices[track].osca.frequency(octaveFreqA);
        comboVoices[track].oscb.frequency(octaveFreqB);

        comboVoices[track].leftCtrl.gain(getStereoPanValues(msynPan).right * (trackToUse.velocity * 0.01));
        comboVoices[track].rightCtrl.gain(getStereoPanValues(msynPan).left * (trackToUse.velocity * 0.01));

        comboVoices[track].filterEnv.attack(msynFatt * (trackToUse.velocity * 0.01));
        comboVoices[track].filterEnv.decay(msynFdec * (trackToUse.velocity * 0.01));
        comboVoices[track].filterEnv.sustain(msynFsus * (trackToUse.velocity * 0.01));
        comboVoices[track].filterEnv.release(msynArel * (trackToUse.velocity * 0.01));

        comboVoices[track].ampEnv.attack(msynAatt * (trackToUse.velocity * 0.01));
        comboVoices[track].ampEnv.decay(msynAdec * (trackToUse.velocity * 0.01));
        comboVoices[track].ampEnv.sustain(msynAsus * (trackToUse.velocity * 0.01));
        comboVoices[track].ampEnv.release(msynArel * (trackToUse.velocity * 0.01));
        
        AudioInterrupts();

        // now trigger envs
        comboVoices[track].ampEnv.noteOn();
        comboVoices[track].filterEnv.noteOn();
    }

    void handleDexedSynthNoteOnForTrack(int track)
    {
        auto &trackToUse = XRSequencer::getHeapTrack(track);

        uint8_t noteToUse = trackToUse.note;
        uint8_t octaveToUse = trackToUse.octave;

        int midiNote = (noteToUse + (12 * (octaveToUse)));

        comboVoices[track].dexed.keydown(midiNote, 50); // TODO: parameterize velocity
    }
    
    void handleFmDrumNoteOnForTrack(int track)
    {
        comboVoices[track].fmdrum.noteOn();
    }

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
        auto &trackToUse = XRSequencer::getHeapTrack(track);
        auto &stepToUse = XRSequencer::getHeapStep(track, step);

        auto msmpSamplePlayRate = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSMP_SAMPLEPLAYRATE]);
        auto msmpLooptype = getValueNormalizedAsUInt8(currentPatternSounds[track].params[MSMP_LOOPTYPE]);
        auto msmpLoopstart = getValueNormalizedAsInt32(currentPatternSounds[track].params[MSMP_LOOPSTART]);
        auto msmpLoopfinish = getValueNormalizedAsInt32(currentPatternSounds[track].params[MSMP_LOOPFINISH]);
        auto msmpPlaystart = (play_start)getValueNormalizedAsUInt8(currentPatternSounds[track].params[MSMP_PLAYSTART]);
        auto msmpAatt = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSMP_AMP_ATTACK]);
        auto msmpAdec = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSMP_AMP_DECAY]);
        auto msmpAsus = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSMP_AMP_SUSTAIN]);
        auto msmpArel = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSMP_AMP_RELEASE]);
        auto msmpPan = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSMP_PAN]);

        std::string trackSampleName(currentPatternSounds[track].sampleName);

        // TODO: allow sample chromatic note playback

        // uint8_t noteToUse = stepToUse.note;
        // if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::NOTE])
        // {
        //     noteToUse = patternMods.tracks[track].steps[step].note;
        // }
        // uint8_t octaveToUse = stepToUse.octave;
        // if (patternMods.tracks[track].step_mod_flags[step].flags[4]) {
        //   octaveToUse = patternMods.tracks[track].steps[step].octave;
        // }

        auto velocityToUse = trackToUse.velocity;
        if (stepToUse.state == XRSequencer::STATE_ACCENTED) {
            velocityToUse = max(trackToUse.velocity, XRSequencer::heapPattern.accent);
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
        // if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::LOOPTYPE])
        // {
        //     looptypeToUse = patternMods.tracks[track].steps[step].looptype;
        // }

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

        if (track > 3)
        {
            int tOffset = track - 4;
            
            AudioNoInterrupts();
            
            sampleVoices[tOffset].leftCtrl.gain(getStereoPanValues(msmpPan).right * (velocityToUse * 0.01));
            sampleVoices[tOffset].rightCtrl.gain(getStereoPanValues(msmpPan).left * (velocityToUse * 0.01));

            sampleVoices[tOffset].ampEnv.attack(msmpAatt * (velocityToUse * 0.01));
            sampleVoices[tOffset].ampEnv.decay(msmpAdec * (velocityToUse * 0.01));
            sampleVoices[tOffset].ampEnv.sustain(msmpAsus * (velocityToUse * 0.01));
            sampleVoices[tOffset].ampEnv.release(msmpArel * (velocityToUse * 0.01));

            sampleVoices[tOffset].sample.setPlaybackRate(speedToUse);
            
            AudioInterrupts();

            sampleVoices[tOffset].ampEnv.noteOn();

            // if sample has valid name, assume it is loaded in PSRAM and can be played
            if (trackSampleName.length() > 0) {
                sampleVoices[tOffset].sample.playRaw(
                    _extPatternSamples[track]->sampledata, 
                    _extPatternSamples[track]->samplesize / 2, 
                    _numChannels
                );
            }

            // always re-initialize loop type
            sampleVoices[tOffset].sample.setLoopType(loopTypeSelMap[looptypeToUse]);

            if (loopTypeSelMap[looptypeToUse] == looptype_none)
            {
                sampleVoices[tOffset].sample.setPlayStart(play_start::play_start_sample);
                sampleVoices[tOffset].sample.setLoopType(loop_type::looptype_none);
            }
            else if (loopTypeSelMap[looptypeToUse] == looptype_repeat)
            {
                // if (trackToUse.chromatic_enabled) {
                //   float foundBaseFreq = noteToFreqArr[noteToUse];
                //   float octaveFreq = foundBaseFreq * (pow(2, keyboardOctave));
                //   //float freq = 440.0 * powf(2.0, (12-69) / 12.0);
                //   uint32_t numSamples = 44100 / octaveFreq;
                //   loopFinishToUse = numSamples;
                // }

                sampleVoices[tOffset].sample.setPlayStart(playstartToUse == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
                sampleVoices[tOffset].sample.setLoopStart(loopstartToUse);
                sampleVoices[tOffset].sample.setLoopFinish(loopfinishToUse);
            }
        }  else {

            AudioNoInterrupts();
            
            comboVoices[track].leftCtrl.gain(getStereoPanValues(msmpPan).right * (velocityToUse * 0.01));
            comboVoices[track].rightCtrl.gain(getStereoPanValues(msmpPan).left * (velocityToUse * 0.01));

            comboVoices[track].ampEnv.attack(msmpAatt * (velocityToUse * 0.01));
            comboVoices[track].ampEnv.decay(msmpAdec * (velocityToUse * 0.01));
            comboVoices[track].ampEnv.sustain(msmpAsus * (velocityToUse * 0.01));
            comboVoices[track].ampEnv.release(msmpArel * (velocityToUse * 0.01));

            comboVoices[track].sample.setPlaybackRate(speedToUse);
            
            AudioInterrupts();

            comboVoices[track].ampEnv.noteOn();

            // if sample has valid name, assume it is loaded in PSRAM and can be played
            if (trackSampleName.length() > 0) {
                comboVoices[track].sample.playRaw(
                    _extPatternSamples[track]->sampledata, 
                    _extPatternSamples[track]->samplesize / 2, 
                    _numChannels
                );
            }

            // always re-initialize loop type
            comboVoices[track].sample.setLoopType(loopTypeSelMap[looptypeToUse]);

            if (loopTypeSelMap[looptypeToUse] == looptype_none)  {
                comboVoices[track].sample.setPlayStart(play_start::play_start_sample);
                comboVoices[track].sample.setLoopType(loop_type::looptype_none);
            } else if (loopTypeSelMap[looptypeToUse] == looptype_repeat) {

                // if (trackToUse.chromatic_enabled) {
                //   float foundBaseFreq = noteToFreqArr[noteToUse];
                //   float octaveFreq = foundBaseFreq * (pow(2, keyboardOctave));
                //   //float freq = 440.0 * powf(2.0, (12-69) / 12.0);
                //   uint32_t numSamples = 44100 / octaveFreq;
                //   loopFinishToUse = numSamples;
                // }

                comboVoices[track].sample.setPlayStart(playstartToUse == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
                comboVoices[track].sample.setLoopStart(loopstartToUse);
                comboVoices[track].sample.setLoopFinish(loopfinishToUse);
            }
        }
    }

    void handleMonoSynthNoteOnForTrackStep(int track, int step)
    {
        auto &trackToUse = XRSequencer::getHeapTrack(track);
        auto &stepToUse = XRSequencer::getHeapStep(track, step);

        auto msynWave = getValueNormalizedAsUInt8(currentPatternSounds[track].params[MSYN_WAVE]);
       
        auto msynPan = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSYN_PAN]);
        auto msynFine = getValueNormalizedAsInt8(currentPatternSounds[track].params[MSYN_FINE]);
        auto msynDetune = getValueNormalizedAsInt8(currentPatternSounds[track].params[MSYN_DETUNE]);

        auto msynFatt = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSYN_FILTER_ATTACK]);
        auto msynFdec = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSYN_FILTER_DECAY]);
        auto msynFsus = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSYN_FILTER_SUSTAIN]);
        auto msynFrel = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSYN_FILTER_RELEASE]);

        auto msynAatt = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSYN_AMP_ATTACK]);
        auto msynAdec = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSYN_AMP_DECAY]);
        auto msynAsus = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSYN_AMP_SUSTAIN]);
        auto msynArel = getValueNormalizedAsFloat(currentPatternSounds[track].params[MSYN_AMP_RELEASE]);

        uint8_t noteToUse = trackToUse.note;
        if (XRSequencer::patternTrackStepMods.tracks[track].steps[step].flags[XRSequencer::NOTE])
        {
            noteToUse = XRSequencer::patternTrackStepMods.tracks[track].steps[step].mods[XRSequencer::NOTE];
        }

        uint8_t octaveToUse = trackToUse.octave;
        if (XRSequencer::patternTrackStepMods.tracks[track].steps[step].flags[XRSequencer::OCTAVE])
        {
            octaveToUse = XRSequencer::patternTrackStepMods.tracks[track].steps[step].mods[XRSequencer::OCTAVE];
        }

        uint8_t velocityToUse = trackToUse.velocity;
        if (stepToUse.state == XRSequencer::STATE_ACCENTED) {
            if (XRSequencer::patternTrackStepMods.tracks[track].steps[step].flags[XRSequencer::VELOCITY]) {
                velocityToUse = XRSequencer::patternTrackStepMods.tracks[track].steps[step].mods[XRSequencer::VELOCITY];
            } else {
                velocityToUse = max(trackToUse.velocity, XRSequencer::heapPattern.accent);
            }
        }

        int waveformToUse = msynWave;
        if (patternSoundStepMods.sounds[track].steps[step].flags[MSYN_WAVE])
        {
            waveformToUse = getWaveformNumber(
                getValueNormalizedAsUInt8(patternSoundStepMods.sounds[track].steps[step].mods[MSYN_WAVE])
            );
        }

        float foundBaseFreq = _noteToFreqArr[noteToUse];
        float octaveFreqA = (foundBaseFreq + (msynFine * 0.01)) * (pow(2, octaveToUse));
        float octaveFreqB = (foundBaseFreq * pow(2.0, (float)msynDetune / 12.0)) * (pow(2, octaveToUse));

        AudioNoInterrupts();

        comboVoices[track].osca.begin(waveformToUse);
        comboVoices[track].oscb.begin(waveformToUse);

        comboVoices[track].osca.frequency(octaveFreqA);
        comboVoices[track].oscb.frequency(octaveFreqB);

        comboVoices[track].leftCtrl.gain(getStereoPanValues(msynPan).right * (velocityToUse * 0.01));
        comboVoices[track].rightCtrl.gain(getStereoPanValues(msynPan).left * (velocityToUse * 0.01));

        comboVoices[track].filterEnv.attack(msynFatt * (velocityToUse * 0.01));
        comboVoices[track].filterEnv.decay(msynFdec * (velocityToUse * 0.01));
        comboVoices[track].filterEnv.sustain(msynFsus * (velocityToUse * 0.01));
        comboVoices[track].filterEnv.release(msynFrel * (velocityToUse * 0.01));

        comboVoices[track].ampEnv.attack(msynAatt * (velocityToUse * 0.01));
        comboVoices[track].ampEnv.decay(msynAdec * (velocityToUse * 0.01));
        comboVoices[track].ampEnv.sustain(msynAsus * (velocityToUse * 0.01));
        comboVoices[track].ampEnv.release(msynArel * (velocityToUse * 0.01));

        AudioInterrupts();

        // now trigger envs
        comboVoices[track].ampEnv.noteOn();
        comboVoices[track].filterEnv.noteOn();
    }

    void handleDexedSynthNoteOnForTrackStep(int track, int step)
    {
        auto &trackToUse = XRSequencer::getHeapTrack(track);
        auto &stepToUse = XRSequencer::getHeapStep(track, step);

        uint8_t noteToUse = trackToUse.note;
        if (XRSequencer::patternTrackStepMods.tracks[track].steps[step].flags[XRSequencer::NOTE])
        {
            noteToUse = XRSequencer::patternTrackStepMods.tracks[track].steps[step].mods[XRSequencer::NOTE];
        }

        uint8_t octaveToUse = trackToUse.octave;
        if (XRSequencer::patternTrackStepMods.tracks[track].steps[step].flags[XRSequencer::OCTAVE])
        {
            octaveToUse = XRSequencer::patternTrackStepMods.tracks[track].steps[step].mods[XRSequencer::OCTAVE];
        }

        uint8_t velocityToUse = trackToUse.velocity;
        if (stepToUse.state == XRSequencer::STATE_ACCENTED) {
            if (XRSequencer::patternTrackStepMods.tracks[track].steps[step].flags[XRSequencer::VELOCITY]) {
                velocityToUse = XRSequencer::patternTrackStepMods.tracks[track].steps[step].mods[XRSequencer::VELOCITY];
            } else {
                velocityToUse = max(trackToUse.velocity, XRSequencer::heapPattern.accent);
            }
        }

        int midiNote = (noteToUse + (12 * (octaveToUse)));

        comboVoices[track].dexed.keydown(midiNote, velocityToUse);
    }

    void handleFmDrumNoteOnForTrackStep(int track, int step)
    {
        auto &trackToUse = XRSequencer::getHeapTrack(track);
        auto &stepToUse = XRSequencer::getHeapStep(track, step);

        auto fmdNoise = getValueNormalizedAsFloat(currentPatternSounds[track].params[FMD_NOISE]);
        auto fmdDecay = getValueNormalizedAsFloat(currentPatternSounds[track].params[FMD_DECAY]);
        auto fmdFm = getValueNormalizedAsFloat(currentPatternSounds[track].params[FMD_FM]);
        auto fmdFreq = getValueNormalizedAsUInt8(currentPatternSounds[track].params[FMD_FREQ]);
        auto fmdPan = getValueNormalizedAsFloat(currentPatternSounds[track].params[FMD_PAN]);

        uint8_t velocityToUse = trackToUse.velocity;
        if (stepToUse.state == XRSequencer::STATE_ACCENTED) {
            if (XRSequencer::patternTrackStepMods.tracks[track].steps[step].flags[XRSequencer::VELOCITY]) {
                velocityToUse = XRSequencer::patternTrackStepMods.tracks[track].steps[step].mods[XRSequencer::VELOCITY];
            } else {
                velocityToUse = max(trackToUse.velocity, XRSequencer::heapPattern.accent);
            }
        }

        AudioNoInterrupts();
        comboVoices[track].fmdrum.noise(fmdNoise);
        comboVoices[track].fmdrum.decay(fmdDecay);
        comboVoices[track].fmdrum.fm(fmdFm);
        comboVoices[track].fmdrum.frequency(fmdFreq);

        comboVoices[track].fmDrumLeftCtrl.gain(getStereoPanValues(fmdPan).right * (velocityToUse * 0.01));
        comboVoices[track].fmDrumRightCtrl.gain(getStereoPanValues(fmdPan).left * (velocityToUse * 0.01));
        AudioInterrupts();

        comboVoices[track].fmdrum.noteOn();
    }

    void handleMIDINoteOnForTrackStep(int track, int step)
    {
        //auto &trackToUse = XRSequencer::getHeapTrack(track);
        //auto &stepToUse = XRSequencer::getHeapStep(track, step);

        XRMIDI::sendNoteOn(64, 100, 1);
    }

    void handleCvGateNoteOnForTrackStep(int track, int step)
    {
        auto &trackToUse = XRSequencer::getHeapTrack(track);

        auto currentSoundForTrack = currentPatternSounds[track];

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

    void handleNoteOffForTrack(int track)
    {
        auto &currTrack = XRSequencer::getHeapTrack(track);
        auto currentSoundForTrack = currentPatternSounds[track];

        switch (currentSoundForTrack.type)
        {
        case T_MONO_SAMPLE:
            {
                if (track < 4) // combo voices
                {
                    comboVoices[track].ampEnv.noteOff();
                }
                else // sample-only voices
                {
                    int tOffset = track - 4;
                    sampleVoices[tOffset].ampEnv.noteOff();
                }
            }
            break;
        case T_MONO_SYNTH:
            {
                if (track < 4) // combo voices
                {
                    comboVoices[track].ampEnv.noteOff();
                    comboVoices[track].filterEnv.noteOff();
                }
            }
            break;
        case T_DEXED_SYNTH:
            {
                uint8_t noteToUse = currTrack.note;
                uint8_t octaveToUse = currTrack.octave;

                int midiNote = (noteToUse + (12 * (octaveToUse)));

                comboVoices[track].dexed.keyup(midiNote);
            }
            break;
        case T_FM_DRUM:
            // n/a
            break;
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
        auto &currTrack = XRSequencer::getHeapTrack(track);
        auto currentSoundForTrack = currentPatternSounds[track];

        // TODO: get track step mods for note, octave, etc

        switch (currentSoundForTrack.type)
        {
        case T_MONO_SAMPLE:
            {
                if (track < 4) // combo voices
                {
                    comboVoices[track].ampEnv.noteOff();
                }
                else // sample-only voices
                {
                    int tOffset = track - 4;
                    sampleVoices[tOffset].ampEnv.noteOff();
                }
            }
            break;
        case T_MONO_SYNTH:
            {
                if (track < 4) // combo voices
                {
                    comboVoices[track].ampEnv.noteOff();
                    comboVoices[track].filterEnv.noteOff();
                }
            }
            break;
        case T_DEXED_SYNTH:
            {
                uint8_t noteToUse = currTrack.note;
                if (XRSequencer::patternTrackStepMods.tracks[track].steps[step].flags[XRSequencer::NOTE])
                {
                    noteToUse = XRSequencer::patternTrackStepMods.tracks[track].steps[step].mods[XRSequencer::NOTE];
                }

                uint8_t octaveToUse = currTrack.octave;
                if (XRSequencer::patternTrackStepMods.tracks[track].steps[step].flags[XRSequencer::OCTAVE])
                {
                    octaveToUse = XRSequencer::patternTrackStepMods.tracks[track].steps[step].mods[XRSequencer::OCTAVE];
                }
                
                int midiNote = (noteToUse + (12 * (octaveToUse)));

                comboVoices[track].dexed.keyup(midiNote);
            }
            break;
        case T_FM_DRUM:
            // n/a
            break;
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
        
        strcpy(currentPatternSounds[track].sampleName, selected.c_str());

        _extPatternSamples[track] = _loader.loadSample(sampleNameStr.c_str());
    }

    void changeComboTrackSoundType(uint8_t t, SOUND_TYPE newType)
    {
        auto currType = currentPatternSounds[t].type;
        if (currType == newType) return;

        currentPatternSounds[t].type = newType;

        AudioNoInterrupts();
        initTrackSound(t);
        initComboVoiceForTrack(t);
        setComboVoiceMixSettingsForTrack(t);
        AudioInterrupts();
    }

    void changeTrackSoundType(uint8_t t, SOUND_TYPE newType)
    {
        if (t < 4)
        {
            changeComboTrackSoundType(t, newType);
            return;
        }

        auto currType = currentPatternSounds[t].type;
        if (currType == newType) return;

        currentPatternSounds[t].type = newType;

        AudioNoInterrupts();
        initTrackSound(t);
        AudioInterrupts();
    }

    void setComboVoiceMixSettingsForTrack(int8_t t)
    {
        auto newType = currentPatternSounds[t].type;

        if (newType == T_EMPTY) {
            return;
        }

        AudioNoInterrupts();

        // make sure voice mix settings are correct for current sound type
        if (newType == T_MONO_SAMPLE)
        {
            auto msmpLvl = getValueNormalizedAsFloat(currentPatternSounds[t].params[MSMP_LEVEL]);

            comboVoices[t].mix.gain(0, msmpLvl); // MONO_SAMPLE ON
            comboVoices[t].mix.gain(1, 0); // MONO_SYNTH  OFF

            comboVoices[t].leftSubMix.gain(0, msmpLvl); // MONO_SAMPLE & MONO_SYNTH ON
            comboVoices[t].leftSubMix.gain(1, 0); // DEXED_SYNTH OFF
            comboVoices[t].leftSubMix.gain(2, 0); // FM_DRUM OFF
            comboVoices[t].rightSubMix.gain(0, msmpLvl); // MONO_SAMPLE & MONO_SYNTH ON
            comboVoices[t].rightSubMix.gain(1, 0); // DEXED_SYNTH OFF
            comboVoices[t].rightSubMix.gain(2, 0); // FM_DRUM OFF
        }
        else if (newType == T_DEXED_SYNTH)
        {
            auto dexeLvl = getValueNormalizedAsFloat(currentPatternSounds[t].params[DEXE_LEVEL]);

            comboVoices[t].leftSubMix.gain(0, 0); // MONO_SAMPLE & MONO_SYNTH OFF
            comboVoices[t].leftSubMix.gain(1, dexeLvl); // DEXED_SYNTH ON
            comboVoices[t].leftSubMix.gain(2, 0); // FM_DRUM OFF
            comboVoices[t].rightSubMix.gain(0, 0); // MONO_SAMPLE & MONO_SYNTH OFF
            comboVoices[t].rightSubMix.gain(1, dexeLvl); // DEXED_SYNTH ON
            comboVoices[t].rightSubMix.gain(2, 0); // FM_DRUM OFF
        }
        else if (newType == T_FM_DRUM)
        {
            auto fmdLvl = getValueNormalizedAsFloat(currentPatternSounds[t].params[FMD_LEVEL]);

            comboVoices[t].leftSubMix.gain(0, 0); // MONO_SAMPLE & MONO_SYNTH OFF
            comboVoices[t].leftSubMix.gain(1, 0); // DEXED_SYNTH OFF
            comboVoices[t].leftSubMix.gain(2, fmdLvl); // FM_DRUM ON
            comboVoices[t].rightSubMix.gain(0, 0); // MONO_SAMPLE & MONO_SYNTH OFF
            comboVoices[t].rightSubMix.gain(1, 0); // DEXED_SYNTH OFF
            comboVoices[t].rightSubMix.gain(2, fmdLvl); // FM_DRUM ON
        }
        else if (newType == T_MONO_SYNTH)
        {
            auto msynLvl = getValueNormalizedAsFloat(currentPatternSounds[t].params[MSYN_LEVEL]);

            comboVoices[t].mix.gain(0, 0); // MONO_SAMPLE OFF
            comboVoices[t].mix.gain(1, msynLvl); // MONO_SYNTH  ON

            comboVoices[t].leftSubMix.gain(0, msynLvl); // MONO_SAMPLE & MONO_SYNTH ON
            comboVoices[t].leftSubMix.gain(1, 0); // DEXED_SYNTH OFF
            comboVoices[t].leftSubMix.gain(2, 0); // FM_DRUM OFF
            comboVoices[t].rightSubMix.gain(0, msynLvl); // MONO_SAMPLE & MONO_SYNTH ON
            comboVoices[t].rightSubMix.gain(1, 0); // DEXED_SYNTH OFF
            comboVoices[t].rightSubMix.gain(2, 0); // FM_DRUM OFF
        }

        AudioInterrupts();
    }

    void triggerTrackManually(uint8_t t, uint8_t note)
    {
        auto currentSoundForTrack = currentPatternSounds[t];

        switch (currentSoundForTrack.type)
        {
        case T_MONO_SAMPLE:
            triggerMonoSampleNoteOn(t, note);

            break;
        case T_MONO_SYNTH:
            triggerMonoSynthNoteOn(t, note);
            
            break;
        case T_DEXED_SYNTH:
            triggerDexedSynthNoteOn(t, note);
            
            break;
        case T_FM_DRUM:
            triggerFmDrumNoteOn(t, note);
            
            break;
        case T_CV_GATE:
            triggerCvGateNoteOn(t, note);
            
            break;
        
        default:
            break;
        }
    }

    void triggerMonoSampleNoteOn(uint8_t t, uint8_t note)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedPattern().tracks[t];

        auto currentSoundForTrack = currentPatternSounds[t];

        auto msmpLooptype = getValueNormalizedAsUInt8(currentSoundForTrack.params[MSMP_LOOPTYPE]);
        auto msmpLoopstart = getValueNormalizedAsInt32(currentSoundForTrack.params[MSMP_LOOPSTART]);
        auto msmpLoopfinish = getValueNormalizedAsInt32(currentSoundForTrack.params[MSMP_LOOPFINISH]);
        auto msmpPlaystart = (play_start)getValueNormalizedAsUInt8(currentSoundForTrack.params[MSMP_PLAYSTART]);
        auto msmpAatt = getValueNormalizedAsFloat(currentSoundForTrack.params[MSMP_AMP_ATTACK]);
        auto msmpAdec = getValueNormalizedAsFloat(currentSoundForTrack.params[MSMP_AMP_DECAY]);
        auto msmpAsus = getValueNormalizedAsFloat(currentSoundForTrack.params[MSMP_AMP_SUSTAIN]);
        auto msmpArel = getValueNormalizedAsFloat(currentSoundForTrack.params[MSMP_AMP_RELEASE]);
        auto msmpPan = getValueNormalizedAsFloat(currentSoundForTrack.params[MSMP_PAN]);

        int tOffset = t - 4;

        if (t < 4)
        {
            AudioNoInterrupts();

            comboVoices[t].leftCtrl.gain(getStereoPanValues(msmpPan).right * (currTrack.velocity * 0.01));
            comboVoices[t].rightCtrl.gain(getStereoPanValues(msmpPan).left * (currTrack.velocity * 0.01));

            comboVoices[t].ampEnv.attack(msmpAatt * (currTrack.velocity * 0.01));
            comboVoices[t].ampEnv.decay(msmpAdec * (currTrack.velocity * 0.01));
            comboVoices[t].ampEnv.sustain(msmpAsus * (currTrack.velocity * 0.01));
            comboVoices[t].ampEnv.release(msmpArel * (currTrack.velocity * 0.01));

            AudioInterrupts();

            comboVoices[t].ampEnv.noteOn();

            std::string trackSampleName(currentSoundForTrack.sampleName);

            if (trackSampleName.length() > 0) {
                comboVoices[t].sample.playRaw(
                    _extPatternSamples[t]->sampledata,
                    _extPatternSamples[t]->samplesize / 2,
                    _numChannels
                );
            }

            // always re-initialize loop type
            comboVoices[t].sample.setLoopType(loopTypeSelMap[msmpLooptype]);

            if (loopTypeSelMap[msmpLooptype] == looptype_none)
            {
                comboVoices[t].sample.setPlayStart(play_start::play_start_sample);
                comboVoices[t].sample.setLoopType(loop_type::looptype_none);
            }
            else if (loopTypeSelMap[msmpLooptype] == looptype_repeat)
            {
                float loopFinishToUse = msmpLoopfinish;

                // if (currTrack.chromatic_enabled)
                // {
                //     float foundBaseFreq = _noteToFreqArr[note];
                //     float octaveFreq = foundBaseFreq * (pow(2, XRKeyMatrix::getKeyboardOctave()));
                //     // float freq = 440.0 * powf(2.0, (12-69) / 12.0);
                //     uint32_t numSamples = 44100 / octaveFreq;
                //     loopFinishToUse = numSamples;
                // }

                comboVoices[t].sample.setPlayStart(msmpPlaystart == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
                comboVoices[t].sample.setLoopStart(msmpLoopstart);
                comboVoices[t].sample.setLoopFinish(loopFinishToUse);
            }
        }
        else
        {
            AudioNoInterrupts();

            sampleVoices[tOffset].leftCtrl.gain(getStereoPanValues(msmpPan).right * (currTrack.velocity * 0.01));
            sampleVoices[tOffset].rightCtrl.gain(getStereoPanValues(msmpPan).left * (currTrack.velocity * 0.01));

            sampleVoices[tOffset].ampEnv.attack(msmpAatt * (currTrack.velocity * 0.01));
            sampleVoices[tOffset].ampEnv.decay(msmpAdec * (currTrack.velocity * 0.01));
            sampleVoices[tOffset].ampEnv.sustain(msmpAsus * (currTrack.velocity * 0.01));
            sampleVoices[tOffset].ampEnv.release(msmpArel * (currTrack.velocity * 0.01));

            AudioInterrupts();

            sampleVoices[tOffset].ampEnv.noteOn();

            std::string trackSampleName(currentSoundForTrack.sampleName);

            if (trackSampleName.length() > 0) {
                sampleVoices[tOffset].sample.playRaw(
                    _extPatternSamples[t]->sampledata,
                    _extPatternSamples[t]->samplesize / 2,
                    _numChannels
                );
            }

            // always re-initialize loop type
            sampleVoices[tOffset].sample.setLoopType(loopTypeSelMap[msmpLooptype]);

            if (loopTypeSelMap[msmpLooptype] == looptype_none)
            {
                sampleVoices[tOffset].sample.setPlayStart(play_start::play_start_sample);
                sampleVoices[tOffset].sample.setLoopType(loop_type::looptype_none);
            }
            else if (loopTypeSelMap[msmpLooptype] == looptype_repeat)
            {
                float loopFinishToUse = msmpLoopfinish;

                // if (currTrack.chromatic_enabled)
                // {
                //     float foundBaseFreq = _noteToFreqArr[note];
                //     float octaveFreq = foundBaseFreq * (pow(2, XRKeyMatrix::getKeyboardOctave()));
                //     // float freq = 440.0 * powf(2.0, (12-69) / 12.0);
                //     uint32_t numSamples = 44100 / octaveFreq;
                //     loopFinishToUse = numSamples;
                // }

                sampleVoices[tOffset].sample.setPlayStart(msmpPlaystart == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
                sampleVoices[tOffset].sample.setLoopStart(msmpLoopstart);
                sampleVoices[tOffset].sample.setLoopFinish(loopFinishToUse);
            }
        }
    }

    void triggerMonoSynthNoteOn(uint8_t t, uint8_t note)
    {
        auto &currTrack = XRSequencer::getHeapTrack(t);

        auto currentSoundForTrack = currentPatternSounds[t];
       
        auto msynPan = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_PAN]);
        auto msynFine = getValueNormalizedAsInt8(currentSoundForTrack.params[MSYN_FINE]);
        auto msynDetune = getValueNormalizedAsInt8(currentSoundForTrack.params[MSYN_DETUNE]);

        auto msynFatt = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_FILTER_ATTACK]);
        auto msynFdec = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_FILTER_DECAY]);
        auto msynFsus = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_FILTER_SUSTAIN]);
        auto msynFrel = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_FILTER_RELEASE]);

        auto msynAatt = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_AMP_ATTACK]);
        auto msynAdec = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_AMP_DECAY]);
        auto msynAsus = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_AMP_SUSTAIN]);
        auto msynArel = getValueNormalizedAsFloat(currentSoundForTrack.params[MSYN_AMP_RELEASE]);

        Serial.printf("detune: %d\n", msynDetune);

        AudioNoInterrupts();

        float foundBaseFreq = _noteToFreqArr[note];
        float octaveFreqA = (foundBaseFreq + (msynFine * 0.01)) * (pow(2, XRKeyMatrix::getKeyboardOctave()));
        float octaveFreqB = (foundBaseFreq * pow(2.0, (float)msynDetune / 12.0)) * (pow(2, XRKeyMatrix::getKeyboardOctave()));

        // Serial.printf("note: %d, foundBaseFreq: %f, octaveFreqA: %f, octaveFreqB: %f\n", note, foundBaseFreq, octaveFreqA, octaveFreqB);

        comboVoices[t].osca.frequency(octaveFreqA);
        comboVoices[t].oscb.frequency(octaveFreqB);

        comboVoices[t].leftCtrl.gain(getStereoPanValues(msynPan).right * (currTrack.velocity * 0.01));
        comboVoices[t].rightCtrl.gain(getStereoPanValues(msynPan).left * (currTrack.velocity * 0.01));

        comboVoices[t].filterEnv.attack(msynFatt * (currTrack.velocity * 0.01));
        comboVoices[t].filterEnv.decay(msynFdec * (currTrack.velocity * 0.01));
        comboVoices[t].filterEnv.sustain(msynFsus * (currTrack.velocity * 0.01));
        comboVoices[t].filterEnv.release(msynFrel * (currTrack.velocity * 0.01));

        comboVoices[t].ampEnv.attack(msynAatt * (currTrack.velocity * 0.01));
        comboVoices[t].ampEnv.decay(msynAdec * (currTrack.velocity * 0.01));
        comboVoices[t].ampEnv.sustain(msynAsus * (currTrack.velocity * 0.01));
        comboVoices[t].ampEnv.release(msynArel * (currTrack.velocity * 0.01));

        AudioInterrupts();

        // now trigger envs
        comboVoices[t].ampEnv.noteOn();
        comboVoices[t].filterEnv.noteOn();
    }

    void triggerDexedSynthNoteOn(uint8_t t, uint8_t note)
    {
        int midiNote = (note + (12 * (XRKeyMatrix::getKeyboardOctave())));

        comboVoices[t].dexed.keydown(midiNote, 50);
    }

    void triggerFmDrumNoteOn(uint8_t t, uint8_t note)
    {
        comboVoices[t].fmdrum.noteOn();
    }

    void triggerCvGateNoteOn(uint8_t t, uint8_t note)
    {
        auto currentSoundForTrack = currentPatternSounds[t];
        auto cvgaPort = getValueNormalizedAsInt8(currentSoundForTrack.params[0]); // TODO: use enum

        // for (int i = 0; i < 128; i++)
        // {
        //     _cvLevels[i] = i * 26;
        // }

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

        auto currentSoundForTrack = currentPatternSounds[currSelTrackNum];

        if (currSelTrackNum > 3) {
            sampleVoices[currSelTrackNum - 4].ampEnv.noteOff();
        } else {

            switch (currentSoundForTrack.type)
            {
            case T_MONO_SAMPLE:
                {
                    comboVoices[currSelTrackNum].ampEnv.noteOff();
                }
                break;
            case T_MONO_SYNTH:
                {
                    comboVoices[currSelTrackNum].ampEnv.noteOff();
                    comboVoices[currSelTrackNum].filterEnv.noteOff();
                }
                break;
            case T_DEXED_SYNTH:
                {
                    int midiNote = (noteOnKeyboard + (12 * (XRKeyMatrix::getKeyboardOctave())));
                    comboVoices[currSelTrackNum].dexed.keyup(midiNote);
                }
                break;
            case T_MIDI:
                {
                    XRMIDI::sendNoteOff(64, 100, 1);
                }
                break;
            case T_CV_GATE:
                {
                    auto cvgaPort = getValueNormalizedAsInt8(currentSoundForTrack.params[0]); // TODO: use enum
                    
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
    }

    PANNED_AMOUNTS getStereoPanValues(float pan)
    {
        PANNED_AMOUNTS amounts;

        amounts.left = 1.0;
        if (pan < 0) {
            amounts.left += pan;
        }

        amounts.right = 1.0;
        if (pan > 0) {
            amounts.right -= pan;
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
            outputStr = "MONO >";
            break;

        case T_DEXED_SYNTH:
            outputStr = "DEXED >";
            break;

        case T_BRAIDS_SYNTH:
            outputStr = "BRAIDS >";
            break;

        case T_FM_DRUM:
            outputStr = "FM-DRUM >";
            break;

        case T_MIDI:
            outputStr = "MIDI";
            break;

        case T_CV_GATE:
            outputStr = "CV/GATE";
            break;

        case T_CV_TRIG:
            outputStr = "CV/TRIG";
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

        case T_DEXED_SYNTH:
            str = "INIT";

            break;

        case T_FM_DRUM:
            str = "INIT";

            break;

        case T_MIDI:
            str = "MIDI";

            break;

        case T_CV_GATE:
            str = "CV/GATE";

            break;

        case T_CV_TRIG:
            str = "CV/TRIG";

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

        std::string outputStr = soundCurrPageNameMap[currentPatternSounds[currTrack].type][currPage];

        return outputStr;
    }

    uint8_t getPageCountForCurrentTrack()
    {
        auto currTrack = XRSequencer::getCurrentSelectedTrackNum();

        return soundPageNumMap[currentPatternSounds[currTrack].type];
    }

    uint8_t getPageCountForTrack(int track)
    {
        return soundPageNumMap[currentPatternSounds[track].type];
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

        auto looptype = getValueNormalizedAsUInt8(currentPatternSounds[currentSelectedTrackNum].params[MSMP_LOOPTYPE]);
        auto chromatic = getValueNormalizedAsBool(currentPatternSounds[currentSelectedTrackNum].params[MSMP_CHROMATIC]);

        uint8_t looptypeToUse = looptype;

        // if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStepNum > -1)
        // {
        //     looptypeToUse = modsForCurrentTrackStep.looptype;
        // }

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

    void applyCurrentDexedPatchToSound()
    {
        auto track = XRSequencer::getCurrentSelectedTrackNum();

        uint8_t dexedParamData[MAXIMUM_DEXED_SOUND_PARAMS];
        comboVoices[track].dexed.getVoiceData(dexedParamData);
        for (int dp=0; dp<MAXIMUM_DEXED_SOUND_PARAMS; dp++)
        {
            currentPatternSounds[track].dexedParams[dp] = dexedParamData[dp];
        }
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
        return (int32_t)(value * 100);
    }

    int32_t getBoolValuePaddedAsInt32(bool value)
    {
        return (int32_t)(value * 100);
    }
}