#include <XRHelpers.h>
#include <map>

namespace XRHelpers
{
    time_t RTCTime;

    std::map<uint8_t, std::string> baseNoteToStr = {
        {0, "C"},
        {1, "C#"},
        {2, "D"},
        {3, "D#"},
        {4, "E"},
        {5, "F"},
        {6, "F#"},
        {7, "G"},
        {8, "G#"},
        {9, "A"},
        {10, "A#"},
        {11, "B"},
        {12, "C"},
    };

    std::string strldz(std::string inputStr, const int zeroNum)
    {
        std::string outputStr;

        unsigned int number_of_zeros = zeroNum - inputStr.length();

        outputStr.insert(0, number_of_zeros, '0');
        outputStr.append(inputStr);

        return outputStr;
    }

    std::string getNewProjectName()
    {
        std::string newProjectName = "project_";

        newProjectName += std::to_string(year());

        if (month() < 10) {
            newProjectName += strldz(std::to_string(month()),2);
        } else {
            newProjectName += std::to_string(month());
        }

        if (day() < 10) {
            newProjectName += strldz(std::to_string(day()),2);
        } else {
            newProjectName += std::to_string(day());
        }

        if (hour() < 10) {
            newProjectName += strldz(std::to_string(hour()),2);
        } else {
            newProjectName += std::to_string(hour());
        }

        if (minute() < 10) {
            newProjectName += strldz(std::to_string(minute()),2);
        } else {
            newProjectName += std::to_string(minute());
        }

        if (second() < 10) {
            newProjectName += strldz(std::to_string(second()),2);
        } else {
            newProjectName += std::to_string(second());
        }

        return newProjectName;
    }

    time_t getTeensy3Time()
    {
        return Teensy3Clock.get();
    }

    void getMachineStateDir(char *buf)
    {
        sprintf(
            buf,
            "/%s/%s/%s", 
            COMPANY_NAME,
            DEVICE_NAME,
            DATA_FOLDER_NAME
        );
    }
    
    void getMachineStateFile(char *buf)
    {
        sprintf(
            buf,
            "/%s-%s.%s",
            MACHINE_STATE_FILENAME_PREFIX,
            FIRMWARE_VERSION,
            "bin"
        );
    }

    void getProjectsDir(char *buf)
    {
        sprintf(
            buf,
            "/%s/%s/%s", 
            COMPANY_NAME,
            DEVICE_NAME,
            PROJECTS_FOLDER_NAME
        );
    }

    std::string getNoteStringForBaseNoteNum(uint8_t num)
    {
        return baseNoteToStr[num];
    }
}