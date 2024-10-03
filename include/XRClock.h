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
        uint8_t templateSize;
        uint8_t templateCount;
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
    void setShuffleForTrack(uint8_t track, bool active);
    void setShuffleForAllTracks(bool active);
    void setShuffleTemplateForGroove(int8_t grooveId, int8_t grooveAmount);
    void setShuffleTemplateForGrooveForAllTracks(int8_t grooveId, int8_t grooveAmount);
    void setShuffleTemplateForTrack(int8_t track, int8_t * t, uint8_t s);
    void initializeShuffleForAllTrackMods();
    
    float getTempo();

    std::string getClockTimeString();
    
    std::string getGrooveString(int8_t id);
    std::string getGrooveAmountString(int8_t grooveId, uint8_t amountId);

    int8_t* getShuffleTemplateForGroove(int8_t grooveId, int8_t grooveAmount);
    
    uint8_t getShuffleTemplateSizeForGroove(int8_t grooveId, int8_t grooveAmount);
    uint8_t getShuffleTemplateCountForGroove(int8_t grooveId);
}

#endif /* XRClock_h */