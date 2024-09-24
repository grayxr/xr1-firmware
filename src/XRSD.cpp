#include <Arduino.h>
#include <XRSD.h>
#include <string>
#include <XRHelpers.h>
#include <XRDisplay.h>
#include <XRKeyInput.h>
#include <XRUX.h>
#include <XRSequencer.h>
#include <XRClock.h>

namespace XRSD
{
    enum WRITE_FILE_TYPE
    {
        ACTIVE_PATTERN = 0,
        ACTIVE_SOUNDS,
        ACTIVE_SOUND_MODS
    };

    SdFs sd;

    //async read
    File asyncReadFile;
    bool asyncFileReadComplete = false;
    uint32_t asyncReadFileTotalRead = 0;

    bool loadNextPatternAsync = false;
    bool loadNextSoundsAsync = false;
    bool loadNextSoundModsAsync = false;

    EXTMEM WRITE_IO wAsyncActivePatternIO;
    EXTMEM WRITE_IO wAsyncActiveSoundsIO;
    EXTMEM WRITE_IO wAsyncActiveSoundModsIO;

    bool saveActivePatternAsync = false;
    bool saveActiveSoundsAsync = false;
    bool saveActiveSoundModsAsync = false;

    bool ready = false;

    bool sdBusy() { return ready ? sd.card()->isBusy() : false; }

    MACHINE_STATE_0_1_0 _machine_state;
    PROJECT _current_project;

    typedef struct
    {
        std::string list[255];
    } SAMPLE_FILE_LIST;

    typedef struct
    {
        std::string list[5];
    } SAMPLE_FILE_LIST_PAGED;

    bool _sampleFileListLoaded = false;
    bool _sampleFileListPagedLoaded = false;

    SAMPLE_FILE_LIST _sampleFileList;
    SAMPLE_FILE_LIST_PAGED _sampleFileListPaged;

    int16_t _lastCursorPos = 0;
    int16_t _currSamplePerPageIdx = 0;

    std::string _currSampleFileHighlighted = "";

    uint8_t _activeSampleSlot = 0;

    int dexedCurrentPool = 0;
    int dexedCurrentBank = 0;
    int dexedCurrentPatch = 0;

    std::string dexedPatchName = "";

    bool get_sd_voice(File sysex, uint8_t voice_number, uint8_t *data);

    bool init()
    {
        SPI.setMOSI(SDCARD_MOSI_PIN);
        SPI.setSCK(SDCARD_SCK_PIN);

        if (!(SD.begin(SDCARD_CS_PIN)))
        {
            return false;
        }

        // Initialize the SDFat
        if (!sd.begin(SD_CONFIG)) {
            return false;
        }

        // initialize async write io
        wAsyncActivePatternIO.filename = "";
        wAsyncActivePatternIO.offset = 0;
        wAsyncActivePatternIO.remaining = 0;
        wAsyncActivePatternIO.complete = false;
        wAsyncActivePatternIO.started = false;

        ready = true;

        return true;
    }

    void rewriteFileBuffered(std::string filename, const byte *data, size_t size)
    {
        File file = SD.open(filename.c_str(), FILE_WRITE_BEGIN);
        if (!file)
        {
            Serial.println("Failed to open file for writing");
            //XRDisplay::drawError("FAILED TO OVERWRITE FILE!");
            //delay(2000);
            //return;
        }

        const size_t bufferSize = 512;
        byte buffer[bufferSize];

        int remaining = size;
        //Serial.printf("size: %d\n", size);

        size_t offset = 0;

        while (remaining > 0) {
            size_t chunkSize = min(bufferSize, remaining);
            //Serial.printf("remainingBefore: %d, chunkSize: %d ", remaining, chunkSize);

            memcpy(buffer, data + offset, chunkSize);
            size_t bytesWritten = file.write(buffer, chunkSize);

            offset += chunkSize;
            remaining -= chunkSize;
            //Serial.printf("bytesWritten: %d, remainingAfter: %d\n", bytesWritten, remaining);
        }

        Serial.println("done writing file!");

        file.close();
    }
    
    void rewriteFileBufferedAsync(WRITE_FILE_TYPE type, const byte *wData)
    {
        if(sdBusy()) {
            Serial.println("SD CARD BUSY, CANNOT ASYNC WRITE!");
            return;
        }

        auto &wIO = type == ACTIVE_PATTERN ? 
            wAsyncActivePatternIO : 
            (type == ACTIVE_SOUNDS ? wAsyncActiveSoundsIO : wAsyncActiveSoundModsIO);

        // Serial.printf("offset: %d, complete: %d, remaining: %d for file: %s\n", wIO.offset, (int)wIO.complete, wIO.remaining, wIO.filename.c_str());
        // // test early return
        // wIO.remaining = 0;
        // wIO.complete = true;
        // return;

        // Serial.printf("expected size: %d for file: %s", wIO.remaining, wIO.filename.c_str());
        
        wIO.file = SD.open(wIO.filename.c_str(), FILE_WRITE_BEGIN);
        if (!wIO.file)
        {
            Serial.printf("Failed to open file for writing: %s\n", wIO.filename.c_str());
            return;
        }

        if (wIO.remaining > 0) {
            uint32_t chunkSize = min(ASYNC_IO_BUFFER_SIZE, wIO.remaining);

            memcpy(wIO.buffer, wData + wIO.offset, chunkSize);

            uint32_t bytesWritten = wIO.file.write(wIO.buffer, chunkSize);

            wIO.offset += chunkSize;
            wIO.remaining -= chunkSize; // should use bytesWritten or chunkSize?

            Serial.printf("remaining: %d\n", wIO.remaining);
        }

        if (wIO.remaining <= 0) {
            Serial.printf("REMAINING <=0, remaining: %d, complete: %d\n", wIO.remaining, (int)wIO.complete);
            wIO.complete = true;
        }

        wIO.file.close();
    }

