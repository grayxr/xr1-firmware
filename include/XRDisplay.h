#ifndef XRDisplay_h
#define XRDisplay_h

#include <Arduino.h>
#include <U8g2lib.h>
#include <XRConfig.h>
#include <string>
#include <vector>

namespace XRDisplay
{
    void init();

    void drawIntro();
    void drawSetTempoOverlay();
    void drawGenericOverlayFrame(void);
    void drawCreateProjectDialog();
    void drawSaveProject();
    void drawModEncoderArea();
    void drawHatchedBackground();
    void drawError(std::string message);
    void drawSequencerScreen(bool queueBlink = false);
    void drawPerformRatchetScreen();
    void drawMenuHeader(std::string inputStr, int8_t value, int8_t startX, bool hideNum);
    void drawControlMods();
    void drawPatternControlMods();
    void drawNormalControlMods();
    void drawExtendedControlMods(bool useRatchetTrack = false);
    void drawSampleFileControlMods();
    void drawControlModsForADSR(int att, int dec, float sus, int rel);
    void drawPageNumIndicators();
    void drawPatternPageNumIndicators();
    void drawGenericMenuList(std::string headerStr, std::string *menuItems, int menuItemMax);
    void drawPagedMenuList(std::string headerStr, std::string *menuItems, int menuItemMax);
    void drawSoundMenuMain();
    void drawSetupMenu();
    void drawSampleBrowser();
    void drawDexedSysexBrowser();
    void drawGeneralConfirmOverlay(std::string message);
    void drawCopyConfirmOverlay(std::string type, uint8_t num);
    void drawCopySelOverlay(std::string type);
    void drawPasteConfirmOverlay(std::string type, uint8_t num);
    void drawStraightDashedLine(int startX, int endX, int yPos);
    void drawStepMicrotimingOverlay(std::string value);
    void toggleMetronomeDirection();
    void drawRatchetPageNumIndicators();
    void drawTrackSoundName();

    std::string getDisplayNote();
    std::string getNumberBufferedStr(std::string inputStr, int8_t value, bool hideNum);
}

#endif /* XRDisplay_h */