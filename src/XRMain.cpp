#include <XRMain.h>
#include <XRHelpers.h>
#include <XRSequencer.h>
#include <XRUX.h>
#include <XRSound.h>

namespace XRMain
{
    elapsedMillis elapsedMs;
    bool ticked = false;

    bool writeBusy = false;
    bool readsDone = false;

    bool rStepOneDone = false;
    bool rStepTwoDone = false;
    bool rStepThreeDone = false;

    bool wStepOneDone = false;
    bool wStepTwoDone = false;
    bool wStepThreeDone = false;

    void handleAsyncFileIO();
    void handleRuntimeDisplayUpdates();

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
        // Serial.printf("sizeof(XRSound::nActiveSoundSet): %d\n", sizeof(XRSound::nActiveSoundSet));
        // Serial.printf("sizeof(XRSound::nActiveSoundModLayer): %d\n", sizeof(XRSound::nActiveSoundModLayer));
        // Serial.printf("sizeof(XRSequencer::nActiveTrackLayer): %d\n", sizeof(XRSequencer::nActiveTrackLayer));
        // Serial.printf("sizeof(XRSequencer::nRatchetTrackLayer): %d\n", sizeof(XRSequencer::nRatchetTrackLayer));
        // Serial.printf("sizeof(XRSequencer::nActivePatternSchema): %d\n", sizeof(XRSequencer::nActivePatternSchema));

