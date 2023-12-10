#include <XRUX.h>

namespace XRUX
{
    UX_MODE _previous_UX_mode = PROJECT_BUSY;
    UX_MODE _current_UX_mode = PROJECT_BUSY;

    void setCurrentMode(UX_MODE mode)
    {
        setPreviousMode(_current_UX_mode);

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