    bool readFileBuffered(std::string filename, void *buf, size_t size)
    {
        File file = SD.open(filename.c_str(), FILE_READ);
        if (!file || !file.available())
        {
            Serial.printf("Failed to open file for reading: %s\n", filename.c_str());
            return false;
        }

        Serial.printf("expected size: %d for file: %s\n", size, filename.c_str());

        // uint8_t* dst = reinterpret_cast<uint8_t*>(buf);
       // uint8_t* dst[512];

        uint32_t total_read = 0;
        int8_t *index = (int8_t*)buf;

        while(file.available()) {
            uint16_t bytesRead = file.read(index, 512);
            if (bytesRead == -1)
                break;

            //Serial.printf("bytesRead: %d\n", bytesRead);

            total_read += bytesRead;
            index += bytesRead;
        }

        file.close();

        Serial.println("done reading file!");

        return true;
    }

    bool readFileBufferedAsync(std::string filename, void *buf, size_t size)
    {
        if(sdBusy()) {
            Serial.println("SD CARD BUSY, CANNOT ASYNC READ!");

            return true;
        }

        Serial.printf("expected size: %d for file: %s", size, filename.c_str());

        asyncReadFile = SD.open(filename.c_str(), FILE_READ);
        if (!asyncReadFile || !asyncReadFile.available())
        {
            Serial.printf("Failed to open file for reading: %s\n", filename.c_str());
            return false;
        }

        Serial.printf(" asyncReadFile available: %d", asyncReadFile.available());

        size_t bufferSize = ASYNC_IO_BUFFER_SIZE;
        int8_t *index = (int8_t*)buf;
        uint32_t chunkSize = min(bufferSize, size - asyncReadFileTotalRead);

        asyncReadFileTotalRead += asyncReadFile.readBytes((char *)index, chunkSize);
        Serial.printf(" asyncReadFileTotalRead: %d, chunkSize: %d\n", asyncReadFileTotalRead, chunkSize);

        if (asyncReadFileTotalRead == size) {
            asyncFileReadComplete = true;
            asyncReadFileTotalRead = 0;
            asyncReadFile.close();

            Serial.printf("done reading file %s!\n", filename.c_str());

            return true;
        }

        asyncReadFile.close();

        return true;
    }

    void initMachineState()
    {
        std::string newProjectName = XRKeyInput::get();
        if (newProjectName.length() < 1)
        {
            XRDisplay::drawError("INVALID PROJ. NAME!");
            return;
        }

        // initialize machine state file
        char machineStateDirBuf[50];
        XRHelpers::getMachineStateDir(machineStateDirBuf);
        std::string machineStateDir(machineStateDirBuf);

        char machineStateFilenameBuf[50];
        XRHelpers::getMachineStateFile(machineStateFilenameBuf);
        std::string machineStateFilename(machineStateFilenameBuf);

        int lastHyphenPos = machineStateFilename.rfind('-');
        std::string machineStateVersion = machineStateFilename.substr(lastHyphenPos + 1, 5); // todo: fix 5 char version string limit
        // Serial.printf("machineStateVersion: %s\n", machineStateVersion.c_str());

        // write machine state binary for current version
        if (machineStateVersion == "0.1.0")
        {
            // verify file dir exists first
            if (!SD.exists(machineStateDir.c_str()))
            {
                if (!SD.mkdir(machineStateDir.c_str()))
                {
                    XRDisplay::drawError("SD MKDIR ERR!");
                    return;
                }
            }

            std::string finalPath = machineStateDir + machineStateFilename;

            File newMachineStateFile = SD.open(finalPath.c_str(), FILE_WRITE);

            strcpy(_machine_state.lastOpenedProject, newProjectName.c_str());

            newMachineStateFile.write((byte *)&_machine_state, sizeof(_machine_state));
            newMachineStateFile.close();
        }
    }

    bool loadMachineState()
    {
        char machineStateDirBuf[50];
        XRHelpers::getMachineStateDir(machineStateDirBuf);
        std::string machineStateDir(machineStateDirBuf);

        char machineStateFilenameBuf[50];
        XRHelpers::getMachineStateFile(machineStateFilenameBuf);
        std::string machineStateFilename(machineStateFilenameBuf);
        std::string finalPath = machineStateDir + machineStateFilename;

        File machineStateFile = SD.open(finalPath.c_str(), FILE_READ);
        if (!machineStateFile.available())
        {
            Serial.println("machine state file not found!");
            return false;
        }

        machineStateFile.read((byte *)&_machine_state, sizeof(_machine_state));
        machineStateFile.close();

        return true;
    }

