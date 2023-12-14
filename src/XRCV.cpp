#include <XRCV.h>

namespace XRCV
{
    void init()
    {
        SPI1.begin();
        SPI1.setMOSI(DAC_MOSI);
        SPI1.setSCK(DAC_SCK);

        pinMode(CS1, OUTPUT);
        digitalWrite(CS1, HIGH);
        pinMode(CS2, OUTPUT);
        digitalWrite(CS2, HIGH);
        pinMode(CS3, OUTPUT);
        digitalWrite(CS3, HIGH);
        pinMode(CS4, OUTPUT);
        digitalWrite(CS4, HIGH);

        // initialize all DAC outputs at 0V
        write(CS1, 0, 0x000);
        write(CS1, 1, 0x000);
        write(CS2, 0, 0x000);
        write(CS2, 1, 0x000);
        write(CS3, 0, 0x000);
        write(CS3, 1, 0x000);
        write(CS4, 0, 0x000);
        write(CS4, 1, 0x000);
    }

    void write(int chip, int channel, int value)
    {
        const byte chan_a_low_gain = 0b00000000;
        const byte chan_b_low_gain = 0b10000000;
        const byte chan_a_high_gain = 0b00010000;
        const byte chan_b_high_gain = 0b10010000;

        byte lowValue = value;
        byte highValue = (value >> 8);

        SPI1.setDataMode(SPI_MODE0);
        digitalWrite(chip, LOW);

        if (value == 0x000)
        {
            // use low gain
            if (channel == 0)
            {
                SPI1.transfer(highValue | chan_a_low_gain);
            }
            else
            {
                SPI1.transfer(highValue | chan_b_low_gain);
            }
        }
        else
        {
            // use high gain
            if (channel == 0)
            {
                SPI1.transfer(highValue | chan_a_high_gain);
            }
            else
            {
                SPI1.transfer(highValue | chan_b_high_gain);
            }
        }

        SPI1.transfer(lowValue);
        digitalWrite(chip, HIGH);
    }
}