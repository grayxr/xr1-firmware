#include <XRSound.h>
#include <XRClock.h>
#include <XRSequencer.h>
#include <XRUX.h>
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

    std::map<int, loop_type> loopTypeSelMap = {
        {0, looptype_none},
        {1, looptype_repeat},
        {2, looptype_repeat}, // used for chromatic repeat
    };

    SOUND_CONTROL_MODS getControlModDataForPattern()
    {
        SOUND_CONTROL_MODS mods;

        auto pattern = XRSequencer::getHeapCurrentSelectedPattern();

        std::string grooveForPattern = pattern.groove_id > -1 ? XRClock::getGrooveString(pattern.groove_id) : "";
        std::string grooveAmountForPattern = XRClock::getGrooveAmountString(pattern.groove_amount);

        mods.aName = "L.STEP";
        mods.bName = "GROOVE";
        mods.cName = "GR.AMT";
        mods.dName = "--";

        mods.aValue = std::to_string(pattern.last_step);
        mods.bValue = pattern.groove_id > -1 ? grooveForPattern : "OFF";
        mods.cValue = pattern.groove_amount > 0 ? grooveAmountForPattern : "--";
        mods.dValue = "--";

        return mods;
    }

    SOUND_CONTROL_MODS getControlModDataForTrack()
    {
        SOUND_CONTROL_MODS mods;

        auto track = XRSequencer::getHeapCurrentSelectedTrack();

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

        auto currentSelectedTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto currentSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto modsForCurrentTrackStep = XRSequencer::getModsForCurrentTrackStep();
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

        auto currentSelectedTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto currentSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto modsForCurrentTrackStep = XRSequencer::getModsForCurrentTrackStep();
        auto currentUXMode = XRUX::getCurrentMode();

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            mods.aName = "L.STEP";
            mods.bName = "FILE";
            mods.cName = "SPEED";
            mods.dName = "--";

            mods.aValue = std::to_string(currentSelectedTrack.last_step);
            mods.bValue = std::to_string(currentSelectedTrack.raw_sample_id + 1);
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

        auto currentSelectedTrack = XRSequencer::getHeapCurrentSelectedTrack();
        //auto currentSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        //auto modsForCurrentTrackStep = XRSequencer::getModsForCurrentTrackStep();
        //auto currentUXMode = XRUX::getCurrentMode();

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            mods.aName = "L.STEP";
            mods.bName = "FILE";
            mods.cName = "POS";
            mods.dName = "--";

            mods.aValue = std::to_string(currentSelectedTrack.last_step);
            mods.bValue = std::to_string(currentSelectedTrack.wav_sample_id + 1);
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

        auto currentSelectedTrack = XRSequencer::getHeapCurrentSelectedTrack();
        //auto currentSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        //auto modsForCurrentTrackStep = XRSequencer::getModsForCurrentTrackStep();
        //auto currentUXMode = XRUX::getCurrentMode();

        switch (currentSelectedPageNum)
        {
        case 0: // MAIN
            mods.aName = "LSTP";
            mods.bName = "LEN";
            mods.cName = "--";
            mods.dName = "--";

            mods.aValue = std::to_string(currentSelectedTrack.last_step); // TODO : impl
            mods.bValue = std::to_string(currentSelectedTrack.length);    // TODO: impl
            mods.cValue = "--";
            mods.dValue = "--";
            break;

        default:
            break;
        }

        return mods;
    }

    SOUND_CONTROL_MODS getMidiControlModData()
    {
        SOUND_CONTROL_MODS mods;

        auto currentSelectedTrack = XRSequencer::getHeapCurrentSelectedTrack();
        //auto currentSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        //auto modsForCurrentTrackStep = XRSequencer::getModsForCurrentTrackStep();
        //auto currentUXMode = XRUX::getCurrentMode();

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

        auto currentSelectedTrack = XRSequencer::getHeapCurrentSelectedTrack();
        //auto currentSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        //auto modsForCurrentTrackStep = XRSequencer::getModsForCurrentTrackStep();
        //auto currentUXMode = XRUX::getCurrentMode();

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

        auto currentSelectedTrack = XRSequencer::getHeapCurrentSelectedTrack();
        //auto currentSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        //auto modsForCurrentTrackStep = XRSequencer::getModsForCurrentTrackStep();
        //auto currentUXMode = XRUX::getCurrentMode();

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

        auto currentSelectedTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto currentSelectedStepNum = XRSequencer::getCurrentSelectedStepNum();
        //auto currentSelectedPageNum = XRSequencer::getCurrentSelectedPage();
        auto modsForCurrentTrackStep = XRSequencer::getModsForCurrentTrackStep();
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
}