    void createNewProject()
    {
        initMachineState();

        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectsPath(projectsPathPrefixBuf); // read char array into string

        // verify file dir exists first
        if (!SD.exists(projectsPath.c_str()))
        {
            if (!SD.mkdir(projectsPath.c_str()))
            {
                XRDisplay::drawError("SD MKDIR ERR!");
                return;
            }
        }

        std::string newProjectName = XRKeyInput::get();

        // populate project struct
        strcpy(_current_project.name, newProjectName.c_str());
        strcpy(_current_project.machineVersion, FIRMWARE_VERSION);
        _current_project.tempo = 120.0;

        std::string newProjectDataDir = projectsPath;
        newProjectDataDir += "/";
        newProjectDataDir += newProjectName;
        newProjectDataDir += "/.data";

        // verify file dir exists first
        if (!SD.exists(newProjectDataDir.c_str()))
        {
            if (!SD.mkdir(newProjectDataDir.c_str()))
            {
                XRDisplay::drawError("SD MKDIR ERR!");
                return;
            }
        }

        std::string newProjectFilePath;
        newProjectFilePath = newProjectDataDir;
        newProjectFilePath += "/settings.bin";

        File newProjectFile = SD.open(newProjectFilePath.c_str(), FILE_WRITE);
        newProjectFile.write((byte *)&_current_project, sizeof(_current_project));
        newProjectFile.close();

        delay(100);
        
        XRSequencer::init();
        XRSound::init();

        saveProject();

        XRUX::setCurrentMode(XRUX::UX_MODE::PATTERN_WRITE);
        XRDisplay::drawSequencerScreen(false);
    }

    void saveProject()
    {
        if (XRSequencer::getSeqState().playbackState == XRSequencer::SEQUENCER_PLAYBACK_STATE::RUNNING) {
            XRSequencer::toggleSequencerPlayback(STOP_BTN_CHAR); // TODO: find better way to stop the sequencer
        }

        XRUX::setCurrentMode(XRUX::UX_MODE::PROJECT_BUSY);

        XRDisplay::drawGeneralConfirmOverlay("SAVING PROJECT...");

        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectsPath(projectsPathPrefixBuf); // read char array into string

        // verify project path exists first
        if (!SD.exists(projectsPath.c_str()))
        {
            Serial.println("Projects directory is missing!");

            XRDisplay::drawError("FAILED TO SAVE PROJECT!");
            return;
        }

        std::string projectDataDir = projectsPath;
        projectDataDir += "/";
        projectDataDir += _current_project.name;
        projectDataDir += "/.data";

        // verify project data dir exists first
        if (!SD.exists(projectDataDir.c_str()))
        {
            Serial.println("Project data directory is missing!");
            
            XRDisplay::drawError("FAILED TO SAVE PROJECT!");
            return;
        }

        Serial.println("Write current project settings data file to SD card!");

        std::string currProjectFilePath;
        currProjectFilePath = projectDataDir;
        currProjectFilePath += "/settings.bin";

        Serial.printf("Saving current project settings file to SD card at path: %s with tempo: %f\n", currProjectFilePath.c_str(),  _current_project.tempo);

        File currProjectFile = SD.open(currProjectFilePath.c_str(), FILE_WRITE);
        currProjectFile.truncate();
        currProjectFile.write((byte *)&_current_project, sizeof(_current_project));
        currProjectFile.close();

        saveCurrentSequencerData();
        saveActiveSounds();
        saveActiveSoundModLayer();

        Serial.println("done saving project!");
    }

    void saveCurrentSequencerData()
    {
        rewriteFileBuffered(getActivePatternFilename(), (byte *)&XRSequencer::activePattern, sizeof(XRSequencer::activePattern));
    }

    bool loadLastProject()
    {
        std::string lastKnownProject(_machine_state.lastOpenedProject);

        if (lastKnownProject.length() == 0)
        {
            XRDisplay::drawError("NO PROJECT IN STATE!");
            delay(1000);
            return false;
        }

        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string
        projectPath += "/";
        projectPath += lastKnownProject;
        projectPath += "/.data/settings.bin";

        File lastKnownProjectFile = SD.open(projectPath.c_str(), FILE_READ);
        if (!lastKnownProjectFile.available())
        {
            XRDisplay::drawError("CANNOT LOAD PROJECT!");
            delay(1000);
            return false;
        }

        lastKnownProjectFile.read((byte *)&_current_project, sizeof(_current_project));
        lastKnownProjectFile.close();

        XRClock::setTempo(_current_project.tempo);

        XRSequencer::init();

        if (!loadActivePattern()) {
            XRDisplay::drawError("FAILED TO LOAD PATTERN DATA!");
            delay(2000);
            return false;
        }

        applyActivePatternGroove();

        XRSound::init();

        loadActiveSounds();
        XRSound::applyActiveSounds();

        loadSoundModLayer(
            XRSequencer::getCurrentSelectedBankNum(),
            XRSequencer::getCurrentSelectedPatternNum(),
            XRSequencer::getCurrentSelectedTrackLayerNum()
        );

        Serial.printf(
            "proj name: %s proj machineVersion: %s proj tempo: %f\n",
            _current_project.name,
            _current_project.machineVersion,
            _current_project.tempo
        );

        XRUX::setCurrentMode(XRUX::UX_MODE::PATTERN_WRITE);
        XRDisplay::drawSequencerScreen(false);

        return true;
    }

