#include <XRMain.h>
#include <XRHelpers.h>
#include <XRSequencer.h>

namespace XRMain
{
    elapsedMillis elapsedMs;

    void boot()
    {
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
        XRSound::init();

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

                // TODO: impl dialog to create new project

                return;
            }

            XRUX::setCurrentMode(XRUX::UX_MODE::PATTERN_WRITE);

            XRSequencer::init();
            
            XRSound::loadVoiceSettings();
            XRSound::initTrackSounds();
        }

        XRAudio::resetMetrics();
    }

    void update()
    {
        XRAudio::handleHeadphones();
        XRKeyMatrix::handleStates();
        XRVersa::handleStates();
        XREncoder::handleStates();
        XRSequencer::handleQueueActions();
    }
}