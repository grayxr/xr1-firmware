#include <XRUX.h>
#include <XRSequencer.h>

namespace XRUX
{
    UX_MODE _previous_UX_mode = PROJECT_BUSY;
    UX_MODE _current_UX_mode = PROJECT_BUSY;

    void setCurrentMode(UX_MODE mode)
    {
        setPreviousMode(_current_UX_mode);

        // when switching to a new mode, always disable auto fill state
        if (XRSequencer::fillState.fillType == XRSequencer::FILL_TYPE::AUTO && mode != UX_MODE::PERFORM_FILL_CHAIN) {
            XRSequencer::fillState.fillType = XRSequencer::FILL_TYPE::MANUAL;
            XRSequencer::fillState.currentStep = 1;
            XRSequencer::fillState.currentMeasure = 1;
        }

        _current_UX_mode = mode;
    }

    void setPreviousMode(UX_MODE mode)
    {
        _previous_UX_mode = mode;
    }

    UX_MODE getCurrentMode()
    {
        return _current_UX_mode;
    }

    UX_MODE getPreviousMode()
    {
        return _previous_UX_mode;
    }
}