    bool loadActivePattern()
    {
        if(!readFileBuffered(getActivePatternFilename(), (byte *)&XRSequencer::activePattern, sizeof(XRSequencer::activePattern))) {
            return false;
        }

        return true;
    }

    void applyActivePatternGroove()
    {
        // if loaded active pattern has a groove, set it on the clock
        if (XRSequencer::activePattern.groove.id > -1) {
            // first apply pattern groove
            XRClock::setShuffle(true);
            XRClock::setShuffleTemplateForGroove(XRSequencer::activePattern.groove.id, XRSequencer::activePattern.groove.amount);

            // then apply track level groove / microtiming
            XRClock::setShuffleForAllTracks(true);
            XRClock::setShuffleTemplateForGrooveForAllTracks(
                XRSequencer::activePattern.groove.id, 
                XRSequencer::activePattern.groove.amount
            );
        } else {
            // otherwise make sure any track microtiming mod grooves are enabled
            XRClock::initializeShuffleForAllTrackMods();
        }
    }

    bool loadNextPattern(int bank, int pattern, bool async)
    {
        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // pattern file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/pattern.bin
        std::string baseDir = projectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer_bank_";
        baseDir += std::to_string(bank);
        baseDir += "_pattern_";
        baseDir += std::to_string(pattern);
        baseDir += ".bin";

        //Serial.printf("loadNextPattern:  %s\n", baseDir.c_str());

        if (async) {
            if (!readFileBufferedAsync(baseDir, (byte *)&XRSequencer::idlePattern, sizeof(XRSequencer::idlePattern))) {
                return false;
            }
        } else {
            if (!readFileBuffered(baseDir, (byte *)&XRSequencer::idlePattern, sizeof(XRSequencer::idlePattern))) {
                return false;
            }
        }

        // File f = SD.open(baseDir.c_str(), FILE_READ);
        // if (!f.available()) {
        //     Serial.println("Active pattern file not available!");
        //     return false;
        // }

        // f.read((byte *)&XRSequencer::idlePattern, sizeof(XRSequencer::idlePattern));
        // f.close();

        //Serial.printf("READING next pattern accent: %d\n", XRSequencer::idlePattern.accent);

        //Serial.printf("sizeof(idlePattern): %d\n", sizeof(XRSequencer::idlePattern));

        return true;
    }

    bool loadActiveSounds()
    {
        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // pattern sounds file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/sounds.bin
        std::string baseDir = projectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer_bank_";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedBankNum());
        baseDir += "_pattern_";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedPatternNum());
        baseDir += "_sounds.bin";

        File soundsFile = SD.open(baseDir.c_str(), FILE_READ);
        if (!soundsFile.available()) {
            Serial.println("Active sounds not available!");

            return false;
        }

        soundsFile.read((byte *)&XRSound::activeSounds, sizeof(XRSound::activeSounds));
        soundsFile.close();

        //Serial.printf("sizeof(activeSounds): %d\n", sizeof(XRSound::activeSounds));

