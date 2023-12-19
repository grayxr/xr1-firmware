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
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectsPath(projectsPathPrefixBuf); // read char array into string

        // verify project path exists first
        if (!SD.exists(projectsPath.c_str()))
        {
            XRDisplay::drawError("FAILED TO SAVE PROJECT!");
            return;
        }

        std::string projectDataDir = projectsPath;
        projectDataDir += "/";
        projectDataDir += _current_project.name;
        projectDataDir += "/.data";

        // verify project file path exists first
        if (!SD.exists(projectDataDir.c_str()))
        {
            XRDisplay::drawError("FAILED TO SAVE PROJECT!");
            return;
        }

        Serial.println("Write current project data file to SD card!");

        std::string currProjectFilePath;
        currProjectFilePath = projectDataDir;
        currProjectFilePath += "/settings.bin";

        Serial.printf("Saving current project file to SD card at path: %s\n", currProjectFilePath.c_str());
        Serial.printf("with tempo: %f\n", _current_project.tempo);

        File currProjectFile = SD.open(currProjectFilePath.c_str(), FILE_WRITE);
        currProjectFile.truncate();
        currProjectFile.write((byte *)&_current_project, sizeof(_current_project));
        currProjectFile.close();

        // make sure current pattern state is saved to RAM2 
        // before storing the entire sequencer state in SD
        XRSequencer::saveCurrentPatternOffHeap();

        std::string currProjectSequencerFilePath;
        currProjectSequencerFilePath = projectDataDir;
        currProjectSequencerFilePath += "/sequencer.bin";

        Serial.println("Write current project sequencer data binary file to SD card!");

        File seqFileW = SD.open(currProjectSequencerFilePath.c_str(), FILE_WRITE);
        seqFileW.truncate();
        seqFileW.write((byte *)&XRSequencer::sequencer, sizeof(XRSequencer::sequencer));
        seqFileW.close();

        savePatternTrackStepModsToSdCard();

        savePatternSounds();
        savePatternSoundStepModsToSdCard();

        Serial.println("done saving project!");
    }

    bool loadLastProject()
    {
        // // TEMP
        // SD.remove("/audio enjoyer/xr-1/.data");
        // SD.remove("/audio enjoyer/xr-1/projects");
        // XRDisplay::drawError("REBOOT!");
        // return;

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

        // set tempo
        XRClock::setTempo(_current_project.tempo);

        XRSequencer::init();

        if (!loadSequencer()) {
            XRDisplay::drawError("FAILED TO LOAD SEQUENCER DATA!");
            delay(1000);
            return false;
        }

        loadPatternTrackStepModsFromSdCard(
            XRSequencer::getCurrentSelectedBankNum(),
            XRSequencer::getCurrentSelectedPatternNum()
        );

        XRSound::init();

        loadPatternSounds(
            XRSequencer::getCurrentSelectedBankNum(),
            XRSequencer::getCurrentSelectedPatternNum()
        );

        // for now just do this,
        // TODO: just make another method to populate current pattern sounds from SD card
        // so we don't need to move next* to current*
        for (int t=0; t< MAXIMUM_SEQUENCER_TRACKS; t++) {
            XRSound::reinitSoundForTrack(t);
        }

        loadPatternSoundStepModsFromSdCard(
            XRSequencer::getCurrentSelectedBankNum(),
            XRSequencer::getCurrentSelectedPatternNum()
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

        std::string samplePath = "/audio enjoyer/xr-1/samples";

        bool samplePathExsts = SD.exists(samplePath.c_str());
        if (!samplePathExsts) {
            SD.mkdir("/audio enjoyer/xr-1/samples");

            return _sampleFileListPaged.list;
        }

        auto sampleDir = SD.open(samplePath.c_str(), FILE_READ);
        if (!sampleDir.isDirectory()) {
            return _sampleFileListPaged.list;
        }

        uint8_t batchSize = 255;
        for (uint8_t b = 0; b < batchSize; b++) {
            auto sampleFile = sampleDir.openNextFile();

            if (sampleFile && !sampleFile.isDirectory()) {
                _sampleFileList.list[b] = sampleFile.name();
            }

            _sampleFileListLoaded = true;
        }

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
        std::string samplePath = "/audio enjoyer/xr-1/samples";

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

    void loadDexedVoiceToCurrentTrack(int t)
    {
        File sysexDir;

        std::string voiceBankName = "/DEXED/0/";
        voiceBankName += std::to_string(dexedCurrentBank);

        Serial.print("voice bank dir name: ");
        Serial.println(voiceBankName.c_str());

        AudioNoInterrupts();
        sysexDir = SD.open(voiceBankName.c_str());
        AudioInterrupts();

        if (!sysexDir || !sysexDir.isDirectory())
        {
            Serial.println("bank folder not found or invalid!");
            return;
        }

        File entry;
        do
        {
            entry = sysexDir.openNextFile();
        } while (entry.isDirectory());

        // last entry is a folder, return error
        if (entry.isDirectory())
        {
            AudioNoInterrupts();
            entry.close();
            sysexDir.close();
            AudioInterrupts();
            Serial.println("no voice data!");
            return;
        }

        Serial.print("entry name: ");
        Serial.println(entry.name());

        uint8_t data[128];
        if (get_sd_voice(entry, dexedCurrentPatch, data))
        {
            Serial.println("got voice data!");

            uint8_t tmp_data[156];
            auto &comboVoice = t > -1 ? XRSound::getComboVoiceForTrack(t) : XRSound::getComboVoiceForCurrentTrack();
            bool ret =  comboVoice.dexed.decodeVoice(tmp_data, data);
            Serial.print("decode result: ");
            Serial.println(ret ? "true" : "false");
            if (ret) {
                comboVoice.dexed.loadVoiceParameters(tmp_data);
                char dexedTempNameBuf[11];
                comboVoice.dexed.getName(dexedTempNameBuf);
                std::string tempDexedPatchName(dexedTempNameBuf);
                dexedPatchName = tempDexedPatchName;
            }
        }
        else
        {
            Serial.println("did NOT get voice data!");
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

    bool loadSequencer()
    {
        // get project path
        char sProjectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(sProjectsPathPrefixBuf);
        std::string sProjectPath(sProjectsPathPrefixBuf); // read char array into string

        std::string sProjectDataDir = sProjectPath;
        sProjectDataDir += "/";
        sProjectDataDir += _current_project.name;
        sProjectDataDir += "/.data";

        std::string currProjectSequencerFilePath;
        currProjectSequencerFilePath = sProjectDataDir;
        currProjectSequencerFilePath += "/sequencer.bin";

        // verify project file path exists first
        if (!SD.exists(currProjectSequencerFilePath.c_str()))
        {
            return false;
        }

        auto &seq = XRSequencer::getSequencer();

        File seqFileR = SD.open(currProjectSequencerFilePath.c_str(), FILE_READ);
        seqFileR.read((byte *)&seq, sizeof(seq));
        seqFileR.close();

        // setup current pattern in heap from first pattern from first bank in RAM2/DMAMEM
        auto &heapPattern = XRSequencer::getHeapPattern();
        heapPattern = seq.banks[0].patterns[0];

        return true;
    }

    bool loadPatternTrackStepModsFromSdCard(int bank, int pattern)
    {
        // get project path
        char mProjectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(mProjectsPathPrefixBuf);
        std::string mProjectPath(mProjectsPathPrefixBuf); // read char array into string

        std::string mProjectDataDir = mProjectPath;
        mProjectDataDir += "/";
        mProjectDataDir += _current_project.name;
        mProjectDataDir += "/.data";

        // pattern mod file names are /{bank}_{pattern}.bin
        std::string mFilePath;
        mFilePath = mProjectDataDir;
        mFilePath += "/";
        mFilePath += std::to_string(XRSequencer::getCurrentSelectedBankNum());
        mFilePath += "_";
        mFilePath += std::to_string(XRSequencer::getCurrentSelectedPatternNum());
        mFilePath += "_track_step_mods.bin";

        File mFile = SD.open(mFilePath.c_str(), FILE_READ);
        if (!mFile.available()) {
            Serial.println("no patternTrackStepMods available to load!");

            return false;
        }

        mFile.read((byte *)&XRSequencer::patternTrackStepMods, sizeof(XRSequencer::patternTrackStepMods));
        mFile.close();

        Serial.printf("sizeof(patternTrackStepMods): %d\n", sizeof(XRSequencer::patternTrackStepMods));
        
        return true;
    }

    void savePatternTrackStepModsToSdCard()
    {
        // get project path
        char mProjectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(mProjectsPathPrefixBuf);
        std::string mProjectPath(mProjectsPathPrefixBuf); // read char array into string

        std::string mProjectDataDir = mProjectPath;
        mProjectDataDir += "/";
        mProjectDataDir += _current_project.name;
        mProjectDataDir += "/.data";

        // pattern mod file names are {bank}_{pattern}.bin
        std::string mFilePath;
        mFilePath = mProjectDataDir;
        mFilePath += "/";
        mFilePath += std::to_string(XRSequencer::getCurrentSelectedBankNum());
        mFilePath += "_";
        mFilePath += std::to_string(XRSequencer::getCurrentSelectedPatternNum());
        mFilePath += "_track_step_mods.bin";

        File mFile = SD.open(mFilePath.c_str(), FILE_WRITE);
        mFile.truncate();
        mFile.write((byte *)&XRSequencer::patternTrackStepMods, sizeof(XRSequencer::patternTrackStepMods));
        mFile.close();

        Serial.printf("sizeof(patternTrackStepMods): %d\n", sizeof(XRSequencer::patternTrackStepMods));
    }

    bool loadPatternSounds(int bank, int pattern)
    {
        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // pattern sounds file names are {bank}_{pattern}_sounds.bin
        std::string fileDir = projectPath;
        fileDir += "/";
        fileDir += _current_project.name;
        fileDir += "/.data/";
        fileDir += std::to_string(bank);
        fileDir += "_";
        fileDir += std::to_string(pattern); 
        fileDir += "_sounds.bin";

        File soundsFile = SD.open(fileDir.c_str(), FILE_READ);
        if (!soundsFile.available()) {
            Serial.println("Next pattern sounds not available!");

            return false;
        }

        soundsFile.read((byte *)&XRSound::nextPatternSounds, sizeof(XRSound::nextPatternSounds));
        soundsFile.close();

        Serial.printf("sizeof(nextPatternSounds): %d\n", sizeof(XRSound::nextPatternSounds));

        return true;
    }

    void savePatternSounds()
    {
        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // pattern sounds file names are {bank}_{pattern}_sounds.bin
        std::string fileDir = projectPath;
        fileDir += "/";
        fileDir += _current_project.name;
        fileDir += "/.data/";
        fileDir += std::to_string(XRSequencer::getCurrentSelectedBankNum());
        fileDir += "_";
        fileDir += std::to_string(XRSequencer::getCurrentSelectedPatternNum()); 
        fileDir += "_sounds.bin";

        File soundsFile = SD.open(fileDir.c_str(), FILE_WRITE);
        soundsFile.truncate();
        soundsFile.write((byte *)&XRSound::currentPatternSounds, sizeof(XRSound::currentPatternSounds));
        soundsFile.close();

        Serial.printf("sizeof(currentPatternSounds): %d\n", sizeof(XRSound::currentPatternSounds));
    }

    bool loadPatternSoundStepModsFromSdCard(int bank, int pattern)
    {
        // get project path
        char mProjectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(mProjectsPathPrefixBuf);
        std::string mProjectPath(mProjectsPathPrefixBuf); // read char array into string

        std::string mProjectDataDir = mProjectPath;
        mProjectDataDir += "/";
        mProjectDataDir += _current_project.name;
        mProjectDataDir += "/.data";

        // pattern mod file names are /{bank}_{pattern}.bin
        std::string mFilePath;
        mFilePath = mProjectDataDir;
        mFilePath += "/";
        mFilePath += std::to_string(bank);
        mFilePath += "_";
        mFilePath += std::to_string(pattern);
        mFilePath += "_sound_step_mods.bin";

        File mFile = SD.open(mFilePath.c_str(), FILE_READ);
        if (!mFile.available()) {
            Serial.println("no patternSoundStepMods available to load!");

            return false;
        }

        mFile.read((byte *)&XRSound::patternSoundStepMods, sizeof(XRSound::patternSoundStepMods));
        mFile.close();

        Serial.printf("sizeof(patternSoundStepMods): %d\n", sizeof(XRSound::patternSoundStepMods));
        
        return true;
    }

    void savePatternSoundStepModsToSdCard()
    {
        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // pattern sounds file names are {bank}_{pattern}_sounds.bin
        std::string fileDir = projectPath;
        fileDir += "/";
        fileDir += _current_project.name;
        fileDir += "/.data/";
        fileDir += std::to_string(XRSequencer::getCurrentSelectedBankNum());
        fileDir += "_";
        fileDir += std::to_string(XRSequencer::getCurrentSelectedPatternNum()); 
        fileDir += "_sound_step_mods.bin";

        File sFile = SD.open(fileDir.c_str(), FILE_WRITE);
        sFile.truncate();
        sFile.write((byte *)&XRSound::patternSoundStepMods, sizeof(XRSound::patternSoundStepMods));
        sFile.close();

        Serial.printf("sizeof(currentPatternSounds): %d\n", sizeof(XRSound::currentPatternSounds));
    }
}