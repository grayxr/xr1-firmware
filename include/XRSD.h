#ifndef XRSD_h
#define XRSD_h

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <XRConfig.h>
#include <string>

namespace XRSD
{
    typedef struct
    {
        char lastOpenedProject[50];
    } MACHINE_STATE_0_1_0;

    typedef struct
    {
        char name[50];
        char machineVersion[12];
        float tempo;
    } PROJECT;

    extern MACHINE_STATE_0_1_0 _machine_state;
    extern PROJECT _current_project;

    extern int dexedCurrentPool;
    extern int dexedCurrentBank;
    extern int dexedCurrentPatch;
    extern std::string dexedPatchName;

    bool init();

    void initMachineState();
    bool loadMachineState();

    void createNewProject();
    bool loadLastProject();
    void saveProject();

    void saveCurrentSequencerData();
    
    bool loadActivePattern();
    void saveActivePatternToSdCard();

    bool loadNextPattern(int bank, int pattern);

    bool loadActiveSounds();
    void saveActiveSounds();

    void applyActivePatternGroove();

    bool loadNextPatternSounds(int bank, int pattern);
    bool loadSoundModLayerFromSdCard(int bank, int pattern, int layer);
    void saveActiveSoundModLayerToSdCard();

    void loadDexedVoiceToCurrentTrack(int t = -1);

    void saveCopiedStep(int track, int sourceStep, int destStep);
    void saveCopiedTrackToSamePattern(int sourceTrack, int destTrack);
    
    std::string *getSampleList(int16_t cursor);
    std::string getCurrSampleFileHighlighted();
    void rewindSampleDir();
    void unloadSampleFileListPaged();

    void setActiveSampleSlot(uint8_t slot);
    uint8_t getActiveSampleSlot();

    std::string getCurrentDexedSysexPool();
    std::string getCurrentDexedSysexBank();
    std::string getCurrentDexedSysexPatchNum();
    std::string getCurrentDexedSysexPatchName();
}

#endif /* XRSD_h */