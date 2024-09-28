#ifndef XRSD_h
#define XRSD_h

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <XRConfig.h>
#include <string>
#include <SdFat.h>

#define SD_CONFIG SdioConfig(FIFO_SDIO)

#define ASYNC_IO_BUFFER_SIZE 512 * 80
#define ASYNC_IO_W_BUFFER_SIZE 512 * 8

namespace XRSD
{
    typedef struct {
        std::string filename;
        byte buffer[ASYNC_IO_W_BUFFER_SIZE];
        uint32_t offset;
        uint32_t remaining;
        uint32_t size;
        bool complete = false;
        bool started = false;
        bool open = false;
    } WRITE_IO;

    extern EXTMEM WRITE_IO wActivePatternSettingsIO;
    extern EXTMEM WRITE_IO wActiveTrackLayerIO;
    extern EXTMEM WRITE_IO wActiveRatchetLayerIO;
    extern EXTMEM WRITE_IO wActiveKitIO;

    extern bool loadNextPatternAsync;
    extern bool loadNextSoundsAsync;
    extern bool loadNextSoundModsAsync;
    extern bool asyncFileReadComplete;
    bool readFileBufferedAsync(std::string filename, void *buf, size_t size);

    extern bool saveActivePatternAsync;
    extern bool saveActiveSoundsAsync;
    extern bool saveActiveSoundModsAsync;
    extern bool asyncFileWriteComplete;
    
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

    void initWriteIO(WRITE_IO &wIO);

    bool sdBusy();

    void initMachineState();
    bool loadMachineState();

    void createNewProject();
    bool loadLastProject();

    void saveProject();
    void saveCurrentProjectDataSync();

    bool loadActivePatternSettingsSync();
    bool loadActiveTrackLayerSync();
    bool loadActiveRatchetLayerSync();
    bool loadActiveKitSync();
    
    bool loadNextPatternSettings(int bank, int pattern, bool async = false);
    bool loadNextTrackLayer(int bank, int pattern, int layer, bool async = false);
    bool loadNextRatchetLayer(int bank, int pattern, bool async = false);
    bool loadNextKit(int bank, int pattern, bool async = false);

    void saveActivePatternSettings(bool async = false);
    void saveActiveRatchetLayer(bool async = false);
    void saveActiveTrackLayer(bool async = false);
    void saveActiveKit(bool async = false);

    void applyActivePatternGroove();
    void loadDexedVoiceToCurrentTrack(int t = -1);

    void saveCopiedStep(int track, int sourceStep, int destStep);
    void saveCopiedTrackToSameLayer(int sourceTrack, int destTrack);
    
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
    
    std::string getPatternSettingsFilename(int8_t bank, int8_t pattern);
    std::string getTrackLayerFilename(int8_t bank, int8_t pattern, int8_t layer);
    std::string getRatchetLayerFilename(int8_t bank, int8_t pattern);
    std::string getKitFilename(int8_t bank, int8_t pattern);
    std::string getActivePatternSettingsFilename();
    std::string getActiveTrackLayerFilename();
    std::string getActiveRatchetLayerFilename();
    std::string getActiveKitFilename();
}

#endif /* XRSD_h */