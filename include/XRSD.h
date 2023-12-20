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

    extern int dexedCurrentBank;
    extern int dexedCurrentPatch;
    extern std::string dexedPatchName;

    bool init();

    void initMachineState();
    bool loadMachineState();

    void createNewProject();
    bool loadLastProject();
    void saveProject();

    bool loadSequencer();
    bool loadPatternTrackStepModsFromSdCard(int bank, int pattern);
    void saveSequencer();
    void savePatternTrackStepModsToSdCard();

    bool loadPatternSounds(int bank, int pattern);
    bool loadPatternSoundStepModsFromSdCard(int bank, int pattern);
    void savePatternSounds();
    void savePatternSoundStepModsToSdCard();

    void loadDexedVoiceToCurrentTrack(int t = -1);
    
    std::string *getSampleList(int16_t cursor);
    std::string getCurrSampleFileHighlighted();
    void rewindSampleDir();
    void unloadSampleFileListPaged();

    std::string getCurrentDexedSysexBank();
    std::string getCurrentDexedSysexPatchName();
}

#endif /* XRSD_h */