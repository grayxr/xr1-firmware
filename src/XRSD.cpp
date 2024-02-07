#include <Arduino.h>
#include <XRSD.h>
#include <string>
#include <XRAsyncPSRAMLoader.h>
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

    DMAMEM SAMPLE_FILE_LIST _sampleFileList;
    DMAMEM SAMPLE_FILE_LIST_PAGED _sampleFileListPaged;

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
        saveActivePatternSounds();
        saveActiveSoundStepModLayerToSdCard();

        Serial.println("done saving project!");
    }

    void saveCurrentSequencerData()
    {
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectsPath(projectsPathPrefixBuf); // read char array into string

        std::string activePatternDir = projectsPath;
        activePatternDir += "/";
        activePatternDir += _current_project.name;
        activePatternDir += "/.data/sequencer/banks/";
        activePatternDir += std::to_string(XRSequencer::getCurrentSelectedBankNum());

        // verify bnk dir exists first
        if (!SD.exists(activePatternDir.c_str()))
        {
            if (!SD.mkdir(activePatternDir.c_str()))
            {
                XRDisplay::drawError("SD MKDIR ERR!");
                return;
            }
        }
        activePatternDir += "/patterns/";
        activePatternDir += std::to_string(XRSequencer::getCurrentSelectedPatternNum());

        // verify ptn dir exists first
        if (!SD.exists(activePatternDir.c_str()))
        {
            if (!SD.mkdir(activePatternDir.c_str()))
            {
                XRDisplay::drawError("SD MKDIR ERR!");
                return;
            }
        }

        std::string patternFilePath = activePatternDir;
        patternFilePath += "/pattern.bin";

        Serial.println("Write active pattern file binary file to SD card at path: ");
        Serial.println(patternFilePath.c_str());

        File patternFileW = SD.open(patternFilePath.c_str(), FILE_WRITE);
        patternFileW.truncate();
        patternFileW.write((byte *)&XRSequencer::activePattern, sizeof(XRSequencer::activePattern));
        patternFileW.close();

        std::string trackLayerFilePath = activePatternDir;
        trackLayerFilePath += "/layers/";
        trackLayerFilePath += std::to_string(XRSequencer::getCurrentSelectedTrackLayerNum());

        // verify lyr dir exists first
        if (!SD.exists(trackLayerFilePath.c_str()))
        {
            if (!SD.mkdir(trackLayerFilePath.c_str()))
            {
                XRDisplay::drawError("SD MKDIR ERR!");
                return;
            }
        }

        trackLayerFilePath += "/track.bin";

        Serial.println("Write active pattern track layer binary file to SD card!");

        File trackLayerFileW = SD.open(trackLayerFilePath.c_str(), FILE_WRITE);
        trackLayerFileW.truncate();
        trackLayerFileW.write((byte *)&XRSequencer::activeTrackLayer, sizeof(XRSequencer::activeTrackLayer));
        trackLayerFileW.close();

        saveActiveTrackStepModLayerToSdCard();
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
            delay(1000);
            return false;
        }

        if (!loadActiveTrackLayer()) {
            XRDisplay::drawError("FAILED TO LOAD TRACK LAYER DATA!");
            delay(1000);
            return false;
        }

        loadActiveTrackStepModLayerFromSdCard(
            XRSequencer::getCurrentSelectedBankNum(),
            XRSequencer::getCurrentSelectedPatternNum(),
            XRSequencer::getCurrentSelectedTrackLayerNum()
        );

        XRSound::init();
        loadActivePatternSounds();
        XRSound::applyActivePatternSounds();

        XRAsyncPSRAMLoader::startAsyncInitOfCurrentSamples();

        loadPatternSoundStepModLayerFromSdCard(
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
        // get project path
        char sProjectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(sProjectsPathPrefixBuf);
        std::string sProjectPath(sProjectsPathPrefixBuf); // read char array into string

        // pattern file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/pattern.bin
        std::string baseDir = sProjectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer/banks/0/patterns/0/pattern.bin";

        Serial.print("attempt to read active pattern file from path: ");
        Serial.println(baseDir.c_str());

        // verify active pattern file path exists first
        if (!SD.exists(baseDir.c_str()))
        {
            Serial.println("could not find pattern file!");

            return false;
        }

        File ptnFileR = SD.open(baseDir.c_str(), FILE_READ);
        ptnFileR.read((byte *)&XRSequencer::activePattern, sizeof(XRSequencer::activePattern));
        ptnFileR.close();

        Serial.printf("sizeof(:activePattern): %d\n", sizeof(XRSequencer::activePattern));

        // if loaded active pattern has a groove, set it on the clock
        if (XRSequencer::activePattern.groove.id > -1) {
            XRClock::setShuffle(true);
            XRClock::setShuffleTemplateForGroove(XRSequencer::activePattern.groove.id, XRSequencer::activePattern.groove.amount);
        }

        return true;
    }

    bool loadActiveTrackLayer()
    {
        // get project path
        char sProjectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(sProjectsPathPrefixBuf);
        std::string sProjectPath(sProjectsPathPrefixBuf); // read char array into string

        // track layer file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/layers/{layer}/track.bin
        std::string baseDir = sProjectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer/banks/0/patterns/0/layers/0/track.bin";

        Serial.print("attempt to read active track layer file from path: ");
        Serial.println(baseDir.c_str());

        // verify active pattern file path exists first
        if (!SD.exists(baseDir.c_str()))
        {
            Serial.println("active track layer file does not exist!");

            return false;
        }

        File trackLayerFileR = SD.open(baseDir.c_str(), FILE_READ);
        trackLayerFileR.read((byte *)&XRSequencer::activeTrackLayer, sizeof(XRSequencer::activeTrackLayer));
        trackLayerFileR.close();

        Serial.printf("sizeof(activeTrackLayer): %d\n", sizeof(XRSequencer::activeTrackLayer));

        return true;
    }

    void saveActiveTrackLayerToSdCard()
    {
        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // track layer file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/layers/{layer}/track.bin
        std::string baseDir = projectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer/banks/";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedBankNum());

        // verify bnk dir exists first
        if (!SD.exists(baseDir.c_str()))
        {
            if (!SD.mkdir(baseDir.c_str()))
            {
                XRDisplay::drawError("SD MKDIR ERR!");
                return;
            }
        }

        baseDir += "/patterns/";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedPatternNum());

        // verify ptn dir exists first
        if (!SD.exists(baseDir.c_str()))
        {
            if (!SD.mkdir(baseDir.c_str()))
            {
                XRDisplay::drawError("SD MKDIR ERR!");
                return;
            }
        }

        baseDir += "/layers/";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedTrackLayerNum());


        // verify lyr dir exists first
        if (!SD.exists(baseDir.c_str()))
        {
            if (!SD.mkdir(baseDir.c_str()))
            {
                XRDisplay::drawError("SD MKDIR ERR!");
                return;
            }
        }

        baseDir += "/track.bin";

        Serial.print("attempt to write active track layer file to path: ");
        Serial.println(baseDir.c_str());

        File lFile = SD.open(baseDir.c_str(), FILE_WRITE);
        lFile.truncate();
        lFile.write((byte *)&XRSequencer::activeTrackLayer, sizeof(XRSequencer::activeTrackLayer));
        lFile.close();

        Serial.printf("sizeof(activeTrackLayer): %d\n", sizeof(XRSequencer::activeTrackLayer));
    }

    bool loadActiveTrackStepModLayerFromSdCard(int bank, int pattern, int layer)
    {
        // get project path
        char mProjectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(mProjectsPathPrefixBuf);
        std::string mProjectPath(mProjectsPathPrefixBuf); // read char array into string

        // track step mod file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/layers/{layer}/track_step_mod.bin
        std::string baseDir = mProjectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer/banks/";
        baseDir += std::to_string(bank);
        baseDir += "/patterns/";
        baseDir += std::to_string(pattern);
        baseDir += "/layers/";
        baseDir += std::to_string(layer);
        baseDir += "/track_step_mod.bin";

        File mFile = SD.open(baseDir.c_str(), FILE_READ);
        if (!mFile.available()) {
            Serial.println("no track step mod layer available to load!");

            return false;
        }

        mFile.read((byte *)&XRSequencer::activeTrackStepModLayer, sizeof(XRSequencer::activeTrackStepModLayer));
        mFile.close();

        Serial.printf("sizeof(activeTrackStepModLayer): %d\n", sizeof(XRSequencer::activeTrackStepModLayer));
        
        return true;
    }

    void saveActiveTrackStepModLayerToSdCard()
    {
        // get project path
        char mProjectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(mProjectsPathPrefixBuf);
        std::string mProjectPath(mProjectsPathPrefixBuf); // read char array into string

        // track step mod file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/layers/{layer}/track_step_mod.bin
        std::string baseDir = mProjectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer/banks/";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedBankNum());

        // verify bnk dir exists first
        if (!SD.exists(baseDir.c_str()))
        {
            if (!SD.mkdir(baseDir.c_str()))
            {
                XRDisplay::drawError("SD MKDIR ERR!");
                return;
            }
        }

        baseDir += "/patterns/";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedPatternNum());

        // verify ptn dir exists first
        if (!SD.exists(baseDir.c_str()))
        {
            if (!SD.mkdir(baseDir.c_str()))
            {
                XRDisplay::drawError("SD MKDIR ERR!");
                return;
            }
        }

        baseDir += "/layers/";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedTrackLayerNum());

        // verify lyr dir exists first
        if (!SD.exists(baseDir.c_str()))
        {
            if (!SD.mkdir(baseDir.c_str()))
            {
                XRDisplay::drawError("SD MKDIR ERR!");
                return;
            }
        }

        baseDir += "/track_step_mod.bin";

        File mFile = SD.open(baseDir.c_str(), FILE_WRITE);
        mFile.truncate();
        mFile.write((byte *)&XRSequencer::activeTrackStepModLayer, sizeof(XRSequencer::activeTrackStepModLayer));
        mFile.close();

        Serial.printf("sizeof(activeTrackStepModLayer): %d\n", sizeof(XRSequencer::activeTrackStepModLayer));
    }

    bool loadNextPattern(int bank, int pattern)
    {
        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // pattern sounds file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/sounds.bin
        std::string baseDir = projectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer/banks/";
        baseDir += std::to_string(bank);
        baseDir += "/patterns/";
        baseDir += std::to_string(pattern);
        baseDir += "/pattern.bin";

        File patternFile = SD.open(baseDir.c_str(), FILE_READ);
        if (!patternFile.available()) {
            Serial.println("Next pattern config not available!");

            return false;
        }

        patternFile.read((byte *)&XRSequencer::nextPattern, sizeof(XRSequencer::nextPattern));
        patternFile.close();

        Serial.printf("sizeof(nextPattern): %d\n", sizeof(XRSequencer::nextPattern));

        return true;
    }

    bool loadNextTrackLayer(int bank, int pattern, int layer)
    {
        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // track layer file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/layers/{layer}/track.bin
        std::string baseDir = projectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer/banks/";
        baseDir += std::to_string(bank);
        baseDir += "/patterns/";
        baseDir += std::to_string(pattern);
        baseDir += "/layers/";
        baseDir += std::to_string(layer);

        std::string trackLayerDir = baseDir;
        trackLayerDir += "/track.bin";

        Serial.print("attempt to read next track layer file from path: ");
        Serial.println(trackLayerDir.c_str());

        File trackLayerFile = SD.open(trackLayerDir.c_str(), FILE_READ);
        if (!trackLayerFile.available()) {
            Serial.println("Next track layer not available!");

            return false;
        }

        trackLayerFile.read((byte *)&XRSequencer::nextTrackLayer, sizeof(XRSequencer::nextTrackLayer));
        trackLayerFile.close();

        Serial.printf("sizeof(nextTrackLayer): %d\n", sizeof(XRSequencer::nextTrackLayer));

        return true;
    }

    bool loadActivePatternSounds()
    {
        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // pattern sounds file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/sounds.bin
        std::string baseDir = projectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer/banks/";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedBankNum());
        baseDir += "/patterns/";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedPatternNum());
        baseDir += "/sounds.bin";

        File soundsFile = SD.open(baseDir.c_str(), FILE_READ);
        if (!soundsFile.available()) {
            Serial.println("Active pattern sounds not available!");

            return false;
        }

        soundsFile.read((byte *)&XRSound::activePatternSounds, sizeof(XRSound::activePatternSounds));
        soundsFile.close();

        Serial.printf("sizeof(activePatternSounds): %d\n", sizeof(XRSound::activePatternSounds));

        return true;
    }

    bool loadNextPatternSounds(int bank, int pattern)
    {
        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // pattern sounds file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/sounds.bin
        std::string baseDir = projectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer/banks/";
        baseDir += std::to_string(bank);
        baseDir += "/patterns/";
        baseDir += std::to_string(pattern);
        baseDir += "/sounds.bin";

        File soundsFile = SD.open(baseDir.c_str(), FILE_READ);
        if (!soundsFile.available()) {
            Serial.println("Next pattern sounds not available!");

            return false;
        }

        soundsFile.read((byte *)&XRSound::nextPatternSounds, sizeof(XRSound::nextPatternSounds));
        soundsFile.close();

        Serial.printf("sizeof(nextPatternSounds): %d\n", sizeof(XRSound::nextPatternSounds));

        return true;
    }

    void saveActivePatternToSdCard()
    {
        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // pattern sounds file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/sounds.bin
        std::string baseDir = projectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer/banks/";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedBankNum());

        // verify bnk dir exists first
        if (!SD.exists(baseDir.c_str()))
        {
            if (!SD.mkdir(baseDir.c_str()))
            {
                XRDisplay::drawError("SD MKDIR ERR!");
                return;
            }
        }

        baseDir += "/patterns/";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedPatternNum());

        // verify ptn dir exists first
        if (!SD.exists(baseDir.c_str()))
        {
            if (!SD.mkdir(baseDir.c_str()))
            {
                XRDisplay::drawError("SD MKDIR ERR!");
                return;
            }
        }

        baseDir += "/pattern.bin";

        File ptnFile = SD.open(baseDir.c_str(), FILE_WRITE);
        ptnFile.truncate();
        ptnFile.write((byte *)&XRSequencer::activePattern, sizeof(XRSequencer::activePattern));
        ptnFile.close();

        Serial.printf("sizeof(activePattern): %d\n", sizeof(XRSequencer::activePattern));
    }

    void saveActivePatternSounds()
    {
        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // pattern sounds file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/sounds.bin
         std::string baseDir = projectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer/banks/";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedBankNum());

        // verify bnk dir exists first
        if (!SD.exists(baseDir.c_str()))
        {
            if (!SD.mkdir(baseDir.c_str()))
            {
                XRDisplay::drawError("SD MKDIR ERR!");
                return;
            }
        }

        baseDir += "/patterns/";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedPatternNum());

        // verify ptn dir exists first
        if (!SD.exists(baseDir.c_str()))
        {
            if (!SD.mkdir(baseDir.c_str()))
            {
                XRDisplay::drawError("SD MKDIR ERR!");
                return;
            }
        }

        baseDir += "/sounds.bin";

        File soundsFile = SD.open(baseDir.c_str(), FILE_WRITE);
        soundsFile.truncate();
        soundsFile.write((byte *)&XRSound::activePatternSounds, sizeof(XRSound::activePatternSounds));
        soundsFile.close();

        Serial.printf("sizeof(activePatternSounds): %d\n", sizeof(XRSound::activePatternSounds));
    }

    bool loadPatternSoundStepModLayerFromSdCard(int bank, int pattern, int layer)
    {
        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // sound step mod file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/layers/{layer}/sound_step_mod.bin
        std::string baseDir = projectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer/banks/";
        baseDir += std::to_string(bank);
        baseDir += "/patterns/";
        baseDir += std::to_string(pattern);
        baseDir += "/layers/";
        baseDir += std::to_string(layer);
        baseDir += "/sound_step_mod.bin";

        File mFile = SD.open(baseDir.c_str(), FILE_READ);
        if (!mFile.available()) {
            Serial.println("no sound step mods available to load!");

            return false;
        }

        mFile.read((byte *)&XRSound::activePatternSoundStepModLayer, sizeof(XRSound::activePatternSoundStepModLayer));
        mFile.close();

        Serial.printf("sizeof(activePatternSoundStepModLayer): %d\n", sizeof(XRSound::activePatternSoundStepModLayer));
        
        return true;
    }

    void saveActiveSoundStepModLayerToSdCard()
    {
        // get project path
        char projectsPathPrefixBuf[50];
        XRHelpers::getProjectsDir(projectsPathPrefixBuf);
        std::string projectPath(projectsPathPrefixBuf); // read char array into string

        // sound step mod file names are {project}/.data/sequencer/banks/{bank}/patterns/{pattern}/layers/{layer}/sound_step_mod.bin
        std::string baseDir = projectPath;
        baseDir += "/";
        baseDir += _current_project.name;
        baseDir += "/.data/sequencer/banks/";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedBankNum());

        // verify bnk dir exists first
        if (!SD.exists(baseDir.c_str()))
        {
            if (!SD.mkdir(baseDir.c_str()))
            {
                XRDisplay::drawError("SD MKDIR ERR!");
                return;
            }
        }

        baseDir += "/patterns/";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedPatternNum());

        // verify ptn dir exists first
        if (!SD.exists(baseDir.c_str()))
        {
            if (!SD.mkdir(baseDir.c_str()))
            {
                XRDisplay::drawError("SD MKDIR ERR!");
                return;
            }
        }

        baseDir += "/layers/";
        baseDir += std::to_string(XRSequencer::getCurrentSelectedTrackLayerNum());

        // verify lyr dir exists first
        if (!SD.exists(baseDir.c_str()))
        {
            if (!SD.mkdir(baseDir.c_str()))
            {
                XRDisplay::drawError("SD MKDIR ERR!");
                return;
            }
        }

        baseDir += "/sound_step_mod.bin";

        File sFile = SD.open(baseDir.c_str(), FILE_WRITE);
        sFile.truncate();
        sFile.write((byte *)&XRSound::activePatternSoundStepModLayer, sizeof(XRSound::activePatternSoundStepModLayer));
        sFile.close();

        Serial.printf("sizeof(activePatternSoundStepModLayer): %d\n", sizeof(XRSound::activePatternSoundStepModLayer));
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
            if (!sampleFile){
                break;
            }
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

#ifndef NO_DEXED
    void loadDexedVoiceToCurrentTrack(int t)
    {
        File sysexDir;

        std::string voiceBankName = "/audio enjoyer/xr-1/sysex/dexed/0/";
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

            if (XRSound::dexedInstances[trackNum].dexed.decodeVoice(tmp_data, data)) {
                XRSound::dexedInstances[trackNum].dexed.loadVoiceParameters(tmp_data);

                char dexedTempNameBuf[11];
                XRSound::dexedInstances[trackNum].dexed.getName(dexedTempNameBuf);

                std::string tempDexedPatchName(dexedTempNameBuf);
                dexedPatchName = tempDexedPatchName;

                std::string currTrackName(XRSound::activePatternSounds[trackNum].name);
                strcpy(XRSound::activePatternSounds[trackNum].name, tempDexedPatchName.c_str());
            }
        }

        AudioNoInterrupts();

        entry.close();
        sysexDir.close();

        AudioInterrupts();
    }
#endif

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

    std::string getCurrentDexedSysexBank()
    {
        return std::to_string(dexedCurrentBank);
    }

    std::string getCurrentDexedSysexPatchName()
    {
        return std::to_string(dexedCurrentPatch);
    }
}