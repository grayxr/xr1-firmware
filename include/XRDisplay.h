#ifndef XRDisplay_h
#define XRDisplay_h

#include <Arduino.h>
#include <U8g2lib.h>
#include <XRConfig.h>
#include <string>

namespace XRDisplay
{
    void init();

    void drawIntro();
    void drawSetTempoOverlay();
    void drawGenericOverlayFrame(void);
    void drawCreateProjectDialog();
    void drawSaveProject();
    void drawHatchedBackground();
    void drawError(std::string message);
    void drawSequencerScreen(bool queueBlink = false);
    void drawPerformRatchetScreen();
    void drawMenuHeader(std::string inputStr, int8_t value, int8_t startX, bool hideNum);
    void drawControlMods();
    void drawPatternControlMods();
    void drawNormalControlMods();
    void drawExtendedControlMods();
    void drawControlModsForADSR(int att, int dec, float sus, int rel);
    void drawPageNumIndicators();
    void drawGenericMenuList(std::string headerStr, std::string *menuItems, int menuItemMax);
    void drawSoundMenuMain();

    std::string getDisplayNote();
}

#endif /* XRDisplay_h */