#include <XRAudio.h>

namespace XRAudio
{
    ResponsiveAnalogRead analog(HEADPHONE_PIN, true);

    float currentHeadphoneVolume;

    void init()
    {
        // Audio connections require memory to work.  For more
        // detailed information, see the MemoryAndCpuUsage example
        AudioMemory(50);

        // Comment these out if not using the audio adaptor board.
        // This may wait forever if the SDA & SCL pins lack
        // pullup resistors
        sgtl5000_1.enable();
        sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
        sgtl5000_1.lineInLevel(0);
        sgtl5000_1.lineOutLevel(29);
        sgtl5000_1.volume(1.0);

        currentHeadphoneVolume = 0.5;
    }

    void handleHeadphones()
    {
        analog.update();

        if (analog.hasChanged())
        {
            float newValue = (float)analog.getValue() / (float)1023;

            if (abs(currentHeadphoneVolume - newValue) >= 0.05)
            {
                currentHeadphoneVolume = newValue;

                sgtl5000_1.volume(currentHeadphoneVolume);
            }
        }
    }

    void logMetrics()
    {
        Serial.printf(
            "Memory: %d/%d CPU: %d/%d",
            AudioMemoryUsage(),
            AudioMemoryUsageMax(),
            AudioProcessorUsage(),
            AudioProcessorUsageMax());
    }

    void resetMetrics()
    {
        AudioMemoryUsageMaxReset();
        AudioProcessorUsageMaxReset();
    }
}