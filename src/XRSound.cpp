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
    ComboVoice comboVoices[COMBO_VOICE_COUNT] = {
        ComboVoice(
            dexed1, vmsample1, vosca1, voscb1, vnoise1, voscmix1, vdc1, vlfilter1, vfilterenv1, vmix1, venv1, vleft1, vright1, vsubmixl1, vsubmixr1),
        ComboVoice(
            dexed2, vmsample2, vosca2, voscb2, vnoise2, voscmix2, vdc2, vlfilter2, vfilterenv2, vmix2, venv2, vleft2, vright2, vsubmixl2, vsubmixr2),
        ComboVoice(
            dexed3, vmsample3, vosca3, voscb3, vnoise3, voscmix3, vdc3, vlfilter3, vfilterenv3, vmix3, venv3, vleft3, vright3, vsubmixl3, vsubmixr3),
        ComboVoice(
            dexed4, vmsample4, vosca4, voscb4, vnoise4, voscmix4, vdc4, vlfilter4, vfilterenv4, vmix4, venv4, vleft4, vright4, vsubmixl4, vsubmixr4),
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

    float _noteToFreqArr[13] = {
        16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.50, 25.96, 27.50, 29.14, 30.87, 32.70
    };

    int _cvLevels[128];

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

    // 8MB max of samples per pattern in external PSRAM, 1 sample allowed per track for now    
    newdigate::audiosample *_extPatternSamples[MAXIMUM_SEQUENCER_TRACKS];
    newdigate::flashloader _loader;
    uint8_t _numChannels = 1;
    
    void init()
    {
        for (int i = 0; i < 128; i++)
        {
            _cvLevels[i] = i * 26;
        }
    }

    ComboVoice &getComboVoiceForCurrentTrack()
    {
        return comboVoices[XRSequencer::getCurrentSelectedTrackNum()];
    }

    SampleVoice &getSampleVoiceForTrack(int t)
    {
        return sampleVoices[t];
    }
    
    SOUND_CONTROL_MODS getControlModDataForPattern()
    {
        SOUND_CONTROL_MODS mods;

        auto &pattern = XRSequencer::getHeapCurrentSelectedPattern();

        std::string grooveForPattern = pattern.groove_id > -1 ? XRClock::getGrooveString(pattern.groove_id) : "";
        std::string grooveAmountForPattern = XRClock::getGrooveAmountString(pattern.groove_amount);

        mods.aName = "L.STEP";
        mods.bName = "GROOVE";
        mods.cName = "GR.AMT";
        mods.dName = "--";

        mods.aValue = std::to_string(pattern.last_step);
        mods.bValue = pattern.groove_id > -1 ? grooveForPattern : "OFF";
        mods.cValue = pattern.groove_id > -1 ? grooveAmountForPattern : "--";
        mods.dValue = "--";

        return mods;
    }

    SOUND_CONTROL_MODS getControlModDataForTrack()
    {
        SOUND_CONTROL_MODS mods;

        auto &track = XRSequencer::getHeapCurrentSelectedTrack();

        switch (track.track_type)
        {
        case XRSequencer::SUBTRACTIVE_SYNTH:
            mods = getSubtractiveSynthControlModData();
            break;

        case XRSequencer::RAW_SAMPLE:
            mods = getRawSampleControlModData();
            break;

        case XRSequencer::WAV_SAMPLE:
            mods = getWavSampleControlModData();
            break;

        case XRSequencer::DEXED:
            mods = getDexedControlModData();
            break;

        case XRSequencer::MIDI_OUT:
            mods = getMidiControlModData();
            break;

        case XRSequencer::CV_GATE:
            mods = getCvGateControlModData();
            break;

        case XRSequencer::CV_TRIG:
            mods = getCvTrigControlModData();
            break;

        default:
            break;
        }

        return mods;
    }

    SOUND_CONTROL_MODS getSubtractiveSynthControlModData()
    {
        SOUND_CONTROL_MODS mods;

        auto &currentSelectedTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto currentSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto &modsForCurrentTrackStep = XRSequencer::getModsForCurrentTrackStep();
        auto currentUXMode = XRUX::getCurrentMode();

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            mods.aName = "LSTP";
            mods.bName = "LEN";
            mods.cName = "VELO";
            mods.dName = "PROB";

            mods.aValue = std::to_string(currentSelectedTrack.last_step);

            if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStepNum > -1)
            {
                mods.bValue = std::to_string(modsForCurrentTrackStep.length);
            }
            else
            {
                mods.bValue = std::to_string(currentSelectedTrack.length);
            }

            if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStepNum > -1)
            {
                mods.cValue = std::to_string(modsForCurrentTrackStep.velocity);
            }
            else
            {
                mods.cValue = std::to_string(currentSelectedTrack.velocity);
            }

            mods.dValue = "100%"; // TODO: impl
            break;

        case 1: // OSC
            mods.aName = "WAVE";
            mods.bName = "DET";
            mods.cName = "FINE";
            mods.dName = "WID";

            if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStepNum > -1)
            {
                mods.aValue = getWaveformName(modsForCurrentTrackStep.waveform);
            }
            else
            {
                mods.aValue = getWaveformName(currentSelectedTrack.waveform);
            }
            mods.bValue = std::to_string(currentSelectedTrack.detune);
            mods.cValue = std::to_string(currentSelectedTrack.fine);
            mods.dValue = std::to_string((float)round(currentSelectedTrack.width * 100) / 100);
            mods.dValue = mods.dValue.substr(0, 4);

            break;

        case 2: // FILTER
            mods.aName = "NOIS";
            mods.bName = "FREQ";
            mods.cName = "RESO";
            mods.dName = "AMT";

            mods.aValue = std::to_string((float)round(currentSelectedTrack.noise * 100) / 100);
            mods.aValue = mods.aValue.substr(0, 3);
            mods.bValue = std::to_string(round(currentSelectedTrack.cutoff));
            mods.bValue = mods.bValue.substr(0, 5);
            mods.cValue = std::to_string((float)round(currentSelectedTrack.res * 100) / 100);
            mods.cValue = mods.cValue.substr(0, 4);
            mods.dValue = std::to_string((float)round(currentSelectedTrack.filterenvamt * 100) / 100);
            mods.dValue = mods.dValue.substr(0, 4);

            break;

        case 3: // FILTER ENV
            mods.aName = "ATT";
            mods.bName = "DEC";
            mods.cName = "SUS";
            mods.dName = "REL";

            mods.aValue = std::to_string((float)round(currentSelectedTrack.filter_attack * 100) / 100);
            mods.aValue = mods.aValue.substr(0, 3);
            mods.bValue = std::to_string((float)round(currentSelectedTrack.filter_decay * 100) / 100);
            mods.bValue = mods.bValue.substr(0, 5);
            mods.cValue = std::to_string((float)round(currentSelectedTrack.filter_sustain * 100) / 100);
            mods.cValue = mods.cValue.substr(0, 3);
            mods.dValue = std::to_string((float)round(currentSelectedTrack.filter_release * 100) / 100);
            mods.dValue = mods.dValue.substr(0, 5);

            break;

        case 4: // AMP ENV
            mods.aName = "ATT";
            mods.bName = "DEC";
            mods.cName = "SUS";
            mods.dName = "REL";

            mods.aValue = std::to_string((float)round(currentSelectedTrack.amp_attack * 100) / 100);
            mods.aValue = mods.aValue.substr(0, 3);
            mods.bValue = std::to_string((float)round(currentSelectedTrack.amp_decay * 100) / 100);
            mods.bValue = mods.bValue.substr(0, 5);
            mods.cValue = std::to_string((float)round(currentSelectedTrack.amp_sustain * 100) / 100);
            mods.cValue = mods.cValue.substr(0, 3);
            mods.dValue = std::to_string((float)round(currentSelectedTrack.amp_release * 100) / 100);
            mods.dValue = mods.dValue.substr(0, 5);

            break;

        case 5: // OUTPUT
            mods.aName = "LVL";
            mods.bName = "PAN";
            mods.cName = "--";
            mods.dName = "--"; // fx send?

            mods.aValue = std::to_string(round(currentSelectedTrack.level * 100));
            mods.bValue = std::to_string((float)round(currentSelectedTrack.pan * 100) / 100);
            mods.bValue = mods.bValue.substr(0, 3);
            mods.cValue = "--";
            mods.dValue = "--";

            mods.bFloatValue = currentSelectedTrack.pan;
            mods.bType = RANGE;

            break;

        default:
            break;
        }

        return mods;
    }

    SOUND_CONTROL_MODS getRawSampleControlModData()
    {
        SOUND_CONTROL_MODS mods;

        auto &currentSelectedTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto currentSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto &modsForCurrentTrackStep = XRSequencer::getModsForCurrentTrackStep();
        auto currentUXMode = XRUX::getCurrentMode();

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            mods.aName = "L.STEP";
            mods.bName = "--";
            mods.cName = "SPEED";
            mods.dName = "FILE";

            mods.aValue = std::to_string(currentSelectedTrack.last_step);
            mods.bValue = "";

            if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStepNum > -1)
            {
                mods.cValue = getPlaybackSpeedStr(modsForCurrentTrackStep.sample_play_rate);
            }
            else
            {
                mods.cValue = getPlaybackSpeedStr(currentSelectedTrack.sample_play_rate);
            }

            mods.dValue = "--";

            break;

        case 1: // LOOPING
        {
            mods.aName = "TYPE";
            mods.bName = "START";
            mods.cName = "FINISH";
            mods.dName = "PLAYST";

            mods.aValue = getLoopTypeName();

            uint32_t loopstartToUse = currentSelectedTrack.loopstart;

            if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStepNum > -1)
            {
                loopstartToUse = modsForCurrentTrackStep.loopstart;
            }

            std::string lsStr = std::to_string(loopstartToUse);
            lsStr += "ms";

            mods.bValue = lsStr;

            uint32_t loopfinishToUse = currentSelectedTrack.loopfinish;

            if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStepNum > -1)
            {
                loopfinishToUse = modsForCurrentTrackStep.loopfinish;
            }

            std::string lfStr = std::to_string(loopfinishToUse);
            lfStr += "ms";

            mods.cValue = lfStr;

            play_start playstartToUse = currentSelectedTrack.playstart;

            if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStepNum > -1)
            {
                playstartToUse = modsForCurrentTrackStep.playstart;
            }

            mods.dValue = playstartToUse == play_start::play_start_loop ? "LOOP" : "SAMPLE";

            break;
        }
        case 2: // AMP ENV
            mods.aName = "ATT";
            mods.bName = "DEC";
            mods.cName = "SUS";
            mods.dName = "REL";

            mods.aValue = std::to_string((float)round(currentSelectedTrack.amp_attack * 100) / 100);
            mods.aValue = mods.aValue.substr(0, 3);
            mods.bValue = std::to_string((float)round(currentSelectedTrack.amp_decay * 100) / 100);
            mods.bValue = mods.bValue.substr(0, 3);
            mods.cValue = std::to_string((float)round(currentSelectedTrack.amp_sustain * 100) / 100);
            mods.cValue = mods.cValue.substr(0, 3);
            mods.dValue = std::to_string((float)round(currentSelectedTrack.amp_release * 100) / 100);
            mods.dValue = mods.dValue.substr(0, 3);

            break;

        case 3: // OUTPUT
            mods.aName = "LEVEL";
            mods.bName = "PAN";
            mods.cName = "--";
            mods.dName = "--"; // fx send?

            mods.aValue = std::to_string(round(currentSelectedTrack.level * 100));
            mods.bValue = std::to_string((float)round(currentSelectedTrack.pan * 100) / 100);
            mods.bValue = mods.bValue.substr(0, 3);
            mods.bFloatValue = currentSelectedTrack.pan;
            mods.bType = RANGE;

            mods.cValue = "--";
            mods.dValue = "--";
            break;

        default:
            break;
        }

        return mods;
    }

    SOUND_CONTROL_MODS getWavSampleControlModData()
    {
        SOUND_CONTROL_MODS mods;

        auto &currentSelectedTrack = XRSequencer::getHeapCurrentSelectedTrack();
        // auto currentSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        // auto modsForCurrentTrackStep = XRSequencer::getModsForCurrentTrackStep();
        // auto currentUXMode = XRUX::getCurrentMode();

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            mods.aName = "L.STEP";
            mods.bName = "FILE";
            mods.cName = "POS";
            mods.dName = "--";

            mods.aValue = std::to_string(currentSelectedTrack.last_step);
            //mods.bValue = std::to_string(currentSelectedTrack.wav_sample_id + 1);
            mods.bValue = "--";
            mods.cValue = "0ms";
            mods.dValue = "--";
            break;

        default:
            break;
        }

        return mods;
    }

    SOUND_CONTROL_MODS getDexedControlModData()
    {
        SOUND_CONTROL_MODS mods;

        auto &currentSelectedTrack = XRSequencer::getHeapCurrentSelectedTrack();
        // auto currentSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        // auto modsForCurrentTrackStep = XRSequencer::getModsForCurrentTrackStep();
        // auto currentUXMode = XRUX::getCurrentMode();

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            mods.aName = "LSTP";
            mods.bName = "LEN";
            mods.cName = "BNK";
            mods.dName = "PAT";

            mods.aValue = std::to_string(currentSelectedTrack.last_step); // TODO : impl
            mods.bValue = std::to_string(currentSelectedTrack.length);    // TODO: impl
            mods.cValue = std::to_string(XRSD::dexedCurrentBank);
            mods.dValue = std::to_string(XRSD::dexedCurrentPatch);
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
        // auto currentSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        // auto modsForCurrentTrackStep = XRSequencer::getModsForCurrentTrackStep();
        // auto currentUXMode = XRUX::getCurrentMode();

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            mods.aName = "LSTP";
            mods.bName = "LEN";
            mods.cName = "CHAN";
            mods.dName = "VELO";

            mods.aValue = std::to_string(currentSelectedTrack.last_step); // TODO : impl
            mods.bValue = std::to_string(currentSelectedTrack.length);    // TODO: impl
            mods.cValue = std::to_string(currentSelectedTrack.channel);   // TODO: impl
            mods.dValue = std::to_string(currentSelectedTrack.velocity);  // TODO: impl
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
        // auto currentSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        // auto modsForCurrentTrackStep = XRSequencer::getModsForCurrentTrackStep();
        // auto currentUXMode = XRUX::getCurrentMode();

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
        {
            mods.aName = "LSTP";
            mods.bName = "LEN";
            mods.cName = "OUT";
            mods.dName = "PROB";

            mods.aValue = std::to_string(currentSelectedTrack.last_step);
            mods.bValue = std::to_string(currentSelectedTrack.length);

            std::string outputChanStr = std::to_string(currentSelectedTrack.channel);
            outputChanStr += "AB";

            mods.cValue = outputChanStr;
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
        // auto currentSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        // auto modsForCurrentTrackStep = XRSequencer::getModsForCurrentTrackStep();
        // auto currentUXMode = XRUX::getCurrentMode();

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            mods.aName = "LSTP";
            mods.bName = "OUT";
            mods.cName = "PROB";
            mods.dName = "--";

            mods.aValue = std::to_string(currentSelectedTrack.last_step);
            mods.bValue = "1AB";  // TODO: impl
            mods.cValue = "100%"; // TODO: impl
            mods.dValue = "--";   // TODO: impl
            break;

        default:
            break;
        }

        return mods;
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
    
    int getWaveformNumber(uint8_t waveformType)
    {
        return _waveformFindMap[(int)waveformType];
    }

    int getWaveformTypeSelection(uint8_t waveformNumber)
    {
        return _waveformSelMap[(int)waveformNumber];
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

        auto &currentSelectedTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto currentSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        // auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto &modsForCurrentTrackStep = XRSequencer::getModsForCurrentTrackStep();
        auto currentUXMode = XRUX::getCurrentMode();

        uint8_t looptypeToUse = currentSelectedTrack.looptype;

        if (currentUXMode == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStepNum > -1)
        {
            looptypeToUse = modsForCurrentTrackStep.looptype;
        }

        if (loopTypeSelMap[looptypeToUse] == loop_type::looptype_none)
        {
            outputStr += "OFF";
        }
        else if (loopTypeSelMap[looptypeToUse] == loop_type::looptype_repeat)
        {
            if (currentSelectedTrack.chromatic_enabled)
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
    
    void loadVoiceSettings()
    {
        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            initSoundsForTrack(t);
        }

        //return;

        auto &seqHeap = XRSequencer::getSequencerHeap();

        for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            if (t > 3) { // sample-only voice tracks
                configureSampleVoiceSettingsOnLoad(t);

                continue;
            }

            ComboVoice trackVoice = comboVoices[t];

            if (seqHeap.pattern.tracks[t].track_type == XRSequencer::TRACK_TYPE::RAW_SAMPLE)
            {
                XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::TRACK_TYPE::RAW_SAMPLE);

                // turn sample volume all the way up
                trackVoice.mix.gain(0, 1);
                // turn synth volume all the way down
                trackVoice.mix.gain(1, 0); // synth

                // turn off dexed, turn on sample and synth
                trackVoice.leftSubMix.gain(0, 1);
                trackVoice.leftSubMix.gain(1, 0);
                trackVoice.rightSubMix.gain(0, 1);
                trackVoice.rightSubMix.gain(1, 0);
            }
            else if (seqHeap.pattern.tracks[t].track_type == XRSequencer::TRACK_TYPE::WAV_SAMPLE)
            {
                XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::TRACK_TYPE::WAV_SAMPLE);

                // only create buffers for stereo samples when needed
                // trackVoice.wSample.createBuffer(2048, AudioBuffer::inExt);

                // turn sample volume all the way up
                // trackVoice.mix.gain(0, 1);
                // // turn synth volumes all the way down
                // trackVoice.mix.gain(1, 0); // synth
            }
            else if (seqHeap.pattern.tracks[t].track_type == XRSequencer::TRACK_TYPE::DEXED)
            {
                XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::TRACK_TYPE::DEXED);

                trackVoice.mix.gain(0, 1); // mono sample
                trackVoice.mix.gain(1, 1); // synth

                // turn on dexed, turn off sample and synth
                trackVoice.leftSubMix.gain(0, 0);
                trackVoice.leftSubMix.gain(1, 1);
                trackVoice.rightSubMix.gain(0, 0);
                trackVoice.rightSubMix.gain(1, 1);
            }
            else if (seqHeap.pattern.tracks[t].track_type == XRSequencer::TRACK_TYPE::SUBTRACTIVE_SYNTH)
            {
                XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::TRACK_TYPE::SUBTRACTIVE_SYNTH);

                auto currTrack = XRSequencer::getHeapTrack(t);

                // TESTING: revert amp env to normal synth setting
                trackVoice.ampEnv.attack(currTrack.amp_attack);
                trackVoice.ampEnv.decay(currTrack.amp_decay);
                trackVoice.ampEnv.sustain(currTrack.amp_sustain);
                trackVoice.ampEnv.release(currTrack.amp_release);

                // turn sample volume all the way down
                trackVoice.mix.gain(0, 0);
                // turn synth volumes all the way up
                trackVoice.mix.gain(1, 1); // ladder

                // turn off dexed, turn on sample and synth
                trackVoice.leftSubMix.gain(0, 1);
                trackVoice.leftSubMix.gain(1, 0);
                trackVoice.rightSubMix.gain(0, 1);
                trackVoice.rightSubMix.gain(1, 0);
            }
            else if (seqHeap.pattern.tracks[t].track_type == XRSequencer::TRACK_TYPE::MIDI_OUT)
            {
                XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::TRACK_TYPE::MIDI_OUT);

                // turn all audio for this track voice down
                trackVoice.mix.gain(0, 0); // mono sample
                trackVoice.mix.gain(1, 0); // synth
            }
            else if (seqHeap.pattern.tracks[t].track_type == XRSequencer::TRACK_TYPE::CV_GATE)
            {
                XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::TRACK_TYPE::CV_GATE);

                // turn all audio for this track voice down
                trackVoice.mix.gain(0, 0); // mono sample
                trackVoice.mix.gain(1, 0); // synth
            }
            else if (seqHeap.pattern.tracks[t].track_type == XRSequencer::TRACK_TYPE::CV_TRIG)
            {
                XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::TRACK_TYPE::CV_TRIG);

                // turn all audio for this track voice down
                trackVoice.mix.gain(0, 0); // mono sample
                trackVoice.mix.gain(1, 0); // synth
            }
        }
    }

    void configureSampleVoiceSettingsOnLoad(int t)
    {
        auto &track = XRSequencer::getHeapTrack(t);
        auto &sampleVoice = sampleVoices[t-4];

        if (track.track_type == XRSequencer::TRACK_TYPE::WAV_SAMPLE)
        {
            // only create buffers for stereo samples when needed
            // sampleVoices[t-4].wSample.createBuffer(2048, AudioBuffer::inExt);

            //XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::TRACK_TYPE::WAV_SAMPLE);
        } else {
            // make sure sample channels are up
            sampleVoice.leftSubMix.gain(0, track.level);
            sampleVoice.rightSubMix.gain(0, track.level);
        }
    }

    void initSoundsForTrack(int t)
    {
    AudioNoInterrupts();

        // TODO: eventually need to restore all sounds for all patterns and their tracks?
        auto &currTrack = XRSequencer::getHeapTrack(t);

        if (currTrack.track_type == XRSequencer::RAW_SAMPLE) {
            std::string sampleName = "/audio enjoyer/xr-1/samples/";
            sampleName += currTrack.sample_name;

            if (sampleName != "/audio enjoyer/xr-1/samples/") {
                Serial.printf("initializing this sample name: %s\n", sampleName.c_str());

                _extPatternSamples[t] = _loader.loadSample(sampleName.c_str());
            }
        }

        if (t < 4) { // combo voice tracks
            // init mono RAW sample
            comboVoices[t].rSample.setPlaybackRate(currTrack.sample_play_rate);
            comboVoices[t].rSample.enableInterpolation(true);

            // init dexed
            //comboVoices[t].dexed.loadInitVoice();
            // TODO: impl loadDexedVoiceToCurrentTrack();
            XRSD::loadDexedVoiceToCurrentTrack();
            
            // comboVoices[t].dexed.setMonoMode(true);
            // comboVoices[t].dexed.setTranspose(36);

            // init synth
            comboVoices[t].osca.begin(currTrack.waveform);
            comboVoices[t].osca.amplitude(currTrack.oscalevel);
            comboVoices[t].osca.frequency(261.63); // C4 TODO: use find freq LUT with track note
            comboVoices[t].osca.pulseWidth(currTrack.width);
            comboVoices[t].oscb.begin(currTrack.waveform);
            comboVoices[t].oscb.amplitude(currTrack.oscblevel);
            comboVoices[t].oscb.frequency(261.63); // C3 TODO: use find freq LUT with track note + detune
            comboVoices[t].oscb.pulseWidth(currTrack.width);
            comboVoices[t].noise.amplitude(currTrack.noise);
            comboVoices[t].oscMix.gain(0, 0.33);
            comboVoices[t].oscMix.gain(1, 0.33);
            comboVoices[t].oscMix.gain(2, 0.33);
            comboVoices[t].dc.amplitude(currTrack.filterenvamt);
            comboVoices[t].lfilter.frequency(currTrack.cutoff);
            comboVoices[t].lfilter.resonance(currTrack.res);
            comboVoices[t].lfilter.octaveControl(4);
            comboVoices[t].filterEnv.attack(currTrack.filter_attack);
            comboVoices[t].filterEnv.decay(currTrack.filter_decay);
            comboVoices[t].filterEnv.sustain(currTrack.filter_sustain);
            comboVoices[t].filterEnv.release(currTrack.filter_release);
            comboVoices[t].ampEnv.attack(currTrack.amp_attack * (currTrack.velocity * 0.01));
            comboVoices[t].ampEnv.decay(currTrack.amp_decay * (currTrack.velocity * 0.01));
            comboVoices[t].ampEnv.sustain(currTrack.amp_sustain * (currTrack.velocity * 0.01));
            comboVoices[t].ampEnv.release(currTrack.amp_release * (currTrack.velocity * 0.01));

            // output
            comboVoices[t].mix.gain(0, 1); // raw sample
            comboVoices[t].mix.gain(1, 0); // synth

            // mono to L&R
            comboVoices[t].leftCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));
            comboVoices[t].rightCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));

            // Sub L&R mixers
            comboVoices[t].leftSubMix.gain(1, currTrack.level);  // wav sample left
            comboVoices[t].leftSubMix.gain(0, currTrack.level);  // dexed left
            comboVoices[t].rightSubMix.gain(1, currTrack.level); // wav sample right
            comboVoices[t].rightSubMix.gain(0, currTrack.level); // dexed right
        }
        else
        { // sample-only voice tracks
            // init mono RAW sample
            int tOffset = t - 4;

            sampleVoices[tOffset].rSample.setPlaybackRate(currTrack.sample_play_rate);
            sampleVoices[tOffset].rSample.enableInterpolation(true);

            sampleVoices[tOffset].ampEnv.attack(currTrack.amp_attack * (currTrack.velocity * 0.01));
            sampleVoices[tOffset].ampEnv.decay(currTrack.amp_decay * (currTrack.velocity * 0.01));
            sampleVoices[tOffset].ampEnv.sustain(currTrack.amp_sustain * (currTrack.velocity * 0.01));
            sampleVoices[tOffset].ampEnv.release(currTrack.amp_release * (currTrack.velocity * 0.01));

            // mono to L&R
            sampleVoices[tOffset].leftCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));
            sampleVoices[tOffset].rightCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));

            // Sub L&R mixers
            sampleVoices[tOffset].leftSubMix.gain(1, currTrack.level);  // raw sample / synth left
            sampleVoices[tOffset].rightSubMix.gain(1, currTrack.level); // raw sample / synth right
        }

        configureVoiceSettingsForTrack(t);

        AudioInterrupts();

        XRSequencer::setTrackNeedsInit(t, false);
    }

    void initAllTrackSounds()
    {
        // configure combo voice audio objects
        for (int v = 0; v < COMBO_VOICE_COUNT; v++)
        {
            // TODO: eventually need to restore all sounds for all patterns and their tracks?
            auto &currTrack = XRSequencer::getHeapTrack(v);

            // init mono RAW sample
            comboVoices[v].rSample.setPlaybackRate(currTrack.sample_play_rate);
            comboVoices[v].rSample.enableInterpolation(true);

            // init dexed
            // comboVoices[v].dexed.loadInitVoice();
            // comboVoices[v].dexed.loadVoiceParameters(fmpiano_sysex);
            XRSD::loadDexedVoiceToCurrentTrack();
            
            // comboVoices[v].dexed.setMonoMode(true);
            // comboVoices[v].dexed.setTranspose(36);

            // init synth
            comboVoices[v].osca.begin(currTrack.waveform);
            comboVoices[v].osca.amplitude(currTrack.oscalevel);
            comboVoices[v].osca.frequency(261.63); // C4 TODO: use find freq LUT with track note
            comboVoices[v].osca.pulseWidth(currTrack.width);
            comboVoices[v].oscb.begin(currTrack.waveform);
            comboVoices[v].oscb.amplitude(currTrack.oscblevel);
            comboVoices[v].oscb.frequency(261.63); // C3 TODO: use find freq LUT with track note + detune
            comboVoices[v].oscb.pulseWidth(currTrack.width);
            comboVoices[v].noise.amplitude(currTrack.noise);
            comboVoices[v].oscMix.gain(0, 0.33);
            comboVoices[v].oscMix.gain(1, 0.33);
            comboVoices[v].oscMix.gain(2, 0.33);
            comboVoices[v].dc.amplitude(currTrack.filterenvamt);
            comboVoices[v].lfilter.frequency(currTrack.cutoff);
            comboVoices[v].lfilter.resonance(currTrack.res);
            comboVoices[v].lfilter.octaveControl(4);
            comboVoices[v].filterEnv.attack(currTrack.filter_attack);
            comboVoices[v].filterEnv.decay(currTrack.filter_decay);
            comboVoices[v].filterEnv.sustain(currTrack.filter_sustain);
            comboVoices[v].filterEnv.release(currTrack.filter_release);
            comboVoices[v].ampEnv.attack(currTrack.amp_attack * (currTrack.velocity * 0.01));
            comboVoices[v].ampEnv.decay(currTrack.amp_decay * (currTrack.velocity * 0.01));
            comboVoices[v].ampEnv.sustain(currTrack.amp_sustain * (currTrack.velocity * 0.01));
            comboVoices[v].ampEnv.release(currTrack.amp_release * (currTrack.velocity * 0.01));

            // output
            comboVoices[v].mix.gain(0, 1); // raw sample
            comboVoices[v].mix.gain(1, 1); // synth

            // mono to L&R
            comboVoices[v].leftCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));
            comboVoices[v].rightCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));

            // Sub L&R mixers
            comboVoices[v].leftSubMix.gain(1, currTrack.level);  // raw sample / synth left
            comboVoices[v].leftSubMix.gain(0, currTrack.level);  // dexed left
            comboVoices[v].rightSubMix.gain(1, currTrack.level); // raw sample / synth right
            comboVoices[v].rightSubMix.gain(0, currTrack.level); // dexed right
        }

        // configure sample voice audio objects
        for (int v = 0; v < SAMPLE_VOICE_COUNT; v++)
        {
            // TODO: eventually need to restore all sounds for all patterns and their tracks?
            auto &currTrack = XRSequencer::getHeapTrack(v + 4); // offset by 4 since the 12 sample voices start at track 5

            // init mono RAW sample
            sampleVoices[v].rSample.setPlaybackRate(currTrack.sample_play_rate);
            sampleVoices[v].rSample.enableInterpolation(true);

            sampleVoices[v].ampEnv.attack(currTrack.amp_attack * (currTrack.velocity * 0.01));
            sampleVoices[v].ampEnv.decay(currTrack.amp_decay * (currTrack.velocity * 0.01));
            sampleVoices[v].ampEnv.sustain(currTrack.amp_sustain * (currTrack.velocity * 0.01));
            sampleVoices[v].ampEnv.release(currTrack.amp_release * (currTrack.velocity * 0.01));
            // sampleVoices[v].ampEnv.releaseNoteOn(15);

            // mono to L&R
            sampleVoices[v].leftCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));
            sampleVoices[v].rightCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));

            // Sub L&R mixers
            sampleVoices[v].leftSubMix.gain(0, currTrack.level);  // raw sample / synth left
            sampleVoices[v].rightSubMix.gain(0, currTrack.level); // raw sample / synth right
            sampleVoices[v].leftSubMix.gain(1, currTrack.level);  // dexed left
            sampleVoices[v].rightSubMix.gain(1, currTrack.level); // dexed right
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
    }

    void configureVoiceSettingsForTrack(int t)
    {
        auto &seqHeap = XRSequencer::getSequencerHeap();

        if (t > 3) { // sample-only voices
            // SampleVoice trackVoice = sampleVoices[t - 4];

            if (seqHeap.pattern.tracks[t].track_type == XRSequencer::TRACK_TYPE::WAV_SAMPLE)
            {
                // only create buffers for stereo samples when needed
                // trackVoice.wSample.createBuffer(2048, AudioBuffer::inExt);
            }

            return;
        }

        ComboVoice trackVoice = comboVoices[t];

        if (seqHeap.pattern.tracks[t].track_type == XRSequencer::TRACK_TYPE::RAW_SAMPLE)
        {
            XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::TRACK_TYPE::RAW_SAMPLE);

            // turn sample volume all the way up
            trackVoice.mix.gain(0, 1);
            // turn synth volume all the way down
            trackVoice.mix.gain(1, 0); // synth
        }
        else if (seqHeap.pattern.tracks[t].track_type == XRSequencer::TRACK_TYPE::DEXED)
        {
            XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::TRACK_TYPE::DEXED);

            trackVoice.mix.gain(0, 0); // mono sample
            trackVoice.mix.gain(1, 0); // synth

            trackVoice.leftSubMix.gain(0, 0);
            trackVoice.leftSubMix.gain(1, 1);
            trackVoice.rightSubMix.gain(0, 0);
            trackVoice.rightSubMix.gain(1, 1);
        }
        else if (seqHeap.pattern.tracks[t].track_type == XRSequencer::TRACK_TYPE::WAV_SAMPLE)
        {
            // XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::TRACK_TYPE::WAV_SAMPLE);

            // only create buffers for stereo samples when needed
            // trackVoice.wSample.createBuffer(2048, AudioBuffer::inExt);

            trackVoice.mix.gain(0, 0); // mono sample
            trackVoice.mix.gain(1, 0); // synth
        }
        else if (seqHeap.pattern.tracks[t].track_type == XRSequencer::TRACK_TYPE::SUBTRACTIVE_SYNTH)
        {
            XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::TRACK_TYPE::SUBTRACTIVE_SYNTH);

            auto currTrack = XRSequencer::getHeapTrack(t);

            // turn sample volume all the way down
            trackVoice.mix.gain(0, 0);
            // turn synth volumes all the way up
            trackVoice.mix.gain(1, 1); // ladder

            // TESTING: revert amp env to normal synth setting
            trackVoice.ampEnv.attack(currTrack.amp_attack);
            trackVoice.ampEnv.decay(currTrack.amp_decay);
            trackVoice.ampEnv.sustain(currTrack.amp_sustain);
            trackVoice.ampEnv.release(currTrack.amp_release);
        }
        else if (seqHeap.pattern.tracks[t].track_type == XRSequencer::TRACK_TYPE::MIDI_OUT)
        {
            XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::TRACK_TYPE::MIDI_OUT);

            // turn all audio for this track voice down
            trackVoice.mix.gain(0, 0); // mono sample
            trackVoice.mix.gain(1, 0); // synth
        }
        else if (seqHeap.pattern.tracks[t].track_type == XRSequencer::TRACK_TYPE::CV_GATE)
        {
            XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::TRACK_TYPE::CV_GATE);

            // turn all audio for this track voice down
            trackVoice.mix.gain(0, 0); // mono sample
            trackVoice.mix.gain(1, 0); // synth
        }
        else if (seqHeap.pattern.tracks[t].track_type == XRSequencer::TRACK_TYPE::CV_TRIG)
        {
            XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::TRACK_TYPE::CV_TRIG);

            // turn all audio for this track voice down
            trackVoice.mix.gain(0, 0); // mono sample
            trackVoice.mix.gain(1, 0); // synth
        }
    }

    void handleRawSampleNoteOnForTrack(int track)
    {
        auto &trackToUse = XRSequencer::getHeapTrack(track);

        if (track > 3) // sample-only voices
        {
            int tOffset = track - 4;
            AudioNoInterrupts();
            sampleVoices[tOffset].leftCtrl.gain(getStereoPanValues(trackToUse.pan).right * (trackToUse.velocity * 0.01));
            sampleVoices[tOffset].rightCtrl.gain(getStereoPanValues(trackToUse.pan).left * (trackToUse.velocity * 0.01));

            sampleVoices[tOffset].ampEnv.attack(trackToUse.amp_attack * (trackToUse.velocity * 0.01));
            sampleVoices[tOffset].ampEnv.decay(trackToUse.amp_decay * (trackToUse.velocity * 0.01));
            sampleVoices[tOffset].ampEnv.sustain(trackToUse.amp_sustain * (trackToUse.velocity * 0.01));
            sampleVoices[tOffset].ampEnv.release(trackToUse.amp_release * (trackToUse.velocity * 0.01));
            AudioInterrupts();

            sampleVoices[tOffset].ampEnv.noteOn();

            std::string trackSampleName(trackToUse.sample_name);

            // if sample has valid name, assume it is loaded in PSRAM and can be played
            if (trackSampleName.length() > 0) {
                sampleVoices[tOffset].rSample.playRaw(
                    _extPatternSamples[track]->sampledata, 
                    _extPatternSamples[track]->samplesize / 2, 
                    _numChannels
                );
            }

            // always re-initialize loop type
            sampleVoices[tOffset].rSample.setLoopType(loopTypeSelMap[trackToUse.looptype]);

            if (loopTypeSelMap[trackToUse.looptype] == looptype_none)
            {
                sampleVoices[tOffset].rSample.setPlayStart(play_start::play_start_sample);
                sampleVoices[tOffset].rSample.setLoopType(loop_type::looptype_none);
            }
            else if (loopTypeSelMap[trackToUse.looptype] == looptype_repeat)
            {
                float loopFinishToUse = trackToUse.loopfinish;

                // if (trackToUse.chromatic_enabled) {
                //   float foundBaseFreq = noteToFreqArr[trackToUse.note];
                //   float octaveFreq = foundBaseFreq * (pow(2, keyboardOctave));
                //   //float freq = 440.0 * powf(2.0, (12-69) / 12.0);
                //   uint32_t numSamples = 44100 / octaveFreq;

                //   loopFinishToUse = numSamples;
                // }

                sampleVoices[tOffset].rSample.setPlayStart(trackToUse.playstart == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
                sampleVoices[tOffset].rSample.setLoopStart(trackToUse.loopstart);
                sampleVoices[tOffset].rSample.setLoopFinish(loopFinishToUse);
            }
        }
        else // combo voices
        {
            AudioNoInterrupts();
            comboVoices[track].leftCtrl.gain(getStereoPanValues(trackToUse.pan).right * (trackToUse.velocity * 0.01));
            comboVoices[track].rightCtrl.gain(getStereoPanValues(trackToUse.pan).left * (trackToUse.velocity * 0.01));

            comboVoices[track].ampEnv.attack(trackToUse.amp_attack * (trackToUse.velocity * 0.01));
            comboVoices[track].ampEnv.decay(trackToUse.amp_decay * (trackToUse.velocity * 0.01));
            comboVoices[track].ampEnv.sustain(trackToUse.amp_sustain * (trackToUse.velocity * 0.01));
            comboVoices[track].ampEnv.release(trackToUse.amp_release * (trackToUse.velocity * 0.01));
            AudioInterrupts();

            comboVoices[track].ampEnv.noteOn();

            std::string trackSampleName(trackToUse.sample_name);

            // if sample has valid name, assume it is loaded in PSRAM and can be played
            if (trackSampleName.length() > 0) {
                comboVoices[track].rSample.playRaw(
                    _extPatternSamples[track]->sampledata, 
                    _extPatternSamples[track]->samplesize / 2, 
                    _numChannels
                );
            }

            // always re-initialize loop type
            comboVoices[track].rSample.setLoopType(loopTypeSelMap[trackToUse.looptype]);

            if (loopTypeSelMap[trackToUse.looptype] == looptype_none)
            {
                comboVoices[track].rSample.setPlayStart(play_start::play_start_sample);
                comboVoices[track].rSample.setLoopType(loop_type::looptype_none);
            }
            else if (loopTypeSelMap[trackToUse.looptype] == looptype_repeat)
            {
                float loopFinishToUse = trackToUse.loopfinish;

                // if (trackToUse.chromatic_enabled) {
                //   float foundBaseFreq = noteToFreqArr[trackToUse.note];
                //   float octaveFreq = foundBaseFreq * (pow(2, keyboardOctave));
                //   //float freq = 440.0 * powf(2.0, (12-69) / 12.0);
                //   uint32_t numSamples = 44100 / octaveFreq;

                //   loopFinishToUse = numSamples;
                // }

                comboVoices[track].rSample.setPlayStart(trackToUse.playstart == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
                comboVoices[track].rSample.setLoopStart(trackToUse.loopstart);
                comboVoices[track].rSample.setLoopFinish(loopFinishToUse);
            }
        }
    }

    void handleWavSampleNoteOnForTrack(int track)
    {
        // auto &trackToUse = XRSequencer::getHeapTrack(track);

        if (track > 3)
        {
            int tOffset = track - 4;

            sampleVoices[tOffset].ampEnv.noteOn();
            // sampleVoices[tOffset].wSample.play(usableWavSampleNames[trackToUse.wav_sample_id], sd1);
        }
        else
        {
            comboVoices[track].ampEnv.noteOn();
            // comboVoices[track].wSample.play(usableWavSampleNames[trackToUse.wav_sample_id], sd1);
        }
    }

    void handleDexedNoteOnForTrack(int track)
    {
        auto &trackToUse = XRSequencer::getHeapTrack(track);

        uint8_t noteToUse = trackToUse.note;
        uint8_t octaveToUse = trackToUse.octave;

        int midiNote = (noteToUse + (12 * (octaveToUse))); // use offset of 32 instead?

        if (track < 4)
        {
            comboVoices[track].dexed.keydown(midiNote, 50);
        }
    }

    void handleSubtractiveSynthNoteOnForTrack(int track)
    {
        auto &trackToUse = XRSequencer::getHeapTrack(track);
        
        AudioNoInterrupts();
        float foundBaseFreq = _noteToFreqArr[trackToUse.note];
        float octaveFreqA = (foundBaseFreq + (trackToUse.fine * 0.01)) * (pow(2, trackToUse.octave));
        float octaveFreqB = (foundBaseFreq * pow(2.0, (float)trackToUse.detune / 12.0)) * (pow(2, trackToUse.octave));

        comboVoices[track].osca.frequency(octaveFreqA);
        comboVoices[track].oscb.frequency(octaveFreqB);

        comboVoices[track].leftCtrl.gain(getStereoPanValues(trackToUse.pan).right * (trackToUse.velocity * 0.01));
        comboVoices[track].rightCtrl.gain(getStereoPanValues(trackToUse.pan).left * (trackToUse.velocity * 0.01));

        comboVoices[track].ampEnv.attack(trackToUse.amp_attack * (trackToUse.velocity * 0.01));
        comboVoices[track].ampEnv.decay(trackToUse.amp_decay * (trackToUse.velocity * 0.01));
        comboVoices[track].ampEnv.sustain(trackToUse.amp_sustain * (trackToUse.velocity * 0.01));
        comboVoices[track].ampEnv.release(trackToUse.amp_release * (trackToUse.velocity * 0.01));
        comboVoices[track].filterEnv.attack(trackToUse.filter_attack);
        comboVoices[track].filterEnv.decay(trackToUse.filter_decay);
        comboVoices[track].filterEnv.sustain(trackToUse.filter_sustain);
        comboVoices[track].filterEnv.release(trackToUse.filter_release);
        AudioInterrupts();

        // now triggers envs
        comboVoices[track].ampEnv.noteOn();
        comboVoices[track].filterEnv.noteOn();
    }

    void handleMIDINoteOnForTrack(int track)
    {
        // TODO: impl
    }

    void handleCvGateNoteOnForTrack(int track)
    {
        // TODO: impl
    }

    void handleRawSampleNoteOnForTrackStep(int track, int step)
    {
        //Serial.println("enter handleRawSampleNoteOnForTrackStep!");

        auto &trackToUse = XRSequencer::getHeapTrack(track);
        auto &stepToUse = XRSequencer::getHeapStep(track, step);
        auto &patternMods = XRSequencer::getModsForCurrentPattern();

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

        uint8_t velocityToUse = trackToUse.velocity;
        if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::VELOCITY])
        {
            velocityToUse = patternMods.tracks[track].steps[step].velocity;
        }
        else
        {
            velocityToUse = stepToUse.velocity;
        }

        uint8_t looptypeToUse = trackToUse.looptype;
        if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::LOOPTYPE])
        {
            looptypeToUse = patternMods.tracks[track].steps[step].looptype;
        }

        uint32_t loopstartToUse = trackToUse.loopstart;
        if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::LOOPSTART])
        {
            loopstartToUse = patternMods.tracks[track].steps[step].loopstart;
        }

        uint32_t loopfinishToUse = trackToUse.loopfinish;
        if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::LOOPFINISH])
        {
            loopfinishToUse = patternMods.tracks[track].steps[step].loopfinish;
        }

        uint8_t playstartToUse = trackToUse.playstart;
        if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::PLAYSTART])
        {
            playstartToUse = patternMods.tracks[track].steps[step].playstart;
        }

        float speedToUse = trackToUse.sample_play_rate;
        if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::SAMPLE_PLAY_RATE])
        {
            speedToUse = patternMods.tracks[track].steps[step].sample_play_rate;
        }

        if (track > 3)
        {
            int tOffset = track - 4;
        AudioNoInterrupts();
            sampleVoices[tOffset].leftCtrl.gain(getStereoPanValues(trackToUse.pan).right * (velocityToUse * 0.01));
            sampleVoices[tOffset].rightCtrl.gain(getStereoPanValues(trackToUse.pan).left * (velocityToUse * 0.01));

            sampleVoices[tOffset].ampEnv.attack(trackToUse.amp_attack * (velocityToUse * 0.01));
            sampleVoices[tOffset].ampEnv.decay(trackToUse.amp_decay * (velocityToUse * 0.01));
            sampleVoices[tOffset].ampEnv.sustain(trackToUse.amp_sustain * (velocityToUse * 0.01));
            sampleVoices[tOffset].ampEnv.release(trackToUse.amp_release * (velocityToUse * 0.01));

            sampleVoices[tOffset].rSample.setPlaybackRate(speedToUse);
        AudioInterrupts();

            sampleVoices[tOffset].ampEnv.noteOn();

            std::string trackSampleName(trackToUse.sample_name);

            // if sample has valid name, assume it is loaded in PSRAM and can be played
            if (trackSampleName.length() > 0) {
                sampleVoices[tOffset].rSample.playRaw(
                    _extPatternSamples[track]->sampledata, 
                    _extPatternSamples[track]->samplesize / 2, 
                    _numChannels
                );
            }

            // always re-initialize loop type
            sampleVoices[tOffset].rSample.setLoopType(loopTypeSelMap[looptypeToUse]);

            if (loopTypeSelMap[looptypeToUse] == looptype_none)
            {
                sampleVoices[tOffset].rSample.setPlayStart(play_start::play_start_sample);
                sampleVoices[tOffset].rSample.setLoopType(loop_type::looptype_none);
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

                sampleVoices[tOffset].rSample.setPlayStart(playstartToUse == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
                sampleVoices[tOffset].rSample.setLoopStart(loopstartToUse);
                sampleVoices[tOffset].rSample.setLoopFinish(loopfinishToUse);
            }
        }  else {

        AudioNoInterrupts();
            comboVoices[track].leftCtrl.gain(getStereoPanValues(trackToUse.pan).right * (velocityToUse * 0.01));
            comboVoices[track].rightCtrl.gain(getStereoPanValues(trackToUse.pan).left * (velocityToUse * 0.01));

            comboVoices[track].ampEnv.attack(trackToUse.amp_attack * (velocityToUse * 0.01));
            comboVoices[track].ampEnv.decay(trackToUse.amp_decay * (velocityToUse * 0.01));
            comboVoices[track].ampEnv.sustain(trackToUse.amp_sustain * (velocityToUse * 0.01));
            comboVoices[track].ampEnv.release(trackToUse.amp_release * (velocityToUse * 0.01));

            comboVoices[track].rSample.setPlaybackRate(speedToUse);
        AudioInterrupts();

            comboVoices[track].ampEnv.noteOn();

            std::string trackSampleName(trackToUse.sample_name);

            // if sample has valid name, assume it is loaded in PSRAM and can be played
            if (trackSampleName.length() > 0) {
                comboVoices[track].rSample.playRaw(
                    _extPatternSamples[track]->sampledata, 
                    _extPatternSamples[track]->samplesize / 2, 
                    _numChannels
                );
            }

            // always re-initialize loop type
            comboVoices[track].rSample.setLoopType(loopTypeSelMap[looptypeToUse]);

            if (loopTypeSelMap[looptypeToUse] == looptype_none)  {
                comboVoices[track].rSample.setPlayStart(play_start::play_start_sample);
                comboVoices[track].rSample.setLoopType(loop_type::looptype_none);
            } else if (loopTypeSelMap[looptypeToUse] == looptype_repeat) {

                // if (trackToUse.chromatic_enabled) {
                //   float foundBaseFreq = noteToFreqArr[noteToUse];
                //   float octaveFreq = foundBaseFreq * (pow(2, keyboardOctave));
                //   //float freq = 440.0 * powf(2.0, (12-69) / 12.0);
                //   uint32_t numSamples = 44100 / octaveFreq;

                //   loopFinishToUse = numSamples;
                // }

                comboVoices[track].rSample.setPlayStart(playstartToUse == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
                comboVoices[track].rSample.setLoopStart(loopstartToUse);
                comboVoices[track].rSample.setLoopFinish(loopfinishToUse);
            }
        }
    }

    void handleWavSampleNoteOnForTrackStep(int track, int step)
    {
        // auto &trackToUse = XRSequencer::getHeapTrack(track);

        if (track > 3)
        {
            int tOffset = track - 4;

            sampleVoices[tOffset].ampEnv.noteOn();
            // sampleVoices[tOffset].wSample.play(usableWavSampleNames[trackToUse.wav_sample_id], sd1);
        }
        else
        {
            comboVoices[track].ampEnv.noteOn();
            // comboVoices[track].wSample.play(usableWavSampleNames[trackToUse.wav_sample_id], sd1);
        }
    }

    void handleDexedNoteOnForTrackStep(int track, int step)
    {
        // auto &trackToUse = XRSequencer::getHeapTrack(track);
        auto &stepToUse = XRSequencer::getHeapStep(track, step);
        auto &patternMods = XRSequencer::getModsForCurrentPattern();

        uint8_t noteToUse = stepToUse.note;
        if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::NOTE])
        {
            noteToUse = patternMods.tracks[track].steps[step].note;
            // Serial.println(noteToUse);
        }

        uint8_t octaveToUse = stepToUse.octave;
        if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::OCTAVE])
        {
            octaveToUse = patternMods.tracks[track].steps[step].octave;
            // Serial.println(octaveToUse);
        }

        // if (track < 4)
        // {
            int midiNote = (noteToUse + (12 * (octaveToUse)));

            comboVoices[track].dexed.keydown(midiNote, stepToUse.velocity);
        // }
    }

    void handleSubtractiveSynthNoteOnForTrackStep(int track, int step)
    {
        auto &trackToUse = XRSequencer::getHeapTrack(track);
        auto &stepToUse = XRSequencer::getHeapStep(track, step);
        auto &patternMods = XRSequencer::getModsForCurrentPattern();

        uint8_t noteToUse = stepToUse.note;
        if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::NOTE])
        {
            noteToUse = patternMods.tracks[track].steps[step].note;
            // Serial.println(noteToUse);
        }

        uint8_t octaveToUse = stepToUse.octave;
        if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::OCTAVE])
        {
            octaveToUse = patternMods.tracks[track].steps[step].octave;
            // Serial.println(octaveToUse);
        }

        uint8_t velocityToUse = trackToUse.velocity;
        if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::VELOCITY])
        {
            velocityToUse = patternMods.tracks[track].steps[step].velocity;
        }
        else
        {
            velocityToUse = stepToUse.velocity;
        }

        uint8_t waveformToUse = trackToUse.waveform;
        if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::WAVEFORM])
        {
            waveformToUse = patternMods.tracks[track].steps[step].waveform;
        }

    AudioNoInterrupts();

        float foundBaseFreq = _noteToFreqArr[noteToUse];
        float octaveFreqA = (foundBaseFreq + (trackToUse.fine * 0.01)) * (pow(2, octaveToUse));
        float octaveFreqB = (foundBaseFreq * pow(2.0, (float)trackToUse.detune / 12.0)) * (pow(2, octaveToUse));

        comboVoices[track].osca.begin(waveformToUse);
        comboVoices[track].oscb.begin(waveformToUse);

        comboVoices[track].osca.frequency(octaveFreqA);
        comboVoices[track].oscb.frequency(octaveFreqB);

        comboVoices[track].leftCtrl.gain(getStereoPanValues(trackToUse.pan).right * (velocityToUse * 0.01));
        comboVoices[track].rightCtrl.gain(getStereoPanValues(trackToUse.pan).left * (velocityToUse * 0.01));

        comboVoices[track].ampEnv.attack(trackToUse.amp_attack * (velocityToUse * 0.01));
        comboVoices[track].ampEnv.decay(trackToUse.amp_decay * (velocityToUse * 0.01));
        comboVoices[track].ampEnv.sustain(trackToUse.amp_sustain * (velocityToUse * 0.01));
        comboVoices[track].ampEnv.release(trackToUse.amp_release * (velocityToUse * 0.01));
        comboVoices[track].filterEnv.attack(trackToUse.filter_attack);
        comboVoices[track].filterEnv.decay(trackToUse.filter_decay);
        comboVoices[track].filterEnv.sustain(trackToUse.filter_sustain);
        comboVoices[track].filterEnv.release(trackToUse.filter_release);

    AudioInterrupts();

        // now trigger envs
        comboVoices[track].ampEnv.noteOn();
        comboVoices[track].filterEnv.noteOn();
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
        auto &stepToUse = XRSequencer::getHeapStep(track, step);
        auto &patternMods = XRSequencer::getModsForCurrentPattern();

        uint8_t noteToUse = stepToUse.note;
        uint8_t octaveToUse = stepToUse.octave;
        if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::NOTE])
        {
            noteToUse = patternMods.tracks[track].steps[step].note;
            // Serial.println(noteToUse);
        }

        if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::OCTAVE])
        {
            octaveToUse = patternMods.tracks[track].steps[step].octave;
            // Serial.println(noteToUse);
        }

        int midiNote = (noteToUse + (12 * (octaveToUse)));

        Serial.print("midiNote: ");
        Serial.print(midiNote);
        Serial.print(" cvLevels[midiNote]: ");
        Serial.println(_cvLevels[midiNote]);

        if (trackToUse.channel == 1)
        {
            XRCV::write(CS1, 0, _cvLevels[midiNote]); // cv
            XRCV::write(CS1, 1, 4095);               // gate
        }
        else if (trackToUse.channel == 2)
        {
            XRCV::write(CS2, 0, _cvLevels[midiNote]); // cv
            XRCV::write(CS2, 1, 4095);               // gate
        }
        else if (trackToUse.channel == 3)
        {
            XRCV::write(CS3, 0, _cvLevels[midiNote]); // cv
            XRCV::write(CS3, 1, 4095);               // gate
        }
        else if (trackToUse.channel == 4)
        {
            XRCV::write(CS4, 0, _cvLevels[midiNote]); // cv
            XRCV::write(CS4, 1, 4095);               // gate
        }
    }

    void handleNoteOffForTrack(int track)
    {
        auto &currTrack = XRSequencer::getHeapTrack(track);

        if (currTrack.track_type == XRSequencer::TRACK_TYPE::SUBTRACTIVE_SYNTH)
        {
            comboVoices[track].ampEnv.noteOff();
            comboVoices[track].filterEnv.noteOff();
        }

        else if (currTrack.track_type == XRSequencer::TRACK_TYPE::DEXED)
        {
            uint8_t noteToUse = currTrack.note;
            uint8_t octaveToUse = currTrack.octave;

            int midiNote = (noteToUse + (12 * (octaveToUse))); // use offset of 32 instead?

            if (track < 4)
            {
                comboVoices[track].dexed.keyup(midiNote);
            }
        }
        else if (currTrack.track_type == XRSequencer::TRACK_TYPE::MIDI_OUT)
        {
            XRMIDI::sendNoteOff(64, 100, 1);
        }
        else if (currTrack.track_type == XRSequencer::TRACK_TYPE::CV_GATE)
        {
            if (currTrack.channel == 1)
            {
                // writeToDAC(CS1, 0, cvLevels[midiNote]); // cv
                XRCV::write(CS1, 1, 0); // gate
            }
            else if (currTrack.channel == 2)
            {
                // writeToDAC(CS2, 0, cvLevels[midiNote]); // cv
                XRCV::write(CS2, 1, 0); // gate
            }
            else if (currTrack.channel == 3)
            {
                // writeToDAC(CS3, 0, cvLevels[midiNote]); // cv
                XRCV::write(CS3, 1, 0); // gate
            }
            else if (currTrack.channel == 4)
            {
                // writeToDAC(CS4, 0, cvLevels[midiNote]); // cv
                XRCV::write(CS4, 1, 0); // gate
            }
        } 
        else
        {
            if (track > 3) // sample-only voices
            {
                int tOffset = track - 4;
                sampleVoices[tOffset].ampEnv.noteOff();
            }
            else
            {
                comboVoices[track].ampEnv.noteOff();
            }
        }
    }

    void handleNoteOffForTrackStep(int track, int step)
    {
        //Serial.println("enter handleNoteOffForTrackStep!");

        auto &currTrack = XRSequencer::getHeapTrack(track);
        auto &currTrackStep = XRSequencer::getHeapStep(track, step);
        auto &patternMods = XRSequencer::getModsForCurrentPattern();

        if (currTrack.track_type == XRSequencer::TRACK_TYPE::SUBTRACTIVE_SYNTH)
        {
            comboVoices[track].ampEnv.noteOff();
            comboVoices[track].filterEnv.noteOff();
        }

        else if (currTrack.track_type == XRSequencer::TRACK_TYPE::DEXED)
        {
            // comboVoices[track].dexed.notesOff();

            uint8_t noteToUse = currTrackStep.note;
            if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::NOTE])
            {
                noteToUse = patternMods.tracks[track].steps[step].note;
                // Serial.println(noteToUse);
            }

            uint8_t octaveToUse = currTrackStep.octave;
            if (patternMods.tracks[track].step_mod_flags[step].flags[XRSequencer::MOD_ATTRS::OCTAVE])
            {
                octaveToUse = patternMods.tracks[track].steps[step].octave;
                // Serial.println(octaveToUse);
            }

            int midiNote = (noteToUse + (12 * (octaveToUse)));

            comboVoices[track].dexed.keyup(midiNote);
        }

        // fix
        else if (currTrack.track_type == XRSequencer::TRACK_TYPE::MIDI_OUT)
        {
            XRMIDI::sendNoteOff(64, 100, 1);
        }
        else if (currTrack.track_type == XRSequencer::TRACK_TYPE::CV_GATE)
        {
            if (currTrack.channel == 1)
            {
                // writeToDAC(CS1, 0, cvLevels[midiNote]); // cv
                XRCV::write(CS1, 1, 0); // gate
            }
            else if (currTrack.channel == 2)
            {
                // writeToDAC(CS2, 0, cvLevels[midiNote]); // cv
                XRCV::write(CS2, 1, 0); // gate
            }
            else if (currTrack.channel == 3)
            {
                // writeToDAC(CS3, 0, cvLevels[midiNote]); // cv
                XRCV::write(CS3, 1, 0); // gate
            }
            else if (currTrack.channel == 4)
            {
                // writeToDAC(CS4, 0, cvLevels[midiNote]); // cv
                XRCV::write(CS4, 1, 0); // gate
            }
        }

        else
        {
            if (track > 3)
            {
                int tOffset = track - 4;
                sampleVoices[tOffset].ampEnv.noteOff();
            }
            else
            {
                comboVoices[track].ampEnv.noteOff();
            }
        }
    }

    void assignSampleToTrack()
    {
        // TODO: impl async loading so there's no audible SPI noise from reading the SD card
        // also, impl freeing any existing sample from the track and loading the new sample (if changing sample)

        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();

        std::string sampleName = "/audio enjoyer/xr-1/samples/";
        std::string selected = XRSD::getCurrSampleFileHighlighted();
        sampleName += selected;

        XRSequencer::assignSampleNameToTrack(selected);

        _extPatternSamples[currTrackNum] = _loader.loadSample(sampleName.c_str());
    }

    void clearSamples()
    {
        _loader.clearSamples();
    }

    void changeSampleTrackSoundType(uint8_t t, int8_t newType)
    {
        auto &currTrack = XRSequencer::getHeapTrack(t);
        auto currType = currTrack.track_type;

        if (currType == newType)
            return;

        if (currType == XRSequencer::WAV_SAMPLE)
        {
            // sampleVoices[t-4].wSample.disposeBuffer();
        }

        if (newType == XRSequencer::RAW_SAMPLE)
        {
            XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::RAW_SAMPLE);
        }
        else if (newType == XRSequencer::DEXED)
        {
            XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::DEXED);
        }
        else if (newType == XRSequencer::WAV_SAMPLE)
        {
            // only create buffers for stereo samples when needed
            // sampleVoices[t-4].wSample.createBuffer(2048, AudioBuffer::inExt);

            XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::WAV_SAMPLE);
        }
        else if (newType == XRSequencer::MIDI_OUT)
        {
            XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::MIDI_OUT);
        }
        else if (newType == XRSequencer::CV_GATE)
        {
            XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::CV_GATE);
        }
        else if (newType == XRSequencer::CV_TRIG)
        {
            XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::CV_TRIG);
        }
    }

    void changeTrackSoundType(int8_t t, int8_t newType)
    {
        if (t > 3)
        {
            changeSampleTrackSoundType(t, newType);
            return;
        }

        ComboVoice trackVoice = comboVoices[t];
        auto &currTrack = XRSequencer::getHeapTrack(t);
        auto currType = currTrack.track_type;

        if (currType == newType)
            return;

        // if (currType == XRSequencer::WAV_SAMPLE)
        // {
        //     // trackVoice.wSample.disposeBuffer();
        // }

        if (newType == XRSequencer::RAW_SAMPLE)
        {
            XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::RAW_SAMPLE);

            // turn sample volume all the way up
            trackVoice.mix.gain(0, 1);
            // turn synth volume all the way down
            trackVoice.mix.gain(1, 0); // synth
        }
        else if (newType == XRSequencer::DEXED)
        {
            XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::DEXED);

            trackVoice.mix.gain(0, 0); // mono sample
            trackVoice.mix.gain(1, 0); // synth

            trackVoice.leftSubMix.gain(0, 1);
            trackVoice.leftSubMix.gain(1, 1);
            trackVoice.rightSubMix.gain(0, 1);
            trackVoice.rightSubMix.gain(1, 1);
        }
        else if (newType == XRSequencer::WAV_SAMPLE)
        {
            XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::WAV_SAMPLE);

            // only create buffers for stereo samples when needed
            // trackVoice.wSample.createBuffer(2048, AudioBuffer::inExt);

            // turn sample volume all the way up
            trackVoice.mix.gain(0, 1);
            // turn synth volumes all the way down
            trackVoice.mix.gain(1, 0); // synth
        }
        else if (newType == XRSequencer::SUBTRACTIVE_SYNTH)
        {
            XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::SUBTRACTIVE_SYNTH);

            auto &currTrack = XRSequencer::getHeapTrack(t);

            // turn sample volume all the way down
            //trackVoice.mix.gain(0, 0);
            trackVoice.mix.gain(0, 0); // TODO: CHANGE
            // turn synth volumes all the way up
            trackVoice.mix.gain(1, 1); // ladder

            // TESTING: revert amp env to normal synth setting
            trackVoice.ampEnv.attack(currTrack.amp_attack);
            trackVoice.ampEnv.decay(currTrack.amp_decay);
            trackVoice.ampEnv.sustain(currTrack.amp_sustain);
            trackVoice.ampEnv.release(currTrack.amp_release);
        }
        else if (newType == XRSequencer::MIDI_OUT)
        {
            XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::MIDI_OUT);

            // turn all audio for this track voice down
            trackVoice.mix.gain(0, 0); // mono sample
            trackVoice.mix.gain(1, 0); // ladder
        }
        else if (newType == XRSequencer::CV_GATE)
        {
            XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::CV_GATE);

            // turn all audio for this track voice down
            trackVoice.mix.gain(0, 0); // mono sample
            trackVoice.mix.gain(1, 0); // ladder
        }
        else if (newType == XRSequencer::CV_TRIG)
        {
            XRSequencer::setTrackTypeForHeapTrack(t, XRSequencer::CV_TRIG);

            // turn all audio for this track voice down
            trackVoice.mix.gain(0, 0); // mono sample
            trackVoice.mix.gain(1, 0); // ladder
        }
    }

    void triggerTrackManually(uint8_t t, uint8_t note)
    {
        auto &track = XRSequencer::getHeapTrack(t);

        if (track.track_type == XRSequencer::RAW_SAMPLE)
        {
            triggerRawSampleNoteOn(t, note);
        }
        else if (track.track_type == XRSequencer::WAV_SAMPLE)
        {
            Serial.println("TODO: impl wav sample manual trigger");
            // triggerWavSampleNoteOn(t, note);
        }
        else if (track.track_type == XRSequencer::DEXED)
        {
            triggerDexedNoteOn(t, note);
        }
        else if (track.track_type == XRSequencer::SUBTRACTIVE_SYNTH)
        {
            Serial.printf("trigger synth track %d manually\n", t);
            
            triggerSubtractiveSynthNoteOn(t, note);
        }
        else if (track.track_type == XRSequencer::CV_GATE)
        {
            triggerCvGateNoteOn(t, note);
        }
    }

    void triggerRawSampleNoteOn(uint8_t t, uint8_t note)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedPattern().tracks[t];
        int tOffset = t - 4;

        if (t < 4)
        {
            AudioNoInterrupts();
            comboVoices[t].leftCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));
            comboVoices[t].rightCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));

            comboVoices[t].ampEnv.attack(currTrack.amp_attack * (currTrack.velocity * 0.01));
            comboVoices[t].ampEnv.decay(currTrack.amp_decay * (currTrack.velocity * 0.01));
            comboVoices[t].ampEnv.sustain(currTrack.amp_sustain * (currTrack.velocity * 0.01));
            comboVoices[t].ampEnv.release(currTrack.amp_release * (currTrack.velocity * 0.01));
            AudioInterrupts();

            comboVoices[t].ampEnv.noteOn();
            comboVoices[t].rSample.playRaw(_extPatternSamples[t]->sampledata, _extPatternSamples[t]->samplesize / 2, _numChannels);

            // always re-initialize loop type
            comboVoices[t].rSample.setLoopType(loopTypeSelMap[currTrack.looptype]);

            if (loopTypeSelMap[currTrack.looptype] == looptype_none)
            {
                comboVoices[t].rSample.setPlayStart(play_start::play_start_sample);
                comboVoices[t].rSample.setLoopType(loop_type::looptype_none);
            }
            else if (loopTypeSelMap[currTrack.looptype] == looptype_repeat)
            {
                float loopFinishToUse = currTrack.loopfinish;

                if (currTrack.chromatic_enabled)
                {
                    float foundBaseFreq = _noteToFreqArr[note];
                    float octaveFreq = foundBaseFreq * (pow(2, XRKeyMatrix::getKeyboardOctave()));
                    // float freq = 440.0 * powf(2.0, (12-69) / 12.0);
                    uint32_t numSamples = 44100 / octaveFreq;

                    loopFinishToUse = numSamples;
                }

                comboVoices[t].rSample.setPlayStart(currTrack.playstart == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
                comboVoices[t].rSample.setLoopStart(currTrack.loopstart);
                comboVoices[t].rSample.setLoopFinish(loopFinishToUse);
            }
        }
        else
        {
            AudioNoInterrupts();
            sampleVoices[tOffset].leftCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));
            sampleVoices[tOffset].rightCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));

            sampleVoices[tOffset].ampEnv.attack(currTrack.amp_attack * (currTrack.velocity * 0.01));
            sampleVoices[tOffset].ampEnv.decay(currTrack.amp_decay * (currTrack.velocity * 0.01));
            sampleVoices[tOffset].ampEnv.sustain(currTrack.amp_sustain * (currTrack.velocity * 0.01));
            sampleVoices[tOffset].ampEnv.release(currTrack.amp_release * (currTrack.velocity * 0.01));
            AudioInterrupts();

            sampleVoices[tOffset].ampEnv.noteOn();
            sampleVoices[tOffset].rSample.playRaw(_extPatternSamples[t]->sampledata, _extPatternSamples[t]->samplesize / 2, _numChannels);
            
            // always re-initialize loop type
            sampleVoices[tOffset].rSample.setLoopType(loopTypeSelMap[currTrack.looptype]);

            if (loopTypeSelMap[currTrack.looptype] == looptype_none)
            {
                sampleVoices[tOffset].rSample.setPlayStart(play_start::play_start_sample);
                sampleVoices[tOffset].rSample.setLoopType(loop_type::looptype_none);
            }
            else if (loopTypeSelMap[currTrack.looptype] == looptype_repeat)
            {
                float loopFinishToUse = currTrack.loopfinish;

                if (currTrack.chromatic_enabled)
                {
                    float foundBaseFreq = _noteToFreqArr[note];
                    float octaveFreq = foundBaseFreq * (pow(2, XRKeyMatrix::getKeyboardOctave()));
                    // float freq = 440.0 * powf(2.0, (12-69) / 12.0);
                    uint32_t numSamples = 44100 / octaveFreq;

                    loopFinishToUse = numSamples;
                }

                sampleVoices[tOffset].rSample.setPlayStart(currTrack.playstart == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
                sampleVoices[tOffset].rSample.setLoopStart(currTrack.loopstart);
                sampleVoices[tOffset].rSample.setLoopFinish(loopFinishToUse);
            }
        }
    }

    void triggerDexedNoteOn(uint8_t t, uint8_t note)
    {
        // auto &currTrack = XRSequencer::getHeapCurrentSelectedPattern().tracks[t];

        // if (t < 4)
        // {
            int midiNote = (note + (12 * (XRKeyMatrix::getKeyboardOctave())));

            comboVoices[t].dexed.keydown(midiNote, 50);
        // }
    }

    void triggerSubtractiveSynthNoteOn(uint8_t t, uint8_t note)
    {
        auto &currTrack = XRSequencer::getHeapTrack(t);

    AudioNoInterrupts();
        float foundBaseFreq = _noteToFreqArr[note];
        float octaveFreqA = (foundBaseFreq + (currTrack.fine * 0.01)) * (pow(2, XRKeyMatrix::getKeyboardOctave()));
        float octaveFreqB = (foundBaseFreq * pow(2.0, (float)currTrack.detune / 12.0)) * (pow(2, XRKeyMatrix::getKeyboardOctave()));

        // Serial.printf("note: %d, foundBaseFreq: %f, octaveFreqA: %f, octaveFreqB: %f\n", note, foundBaseFreq, octaveFreqA, octaveFreqB);

        comboVoices[t].osca.frequency(octaveFreqA);
        comboVoices[t].oscb.frequency(octaveFreqB);

        comboVoices[t].leftCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));
        comboVoices[t].rightCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));

        comboVoices[t].ampEnv.attack(currTrack.amp_attack * (currTrack.velocity * 0.01));
        comboVoices[t].ampEnv.decay(currTrack.amp_decay * (currTrack.velocity * 0.01));
        comboVoices[t].ampEnv.sustain(currTrack.amp_sustain * (currTrack.velocity * 0.01));
        comboVoices[t].ampEnv.release(currTrack.amp_release * (currTrack.velocity * 0.01));

        // float a1 = currTrack.amp_attack * (currTrack.velocity * 0.01);
        // float d1 = currTrack.amp_decay * (currTrack.velocity * 0.01);
        // float s1 = currTrack.amp_sustain * (currTrack.velocity * 0.01);
        // float r1 = currTrack.amp_release * (currTrack.velocity * 0.01);
        
        // Serial.printf("a: %f, d: %f, s: %f, r: %f\n", a1, d1, s1, r1);

        comboVoices[t].filterEnv.attack(currTrack.filter_attack);
        comboVoices[t].filterEnv.decay(currTrack.filter_decay);
        comboVoices[t].filterEnv.sustain(currTrack.filter_sustain);
        comboVoices[t].filterEnv.release(currTrack.filter_release);
    AudioInterrupts();

        // now trigger envs
        comboVoices[t].ampEnv.noteOn();
        comboVoices[t].filterEnv.noteOn();
    }

    void triggerCvGateNoteOn(uint8_t t, uint8_t note)
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedPattern().tracks[t];

        for (int i = 0; i < 128; i++)
        {
            _cvLevels[i] = i * 26;
        }

        uint8_t noteToUse = note;
        uint8_t octaveToUse = XRKeyMatrix::getKeyboardOctave(); // +1 ?

        int midiNote = (noteToUse + (12 * (octaveToUse))); // C0 = 12

        if (currTrack.channel == 1)
        {
            XRCV::write(CS1, 0, _cvLevels[midiNote]); // cv
            XRCV::write(CS1, 1, 4095);               // gate
        }
        else if (currTrack.channel == 2)
        {
            XRCV::write(CS2, 0, _cvLevels[midiNote]); // cv
            XRCV::write(CS2, 1, 4095);               // gate
        }
        else if (currTrack.channel == 3)
        {
            XRCV::write(CS3, 0, _cvLevels[midiNote]); // cv
            XRCV::write(CS3, 1, 4095);               // gate
        }
        else if (currTrack.channel == 4)
        {
            XRCV::write(CS4, 0, _cvLevels[midiNote]); // cv
            XRCV::write(CS4, 1, 4095);               // gate
        }
    }

    void noteOffTrackManually(int noteOnKeyboard)
    {
        auto &currSelTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto currSelTrackNum = XRSequencer::getCurrentSelectedTrackNum();

        if (currSelTrackNum > 3) {
            sampleVoices[currSelTrackNum - 4].ampEnv.noteOff();
        } else {
            comboVoices[currSelTrackNum].ampEnv.noteOff();

            if (currSelTrack.track_type == XRSequencer::TRACK_TYPE::SUBTRACTIVE_SYNTH)
            {
                comboVoices[currSelTrackNum].filterEnv.noteOff();
            }
            else if (currSelTrack.track_type == XRSequencer::TRACK_TYPE::DEXED)
            {
                int midiNote = (noteOnKeyboard + (12 * (XRKeyMatrix::getKeyboardOctave())));

                comboVoices[currSelTrackNum].dexed.keyup(midiNote);
            }
        }

        if (currSelTrack.track_type == XRSequencer::MIDI_OUT) {
            XRMIDI::sendNoteOff(64, 100, 1);
        } else if (currSelTrack.track_type == XRSequencer::CV_GATE) {
            if (currSelTrack.channel == 1)
            {
                // writeToDAC(CS1, 0, cvLevels[midiNote]); // cv
                XRCV::write(CS1, 1, 0); // gate
            }
            else if (currSelTrack.channel == 2)
            {
                // writeToDAC(CS2, 0, cvLevels[midiNote]); // cv
                XRCV::write(CS2, 1, 0); // gate
            }
            else if (currSelTrack.channel == 3)
            {
                // writeToDAC(CS3, 0, cvLevels[midiNote]); // cv
                XRCV::write(CS3, 1, 0); // gate
            }
            else if (currSelTrack.channel == 4)
            {
                // writeToDAC(CS4, 0, cvLevels[midiNote]); // cv
                XRCV::write(CS4, 1, 0); // gate
            }
        }
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
}