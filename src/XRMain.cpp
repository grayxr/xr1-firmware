#include <XRMain.h>
#include <XRHelpers.h>
#include <XRSequencer.h>

namespace XRMain
{
    elapsedMillis elapsedMs;
    
    void boot()
    {
        XRDisplay::init();

        if (!XRSD::init()) {
            XRDisplay::drawError("SD CARD ERROR!");
            return;
        }

        XRAudio::init();
        XREncoder::init();
        XRCV::init();
        XRLED::init();

        if (!XRVersa::init()) {
            XRDisplay::drawError("KEYBOARD ERROR!");
            return;
        }

        XRKeyMatrix::init();
        XRKeyMatrix::handleStates(true); // skip any false reads on boot

        XRMIDI::init();
        XRClock::init();

        XRDisplay::drawIntro();

        delay(1000);

        bool loadSuccess = XRSD::loadMachineState();
        if (!loadSuccess) {
            XRUX::setCurrentMode(XRUX::UX_MODE::PROJECT_INITIALIZE);
            XRDisplay::drawCreateProjectDialog();
        } else {
            XRUX::setCurrentMode(XRUX::UX_MODE::PROJECT_BUSY);

            bool loadProjSuccess = XRSD::loadLastProject();
            if (!loadProjSuccess) {
                XRDisplay::drawError("ERROR LOADING PROJECT!");
                return;
            }

            XRUX::setCurrentMode(XRUX::UX_MODE::PATTERN_WRITE);
            XRSequencer::init();
            
            //XRAudio::loadVoiceSettings();
        }

        XRAudio::resetMetrics();
    }

    void update()
    {
        XRAudio::handleHeadphones();
        XRKeyMatrix::handleStates();
        XRVersa::handleStates();
        XREncoder::handleStates();
        // handleQueueActions();
    }
}