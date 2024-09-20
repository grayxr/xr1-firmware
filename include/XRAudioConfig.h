#ifndef XRAudioConfig_h
#define XRAudioConfig_h

#include <Arduino.h>
#include <Audio.h>
#include <TeensyVariablePlayback.h>
#include <synth_dexed.h>
#include <synth_fm_drum.h>
//#include <synth_braids.h>

// GUItool: begin automatically generated code
extern AudioPlaySdResmp         monoSample3; //xy=357.73807525634766,3071.428740501404
extern AudioPlaySdResmp         monoSample2; //xy=360.59522247314453,2910.000029563904
extern AudioPlaySdResmp         monoSample4;  //xy=359.4523391723633,3232.000160217285
extern AudioPlaySdResmp         monoSample1; //xy=362.21429443359375,2745.14293384552
extern AudioSynthWaveformDc     monoSynthDc2;   //xy=411.0000686645508,385.0000114440918
extern AudioSynthWaveformDc     monoSynthDc1;   //xy=413.0000686645508,189.0000114440918
extern AudioSynthWaveformDc     monoSynthDc3;   //xy=413.0000686645508,583.0000114440918
extern AudioPlaySdResmp         monoSample11; //xy=398.5714645385742,4214.428743362427
extern AudioSynthWaveformDc     monoSynthDc4;   //xy=413.0000686645508,780.0000114440918
extern AudioPlaySdResmp         monoSample10; //xy=401.4286117553711,4053.0000324249268
extern AudioSynthWaveform       monoSynthOscA4; //xy=415.0000686645508,663.0000114440918
extern AudioPlaySdResmp         monoSample12; //xy=400.28572845458984,4375.000163078308
extern AudioSynthWaveform       monoSynthOscA2; //xy=417.0000686645508,265.0000114440918
extern AudioSynthWaveform       monoSynthOscB2; //xy=417.0000686645508,306.0000114440918
extern AudioPlaySdResmp         monoSample9; //xy=403.0476837158203,3888.142936706543
extern AudioSynthNoiseWhite     monoSynthNoise2; //xy=419.0000686645508,346.0000114440918
extern AudioSynthWaveform       monoSynthOscB4; //xy=418.0000686645508,701.0000114440918
extern AudioSynthWaveform       monoSynthOscA1; //xy=421.0000686645508,70.0000114440918
extern AudioSynthWaveform       monoSynthOscB1; //xy=421.0000686645508,110.0000114440918
extern AudioSynthNoiseWhite     monoSynthNoise1; //xy=421.0000686645508,150.0000114440918
extern AudioSynthWaveform       monoSynthOscA3; //xy=420.0000686645508,466.0000114440918
extern AudioSynthWaveform       monoSynthOscB3; //xy=420.0000686645508,505.0000114440918
extern AudioSynthNoiseWhite     monoSynthNoise3; //xy=421.0000686645508,544.0000114440918
extern AudioSynthNoiseWhite     monoSynthNoise4; //xy=421.0000686645508,740.0000114440918
extern AudioEffectEnvelope      monoSampleAmpEnv4; //xy=574.1666488647461,3227.000029563904
extern AudioEffectEnvelope      monoSampleAmpEnv3; //xy=575.1666488647461,3067.000029563904
extern AudioEffectEnvelope      monoSampleAmpEnv2; //xy=577.1666488647461,2904.000029563904
extern AudioEffectEnvelope      monoSampleAmpEnv1; //xy=578.1666488647461,2739.000029563904
extern AudioAmplifier           monoSampleAmpAccent4; //xy=584.1666488647461,3273.000029563904
extern AudioAmplifier           monoSampleAmpAccent3; //xy=586.1666488647461,3113.000029563904
extern AudioAmplifier           monoSampleAmpAccent2; //xy=588.1666488647461,2950.000029563904
extern AudioAmplifier           monoSampleAmpAccent1; //xy=589.1666488647461,2785.000029563904
extern AudioAmplifier           monoSampleAmp4; //xy=589.1666488647461,3320.000029563904
extern AudioAmplifier           monoSampleAmp3; //xy=591.1666488647461,3160.000029563904
extern AudioAmplifier           monoSampleAmp2; //xy=593.1666488647461,2997.000029563904
extern AudioAmplifier           monoSampleAmp1; //xy=594.1666488647461,2832.000029563904
extern AudioEffectEnvelope      monoSampleAmpEnv12; //xy=615.0000381469727,4370.000032424927
extern AudioEffectEnvelope      monoSampleAmpEnv11; //xy=616.0000381469727,4210.000032424927
extern AudioEffectEnvelope      monoSampleAmpEnv10; //xy=618.0000381469727,4047.0000324249268
extern AudioEffectEnvelope      monoSampleAmpEnv9; //xy=619.0000381469727,3882.0000324249268
extern AudioAmplifier           monoSampleAmpAccent12; //xy=625.0000381469727,4416.000032424927
extern AudioAmplifier           monoSampleAmpAccent11; //xy=627.0000381469727,4256.000032424927
extern AudioAmplifier           monoSampleAmpAccent10; //xy=629.0000381469727,4093.0000324249268
extern AudioAmplifier           monoSampleAmpAccent9; //xy=630.0000381469727,3928.0000324249268
extern AudioAmplifier           monoSampleAmp12; //xy=630.0000381469727,4463.000032424927
extern AudioAmplifier           monoSampleAmp11; //xy=632.0000381469727,4303.000032424927
extern AudioAmplifier           monoSampleAmp10; //xy=634.0000381469727,4140.000032424927
extern AudioAmplifier           monoSampleAmp9; //xy=635.0000381469727,3975.0000324249268
extern AudioMixer4              monoSynthMix2;  //xy=683.0000686645508,284.0000114440918
extern AudioMixer4              monoSynthMix1;  //xy=685.0000686645508,88.0000114440918
extern AudioMixer4              monoSynthMix3;  //xy=685.0000686645508,482.0000114440918
extern AudioMixer4              monoSynthMix4;  //xy=685.0000686645508,679.0000114440918
extern AudioEffectEnvelope      monoSynthFilterEnv2; //xy=701.0000686645508,339.0000114440918
extern AudioEffectEnvelope      monoSynthFilterEnv1; //xy=703.0000686645508,143.0000114440918
extern AudioEffectEnvelope      monoSynthFilterEnv3; //xy=703.0000686645508,537.0000114440918
extern AudioEffectEnvelope      monoSynthFilterEnv4; //xy=703.0000686645508,734.0000114440918
extern AudioAmplifier           monoSynthFilterAccent2; //xy=710.0000686645508,378.0000114440918
extern AudioAmplifier           monoSynthFilterAccent1; //xy=713.0000686645508,182.0000114440918
extern AudioAmplifier           monoSynthFilterAccent3; //xy=712.0000686645508,576.0000114440918
extern AudioAmplifier           monoSynthFilterAccent4; //xy=712.0000686645508,773.0000114440918
extern AudioAmplifier           monoSampleLeft4; //xy=817.1666488647461,3297.000029563904
extern AudioAmplifier           monoSampleLeft3; //xy=819.1666488647461,3137.000029563904
extern AudioAmplifier           monoSampleLeft2; //xy=820.1666488647461,2974.000029563904
extern AudioAmplifier           monoSampleLeft1; //xy=822.1666488647461,2809.000029563904
extern AudioAmplifier           monoSampleRight4; //xy=821.1666488647461,3336.000029563904
extern AudioAmplifier           monoSampleRight3; //xy=822.1666488647461,3176.000029563904
extern AudioAmplifier           monoSampleRight2; //xy=824.1666488647461,3013.000029563904
extern AudioAmplifier           monoSampleRight1; //xy=825.1666488647461,2848.000029563904
extern AudioAmplifier           monoSampleLeft12; //xy=858.0000381469727,4440.000032424927
extern AudioAmplifier           monoSampleLeft11; //xy=860.0000381469727,4280.000032424927
extern AudioAmplifier           monoSampleLeft10; //xy=861.0000381469727,4117.000032424927
extern AudioAmplifier           monoSampleLeft9; //xy=863.0000381469727,3952.0000324249268
extern AudioAmplifier           monoSampleRight12; //xy=862.0000381469727,4479.000032424927
extern AudioAmplifier           monoSampleRight11; //xy=863.0000381469727,4319.000032424927
extern AudioAmplifier           monoSampleRight10; //xy=865.0000381469727,4156.000032424927
extern AudioAmplifier           monoSampleRight9; //xy=866.0000381469727,3991.0000324249268
extern AudioFilterLadder        monoSynthFilter2; //xy=963.0000686645508,336.0000114440918
extern AudioFilterLadder        monoSynthFilter1; //xy=965.0000686645508,140.0000114440918
extern AudioFilterLadder        monoSynthFilter3; //xy=965.0000686645508,535.0000114440918
extern AudioFilterLadder        monoSynthFilter4; //xy=965.0000686645508,731.0000114440918
extern AudioMixer4              voiceSubMixLeft5; //xy=1128.166648864746,3025.000029563904
extern AudioMixer4              voiceSubMixRight5; //xy=1132.166648864746,3093.000029563904
extern AudioAmplifier           monoSampleAmpDelaySend1; //xy=1156.5,2736.000277519226
extern AudioAmplifier           monoSampleAmpDelaySend2; //xy=1157.5,2776.000277519226
extern AudioAmplifier           monoSampleAmpDelaySend3; //xy=1157.5,2816.000277519226
extern AudioAmplifier           monoSampleAmpDelaySend4; //xy=1159.5,2856.000277519226
extern AudioMixer4              voiceSubMixLeft7; //xy=1169.0000381469727,4168.000032424927
extern AudioMixer4              voiceSubMixRight7; //xy=1173.0000381469727,4236.000032424927
extern AudioAmplifier           monoSampleAmpDelaySend9; //xy=1197.3333892822266,3879.000280380249
extern AudioAmplifier           monoSynthAmp2;  //xy=1212.0000686645508,361.0000114440918
extern AudioAmplifier           monoSampleAmpDelaySend10; //xy=1198.3333892822266,3919.000280380249
extern AudioAmplifier           monoSampleAmpDelaySend11; //xy=1198.3333892822266,3959.000280380249
extern AudioAmplifier           monoSynthAmp1;  //xy=1214.0000686645508,166.0000114440918
extern AudioAmplifier           monoSynthAmp3;  //xy=1214.0000686645508,560.0000114440918
extern AudioAmplifier           monoSampleAmpDelaySend12; //xy=1200.3333892822266,3999.000280380249
extern AudioAmplifier           monoSynthAmp4;  //xy=1214.0000686645508,756.0000114440918
extern AudioEffectEnvelope      monoSynthAmpEnv2; //xy=1225.0000686645508,285.0000114440918
extern AudioEffectEnvelope      monoSynthAmpEnv1; //xy=1227.0000686645508,89.0000114440918
extern AudioEffectEnvelope      monoSynthAmpEnv3; //xy=1227.0000686645508,483.0000114440918
extern AudioEffectEnvelope      monoSynthAmpEnv4; //xy=1227.0000686645508,680.0000114440918
extern AudioAmplifier           monoSynthAmpAccent2; //xy=1234.0000686645508,323.0000114440918
extern AudioAmplifier           monoSynthAmpAccent1; //xy=1236.0000686645508,127.0000114440918
extern AudioAmplifier           monoSynthAmpAccent3; //xy=1236.0000686645508,521.0000114440918
extern AudioAmplifier           monoSynthAmpAccent4; //xy=1236.0000686645508,718.0000114440918
extern AudioSynthDexed          dexed7;         //xy=1270.8333587646484,1706.0000743865967
extern AudioSynthDexed          dexed5;         //xy=1274.8333587646484,1448.0000743865967
extern AudioSynthDexed          dexed6;         //xy=1274.8333587646484,1579.0000743865967
extern AudioSynthDexed          dexed8;         //xy=1273.8333587646484,1836.0000743865967
extern AudioSynthDexed          dexed3;         //xy=1279.0000228881836,1154.000023841858
extern AudioSynthDexed          dexed1;         //xy=1283.0000228881836,896.0000238418579
extern AudioSynthDexed          dexed2;         //xy=1283.0000228881836,1027.000023841858
extern AudioSynthDexed          dexed4;         //xy=1282.0000228881836,1284.000023841858
extern AudioSynthFMDrum         fmDrum3;        //xy=1278.0000228881836,2295.333291053772
extern AudioSynthFMDrum         fmDrum2;        //xy=1283.0000228881836,2151.333291053772
extern AudioSynthFMDrum         fmDrum1;        //xy=1288.0000228881836,2006.333291053772
extern AudioSynthFMDrum         fmDrum4;        //xy=1289.0000228881836,2432.333291053772
extern AudioAmplifier           dexedAmp7;      //xy=1413.8333587646484,1748.0000743865967
extern AudioAmplifier           dexedAmp6;      //xy=1416.8333587646484,1621.0000743865967
extern AudioAmplifier           dexedAmp5;      //xy=1417.8333587646484,1490.0000743865967
extern AudioAmplifier           dexedAmp8;      //xy=1416.8333587646484,1878.0000743865967
extern AudioAmplifier           dexedAmp3;      //xy=1422.0000228881836,1196.000023841858
extern AudioAmplifier           dexedAmp2;      //xy=1425.0000228881836,1069.000023841858
extern AudioAmplifier           dexedAmp1;      //xy=1426.0000228881836,938.0000238418579
extern AudioAmplifier           dexedAmp4;      //xy=1425.0000228881836,1326.000023841858
extern AudioAmplifier           dexedAmpAccent7; //xy=1434.8333587646484,1705.0000743865967
extern AudioAmplifier           dexedAmpAccent6; //xy=1438.8333587646484,1578.0000743865967
extern AudioAmplifier           dexedAmpAccent5; //xy=1439.8333587646484,1447.0000743865967
extern AudioAmplifier           dexedAmpAccent8; //xy=1438.8333587646484,1835.0000743865967
extern AudioAmplifier           dexedAmpAccent3; //xy=1443.0000228881836,1153.000023841858
extern AudioAmplifier           dexedAmpAccent2; //xy=1447.0000228881836,1026.000023841858
extern AudioAmplifier           dexedAmpAccent1; //xy=1448.0000228881836,895.0000238418579
extern AudioAmplifier           dexedAmpAccent4; //xy=1447.0000228881836,1283.000023841858
extern AudioPlaySdResmp         monoSample7; //xy=1448.5714492797852,3499.4286823272705
extern AudioPlaySdResmp         monoSample6; //xy=1451.428596496582,3337.9999713897705
extern AudioPlaySdResmp         monoSample8; //xy=1450.2857131958008,3660.000102043152
extern AudioPlaySdResmp         monoSample5; //xy=1453.0476684570312,3173.1428756713867
extern AudioAmplifier           fmDrumAmpAccent3; //xy=1457.0000228881836,2295.333291053772
extern AudioAmplifier           fmDrumAmp3;     //xy=1459.0000228881836,2343.333291053772
extern AudioAmplifier           fmDrumAmpAccent2; //xy=1462.0000228881836,2152.333291053772
extern AudioPlaySdResmp         monoSample15; //xy=1454.4049911499023,4592.428749084473
extern AudioAmplifier           fmDrumAmp2;     //xy=1464.0000228881836,2200.333291053772
extern AudioPlaySdResmp         monoSample14; //xy=1457.2621383666992,4431.000038146973
extern AudioAmplifier           fmDrumAmpAccent1; //xy=1467.0000228881836,2006.333291053772
extern AudioPlaySdResmp         monoSample16; //xy=1456.119255065918,4753.000168800354
extern AudioPlaySdResmp         monoSample13; //xy=1458.8812103271484,4266.142942428589
extern AudioAmplifier           fmDrumAmp1;     //xy=1469.0000228881836,2055.333291053772
extern AudioAmplifier           fmDrumAmpAccent4; //xy=1468.0000228881836,2432.333291053772
extern AudioAmplifier           fmDrumAmp4;     //xy=1470.0000228881836,2480.333291053772
extern AudioAmplifier           monoSynthLeft2; //xy=1530.0000686645508,321.0000114440918
extern AudioAmplifier           monoSynthLeft1; //xy=1532.0000686645508,125.0000114440918
extern AudioAmplifier           monoSynthLeft3; //xy=1532.0000686645508,519.0000114440918
extern AudioAmplifier           monoSynthLeft4; //xy=1532.0000686645508,716.0000114440918
extern AudioAmplifier           monoSynthRight2; //xy=1534.0000686645508,358.0000114440918
extern AudioAmplifier           monoSynthRight1; //xy=1536.0000686645508,162.0000114440918
extern AudioAmplifier           monoSynthRight3; //xy=1536.0000686645508,556.0000114440918
extern AudioAmplifier           monoSynthRight4; //xy=1536.0000686645508,753.0000114440918
extern AudioAmplifier           fmDrumLeft3;    //xy=1654.0000228881836,2325.333291053772
extern AudioAmplifier           fmDrumLeft2;    //xy=1659.0000228881836,2182.333291053772
extern AudioAmplifier           fmDrumRight3;   //xy=1659.0000228881836,2364.333291053772
extern AudioAmplifier           dexedLeft7;     //xy=1662.8333587646484,1732.0000743865967
extern AudioAmplifier           fmDrumLeft1;    //xy=1664.0000228881836,2036.333291053772
extern AudioAmplifier           fmDrumRight2;   //xy=1664.0000228881836,2220.333291053772
extern AudioAmplifier           dexedLeft6;     //xy=1666.8333587646484,1604.0000743865967
extern AudioAmplifier           dexedLeft5;     //xy=1667.8333587646484,1474.0000743865967
extern AudioAmplifier           dexedLeft8;     //xy=1666.8333587646484,1861.0000743865967
extern AudioAmplifier           fmDrumLeft4;    //xy=1665.0000228881836,2462.333291053772
extern AudioAmplifier           dexedLeft3;     //xy=1671.0000228881836,1180.000023841858
extern AudioAmplifier           dexedRight7;    //xy=1668.8333587646484,1769.0000743865967
extern AudioAmplifier           fmDrumRight1;   //xy=1669.0000228881836,2075.333291053772
extern AudioAmplifier           dexedRight6;    //xy=1671.8333587646484,1642.0000743865967
extern AudioAmplifier           dexedRight5;    //xy=1672.8333587646484,1511.0000743865967
extern AudioAmplifier           dexedLeft2;     //xy=1675.0000228881836,1052.000023841858
extern AudioAmplifier           dexedRight8;    //xy=1671.8333587646484,1899.0000743865967
extern AudioEffectEnvelope      monoSampleAmpEnv8; //xy=1665.0000228881836,3654.9999713897705
extern AudioAmplifier           dexedLeft1;     //xy=1676.0000228881836,922.0000238418579
extern AudioEffectEnvelope      monoSampleAmpEnv7; //xy=1666.0000228881836,3494.9999713897705
extern AudioAmplifier           fmDrumRight4;   //xy=1670.0000228881836,2501.333291053772
extern AudioAmplifier           dexedLeft4;     //xy=1675.0000228881836,1309.000023841858
extern AudioEffectEnvelope      monoSampleAmpEnv6; //xy=1668.0000228881836,3331.9999713897705
extern AudioEffectEnvelope      monoSampleAmpEnv5; //xy=1669.0000228881836,3166.9999713897705
extern AudioAmplifier           dexedRight3;    //xy=1677.0000228881836,1217.000023841858
extern AudioAmplifier           dexedRight2;    //xy=1680.0000228881836,1090.000023841858
extern AudioAmplifier           dexedRight1;    //xy=1681.0000228881836,959.0000238418579
extern AudioAmplifier           dexedRight4;    //xy=1680.0000228881836,1347.000023841858
extern AudioAmplifier           monoSampleAmpAccent8; //xy=1675.0000228881836,3700.9999713897705
extern AudioEffectEnvelope      monoSampleAmpEnv16; //xy=1670.8335647583008,4748.000038146973
extern AudioEffectEnvelope      monoSampleAmpEnv15; //xy=1671.8335647583008,4588.000038146973
extern AudioAmplifier           monoSampleAmpAccent7; //xy=1677.0000228881836,3540.9999713897705
extern AudioEffectEnvelope      monoSampleAmpEnv14; //xy=1673.8335647583008,4425.000038146973
extern AudioEffectEnvelope      monoSampleAmpEnv13; //xy=1674.8335647583008,4260.000038146973
extern AudioAmplifier           monoSampleAmpAccent6; //xy=1679.0000228881836,3377.9999713897705
extern AudioAmplifier           monoSampleAmpAccent5; //xy=1680.0000228881836,3212.9999713897705
extern AudioAmplifier           monoSampleAmp8; //xy=1680.0000228881836,3747.9999713897705
extern AudioAmplifier           monoSampleAmp7; //xy=1682.0000228881836,3587.9999713897705
extern AudioAmplifier           monoSampleAmp6; //xy=1684.0000228881836,3424.9999713897705
extern AudioAmplifier           monoSampleAmp5; //xy=1685.0000228881836,3259.9999713897705
extern AudioAmplifier           monoSampleAmpAccent16; //xy=1680.8335647583008,4794.000038146973
extern AudioAmplifier           monoSampleAmpAccent15; //xy=1682.8335647583008,4634.000038146973
extern AudioAmplifier           monoSampleAmpAccent14; //xy=1684.8335647583008,4471.000038146973
extern AudioAmplifier           monoSampleAmpAccent13; //xy=1685.8335647583008,4306.000038146973
extern AudioAmplifier           monoSampleAmp16; //xy=1685.8335647583008,4841.000038146973
extern AudioAmplifier           monoSampleAmp15; //xy=1687.8335647583008,4681.000038146973
extern AudioAmplifier           monoSampleAmp14; //xy=1689.8335647583008,4518.000038146973
extern AudioAmplifier           monoSampleAmp13; //xy=1690.8335647583008,4353.000038146973
extern AudioAmplifier           monoSampleLeft8; //xy=1908.0000228881836,3724.9999713897705
extern AudioAmplifier           monoSampleLeft7; //xy=1910.0000228881836,3564.9999713897705
extern AudioAmplifier           monoSampleLeft6; //xy=1911.0000228881836,3401.9999713897705
extern AudioAmplifier           monoSampleLeft5; //xy=1913.0000228881836,3236.9999713897705
extern AudioAmplifier           monoSampleRight8; //xy=1912.0000228881836,3763.9999713897705
extern AudioAmplifier           monoSampleRight7;  //xy=1913.0000228881836,3603.9999713897705
extern AudioAmplifier           monoSampleRight6; //xy=1915.0000228881836,3440.9999713897705
extern AudioAmplifier           monoSampleRight5; //xy=1916.0000228881836,3275.9999713897705
extern AudioAmplifier           monoSampleLeft16; //xy=1913.8335647583008,4818.000038146973
extern AudioAmplifier           monoSampleLeft15; //xy=1915.8335647583008,4658.000038146973
extern AudioAmplifier           monoSampleLeft14; //xy=1916.8335647583008,4495.000038146973
extern AudioAmplifier           monoSampleLeft13; //xy=1918.8335647583008,4330.000038146973
extern AudioAmplifier           monoSampleRight16; //xy=1917.8335647583008,4857.000038146973
extern AudioAmplifier           monoSampleRight15; //xy=1918.8335647583008,4697.000038146973
extern AudioAmplifier           monoSampleRight14; //xy=1920.8335647583008,4534.000038146973
extern AudioAmplifier           monoSampleRight13; //xy=1921.8335647583008,4369.000038146973
extern AudioAmplifier           dexedAmpDelaySend1; //xy=2084.000030517578,909.0000214576721
extern AudioAmplifier           dexedAmpDelaySend2; //xy=2084.000030517578,948.0000214576721
extern AudioAmplifier           dexedAmpDelaySend4; //xy=2084.000030517578,1028.0000214576721
extern AudioAmplifier           dexedAmpDelaySend3; //xy=2085.000030517578,988.0000214576721
extern AudioAmplifier           dexedAmpDelaySend5; //xy=2083.8334045410156,1490.500147819519
extern AudioAmplifier           dexedAmpDelaySend6; //xy=2083.8334045410156,1529.500147819519
extern AudioAmplifier           dexedAmpDelaySend8; //xy=2083.8334045410156,1609.500147819519
extern AudioAmplifier           dexedAmpDelaySend7; //xy=2084.8334045410156,1569.500147819519
extern AudioMixer4              voiceSubMixLeft3; //xy=2085.6905822753906,1709.6191682815552
extern AudioMixer4              voiceSubMixLeft2; //xy=2088.000030517578,1148.0000214576721
extern AudioMixer4              voiceSubMixLeft4; //xy=2087.000030517578,2282.3332872390747
extern AudioAmplifier           fmDrumAmpDelaySend4; //xy=2088.000030517578,2173.3332872390747
extern AudioMixer4              voiceSubMixRight2; //xy=2092.000030517578,1216.0000214576721
extern AudioMixer4              voiceSubMixRight3; //xy=2090.0477600097656,1778.6907014846802
extern AudioAmplifier           fmDrumAmpDelaySend1; //xy=2089.000030517578,2047.3332872390747
extern AudioAmplifier           fmDrumAmpDelaySend2; //xy=2089.000030517578,2089.3332872390747
extern AudioAmplifier           fmDrumAmpDelaySend3; //xy=2089.000030517578,2131.3332872390747
extern AudioMixer4              voiceSubMixRight4; //xy=2091.000030517578,2350.3332872390747
extern AudioAmplifier           monoSynthAmpDelaySend1; //xy=2102.000030517578,141.00002145767212
extern AudioAmplifier           monoSynthAmpDelaySend2; //xy=2102.000030517578,180.00002145767212
extern AudioAmplifier           monoSynthAmpDelaySend3; //xy=2104.000030517578,220.00002145767212
extern AudioAmplifier           monoSynthAmpDelaySend4; //xy=2104.000030517578,261.0000214576721
extern AudioMixer4              voiceSubMixLeft1; //xy=2106.000030517578,458.0000214576721
extern AudioMixer4              voiceSubMixRight1; //xy=2110.000030517578,526.0000214576721
extern AudioMixer4              voiceSubMixLeft6;  //xy=2219.0000228881836,3452.9999713897705
extern AudioMixer4              voiceSubMixRight6; //xy=2223.0000228881836,3520.9999713897705
extern AudioMixer4              voiceSubMixLeft8; //xy=2224.833564758301,4546.000038146973
extern AudioMixer4              voiceSubMixRight8;  //xy=2228.833564758301,4614.000038146973
extern AudioAmplifier           monoSampleAmpDelaySend5; //xy=2247.3333740234375,3164.000219345093
extern AudioAmplifier           monoSampleAmpDelaySend6; //xy=2248.3333740234375,3204.000219345093
extern AudioAmplifier           monoSampleAmpDelaySend7; //xy=2248.3333740234375,3244.000219345093
extern AudioAmplifier           monoSampleAmpDelaySend8; //xy=2250.3333740234375,3284.000219345093
extern AudioAmplifier           monoSampleAmpDelaySend13; //xy=2253.1669158935547,4257.000286102295
extern AudioAmplifier           monoSampleAmpDelaySend14; //xy=2254.1669158935547,4297.000286102295
extern AudioAmplifier           monoSampleAmpDelaySend15; //xy=2254.1669158935547,4337.000286102295
extern AudioAmplifier           monoSampleAmpDelaySend16; //xy=2256.1669158935547,4377.000286102295
extern AudioMixer4              delaySubMix5;   //xy=2665.0000610351562,3083.3335132598877
extern AudioMixer4              delaySubMix6;   //xy=2673.000068664551,3446.667013168335
extern AudioMixer4              delaySubMix7;   //xy=2673.0000648498535,3814.000270843506
extern AudioMixer4              delaySubMix8;   //xy=2678.857490539551,4259.571321487427
extern AudioMixer4              delaySubMix4;   //xy=3025.3332901000977,780.0001521110535
extern AudioMixer4              delaySubMix3;   //xy=3027.6667861938477,613.1666884422302
extern AudioMixer4              delaySubMix2;   //xy=3030.6667861938477,406.6666798591614
extern AudioMixer4              delaySubMix1;   //xy=3033.666732788086,228.66667699813843
extern AudioMixer4              delayMix1;      //xy=3490.6668014526367,976.6667671203613
extern AudioMixer4              voiceMixLeft1;  //xy=3506.666790008545,1565.000020980835
extern AudioMixer4              voiceMixRight1; //xy=3510.666790008545,1632.000020980835
extern AudioMixer4              delayMix2;      //xy=3520.0000610351562,3162.333522796631
extern AudioMixer4              voiceMixLeft2;  //xy=3533.6670455932617,3857.333553314209
extern AudioMixer4              voiceMixRight2; //xy=3540.6670455932617,3924.333553314209
extern AudioInputI2S            i2s2;           //xy=4032.0000381469727,2884.000062942505
extern AudioAnalyzePeak         peak_left;      //xy=4038.0000381469727,2836.000062942505
extern AudioAnalyzePeak         peak_right;     //xy=4042.0000381469727,2936.000062942505
extern AudioEffectDelay         delay1;         //xy=4045.0000381469727,2204.000062942505
extern AudioSynthSimpleDrum     metronome;          //xy=4054.7776947021484,2587.1267318725586
extern AudioMixer4              mainMixerLeft;  //xy=4067.0000381469727,2459.000062942505
extern AudioMixer4              mainMixerRight; //xy=4070.0000381469727,2531.000062942505
extern AudioAmplifier           delayLeftAmp1;  //xy=4074.0000381469727,2296.000062942505
extern AudioAmplifier           delayRightAmp1; //xy=4075.0000381469727,2336.000062942505
extern AudioMixer4              delayFeedbackMixer1; //xy=4088.0000381469727,2096.000062942505
extern AudioMixer4              inputMixerLeft; //xy=4238.000038146973,2854.000062942505
extern AudioMixer4              inputMixerRight; //xy=4240.000038146973,2929.000062942505
extern AudioMixer4              OutputMixerLeft; //xy=4498.000038146973,2803.000062942505
extern AudioMixer4              OutputMixerRight; //xy=4503.000038146973,2874.000062942505
extern AudioOutputI2S           i2s1;           //xy=4694.000038146973,2845.000062942505

