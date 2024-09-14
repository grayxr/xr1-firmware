#include <XRAsyncPSRAMLoader.h>
#include <XRMain.h>
#include <XRHelpers.h>
#include <XRSequencer.h>
#include <XRSound.h>

namespace XRMain
{
    elapsedMillis elapsedMs;

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

        XRAudio::resetMetrics();
    }

    void update()
    {
        XRAsyncPSRAMLoader::handleAsyncPSRAMLoading();

        // TODO: execute async checklist for pattern queue actions
        auto &queuedPattern = XRSequencer::getQueuedPatternState();

        if (!(elapsedMs % 2) && XRSequencer::patternChecklist != 5 && (queuedPattern.bank > -1 && queuedPattern.number > -1)) {
            auto newBank = queuedPattern.bank;
            auto newPattern = queuedPattern.number;

            if (XRSequencer::patternChecklist == 0) {
                Serial.println("checklist index 0");

                // IMPORTANT: must change sounds before changing sequencer data!
                if (XRSound::patternSoundsDirty) {
                    XRSD::saveActivePatternSounds();
                }
                if (!XRSD::loadNextPatternSounds(newBank, newPattern))
                {
                    XRSound::initNextPatternSounds();
                }
                XRSequencer::patternChecklist++;
            } else if (XRSequencer::patternChecklist == 1) {
                Serial.println("checklist index 1");
                // IMPORTANT: must change sounds before changing sequencer data!
                if (XRSound::patternSoundStepModsDirty) {
                    XRSD::saveActiveSoundStepModLayerToSdCard();
                }
                if (!XRSD::loadPatternSoundStepModLayerFromSdCard(newBank, newPattern, 0)) {
                    XRSound::initPatternSoundStepMods();
                }
                XRSequencer::patternChecklist++;
            } else if (XRSequencer::patternChecklist == 2) {
                Serial.println("checklist index 2");
                XRSD::saveActivePatternToSdCard();
                if (!XRSD::loadNextPattern(newBank, newPattern)){
                    XRSequencer::initNextPattern();
                }
                XRSequencer::patternChecklist++;
            } else if (XRSequencer::patternChecklist == 3) {
                Serial.println("checklist index 3");
                XRSD::saveActiveTrackLayerToSdCard();
                if (!XRSD::loadNextTrackLayer(newBank, newPattern, 0)){
                    XRSequencer::initNextTrackLayer();
                }
                XRSequencer::patternChecklist++;
            } else if (XRSequencer::patternChecklist == 4) {
                Serial.println("checklist index 4");
                XRSD::saveActiveTrackStepModLayerToSdCard();
                if (!XRSD::loadActiveTrackStepModLayerFromSdCard(newBank, newPattern, 0)) {
                    XRSequencer::initActiveTrackStepModLayer();
                }
                XRSequencer::patternChecklist++;
            }
        }

        XRAudio::handleHeadphones();
        XRKeyMatrix::handleStates(false);
        XRVersa::handleStates();
        XREncoder::handleStates();

        XRSequencer::handlePatternDequeueActions();
        XRSequencer::handleTrackLayerDequeueActions();
    }
}