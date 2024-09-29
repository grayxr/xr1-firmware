#include <Arduino.h>
#include <XRMain.h>

void setup()
{
    // //**************************
    // //Reset QSPI clock from 88Mhz to 132 Mhz.
    //     CCM_CCGR7 |= CCM_CCGR7_FLEXSPI2(CCM_CCGR_OFF);
    //     CCM_CBCMR = (CCM_CBCMR & ~(CCM_CBCMR_FLEXSPI2_PODF_MASK | CCM_CBCMR_FLEXSPI2_CLK_SEL_MASK))
    //         | CCM_CBCMR_FLEXSPI2_PODF(4) | CCM_CBCMR_FLEXSPI2_CLK_SEL(2); // 528/5 = 132 MHz
    //     CCM_CCGR7 |= CCM_CCGR7_FLEXSPI2(CCM_CCGR_ON);
    // //**************************

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