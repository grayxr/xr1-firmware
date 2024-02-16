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
        XRAudio::handleHeadphones();
        XRKeyMatrix::handleStates(false);
        XRVersa::handleStates();
        XREncoder::handleStates();

        XRSequencer::handlePatternQueueActions();
        XRSequencer::handleTrackLayerQueueActions();
    }
}