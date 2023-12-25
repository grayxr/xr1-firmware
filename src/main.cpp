#include <Arduino.h>
#include <XRMain.h>

void setup()
{
    Serial.begin(9600);
#ifndef BUILD_FOR_LINUX
    if (CrashReport)
    {
        Serial.print(CrashReport);
    }
#endif

    XRMain::boot();
}

void loop(void)
{
    XRMain::update();
}

#ifdef BUILD_FOR_LINUX
int st7735_main(int argc, char** argv) {
    SD.setSDCardFolderPath("/Users/nicholasnewdigate/Audio/dev-audio");
    return 0;
}
#endif