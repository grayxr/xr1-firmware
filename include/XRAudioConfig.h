#ifndef XRAudioConfig_h
#define XRAudioConfig_h

#include <Arduino.h>
#include <Audio.h>
#include <TeensyVariablePlayback.h>
#include <synth_dexed.h>

// GUItool: begin automatically generated code
extern AudioSynthNoiseWhite vnoise3;    // xy=390,865
extern AudioSynthNoiseWhite vnoise2;    // xy=392,605
extern AudioSynthNoiseWhite vnoise4;    // xy=390,1114
extern AudioSynthNoiseWhite vnoise1;    // xy=394,356
extern AudioSynthWaveformDc vdc2;       // xy=393,650
extern AudioSynthWaveformDc vdc3;       // xy=392,910
extern AudioSynthWaveform voscb2;       // xy=394,557
extern AudioSynthWaveform voscb3;       // xy=393,817
extern AudioSynthWaveformDc vdc4;       // xy=392,1160
extern AudioSynthWaveform vosca2;       // xy=395,512
extern AudioSynthWaveform vosca3;       // xy=394,772
extern AudioSynthWaveform voscb4;       // xy=393,1067
extern AudioSynthWaveformDc vdc1;       // xy=396,401
extern AudioSynthWaveform vosca4;       // xy=394,1022
extern AudioSynthWaveform voscb1;       // xy=397,308
extern AudioSynthWaveform vosca1;       // xy=398,263
extern AudioEffectEnvelope vfilterenv2; // xy=556,588
extern AudioMixer4 voscmix2;            // xy=557,532
extern AudioMixer4 voscmix3;            // xy=556,791
extern AudioMixer4 voscmix4;            // xy=556,1041
extern AudioEffectEnvelope vfilterenv3; // xy=556.857105255127,848.9999847412109
extern AudioEffectEnvelope vfilterenv1; // xy=559,342
extern AudioMixer4 voscmix1;            // xy=560,283
extern AudioEffectEnvelope vfilterenv4; // xy=559,1097
extern AudioPlayArrayResmp vmsample1;   // xy=726.0000305175781,243.6666603088379
extern AudioPlayArrayResmp vmsample2;   // xy=730.0000305175781,492.00000953674316
extern AudioPlayArrayResmp vmsample3;   // xy=730.0000305175781,749.0000085830688
extern AudioPlayArrayResmp vmsample4;   // xy=730.0000076293945,1000.6666650772095
extern AudioFilterLadder vlfilter1;     // xy=736.0000915527344,296.3333549499512
extern AudioFilterLadder vlfilter2;     // xy=739.999927520752,544.1667289733887
extern AudioFilterLadder vlfilter3;     // xy=741.0000305175781,803.6666793823242
extern AudioFilterLadder vlfilter4;     // xy=740.9999885559082,1053.6666288375854
extern AudioPlayArrayResmp vmsample8;   // xy=864.6666145324707,1867.6665539741516
extern AudioPlayArrayResmp vmsample5;   // xy=871.166675567627,1300.999927997589
extern AudioPlayArrayResmp vmsample7;   // xy=871.3333015441895,1667.6665539741516
extern AudioPlayArrayResmp vmsample6;   // xy=874.666675567627,1490.999927997589
extern AudioMixer4 vmix2;               // xy=887,531
extern AudioMixer4 vmix3;               // xy=886,791
extern AudioMixer4 vmix4;               // xy=886,1041
extern AudioMixer4 vmix1;               // xy=890,282
extern AudioPlayArrayResmp vmsample16;  // xy=880.6666145324707,3962.6665539741516
extern AudioPlayArrayResmp vmsample13;  // xy=887.166675567627,3395.999927997589
extern AudioPlayArrayResmp vmsample15;  // xy=887.3333015441895,3762.6665539741516
extern AudioPlayArrayResmp vmsample14;  // xy=890.666675567627,3585.999927997589
extern AudioPlayArrayResmp vmsample12;  // xy=908.1666145324707,2927.6665539741516
extern AudioPlayArrayResmp vmsample9;   // xy=914.666675567627,2360.999927997589
extern AudioPlayArrayResmp vmsample11;  // xy=914.8333015441895,2727.6665539741516
extern AudioPlayArrayResmp vmsample10;  // xy=918.166675567627,2550.999927997589
extern AudioEffectEnvelope venv3;       // xy=1023,790
extern AudioEffectEnvelope venv2;       // xy=1025,530
extern AudioEffectEnvelope venv4;       // xy=1023,1040
extern AudioEffectEnvelope venv8;       // xy=1019.9999389648438,1866.6666259765625
extern AudioEffectEnvelope venv1;       // xy=1028,281
extern AudioSynthDexed dexed1;          // xy=1029.750015258789,391.5000057220459
extern AudioEffectEnvelope venv5;       // xy=1026.5,1300
extern AudioSynthDexed dexed3;          // xy=1029.500015258789,904.2500133514404
extern AudioEffectEnvelope venv7;       // xy=1026.6666259765625,1666.6666259765625
extern AudioEffectEnvelope venv6;       // xy=1030,1490
extern AudioSynthDexed dexed2;          // xy=1034.2500343322754,644.7500195503235
extern AudioSynthDexed dexed4;          // xy=1032.7500343322754,1152.5000343322754
extern AudioEffectEnvelope venv16;      // xy=1035.9999389648438,3961.6666259765625
extern AudioEffectEnvelope venv13;      // xy=1042.5,3395
extern AudioEffectEnvelope venv15;      // xy=1042.6666259765625,3761.6666259765625
extern AudioEffectEnvelope venv14;      // xy=1046,3585
extern AudioEffectEnvelope venv12;      // xy=1063.4999389648438,2926.6666259765625
extern AudioEffectEnvelope venv9;       // xy=1070,2360
extern AudioEffectEnvelope venv11;      // xy=1070.1666259765625,2726.6666259765625
extern AudioEffectEnvelope venv10;      // xy=1073.5,2550
extern AudioAmplifier dright3;          // xy=1176.2500190734863,920.0000133514404
extern AudioAmplifier vleft3;           // xy=1177,772
extern AudioAmplifier vleft8;           // xy=1172.9999389648438,1848.6666259765625
extern AudioAmplifier dleft3;           // xy=1177.5000190734863,878.7500128746033
extern AudioAmplifier vleft2;           // xy=1179,512
extern AudioAmplifier vleft4;           // xy=1177,1022
extern AudioAmplifier vright3;          // xy=1178,811
extern AudioAmplifier dright2;          // xy=1178.7500190734863,657.5000104904175
extern AudioAmplifier vleft1;           // xy=1181,263
extern AudioAmplifier dright4;          // xy=1177.5000190734863,1167.5000176429749
extern AudioAmplifier vright2;          // xy=1180,551
extern AudioAmplifier vright4;          // xy=1178,1060
extern AudioAmplifier dleft2;           // xy=1180.0000190734863,616.2500100135803
extern AudioAmplifier vright8;          // xy=1174.9999389648438,1887.6666259765625
extern AudioAmplifier dleft4;           // xy=1178.7500190734863,1126.2500171661377
extern AudioAmplifier vright1;          // xy=1183,302
extern AudioAmplifier vleft5;           // xy=1179.5,1282
extern AudioAmplifier vleft7;           // xy=1179.6666259765625,1648.6666259765625
extern AudioAmplifier dright1;          // xy=1185.0000190734863,406.2500057220459
extern AudioAmplifier vright5;          // xy=1181.5,1321
extern AudioAmplifier dleft1;           // xy=1186.2500190734863,365.00000524520874
extern AudioAmplifier vright7;          // xy=1181.6666259765625,1687.6666259765625
extern AudioAmplifier vleft6;           // xy=1183,1472
extern AudioAmplifier vright6;          // xy=1185,1511
extern AudioAmplifier vleft16;          // xy=1188.9999389648438,3943.6666259765625
extern AudioAmplifier vright16;         // xy=1190.9999389648438,3982.6666259765625
extern AudioAmplifier vleft13;          // xy=1195.5,3377
extern AudioAmplifier vleft15;          // xy=1195.6666259765625,3743.6666259765625
extern AudioAmplifier vright13;         // xy=1197.5,3416
extern AudioAmplifier vright15;         // xy=1197.6666259765625,3782.6666259765625
extern AudioAmplifier vleft14;          // xy=1199,3567
extern AudioAmplifier vright14;         // xy=1201,3606
extern AudioAmplifier vleft12;          // xy=1216.4999389648438,2908.6666259765625
extern AudioAmplifier vright12;         // xy=1218.4999389648438,2947.6666259765625
extern AudioAmplifier vleft9;           // xy=1223,2342
extern AudioAmplifier vleft11;          // xy=1223.1666259765625,2708.6666259765625
extern AudioAmplifier vright9;          // xy=1225,2381
extern AudioAmplifier vright11;         // xy=1225.1666259765625,2747.6666259765625
extern AudioAmplifier vleft10;          // xy=1226.5,2532
extern AudioAmplifier vright10;         // xy=1228.5,2571
extern AudioMixer4 vsubmixr4;           // xy=1379.6667213439941,1071.9999599456787
extern AudioMixer4 vsubmixl4;           // xy=1381.333351135254,1001.6666660308838
extern AudioMixer4 vsubmixl3;           // xy=1383.0001373291016,764.3333435058594
extern AudioMixer4 vsubmixr3;           // xy=1384.666763305664,833.666618347168
extern AudioMixer4 vsubmixl8;           // xy=1383.5714073181152,1851.238042831421
extern AudioMixer4 vsubmixr8;           // xy=1384.2380332946777,1923.238039970398
extern AudioMixer4 vsubmixl1;           // xy=1391.5714233517647,265.5714111328125
extern AudioMixer4 vsubmixr1;           // xy=1392.238094329834,337.57141399383545
extern AudioMixer4 vsubmixl5;           // xy=1390.0714683532715,1284.5714168548584
extern AudioMixer4 vsubmixr5;           // xy=1390.738094329834,1356.5714139938354
extern AudioMixer4 vsubmixl7;           // xy=1390.238094329834,1651.238042831421
extern AudioMixer4 vsubmixl2;           // xy=1395.2379722595215,510.2857246398926
extern AudioMixer4 vsubmixr2;           // xy=1395.2381744384766,581.047568321228
extern AudioMixer4 vsubmixr7;           // xy=1390.9047203063965,1723.238039970398
extern AudioMixer4 vsubmixl6;           // xy=1393.5714683532715,1474.5714168548584
extern AudioMixer4 vsubmixr6;           // xy=1394.238094329834,1546.5714139938354
extern AudioMixer4 vsubmixl16;          // xy=1399.5714073181152,3946.238042831421
extern AudioMixer4 vsubmixr16;          // xy=1400.2380332946777,4018.238039970398
extern AudioMixer4 vsubmixl13;          // xy=1406.0714683532715,3379.5714168548584
extern AudioMixer4 vsubmixr13;          // xy=1406.738094329834,3451.5714139938354
extern AudioMixer4 vsubmixl15;          // xy=1406.238094329834,3746.238042831421
extern AudioMixer4 vsubmixr15;          // xy=1406.9047203063965,3818.238039970398
extern AudioMixer4 vsubmixl14;          // xy=1409.5714683532715,3569.5714168548584
extern AudioMixer4 vsubmixr14;          // xy=1410.238094329834,3641.5714139938354
extern AudioMixer4 vsubmixl12;          // xy=1427.0714073181152,2911.238042831421
extern AudioMixer4 vsubmixr12;          // xy=1427.7380332946777,2983.238039970398
extern AudioMixer4 vsubmixr9;           // xy=1434.238094329834,2416.5714139938354
extern AudioMixer4 vsubmixl11;          // xy=1433.738094329834,2711.238042831421
extern AudioMixer4 vsubmixr11;          // xy=1434.4047203063965,2783.238039970398
extern AudioMixer4 vsubmixl10;          // xy=1437.0714683532715,2534.5714168548584
extern AudioMixer4 vsubmixl9;           // xy=1437.857063293457,2341.7143173217773
extern AudioMixer4 vsubmixr10;          // xy=1437.738094329834,2606.5714139938354
extern AudioMixer4 mixerLeft2;          // xy=1859.1666984558105,1580.8333568572998
extern AudioMixer4 mixerRight4;         // xy=1851.642807006836,3732.6428833007812
extern AudioMixer4 mixerLeft4;          // xy=1852.5,3652.5
extern AudioMixer4 mixerRight2;         // xy=1861.642734527588,1657.6428089141846
extern AudioMixer4 mixerLeft1;          // xy=1895.0000534057617,614.9999885559082
extern AudioMixer4 mixerRight3;         // xy=1889.1428604125977,2710.1428718566895
extern AudioMixer4 mixerLeft3;          // xy=1890.0000534057617,2629.999988555908
extern AudioMixer4 mixerRight1;         // xy=1899.9761505126953,695.1428909301758
extern AudioMixer4 mainMixerLeft;       // xy=2377.5001792907715,2080.0001640319824
extern AudioMixer4 mainMixerRight;      // xy=2380.8331031799316,2151.6668338775635
extern AudioInputI2S i2s2;              // xy=2400.000419616699,2333.333345413208
extern AudioAnalyzePeak peak_left;      // xy=2406.666679382324,2285.000177383423
extern AudioAnalyzePeak peak_right;     // xy=2410.000015258789,2384.999855041504
extern AudioMixer4 inputMixerLeft;      // xy=2606.6663665771484,2303.3330993652344
extern AudioMixer4 inputMixerRight;     // xy=2608.333351135254,2378.3333463668823
extern AudioMixer4 OutputMixerLeft;     // xy=2866.6664505004883,2251.6665935516357
extern AudioMixer4 OutputMixerRight;    // xy=2871.6666107177734,2323.3333435058594
extern AudioOutputI2S i2s1;             // xy=3062.880729675293,2294.166513442993

