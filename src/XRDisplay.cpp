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
    U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/13, /* data=*/11, /* cs=*/10, /* dc=*/14, /* reset=*/15);

    void init()
    {
        u8g2.begin();
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
        u8g2.drawLine(2, 12, width, 12);
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

        std::string headerStr = "NEW PROJ.";
        u8g2.drawStr(8, 6, headerStr.c_str());

        std::string projectNameStr = XRKeyInput::get();

        u8g2.drawStr(50, 6, "ESC BACK  SEL SAVE");
        u8g2.drawFrame(48, 5, 15, 9);
        u8g2.drawFrame(88, 5, 15, 9);

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

    void drawSequencerScreen(bool queueBlink)
    {
        Serial.println("enter drawSequencerScreen!");

        u8g2.clearBuffer();

        uint8_t currentBpm = (uint8_t)XRClock::getTempo();
        std::string currentBpmStr = XRHelpers::strldz(std::to_string(currentBpm), 3);
        u8g2.drawStr(104, 0, currentBpmStr.c_str());

        auto seqState = XRSequencer::getSeqState();

        // transport indicator (play, pause, stop)
        uint8_t transportIconStartX = 119;
        if (seqState.playbackState == XRSequencer::STOPPED)
        {
            u8g2.drawBox(transportIconStartX, 1, 5, 5);
        }
        else if (seqState.playbackState == XRSequencer::RUNNING)
        {
            u8g2.drawTriangle(transportIconStartX, 0, transportIconStartX, 6, transportIconStartX + 6, 3);
        }
        else if (seqState.playbackState == XRSequencer::PAUSED)
        {
            u8g2.drawBox(transportIconStartX, 1, 2, 5);
            u8g2.drawBox(transportIconStartX + 3, 1, 2, 5);
        }

        auto currentUXMode = XRUX::getCurrentMode();
        if (currentUXMode == XRUX::PERFORM_MUTE)
        {
            u8g2.drawStr(0, 0, "MUTE MODE");
            u8g2.drawLine(0, 10, DISPLAY_MAX_WIDTH, 10);

            u8g2.drawStr(23, 15, "SELECT TRACKS TO MUTE");

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
            u8g2.drawStr(0, 0, "SOLO MODE");
            u8g2.drawLine(0, 10, DISPLAY_MAX_WIDTH, 10);

            u8g2.drawStr(23, 15, "SELECT TRACK TO SOLO");

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
        else if (currentUXMode == XRUX::PERFORM_RATCHET)
        {
            drawPerformRatchetScreen();

            return;
        }

        auto currentSelectedBank = XRSequencer::getCurrentSelectedBankNum();
        auto currentSelectedPattern = XRSequencer::getCurrentSelectedPatternNum();
        auto currentSelectedTrack = XRSequencer::getCurrentSelectedTrackNum();
        auto &queuedPattern = XRSequencer::getQueuedPattern();

        bool bnkBlink = false;
        int bnkNumber = currentSelectedBank + 1;

        if (queuedPattern.bank > -1)
        {
            bnkNumber = queuedPattern.bank + 1;
            bnkBlink = (queuedPattern.bank != currentSelectedBank);
        }

        drawMenuHeader("BNK:", bnkNumber, 0, (queueBlink && bnkBlink));

        bool ptnBlink = false;
        int ptnNumber = currentSelectedPattern + 1;

        if (queuedPattern.number > -1)
        {
            ptnNumber = queuedPattern.number + 1;
            ptnBlink = (queuedPattern.number != currentSelectedPattern);

            Serial.printf("ptnNumber: %d, ptnBlink: %d, queueBlink: %d\n", ptnNumber, ptnBlink, queueBlink);
        }


        if (currentUXMode == XRUX::PATTERN_WRITE)
        {
            u8g2.setColorIndex((u_int8_t)1);
            u8g2.drawBox(26, 0, 29, 7);
            u8g2.setColorIndex((u_int8_t)0);
            drawMenuHeader("PTN:", ptnNumber, 29, (queueBlink && ptnBlink));
            u8g2.setColorIndex((u_int8_t)1);
        }
        else if (currentUXMode == XRUX::TRACK_WRITE || currentUXMode == XRUX::TRACK_SEL || currentUXMode == XRUX::SUBMITTING_STEP_VALUE)
        {
            drawMenuHeader("PTN:", ptnNumber, 29, (queueBlink && ptnBlink));
        }

        if (currentUXMode == XRUX::TRACK_WRITE || currentUXMode == XRUX::TRACK_SEL || currentUXMode == XRUX::SUBMITTING_STEP_VALUE)
        {
            u8g2.setColorIndex((u_int8_t)1);
            u8g2.drawBox(55, 0, 29, 7);
            u8g2.setColorIndex((u_int8_t)0);
            drawMenuHeader("TRK:", currentSelectedTrack + 1, 58, false);
            u8g2.setColorIndex((u_int8_t)1);
        }

        if (currentUXMode == XRUX::PATTERN_WRITE)
        {
            // draw pattern header box
            u8g2.setColorIndex((u_int8_t)1);
            u8g2.drawBox(0, 9, 128, 9);
            u8g2.setColorIndex((u_int8_t)0);
            std::string patternHeaderStr = "PATTERN EDIT";
            u8g2.drawStr(2, 10, patternHeaderStr.c_str());
            u8g2.setColorIndex((u_int8_t)1);

            // draw control mod area
            drawPatternControlMods();

            u8g2.drawLine(0, 52, 128, 52);
            u8g2.drawStr(0, 56, "MAIN");
        }
        else if (
            currentUXMode == XRUX::TRACK_WRITE ||
            currentUXMode == XRUX::TRACK_SEL ||
            currentUXMode == XRUX::SUBMITTING_STEP_VALUE)
        {
            auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
            auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
            auto currSoundForTrack = XRSound::currentPatternSounds[currTrackNum];

            // draw track meta type box
            int trackMetaStrX = 2;
            if (currSoundForTrack.type == XRSound::T_MONO_SYNTH || currSoundForTrack.type == XRSound::T_MIDI)
            {
                trackMetaStrX = 4;
            }

            u8g2.setColorIndex((u_int8_t)1);
            u8g2.drawBox(0, 9, 128, 9);
            u8g2.setColorIndex((u_int8_t)0);

            std::string trackMetaStr = XRSequencer::getTrackMetaStr(currSoundForTrack.type);
            std::string trackInfoStr;

            if (currSoundForTrack.type == XRSound::T_MONO_SYNTH)
            {
                trackInfoStr += XRSequencer::getTrackTypeNameStr(currSoundForTrack.type);
            }
            else if (currSoundForTrack.type == XRSound::T_DEXED_SYNTH)
            {
                trackInfoStr += XRSequencer::getTrackTypeNameStr(currSoundForTrack.type);
            }
            else if (currSoundForTrack.type == XRSound::T_FM_DRUM)
            {
                trackInfoStr += XRSequencer::getTrackTypeNameStr(currSoundForTrack.type);
            }
            else if (currSoundForTrack.type == XRSound::T_MONO_SAMPLE)
            {
                std::string sampleName(currSoundForTrack.sampleName);
                
                trackInfoStr += sampleName.length() > 0 ? sampleName : "N/A"; // TODO change to use patch name?
            }
            else if (currSoundForTrack.type == XRSound::T_MIDI)
            {
                trackInfoStr += "";
            }
            else if (currSoundForTrack.type == XRSound::T_CV_GATE || currSoundForTrack.type == XRSound::T_CV_TRIG)
            {
                trackInfoStr += "";
            }

            std::string combStr = trackMetaStr + " " + trackInfoStr;
            u8g2.drawStr(trackMetaStrX, 10, combStr.c_str());
            u8g2.setColorIndex((u_int8_t)1);

            auto currentSelectedPage = XRSequencer::getCurrentSelectedPage();

            // draw track description / main icon area
            if (
                currSoundForTrack.type == XRSound::T_MONO_SYNTH ||
                currSoundForTrack.type == XRSound::T_DEXED_SYNTH ||
                currSoundForTrack.type == XRSound::T_MIDI ||
                currSoundForTrack.type == XRSound::T_CV_GATE)
            {
                if (
                    (currSoundForTrack.type != XRSound::T_MONO_SYNTH) ||
                    ((currSoundForTrack.type == XRSound::T_MONO_SYNTH && currentSelectedPage != 3) &&
                     (currSoundForTrack.type == XRSound::T_MONO_SYNTH && currentSelectedPage != 4)))
                {
                    u8g2.drawStr(6, 23, "NOTE");
                    u8g2.setFont(bitocra13_c);
                    u8g2.drawStr(8, 32, getDisplayNote().c_str());
                    u8g2.setFont(bitocra7_c);
                }
            }
            else if (currSoundForTrack.type == XRSound::T_CV_TRIG)
            {
                // u8g2.drawLine(3,42,14,42);
                // u8g2.drawLine(14,42,14,28);
                // u8g2.drawLine(14,28,19,28);
                // u8g2.drawLine(19,28,19,42);
                // u8g2.drawLine(19,42,24,42);
            }

            // draw control mod area
            drawControlMods();
            drawPageNumIndicators();
        }

        u8g2.sendBuffer();
    }

    void drawPerformRatchetScreen()
    {
        auto ratchetTrack = XRSequencer::getRatchetTrack();
        auto ratchetDivision = XRSequencer::getRatchetDivision();

        u8g2.drawStr(0, 0, "RATCHET MODE");
        u8g2.drawLine(0, 10, DISPLAY_MAX_WIDTH, 10);

        if (ratchetTrack > -1)
        {
            std::string rchTrkStr = "TRK:";
            rchTrkStr += XRHelpers::strldz(std::to_string(ratchetTrack + 1), 2);
            u8g2.drawStr(0, 12, rchTrkStr.c_str());
        }
        else if (ratchetTrack == -1)
        {
            u8g2.drawStr(3, 12, "HOLD A TRACK BUTTON TO RATCHET");
        }

        // draw keyboard
        drawKeyboard(0);

        if (ratchetDivision > 0)
        {
            // ratchet_division = 24 = 96/24 = 4 = 1/4
            uint8_t divDenominator = (96 / ratchetDivision);

            std::string divStr = "1/";
            divStr += std::to_string(divDenominator);

            int xMult = 14;
            int xMultB = 13;

            // even divisions (bottom of keyboard)
            if (ratchetDivision == 24)
            { // 1/4
                u8g2.drawTriangle(14, 60, 17, 55, 20, 60);
                u8g2.drawStr(26, 54, divStr.c_str());
            }
            else if (ratchetDivision == 12)
            { // 1/8
                u8g2.drawTriangle(14 + (xMult * 1), 60, 17 + (xMult * 1), 55, 20 + (xMult * 1), 60);
                u8g2.drawStr(26 + (xMult * 1), 54, divStr.c_str());
            }
            else if (ratchetDivision == 6)
            { // 1/16
                u8g2.drawTriangle(14 + (xMult * 2), 60, 17 + (xMult * 2), 55, 20 + (xMult * 2), 60);
                u8g2.drawStr(26 + (xMult * 2), 54, divStr.c_str());
            }
            else if (ratchetDivision == 3)
            { // 1/32
                u8g2.drawTriangle(14 + (xMult * 3), 60, 17 + (xMult * 3), 55, 20 + (xMult * 3), 60);
                u8g2.drawStr(26 + (xMult * 3), 54, divStr.c_str());
            }
            else if (ratchetDivision == 2)
            { // 1/48
                u8g2.drawTriangle(14 + (xMult * 4), 60, 17 + (xMult * 4), 55, 20 + (xMult * 4), 60);
                u8g2.drawStr(26 + (xMult * 4), 54, divStr.c_str());
            }
            else if (ratchetDivision == 1)
            { // 1/96
                u8g2.drawTriangle(14 + (xMult * 5), 60, 17 + (xMult * 5), 55, 20 + (xMult * 5), 60);
                u8g2.drawStr(26 + (xMult * 5), 54, divStr.c_str());
            }
            // odd divisions (top of keyboard)
            else if (ratchetDivision == 16)
            { // 1/6
                u8g2.drawTriangle(22, 21, 24, 25, 27, 21);
                u8g2.drawStr(33, 19, divStr.c_str());
            }
            else if (ratchetDivision == 8)
            { // 1/12
                u8g2.drawTriangle(22 + (xMultB * 1), 21, 24 + (xMultB * 1), 25, 27 + (xMultB * 1), 21);
                u8g2.drawStr(33 + (xMultB * 1), 19, divStr.c_str());
            }
            else if (ratchetDivision == 4)
            { // 1/24
                u8g2.drawTriangle(22 + (xMultB * 3), 21, 24 + (xMultB * 3), 25, 27 + (xMultB * 3), 21);
                u8g2.drawStr(33 + (xMultB * 3), 19, divStr.c_str());
            }
        }
        else
        {
            u8g2.drawStr(10, 54, "HOLD A RATCHET DIVISION KEY");
        }

        u8g2.sendBuffer();

        return;
    }

    void drawMenuHeader(std::string inputStr, int8_t value, int8_t startX, bool hideNum)
    {
        if (!hideNum)
            inputStr += XRHelpers::strldz(std::to_string(value), 2);

        u8g2.drawStr(startX, 0, inputStr.c_str());
    }

    void drawPatternControlMods()
    {
        int ctrlModHeaderY = 20;
        // int ctrlModHeaderBoxSize = 9;
        int ctrlModHeaderStartX = 0;
        int ctrlModSpaceWidth = (DISPLAY_MAX_WIDTH / 4);
        int ctrlModHeaderStartCenteredX = (DISPLAY_MAX_WIDTH / 4) / 2;

        u8g2.drawLine(ctrlModHeaderStartX, 30, 128, 30);
        u8g2.drawLine(ctrlModHeaderStartX + (ctrlModSpaceWidth * 1), 20, ctrlModHeaderStartX + (ctrlModSpaceWidth * 1), 52);
        u8g2.drawLine(ctrlModHeaderStartX + (ctrlModSpaceWidth * 2), 20, ctrlModHeaderStartX + (ctrlModSpaceWidth * 2), 52);
        u8g2.drawLine(ctrlModHeaderStartX + (ctrlModSpaceWidth * 3), 20, ctrlModHeaderStartX + (ctrlModSpaceWidth * 3), 52);

        auto mods = XRSound::getControlModDataForPattern();

        int aPosX = ctrlModHeaderStartCenteredX;
        aPosX -= (mods.aName.length() > 0 ? u8g2.getStrWidth(mods.aName.c_str()) / 2 : 0);

        int bPosX = ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 1);
        bPosX -= (mods.bName.length() > 0 ? u8g2.getStrWidth(mods.bName.c_str()) / 2 : 0);

        int cPosX = ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 2);
        cPosX -= (mods.cName.length() > 0 ? u8g2.getStrWidth(mods.cName.c_str()) / 2 : 0);

        int dPosX = ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 3);
        dPosX -= (mods.dName.length() > 0 ? u8g2.getStrWidth(mods.dName.c_str()) / 2 : 0);

        u8g2.drawStr(aPosX, ctrlModHeaderY + 1, mods.aName.c_str());
        u8g2.drawStr(bPosX, ctrlModHeaderY + 1, mods.bName.c_str());
        u8g2.drawStr(cPosX, ctrlModHeaderY + 1, mods.cName.c_str());
        u8g2.drawStr(dPosX, ctrlModHeaderY + 1, mods.dName.c_str());

        int aValuePos = ctrlModHeaderStartCenteredX;
        aValuePos -= (mods.aValue.length() > 0 ? u8g2.getStrWidth(mods.aValue.c_str()) / 2 : 0);

        int bValuePos = ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 1);
        bValuePos -= (mods.bValue.length() > 0 ? u8g2.getStrWidth(mods.bValue.c_str()) / 2 : 0);

        int cValuePos = ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 2);
        cValuePos -= (mods.cValue.length() > 0 ? u8g2.getStrWidth(mods.cValue.c_str()) / 2 : 0);

        int dValuePos = ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 3);
        dValuePos -= (mods.dValue.length() > 0 ? u8g2.getStrWidth(mods.dValue.c_str()) / 2 : 0);

        u8g2.drawStr(aValuePos, ctrlModHeaderY + 17, mods.aValue.c_str());
        u8g2.drawStr(bValuePos, ctrlModHeaderY + 17, mods.bValue.c_str());
        u8g2.drawStr(cValuePos, ctrlModHeaderY + 17, mods.cValue.c_str());
        u8g2.drawStr(dValuePos, ctrlModHeaderY + 17, mods.dValue.c_str());
    }

    void drawControlMods()
    {
        auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();
        auto currTrackNum = XRSequencer::getCurrentSelectedTrackNum();
        auto currPageSelected = XRSequencer::getCurrentSelectedPage();
        auto currSoundForTrack = XRSound::currentPatternSounds[currTrackNum];

        if (currSoundForTrack.type == XRSound::T_MONO_SAMPLE && currPageSelected == 2)
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
        else if (
            (currSoundForTrack.type == XRSound::T_EMPTY) ||
            (currSoundForTrack.type == XRSound::T_CV_TRIG) ||
            (currSoundForTrack.type == XRSound::T_FM_DRUM) ||
            (currSoundForTrack.type == XRSound::T_MONO_SAMPLE && currPageSelected == 0) ||
            (currSoundForTrack.type == XRSound::T_MONO_SAMPLE && currPageSelected == 1) ||
            (currSoundForTrack.type == XRSound::T_MONO_SAMPLE && currPageSelected == 3))
        {
            drawExtendedControlMods();

            // sample folder icon
            if (currSoundForTrack.type == XRSound::T_MONO_SAMPLE && currPageSelected == 0) {
                u8g2.setColorIndex((u_int8_t)0);
                u8g2.drawBox(105, 36, 15, 10);
                u8g2.setColorIndex((u_int8_t)1);
                u8g2.drawFrame(105, 36, 15, 10);
                u8g2.drawStr(107, 37, "uSD");
            }
        }
        else
        {
            drawNormalControlMods();
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
    }

    void drawExtendedControlMods()
    {
        int ctrlModHeaderY = 20;
        int ctrlModHeaderStartX = 0;
        int ctrlModHeaderStartCenteredX = ((DISPLAY_MAX_WIDTH / 4) / 2);
        int ctrlModSpaceWidth = (DISPLAY_MAX_WIDTH / 4);

        // header
        u8g2.drawLine(ctrlModHeaderStartX, 30, 128, 30);

        // dividers
        u8g2.drawLine(ctrlModSpaceWidth, 20, ctrlModSpaceWidth, 52);
        u8g2.drawLine(ctrlModSpaceWidth + (ctrlModSpaceWidth * 1), 20, ctrlModSpaceWidth + (ctrlModSpaceWidth * 1), 52);
        u8g2.drawLine(ctrlModSpaceWidth + (ctrlModSpaceWidth * 2), 20, ctrlModSpaceWidth + (ctrlModSpaceWidth * 2), 52);

        auto mods = XRSound::getControlModDataForTrack();

        int aPosX = ctrlModHeaderStartCenteredX;
        aPosX -= (mods.aName.length() > 0 ? u8g2.getStrWidth(mods.aName.c_str()) / 2 : 0);

        int bPosX = ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 1);
        bPosX -= (mods.bName.length() > 0 ? u8g2.getStrWidth(mods.bName.c_str()) / 2 : 0);

        int cPosX = ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 2);
        cPosX -= (mods.cName.length() > 0 ? u8g2.getStrWidth(mods.cName.c_str()) / 2 : 0);

        int dPosX = ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 3);
        dPosX -= (mods.dName.length() > 0 ? u8g2.getStrWidth(mods.dName.c_str()) / 2 : 0);

        u8g2.drawStr(aPosX, ctrlModHeaderY + 1, mods.aName.c_str());
        u8g2.drawStr(bPosX, ctrlModHeaderY + 1, mods.bName.c_str());
        u8g2.drawStr(cPosX, ctrlModHeaderY + 1, mods.cName.c_str());
        u8g2.drawStr(dPosX, ctrlModHeaderY + 1, mods.dName.c_str());

        int aValuePos = ctrlModHeaderStartCenteredX;
        aValuePos -= (mods.aValue.length() > 0 ? u8g2.getStrWidth(mods.aValue.c_str()) / 2 : 0);

        int bValuePos = ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 1);
        bValuePos -= (mods.bValue.length() > 0 ? u8g2.getStrWidth(mods.bValue.c_str()) / 2 : 0);

        int cValuePos = ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 2);
        cValuePos -= (mods.cValue.length() > 0 ? u8g2.getStrWidth(mods.cValue.c_str()) / 2 : 0);

        int dValuePos = ctrlModHeaderStartCenteredX + (ctrlModSpaceWidth * 3);
        dValuePos -= (mods.dValue.length() > 0 ? u8g2.getStrWidth(mods.dValue.c_str()) / 2 : 0);

        u8g2.drawStr(aValuePos, ctrlModHeaderY + 17, mods.aValue.c_str());

        if (mods.bType == XRSound::RANGE)
        {
            int centerLineStartX = ctrlModHeaderStartX + (ctrlModSpaceWidth * 1);
            int rangeMiddleX = (centerLineStartX + (ctrlModSpaceWidth / 2));
            u8g2.drawVLine(centerLineStartX + 6, ctrlModHeaderY + 16, 9);                      // left range bound
            u8g2.drawHLine(centerLineStartX + 6, ctrlModHeaderY + 20, ctrlModSpaceWidth - 12); // range width
            u8g2.drawVLine(rangeMiddleX, ctrlModHeaderY + 16, 9);                              // range middle
            u8g2.drawVLine(centerLineStartX + ctrlModSpaceWidth - 6, ctrlModHeaderY + 16, 9);  // right range bound

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
    }

    void drawControlModsForADSR(int att, int dec, float sus, int rel)
    {
        int ctrlModHeaderStartX = 1;
        int adsrMaxTopPosY = 24;
        int adsrMaxBottomPosY = 48;

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

        // if (XRUX::getCurrentMode() == XRUX::SUBMITTING_STEP_VALUE && currentSelectedStep > -1)
        // {
        //     auto &trackStepMods = XRSequencer::getModsForCurrentTrackStep();

        //     outputStr += XRHelpers::getNoteStringForBaseNoteNum(trackStepMods.note);
        //     outputStr += std::to_string(trackStepMods.octave);
        // }
        // else
        // {
            auto &currTrack = XRSequencer::getHeapCurrentSelectedTrack();

            outputStr += XRHelpers::getNoteStringForBaseNoteNum(currTrack.note);
            outputStr += std::to_string(currTrack.octave);
        //}

        return outputStr;
    }

    void drawPageNumIndicators()
    {
        auto currSelectedPage = XRSequencer::getCurrentSelectedPage();
        auto currTrackPageCount = XRSequencer::getCurrentTrackPageCount();
        auto currPageNameForTrack = XRSequencer::getCurrPageNameForTrack();

        int pageNumBasedStartX = 81 - (3 * currTrackPageCount);
        int pageTabPosY = 56;
        int pageTabFooterNameStartX = 17;

        u8g2.drawLine(0, 52, 128, 52);
        u8g2.drawStr(0, pageTabPosY, currPageNameForTrack.c_str());

        if (currTrackPageCount == 1)
            return;

        if (currTrackPageCount == 5)
        {
            pageTabFooterNameStartX -= 10;
        }
        else if (currTrackPageCount == 4)
        {
            pageTabFooterNameStartX -= 1;
        }

        int pageBoxStartX = (pageNumBasedStartX + 30) - (currTrackPageCount * 5);
        pageBoxStartX += 8 / (currTrackPageCount);

        int pageNumStartX = pageBoxStartX + 3;
        int pageBetweenPaddingtX = 10;

        u8g2.drawBox(pageBoxStartX + (pageBetweenPaddingtX * currSelectedPage), 55, 9, 9);

        for (int l = 0; l < currTrackPageCount; l++)
        {
            if (l == currSelectedPage)
            {
                u8g2.setColorIndex((u_int8_t)0);
                u8g2.drawStr(pageNumStartX + (pageBetweenPaddingtX * l), pageTabPosY, std::to_string(l + 1).c_str());
                u8g2.setColorIndex((u_int8_t)1);
            }
            else
            {
                u8g2.drawStr(pageNumStartX + (pageBetweenPaddingtX * l), pageTabPosY, std::to_string(l + 1).c_str());
            }
        }
    }

    void drawSoundMenuMain()
    {
        Serial.println("enter drawSoundMenuMain!");

        drawGenericMenuList("SOUND", XRMenu::getSoundMenuItems(), SOUND_MENU_ITEM_MAX);

        u8g2.sendBuffer();
    }

    void drawSetupMenu()
    {
        drawGenericMenuList("SETUP", XRMenu::getSetupMenuItems(), SETUP_MENU_ITEM_MAX);

        u8g2.sendBuffer();
    }

    void drawGenericMenuList(std::string headerStr, std::string *menuItems, int menuItemMax)
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
        int menuCursorTriangleY1 = 21 + (menuItemCursorIdx * menuCursorLineSpacingY);
        int menuCursorTriangleY2 = 15 + (menuItemCursorIdx * menuCursorLineSpacingY);
        int menuCursorTriangleY3 = 18 + (menuItemCursorIdx * menuCursorLineSpacingY);

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
        drawPagedMenuList("SAMPLES", list, 5);

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