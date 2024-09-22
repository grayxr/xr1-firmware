#include <XRClock.h>
#include <XRSequencer.h>
#include <XRHelpers.h>

namespace XRClock
{
    int8_t shuffle_50[2] = {0, 0};
    int8_t shuffle_54[2] = {0, 2};
    int8_t shuffle_58[2] = {0, 4};
    int8_t shuffle_62[2] = {0, 6};
    int8_t shuffle_66[2] = {0, 8};
    int8_t shuffle_71[2] = {0, 10};
    int8_t shuffle_75[2] = {0, 12};

    std::string _groove_names[1] = {"909"};
    std::string _shuffle_name[7] = {"50%", "54%", "58%", "62%", "66%", "71%", "75%"};
    int8_t *_shuffle_templates[7] = {shuffle_50, shuffle_54, shuffle_58, shuffle_62, shuffle_66, shuffle_71, shuffle_75};

    GROOVE_CONFIG _tr909_groove = {
        "909",
        {"50%", "54%", "58%", "62%", "66%", "71%", "75%"},
        {shuffle_50, shuffle_54, shuffle_58, shuffle_62, shuffle_66, shuffle_71, shuffle_75},
        2,
        7
    };

    int8_t bap_1[16] = {0, -1, 0, -2, 0, -1, 0, 2, 0, -1, 0, -2, 0, 3, 0, -1};
    int8_t bap_2[16] = {0, -1, -2, 0, 1, -1, 2, -1, 0, -4, 2, -1, 0, 2, 1, 0};
    int8_t bap_3[16] = {0, 1, 2, 0, 5, 0, 3, 0, 3, 0, 2, 1, 5, 0, 4, 2};
    GROOVE_CONFIG _bap_groove = {
        "BAP",
        {"1","2","3"},
        {bap_1,bap_2,bap_3},
        16,
        3
    };

    GROOVES _grooves = {{_tr909_groove,_bap_groove}};

    void init()
    {
        uClock.init();
        uClock.setOnPPQN(XRSequencer::ClockOut96PPQN);
        uClock.setOnStep(XRSequencer::ClockOut16PPQN);       
        uClock.setOnRigidStep(XRSequencer::ClockOutRigid16PPQN);       
        uClock.setTrackOnStep(XRSequencer::ClockOutTracked16PPQN);
        uClock.setOnClockStart(XRSequencer::onClockStart);
        uClock.setOnClockStop(XRSequencer::onClockStop);
        uClock.setTempo(120);
        uClock.setShuffle(false);
        uClock.setShuffleTemplate(_shuffle_templates[1], 2);

        for (size_t i = 0; i < MAXIMUM_SEQUENCER_TRACKS; i++)
        {
            uClock.setTrackShuffleTemplate(i, _shuffle_templates[1], 2);
        }
        
    }

    void setTempo(float tempo)
    {
        uClock.setTempo(tempo);
    }

    float getTempo()
    {
        return uClock.getTempo();
    }

    std::string getClockTimeString()
    {
        auto start = uClock.getPlayTime();

        auto hrs = uClock.getNumberOfHours(start);
        auto mins = uClock.getNumberOfMinutes(start);
        auto secs = uClock.getNumberOfSeconds(start);

        auto hrsStr = XRHelpers::strldz(std::to_string(hrs), 2);
        auto minStr = XRHelpers::strldz(std::to_string(mins), 2);
        auto secStr = XRHelpers::strldz(std::to_string(secs), 2);

        std::string time = hrsStr + ":" + minStr + ":" + secStr;

        return time;
    }

    std::string getGrooveString(int8_t id)
    {
        return _grooves.configs[id].name;
    }

    std::string getGrooveAmountString(int8_t grooveId, uint8_t amountId)
    {
        return _grooves.configs[grooveId].option_names[amountId];
    }

    int8_t* getShuffleTemplateForGroove(int8_t grooveId, int8_t grooveAmount)
    {
       return _grooves.configs[grooveId].templates[grooveAmount];
    }

    uint8_t getShuffleTemplateSizeForGroove(int8_t grooveId, int8_t grooveAmount)
    {
       return _grooves.configs[grooveId].templateSize; // todo: fix
    }

    uint8_t getShuffleTemplateCountForGroove(int8_t grooveId)
    {
       return _grooves.configs[grooveId].templateCount;
    }

    void start()
    {
        uClock.start();
    }