        XRAudio::resetMetrics();
    }

    void update()
    {
        XRAudio::handleHeadphones();    
        XRKeyMatrix::handleStates(false);
        XRVersa::handleStates();
        XREncoder::handleStates();
        XRSequencer::handleTriggerStates();

        handleRuntimeDisplayUpdates();
        handleAsyncFileIO();

        XRSequencer::handlePatternQueueActions();
        //XRSequencer::handleTrackLayerQueueActions();
    }

    void handleAsyncFileIO()
    {
        if (!rStepOneDone && XRSD::loadNextPatternAsync && !XRSD::asyncFileReadComplete) {
            auto &queuedPattern = XRSequencer::getQueuedPatternState();

            if (queuedPattern.bank == -1 || queuedPattern.number == -1) {
                Serial.println("queued pattern state is invalid! cancel load...");
                XRSD::loadNextPatternAsync = false;
                rStepOneDone = true;

                XRSequencer::initIdlePattern();
            } else {
                auto res1 = XRSD::loadNextPattern(queuedPattern.bank, queuedPattern.number, true);

                if (!res1) {
                    Serial.println("next pattern probably does not exist! cancel load...");
                    XRSD::loadNextPatternAsync = false;
                    rStepOneDone = true;

                    XRSequencer::initIdlePattern();
                }
            }
        } else if (XRSD::loadNextPatternAsync && XRSD::asyncFileReadComplete) {
            XRSD::loadNextPatternAsync = false;
            XRSD::asyncFileReadComplete = false;
            rStepOneDone = true;

            Serial.println("next pattern done loading!");
        }

        // write active sounds async if dirty here

        if (rStepOneDone && !rStepTwoDone && XRSD::loadNextSoundsAsync && !XRSD::asyncFileReadComplete) {
            auto &queuedPattern = XRSequencer::getQueuedPatternState();
            if (queuedPattern.bank == -1 || queuedPattern.number == -1) {
                Serial.println("queued pattern state is invalid! cancel load...");
                XRSD::loadNextSoundsAsync = false;
                rStepTwoDone = true;
                
                XRSound::loadNextDexedInstances();

                XRSound::initIdleSounds();
            } else {
                auto res2 = XRSD::loadNextPatternSounds(queuedPattern.bank, queuedPattern.number, true);
                if (!res2) {
                    Serial.println("next sounds probably do not exist! cancel load...");
                    XRSD::loadNextSoundsAsync = false;
                    rStepTwoDone = true;
                    
                    XRSound::loadNextDexedInstances();

                    XRSound::initIdleSounds();
                }
            }
        } else if (rStepOneDone && XRSD::loadNextSoundsAsync && XRSD::asyncFileReadComplete) {
            XRSound::loadNextDexedInstances();
            
            XRSD::loadNextSoundsAsync = false;
            XRSD::asyncFileReadComplete = false;
            rStepTwoDone = true;

            Serial.println("next sounds done loading!");
        }

        // write active sound mods async if dirty here

        if (rStepOneDone && rStepTwoDone && !rStepThreeDone && XRSD::loadNextSoundModsAsync && !XRSD::asyncFileReadComplete) {
            auto &queuedPattern = XRSequencer::getQueuedPatternState();
            if (queuedPattern.bank == -1 || queuedPattern.number == -1) {
                Serial.println("queued pattern state is invalid! cancel load...");
                XRSD::loadNextSoundModsAsync = false;
                rStepThreeDone = true;

                XRSound::initIdleSoundStepMods();
            } else {
                auto res3 = XRSD::loadNextSoundModLayer(queuedPattern.bank, queuedPattern.number, 0, true);
                if (!res3) {
                    Serial.println("next sound mods probably do not exist! cancel load...");
                    XRSD::loadNextSoundModsAsync = false;
                    rStepThreeDone = true;

                    XRSound::initIdleSoundStepMods();
                }
            }
        } else if (rStepTwoDone && XRSD::loadNextSoundModsAsync && XRSD::asyncFileReadComplete) {
            XRSD::loadNextSoundModsAsync = false;
            XRSD::asyncFileReadComplete = false;
            rStepThreeDone = true;

            Serial.println("next sound mods done loading!");
        }

        if (rStepOneDone && rStepTwoDone && rStepThreeDone) {
            Serial.println("all async file io done!");

            rStepOneDone = false;
            rStepTwoDone = false;
            rStepThreeDone = false;

            readsDone = true;
        }

        // write active pattern async if dirty here
        if (readsDone && XRSD::saveActivePatternAsync && !XRSD::wAsyncActivePatternIO.complete) {
            // start async active pattern write
            if (!XRSD::wAsyncActivePatternIO.started) {
                Serial.println("starting async active pattern write...");
                Serial.printf("filename: %s, remaining: %d\n", XRSD::getActivePatternFilename().c_str(), XRSD::wAsyncActivePatternIO.remaining);

                XRSD::wAsyncActivePatternIO.started = true;
                XRSD::wAsyncActivePatternIO.complete = false;
                XRSD::wAsyncActivePatternIO.open = false;
                XRSD::wAsyncActivePatternIO.filename = XRSD::getActivePatternFilename();
                XRSD::wAsyncActivePatternIO.remaining = sizeof(XRSequencer::writePattern);
                XRSD::wAsyncActivePatternIO.size = sizeof(XRSequencer::writePattern);
            }

            yield();
            XRSD::saveActivePattern(true);
        } else if (XRSD::saveActivePatternAsync && XRSD::wAsyncActivePatternIO.complete) {
            Serial.println("finished async active pattern write...");

            // disable async active pattern write
            XRSD::saveActivePatternAsync = false;

            // reset active pattern write state
            XRSD::wAsyncActivePatternIO.started = false;
            XRSD::wAsyncActivePatternIO.complete = false;
            XRSD::wAsyncActivePatternIO.open = false;
            XRSD::wAsyncActivePatternIO.filename = "";
            XRSD::wAsyncActivePatternIO.offset = 0;
            XRSequencer::patternDirty = false;
        }
    }

    void handleRuntimeDisplayUpdates()
    {
        if (XRSequencer::getSeqState().playbackState == XRSequencer::RUNNING && XRUX::getCurrentMode() == XRUX::PATTERN_WRITE && elapsedMs >= 250)
        {
            elapsedMs -= 250;

            XRDisplay::drawSequencerScreen();
        }
    }
}