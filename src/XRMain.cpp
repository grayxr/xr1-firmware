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

    bool writeBusy = false;
    bool readsDone = false;

    bool rStepOneDone = false;
    bool rStepTwoDone = false;
    bool rStepThreeDone = false;
    bool rStepFourDone = false;

    bool wStepOneDone = false;
    bool wStepTwoDone = false;
    bool wStepThreeDone = false;
    bool wStepFourDone = false;

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
        XRSequencer::handleTriggerStates();

        handleWriteUpdates();
        XRAsyncIO::update();

        XRSequencer::handlePatternQueueActions();
        //XRSequencer::handleTrackLayerQueueActions();

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
                XRSequencer::patternSettingsForWrite = XRSequencer::activePatternSettings;
                XRAsyncIO::addItem({
                    XRAsyncIO::FILE_TYPE::PATTERN_SETTINGS,
                    XRAsyncIO::FILE_IO_TYPE::WRITE,
                    XRSD::getActivePatternSettingsFilename(),
                    sizeof(XRSequencer::patternSettingsForWrite),
                });

                XRSequencer::patternSettingsDirty = false;
            }

            if (XRSequencer::ratchetLayerDirty) {
                XRSequencer::ratchetLayerForWrite = XRSequencer::activeRatchetLayer;
                XRAsyncIO::addItem({
                    XRAsyncIO::FILE_TYPE::RATCHET_LAYER,
                    XRAsyncIO::FILE_IO_TYPE::WRITE,
                    XRSD::getActiveRatchetLayerFilename(),
                    sizeof(XRSequencer::ratchetLayerForWrite),
                });

                XRSequencer::ratchetLayerDirty = false;
            }

            if (XRSequencer::trackLayerDirty) {
                XRSequencer::trackLayerForWrite = XRSequencer::activeTrackLayer;
                XRAsyncIO::addItem({
                    XRAsyncIO::FILE_TYPE::TRACK_LAYER,
                    XRAsyncIO::FILE_IO_TYPE::WRITE,
                    XRSD::getActiveTrackLayerFilename(),
                    sizeof(XRSequencer::trackLayerForWrite),
                });

                XRSequencer::trackLayerDirty = false;
            }

            if (XRSound::kitDirty) {
                XRSound::kitForWrite = XRSound::activeKit;
                XRAsyncIO::addItem({
                    XRAsyncIO::FILE_TYPE::KIT,
                    XRAsyncIO::FILE_IO_TYPE::WRITE,
                    XRSD::getActiveKitFilename(),
                    sizeof(XRSound::kitForWrite),
                });

                XRSound::kitDirty = false;
            }
        }
    }
}