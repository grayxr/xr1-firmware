#include <XREncoder.h>
#include <XRUX.h>
#include <XRMenu.h>
#include <XRDisplay.h>
#include <XRClock.h>
#include <XRSD.h>

namespace XREncoder
{
    elapsedMillis elapsedMs;

    int addresses[5] = {
        0x36, 0x37, 0x38, 0x39, 0x40};

    int16_t currentValues[5] = {
        0, 0, 0, 0, 0};

    int16_t lastValues[5] = {
        0, 0, 0, 0, 0};

    void init()
    {
        Wire1.begin();

        for (int i = 0; i < 5; i++)
        {
            config(addresses[i], -3000, 3000, 1, 0, 0);
        }
    }

    void config(int addr, int16_t rmin, int16_t rmax, int16_t rstep, int16_t rval, uint8_t rloop)
    {
        Wire1.beginTransmission(addr);
        Wire1.write((uint8_t)(rval & 0xff));
        Wire1.write((uint8_t)(rval >> 8));
        Wire1.write(0);
        Wire1.write(rloop);
        Wire1.write((uint8_t)(rmin & 0xff));
        Wire1.write((uint8_t)(rmin >> 8));
        Wire1.write((uint8_t)(rmax & 0xff));
        Wire1.write((uint8_t)(rmax >> 8));
        Wire1.write((uint8_t)(rstep & 0xff));
        Wire1.write((uint8_t)(rstep >> 8));
        Wire1.endTransmission();
    }

    void set(int addr, int16_t rval)
    {
        Wire1.beginTransmission(addr);
        Wire1.write((uint8_t)(rval & 0xff));
        Wire1.write((uint8_t)(rval >> 8));
        Wire1.endTransmission();
    }

    void setCurrentValueForAddress(int address)
    {
        currentValues[address] = get(addresses[address]);
    }

    int16_t get(int address)
    {
        Wire1.requestFrom(address, 2);

        return ((uint16_t)Wire1.read() | ((uint16_t)Wire1.read() << 8));
    }

    int getDiff(int address)
    {
        // set current state
        currentValues[address] = get(addresses[address]);

        int diff = currentValues[address] - lastValues[address];

        // set latest state
        lastValues[address] = currentValues[address];

        return diff;
    }

    void handleStates()
    {
        auto currentUXMode = XRUX::getCurrentMode();

        bool uxModesWithMenu = (
            currentUXMode == XRUX::UX_MODE::SOUND_MENU_MAIN || 
            currentUXMode == XRUX::UX_MODE::CHANGE_SETUP  || 
            currentUXMode == XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK
        );

        if (!(elapsedMs % 25) && uxModesWithMenu) {
            if (currentUXMode == XRUX::UX_MODE::SOUND_MENU_MAIN) {
                if (handleMenuCursor(SOUND_MENU_ITEM_MAX)) {
                    XRDisplay::drawSoundMenuMain();
                }
            } else if (currentUXMode == XRUX::UX_MODE::CHANGE_SETUP) {
                if (handleMenuCursor(SETUP_MENU_ITEM_MAX)) {
                    XRDisplay::drawSetupMenu();
                }
            } else if (currentUXMode == XRUX::UX_MODE::ASSIGN_SAMPLE_TO_TRACK) {
                if (handleMenuCursor(255)) {
                    XRDisplay::drawSampleBrowser();
                }
            }
            
            return;
        }

        if (!(elapsedMs % 25) && currentUXMode == XRUX::UX_MODE::SET_TEMPO) {
            handleEncoderSetTempo();

            return;
        }

        // else if (!(elapsed % 25) && current_UI_mode == PATTERN_WRITE)
        // {
        //     handleEncoderSetPatternMods();
        // }
        // else if (!(elapsed % 25) && (current_UI_mode == TRACK_WRITE || current_UI_mode == SUBMITTING_STEP_VALUE))
        // {
        //     if (current_UI_mode == TRACK_WRITE)
        //     {
        //         handleEncoderTraversePages();
        //     }

        //     handleEncoderSetTrackMods();
        // }
    }

    void handleEncoderSetTempo()
    {
        int diff = getDiff(MAIN_ENCODER_ADDRESS);

        if (diff != 0)
        {
            float currTempo = XRClock::getTempo();
            float newTempo = currTempo + diff;

            if (!(newTempo < 30 || newTempo > 300))
            {
                if ((newTempo - currTempo >= 1) || (currTempo - newTempo) >= 1)
                {
                    uClock.setTempo(newTempo);

                    XRSD::_current_project.tempo = newTempo;

                    XRDisplay::drawSetTempoOverlay();
                }
            }
        }
    }

    bool handleMenuCursor(int menuItems)
    {
        int diff = getDiff(MAIN_ENCODER_ADDRESS);

        if (menuItems > 2 && diff != 0)
        {
            uint8_t currPos = XRMenu::getCursorPosition();
            uint8_t newPos = constrain(currPos + diff, 0, menuItems - 1);

            XRMenu::setCursorPosition(newPos);

            return true;
        }

        return false;
    }
}