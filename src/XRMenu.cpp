#include <XRMenu.h>
#include <string>

namespace XRMenu
{
    int16_t _cursorPos = 0;

    std::string _soundMenuItems[SOUND_MENU_ITEM_MAX] = {
        "save track sound",
        "load track sound",
        "record sample",
    };

    std::string _dexedSoundMenuItems[DEXED_SOUND_MENU_ITEM_MAX] = {
        "save track sound",
        "load track sound",
        "record sample",
        "browse dexed sysex",
    };

    std::string _setupMenuItems[SETUP_MENU_ITEM_MAX] = {
        "save project",
        "load/create project",
        "clock settings",
    };

    std::string *getSoundMenuItems()
    {
        return _soundMenuItems;
    }

    std::string *getDexedSoundMenuItems()
    {
        return _dexedSoundMenuItems;
    }

    std::string getSoundMenuItemsString()
    {
        return "record sample\nsave track sound\nload track sound";
    }

    std::string getDexedSoundMenuItemsString()
    {
        return "\nbrowse dexed sysex";
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