        return true;
    }

    bool loadNextPatternSounds(int bank, int pattern, bool async)
    {
        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // pattern sounds file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/sounds.bin
        std::string baseDir = projectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer_bank_";
        baseDir += std::to_string(bank);
        baseDir += "_pattern_";
        baseDir += std::to_string(pattern);
        baseDir += "_sounds.bin";

        //Serial.printf("idleSounds:  %s\n", baseDir.c_str());

        if (async) {
            if (!readFileBufferedAsync(baseDir, (byte *)&XRSound::idleSounds, sizeof(XRSound::idleSounds))) {
                return false;
            }
        } else {
            if (!readFileBuffered(baseDir, (byte *)&XRSound::idleSounds, sizeof(XRSound::idleSounds))) {
                return false;
            }
        }

        // File f = SD.open(baseDir.c_str(), FILE_READ);
        // if (!f.available()) {
        //     Serial.println("Next sounds file not available!");
        //     return false;
        // }

        // f.read((byte *)&XRSound::idleSounds, sizeof(XRSound::idleSounds));
        // f.close();

        //Serial.printf("sizeof(idleSounds): %d\n", sizeof(XRSound::idleSounds));

        return true;
    }

    bool loadSoundModLayer(int bank, int pattern, int layer, bool async)
    {
        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // sound mod layer file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/layers/{layer}/sound_mod.bin
        std::string baseDir = projectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer/bank_";
        baseDir += std::to_string(bank);
        baseDir += "_pattern_";
        baseDir += std::to_string(pattern);
        baseDir += "_layer_";
        baseDir += std::to_string(layer);
        baseDir += "_sound_mod.bin";

       // Serial.printf("activeSoundModLayer:  %s\n", baseDir.c_str());

        if (async) {
            if (!readFileBufferedAsync(baseDir, (byte *)&XRSound::activeSoundModLayer, sizeof(XRSound::activeSoundModLayer))) {
                return false;
            }
        } else {
            if (!readFileBuffered(baseDir, (byte *)&XRSound::activeSoundModLayer, sizeof(XRSound::activeSoundModLayer))) {
                return false;
            }
        }

        // File f = SD.open(baseDir.c_str(), FILE_READ);
        // if (!f.available()) {
        //     Serial.println("Sound mod file not available!");
        //     return false;
        // }

        // f.read((byte *)&XRSound::activeSoundModLayer, sizeof(XRSound::activeSoundModLayer));
        // f.close();
        
        //Serial.printf("sizeof(activeSoundModLayer): %d\n", sizeof(XRSound::activeSoundModLayer));
        
        return true;
    }

    bool loadNextSoundModLayer(int bank, int pattern, int layer, bool async)
    {
        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // sound mod layer file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/layers/{layer}/sound_mod.bin
        std::string baseDir = projectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer_bank_";
        baseDir += std::to_string(bank);
        baseDir += "_pattern_";
        baseDir += std::to_string(pattern);
        baseDir += "_layer_";
        baseDir += std::to_string(layer);
        baseDir += "_sound_mod.bin";

        //Serial.printf("idleSoundModLayer:  %s\n", baseDir.c_str());

        if (async) {
            if (!readFileBufferedAsync(baseDir, (byte *)&XRSound::idleSoundModLayer, sizeof(XRSound::idleSoundModLayer))) {
                return false;
            }
        } else {
            if (!readFileBuffered(baseDir, (byte *)&XRSound::idleSoundModLayer, sizeof(XRSound::idleSoundModLayer))) {
                return false;
            }
        }

        // File f = SD.open(baseDir.c_str(), FILE_READ);
        // if (!f.available()) {
        //     Serial.println("Sound mod file not available!");
        //     return false;
        // }

        // f.read((byte *)&XRSound::activeSoundModLayer, sizeof(XRSound::activeSoundModLayer));
        // f.close();
        
        //Serial.printf("sizeof(activeSoundModLayer): %d\n", sizeof(XRSound::activeSoundModLayer));
        
        return true;
    }

    std::string getActivePatternFilename()
    {
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // pattern file names are {project}/.data/sequencer_bank_{bank}_pattern_{pattern}.bin
        std::string activePatternDir = projectPath;
        activePatternDir += "/";
        activePatternDir += _current_project.name;
        activePatternDir += "/.data/sequencer_bank_";
        activePatternDir += std::to_string(XRSequencer::getCurrentSelectedBankNum());
        activePatternDir += "_pattern_";
        activePatternDir += std::to_string(XRSequencer::getCurrentSelectedPatternNum());
        activePatternDir += ".bin";

        Serial.printf("GET FILENAME FOR activePattern: %s\n", activePatternDir.c_str());

        return activePatternDir;
    }

    void saveActivePattern(bool async)
    {
        if (async) {
            rewriteFileBufferedAsync(WRITE_FILE_TYPE::ACTIVE_PATTERN, (byte *)&XRSequencer::activePattern);
        } else {
            rewriteFileBuffered(getActivePatternFilename(), (byte *)&XRSequencer::activePattern, sizeof(XRSequencer::activePattern));
        }
    }

    void saveActiveSounds()
    {
        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // pattern sounds file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/sounds.bin
         std::string baseDir = projectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer_bank_";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedBankNum());
        baseDir += "_pattern_";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedPatternNum());
        baseDir += "_sounds.bin";

        //Serial.printf("activeSounds: %s\n", baseDir.c_str());

        rewriteFileBuffered(baseDir, (byte *)&XRSound::activeSounds, sizeof(XRSound::activeSounds));

        //Serial.printf("sizeof(activePatternSounds): %d\n", sizeof(XRSound::activePatternSounds));
    }

    void saveActiveSoundModLayer()
    {
        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // sound step mod file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/layers/{layer}/sound_mod.bin
        std::string baseDir = projectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer_bank_";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedBankNum());
        baseDir += "_pattern_";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedPatternNum());
        baseDir += "_layer_";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedTrackLayerNum());
        baseDir += "_sound_mod.bin";

        //Serial.printf("activeSoundModLayer:  %s\n", baseDir.c_str());

        rewriteFileBuffered(baseDir, (byte *)&XRSound::activeSoundModLayer, sizeof(XRSound::activeSoundModLayer));

        //Serial.printf("sizeof(activeSoundModLayer): %d\n", sizeof(XRSound::activeSoundModLayer));
    }

    void saveCopiedStep(int track, int sourceStep, int destStep)
    {
        // XRSequencer::activeTrackLayer.tracks[track].steps[destStep] = XRSequencer::activeTrackLayer.tracks[track].steps[sourceStep];
        // XRSequencer::activeTrackStepModLayer.tracks[track].steps[destStep] = XRSequencer::activeTrackStepModLayer.tracks[track].steps[sourceStep];
        // XRSound::activePatternSoundStepModLayer.sounds[track].steps[destStep] = XRSound::activePatternSoundStepModLayer.sounds[track].steps[sourceStep];

        // // TODO: save above to SD card as well ?
    }

    void saveCopiedTrackToSamePattern(int sourceTrack, int destTrack)
    {
        // // -- BEGIN COPY TRACK SOUND

        // // Serial.printf("COPY TRACK: BEGIN COPY SOUND! \n");

        // // get project path
        // char projectsPathPrefixBuf[50];
        // XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        // std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // std::string baseDir = projectPath;
        // baseDir += "/";
        // baseDir += _current_project.name;
        // baseDir += "/.data/sequencer/banks/";
        // baseDir += std::to_string(XRSequencer::getCurrentSelectedBankNum());

        // // verify bnk dir exists first
        // if (!SD.exists(baseDir.c_str()))
        // {
        //     if (!SD.mkdir(baseDir.c_str()))
        //     {
        //         XRDisplay::drawError("SD MKDIR ERR!");
        //         return;
        //     }
        // }

        // // IMPORTANT: FOR NOW ONLY ALLOW COPYING TRACKS WITHIN SAME PATTERN
        // // LATER ALLOW COPYING TRACKS ACROSS PATTERNS

        // baseDir += "/patterns/";
        // baseDir += std::to_string(XRSequencer::getCurrentSelectedPatternNum());

        // // verify ptn dir exists first
        // if (!SD.exists(baseDir.c_str()))
        // {
        //     if (!SD.mkdir(baseDir.c_str()))
        //     {
        //         XRDisplay::drawError("SD MKDIR ERR!");
        //         return;
        //     }
        // }
        
        // std::string sFileDir = baseDir;
        // sFileDir += "/sounds.bin";

        // // File sFileR = SD.open(baseDir.c_str(), FILE_READ);
        // // sFileR.read((byte *)&XRSound::patternSoundsCopyBuffer, sizeof(XRSound::patternSoundsCopyBuffer));
        // // sFileR.close();

        // // put sound from source track into copy buffer destination track sound
        // XRSound::activePatternSounds[destTrack] = XRSound::activePatternSounds[sourceTrack];

        // // save the sounds file

        // File sFileW = SD.open(sFileDir.c_str(), FILE_WRITE);
        // sFileW.truncate();
        // sFileW.write((byte *)&XRSound::activePatternSounds, sizeof(XRSound::activePatternSounds));
        // sFileW.close();

        // // Serial.printf("COPY TRACK: DONE COPY SOUND! \n");

        // // --- END COPY TRACK SOUND

        // std::string baseLayerDir = baseDir;
        // baseLayerDir += "/layers";

        // // verify layer dir exists first
        // if (!SD.exists(baseLayerDir.c_str()))
        // {
        //     if (!SD.mkdir(baseLayerDir.c_str()))
        //     {
        //         XRDisplay::drawError("SD MKDIR ERR!");
        //         return;
        //     }
        // }

        // // -- BEGIN LAYER LOOP

        // auto baseLayerDirObj = SD.open(baseLayerDir.c_str(), FILE_READ);
        // if (!baseLayerDirObj.isDirectory())
        // {
        //     XRDisplay::drawError("SD ERR!");
        //     baseLayerDirObj.close();
        //     return;
        // }

        // // Serial.printf("COPY TRACK: BEGIN COPY LAYERS! \n");

        // // search all potential track layer folders
        // for (int l=0; l < MAXIMUM_SEQUENCER_TRACK_LAYERS; l++)
        // {
        //     std::string layerDir = baseLayerDir;
        //     layerDir += "/";
        //     layerDir += std::to_string(l);

        //     auto layerDirObj = SD.open(layerDir.c_str(), FILE_READ);
            
        //     // Serial.print("COPY TRACK: LAYER DIR: ");
        //     // Serial.println(layerDir.c_str());

        //     // Serial.printf("layerDirObj.isDirectory: %d \n", layerDirObj.isDirectory());

        //     if (layerDirObj && layerDirObj.isDirectory()) 
        //     {
        //         // copy track data
        //         std::string lTrackDir = layerDir;
        //         lTrackDir += "/track.bin";
                
        //         File lTrackFileR = SD.open(lTrackDir.c_str(), FILE_READ);
        //         if (lTrackFileR.available())
        //         {
        //             // Serial.printf("COPY TRACK: BEGIN COPY TRACK DATA FOR LAYER %d! \n", l);
        //             // XRSequencer::initTrackLayerCopyBuffer();

        //             // lTrackFileR.read((byte *)&XRSequencer::trackLayerCopyBuffer, sizeof(XRSequencer::trackLayerCopyBuffer));
        //             // lTrackFileR.close();

        //             // XRSequencer::trackLayerCopyBuffer.tracks[destTrack] = XRSequencer::activeTrackLayer.tracks[sourceTrack];
        //             XRSequencer::activeTrackLayer.tracks[destTrack] = XRSequencer::activeTrackLayer.tracks[sourceTrack];

        //             File lTrackFileW = SD.open(lTrackDir.c_str(), FILE_WRITE);
        //             lTrackFileW.truncate();
        //             lTrackFileW.write((byte *)&XRSequencer::activeTrackLayer, sizeof(XRSequencer::activeTrackLayer));
        //             lTrackFileW.close();
        //             lTrackFileR.close();

        //             // Serial.printf("COPY TRACK: DONE COPY TRACK DATA FOR LAYER %d! \n", l);
        //         } else {
        //             lTrackFileR.close();
        //         }

        //         // copy step mod data
        //         std::string lTrackStepModDir = layerDir;
        //         lTrackStepModDir += "/track_step_mod.bin";

        //         File lTrackStepModR = SD.open(lTrackStepModDir.c_str(), FILE_READ);
        //         if (lTrackStepModR.available())
        //         {
        //             // Serial.printf("COPY TRACK: BEGIN COPY TRACK STEP MOD DATA FOR LAYER %d! \n", l);

        //             // TODO: impl this?
        //             // XRSequencer::initTrackStepModLayerCopyBuffer(); 

        //             // lTrackStepModR.read((byte *)&XRSequencer::trackStepModLayerCopyBuffer, sizeof(XRSequencer::trackStepModLayerCopyBuffer));
        //             // lTrackStepModR.close();

        //             // XRSequencer::trackStepModLayerCopyBuffer.tracks[destTrack] = XRSequencer::activeTrackStepModLayer.tracks[sourceTrack];
        //             XRSequencer::activeTrackStepModLayer.tracks[destTrack] = XRSequencer::activeTrackStepModLayer.tracks[sourceTrack];

        //             File lTrackStepModW = SD.open(lTrackStepModDir.c_str(), FILE_WRITE);
        //             lTrackStepModW.truncate();
        //             lTrackStepModW.write((byte *)&XRSequencer::activeTrackStepModLayer, sizeof(XRSequencer::activeTrackStepModLayer));
        //             lTrackStepModW.close();
        //             lTrackStepModR.close();

        //             // Serial.printf("COPY TRACK: DONE COPY TRACK STEP MOD DATA FOR LAYER %d! \n", l);
        //         } else {
        //             lTrackStepModR.close();
        //         }

        //         // copy sound step mod data
        //         std::string lSoundStepModDir = layerDir;
        //         lSoundStepModDir += "/sound_step_mod.bin";

        //         File lSoundStepModR = SD.open(lSoundStepModDir.c_str(), FILE_READ);
        //         if (lSoundStepModR.available())
        //         {
        //             // Serial.printf("COPY TRACK: BEGIN COPY SOUND STEP MOD DATA FOR LAYER %d! \n", l);

        //             // TODO: impl this?
        //             // XRSound::initPatternSoundStepModLayerCopyBuffer(); 

        //             // lSoundStepModR.read((byte *)&XRSound::patternSoundStepModLayerCopyBuffer, sizeof(XRSound::patternSoundStepModLayerCopyBuffer));
        //             // lSoundStepModR.close();

        //             //XRSound::patternSoundStepModLayerCopyBuffer.sounds[destTrack] = XRSound::activePatternSoundStepModLayer.sounds[sourceTrack];
        //             XRSound::activePatternSoundStepModLayer.sounds[destTrack] = XRSound::activePatternSoundStepModLayer.sounds[sourceTrack];

        //             File lSoundStepModW = SD.open(lTrackStepModDir.c_str(), FILE_WRITE);
        //             lSoundStepModW.truncate();
        //             lSoundStepModW.write((byte *)&XRSound::activePatternSoundStepModLayer, sizeof(XRSound::activePatternSoundStepModLayer));
        //             lSoundStepModW.close();
        //             lSoundStepModR.close();

        //             // Serial.printf("COPY TRACK: DONE COPY SOUND STEP MOD DATA FOR LAYER %d! \n", l);
        //         } else {
        //             lSoundStepModR.close();
        //         }
        //     }

        //     layerDirObj.close();
        // }

        // // Serial.printf("COPY TRACK: DONE COPY LAYERS! \n");

        // baseLayerDirObj.close();

        // // -- END LAYER LOOP
    }

    std::string *getSampleList(int16_t cursor)
    {
        if (_sampleFileListLoaded) {
            _currSampleFileHighlighted = _sampleFileList.list[cursor];

            uint8_t pageSize = 5;

            for (uint8_t p = 0; p < pageSize; p++) {
                if ((p+cursor) <= 255) {
                    _sampleFileListPaged.list[p] = _sampleFileList.list[p+cursor];
                }
            }

            return _sampleFileListPaged.list;
        }

        std::string samplePath = "/samples";

        bool samplePathExsts = SD.exists(samplePath.c_str());
        if (!samplePathExsts) {
            SD.mkdir("/samples");

            return _sampleFileListPaged.list;
        }

        auto sampleDir = SD.open(samplePath.c_str(), FILE_READ);
        if (!sampleDir.isDirectory()) {
            return _sampleFileListPaged.list;
        }

        _sampleFileList.list[0] = "--NONE--";

        uint16_t batchSize = 256;
        for (uint16_t b = 1; b < batchSize; b++) {
            auto sampleFile = sampleDir.openNextFile();

            if (sampleFile && !sampleFile.isDirectory()) {
                _sampleFileList.list[b] = sampleFile.name();
            }
            _sampleFileListLoaded = true;
            if (!sampleFile){
                break;
            }
        }

        sampleDir.close();

        uint8_t pageSize = 5;
        for (uint8_t p = 0; p < pageSize; p++) {
            _sampleFileListPaged.list[p] = _sampleFileList.list[p];
        }
        
        _sampleFileListPagedLoaded = true;
        
        _currSampleFileHighlighted = _sampleFileList.list[cursor];

        return  _sampleFileListPaged.list;
    }

    void rewindSampleDir()
    { 
        std::string samplePath = "/samples";

        bool samplePathExsts = SD.exists(samplePath.c_str());
        if (!samplePathExsts) {
            Serial.println("ERROR! unable to reset sample dir!");

            return;
        }

        auto sampleDir = SD.open(samplePath.c_str(), FILE_READ);
        if (!sampleDir.isDirectory()) {
            Serial.println("ERROR! unable to reset sample dir!");

            return;
        }

        sampleDir.rewindDirectory();
    }

    void unloadSampleFileListPaged()
    {
        _sampleFileListPagedLoaded = false;
    }

    void setActiveSampleSlot(uint8_t slot)
    {
        _activeSampleSlot = slot;
    }

    uint8_t getActiveSampleSlot()
    {
        return _activeSampleSlot;
    }

    void loadDexedVoiceToCurrentTrack(int t)
    {
        File sysexDir;

        std::string voiceBankName = "/audio enjoyer/xr-1/sysex/dexed/";
        voiceBankName += std::to_string(dexedCurrentPool);
        voiceBankName += "/";
        voiceBankName += std::to_string(dexedCurrentBank);

        AudioNoInterrupts();

        sysexDir = SD.open(voiceBankName.c_str());

        AudioInterrupts();

        if (!sysexDir || !sysexDir.isDirectory())
        {
            return;
        }

        File entry;
        do
        {
            entry = sysexDir.openNextFile();
        } while (entry.isDirectory());

        if (entry.isDirectory())
        {
            AudioNoInterrupts();

            entry.close();
            sysexDir.close();

            AudioInterrupts();

            return;
        }

        uint8_t data[128];

        if (get_sd_voice(entry, dexedCurrentPatch, data))
        {
            uint8_t tmp_data[156];

            int8_t trackNum = 0;
            if (t > -1) {
                trackNum = t;
            } else {
                trackNum = XRSequencer::getCurrentSelectedTrackNum();
            }

            auto di = XRDexedManager::getActiveInstanceForTrack(trackNum);

            if (XRSound::dexedInstances[di].dexed.decodeVoice(tmp_data, data)) {
                XRSound::dexedInstances[di].dexed.loadVoiceParameters(tmp_data);

                char dexedTempNameBuf[11];
                XRSound::dexedInstances[di].dexed.getName(dexedTempNameBuf);

                std::string tempDexedPatchName(dexedTempNameBuf);
                dexedPatchName = tempDexedPatchName;

                std::string currTrackName(XRSound::activeSounds[trackNum].name);
                strcpy(XRSound::activeSounds[trackNum].name, tempDexedPatchName.c_str());
            }
        }

        AudioNoInterrupts();

        entry.close();
        sysexDir.close();

        AudioInterrupts();
    }

    bool get_sd_voice(File sysex, uint8_t voice_number, uint8_t *data)
    {
        uint16_t n;
        int32_t bulk_checksum_calc = 0;
        int8_t bulk_checksum;

        AudioNoInterrupts();
        if (sysex.size() != 4104) // check sysex size
        {
            return (false);
        }

        sysex.seek(0);
        if (sysex.read() != 0xf0) // check sysex start-byte
        {
            return (false);
        }
        if (sysex.read() != 0x43) // check sysex vendor is Yamaha
        {
            return (false);
        }
        sysex.seek(4103);
        if (sysex.read() != 0xf7) // check sysex end-byte
        {
            return (false);
        }
        sysex.seek(3);
        if (sysex.read() != 0x09) // check for sysex type (0x09=32 voices)
        {
            return (false);
        }
        sysex.seek(4102); // Bulk checksum
        bulk_checksum = sysex.read();

        sysex.seek(6); // start of bulk data
        for (n = 0; n < 4096; n++)
        {
            uint8_t d = sysex.read();
            if (n >= voice_number * 128 && n < (voice_number + 1) * 128)
                data[n - (voice_number * 128)] = d;
            bulk_checksum_calc -= d;
        }
        bulk_checksum_calc &= 0x7f;
        AudioInterrupts();

        if (bulk_checksum_calc != bulk_checksum)
        {
            return (false);
        }

        return (true);
    }

    std::string getCurrSampleFileHighlighted()
    {
        return _currSampleFileHighlighted;
    }

    std::string getCurrentDexedSysexPool()
    {
        return std::to_string(dexedCurrentPool);
    }
    
    std::string getCurrentDexedSysexBank()
    {
        return std::to_string(dexedCurrentBank);
    }

    std::string getCurrentDexedSysexPatchNum()
    {
        return std::to_string(dexedCurrentPatch);
    }

    std::string getCurrentDexedSysexPatchName()
    {
        return dexedPatchName;
    }
}