extern AudioConnection patchCord1;
extern AudioConnection patchCord2;
extern AudioConnection patchCord3;
extern AudioConnection patchCord4;
extern AudioConnection patchCord5;
extern AudioConnection patchCord6;
extern AudioConnection patchCord7;
extern AudioConnection patchCord8;
extern AudioConnection patchCord9;
extern AudioConnection patchCord10;
extern AudioConnection patchCord11;
extern AudioConnection patchCord12;
extern AudioConnection patchCord13;
extern AudioConnection patchCord14;
extern AudioConnection patchCord15;
extern AudioConnection patchCord16;
extern AudioConnection patchCord17;
extern AudioConnection patchCord18;
extern AudioConnection patchCord19;
extern AudioConnection patchCord20;
extern AudioConnection patchCord21;
extern AudioConnection patchCord22;
extern AudioConnection patchCord23;
extern AudioConnection patchCord24;
extern AudioConnection patchCord25;
extern AudioConnection patchCord26;
extern AudioConnection patchCord27;
extern AudioConnection patchCord28;
extern AudioConnection patchCord29;
extern AudioConnection patchCord30;
extern AudioConnection patchCord31;
extern AudioConnection patchCord32;
extern AudioConnection patchCord33;
extern AudioConnection patchCord34;
extern AudioConnection patchCord35;
extern AudioConnection patchCord36;
extern AudioConnection patchCord37;
extern AudioConnection patchCord38;
extern AudioConnection patchCord39;
extern AudioConnection patchCord40;
extern AudioConnection patchCord41;
extern AudioConnection patchCord42;
extern AudioConnection patchCord43;
extern AudioConnection patchCord44;
extern AudioConnection patchCord45;
extern AudioConnection patchCord46;
extern AudioConnection patchCord47;
extern AudioConnection patchCord48;
extern AudioConnection patchCord49;
extern AudioConnection patchCord50;
extern AudioConnection patchCord51;
extern AudioConnection patchCord52;
extern AudioConnection patchCord53;
extern AudioConnection patchCord54;
extern AudioConnection patchCord55;
extern AudioConnection patchCord56;
extern AudioConnection patchCord57;
extern AudioConnection patchCord58;
extern AudioConnection patchCord59;
extern AudioConnection patchCord60;
extern AudioConnection patchCord61;
extern AudioConnection patchCord62;
extern AudioConnection patchCord63;
extern AudioConnection patchCord64;
extern AudioConnection patchCord65;
extern AudioConnection patchCord66;
extern AudioConnection patchCord67;
extern AudioConnection patchCord68;
extern AudioConnection patchCord69;
extern AudioConnection patchCord70;
extern AudioConnection patchCord71;
extern AudioConnection patchCord72;
extern AudioConnection patchCord73;
extern AudioConnection patchCord74;
extern AudioConnection patchCord75;
extern AudioConnection patchCord76;
extern AudioConnection patchCord77;
extern AudioConnection patchCord78;
extern AudioConnection patchCord79;
extern AudioConnection patchCord80;
extern AudioConnection patchCord81;
extern AudioConnection patchCord82;
extern AudioConnection patchCord83;
extern AudioConnection patchCord84;
extern AudioConnection patchCord85;
extern AudioConnection patchCord86;
extern AudioConnection patchCord87;
extern AudioConnection patchCord88;
extern AudioConnection patchCord89;
extern AudioConnection patchCord90;
extern AudioConnection patchCord91;
extern AudioConnection patchCord92;
extern AudioConnection patchCord93;
extern AudioConnection patchCord94;
extern AudioConnection patchCord95;
extern AudioConnection patchCord96;
extern AudioConnection patchCord97;
extern AudioConnection patchCord98;
extern AudioConnection patchCord99;
extern AudioConnection patchCord100;
extern AudioConnection patchCord101;
extern AudioConnection patchCord102;
extern AudioConnection patchCord103;
extern AudioConnection patchCord104;
extern AudioConnection patchCord105;
extern AudioConnection patchCord106;
extern AudioConnection patchCord107;
extern AudioConnection patchCord108;
extern AudioConnection patchCord109;
extern AudioConnection patchCord110;
extern AudioConnection patchCord111;
extern AudioConnection patchCord112;
extern AudioConnection patchCord113;
extern AudioConnection patchCord114;
extern AudioConnection patchCord115;
extern AudioConnection patchCord116;
extern AudioConnection patchCord117;
extern AudioConnection patchCord118;
extern AudioConnection patchCord119;
extern AudioConnection patchCord120;
extern AudioConnection patchCord121;
extern AudioConnection patchCord122;
extern AudioConnection patchCord123;
extern AudioConnection patchCord124;
extern AudioConnection patchCord125;
extern AudioConnection patchCord126;
extern AudioConnection patchCord127;
extern AudioConnection patchCord128;
extern AudioConnection patchCord129;
extern AudioConnection patchCord130;
extern AudioConnection patchCord131;
extern AudioConnection patchCord132;
extern AudioConnection patchCord133;
extern AudioConnection patchCord134;
extern AudioConnection patchCord135;
extern AudioConnection patchCord136;
extern AudioConnection patchCord137;
extern AudioConnection patchCord138;
extern AudioConnection patchCord139;
extern AudioConnection patchCord140;
extern AudioConnection patchCord141;
extern AudioConnection patchCord142;
extern AudioConnection patchCord143;
extern AudioConnection patchCord144;
extern AudioConnection patchCord145;
extern AudioConnection patchCord146;
extern AudioConnection patchCord147;
extern AudioConnection patchCord148;
extern AudioConnection patchCord149;
extern AudioConnection patchCord150;
extern AudioConnection patchCord151;
extern AudioConnection patchCord152;
extern AudioConnection patchCord153;
extern AudioConnection patchCord154;
extern AudioConnection patchCord155;
extern AudioConnection patchCord156;
extern AudioConnection patchCord157;
extern AudioConnection patchCord158;
extern AudioConnection patchCord159;
extern AudioConnection patchCord160;
extern AudioConnection patchCord161;
extern AudioConnection patchCord162;
extern AudioConnection patchCord163;
extern AudioConnection patchCord164;
extern AudioConnection patchCord165;
extern AudioConnection patchCord166;
extern AudioConnection patchCord167;
extern AudioConnection patchCord168;
extern AudioConnection patchCord169;
extern AudioConnection patchCord170;
extern AudioConnection patchCord171;
extern AudioConnection patchCord172;
extern AudioConnection patchCord173;
extern AudioConnection patchCord174;
extern AudioConnection patchCord175;
extern AudioConnection patchCord176;
extern AudioConnection patchCord177;
extern AudioConnection patchCord178;
extern AudioConnection patchCord179;
extern AudioConnection patchCord180;
extern AudioConnection patchCord181;
extern AudioConnection patchCord182;

extern AudioControlSGTL5000 sgtl5000_1; // xy=3069.094924926758,2246.0713777542114
// GUItool: end automatically generated code

#endif /* XRAudioConfig_h */