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

    bool init();
    bool loadMachineState();
    bool loadLastProject();

    void initMachineState();
    void createNewProject();
    void saveProject();

    void savePatternModsToSdCard();
    void loadPatternModsFromSdCard();
}

#endif /* XRSD_h */