#ifndef XRAudioConfig_h
#define XRAudioConfig_h

#include <Arduino.h>
#include <Audio.h>
#include <TeensyVariablePlayback.h>
#include <synth_dexed.h>
#include <synth_fm_drum.h>
//#include <synth_braids.h>
 
// GUItool: begin automatically generated code
extern AudioSynthDexed          dexed7; //xy=94.16667175292969,1391.6666383743286
extern AudioSynthDexed          dexed5; //xy=98.16667175292969,1133.6666383743286
extern AudioSynthDexed          dexed6; //xy=98.16667175292969,1264.6666383743286
extern AudioSynthDexed          dexed8; //xy=97.16667175292969,1521.6666383743286
extern AudioAmplifier           dexedAmp7; //xy=237.1666717529297,1433.6666383743286
extern AudioAmplifier           dexedAmp6; //xy=240.1666717529297,1306.6666383743286
extern AudioAmplifier           dexedAmp5; //xy=241.1666717529297,1175.6666383743286
extern AudioAmplifier           dexedAmp8; //xy=240.1666717529297,1563.6666383743286
extern AudioSynthWaveformDc     monoSynthDc2;   //xy=245,425
extern AudioSynthWaveformDc     monoSynthDc1;   //xy=247,229
extern AudioSynthWaveformDc     monoSynthDc3;   //xy=247,623
extern AudioSynthWaveformDc     monoSynthDc4;   //xy=247,820
extern AudioSynthWaveform       monoSynthOscA4; //xy=249,703
extern AudioSynthWaveform       monoSynthOscA2; //xy=251,305
extern AudioSynthWaveform       monoSynthOscB2; //xy=251,346
extern AudioSynthNoiseWhite     monoSynthNoise2; //xy=253,386
extern AudioSynthWaveform       monoSynthOscB4; //xy=252,741
extern AudioSynthWaveform       monoSynthOscA1; //xy=255,110
extern AudioSynthWaveform       monoSynthOscB1; //xy=255,150
extern AudioSynthNoiseWhite     monoSynthNoise1; //xy=255,190
extern AudioSynthWaveform       monoSynthOscA3; //xy=254,506
extern AudioSynthWaveform       monoSynthOscB3; //xy=254,545
extern AudioSynthNoiseWhite     monoSynthNoise3; //xy=255,584
extern AudioSynthNoiseWhite     monoSynthNoise4; //xy=255,780
extern AudioAmplifier           dexedAmpAccent7; //xy=258.1666717529297,1390.6666383743286
extern AudioAmplifier           dexedAmpAccent6; //xy=262.1666717529297,1263.6666383743286
extern AudioAmplifier           dexedAmpAccent5; //xy=263.1666717529297,1132.6666383743286
extern AudioAmplifier           dexedAmpAccent8; //xy=262.1666717529297,1520.6666383743286
extern AudioAmplifier           dexedLeft7; //xy=486.1666717529297,1417.6666383743286
extern AudioAmplifier           dexedLeft6;  //xy=490.1666717529297,1289.6666383743286
extern AudioAmplifier           dexedLeft5; //xy=491.1666717529297,1159.6666383743286
extern AudioAmplifier           dexedLeft8; //xy=490.1666717529297,1546.6666383743286
extern AudioAmplifier           dexedRight7; //xy=492.1666717529297,1454.6666383743286
extern AudioAmplifier           dexedRight6; //xy=495.1666717529297,1327.6666383743286
extern AudioAmplifier           dexedRight5; //xy=496.1666717529297,1196.6666383743286
extern AudioAmplifier           dexedRight8; //xy=495.1666717529297,1584.6666383743286
extern AudioMixer4              monoSynthMix2;  //xy=517,324
extern AudioMixer4              monoSynthMix1;  //xy=519,128
extern AudioMixer4              monoSynthMix3;  //xy=519,522
extern AudioMixer4              monoSynthMix4;  //xy=519,719
extern AudioEffectEnvelope      monoSynthFilterEnv2; //xy=535,379
extern AudioEffectEnvelope      monoSynthFilterEnv1; //xy=537,183
extern AudioEffectEnvelope      monoSynthFilterEnv3; //xy=537,577
extern AudioEffectEnvelope      monoSynthFilterEnv4; //xy=537,774
extern AudioAmplifier           monoSynthFilterAccent2; //xy=544,418
extern AudioAmplifier           monoSynthFilterAccent1; //xy=547,222
extern AudioAmplifier           monoSynthFilterAccent3; //xy=546,616
extern AudioAmplifier           monoSynthFilterAccent4; //xy=546,813
extern AudioMixer4              delaySubMix8; //xy=722.0000076293945,3122.6663856506348
extern AudioAmplifier           dexedAmpDelaySend5; //xy=732.5000152587891,1158.3333292007446
extern AudioAmplifier           dexedAmpDelaySend6; //xy=732.5000152587891,1197.3333292007446
extern AudioAmplifier           dexedAmpDelaySend8; //xy=732.5000152587891,1277.3333292007446
extern AudioAmplifier           dexedAmpDelaySend7; //xy=733.5000152587891,1237.3333292007446
extern AudioMixer4              voiceSubMixLeft8; //xy=736.5000152587891,1397.3333292007446
extern AudioMixer4              voiceSubMixRight8; //xy=740.5000152587891,1465.3333292007446
extern AudioFilterLadder        monoSynthFilter2; //xy=797,376
extern AudioFilterLadder        monoSynthFilter1; //xy=799,180
extern AudioFilterLadder        monoSynthFilter3; //xy=799,575
extern AudioFilterLadder        monoSynthFilter4; //xy=799,771
extern AudioPlayArrayResmp      monoSample3; //xy=900.5555572509766,2402.9165391921997
extern AudioPlayArrayResmp      monoSample4; //xy=900.4166412353516,2561.5276260375977
extern AudioPlayArrayResmp      monoSample1; //xy=903.3333892822266,2072.500030517578
extern AudioPlayArrayResmp      monoSample2; //xy=906.1110992431641,2234.583285331726
extern AudioPlayArrayResmp      monoSample11; //xy=911.9722442626953,3777.6665601730347
extern AudioPlayArrayResmp      monoSample12; //xy=914.3333282470703,3940.0276470184326
extern AudioPlayArrayResmp      monoSample15; //xy=914.2221832275391,4496.416570663452
extern AudioPlayArrayResmp      monoSample16;  //xy=914.0832672119141,4658.777900695801
extern AudioPlayArrayResmp      monoSample10; //xy=918.7777481079102,3615.5833263397217
extern AudioPlayArrayResmp      monoSample7; //xy=921.9721832275391,3092.166549682617
extern AudioPlayArrayResmp      monoSample13; //xy=918.2500152587891,4169.750061988831
extern AudioPlayArrayResmp      monoSample14; //xy=918.5277252197266,4334.3335609436035
extern AudioPlayArrayResmp      monoSample5; //xy=925.9999923706055,2764.2500228881836
extern AudioPlayArrayResmp      monoSample8; //xy=924.3332672119141,3250.777636528015
extern AudioPlayArrayResmp      monoSample9; //xy=926.0000152587891,3452.250051498413
extern AudioPlayArrayResmp      monoSample6; //xy=928.7777252197266,2927.583297729492
extern AudioSynthDexed          dexed3;         //xy=963,1174
extern AudioSynthDexed          dexed1;         //xy=967,916
extern AudioSynthDexed          dexed2;         //xy=967,1047
extern AudioSynthDexed          dexed4;         //xy=966,1304
extern AudioSynthFMDrum         fmDrum3;        //xy=975.9999771118164,1732.666657447815
extern AudioSynthFMDrum         fmDrum2;        //xy=980.9999771118164,1588.666657447815
extern AudioSynthFMDrum         fmDrum1;        //xy=985.9999771118164,1443.666657447815
extern AudioSynthFMDrum         fmDrum4; //xy=986.6666259765625,1869.9998779296875
extern AudioAmplifier           monoSynthAmp2;  //xy=1046,401
extern AudioAmplifier           monoSynthAmp1;  //xy=1048,206
extern AudioAmplifier           monoSynthAmp3;  //xy=1048,600
extern AudioAmplifier           monoSynthAmp4;  //xy=1048,796
extern AudioEffectEnvelope      monoSynthAmpEnv2; //xy=1059,325
extern AudioEffectEnvelope      monoSynthAmpEnv1; //xy=1061,129
extern AudioEffectEnvelope      monoSynthAmpEnv3; //xy=1061,523
extern AudioEffectEnvelope      monoSynthAmpEnv4; //xy=1061,720
extern AudioAmplifier           monoSynthAmpAccent2; //xy=1068,363
extern AudioAmplifier           monoSynthAmpAccent1; //xy=1070,167
extern AudioAmplifier           monoSynthAmpAccent3; //xy=1070,561
extern AudioAmplifier           monoSynthAmpAccent4; //xy=1070,758
extern AudioAmplifier           dexedAmp3;      //xy=1106,1216
extern AudioAmplifier           dexedAmp2;      //xy=1109,1089
extern AudioAmplifier           dexedAmp1;      //xy=1110,958
extern AudioAmplifier           dexedAmp4;      //xy=1109,1346
extern AudioEffectEnvelope      monoSampleAmpEnv4; //xy=1115,2559
extern AudioEffectEnvelope      monoSampleAmpEnv3; //xy=1116,2399
extern AudioEffectEnvelope      monoSampleAmpEnv2; //xy=1118,2236
extern AudioEffectEnvelope      monoSampleAmpEnv1; //xy=1119,2071
extern AudioAmplifier           dexedAmpAccent3; //xy=1127,1173
extern AudioAmplifier           dexedAmpAccent2; //xy=1131,1046
extern AudioAmplifier           monoSampleAmpAccent4; //xy=1125,2605
extern AudioAmplifier           dexedAmpAccent1; //xy=1132,915
extern AudioAmplifier           dexedAmpAccent4; //xy=1131,1303
extern AudioAmplifier           monoSampleAmpAccent3; //xy=1127,2445
extern AudioAmplifier           monoSampleAmpAccent2; //xy=1129,2282
extern AudioAmplifier           monoSampleAmpAccent1; //xy=1130,2117
extern AudioAmplifier           monoSampleAmp4; //xy=1130,2652
extern AudioAmplifier           monoSampleAmp3; //xy=1132,2492
extern AudioAmplifier           monoSampleAmp2; //xy=1134,2329
extern AudioAmplifier           monoSampleAmp1; //xy=1135,2164
extern AudioEffectEnvelope      monoSampleAmpEnv16; //xy=1130,4658
extern AudioEffectEnvelope      monoSampleAmpEnv15; //xy=1132,4498
extern AudioEffectEnvelope      monoSampleAmpEnv14; //xy=1133,4335
extern AudioEffectEnvelope      monoSampleAmpEnv8; //xy=1138,3251
extern AudioEffectEnvelope      monoSampleAmpEnv13; //xy=1135,4170
extern AudioEffectEnvelope      monoSampleAmpEnv7; //xy=1140,3091
extern AudioEffectEnvelope      monoSampleAmpEnv12; //xy=1137,3940
extern AudioEffectEnvelope      monoSampleAmpEnv11; //xy=1138,3780
extern AudioEffectEnvelope      monoSampleAmpEnv6; //xy=1142,2928
extern AudioEffectEnvelope      monoSampleAmpEnv5; //xy=1143,2763
extern AudioEffectEnvelope      monoSampleAmpEnv10; //xy=1140,3616
extern AudioEffectEnvelope      monoSampleAmpEnv9; //xy=1141,3451
extern AudioAmplifier           monoSampleAmpAccent16; //xy=1141,4704
extern AudioAmplifier           monoSampleAmpAccent15; //xy=1142,4544
extern AudioAmplifier           monoSampleAmpAccent14; //xy=1144,4381
extern AudioAmplifier           monoSampleAmpAccent13; //xy=1145,4216
extern AudioAmplifier           fmDrumAmpAccent3; //xy=1154.9999771118164,1732.666657447815
extern AudioAmplifier           monoSampleAmpAccent8; //xy=1149,3298
extern AudioAmplifier           monoSampleAmpAccent12; //xy=1147,3986
extern AudioAmplifier           monoSampleAmpAccent7; //xy=1151,3138
extern AudioAmplifier           monoSampleAmpAccent6; //xy=1152,2974
extern AudioAmplifier           fmDrumAmp3;     //xy=1156.9999771118164,1780.666657447815
extern AudioAmplifier           monoSampleAmpAccent11; //xy=1149,3826
extern AudioAmplifier           monoSampleAmp16; //xy=1146,4751
extern AudioAmplifier           monoSampleAmpAccent5; //xy=1154,2810
extern AudioAmplifier           monoSampleAmp15; //xy=1147,4591
extern AudioAmplifier           monoSampleAmpAccent10; //xy=1151,3663
extern AudioAmplifier           monoSampleAmpAccent9; //xy=1152,3498
extern AudioAmplifier           fmDrumAmpAccent2; //xy=1159.9999771118164,1589.666657447815
extern AudioAmplifier           monoSampleAmp14; //xy=1149,4428
extern AudioAmplifier           monoSampleAmp13; //xy=1150,4263
extern AudioAmplifier           monoSampleAmp8; //xy=1154,3344
extern AudioAmplifier           monoSampleAmp12; //xy=1152,4033
extern AudioAmplifier           fmDrumAmp2;     //xy=1161.9999771118164,1637.666657447815
extern AudioAmplifier           monoSampleAmp7; //xy=1156,3184
extern AudioAmplifier           monoSampleAmp6; //xy=1157,3021
extern AudioAmplifier           monoSampleAmp11; //xy=1154,3873
extern AudioAmplifier           monoSampleAmp5; //xy=1159,2856
extern AudioAmplifier           fmDrumAmpAccent1; //xy=1164.9999771118164,1443.666657447815
extern AudioAmplifier           monoSampleAmp10; //xy=1156,3709
extern AudioAmplifier           monoSampleAmp9; //xy=1157,3545
extern AudioAmplifier           fmDrumAmp1;     //xy=1166.9999771118164,1492.666657447815
extern AudioAmplifier           fmDrumAmpAccent4; //xy=1165.6666259765625,1869.9998779296875
extern AudioAmplifier           fmDrumAmp4; //xy=1167.6666259765625,1917.9998779296875
extern AudioAmplifier           fmDrumLeft3;    //xy=1351.9999771118164,1762.666657447815
extern AudioAmplifier           dexedLeft3;     //xy=1355,1200
extern AudioAmplifier           dexedLeft2;     //xy=1359,1072
extern AudioAmplifier           fmDrumLeft2;    //xy=1356.9999771118164,1619.666657447815
extern AudioAmplifier           dexedLeft1;     //xy=1360,942
extern AudioAmplifier           fmDrumRight3;   //xy=1356.9999771118164,1801.666657447815
extern AudioAmplifier           dexedLeft4;     //xy=1359,1329
extern AudioAmplifier           monoSynthLeft2; //xy=1364,361
extern AudioAmplifier           dexedRight3;    //xy=1361,1237
extern AudioAmplifier           monoSynthLeft1; //xy=1366,165
extern AudioAmplifier           fmDrumLeft1;    //xy=1361.9999771118164,1473.666657447815
extern AudioAmplifier           monoSynthLeft3; //xy=1366,559
extern AudioAmplifier           dexedRight2;    //xy=1364,1110
extern AudioAmplifier           monoSampleLeft4; //xy=1358,2629
extern AudioAmplifier           fmDrumRight2;   //xy=1361.9999771118164,1657.666657447815
extern AudioAmplifier           dexedRight1;    //xy=1365,979
extern AudioAmplifier           monoSynthLeft4; //xy=1366,756
extern AudioAmplifier           dexedRight4;    //xy=1364,1367
extern AudioAmplifier           monoSynthRight2; //xy=1368,398
extern AudioAmplifier           monoSampleLeft3; //xy=1360,2469
extern AudioAmplifier           monoSampleLeft2; //xy=1361,2306
extern AudioAmplifier           fmDrumLeft4; //xy=1362.6666259765625,1899.9998779296875
extern AudioAmplifier           monoSynthRight1; //xy=1370,202
extern AudioAmplifier           monoSampleLeft1; //xy=1363,2141
extern AudioAmplifier           monoSynthRight3; //xy=1370,596
extern AudioAmplifier           monoSampleRight4; //xy=1362,2668
extern AudioAmplifier           monoSampleRight3; //xy=1363,2508
extern AudioAmplifier           fmDrumRight1;   //xy=1366.9999771118164,1512.666657447815
extern AudioAmplifier           monoSynthRight4; //xy=1370,793
extern AudioAmplifier           monoSampleRight2; //xy=1365,2345
extern AudioAmplifier           monoSampleRight1; //xy=1366,2180
extern AudioAmplifier           fmDrumRight4; //xy=1367.6666259765625,1938.9998779296875
extern AudioAmplifier           monoSampleLeft16; //xy=1374,4728
extern AudioAmplifier           monoSampleLeft15; //xy=1375,4568
extern AudioAmplifier           monoSampleLeft14; //xy=1377,4405
extern AudioAmplifier           monoSampleLeft13; //xy=1378,4240
extern AudioAmplifier           monoSampleLeft8; //xy=1382,3322
extern AudioAmplifier           monoSampleLeft12; //xy=1380,4010
extern AudioAmplifier           monoSampleRight16; //xy=1377,4767
extern AudioAmplifier           monoSampleLeft7; //xy=1384,3162
extern AudioAmplifier           monoSampleLeft6; //xy=1385,2998
extern AudioAmplifier           monoSampleLeft5; //xy=1386,2833
extern AudioAmplifier           monoSampleLeft11; //xy=1382,3850
extern AudioAmplifier           monoSampleRight15; //xy=1379,4607
extern AudioAmplifier           monoSampleLeft10; //xy=1384,3687
extern AudioAmplifier           monoSampleRight14; //xy=1381,4444
extern AudioAmplifier           monoSampleLeft9; //xy=1385,3522
extern AudioAmplifier           monoSampleRight13; //xy=1382,4279
extern AudioAmplifier           monoSampleRight8; //xy=1386,3361
extern AudioAmplifier           monoSampleRight7; //xy=1387,3201
extern AudioAmplifier           monoSampleRight12; //xy=1384,4049
extern AudioAmplifier           monoSampleRight6; //xy=1389,3037
extern AudioAmplifier           monoSampleRight5; //xy=1390,2872
extern AudioAmplifier           monoSampleRight11; //xy=1386,3889
extern AudioAmplifier           monoSampleRight10; //xy=1387,3726
extern AudioAmplifier           monoSampleRight9; //xy=1388,3561
extern AudioMixer4              voiceSubMixLeft3; //xy=1659,1704
extern AudioAmplifier           fmDrumAmpDelaySend1; //xy=1661,1469
extern AudioAmplifier           fmDrumAmpDelaySend2; //xy=1661,1511
extern AudioAmplifier           fmDrumAmpDelaySend4; //xy=1660.6666107177734,1595.999942779541
extern AudioAmplifier           fmDrumAmpDelaySend3; //xy=1661,1553
extern AudioMixer4              voiceSubMixRight3; //xy=1663,1772
extern AudioMixer4              voiceSubMixLeft4; //xy=1669,2357
extern AudioAmplifier           dexedAmpDelaySend1; //xy=1678,914
extern AudioAmplifier           dexedAmpDelaySend2; //xy=1678,953
extern AudioAmplifier           dexedAmpDelaySend4; //xy=1678,1033
extern AudioMixer4              voiceSubMixRight4; //xy=1673,2425
extern AudioAmplifier           dexedAmpDelaySend3; //xy=1679,993
extern AudioMixer4              voiceSubMixLeft7; //xy=1666,4449
extern AudioMixer4              voiceSubMixLeft5; //xy=1673,3061
extern AudioMixer4              voiceSubMixLeft2; //xy=1682,1153
extern AudioMixer4              voiceSubMixRight7; //xy=1670,4517
extern AudioMixer4              voiceSubMixRight5; //xy=1677,3129
extern AudioAmplifier           monoSampleAmpDelaySend5; //xy=1679,2759
extern AudioMixer4              voiceSubMixRight2; //xy=1686,1221
extern AudioAmplifier           monoSampleAmpDelaySend6; //xy=1681,2799
extern AudioAmplifier           monoSampleAmpDelaySend1; //xy=1684,2068
extern AudioAmplifier           monoSampleAmpDelaySend7; //xy=1681,2839
extern AudioAmplifier           monoSampleAmpDelaySend2; //xy=1685,2108
extern AudioAmplifier           monoSampleAmpDelaySend8; //xy=1682,2879
extern AudioAmplifier           monoSampleAmpDelaySend3; //xy=1685,2148
extern AudioAmplifier           monoSampleAmpDelaySend13; //xy=1679,4153
extern AudioAmplifier           monoSampleAmpDelaySend4; //xy=1687,2188
extern AudioAmplifier           monoSampleAmpDelaySend14; //xy=1679,4193
extern AudioAmplifier           monoSampleAmpDelaySend15; //xy=1679,4233
extern AudioMixer4              voiceSubMixLeft6; //xy=1681,3741
extern AudioAmplifier           monoSynthAmpDelaySend1; //xy=1696,146
extern AudioAmplifier           monoSynthAmpDelaySend2; //xy=1696,185
extern AudioAmplifier           monoSampleAmpDelaySend16; //xy=1681,4273
extern AudioAmplifier           monoSynthAmpDelaySend3; //xy=1698,225
extern AudioAmplifier           monoSynthAmpDelaySend4; //xy=1698,266
extern AudioMixer4              voiceSubMixRight6; //xy=1685,3809
extern AudioMixer4              voiceSubMixLeft1; //xy=1700,463
extern AudioMixer4              voiceSubMixRight1; //xy=1704,531
extern AudioAmplifier           monoSampleAmpDelaySend9; //xy=1699,3456
extern AudioAmplifier           monoSampleAmpDelaySend10; //xy=1704,3495
extern AudioAmplifier           monoSampleAmpDelaySend11; //xy=1704,3535
extern AudioAmplifier           monoSampleAmpDelaySend12; //xy=1705,3575
extern AudioMixer4              voiceMixLeft1;  //xy=2114,1330
extern AudioMixer4              voiceMixRight1; //xy=2118,1397
extern AudioMixer4              voiceMixLeft2;  //xy=2126,3289
extern AudioMixer4              voiceMixRight2; //xy=2133,3356
extern AudioMixer4              delaySubMix1;   //xy=2476,157
extern AudioMixer4              delaySubMix2;   //xy=2478,925
extern AudioMixer4              delaySubMix3;   //xy=2515,1589
extern AudioMixer4              delaySubMix7;   //xy=2513,4170
extern AudioMixer4              delaySubMix6;   //xy=2518,3472
extern AudioMixer4              delaySubMix5;   //xy=2538,2774
extern AudioMixer4              delaySubMix4;   //xy=2541,2080
extern AudioMixer4              delayMix1;      //xy=2953,1240
extern AudioMixer4              delayMix2;      //xy=2978,3081
extern AudioInputI2S            i2s2;           //xy=3346,2839
extern AudioAnalyzePeak         peak_left;      //xy=3352,2791
extern AudioAnalyzePeak         peak_right;     //xy=3356,2891
extern AudioEffectDelay         delay1;         //xy=3359,2159
extern AudioMixer4              mainMixerLeft;  //xy=3381,2414
extern AudioMixer4              mainMixerRight; //xy=3384,2486
extern AudioAmplifier           delayLeftAmp1;  //xy=3388,2251
extern AudioAmplifier           delayRightAmp1; //xy=3389,2291
extern AudioMixer4              delayFeedbackMixer1; //xy=3402,2051
extern AudioMixer4              inputMixerLeft; //xy=3552,2809
extern AudioMixer4              inputMixerRight; //xy=3554,2884
extern AudioMixer4              OutputMixerLeft; //xy=3812,2758
extern AudioMixer4              OutputMixerRight; //xy=3817,2829
extern AudioOutputI2S           i2s1;           //xy=4008,2800
 
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
 
extern AudioControlSGTL5000     sgtl5000_1;     //xy=4015,2752
// GUItool: end automatically generated code

#endif /* XRAudioConfig_h */