extern AudioConnection          patchCord1;
extern AudioConnection          patchCord2;
extern AudioConnection          patchCord3;
extern AudioConnection          patchCord4;
extern AudioConnection          patchCord5;
extern AudioConnection          patchCord6;
extern AudioConnection          patchCord7;
extern AudioConnection          patchCord8;
extern AudioConnection          patchCord9;
extern AudioConnection          patchCord10;
extern AudioConnection          patchCord11;
extern AudioConnection          patchCord12;
extern AudioConnection          patchCord13;
extern AudioConnection          patchCord14;
extern AudioConnection          patchCord15;
extern AudioConnection          patchCord16;
extern AudioConnection          patchCord17;
extern AudioConnection          patchCord18;
extern AudioConnection          patchCord19;
extern AudioConnection          patchCord20;
extern AudioConnection          patchCord21;
extern AudioConnection          patchCord22;
extern AudioConnection          patchCord23;
extern AudioConnection          patchCord24;
extern AudioConnection          patchCord25;
extern AudioConnection          patchCord26;
extern AudioConnection          patchCord27;
extern AudioConnection          patchCord28;
extern AudioConnection          patchCord29;
extern AudioConnection          patchCord30;
extern AudioConnection          patchCord31;
extern AudioConnection          patchCord32;
extern AudioConnection          patchCord33;
extern AudioConnection          patchCord34;
extern AudioConnection          patchCord35;
extern AudioConnection          patchCord36;
extern AudioConnection          patchCord37;
extern AudioConnection          patchCord38;
extern AudioConnection          patchCord39;
extern AudioConnection          patchCord40;
extern AudioConnection          patchCord41;
extern AudioConnection          patchCord42;
extern AudioConnection          patchCord43;
extern AudioConnection          patchCord44;
extern AudioConnection          patchCord45;
extern AudioConnection          patchCord46;
extern AudioConnection          patchCord47;
extern AudioConnection          patchCord48;
extern AudioConnection          patchCord49;
extern AudioConnection          patchCord50;
extern AudioConnection          patchCord51;
extern AudioConnection          patchCord52;
extern AudioConnection          patchCord53;
extern AudioConnection          patchCord54;
extern AudioConnection          patchCord55;
extern AudioConnection          patchCord56;
extern AudioConnection          patchCord57;
extern AudioConnection          patchCord58;
extern AudioConnection          patchCord59;
extern AudioConnection          patchCord60;
extern AudioConnection          patchCord61;
extern AudioConnection          patchCord62;
extern AudioConnection          patchCord63;
extern AudioConnection          patchCord64;
extern AudioConnection          patchCord65;
extern AudioConnection          patchCord66;
extern AudioConnection          patchCord67;
extern AudioConnection          patchCord68;
extern AudioConnection          patchCord69;
extern AudioConnection          patchCord70;
extern AudioConnection          patchCord71;
extern AudioConnection          patchCord72;
extern AudioConnection          patchCord73;
extern AudioConnection          patchCord74;
extern AudioConnection          patchCord75;
extern AudioConnection          patchCord76;
extern AudioConnection          patchCord77;
extern AudioConnection          patchCord78;
extern AudioConnection          patchCord79;
extern AudioConnection          patchCord80;
extern AudioConnection          patchCord81;
extern AudioConnection          patchCord82;
extern AudioConnection          patchCord83;
extern AudioConnection          patchCord84;
extern AudioConnection          patchCord85;
extern AudioConnection          patchCord86;
extern AudioConnection          patchCord87;
extern AudioConnection          patchCord88;
extern AudioConnection          patchCord89;
extern AudioConnection          patchCord90;
extern AudioConnection          patchCord91;
extern AudioConnection          patchCord92;
extern AudioConnection          patchCord93;
extern AudioConnection          patchCord94;
extern AudioConnection          patchCord95;
extern AudioConnection          patchCord96;
extern AudioConnection          patchCord97;
extern AudioConnection          patchCord98;
extern AudioConnection          patchCord99;
extern AudioConnection          patchCord100;
extern AudioConnection          patchCord101;
extern AudioConnection          patchCord102;
extern AudioConnection          patchCord103;
extern AudioConnection          patchCord104;
extern AudioConnection          patchCord105;
extern AudioConnection          patchCord106;
extern AudioConnection          patchCord107;
extern AudioConnection          patchCord108;
extern AudioConnection          patchCord109;
extern AudioConnection          patchCord110;
extern AudioConnection          patchCord111;
extern AudioConnection          patchCord112;
extern AudioConnection          patchCord113;
extern AudioConnection          patchCord114;
extern AudioConnection          patchCord115;
extern AudioConnection          patchCord116;
extern AudioConnection          patchCord117;
extern AudioConnection          patchCord118;
extern AudioConnection          patchCord119;
extern AudioConnection          patchCord120;
extern AudioConnection          patchCord121;
extern AudioConnection          patchCord122;
extern AudioConnection          patchCord123;
extern AudioConnection          patchCord124;
extern AudioConnection          patchCord125;
extern AudioConnection          patchCord126;
extern AudioConnection          patchCord127;
extern AudioConnection          patchCord128;
extern AudioConnection          patchCord129;
extern AudioConnection          patchCord130;
extern AudioConnection          patchCord131;
extern AudioConnection          patchCord132;
extern AudioConnection          patchCord133;
extern AudioConnection          patchCord134;
extern AudioConnection          patchCord135;
extern AudioConnection          patchCord136;
extern AudioConnection          patchCord137;
extern AudioConnection          patchCord138;
extern AudioConnection          patchCord139;
extern AudioConnection          patchCord140;
extern AudioConnection          patchCord141;
extern AudioConnection          patchCord142;
extern AudioConnection          patchCord143;
extern AudioConnection          patchCord144;
extern AudioConnection          patchCord145;
extern AudioConnection          patchCord146;
extern AudioConnection          patchCord147;
extern AudioConnection          patchCord148;
extern AudioConnection          patchCord149;
extern AudioConnection          patchCord150;
extern AudioConnection          patchCord151;
extern AudioConnection          patchCord152;
extern AudioConnection          patchCord153;
extern AudioConnection          patchCord154;
extern AudioConnection          patchCord155;
extern AudioConnection          patchCord156;
extern AudioConnection          patchCord157;
extern AudioConnection          patchCord158;
extern AudioConnection          patchCord159;
extern AudioConnection          patchCord160;
extern AudioConnection          patchCord161;
extern AudioConnection          patchCord162;
extern AudioConnection          patchCord163;
extern AudioConnection          patchCord164;
extern AudioConnection          patchCord165;
extern AudioConnection          patchCord166;
extern AudioConnection          patchCord167;
extern AudioConnection          patchCord168;
extern AudioConnection          patchCord169;
extern AudioConnection          patchCord170;
extern AudioConnection          patchCord171;
extern AudioConnection          patchCord172;
extern AudioConnection          patchCord173;
extern AudioConnection          patchCord174;
extern AudioConnection          patchCord175;
extern AudioConnection          patchCord176;
extern AudioConnection          patchCord177;
extern AudioConnection          patchCord178;
extern AudioConnection          patchCord179;
extern AudioConnection          patchCord180;
extern AudioConnection          patchCord181;
extern AudioConnection          patchCord182;
extern AudioConnection          patchCord183;
extern AudioConnection          patchCord184;
extern AudioConnection          patchCord185;
extern AudioConnection          patchCord186;
extern AudioConnection          patchCord187;
extern AudioConnection          patchCord188;
extern AudioConnection          patchCord189;
extern AudioConnection          patchCord190;
extern AudioConnection          patchCord191;
extern AudioConnection          patchCord192;
extern AudioConnection          patchCord193;
extern AudioConnection          patchCord194;
extern AudioConnection          patchCord195;
extern AudioConnection          patchCord196;
extern AudioConnection          patchCord197;
extern AudioConnection          patchCord198;
extern AudioConnection          patchCord199;
extern AudioConnection          patchCord200;
extern AudioConnection          patchCord201;
extern AudioConnection          patchCord202;
extern AudioConnection          patchCord203;
extern AudioConnection          patchCord204;
extern AudioConnection          patchCord205;
extern AudioConnection          patchCord206;
extern AudioConnection          patchCord207;
extern AudioConnection          patchCord208;
extern AudioConnection          patchCord209;
extern AudioConnection          patchCord210;
extern AudioConnection          patchCord211;
extern AudioConnection          patchCord212;
extern AudioConnection          patchCord213;
extern AudioConnection          patchCord214;
extern AudioConnection          patchCord215;
extern AudioConnection          patchCord216;
extern AudioConnection          patchCord217;
extern AudioConnection          patchCord218;
extern AudioConnection          patchCord219;
extern AudioConnection          patchCord220;
extern AudioConnection          patchCord221;
extern AudioConnection          patchCord222;
extern AudioConnection          patchCord223;
extern AudioConnection          patchCord224;
extern AudioConnection          patchCord225;
extern AudioConnection          patchCord226;
extern AudioConnection          patchCord227;
extern AudioConnection          patchCord228;
extern AudioConnection          patchCord229;
extern AudioConnection          patchCord230;
extern AudioConnection          patchCord231;
extern AudioConnection          patchCord232;
extern AudioConnection          patchCord233;
extern AudioConnection          patchCord234;
extern AudioConnection          patchCord235;
extern AudioConnection          patchCord236;
extern AudioConnection          patchCord237;
extern AudioConnection          patchCord238;
extern AudioConnection          patchCord239;
extern AudioConnection          patchCord240;
extern AudioConnection          patchCord241;
extern AudioConnection          patchCord242;
extern AudioConnection          patchCord243;
extern AudioConnection          patchCord244;
extern AudioConnection          patchCord245;
extern AudioConnection          patchCord246;
extern AudioConnection          patchCord247;
extern AudioConnection          patchCord248;
extern AudioConnection          patchCord249;
extern AudioConnection          patchCord250;
extern AudioConnection          patchCord251;
extern AudioConnection          patchCord252;
extern AudioConnection          patchCord253;
extern AudioConnection          patchCord254;
extern AudioConnection          patchCord255;
extern AudioConnection          patchCord256;
extern AudioConnection          patchCord257;
extern AudioConnection          patchCord258;
extern AudioConnection          patchCord259;
extern AudioConnection          patchCord260;
extern AudioConnection          patchCord261;
extern AudioConnection          patchCord262;
extern AudioConnection          patchCord263;
extern AudioConnection          patchCord264;
extern AudioConnection          patchCord265;
extern AudioConnection          patchCord266;
extern AudioConnection          patchCord267;
extern AudioConnection          patchCord268;
extern AudioConnection          patchCord269;
extern AudioConnection          patchCord270;
extern AudioConnection          patchCord271;
extern AudioConnection          patchCord272;
extern AudioConnection          patchCord273;
extern AudioConnection          patchCord274;
extern AudioConnection          patchCord275;
extern AudioConnection          patchCord276;
extern AudioConnection          patchCord277;
extern AudioConnection          patchCord278;
extern AudioConnection          patchCord279;
extern AudioConnection          patchCord280;
extern AudioConnection          patchCord281;
extern AudioConnection          patchCord282;
extern AudioConnection          patchCord283;
extern AudioConnection          patchCord284;
extern AudioConnection          patchCord285;
extern AudioConnection          patchCord286;
extern AudioConnection          patchCord287;
extern AudioConnection          patchCord288;
extern AudioConnection          patchCord289;
extern AudioConnection          patchCord290;
extern AudioConnection          patchCord291;
extern AudioConnection          patchCord292;
extern AudioConnection          patchCord293;
extern AudioConnection          patchCord294;
extern AudioConnection          patchCord295;
extern AudioConnection          patchCord296;
extern AudioConnection          patchCord297;
extern AudioConnection          patchCord298;
extern AudioConnection          patchCord299;
extern AudioConnection          patchCord300;
extern AudioConnection          patchCord301;
extern AudioConnection          patchCord302;
extern AudioConnection          patchCord303;
extern AudioConnection          patchCord304;
extern AudioConnection          patchCord305;
extern AudioConnection          patchCord306;
extern AudioConnection          patchCord307;
extern AudioConnection          patchCord308;
extern AudioConnection          patchCord309;
extern AudioConnection          patchCord310;
extern AudioConnection          patchCord311;
extern AudioConnection          patchCord312;
extern AudioConnection          patchCord313;
extern AudioConnection          patchCord314;
extern AudioConnection          patchCord315;
extern AudioConnection          patchCord316;
extern AudioConnection          patchCord317;
extern AudioConnection          patchCord318;
extern AudioConnection          patchCord319;
extern AudioConnection          patchCord320;
extern AudioConnection          patchCord321;
extern AudioConnection          patchCord322;
extern AudioConnection          patchCord323;
extern AudioConnection          patchCord324;
extern AudioConnection          patchCord325;
extern AudioConnection          patchCord326;
extern AudioConnection          patchCord327;
extern AudioConnection          patchCord328;
extern AudioConnection          patchCord329;
extern AudioConnection          patchCord330;
extern AudioConnection          patchCord331;
extern AudioConnection          patchCord332;
extern AudioConnection          patchCord333;
extern AudioConnection          patchCord334;
extern AudioConnection          patchCord335;
extern AudioConnection          patchCord336;
extern AudioConnection          patchCord337;
extern AudioConnection          patchCord338;
extern AudioConnection          patchCord339;
extern AudioConnection          patchCord340;
extern AudioConnection          patchCord341;
extern AudioConnection          patchCord342;
extern AudioConnection          patchCord343;
extern AudioConnection          patchCord344;
extern AudioConnection          patchCord345;
extern AudioConnection          patchCord346;
extern AudioConnection          patchCord347;
extern AudioConnection          patchCord348;
extern AudioConnection          patchCord349;
extern AudioConnection          patchCord350;
extern AudioConnection          patchCord351;
extern AudioConnection          patchCord352;
extern AudioConnection          patchCord353;
extern AudioConnection          patchCord354;
extern AudioConnection          patchCord355;
extern AudioConnection          patchCord356;

extern AudioControlSGTL5000     sgtl5000_1;     //xy=4701.000038146973,2797.000062942505
// GUItool: end automatically generated code


#endif /* XRAudioConfig_h */