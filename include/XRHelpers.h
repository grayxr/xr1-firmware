#ifndef XRHelpers_h
#define XRHelpers_h

#include <Arduino.h>
#include <TimeLib.h>
#include <string>
#include <map>
#include <XRConfig.h>

namespace XRHelpers
{
    extern std::map<int8_t, char> stepCharMap;

    time_t getTeensy3Time();

    void getMachineStateDir(char *buf);
    void getMachineStateFile(char *buf);
    void getProjectsDir(char *buf);

    std::string strldz(std::string inputStr, const int zeroNum);
    std::string getNewProjectName();
    std::string getNoteStringForBaseNoteNum(uint8_t num);

    long aMap(long x, long in_min, long in_max, long out_min, long out_max);
}

#endif /* XRHelpers_h */