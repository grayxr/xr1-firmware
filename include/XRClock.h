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
    void setTempo(float tempo);
    
    float getTempo();

    std::string getGrooveString(int8_t id);
    std::string getGrooveAmountString(uint8_t id);
}

#endif /* XRClock_h */