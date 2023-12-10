#include <XRMenu.h>
#include <string>

namespace XRMenu
{
    uint8_t _cursorPos = 0;

    std::string _soundMenuItems[SOUND_MENU_ITEM_MAX] = {
        "ASSIGN SOUND TO TRACK",
        "SAVE TRACK SOUND",
        "LOAD TRACK SOUND"};

    std::string *getSoundMenuItems()
    {
        return _soundMenuItems;
    }

    uint8_t getCursorPosition()
    {
        return _cursorPos;
    }

    void setCursorPosition(uint8_t idx)
    {
        _cursorPos = idx;
    }

    void resetCursor()
    {
        _cursorPos = 0;
    }
}