#include <Arduino.h>
#include <XRMain.h>

void setup()
{
    Serial.begin(9600);

    if (CrashReport)
    {
        Serial.print(CrashReport);
    }

    XRMain::boot();
}

void loop(void)
{
    XRMain::update();
}