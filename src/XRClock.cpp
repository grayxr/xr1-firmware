#include <XRClock.h>
#include <XRSequencer.h>

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
        uClock.setOnClockStart(XRSequencer::onClockStart);
        uClock.setOnClockStop(XRSequencer::onClockStop);
        uClock.setTempo(120);
        uClock.setShuffle(false);
        uClock.setShuffleTemplate(_shuffle_templates[1], 2);
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

    void setShuffleTemplateForGroove(int8_t grooveId, int8_t grooveAmount)
    {
        auto t = _grooves.configs[grooveId].templates[grooveAmount];
        auto s = _grooves.configs[grooveId].templateSize;

        uClock.setShuffleTemplate(t, s);
    }
}