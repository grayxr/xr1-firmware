#ifndef XRHelpers_h
#define XRHelpers_h

#include <Arduino.h>
#include <TimeLib.h>
#include <string>
#include <XRConfig.h>

namespace XRHelpers
{
    time_t getTeensy3Time();

    void getMachineStateDir(char *buf);
    void getMachineStateFile(char *buf);
    void getProjectsDir(char *buf);

    std::string strldz(std::string inputStr, const int zeroNum);
    std::string getNewProjectName();
    std::string getNoteStringForBaseNoteNum(uint8_t num);
}

#endif /* XRHelpers_h */