#include <XRDisplay.h>
#include <XRMenu.h>
#include <XRSequencer.h>
#include <XRUX.h>
#include <XRClock.h>
#include <XRFonts.h>
#include <XRKeyInput.h>
#include <XRSD.h>
#include <XRHelpers.h>
#include <string>
#include <map>

namespace XRDisplay
{
    //U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/13, /* data=*/11, /* cs=*/10, /* dc=*/14, /* reset=*/15);
    U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/10, /* dc=*/14, /* reset=*/15);

    #define XR_DISP_SHIFT_Y 1

    bool metronomeSwing = false;

    void init()
    {
        //u8g2.setBusClock(8000000);
        u8g2.begin();
        u8g2.setContrast(0);
        u8g2.setFont(small_font);
        u8g2.setFontRefHeightAll();
        u8g2.setDrawColor(1);
        u8g2.setFontPosTop();
        u8g2.setFontDirection(0);
    }

    void drawIntro()
    {
        u8g2.clearBuffer();

        int boxWidth = 60;
        int boxHeight = 30;
        u8g2.setColorIndex((u_int8_t)0);
        u8g2.drawBox((DISPLAY_MAX_WIDTH / 2) - (boxWidth / 2), (DISPLAY_MAX_HEIGHT / 2) - (boxHeight / 2) - 16, boxWidth, boxHeight);
        u8g2.setColorIndex((u_int8_t)1);
        u8g2.setFont(bitocra13_c); // u8g2_font_8x13_mr
        u8g2.drawStr(52, 8, DEVICE_NAME);

        int fBoxWidth = 60;
        int fBoxHeight = 10;
        u8g2.setColorIndex((u_int8_t)0);
        u8g2.drawBox((DISPLAY_MAX_WIDTH / 2) - (fBoxWidth / 2), 28, fBoxWidth, fBoxHeight);
        u8g2.setColorIndex((u_int8_t)1);
        u8g2.setFont(small_font); // u8g2_font_6x10_tf
        u8g2.setFontRefHeightExtendedText();
        u8g2.drawStr(38, 29, COMPANY_NAME);

        u8g2.setColorIndex((u_int8_t)0);
        u8g2.drawBox((DISPLAY_MAX_WIDTH / 2) - (36 / 2), 45, 36, fBoxHeight);
        u8g2.setColorIndex((u_int8_t)1);
        u8g2.setFont(small_font); // u8g2_font_6x10_tf
        u8g2.setFontRefHeightExtendedText();

        std::string versionStr = "v";
        versionStr += FIRMWARE_VERSION;
        u8g2.drawStr(52, 47, versionStr.c_str());

        u8g2.sendBuffer();

        u8g2.setFont(small_font); // u8g2_font_6x10_tf
        u8g2.setFontRefHeightExtendedText();
    }

    void drawSetTempoOverlay()
    {
        Serial.println("enter drawSetTempoOverlay!");

        drawGenericOverlayFrame();

        std::string tempoStr = "SET TEMPO";
        u8g2.drawStr(44, 6, tempoStr.c_str());
        u8g2.setFont(bitocra13_c);

        int tempoVal = (int)XRClock::getTempo();
        std::string tempoValStr = std::to_string(tempoVal);
        int tempoValX = 52;

        u8g2.drawStr(tempoValX + (tempoVal > 99 ? 0 : 4), 31, tempoValStr.c_str());
        u8g2.setFont(small_font);

        u8g2.sendBuffer();
    }

    void drawGenericOverlayFrame(void)
    {
        int width = 124;
        int height = 60;

        u8g2.setColorIndex((u_int8_t)0);
        u8g2.drawBox(2, 2, width, height);
        u8g2.setColorIndex((u_int8_t)1);
        u8g2.drawFrame(2, 2, width, height);
        u8g2.drawLine(2, 14, width, 14);
    }

    void drawKeyboard(int verticalOffset)
    {
        // draw keyboard
        u8g2.drawFrame(9, 28 + verticalOffset, 110, 24);
        u8g2.drawLine(25, 40 + verticalOffset, 25, 51 + verticalOffset);
        u8g2.drawFrame(19, 28 + verticalOffset, 13, 12);
        u8g2.drawLine(37, 40 + verticalOffset, 37, 51 + verticalOffset);
        u8g2.drawLine(51, 28 + verticalOffset, 51, 51 + verticalOffset);
        u8g2.drawFrame(31, 28 + verticalOffset, 12, 12);
        u8g2.drawLine(65, 40 + verticalOffset, 65, 51 + verticalOffset);
        u8g2.drawFrame(59, 28 + verticalOffset, 13, 12);
        u8g2.drawLine(77, 40 + verticalOffset, 77, 51 + verticalOffset);
        u8g2.drawFrame(71, 28 + verticalOffset, 13, 12);
        u8g2.drawLine(89, 40 + verticalOffset, 89, 51 + verticalOffset);
        u8g2.drawFrame(83, 28 + verticalOffset, 13, 12);
        u8g2.drawLine(104, 28 + verticalOffset, 104, 51 + verticalOffset);
    }

    void drawCreateProjectDialog()
    {
        u8g2.clearBuffer();

        drawGenericOverlayFrame();

        std::string headerStr = "new proj.";
        u8g2.drawStr(8, 5, headerStr.c_str());

        std::string projectNameStr = XRKeyInput::get();

        u8g2.drawStr(50, 5, "esc back  sel save");
        u8g2.drawFrame(48, 4, 15, 9);
        u8g2.drawFrame(88, 4, 15, 9);

        // draw actual project name
        u8g2.setCursor(9, 20);
        u8g2.print(projectNameStr.c_str());

        // draw cursor
        int cursorStartX = u8g2.getCursorX();
        int cursorTopY = 20;
        int cursorBottomY = 26;
        u8g2.drawLine(cursorStartX, cursorTopY, cursorStartX, cursorBottomY);

        // draw dashed baseline
        // TODO: extract to function
        int dashedLineStartX = 7;
        int dashedLineWidthX = 1;
        int lastDashEndX = 0;
        for (int d = 0; d < 37; d++)
        {
            int currDashX = lastDashEndX + (d == 0 ? dashedLineStartX : 0) + 2;
            int nextDashEndX = currDashX + dashedLineWidthX;
            u8g2.drawLine(currDashX, 28, nextDashEndX, 28);
            lastDashEndX = nextDashEndX;
        }

        drawKeyboard(5);

        // draw prev/next key group arrows
        u8g2.drawTriangle(23, 38, 27, 34, 27, 42); // left arrow
        u8g2.drawTriangle(39, 38, 35, 34, 35, 42); // right arrow

        // draw key group indicators
        u8g2.drawStr(63, 35, "a");
        u8g2.drawStr(76, 35, "1");
        u8g2.drawStr(88, 35, "!");

        int keyStartIdx = 0;
        int visibleKeyLimit = 8;
        for (int k = keyStartIdx; k < visibleKeyLimit; k++)
        {
            int space = k * 13;

            u8g2.drawStr(16 + space, 47, XRKeyInput::getKeyForCurrentRow(k).c_str());
        }

        u8g2.sendBuffer();
    }

    void drawSaveProject()
    {
        int creatingBoxWidth = 50;
        int creatingBoxHeight = 20;
        int creatingBoxStartX = (DISPLAY_MAX_WIDTH / 2) - (creatingBoxWidth / 2);
        int creatingBoxStartY = (DISPLAY_MAX_HEIGHT / 2) - (creatingBoxHeight / 2);
        int creatingBoxMsgStartX = creatingBoxStartX + 8;
        int creatingBoxMsgStartY = creatingBoxStartY + 6;

        drawHatchedBackground();

        u8g2.setColorIndex((u_int8_t)0);
        u8g2.drawBox(creatingBoxStartX - 3, creatingBoxStartY - 3, creatingBoxWidth + 6, creatingBoxHeight + 6);
        u8g2.setColorIndex((u_int8_t)1);

        // show project creation status indicator?
        u8g2.drawFrame(creatingBoxStartX, creatingBoxStartY, creatingBoxWidth, creatingBoxHeight);
        u8g2.drawStr(creatingBoxMsgStartX, creatingBoxMsgStartY, "SAVING!");

        u8g2.sendBuffer();
    }

    void drawError(std::string message)
    {
        u8g2.clearBuffer();

        drawGenericOverlayFrame();

        u8g2.drawStr(37, 6, "ERROR!");
        u8g2.drawStr(10, 22, message.c_str());

        u8g2.sendBuffer();
    }

    void drawStepMicrotimingOverlay(std::string value)
    {
        drawGenericOverlayFrame();

        u8g2.drawStr(32, 5, "step microtiming");

        int valPosX = DISPLAY_MAX_WIDTH / 2;
        valPosX -= (value.length() > 0 ? u8g2.getStrWidth(value.c_str()) / 2 : 0);

        u8g2.setFont(bitocra13_c);
        u8g2.drawStr(valPosX, 30, value.c_str());
        u8g2.setFont(small_font);

        u8g2.sendBuffer();
    }

    void drawHatchedBackground()
    {
        int leftBoundX = 0;
        // int rightBoundX = DISPLAY_MAX_WIDTH;
        int topBoundX = 0;
        // int bottomBoundX = DISPLAY_MAX_HEIGHT;

        int pixelRows = DISPLAY_MAX_HEIGHT;
        int pixelCols = DISPLAY_MAX_WIDTH;

        // initialize row template
        int t[DISPLAY_MAX_WIDTH];
        int spacing = 4;

        // fill up template
        for (int i = 0; i < DISPLAY_MAX_WIDTH; i++)
        {
            if (i == 0 || (i % spacing == 0))
            {
                t[i] = 1;
            }
            else
            {
                t[i] = 0;
            }
        }

        for (int r = 0; r < pixelRows; r++)
        {
            if (r != 0)
            {
                // Rotate the given array one time toward right
                for (int i = 0; i < 1; i++)
                {
                    int j;
                    int last;

                    // Stores the last element of array
                    last = t[DISPLAY_MAX_WIDTH - 1];

                    for (j = DISPLAY_MAX_WIDTH - 1; j > 0; j--)
                    {
                        // Shift element of array by one
                        t[j] = t[j - 1];
                    }
                    // Last element of array will be added to the start of array.
                    t[0] = last;
                }
            }

            for (int c = 0; c < pixelCols; c++)
            {
                if (t[c] == 1)
                {
                    u8g2.drawPixel(leftBoundX + c, topBoundX + r);
                }
            }
        }
    }

    void drawStraightDashedLine(int startX, int endX, int yPos)
    {
        int width = (endX - startX);

        int spacing = 2;

        // fill up template
        for (int i = 0; i < width; i++)
        {
            if (i == 0 || (i % spacing == 0))
            {
                u8g2.drawPixel(startX + i, yPos);
            }
        }
    }

    void drawModEncoderArea()
    {
        // mod encoder area
        u8g2.drawFrame(4, XR_DISP_SHIFT_Y+24, 121, 27);
        u8g2.drawLine(5, XR_DISP_SHIFT_Y+34, 123, XR_DISP_SHIFT_Y+34);
        u8g2.drawLine(33, XR_DISP_SHIFT_Y+24, 33, XR_DISP_SHIFT_Y+50);
        u8g2.drawLine(64, XR_DISP_SHIFT_Y+24, 64, XR_DISP_SHIFT_Y+50);
        u8g2.drawLine(94, XR_DISP_SHIFT_Y+24, 94, XR_DISP_SHIFT_Y+50);
    }
    
    void drawSequencerScreen(bool queueBlink)
    {
        //Serial.println("enter drawSequencerScreen!");

        u8g2.clearBuffer();

        auto currentUXMode = XRUX::getCurrentMode();

        if (currentUXMode == XRUX::PERFORM_TAP)
        {
            u8g2.drawStr(0, 0, "tap mode");
            u8g2.drawLine(0, 10, DISPLAY_MAX_WIDTH, 10);

            u8g2.drawStr(27, 15, "tap in track layer");

            // left trk icon
            u8g2.drawLine(20, 27, 39, 27);
            u8g2.drawLine(39, 27, 39, 55);
            u8g2.drawLine(39, 55, 20, 55);
            u8g2.drawLine(20, 31, 34, 31);
            u8g2.drawLine(34, 31, 34, 50);
            u8g2.drawLine(34, 50, 20, 50);
            u8g2.drawBox(20, 34, 8, 3);

            // middle trk icon
            u8g2.drawFrame(49, 27, 30, 28);
            u8g2.drawFrame(54, 31, 20, 19);
            u8g2.drawBox(60, 34, 8, 3);

            // right trk icon
            u8g2.drawLine(107, 27, 88, 27);
            u8g2.drawLine(88, 27, 88, 55);
            u8g2.drawLine(88, 55, 107, 55);
            u8g2.drawLine(107, 31, 93, 31);
            u8g2.drawLine(93, 31, 93, 50);
            u8g2.drawLine(93, 50, 107, 50);
            u8g2.drawBox(100, 34, 8, 3);

            u8g2.sendBuffer();

            return;
        }
        else if (currentUXMode == XRUX::PERFORM_MUTE)
        {
            u8g2.drawStr(0, 0, "mute mode");
            u8g2.drawLine(0, 10, DISPLAY_MAX_WIDTH, 10);

            u8g2.drawStr(23, 15, "select tracks to mute");

            // left trk icon
            u8g2.drawLine(20, 27, 39, 27);
            u8g2.drawLine(39, 27, 39, 55);
            u8g2.drawLine(39, 55, 20, 55);
            u8g2.drawLine(20, 31, 34, 31);
            u8g2.drawLine(34, 31, 34, 50);
            u8g2.drawLine(34, 50, 20, 50);
            u8g2.drawBox(20, 34, 8, 3);

            // middle trk icon
            u8g2.drawFrame(49, 27, 30, 28);
            u8g2.drawFrame(54, 31, 20, 19);
            u8g2.drawBox(60, 34, 8, 3);

            // right trk icon
            u8g2.drawLine(107, 27, 88, 27);
            u8g2.drawLine(88, 27, 88, 55);
            u8g2.drawLine(88, 55, 107, 55);
            u8g2.drawLine(107, 31, 93, 31);
            u8g2.drawLine(93, 31, 93, 50);
            u8g2.drawLine(93, 50, 107, 50);
            u8g2.drawBox(100, 34, 8, 3);

            u8g2.sendBuffer();

            return;
        }
        else if (currentUXMode == XRUX::PERFORM_SOLO)
        {
            u8g2.drawStr(0, 0, "solo mode");
            u8g2.drawLine(0, 10, DISPLAY_MAX_WIDTH, 10);

            u8g2.drawStr(23, 15, "select tracks to solo");

            // left trk icon
            u8g2.drawLine(20, 27, 39, 27);
            u8g2.drawLine(39, 27, 39, 55);
            u8g2.drawLine(39, 55, 20, 55);
            u8g2.drawLine(20, 31, 34, 31);
            u8g2.drawLine(34, 31, 34, 50);
            u8g2.drawLine(34, 50, 20, 50);

            u8g2.drawLine(20, 34, 28, 34);
            u8g2.drawLine(28, 34, 28, 37);
            u8g2.drawLine(28, 37, 20, 37);

            // middle trk icon
            u8g2.drawFrame(49, 27, 30, 28);
            u8g2.drawFrame(54, 31, 20, 19);
            u8g2.drawBox(60, 34, 8, 3);

            // right trk icon
            u8g2.drawLine(107, 27, 88, 27);
            u8g2.drawLine(88, 27, 88, 55);
            u8g2.drawLine(88, 55, 107, 55);
            u8g2.drawLine(107, 31, 93, 31);
            u8g2.drawLine(93, 31, 93, 50);
            u8g2.drawLine(93, 50, 107, 50);

            u8g2.drawLine(107, 34, 100, 34);
            u8g2.drawLine(100, 34, 100, 37);
            u8g2.drawLine(100, 37, 107, 37);

            u8g2.sendBuffer();
            return;
        }
        else if (currentUXMode == XRUX::PERFORM_RATCHET || currentUXMode == XRUX::SUBMITTING_RATCHET_STEP_VALUE)
        {
            auto ratchetTrack = XRSequencer::getRatchetTrack();

            // main header area
            u8g2.setFont(large_font);

            //auto headerStr = getNumberBufferedStr("track", currentSelectedTrack + 1, false);
            auto headerStr = "track" + std::to_string(ratchetTrack + 1);
            u8g2.drawStr(4, XR_DISP_SHIFT_Y, headerStr.c_str());

            u8g2.setFont(small_font);

            drawPerformRatchetScreen();

            if (XRSequencer::onRatchetStepPage() || XRUX::getCurrentMode() == XRUX::SUBMITTING_RATCHET_STEP_VALUE) {
                drawTrackSoundName();
            }

            std::string performStr = "perform: ratchet";
            u8g2.drawStr(62, XR_DISP_SHIFT_Y+4, performStr.c_str());

            u8g2.sendBuffer();

            return;
        }

        auto currentSelectedBank = XRSequencer::getCurrentSelectedBankNum();
        auto currentSelectedPattern = XRSequencer::getCurrentSelectedPatternNum();
        auto currentSelectedTrack = XRSequencer::getCurrentSelectedTrackNum();
        auto &queuedPattern = XRSequencer::getQueuedPatternState();

        if (currentUXMode == XRUX::PATTERN_WRITE || currentUXMode == XRUX::PATTERN_SEL || currentUXMode == XRUX::PATTERN_CHANGE_QUEUED)
        {
            // main header area
            u8g2.setFont(large_font);

            bool ptnBlink = false;
            int ptnNumber = currentSelectedPattern + 1;
            if (queuedPattern.number > -1)
            {
                ptnNumber = queuedPattern.number + 1;
                ptnBlink = (queuedPattern.number != currentSelectedPattern);
            }

            //auto headerStr = getNumberBufferedStr("pattern", currentSelectedPattern + 1, false);
            auto headerStr = "pattern" + ((queueBlink && ptnBlink) ? "" : std::to_string(ptnNumber));
            u8g2.drawStr(4, XR_DISP_SHIFT_Y, headerStr.c_str());

            // timecode / track info area
            u8g2.setFont(small_font);

            std::string runtimeStr = XRClock::getClockTimeString();
            u8g2.drawStr(4, XR_DISP_SHIFT_Y+14, runtimeStr.c_str());
        }
        else if (currentUXMode == XRUX::TRACK_WRITE || currentUXMode == XRUX::TRACK_SEL || currentUXMode == XRUX::SUBMITTING_STEP_VALUE)
        {
            // main header area
            u8g2.setFont(large_font);

            //auto headerStr = getNumberBufferedStr("track", currentSelectedTrack + 1, false);
            auto headerStr = "track" + std::to_string(currentSelectedTrack + 1);
            u8g2.drawStr(4, XR_DISP_SHIFT_Y, headerStr.c_str());
        }

        u8g2.setFont(small_font);

        // legend area

        auto currTrackLayerNum = XRSequencer::getCurrentSelectedTrackLayerNum();
        // auto lStr = getNumberBufferedStr("layer", currTrackLayerNum+1, false);
        auto lStr = "layer" + std::to_string(currTrackLayerNum + 1);

        bool bnkBlink = false;
        int bnkNumber = currentSelectedBank + 1;
        if (queuedPattern.bank > -1)
        {
            bnkNumber = queuedPattern.bank + 1;
            bnkBlink = (queuedPattern.bank != currentSelectedBank);
        }
        //auto bStr = getNumberBufferedStr("bank", bnkNumber, (queueBlink && bnkBlink));
        auto bStr = "bank" + ((queueBlink && bnkBlink) ? "" : std::to_string(bnkNumber));

        u8g2.drawStr(70, XR_DISP_SHIFT_Y+4, lStr.c_str());
        u8g2.drawStr(74, XR_DISP_SHIFT_Y+14, bStr.c_str());

        // if (currentUXMode == XRUX::TRACK_WRITE || currentUXMode == XRUX::TRACK_SEL || currentUXMode == XRUX::SUBMITTING_STEP_VALUE)
        // {
        //     bool ptnBlink = false;
        //     int ptnNumber = currentSelectedPattern + 1;
        //     if (queuedPattern.number > -1)
        //     {
        //         ptnNumber = queuedPattern.number + 1;
        //         ptnBlink = (queuedPattern.number != currentSelectedPattern);
        //     }

        //     std::string pStr = "(";
        //     pStr += getNumberBufferedStr("p", ptnNumber, (queueBlink && ptnBlink));
        //     pStr += ")";

        //     u8g2.drawStr(55, XR_DISP_SHIFT_Y+4, pStr.c_str());
        // }

        // metronome indicator area
        
        // triangle
        int xShift = 8;
        int middleX = xShift+98;
        u8g2.drawLine(xShift+95, XR_DISP_SHIFT_Y+9, middleX, XR_DISP_SHIFT_Y+5);
        u8g2.drawLine(middleX, XR_DISP_SHIFT_Y+5, xShift+101, XR_DISP_SHIFT_Y+9);
        u8g2.drawLine(xShift+95, XR_DISP_SHIFT_Y+9, xShift+101, XR_DISP_SHIFT_Y+9);
        // middle stick
        auto middleStickTopX = metronomeSwing ? 95 : 101;
        u8g2.drawLine(middleX, XR_DISP_SHIFT_Y+9, xShift+middleStickTopX, XR_DISP_SHIFT_Y+6);

        // bpm area

        uint8_t currentBpm = (uint8_t)XRClock::getTempo();
        std::string currentBpmStr = XRHelpers::strldz(std::to_string(currentBpm), 3);
        u8g2.drawStr(113, XR_DISP_SHIFT_Y+4, currentBpmStr.c_str());

        if (currentUXMode == XRUX::TRACK_WRITE || currentUXMode == XRUX::TRACK_SEL || currentUXMode == XRUX::SUBMITTING_STEP_VALUE)
        {
            // note indicator area
            auto dNote = getDisplayNote();
            int xNoteShift = dNote.length() > 2 ? -4 : 0;

            u8g2.drawStr(xNoteShift+117, XR_DISP_SHIFT_Y+14, getDisplayNote().c_str());
        }

        if (currentUXMode == XRUX::PATTERN_WRITE || currentUXMode == XRUX::PATTERN_SEL || currentUXMode == XRUX::PATTERN_CHANGE_QUEUED)
        {
            drawModEncoderArea();

            // draw control mod area
            drawPatternControlMods();
            drawPatternPageNumIndicators();
        }
        else if (
            currentUXMode == XRUX::TRACK_WRITE ||
            currentUXMode == XRUX::TRACK_SEL ||
            currentUXMode == XRUX::SUBMITTING_STEP_VALUE ||
            currentUXMode == XRUX::SUBMITTING_RATCHET_STEP_VALUE)
        {
            // track info area
            drawTrackSoundName();

            // draw control mod area
            drawControlMods();
            drawPageNumIndicators();
        }

        u8g2.sendBuffer();
    }

    void drawTrackSoundName()
    {
        // track info area
        auto currTrackNum = (XRSequencer::onRatchetStepPage() || XRUX::getCurrentMode() == XRUX::SUBMITTING_RATCHET_STEP_VALUE) ? 
            XRSequencer::getRatchetTrack() : 
            XRSequencer::getCurrentSelectedTrackNum();
            
        auto currSoundForTrack = XRSound::activeSounds[currTrackNum];

        // draw track meta type box
        int trackMetaStrX = 3;

        std::string trackMetaStr = XRSound::getSoundMetaStr(currSoundForTrack.type);
        std::string trackInfoStr;

        if (
            currSoundForTrack.type == XRSound::T_MONO_SAMPLE ||
            currSoundForTrack.type == XRSound::T_MONO_SYNTH ||
            currSoundForTrack.type == XRSound::T_DEXED_SYNTH ||
            currSoundForTrack.type == XRSound::T_FM_DRUM
        ){
            std::string soundName(currSoundForTrack.name);

            trackInfoStr += soundName.length() > 0 ? soundName : "init";
        }
        else if (
            currSoundForTrack.type == XRSound::T_MIDI || 
            currSoundForTrack.type == XRSound::T_CV_GATE || 
            currSoundForTrack.type == XRSound::T_CV_TRIG
        ){
            trackInfoStr += "";
        }

        u8g2.drawStr(4, XR_DISP_SHIFT_Y+14, trackMetaStr.c_str());

        if (currSoundForTrack.type != XRSound::T_EMPTY) {
            u8g2.drawTriangle(19,XR_DISP_SHIFT_Y+14,19,XR_DISP_SHIFT_Y+20,22,XR_DISP_SHIFT_Y+17);
            u8g2.drawStr(25, XR_DISP_SHIFT_Y+14, trackInfoStr.c_str());
        }
    }

    void drawPerformRatchetScreen()
    {
        auto ratchetTrack = XRSequencer::getRatchetTrack();
        auto ratchetDivision = XRSequencer::getRatchetDivision();
        auto currRatchetPageNum = XRSequencer::getCurrentRatchetPageNum();

        // u8g2.drawStr(4, 0, "perform: ratchet/arp");
        // u8g2.drawLine(4, 8, 122, 8);

        // std::string rchTrkStr = "trk:";
        // rchTrkStr += XRHelpers::strldz(std::to_string(ratchetTrack + 1), 2);
        // u8g2.drawStr(100, 0, rchTrkStr.c_str());

        if (currRatchetPageNum == 0) {
            // draw keyboard
            drawKeyboard(-7);

            if (ratchetDivision > 0)
            {
                // ratchet_division = 24 = 96/24 = 4 = 1/4
                uint8_t divDenominator = (96 / ratchetDivision);

                std::string divStr = "1/";
                divStr += std::to_string(divDenominator);

                int xMult = 14;
                int xMultB = 13;
                int yShift = -7;

                // even divisions (bottom of keyboard)
                if (ratchetDivision == 24)
                { // 1/4
                    u8g2.drawTriangle(14, yShift+60, 17, yShift+55, 20, yShift+60);
                    u8g2.drawStr(26, yShift+54, divStr.c_str());
                }
                else if (ratchetDivision == 12)
                { // 1/8
                    u8g2.drawTriangle(14 + (xMult * 1), yShift+60, 17 + (xMult * 1), yShift+55, 20 + (xMult * 1), yShift+60);
                    u8g2.drawStr(26 + (xMult * 1), yShift+54, divStr.c_str());
                }
                else if (ratchetDivision == 6)
                { // 1/16
                    u8g2.drawTriangle(14 + (xMult * 2), yShift+60, 17 + (xMult * 2), yShift+55, 20 + (xMult * 2), yShift+60);
                    u8g2.drawStr(26 + (xMult * 2), yShift+54, divStr.c_str());
                }
                else if (ratchetDivision == 3)
                { // 1/32
                    u8g2.drawTriangle(14 + (xMult * 3), yShift+60, 17 + (xMult * 3), yShift+55, 20 + (xMult * 3), yShift+60);
                    u8g2.drawStr(26 + (xMult * 3), yShift+54, divStr.c_str());
                }
                else if (ratchetDivision == 2)
                { // 1/48
                    u8g2.drawTriangle(14 + (xMult * 4), yShift+60, 17 + (xMult * 4), yShift+55, 20 + (xMult * 4), yShift+60);
                    u8g2.drawStr(26 + (xMult * 4), yShift+54, divStr.c_str());
                }
                else if (ratchetDivision == 1)
                { // 1/96
                    u8g2.drawTriangle(14 + (xMult * 5), yShift+60, 17 + (xMult * 5), yShift+55, 20 + (xMult * 5), yShift+60);
                    u8g2.drawStr(26 + (xMult * 5), yShift+54, divStr.c_str());
                }
                // odd divisions (top of keyboard)
                else if (ratchetDivision == 16)
                { // 1/6
                    u8g2.drawTriangle(22, yShift+21, 24, yShift+25, 27, yShift+21);
                    u8g2.drawStr(33, yShift+19, divStr.c_str());
                }
                else if (ratchetDivision == 8)
                { // 1/12
                    u8g2.drawTriangle(22 + (xMultB * 1), yShift+21, 24 + (xMultB * 1), yShift+25, 27 + (xMultB * 1), yShift+21);
                    u8g2.drawStr(33 + (xMultB * 1), yShift+19, divStr.c_str());
                }
                else if (ratchetDivision == 4)
                { // 1/24
                    u8g2.drawTriangle(22 + (xMultB * 3), yShift+21, 24 + (xMultB * 3), yShift+25, 27 + (xMultB * 3), yShift+21);
                    u8g2.drawStr(33 + (xMultB * 3), yShift+19, divStr.c_str());
                }
            }
        } else {
            drawExtendedControlMods(true);
            drawModEncoderArea();
        }

        drawRatchetPageNumIndicators();

        return;
    }

    void drawRatchetPageNumIndicators()
    {
        auto currRatchetPageNum = XRSequencer::getCurrentRatchetPageNum();
        auto ratchetPageCount = 2;

        std::string ratchetPageNameMap[ratchetPageCount] = {
            "ratchet",
            "arp"
        };

        auto currPageNameForTrack = ratchetPageNameMap[currRatchetPageNum];
        auto pageTabPosY = XR_DISP_SHIFT_Y + 53;

        u8g2.drawStr(4, pageTabPosY, currPageNameForTrack.c_str());

        int pageNumBasedStartX = 128;
        int pageNumStartX = pageNumBasedStartX - 9;
        int pageBetweenPaddingX = 9;

        auto multX = ratchetPageCount - currRatchetPageNum;
        auto pageBoxStartX = 128 - (multX * pageBetweenPaddingX);

        u8g2.drawBox(pageBoxStartX, 54, 7, 7);

        for (int l = 0; l < ratchetPageCount; l++)
        {
            auto distMultX = ratchetPageCount - (l+1);

            if (l == currRatchetPageNum)
            {
                u8g2.setColorIndex((u_int8_t)0);
                u8g2.drawStr(pageNumStartX + 2 - (pageBetweenPaddingX * distMultX), pageTabPosY, std::to_string(l + 1).c_str());
                u8g2.setColorIndex((u_int8_t)1);
            }
            else
            {
                u8g2.drawStr(pageNumStartX + 2 - (pageBetweenPaddingX * distMultX), pageTabPosY, std::to_string(l + 1).c_str());
            }
        }
    }

    void drawMenuHeader(std::string inputStr, int8_t value, int8_t startX, bool hideNum)
    {
        inputStr = getNumberBufferedStr(inputStr, value, hideNum);

        u8g2.drawStr(startX, 0, inputStr.c_str());
    }

    void toggleMetronomeDirection()
    {
        metronomeSwing = !metronomeSwing;
    }

    std::string getNumberBufferedStr(std::string inputStr, int8_t value, bool hideNum)
    {
        if (!hideNum)
            inputStr += XRHelpers::strldz(std::to_string(value), 2);

        return inputStr;
    }

    void drawPatternControlMods()
    {
        int ctrlModHeaderY = XR_DISP_SHIFT_Y+25;
        int ctrlModHeaderStartX = 4;
        int ctrlModSpaceWidth = (122 / 4);
        int ctrlModHeaderStartCenteredX = (122 / 4) / 2;

        auto mods = XRSound::getControlModDataForPattern();

        int aPosX = ctrlModHeaderStartCenteredX + 3;
        aPosX -= (mods.aName.length() > 0 ? u8g2.getStrWidth(mods.aName.c_str()) / 2 : 0);

        int bPosX = ctrlModHeaderStartCenteredX + 3 + (ctrlModSpaceWidth * 1);
        bPosX -= (mods.bName.length() > 0 ? u8g2.getStrWidth(mods.bName.c_str()) / 2 : 0);

        int cPosX = ctrlModHeaderStartCenteredX + 4 + (ctrlModSpaceWidth * 2);
        cPosX -= (mods.cName.length() > 0 ? u8g2.getStrWidth(mods.cName.c_str()) / 2 : 0);

        int dPosX = ctrlModHeaderStartCenteredX + 4 + (ctrlModSpaceWidth * 3);
        dPosX -= (mods.dName.length() > 0 ? u8g2.getStrWidth(mods.dName.c_str()) / 2 : 0);

        u8g2.drawStr(aPosX, ctrlModHeaderY + 1, mods.aName.c_str());
        u8g2.drawStr(bPosX, ctrlModHeaderY + 1, mods.bName.c_str());
        u8g2.drawStr(cPosX, ctrlModHeaderY + 1, mods.cName.c_str());
        u8g2.drawStr(dPosX, ctrlModHeaderY + 1, mods.dName.c_str());

        int aValuePos = ctrlModHeaderStartCenteredX + 3;
        aValuePos -= (mods.aValue.length() > 0 ? u8g2.getStrWidth(mods.aValue.c_str()) / 2 : 0);

        int bValuePos = ctrlModHeaderStartCenteredX + 3 + (ctrlModSpaceWidth * 1);
        bValuePos -= (mods.bValue.length() > 0 ? u8g2.getStrWidth(mods.bValue.c_str()) / 2 : 0);

        int cValuePos = ctrlModHeaderStartCenteredX + 4 + (ctrlModSpaceWidth * 2);
        cValuePos -= (mods.cValue.length() > 0 ? u8g2.getStrWidth(mods.cValue.c_str()) / 2 : 0);

        int dValuePos = ctrlModHeaderStartCenteredX + 4 + (ctrlModSpaceWidth * 3);
        dValuePos -= (mods.dValue.length() > 0 ? u8g2.getStrWidth(mods.dValue.c_str()) / 2 : 0);

        u8g2.drawStr(aValuePos, ctrlModHeaderY + 13, mods.aValue.c_str());
        u8g2.drawStr(bValuePos, ctrlModHeaderY + 13, mods.bValue.c_str());
        
        if (mods.cType == XRSound::RANGE)
        {
            int centerLineStartX = ctrlModHeaderStartX + (ctrlModSpaceWidth * 2);
            int rangeMiddleX = (centerLineStartX + (ctrlModSpaceWidth / 2));

            u8g2.drawVLine(centerLineStartX + 6, ctrlModHeaderY + 12, 9);                      // left range bound
            u8g2.drawHLine(centerLineStartX + 6, ctrlModHeaderY + 16, ctrlModSpaceWidth - 12); // range width
            u8g2.drawVLine(rangeMiddleX, ctrlModHeaderY + 12, 9);                              // range middle
            u8g2.drawVLine(centerLineStartX + ctrlModSpaceWidth - 6, ctrlModHeaderY + 12, 9);  // right range bound

            // range pos cursor
            int rangePosI = rangeMiddleX + (int)(mods.cFloatValue * 10);
            u8g2.drawFilledEllipse(rangePosI, ctrlModHeaderY + 16, 1, 2);
        }
        else
        {
            u8g2.drawStr(cValuePos, ctrlModHeaderY + 13, mods.cValue.c_str());
        }

        u8g2.drawStr(dValuePos, ctrlModHeaderY + 13, mods.dValue.c_str());
    }

    void drawControlMods()
    {
        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currPageSelected = XRSequencer::getCurrentSelectedPage();
        auto currSoundForTrack = XRSound::activeSounds[currTrackNum];

        if (currSoundForTrack.type == XRSound::T_MONO_SAMPLE && currPageSelected == 4)
        {
            auto msmpAatt = XRSound::getValueNormalizedAsUInt32(currSoundForTrack.params[XRSound::MSMP_AMP_ATTACK]);
            auto msmpAdec = XRSound::getValueNormalizedAsUInt32(currSoundForTrack.params[XRSound::MSMP_AMP_DECAY]);
            auto msmpAsus = XRSound::getValueNormalizedAsFloat(currSoundForTrack.params[XRSound::MSMP_AMP_SUSTAIN]);
            auto msmpArel = XRSound::getValueNormalizedAsUInt32(currSoundForTrack.params[XRSound::MSMP_AMP_RELEASE]);

            drawControlModsForADSR(msmpAatt, msmpAdec, msmpAsus, msmpArel);
        }
        else if (currSoundForTrack.type == XRSound::T_MONO_SYNTH && currPageSelected == 4)
        {
            auto msynAatt = XRSound::getValueNormalizedAsUInt32(currSoundForTrack.params[XRSound::MSYN_AMP_ATTACK]);
            auto msynAdec = XRSound::getValueNormalizedAsUInt32(currSoundForTrack.params[XRSound::MSYN_AMP_DECAY]);
            auto msynAsus = XRSound::getValueNormalizedAsFloat(currSoundForTrack.params[XRSound::MSYN_AMP_SUSTAIN]);
            auto msynArel = XRSound::getValueNormalizedAsUInt32(currSoundForTrack.params[XRSound::MSYN_AMP_RELEASE]);

            drawControlModsForADSR(msynAatt, msynAdec, msynAsus, msynArel);
        }
        else if (currSoundForTrack.type == XRSound::T_MONO_SYNTH && currPageSelected == 3)
        {
            auto msynFatt = XRSound::getValueNormalizedAsUInt32(currSoundForTrack.params[XRSound::MSYN_FILTER_ATTACK]);
            auto msynFdec = XRSound::getValueNormalizedAsUInt32(currSoundForTrack.params[XRSound::MSYN_FILTER_DECAY]);
            auto msynFsus = XRSound::getValueNormalizedAsFloat(currSoundForTrack.params[XRSound::MSYN_FILTER_SUSTAIN]);
            auto msynFrel = XRSound::getValueNormalizedAsUInt32(currSoundForTrack.params[XRSound::MSYN_FILTER_RELEASE]);

            drawControlModsForADSR(msynFatt, msynFdec, msynFsus, msynFrel);
        }
        // else if (
        //     currPageSelected == 0 &&
        //     ((currSoundForTrack.type == XRSound::T_MONO_SYNTH) ||
        //     (currSoundForTrack.type == XRSound::T_DEXED_SYNTH))
        // ) {
        //     drawNormalControlMods();
        //} 
        else if (currSoundForTrack.type == XRSound::T_MONO_SAMPLE && currPageSelected == 1)
        {
            drawSampleFileControlMods();
        }
        else
        {
            drawModEncoderArea();
            drawExtendedControlMods();
        }
    }

    void drawNormalControlMods()
    {
        int ctrlModHeaderY = 20;
        int ctrlModHeaderStartX = 29;
        int ctrlModHeaderStartCenteredX = (((DISPLAY_MAX_WIDTH - 29) / 4) / 2) + 29;
        int ctrlModSpacerMult = 25;

        // header
        u8g2.drawLine(ctrlModHeaderStartX, 30, 128, 30);
        // dividers
        u8g2.drawLine(ctrlModHeaderStartX, 20, ctrlModHeaderStartX, 52);
        u8g2.drawLine(ctrlModHeaderStartX + (ctrlModSpacerMult * 1), 20, ctrlModHeaderStartX + (ctrlModSpacerMult * 1), 52);
        u8g2.drawLine(ctrlModHeaderStartX + (ctrlModSpacerMult * 2), 20, ctrlModHeaderStartX + (ctrlModSpacerMult * 2), 52);
        u8g2.drawLine(ctrlModHeaderStartX + (ctrlModSpacerMult * 3), 20, ctrlModHeaderStartX + (ctrlModSpacerMult * 3), 52);

        auto mods = XRSound::getControlModDataForTrack();

        int aPosX = ctrlModHeaderStartCenteredX;
        aPosX -= (mods.aName.length() > 0 ? u8g2.getStrWidth(mods.aName.c_str()) / 2 : 0);

        int bPosX = ctrlModHeaderStartCenteredX + (ctrlModSpacerMult * 1);
        bPosX -= (mods.bName.length() > 0 ? u8g2.getStrWidth(mods.bName.c_str()) / 2 : 0);

        int cPosX = ctrlModHeaderStartCenteredX + (ctrlModSpacerMult * 2);
        cPosX -= (mods.cName.length() > 0 ? u8g2.getStrWidth(mods.cName.c_str()) / 2 : 0);

        int dPosX = ctrlModHeaderStartCenteredX + (ctrlModSpacerMult * 3);
        dPosX -= (mods.dName.length() > 0 ? u8g2.getStrWidth(mods.dName.c_str()) / 2 : 0);

        u8g2.drawStr(aPosX, ctrlModHeaderY + 1, mods.aName.c_str());
        u8g2.drawStr(bPosX, ctrlModHeaderY + 1, mods.bName.c_str());
        u8g2.drawStr(cPosX, ctrlModHeaderY + 1, mods.cName.c_str());
        u8g2.drawStr(dPosX, ctrlModHeaderY + 1, mods.dName.c_str());

        int aValuePos = ctrlModHeaderStartCenteredX;
        aValuePos -= (mods.aValue.length() > 0 ? u8g2.getStrWidth(mods.aValue.c_str()) / 2 : 0);

        int bValuePos = ctrlModHeaderStartCenteredX + (ctrlModSpacerMult * 1);
        bValuePos -= (mods.bValue.length() > 0 ? u8g2.getStrWidth(mods.bValue.c_str()) / 2 : 0);

        int cValuePos = ctrlModHeaderStartCenteredX + (ctrlModSpacerMult * 2);
        cValuePos -= (mods.cValue.length() > 0 ? u8g2.getStrWidth(mods.cValue.c_str()) / 2 : 0);

        int dValuePos = ctrlModHeaderStartCenteredX + (ctrlModSpacerMult * 3);
        dValuePos -= (mods.dValue.length() > 0 ? u8g2.getStrWidth(mods.dValue.c_str()) / 2 : 0);

        u8g2.drawStr(aValuePos, ctrlModHeaderY + 17, mods.aValue.c_str());

        if (mods.bType == XRSound::SOUND_CONTROL_MOD_TYPE::RANGE)
        {
            int centerLineStartX = ctrlModHeaderStartX + (ctrlModSpacerMult * 1);
            int rangeMiddleX = (centerLineStartX + (ctrlModSpacerMult / 2));
            u8g2.drawVLine(centerLineStartX + 3, ctrlModHeaderY + 16, 9);                     // left range bound
            u8g2.drawHLine(centerLineStartX + 3, ctrlModHeaderY + 20, ctrlModSpacerMult - 6); // range width
            u8g2.drawVLine(rangeMiddleX, ctrlModHeaderY + 16, 9);                             // range middle
            u8g2.drawVLine(centerLineStartX + ctrlModSpacerMult - 3, ctrlModHeaderY + 16, 9); // right range bound

            // range pos cursor
            int rangePosI = rangeMiddleX + (int)(mods.bFloatValue * 10);
            u8g2.drawFilledEllipse(rangePosI, ctrlModHeaderY + 20, 1, 2);
        }
        else
        {
            u8g2.drawStr(bValuePos, ctrlModHeaderY + 17, mods.bValue.c_str());
        }

        u8g2.drawStr(cValuePos, ctrlModHeaderY + 17, mods.cValue.c_str());
        u8g2.drawStr(dValuePos, ctrlModHeaderY + 17, mods.dValue.c_str());

        if (mods.isAbleToStepModA) {
            if (mods.isActiveStepModA) {
                u8g2.drawLine(
                    (ctrlModHeaderStartCenteredX - 9),
                    ctrlModHeaderY + 29,
                    (ctrlModHeaderStartCenteredX - 9) + 18,
                    ctrlModHeaderY + 29
                );
            } else {
                drawStraightDashedLine(
                    (ctrlModHeaderStartCenteredX - 9),
                    (ctrlModHeaderStartCenteredX - 9) + 20,
                    ctrlModHeaderY + 29
                );
            }
        }

        if (mods.isAbleToStepModB) {
            if (mods.isActiveStepModB) {
                u8g2.drawLine(
                    (ctrlModHeaderStartCenteredX + (ctrlModSpacerMult * 1) - 9),
                    ctrlModHeaderY + 29,
                    (ctrlModHeaderStartCenteredX + (ctrlModSpacerMult * 1) - 9) + 18,
                    ctrlModHeaderY + 29
                );
            } else {
                drawStraightDashedLine(
                    (ctrlModHeaderStartCenteredX + (ctrlModSpacerMult * 1) - 9),
                    (ctrlModHeaderStartCenteredX + (ctrlModSpacerMult * 1) - 9) + 20,
                    ctrlModHeaderY + 29
                );
            }
        }

        if (mods.isAbleToStepModC) {
            if (mods.isActiveStepModC) {
                u8g2.drawLine(
                    (ctrlModHeaderStartCenteredX + (ctrlModSpacerMult * 2) - 9),
                    ctrlModHeaderY + 29,
                    (ctrlModHeaderStartCenteredX + (ctrlModSpacerMult * 2) - 9) + 18,
                    ctrlModHeaderY + 29
                );
            } else {
                drawStraightDashedLine(
                    (ctrlModHeaderStartCenteredX + (ctrlModSpacerMult * 2) - 9),
                    (ctrlModHeaderStartCenteredX + (ctrlModSpacerMult * 2) - 9) + 20,
                    ctrlModHeaderY + 29
                );
            }
        }

        if (mods.isAbleToStepModD) {
            if (mods.isActiveStepModD) {
                u8g2.drawLine(
                    (ctrlModHeaderStartCenteredX + (ctrlModSpacerMult * 3) - 9),
                    ctrlModHeaderY + 29,
                    (ctrlModHeaderStartCenteredX + (ctrlModSpacerMult * 3) - 9) + 18,
                    ctrlModHeaderY + 29
                );
            } else {
                drawStraightDashedLine(
                    (ctrlModHeaderStartCenteredX + (ctrlModSpacerMult * 3) - 9),
                    (ctrlModHeaderStartCenteredX + (ctrlModSpacerMult * 3) - 9) + 20,
                    ctrlModHeaderY + 29
                );
            }
        }
    }

    void drawExtendedControlMods(bool useRatchetTrack)
    {
        int ctrlModHeaderY = XR_DISP_SHIFT_Y+25;
        int ctrlModHeaderStartX = 4;
        int ctrlModHeaderStartCenteredX = ((122 / 4) / 2);
        int ctrlModSpaceWidth = (122 / 4);

        auto mods = XRSound::getControlModDataForTrack();
        if (XRSequencer::onRatchetStepPage() || XRUX::getCurrentMode() == XRUX::SUBMITTING_RATCHET_STEP_VALUE) {
            mods = XRSound::getRatchetControlModData();
        }

        int aPosX = ctrlModHeaderStartCenteredX + 3;
        aPosX -= (mods.aName.length() > 0 ? u8g2.getStrWidth(mods.aName.c_str()) / 2 : 0);

        int bPosX = ctrlModHeaderStartCenteredX + 3 + (ctrlModSpaceWidth * 1);
        bPosX -= (mods.bName.length() > 0 ? u8g2.getStrWidth(mods.bName.c_str()) / 2 : 0);

        int cPosX = ctrlModHeaderStartCenteredX + 4 + (ctrlModSpaceWidth * 2);
        cPosX -= (mods.cName.length() > 0 ? u8g2.getStrWidth(mods.cName.c_str()) / 2 : 0);

        int dPosX = ctrlModHeaderStartCenteredX + 4 + (ctrlModSpaceWidth * 3);
        dPosX -= (mods.dName.length() > 0 ? u8g2.getStrWidth(mods.dName.c_str()) / 2 : 0);

        u8g2.drawStr(aPosX, ctrlModHeaderY + 1, mods.aName.c_str());
        u8g2.drawStr(bPosX, ctrlModHeaderY + 1, mods.bName.c_str());
        u8g2.drawStr(cPosX, ctrlModHeaderY + 1, mods.cName.c_str());
        u8g2.drawStr(dPosX, ctrlModHeaderY + 1, mods.dName.c_str());

        int aValuePos = ctrlModHeaderStartCenteredX + 3;
        aValuePos -= (mods.aValue.length() > 0 ? u8g2.getStrWidth(mods.aValue.c_str()) / 2 : 0);

        int bValuePos = ctrlModHeaderStartCenteredX + 3 + (ctrlModSpaceWidth * 1);
        bValuePos -= (mods.bValue.length() > 0 ? u8g2.getStrWidth(mods.bValue.c_str()) / 2 : 0);

        int cValuePos = ctrlModHeaderStartCenteredX + 4 + (ctrlModSpaceWidth * 2);
        cValuePos -= (mods.cValue.length() > 0 ? u8g2.getStrWidth(mods.cValue.c_str()) / 2 : 0);

        int dValuePos = ctrlModHeaderStartCenteredX + 4 + (ctrlModSpaceWidth * 3);
        dValuePos -= (mods.dValue.length() > 0 ? u8g2.getStrWidth(mods.dValue.c_str()) / 2 : 0);

        u8g2.drawStr(aValuePos, ctrlModHeaderY + 13, mods.aValue.c_str());

        if (mods.bType == XRSound::RANGE)
        {
            int centerLineStartX = ctrlModHeaderStartX + (ctrlModSpaceWidth * 1);
            int rangeMiddleX = (centerLineStartX + (ctrlModSpaceWidth / 2));
            
            u8g2.drawVLine(centerLineStartX + 5, ctrlModHeaderY + 12, 9);                      // left range bound
            u8g2.drawHLine(centerLineStartX + 5, ctrlModHeaderY + 16, ctrlModSpaceWidth - 10); // range width
            u8g2.drawVLine(rangeMiddleX, ctrlModHeaderY + 12, 9);                              // range middle
            u8g2.drawVLine(centerLineStartX + ctrlModSpaceWidth - 5, ctrlModHeaderY + 12, 9);  // right range bound

            // range pos cursor
            int rangePosI = rangeMiddleX + (int)(mods.bFloatValue * 10);
            u8g2.drawFilledEllipse(rangePosI, ctrlModHeaderY + 16, 1, 2);
        }
        else
        {
            u8g2.drawStr(bValuePos, ctrlModHeaderY + 13, mods.bValue.c_str());
        }

        u8g2.drawStr(cValuePos, ctrlModHeaderY + 13, mods.cValue.c_str());
        u8g2.drawStr(dValuePos, ctrlModHeaderY + 13, mods.dValue.c_str());

        auto activeStepModLineY = ctrlModHeaderY + 23;
        auto activeStepModLineShiftX = -9;

        if (mods.isAbleToStepModA) {
            if (mods.isActiveStepModA) {
                u8g2.drawLine(
                    (ctrlModHeaderStartCenteredX + activeStepModLineShiftX),
                    activeStepModLineY,
                    (ctrlModHeaderStartCenteredX + activeStepModLineShiftX) + 24,
                    activeStepModLineY
                );
            } else {
                drawStraightDashedLine(
                    (ctrlModHeaderStartCenteredX + activeStepModLineShiftX),
                    (ctrlModHeaderStartCenteredX + activeStepModLineShiftX) + 25,
                    activeStepModLineY
                );
            }
        }

        if (mods.isAbleToStepModB) {
            if (mods.isActiveStepModB) {
                u8g2.drawLine(
                    (ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 1) + activeStepModLineShiftX),
                    activeStepModLineY,
                    (ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 1) + activeStepModLineShiftX) + 24,
                    activeStepModLineY
                );
            } else {
                drawStraightDashedLine(
                    (ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 1) + activeStepModLineShiftX),
                    (ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 1) + activeStepModLineShiftX) + 25,
                    activeStepModLineY
                );
            }
        }

        if (mods.isAbleToStepModC) {
            if (mods.isActiveStepModC) {
                u8g2.drawLine(
                    (ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 2) + activeStepModLineShiftX),
                    activeStepModLineY,
                    (ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 2) + activeStepModLineShiftX) + 24,
                    activeStepModLineY
                );
            } else {
                drawStraightDashedLine(
                    (ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 2) + activeStepModLineShiftX),
                    (ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 2) + activeStepModLineShiftX) + 25,
                    activeStepModLineY
                );
            }
        }

        if (mods.isAbleToStepModD) {
            if (mods.isActiveStepModD) {
                u8g2.drawLine(
                    (ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 3) + activeStepModLineShiftX),
                    activeStepModLineY,
                    (ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 3) + activeStepModLineShiftX) + 24,
                    activeStepModLineY
                );
            } else {
                drawStraightDashedLine(
                    (ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 3) + activeStepModLineShiftX),
                    (ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 3) + activeStepModLineShiftX) + 25,
                    activeStepModLineY
                );
            }
        }
    }

    void drawSampleFileControlMods()
    {
        int ctrlModHeaderY = XR_DISP_SHIFT_Y + 25;
        int ctrlModHeaderStartX = 0;
        int ctrlModHeaderStartCenteredX = ((122 / 4) / 2);
        int ctrlModSpaceWidth = (122 / 4);

        u8g2.drawFrame(4, XR_DISP_SHIFT_Y+24, 121, 27);
        u8g2.drawLine(5, XR_DISP_SHIFT_Y+34, 122, XR_DISP_SHIFT_Y+34);
        u8g2.drawLine(33, XR_DISP_SHIFT_Y+24, 33, XR_DISP_SHIFT_Y+50);
        u8g2.drawLine(64, XR_DISP_SHIFT_Y+24, 64, XR_DISP_SHIFT_Y+50);

        auto mods = XRSound::getControlModDataForTrack();

        int aPosX = ctrlModHeaderStartCenteredX + 3;
        aPosX -= (mods.aName.length() > 0 ? u8g2.getStrWidth(mods.aName.c_str()) / 2 : 0);

        int bPosX = ctrlModHeaderStartCenteredX + 3 + (ctrlModSpaceWidth * 1);
        bPosX -= (mods.bName.length() > 0 ? u8g2.getStrWidth(mods.bName.c_str()) / 2 : 0);

        u8g2.drawStr(aPosX, ctrlModHeaderY + 1, mods.aName.c_str());
        u8g2.drawStr(bPosX, ctrlModHeaderY + 1, mods.bName.c_str());

        int aValuePos = ctrlModHeaderStartCenteredX + 3;
        aValuePos -= (mods.aValue.length() > 0 ? u8g2.getStrWidth(mods.aValue.c_str()) / 2 : 0);

        int bValuePos = ctrlModHeaderStartCenteredX + 3 + (ctrlModSpaceWidth * 1);
        bValuePos -= (mods.bValue.length() > 0 ? u8g2.getStrWidth(mods.bValue.c_str()) / 2 : 0);

        int cValuePos = ctrlModHeaderStartCenteredX + 4 + (ctrlModSpaceWidth * 2);
        cValuePos -= (mods.cValue.length() > 0 ? u8g2.getStrWidth(mods.cValue.c_str()) / 2 : 0);

        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currSoundForTrack = XRSound::activeSounds[currTrackNum];

        std::string sampleName(currSoundForTrack.sampleName);
        std::string sampleNameB(currSoundForTrack.sampleNameB);
        
        std::string fileName1 = "1.";
        std::string fileName2 = "2.";

        fileName1 += sampleName.length() > 0 ? sampleName : " --";
        fileName2 += sampleNameB.length() > 0 ? sampleNameB : " --";

        u8g2.drawStr(cValuePos - 9, ctrlModHeaderY + 1, "filename");
        u8g2.drawStr(cValuePos - 9, ctrlModHeaderY + 10, fileName1.c_str());
        u8g2.drawStr(cValuePos - 9, ctrlModHeaderY + 17, fileName2.c_str());

        if (sampleName.length() > 0 && sampleNameB.length() == 0) {
            drawStraightDashedLine(aValuePos - 10, aValuePos + 18, ctrlModHeaderY + 17);

            u8g2.drawStr(aValuePos, ctrlModHeaderY + 10, "on");
            u8g2.drawStr(aValuePos - 8, ctrlModHeaderY + 18, "accent");
            u8g2.drawStr(bValuePos, ctrlModHeaderY + 13, "--");
        } else if (sampleName.length() == 0 && sampleNameB.length() > 0) {
            u8g2.drawStr(aValuePos, ctrlModHeaderY + 13, "--");

            drawStraightDashedLine(bValuePos - 10, bValuePos + 18, ctrlModHeaderY + 16);

            u8g2.drawStr(bValuePos, ctrlModHeaderY + 10, "on");
            u8g2.drawStr(bValuePos - 8, ctrlModHeaderY + 10, "accent");
        } else if (sampleName.length() == 0 && sampleNameB.length() == 0) {
            u8g2.drawStr(aValuePos, ctrlModHeaderY + 13, "--");
            u8g2.drawStr(bValuePos, ctrlModHeaderY + 13, "--");
        }

        if (sampleName.length() > 0 && sampleNameB.length() > 0) {
            u8g2.drawStr(aValuePos, ctrlModHeaderY + 13, "on");
            u8g2.drawStr(bValuePos - 8, ctrlModHeaderY + 13, "accent");
        }
    }

    void drawControlModsForADSR(int att, int dec, float sus, int rel)
    {
        int ctrlModHeaderStartX = 4;
        int adsrMaxTopPosY = XR_DISP_SHIFT_Y + 24;
        int adsrMaxBottomPosY = XR_DISP_SHIFT_Y + 46;

        // underline
        u8g2.drawLine(ctrlModHeaderStartX, adsrMaxBottomPosY+4, 124, adsrMaxBottomPosY+4);

        // attack
        int attackStartPosX = ctrlModHeaderStartX;
        int attackEndRawPosX = ctrlModHeaderStartX + (int)(0.025 * att);
        int attackEndPosX = attackEndRawPosX;

        u8g2.drawLine(attackStartPosX, adsrMaxBottomPosY, attackEndPosX, adsrMaxTopPosY);

        // decay and sustain
        int sustainStartRawPosY = adsrMaxBottomPosY - (int)(31 * sus);
        int sustainStartPosY = max(adsrMaxTopPosY, sustainStartRawPosY);

        int decayEndRawPosX = attackEndPosX + (int)(0.05 * dec);
        int decayEndPosX = decayEndRawPosX;

        int sustainEndPosX = decayEndPosX + 20; // static x Pos

        u8g2.drawLine(attackEndPosX, adsrMaxTopPosY, decayEndPosX, sustainStartPosY);
        u8g2.drawLine(decayEndPosX, sustainStartPosY, sustainEndPosX, sustainStartPosY);

        // release
        int releaseEndRawPosX = sustainEndPosX + (int)(0.01 * rel);
        int releaseEndPosX = releaseEndRawPosX;

        u8g2.drawLine(sustainEndPosX, sustainStartPosY, releaseEndPosX, adsrMaxBottomPosY);
    }

    std::string getDisplayNote()
    {
        std::string outputStr;

        auto currentSelectedStep = XRSequencer::getCurrentSelectedStepNum();
        auto currentTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currentLayerNum = XRSequencer::getCurrentSelectedTrackLayerNum();
        auto &currStep = XRSequencer::activePattern.layers[currentLayerNum].tracks[currentTrackNum].steps[currentSelectedStep];

        if (
            XRUX::getCurrentMode() == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStep > -1 &&
            currStep.flags[XRSequencer::NOTE]
        ) {
            auto noteMod = currStep.mods[XRSequencer::NOTE];
            auto octaveMod = currStep.mods[XRSequencer::OCTAVE];

            outputStr += XRHelpers::getNoteStringForBaseNoteNum(noteMod);
            outputStr += std::to_string(octaveMod);
        }
        else
        {
            auto &currTrack = XRSequencer::getCurrentSelectedTrack();

            outputStr += XRHelpers::getNoteStringForBaseNoteNum(currTrack.note);
            outputStr += std::to_string(currTrack.octave);
        }

        return outputStr;
    }

    void drawPageNumIndicators()
    {
        auto currSelectedPage = XRSequencer::getCurrentSelectedPage();

        uint8_t currTrackPageCount = 0;
        std::string currPageNameForTrack = "";

        currTrackPageCount = XRSound::getPageCountForCurrentTrack();
        currPageNameForTrack = XRSound::getPageNameForCurrentTrack();

        int pageTabPosY = XR_DISP_SHIFT_Y + 53;

        u8g2.drawStr(4, pageTabPosY, currPageNameForTrack.c_str());

        if (currTrackPageCount == 1 || currTrackPageCount == 0)
            return;

        int pageNumBasedStartX = 128;
        int pageNumStartX = pageNumBasedStartX - 9;
        int pageBetweenPaddingX = 9;

        auto multX = currTrackPageCount - currSelectedPage;
        auto pageBoxStartX = 128 - (multX * pageBetweenPaddingX);

        u8g2.drawBox(pageBoxStartX, 54, 7, 7);

        for (int l = 0; l < currTrackPageCount; l++)
        {
            auto distMultX = currTrackPageCount - (l+1);

            if (l == currSelectedPage)
            {
                u8g2.setColorIndex((u_int8_t)0);
                u8g2.drawStr(pageNumStartX + 2 - (pageBetweenPaddingX * distMultX), pageTabPosY, std::to_string(l + 1).c_str());
                u8g2.setColorIndex((u_int8_t)1);
            }
            else
            {
                u8g2.drawStr(pageNumStartX + 2 - (pageBetweenPaddingX * distMultX), pageTabPosY, std::to_string(l + 1).c_str());
            }
        }
    }

    void drawPatternPageNumIndicators()
    {
        auto currSelectedPage = XRSequencer::getCurrentSelectedPage();
        if (currSelectedPage > 1) {
            XRSequencer::setSelectedPage(0);
            currSelectedPage = 0;
        }

        uint8_t patternPageCount = MAXIMUM_PATTERN_PAGES;
        std::string currPageNameForPattern = "";
        currPageNameForPattern = XRSound::patternPageNames[currSelectedPage];

        int pageTabPosY = XR_DISP_SHIFT_Y+53;

        u8g2.drawStr(4, pageTabPosY, currPageNameForPattern.c_str());

        if (patternPageCount == 1 || patternPageCount == 0)
            return;

        int pageNumBasedStartX = 124;
        int pageNumStartX = pageNumBasedStartX - 9;
        int pageBetweenPaddingX = 9;

        auto multX = patternPageCount - currSelectedPage;
        auto pageBoxStartX = 124 - (multX * pageBetweenPaddingX);

        u8g2.drawBox(pageBoxStartX, XR_DISP_SHIFT_Y+53, 7, 7);

        for (int l = 0; l < patternPageCount; l++)
        {
            auto distMultX = patternPageCount - (l+1);

            if (l == currSelectedPage)
            {
                u8g2.setColorIndex((u_int8_t)0);
                u8g2.drawStr(pageNumStartX + 2 - (pageBetweenPaddingX * distMultX), pageTabPosY, std::to_string(l + 1).c_str());
                u8g2.setColorIndex((u_int8_t)1);
            }
            else
            {
                u8g2.drawStr(pageNumStartX + 2 - (pageBetweenPaddingX * distMultX), pageTabPosY, std::to_string(l + 1).c_str());
            }
        }
    }

    void drawSoundMenuMain()
    {
        auto menuItems = XRMenu::getSoundMenuItems();
        auto menuItemMax = SOUND_MENU_ITEM_MAX;

        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();

        if (XRSound::activeSounds[currTrackNum].type == XRSound::T_DEXED_SYNTH) {
            menuItems = XRMenu::getDexedSoundMenuItems();
            menuItemMax = DEXED_SOUND_MENU_ITEM_MAX;
        }

        drawGenericMenuList("sound", menuItems, menuItemMax);

        u8g2.sendBuffer();
    }

    void drawSetupMenu()
    {
        auto menuItems = XRMenu::getSetupMenuItems();

        drawGenericMenuList("setup", menuItems, SETUP_MENU_ITEM_MAX);

        u8g2.sendBuffer();
    }

    void drawGenericMenuList(std::string headerStr, std::string *menuItems, int menuItemMax)
    {
        drawGenericOverlayFrame();

        // menu header
        u8g2.drawStr(6, 5, headerStr.c_str());

        // menu items
        int menuItemStartX = 12;
        int menuItemStartY = 17;
        int menuItemLineSpacingY = 9;
        int menuCurrentLine = 0;

        int itemsPresent = 0;

        for (int m = 0; m < menuItemMax; m++)
        {
            if (menuItems[m].length() > 0) {
                u8g2.drawStr(menuItemStartX, menuItemStartY + (menuCurrentLine * menuItemLineSpacingY), menuItems[m].c_str());
                ++menuCurrentLine;
                ++itemsPresent;
            }
        }

        if (itemsPresent == 0) {
            u8g2.drawStr(menuItemStartX, menuItemStartY, "--");
            return;
        }

        // cursor
        int menuItemCursorIdx = XRMenu::getCursorPosition();

        int menuCursorStartX = 6;
        int menuCursorLineSpacingY = 9;
        int menuCursorTriangleSize = 3;
        int menuCursorTriangleY1 = 23 + (menuItemCursorIdx * menuCursorLineSpacingY);
        int menuCursorTriangleY2 = 17 + (menuItemCursorIdx * menuCursorLineSpacingY);
        int menuCursorTriangleY3 = 20 + (menuItemCursorIdx * menuCursorLineSpacingY);

        u8g2.drawTriangle(
            menuCursorStartX,
            menuCursorTriangleY1,
            menuCursorStartX,
            menuCursorTriangleY2,
            menuCursorStartX + menuCursorTriangleSize,
            menuCursorTriangleY3);

        // esc / sel button legend
        // u8g2.drawStr(93, 6, "ESC");
        // u8g2.drawStr(110, 6, "SEL");
        // u8g2.drawFrame(91, 5, 15, 9);
        // u8g2.drawFrame(108, 5, 15, 9);
    }

    void drawPagedMenuList(std::string headerStr, std::string *menuItems, int menuItemMax)
    {
        drawGenericOverlayFrame();

        // menu header
        u8g2.drawStr(6, 4, headerStr.c_str());

        // menu items
        int menuItemStartX = 12;
        int menuItemStartY = 15;
        int menuItemLineSpacingY = 9;
        int menuCurrentLine = 0;
        int itemsPresent = 0;

        int cursorPos = XRMenu::getCursorPosition();

        auto currentFile = XRSD::getCurrSampleFileHighlighted();

        for (int m = 0; m < menuItemMax; m++)
        {
            if (menuItems[m].length() > 0) {
                if ((m == 0 && cursorPos == 0) || currentFile == menuItems[m]) {
                    u8g2.drawBox(
                        menuItemStartX-9,
                        menuItemStartY + (menuCurrentLine * menuItemLineSpacingY) - 1,
                        122,
                        10
                    );
                    u8g2.setColorIndex((u_int8_t)0);
                    u8g2.drawStr(menuItemStartX, menuItemStartY + (menuCurrentLine * menuItemLineSpacingY), menuItems[m].c_str());
                    u8g2.setColorIndex((u_int8_t)1);
                } else {
                    u8g2.drawStr(menuItemStartX, menuItemStartY + (menuCurrentLine * menuItemLineSpacingY), menuItems[m].c_str());
                }

                ++menuCurrentLine;
                ++itemsPresent;
            }
        }

        if (itemsPresent == 0) {
            u8g2.drawStr(menuItemStartX, menuItemStartY, "--");
            return;
        }

        // esc / sel button legend
        // u8g2.drawStr(93, 6, "ESC");
        // u8g2.drawStr(110, 6, "SEL");
        // u8g2.drawFrame(91, 5, 15, 9);
        // u8g2.drawFrame(108, 5, 15, 9);
    }

    void drawSampleBrowser()
    {
        auto cursor = XRMenu::getCursorPosition();
        auto list = XRSD::getSampleList(cursor);

        // todo: impl minimap scroll bar
        drawPagedMenuList("SELECT SAMPLE", list, 5);

        u8g2.sendBuffer();
    }

    void drawDexedSysexBrowser()
    {
        int ctrlHeaderY = 17;
        int ctrlSpaceWidth = (DISPLAY_MAX_WIDTH / 3);
        int ctrlSpaceStartCenteredX = ((DISPLAY_MAX_WIDTH / 3) / 2);

        // todo: impl minimap scroll bar
        drawGenericOverlayFrame();

        std::string headerStr = "load patch: ";
        headerStr += XRSD::dexedPatchName;

        // menu header
        u8g2.drawStr(6, 5, headerStr.c_str());

        u8g2.drawLine(3, 25, 124, 25);
        u8g2.drawLine(ctrlSpaceWidth, 15, ctrlSpaceWidth, 49);
        u8g2.drawLine(ctrlSpaceWidth * 2, 15, ctrlSpaceWidth * 2, 49);

        std::string poolHeaderStr = "syx.pool";
        std::string bankHeaderStr = "syx.bank";
        std::string patchHeaderStr = "syx.patch";

        std::string poolStr = XRSD::getCurrentDexedSysexPool();
        std::string bankStr = XRSD::getCurrentDexedSysexBank();
        std::string patchStr = XRSD::getCurrentDexedSysexPatchNum();

        int aPosX = ctrlSpaceStartCenteredX;
        aPosX -= (poolStr.length() > 0 ? u8g2.getStrWidth(poolStr.c_str()) / 2 : 0);

        int bPosX = ctrlSpaceStartCenteredX + (ctrlSpaceWidth * 1);
        bPosX -= (bankStr.length() > 0 ? u8g2.getStrWidth(bankStr.c_str()) / 2 : 0);

        int cPosX = ctrlSpaceStartCenteredX + (ctrlSpaceWidth * 2);
        cPosX -= (patchStr.length() > 0 ? u8g2.getStrWidth(patchStr.c_str()) / 2 : 0);

        u8g2.drawStr(6, ctrlHeaderY, poolHeaderStr.c_str());
        u8g2.drawStr(aPosX, ctrlHeaderY + 17, poolStr.c_str());

        u8g2.drawStr(48, ctrlHeaderY, bankHeaderStr.c_str());
        u8g2.drawStr(bPosX, ctrlHeaderY + 17, bankStr.c_str());

        u8g2.drawStr(87, ctrlHeaderY, patchHeaderStr.c_str());
        u8g2.drawStr(cPosX, ctrlHeaderY + 17, patchStr.c_str());

        // esc / sel button legend
        u8g2.drawLine(3, 49, 124, 49);
        u8g2.drawStr(93, 52, "esc");
        u8g2.drawStr(110, 52, "sel");
        u8g2.drawFrame(91, 51, 15, 9);
        u8g2.drawFrame(108, 51, 15, 9);

        u8g2.sendBuffer();
    }

    void drawGeneralConfirmOverlay(std::string message)
    {
        int centerX = (128 / 2);
        int centerY = (64 / 2);

        int boxWidth = 100;
        int boxHeight = 30;

        int boxStartX = centerX - (boxWidth / 2);
        int boxStartY = centerY - (boxHeight / 2) - 3;

        int msgStartX = (centerX - (boxWidth / 2)) + 14;
        int msgStartY = (centerY - (boxHeight / 2)) + 8;

        u8g2.setColorIndex((u_int8_t)0);
        u8g2.drawBox(boxStartX, boxStartY, boxWidth, boxHeight);
        u8g2.setColorIndex((u_int8_t)1);
        u8g2.drawFrame(boxStartX, boxStartY, boxWidth, boxHeight);
        u8g2.drawStr(msgStartX, msgStartY, message.c_str());

        u8g2.sendBuffer();
    }

    void drawCopyConfirmOverlay(std::string type, uint8_t num)
    {
        std::string copyStr = "CHOOSE WHERE TO PASTE";

        int centerX = (128 / 2);
        int centerY = (64 / 2);

        int boxWidth = 100;
        int boxHeight = 30;

        int boxStartX = centerX - (boxWidth / 2);
        int boxStartY = centerY - (boxHeight / 2);

        int msgStartX = (centerX - (boxWidth / 2)) + 6;
        int msgStartY = (centerY - (boxHeight / 2)) + 6;

        u8g2.setColorIndex((u_int8_t)0);
        u8g2.drawBox(boxStartX, boxStartY, boxWidth, boxHeight);
        u8g2.setColorIndex((u_int8_t)1);
        u8g2.drawFrame(boxStartX, boxStartY, boxWidth, boxHeight);
        u8g2.drawStr(msgStartX, msgStartY, copyStr.c_str());

        std::string suffixStr = type;
        suffixStr += " ";
        suffixStr += std::to_string(num);

        u8g2.drawStr(msgStartX, msgStartY + 10, suffixStr.c_str());

        u8g2.sendBuffer();
    }

    void drawCopySelOverlay(std::string type)
    {
        std::string copyStr = "SELECT ";
        copyStr += type;
        copyStr += " TO COPY";

        int centerX = (128 / 2);
        int centerY = (64 / 2);

        int boxWidth = 100;
        int boxHeight = 20;

        int boxStartX = centerX - (boxWidth / 2);
        int boxStartY = centerY - (boxHeight / 2);

        int msgStartX = (centerX - (boxWidth / 2)) + 6;
        int msgStartY = (centerY - (boxHeight / 2)) + 6;

        u8g2.setColorIndex((u_int8_t)0);
        u8g2.drawBox(boxStartX, boxStartY, boxWidth, boxHeight);
        u8g2.setColorIndex((u_int8_t)1);
        u8g2.drawFrame(boxStartX, boxStartY, boxWidth, boxHeight);
        u8g2.drawStr(msgStartX, msgStartY, copyStr.c_str());

        u8g2.sendBuffer();
    }

    void drawPasteConfirmOverlay(std::string type, uint8_t num)
    {
        std::string pasteStr;
        pasteStr += type;
        pasteStr += " PASTED TO ";

        int centerX = (128 / 2);
        int centerY = (64 / 2);

        int boxWidth = 100;
        int boxHeight = 30;

        int boxStartX = centerX - (boxWidth / 2);
        int boxStartY = centerY - (boxHeight / 2);

        int msgStartX = (centerX - (boxWidth / 2)) + 6;
        int msgStartY = (centerY - (boxHeight / 2)) + 6;

        u8g2.setColorIndex((u_int8_t)0);
        u8g2.drawBox(boxStartX, boxStartY, boxWidth, boxHeight);
        u8g2.setColorIndex((u_int8_t)1);
        u8g2.drawFrame(boxStartX, boxStartY, boxWidth, boxHeight);
        u8g2.drawStr(msgStartX, msgStartY, pasteStr.c_str());

        std::string suffixStr = type;
        suffixStr += " ";
        suffixStr += std::to_string(num);

        u8g2.drawStr(msgStartX, msgStartY + 10, suffixStr.c_str());

        u8g2.sendBuffer();
    }
}