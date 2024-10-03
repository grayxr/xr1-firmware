#include <XRMain.h>
#include <XRHelpers.h>
#include <XRSequencer.h>
#include <XRSound.h>
#include <XRUX.h>
#include <XRAsyncIO.h>

namespace XRMain
{
    elapsedMillis runtimeDisplayMs;
    elapsedMillis seqWriteIntervalMs;

    bool ticked = false;
    bool startWrite = false;
    bool writeDisplayUpdate = false;

    void handleRuntimeDisplayUpdates();
    void handleWriteUpdates();

    void boot()
    {
        randomSeed(analogRead(0));

        XRSound::muteAllOutput();

        XRDisplay::init();

        if (!XRSD::init())
        {
            XRDisplay::drawError("SD CARD ERROR!");

            return;
        }

        XRAudio::init();
        XREncoder::init();
        XRCV::init();
        XRLED::init();

        if (!XRVersa::init())
        {
            XRDisplay::drawError("KEYBOARD ERROR!");

            return;
        }

        XRKeyMatrix::init();
        XRKeyMatrix::handleStates(true); // skip any false reads on boot

        XRMIDI::init();
        XRClock::init();

        XRDisplay::drawIntro();

        delay(1000);



        // // testing

        // XRDisplay::drawSampleWaveform();

        // return; // testing



        if (!XRSD::loadMachineState())
        {
            XRUX::setCurrentMode(XRUX::UX_MODE::PROJECT_INITIALIZE);
            XRDisplay::drawCreateProjectDialog();
        }
        else
        {
            XRUX::setCurrentMode(XRUX::UX_MODE::PROJECT_BUSY);

            if (!XRSD::loadLastProject())
            {
                XRDisplay::drawError("ERROR LOADING PROJECT!");

                XRUX::setCurrentMode(XRUX::UX_MODE::PROJECT_INITIALIZE);
                XRDisplay::drawCreateProjectDialog();

                return;
            }
        }

        // delay(5000);
        
        // // log new data structures
        // Serial.printf("sizeof(XRSound::activeKit): %d\n", sizeof(XRSound::activeKit));
        // Serial.printf("sizeof(XRSequencer::activeTrackLayer): %d\n", sizeof(XRSequencer::activeTrackLayer));
        // Serial.printf("sizeof(XRSequencer::activeRatchetLayer): %d\n", sizeof(XRSequencer::activeRatchetLayer));
        // Serial.printf("sizeof(XRSequencer::activePatternSettings): %d\n", sizeof(XRSequencer::activePatternSettings));

        XRAudio::resetMetrics();
    }

    void update()
    {
        XRAudio::handleHeadphones(); 

        XRKeyMatrix::handleStates(false);
        XRVersa::handleStates();
        XREncoder::handleStates();

        XRSequencer::handlePatternQueueActions();
        XRSequencer::handleTrackLayerQueueActions();

        XRSequencer::handleTriggerStates();

        handleWriteUpdates();
        XRAsyncIO::update();

        handleRuntimeDisplayUpdates();
    }

    void handleRuntimeDisplayUpdates()
    {
        if (XRSequencer::getSeqState().playbackState == XRSequencer::RUNNING && XRUX::getCurrentMode() == XRUX::PATTERN_WRITE && runtimeDisplayMs >= 250)
        {
            runtimeDisplayMs -= 250;

            XRDisplay::drawSequencerScreen();
        }
    }

    void handleWriteUpdates()
    {
        if (seqWriteIntervalMs >= 1000) {
            seqWriteIntervalMs -= 1000;

            // writes

            if (XRSequencer::patternSettingsDirty) {
                startWrite = true;

                XRSequencer::patternSettingsForWrite = XRSequencer::activePatternSettings;
                XRAsyncIO::addItem({
                    XRAsyncIO::FILE_TYPE::PATTERN_SETTINGS,
                    XRAsyncIO::FILE_IO_TYPE::WRITE,
                    XRSD::getActivePatternSettingsFilename(),
                    sizeof(XRSequencer::patternSettingsForWrite),
                    nullptr,
                    nullptr
                });

                XRSequencer::patternSettingsDirty = false;
            }

            if (XRSequencer::ratchetLayerDirty) {
                startWrite = true;

                XRSequencer::ratchetLayerForWrite = XRSequencer::activeRatchetLayer;
                XRAsyncIO::addItem({
                    XRAsyncIO::FILE_TYPE::RATCHET_LAYER,
                    XRAsyncIO::FILE_IO_TYPE::WRITE,
                    XRSD::getActiveRatchetLayerFilename(),
                    sizeof(XRSequencer::ratchetLayerForWrite),
                    nullptr,
                    nullptr
                });

                XRSequencer::ratchetLayerDirty = false;
            }

            if (XRSequencer::trackLayerDirty) {
                startWrite = true;

                Serial.printf("trackLayerDirty! %s / current active layer: %d\n", XRSD::getActiveTrackLayerFilename(), XRSequencer::getCurrentSelectedTrackLayerNum());

                XRSequencer::trackLayerForWrite = XRSequencer::activeTrackLayer;
                XRAsyncIO::addItem({
                    XRAsyncIO::FILE_TYPE::TRACK_LAYER,
                    XRAsyncIO::FILE_IO_TYPE::WRITE,
                    XRSD::getActiveTrackLayerFilename(),
                    sizeof(XRSequencer::trackLayerForWrite),
                    nullptr,
                    nullptr
                });

                XRSequencer::trackLayerDirty = false;
            }

            if (XRSound::kitDirty) {
                startWrite = true;

                XRSound::kitForWrite = XRSound::activeKit;
                XRAsyncIO::addItem({
                    XRAsyncIO::FILE_TYPE::KIT,
                    XRAsyncIO::FILE_IO_TYPE::WRITE,
                    XRSD::getActiveKitFilename(),
                    sizeof(XRSound::kitForWrite),
                    nullptr,
                    nullptr
                });

                XRSound::kitDirty = false;
            }

            if (startWrite) {
                XRSD::writeState = XRSD::WRITE_STATE::START;
                startWrite = false;
            }

            if (
                XRUX::getCurrentMode() == XRUX::PATTERN_WRITE || XRUX::getCurrentMode() == XRUX::TRACK_WRITE ||
                XRUX::getCurrentMode() == XRUX::PERFORM_RATCHET
            ){
                if (!writeDisplayUpdate && XRSD::writeState == XRSD::WRITE_STATE::START) {
                    writeDisplayUpdate = true;
                    yield();
                    XRDisplay::drawSequencerScreen(false);
                    Serial.println("DISPLAY WRITE START!");
                } else if (writeDisplayUpdate && (XRSD::writeState == XRSD::WRITE_STATE::COMPLETE || XRSD::writeState == XRSD::WRITE_STATE::IDLE)) {
                    writeDisplayUpdate = false;
                    yield();
                    XRDisplay::drawSequencerScreen(false);
                    Serial.println("DISPLAY WRITE IDLE/COMPLETE!");
                }
            }
        }
    }

}