#include <XRClock.h>
#include <XRSequencer.h>

namespace XRClock
{
    // MPC60 groove signatures?
    int8_t shuffle_54[2] = {0, 1};
    int8_t shuffle_58[2] = {0, 2};
    int8_t shuffle_62[2] = {0, 3};
    int8_t shuffle_66[2] = {0, 4};
    int8_t shuffle_71[2] = {0, 5};
    std::string _groove_names[1] = {"909"};
    std::string _shuffle_name[5] = {"54%", "58%", "62%", "66%", "71%"};
    int8_t* _shuffle_templates[5] = {shuffle_54, shuffle_58, shuffle_62, shuffle_66, shuffle_71};

    // off-tempo hiphop style? 
    // int8_t shuffle_off_tempo[16] = {0, -1, 0, -2, 0, -1, 0, 2, 0, -1, 0, -2, 0, 3, 0, -1};
    GROOVE_CONFIG _tr909_groove = {
        "909", 
        {"54%", "58%", "62%", "66%", "71%"},
        {shuffle_54, shuffle_58, shuffle_62, shuffle_66, shuffle_71}
    };

    GROOVES _grooves = {{_tr909_groove}};

    void init()
    {

        uClock.init();
        uClock.setClock96PPQNOutput(XRSequencer::ClockOut96PPQN);
        uClock.setClock16PPQNOutput(XRSequencer::ClockOut16PPQN);
        uClock.setOnClockStartOutput(XRSequencer::onClockStart);  
        uClock.setOnClockStopOutput(XRSequencer::onClockStop);
        uClock.setTempo(120);
        uClock.setShuffle(false);
        uClock.setShuffleTemplate(_shuffle_templates[1]);
    }

    void setTempo(float tempo)
    {
        uClock.setTempo(tempo);
    }

    float getTempo()
    {
        return uClock.getTempo();
    }

    std::string getGrooveString(int8_t id)
    {
        return _grooves.configs[id].name;
    }

    std::string getGrooveAmountString(uint8_t id)
    {
        return _shuffle_name[id];
    }
}