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

    extern EXTMEM WRITE_IO wAsyncActivePatternIO;
    extern EXTMEM WRITE_IO wAsyncActiveSoundsIO;
    extern EXTMEM WRITE_IO wAsyncActiveSoundModsIO;

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

    void initMachineState();
    bool loadMachineState();

    void createNewProject();
    bool loadLastProject();
    void saveProject();

    void saveCurrentSequencerData();
    
    bool loadActivePattern();
    void saveActivePattern(bool async = false);

    bool loadNextPattern(int bank, int pattern, bool async = false);

    bool loadActiveSounds();
    void saveActiveSounds();

    void applyActivePatternGroove();

    bool loadNextPatternSounds(int bank, int pattern, bool async = false);
    bool loadSoundModLayer(int bank, int pattern, int layer, bool async = false);
    bool loadNextSoundModLayer(int bank, int pattern, int layer, bool async = false);
    void saveActiveSoundModLayer();

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
    
    std::string getActivePatternFilename();
}

#endif /* XRSD_h */