#include <XRHelpers.h>
#include <map>

namespace XRHelpers
{
    time_t RTCTime;

    std::map<int8_t, char> stepCharMap = {
        {1, 'm'},
        {2, 'n'},
        {3, 'o'},
        {4, 'p'},
        {5, 's'},
        {6, 't'},
        {7, 'u'},
        {8, 'v'},
        {9, 'y'},
        {10, 'z'},
        {11, '1'},
        {12, '2'},
        {13, '5'},
        {14, '6'},
        {15, '7'},
        {16, '8'},
    };

    std::map<uint8_t, std::string> baseNoteToStr = {
        {0, "c"},
        {1, "c#"},
        {2, "d"},
        {3, "d#"},
        {4, "e"},
        {5, "f"},
        {6, "f#"},
        {7, "g"},
        {8, "g#"},
        {9, "a"},
        {10, "a#"},
        {11, "b"},
        {12, "c"},
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
            "/%s", 
            DEVICE_NAME
        );
    }
    
    void getMachineStateFile(char *buf)
    {
        sprintf(
            buf,
            "/.%s-%s.%s",
            MACHINE_STATE_FILENAME_PREFIX,
            FIRMWARE_VERSION,
            "bin"
        );
    }

    void getProjectsDir(char *buf)
    {
        sprintf(
            buf,
            "/%s",
            DEVICE_NAME
        );
    }

    std::string getNoteStringForBaseNoteNum(uint8_t num)
    {
        return baseNoteToStr[num];
    }
}