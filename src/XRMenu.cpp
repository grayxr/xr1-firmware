#include <XRMenu.h>
#include <string>

namespace XRMenu
{
    int16_t _cursorPos = 0;

    std::string _soundMenuItems[SOUND_MENU_ITEM_MAX] = {
        "SAVE TRACK SOUND",
        "LOAD TRACK SOUND",
        "RECORD SAMPLE",
    };

    // XRUX::UX_MODE_soundMenuSubMenuModes[SOUND_MENU_ITEM_MAX] = {
    //     "SAVE TRACK SOUND",
    //     "LOAD TRACK SOUND",
    //     "Sample",
    // };

    std::string _setupMenuItems[SETUP_MENU_ITEM_MAX] = {
        "SAVE PROJECT",
        "LOAD/CREATE PROJECT",
        "CLOCK SETTINGS",
    };

    std::string *getSoundMenuItems()
    {
        return _soundMenuItems;
    }

    std::string *getSetupMenuItems()
    {
        return _setupMenuItems;
    }

    int16_t getCursorPosition()
    {
        return _cursorPos;
    }

    void setCursorPosition(int16_t idx)
    {
        _cursorPos = idx;
    }

    void incCursorPosition()
    {
        ++_cursorPos;

        _cursorPos = constrain(_cursorPos, 0, 5);
    }

    void decCursorPosition()
    {
        --_cursorPos;

        _cursorPos = constrain(_cursorPos, 0, 5);
    }

    void resetCursor()
    {
        _cursorPos = 0;
    }
}