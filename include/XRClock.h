#ifndef XRClock_h
#define XRClock_h

#include <Arduino.h>
#include <uClock.h>
#include <XRConfig.h>
#include <string>

namespace XRClock
{
    typedef struct {
        std::string name;
        std::string option_names[MAXIMUM_GROOVE_OPTIONS];
        int8_t* templates[MAXIMUM_GROOVE_OPTIONS];
    } GROOVE_CONFIG;

    typedef struct {
        GROOVE_CONFIG configs[MAXIMUM_GROOVE_CONFIGS];
    } GROOVES;

    void init();
    
    void start();
    void pause();
    void stop();

    void setTempo(float tempo);
    void setShuffle(bool active);
    void setShuffleTemplate(int8_t* shuffleTemplate);
    void setShuffleTemplateForGroove(int8_t grooveId, int8_t grooveAmount);
    
    float getTempo();

    std::string getGrooveString(int8_t id);
    std::string getGrooveAmountString(uint8_t id);

    int8_t* getShuffleTemplateForGroove(int8_t grooveId, int8_t grooveAmount);
}

#endif /* XRClock_h */