    void pause()
    {
        uClock.pause();
    }

    void stop()
    {
        uClock.stop();
    }

    void setShuffle(bool active)
    {
        uClock.setShuffle(active);
    }

    void setShuffleForTrack(uint8_t track, bool active)
    {
        uClock.setTrackShuffle(track, active);
    }

    void setShuffleForAllTracks(bool active)
    {
        auto layer = XRSequencer::getCurrentSelectedTrackLayerNum();
        auto &activeLayer = XRSequencer::activePattern.layers[layer];

        for (size_t t=0; t < MAXIMUM_SEQUENCER_TRACKS; t++) {
            // if a track has any step with microtiming, we leave the shuffle active on the track
            for (size_t s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
            {
                if (activeLayer.tracks[t].steps[s].flags[XRSequencer::MICROTIMING]) {
                    // make sure microtiming based shuffle is always enabled
                    active = true;
                    break;
                }
            }

            uClock.setTrackShuffle(t, active);
        }
    }

    void setShuffleTemplateForGroove(int8_t grooveId, int8_t grooveAmount)
    {
        auto t = _grooves.configs[grooveId].templates[grooveAmount];
        auto s = _grooves.configs[grooveId].templateSize;

        uClock.setShuffleTemplate(t, s);
    }

    void setShuffleTemplateForGrooveForAllTracks(int8_t grooveId, int8_t grooveAmount)
    {
        auto baseTemplate = _grooves.configs[grooveId].templates[grooveAmount];
        auto baseSize = _grooves.configs[grooveId].templateSize;

        auto layer = XRSequencer::getCurrentSelectedTrackLayerNum();
        auto &activeLayer = XRSequencer::activePattern.layers[layer];

        for (size_t t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            auto &currTrack = XRSequencer::getTrack(t);

            int8_t tmpl[currTrack.lstep] = {0};
            int8_t size = currTrack.lstep;

            // if a track has any step with microtiming, we merge the pattern groove into the track shuffle template
            // and apply it to the steps that don't already have microtiming mods
            for (size_t s = 0; s < currTrack.lstep; s++)
            {
                auto val = baseTemplate[s % baseSize];

                if (activeLayer.tracks[t].steps[s].flags[XRSequencer::MICROTIMING]) {
                    val = activeLayer.tracks[t].steps[s].mods[XRSequencer::MICROTIMING];
                }

                //if (t == 0) Serial.printf("track 1 step: %d baseSize: %d, orig: %d, val: %d\n", s, baseSize, baseTemplate[s % baseSize], val);

                tmpl[s] = val;
            }

            // print tmpl as one string
            // if (t == 0) {
            //     Serial.printf("track 1 template: ");
            //     for (size_t i = 0; i < currTrack.lstep; i++)
            //     {
            //         Serial.printf("%d, ", tmpl[i]);
            //     }
            //     Serial.printf("\n");
            // }
                
            uClock.setTrackShuffleTemplate(t, tmpl, size);
        }
    }

    void initializeShuffleForAllTrackMods()
    {
        auto layer = XRSequencer::getCurrentSelectedTrackLayerNum();
        auto &activeLayer = XRSequencer::activePattern.layers[layer];

        for (size_t t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
        {
            auto &currTrack = XRSequencer::getTrack(t);

            bool shuffleEnabled = false;            
            int8_t tmpl[currTrack.lstep] = {0};
            int8_t size = currTrack.lstep;

            for (size_t s = 0; s < currTrack.lstep; s++)
            {
                if (activeLayer.tracks[t].steps[s].flags[XRSequencer::MICROTIMING]) {
                    shuffleEnabled = true;

                    tmpl[s] = activeLayer.tracks[t].steps[s].mods[XRSequencer::MICROTIMING];
                }
            }

            if (shuffleEnabled) {
                // print tmpl as one string
                // if (t == 0) {
                //     Serial.printf("track 1 template: ");
                //     for (size_t i = 0; i < currTrack.lstep; i++)
                //     {
                //         Serial.printf("%d, ", tmpl[i]);
                //     }
                //     Serial.printf("\n");
                // }
                    
                uClock.setTrackShuffle(t, true);
                uClock.setTrackShuffleTemplate(t, tmpl, size);
            }
        }
    }

    void setShuffleTemplateForTrack(int8_t track, int8_t * t, uint8_t s)
    {
        uClock.setTrackShuffleTemplate(track, t, s);
    }
}