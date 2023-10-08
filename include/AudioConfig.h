#ifndef AudioConfig_h
#define AudioConfig_h

#include <Arduino.h>
#include <Config.h>
#include <Audio.h>
#include <SerialFlash.h>
#include <TeensyVariablePlayback.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthNoiseWhite     vnoise3;        //xy=390,865
AudioSynthNoiseWhite     vnoise2;        //xy=392,605
AudioSynthNoiseWhite     vnoise4;        //xy=390,1114
AudioSynthNoiseWhite     vnoise1;        //xy=394,356
AudioSynthWaveformDc     vdc2;           //xy=393,650
AudioSynthWaveformDc     vdc3;           //xy=392,910
AudioSynthWaveform voscb2;         //xy=394,557
AudioSynthWaveform voscb3;         //xy=393,817
AudioSynthWaveformDc     vdc4;           //xy=392,1160
AudioSynthWaveform vosca2;         //xy=395,512
AudioSynthWaveform vosca3;         //xy=394,772
AudioSynthWaveform voscb4;         //xy=393,1067
AudioSynthWaveformDc     vdc1;           //xy=396,401
AudioSynthWaveform vosca4;         //xy=394,1022
AudioSynthWaveform voscb1;         //xy=397,308
AudioSynthWaveform vosca1;         //xy=398,263
AudioEffectEnvelope      vfilterenv2;    //xy=556,588
AudioMixer4              voscmix2;       //xy=557,532
AudioMixer4              voscmix3;       //xy=556,791
AudioMixer4              voscmix4;       //xy=556,1041
AudioEffectEnvelope      vfilterenv3;    //xy=556.857105255127,848.9999847412109
AudioEffectEnvelope      vfilterenv1;    //xy=559,342
AudioMixer4              voscmix1;       //xy=560,283
AudioEffectEnvelope      vfilterenv4;    //xy=559,1097
AudioPlayArrayResmp  vmsample1;  //xy=726.0000305175781,243.6666603088379
AudioPlayArrayResmp  vmsample2; //xy=730.0000305175781,492.00000953674316
AudioPlayArrayResmp  vmsample3; //xy=730.0000305175781,749.0000085830688
AudioPlayArrayResmp  vmsample4; //xy=730.0000076293945,1000.6666650772095
AudioFilterLadder        vlfilter1;      //xy=736.0000915527344,296.3333549499512
AudioFilterLadder        vlfilter2; //xy=739.999927520752,544.1667289733887
AudioFilterLadder        vlfilter3;      //xy=741.0000305175781,803.6666793823242
AudioFilterLadder        vlfilter4;      //xy=740.9999885559082,1053.6666288375854
AudioPlayArrayResmp  vmsample8; //xy=864.6666145324707,1867.6665539741516
AudioPlayArrayResmp  vmsample5; //xy=871.166675567627,1300.999927997589
AudioPlayArrayResmp  vmsample7; //xy=871.3333015441895,1667.6665539741516
AudioPlayArrayResmp  vmsample6; //xy=874.666675567627,1490.999927997589
AudioMixer4              vmix2;          //xy=887,531
AudioMixer4              vmix3;          //xy=886,791
AudioMixer4              vmix4;          //xy=886,1041
AudioMixer4              vmix1;          //xy=890,282
AudioPlayArrayResmp  vmsample16; //xy=880.6666145324707,3962.6665539741516
AudioPlayArrayResmp  vmsample13; //xy=887.166675567627,3395.999927997589
AudioPlayArrayResmp  vmsample15; //xy=887.3333015441895,3762.6665539741516
AudioPlayArrayResmp  vmsample14; //xy=890.666675567627,3585.999927997589
AudioPlayArrayResmp  vmsample12; //xy=908.1666145324707,2927.6665539741516
AudioPlayArrayResmp  vmsample9; //xy=914.666675567627,2360.999927997589
AudioPlayArrayResmp  vmsample11; //xy=914.8333015441895,2727.6665539741516
AudioPlayArrayResmp  vmsample10; //xy=918.166675567627,2550.999927997589
AudioEffectEnvelope      venv3;          //xy=1023,790
AudioEffectEnvelope      venv2;          //xy=1025,530
AudioEffectEnvelope      venv4;          //xy=1023,1040
AudioEffectEnvelope      venv8; //xy=1019.9999389648438,1866.6666259765625
AudioEffectEnvelope      venv1;          //xy=1028,281
AudioEffectEnvelope      venv5; //xy=1026.5,1300
AudioEffectEnvelope      venv7; //xy=1026.6666259765625,1666.6666259765625
AudioEffectEnvelope      venv6; //xy=1030,1490
AudioEffectEnvelope      venv16; //xy=1035.9999389648438,3961.6666259765625
AudioEffectEnvelope      venv13; //xy=1042.5,3395
AudioEffectEnvelope      venv15; //xy=1042.6666259765625,3761.6666259765625
AudioEffectEnvelope      venv14; //xy=1046,3585
AudioEffectEnvelope      venv12; //xy=1063.4999389648438,2926.6666259765625
AudioEffectEnvelope      venv9; //xy=1070,2360
AudioEffectEnvelope      venv11; //xy=1070.1666259765625,2726.6666259765625
AudioEffectEnvelope      venv10; //xy=1073.5,2550
AudioPlayWAVstereo       vssample2;      //xy=1172,457
AudioPlayWAVstereo       vssample3;      //xy=1171,716
AudioPlayWAVstereo       vssample8; //xy=1166.9999389648438,1793.6666259765625
AudioPlayWAVstereo       vssample4;      //xy=1171,966
AudioPlayWAVstereo       vssample1;      //xy=1175,208
AudioPlayWAVstereo       vssample5; //xy=1173.5,1227
AudioPlayWAVstereo       vssample7; //xy=1173.6666259765625,1593.6666259765625
AudioAmplifier           vleft3;         //xy=1177,772
AudioAmplifier           vleft8; //xy=1172.9999389648438,1848.6666259765625
AudioAmplifier           vleft2;         //xy=1179,512
AudioAmplifier           vleft4;         //xy=1177,1022
AudioAmplifier           vright3;        //xy=1178,811
AudioAmplifier           vleft1;         //xy=1181,263
AudioAmplifier           vright2;        //xy=1180,551
AudioAmplifier           vright4;        //xy=1178,1060
AudioAmplifier           vright8; //xy=1174.9999389648438,1887.6666259765625
AudioPlayWAVstereo       vssample6; //xy=1177,1417
AudioAmplifier           vright1;        //xy=1183,302
AudioAmplifier           vleft5; //xy=1179.5,1282
AudioAmplifier           vleft7; //xy=1179.6666259765625,1648.6666259765625
AudioAmplifier           vright5; //xy=1181.5,1321
AudioAmplifier           vright7; //xy=1181.6666259765625,1687.6666259765625
AudioAmplifier           vleft6; //xy=1183,1472
AudioAmplifier           vright6; //xy=1185,1511
AudioPlayWAVstereo       vssample16; //xy=1182.9999389648438,3888.6666259765625
AudioPlayWAVstereo       vssample13; //xy=1189.5,3322
AudioPlayWAVstereo       vssample15; //xy=1189.6666259765625,3688.6666259765625
AudioAmplifier           vleft16; //xy=1188.9999389648438,3943.6666259765625
AudioAmplifier           vright16; //xy=1190.9999389648438,3982.6666259765625
AudioPlayWAVstereo       vssample14; //xy=1193,3512
AudioAmplifier           vleft13; //xy=1195.5,3377
AudioAmplifier           vleft15; //xy=1195.6666259765625,3743.6666259765625
AudioAmplifier           vright13; //xy=1197.5,3416
AudioAmplifier           vright15; //xy=1197.6666259765625,3782.6666259765625
AudioAmplifier           vleft14; //xy=1199,3567
AudioAmplifier           vright14; //xy=1201,3606
AudioPlayWAVstereo       vssample12; //xy=1210.4999389648438,2853.6666259765625
AudioPlayWAVstereo       vssample9; //xy=1217,2287
AudioPlayWAVstereo       vssample11; //xy=1217.1666259765625,2653.6666259765625
AudioAmplifier           vleft12; //xy=1216.4999389648438,2908.6666259765625
AudioAmplifier           vright12; //xy=1218.4999389648438,2947.6666259765625
AudioPlayWAVstereo       vssample10; //xy=1220.5,2477
AudioAmplifier           vleft9; //xy=1223,2342
AudioAmplifier           vleft11; //xy=1223.1666259765625,2708.6666259765625
AudioAmplifier           vright9; //xy=1225,2381
AudioAmplifier           vright11; //xy=1225.1666259765625,2747.6666259765625
AudioAmplifier           vleft10; //xy=1226.5,2532
AudioAmplifier           vright10; //xy=1228.5,2571
AudioMixer4              vsubmixr4;      //xy=1379.6667213439941,1071.9999599456787
AudioMixer4              vsubmixl4;      //xy=1381.333351135254,1001.6666660308838
AudioMixer4              vsubmixl3;      //xy=1383.0001373291016,764.3333435058594
AudioMixer4              vsubmixr3;      //xy=1384.666763305664,833.666618347168
AudioMixer4              vsubmixl8; //xy=1383.5714073181152,1851.238042831421
AudioMixer4              vsubmixr8; //xy=1384.2380332946777,1923.238039970398
AudioMixer4              vsubmixl1;      //xy=1391.5714683532715,265.5714168548584
AudioMixer4              vsubmixr1;      //xy=1392.238094329834,337.57141399383545
AudioMixer4              vsubmixl5; //xy=1390.0714683532715,1284.5714168548584
AudioMixer4              vsubmixr5; //xy=1390.738094329834,1356.5714139938354
AudioMixer4              vsubmixl7; //xy=1390.238094329834,1651.238042831421
AudioMixer4              vsubmixl2;      //xy=1395.2379722595215,510.2857246398926
AudioMixer4              vsubmixr2;      //xy=1395.2381744384766,581.047568321228
AudioMixer4              vsubmixr7; //xy=1390.9047203063965,1723.238039970398
AudioMixer4              vsubmixl6; //xy=1393.5714683532715,1474.5714168548584
AudioMixer4              vsubmixr6; //xy=1394.238094329834,1546.5714139938354
AudioMixer4              vsubmixl16; //xy=1399.5714073181152,3946.238042831421
AudioMixer4              vsubmixr16; //xy=1400.2380332946777,4018.238039970398
AudioMixer4              vsubmixl13; //xy=1406.0714683532715,3379.5714168548584
AudioMixer4              vsubmixr13; //xy=1406.738094329834,3451.5714139938354
AudioMixer4              vsubmixl15; //xy=1406.238094329834,3746.238042831421
AudioMixer4              vsubmixr15; //xy=1406.9047203063965,3818.238039970398
AudioMixer4              vsubmixl14; //xy=1409.5714683532715,3569.5714168548584
AudioMixer4              vsubmixr14; //xy=1410.238094329834,3641.5714139938354
AudioMixer4              vsubmixl12; //xy=1427.0714073181152,2911.238042831421
AudioMixer4              vsubmixr12; //xy=1427.7380332946777,2983.238039970398
AudioMixer4              vsubmixr9; //xy=1434.238094329834,2416.5714139938354
AudioMixer4              vsubmixl11; //xy=1433.738094329834,2711.238042831421
AudioMixer4              vsubmixr11; //xy=1434.4047203063965,2783.238039970398
AudioMixer4              vsubmixl10; //xy=1437.0714683532715,2534.5714168548584
AudioMixer4              vsubmixl9; //xy=1437.857063293457,2341.7143173217773
AudioMixer4              vsubmixr10; //xy=1437.738094329834,2606.5714139938354
AudioMixer4              mixerLeft2; //xy=1859.1666984558105,1580.8333568572998
AudioMixer4              mixerRight4; //xy=1851.642807006836,3732.6428833007812
AudioMixer4              mixerLeft4; //xy=1852.5,3652.5
AudioMixer4              mixerRight2; //xy=1861.642734527588,1657.6428089141846
AudioMixer4              mixerLeft1;  //xy=1895.0000534057617,614.9999885559082
AudioMixer4              mixerRight3; //xy=1889.1428604125977,2710.1428718566895
AudioMixer4              mixerLeft3; //xy=1890.0000534057617,2629.999988555908
AudioMixer4              mixerRight1; //xy=1899.9761505126953,695.1428909301758
AudioMixer4              mainMixerLeft;         //xy=2377.5001792907715,2080.0001640319824
AudioMixer4              mainMixerRight;         //xy=2380.8331031799316,2151.6668338775635
AudioInputI2S            i2s2;           //xy=2400.000419616699,2333.333345413208
AudioAnalyzePeak         peak_left;          //xy=2406.666679382324,2285.000177383423
AudioAnalyzePeak         peak_right; //xy=2410.000015258789,2384.999855041504
AudioMixer4              inputMixerLeft;         //xy=2606.6663665771484,2303.3330993652344
AudioMixer4              inputMixerRight; //xy=2608.333351135254,2378.3333463668823
AudioMixer4              OutputMixerLeft; //xy=2866.6664505004883,2251.6665935516357
AudioMixer4              OutputMixerRight; //xy=2871.6666107177734,2323.3333435058594
AudioOutputI2S           i2s1;           //xy=3062.880729675293,2294.166513442993

AudioConnection          patchCord1(vnoise3, 0, voscmix3, 2);
AudioConnection          patchCord2(vnoise2, 0, voscmix2, 2);
AudioConnection          patchCord3(vnoise4, 0, voscmix4, 2);
AudioConnection          patchCord4(vnoise1, 0, voscmix1, 2);
AudioConnection          patchCord5(vdc2, vfilterenv2);
AudioConnection          patchCord6(vdc3, vfilterenv3);
AudioConnection          patchCord7(voscb2, 0, voscmix2, 1);
AudioConnection          patchCord8(voscb3, 0, voscmix3, 1);
AudioConnection          patchCord9(vdc4, vfilterenv4);
AudioConnection          patchCord10(vosca2, 0, voscmix2, 0);
AudioConnection          patchCord11(vosca3, 0, voscmix3, 0);
AudioConnection          patchCord12(voscb4, 0, voscmix4, 1);
AudioConnection          patchCord13(vdc1, vfilterenv1);
AudioConnection          patchCord14(vosca4, 0, voscmix4, 0);
AudioConnection          patchCord15(voscb1, 0, voscmix1, 1);
AudioConnection          patchCord16(vosca1, 0, voscmix1, 0);
AudioConnection          patchCord17(vfilterenv2, 0, vlfilter2, 1);
AudioConnection          patchCord18(vfilterenv2, 0, vlfilter2, 2);
AudioConnection          patchCord19(voscmix2, 0, vlfilter2, 0);
AudioConnection          patchCord20(voscmix3, 0, vlfilter3, 0);
AudioConnection          patchCord21(voscmix4, 0, vlfilter4, 0);
AudioConnection          patchCord22(vfilterenv3, 0, vlfilter3, 1);
AudioConnection          patchCord23(vfilterenv3, 0, vlfilter3, 2);
AudioConnection          patchCord24(vfilterenv1, 0, vlfilter1, 1);
AudioConnection          patchCord25(vfilterenv1, 0, vlfilter1, 2);
AudioConnection          patchCord26(voscmix1, 0, vlfilter1, 0);
AudioConnection          patchCord27(vfilterenv4, 0, vlfilter4, 1);
AudioConnection          patchCord28(vfilterenv4, 0, vlfilter4, 2);
AudioConnection          patchCord29(vmsample1, 0, vmix1, 0);
AudioConnection          patchCord30(vmsample2, 0, vmix2, 0);
AudioConnection          patchCord31(vmsample3, 0, vmix3, 0);
AudioConnection          patchCord32(vmsample4, 0, vmix4, 0);
AudioConnection          patchCord33(vlfilter1, 0, vmix1, 1);
AudioConnection          patchCord34(vlfilter2, 0, vmix2, 1);
AudioConnection          patchCord35(vlfilter3, 0, vmix3, 1);
AudioConnection          patchCord36(vlfilter4, 0, vmix4, 1);
AudioConnection          patchCord37(vmsample8, venv8);
AudioConnection          patchCord38(vmsample5, venv5);
AudioConnection          patchCord39(vmsample7, venv7);
AudioConnection          patchCord40(vmsample6, venv6);
AudioConnection          patchCord41(vmix2, venv2);
AudioConnection          patchCord42(vmix3, venv3);
AudioConnection          patchCord43(vmix4, venv4);
AudioConnection          patchCord44(vmix1, venv1);
AudioConnection          patchCord45(vmsample16, venv16);
AudioConnection          patchCord46(vmsample13, venv13);
AudioConnection          patchCord47(vmsample15, venv15);
AudioConnection          patchCord48(vmsample14, venv14);
AudioConnection          patchCord49(vmsample12, venv12);
AudioConnection          patchCord50(vmsample9, venv9);
AudioConnection          patchCord51(vmsample11, venv11);
AudioConnection          patchCord52(vmsample10, venv10);
AudioConnection          patchCord53(venv3, vleft3);
AudioConnection          patchCord54(venv3, vright3);
AudioConnection          patchCord55(venv2, vleft2);
AudioConnection          patchCord56(venv2, vright2);
AudioConnection          patchCord57(venv4, vleft4);
AudioConnection          patchCord58(venv4, vright4);
AudioConnection          patchCord59(venv8, vleft8);
AudioConnection          patchCord60(venv8, vright8);
AudioConnection          patchCord61(venv1, vleft1);
AudioConnection          patchCord62(venv1, vright1);
AudioConnection          patchCord63(venv5, vleft5);
AudioConnection          patchCord64(venv5, vright5);
AudioConnection          patchCord65(venv7, vleft7);
AudioConnection          patchCord66(venv7, vright7);
AudioConnection          patchCord67(venv6, vleft6);
AudioConnection          patchCord68(venv6, vright6);
AudioConnection          patchCord69(venv16, vleft16);
AudioConnection          patchCord70(venv16, vright16);
AudioConnection          patchCord71(venv13, vleft13);
AudioConnection          patchCord72(venv13, vright13);
AudioConnection          patchCord73(venv15, vleft15);
AudioConnection          patchCord74(venv15, vright15);
AudioConnection          patchCord75(venv14, vleft14);
AudioConnection          patchCord76(venv14, vright14);
AudioConnection          patchCord77(venv12, vleft12);
AudioConnection          patchCord78(venv12, vright12);
AudioConnection          patchCord79(venv9, vleft9);
AudioConnection          patchCord80(venv9, vright9);
AudioConnection          patchCord81(venv11, vleft11);
AudioConnection          patchCord82(venv11, vright11);
AudioConnection          patchCord83(venv10, vleft10);
AudioConnection          patchCord84(venv10, vright10);
AudioConnection          patchCord85(vssample2, 0, vsubmixl2, 0);
AudioConnection          patchCord86(vssample2, 1, vsubmixr2, 0);
AudioConnection          patchCord87(vssample3, 0, vsubmixl3, 0);
AudioConnection          patchCord88(vssample3, 1, vsubmixr3, 0);
AudioConnection          patchCord89(vssample8, 0, vsubmixl8, 0);
AudioConnection          patchCord90(vssample8, 1, vsubmixr8, 0);
AudioConnection          patchCord91(vssample4, 0, vsubmixl4, 0);
AudioConnection          patchCord92(vssample4, 1, vsubmixr4, 0);
AudioConnection          patchCord93(vssample1, 0, vsubmixl1, 0);
AudioConnection          patchCord94(vssample1, 1, vsubmixr1, 0);
AudioConnection          patchCord95(vssample5, 0, vsubmixl5, 0);
AudioConnection          patchCord96(vssample5, 1, vsubmixr5, 0);
AudioConnection          patchCord97(vssample7, 0, vsubmixl7, 0);
AudioConnection          patchCord98(vssample7, 1, vsubmixr7, 0);
AudioConnection          patchCord99(vleft3, 0, vsubmixl3, 1);
AudioConnection          patchCord100(vleft8, 0, vsubmixl8, 1);
AudioConnection          patchCord101(vleft2, 0, vsubmixl2, 1);
AudioConnection          patchCord102(vleft4, 0, vsubmixl4, 1);
AudioConnection          patchCord103(vright3, 0, vsubmixr3, 1);
AudioConnection          patchCord104(vleft1, 0, vsubmixl1, 1);
AudioConnection          patchCord105(vright2, 0, vsubmixr2, 1);
AudioConnection          patchCord106(vright4, 0, vsubmixr4, 1);
AudioConnection          patchCord107(vright8, 0, vsubmixr8, 1);
AudioConnection          patchCord108(vssample6, 0, vsubmixl6, 0);
AudioConnection          patchCord109(vssample6, 1, vsubmixr6, 0);
AudioConnection          patchCord110(vright1, 0, vsubmixr1, 1);
AudioConnection          patchCord111(vleft5, 0, vsubmixl5, 1);
AudioConnection          patchCord112(vleft7, 0, vsubmixl7, 1);
AudioConnection          patchCord113(vright5, 0, vsubmixr5, 1);
AudioConnection          patchCord114(vright7, 0, vsubmixr7, 1);
AudioConnection          patchCord115(vleft6, 0, vsubmixl6, 1);
AudioConnection          patchCord116(vright6, 0, vsubmixr6, 1);
AudioConnection          patchCord117(vssample16, 0, vsubmixl16, 0);
AudioConnection          patchCord118(vssample16, 1, vsubmixr16, 0);
AudioConnection          patchCord119(vssample13, 0, vsubmixl13, 0);
AudioConnection          patchCord120(vssample13, 1, vsubmixr13, 0);
AudioConnection          patchCord121(vssample15, 0, vsubmixl15, 0);
AudioConnection          patchCord122(vssample15, 1, vsubmixr15, 0);
AudioConnection          patchCord123(vleft16, 0, vsubmixl16, 1);
AudioConnection          patchCord124(vright16, 0, vsubmixr16, 1);
AudioConnection          patchCord125(vssample14, 0, vsubmixl14, 0);
AudioConnection          patchCord126(vssample14, 1, vsubmixr14, 0);
AudioConnection          patchCord127(vleft13, 0, vsubmixl13, 1);
AudioConnection          patchCord128(vleft15, 0, vsubmixl15, 1);
AudioConnection          patchCord129(vright13, 0, vsubmixr13, 1);
AudioConnection          patchCord130(vright15, 0, vsubmixr15, 1);
AudioConnection          patchCord131(vleft14, 0, vsubmixl14, 1);
AudioConnection          patchCord132(vright14, 0, vsubmixr14, 1);
AudioConnection          patchCord133(vssample12, 0, vsubmixl12, 0);
AudioConnection          patchCord134(vssample12, 1, vsubmixr12, 0);
AudioConnection          patchCord135(vssample9, 0, vsubmixl9, 0);
AudioConnection          patchCord136(vssample9, 1, vsubmixr9, 0);
AudioConnection          patchCord137(vssample11, 0, vsubmixl11, 0);
AudioConnection          patchCord138(vssample11, 1, vsubmixr11, 0);
AudioConnection          patchCord139(vleft12, 0, vsubmixl12, 1);
AudioConnection          patchCord140(vright12, 0, vsubmixr12, 1);
AudioConnection          patchCord141(vssample10, 0, vsubmixl10, 0);
AudioConnection          patchCord142(vssample10, 1, vsubmixr10, 0);
AudioConnection          patchCord143(vleft9, 0, vsubmixl9, 1);
AudioConnection          patchCord144(vleft11, 0, vsubmixl11, 1);
AudioConnection          patchCord145(vright9, 0, vsubmixr9, 1);
AudioConnection          patchCord146(vright11, 0, vsubmixr11, 1);
AudioConnection          patchCord147(vleft10, 0, vsubmixl10, 1);
AudioConnection          patchCord148(vright10, 0, vsubmixr10, 1);
AudioConnection          patchCord149(vsubmixr4, 0, mixerRight1, 3);
AudioConnection          patchCord150(vsubmixl4, 0, mixerLeft1, 3);
AudioConnection          patchCord151(vsubmixl3, 0, mixerLeft1, 2);
AudioConnection          patchCord152(vsubmixr3, 0, mixerRight1, 2);
AudioConnection          patchCord153(vsubmixl8, 0, mixerLeft2, 3);
AudioConnection          patchCord154(vsubmixr8, 0, mixerRight2, 3);
AudioConnection          patchCord155(vsubmixl1, 0, mixerLeft1, 0);
AudioConnection          patchCord156(vsubmixr1, 0, mixerRight1, 0);
AudioConnection          patchCord157(vsubmixl5, 0, mixerLeft2, 0);
AudioConnection          patchCord158(vsubmixr5, 0, mixerRight2, 0);
AudioConnection          patchCord159(vsubmixl7, 0, mixerLeft2, 2);
AudioConnection          patchCord160(vsubmixl2, 0, mixerLeft1, 1);
AudioConnection          patchCord161(vsubmixr2, 0, mixerRight1, 1);
AudioConnection          patchCord162(vsubmixr7, 0, mixerRight2, 2);
AudioConnection          patchCord163(vsubmixl6, 0, mixerLeft2, 1);
AudioConnection          patchCord164(vsubmixr6, 0, mixerRight2, 1);
AudioConnection          patchCord165(vsubmixl16, 0, mixerLeft4, 3);
AudioConnection          patchCord166(vsubmixr16, 0, mixerRight4, 3);
AudioConnection          patchCord167(vsubmixl13, 0, mixerLeft4, 0);
AudioConnection          patchCord168(vsubmixr13, 0, mixerRight4, 0);
AudioConnection          patchCord169(vsubmixl15, 0, mixerLeft4, 2);
AudioConnection          patchCord170(vsubmixr15, 0, mixerRight4, 2);
AudioConnection          patchCord171(vsubmixl14, 0, mixerLeft4, 1);
AudioConnection          patchCord172(vsubmixr14, 0, mixerRight4, 1);
AudioConnection          patchCord173(vsubmixl12, 0, mixerLeft3, 3);
AudioConnection          patchCord174(vsubmixr12, 0, mixerRight3, 3);
AudioConnection          patchCord175(vsubmixr9, 0, mixerRight3, 0);
AudioConnection          patchCord176(vsubmixl11, 0, mixerLeft3, 2);
AudioConnection          patchCord177(vsubmixr11, 0, mixerRight3, 2);
AudioConnection          patchCord178(vsubmixl10, 0, mixerLeft3, 1);
AudioConnection          patchCord179(vsubmixl9, 0, mixerLeft3, 0);
AudioConnection          patchCord180(vsubmixr10, 0, mixerRight3, 1);
AudioConnection          patchCord181(mixerLeft2, 0, mainMixerLeft, 1);
AudioConnection          patchCord182(mixerRight4, 0, mainMixerRight, 3);
AudioConnection          patchCord183(mixerLeft4, 0, mainMixerLeft, 3);
AudioConnection          patchCord184(mixerRight2, 0, mainMixerRight, 1);
AudioConnection          patchCord185(mixerLeft1, 0, mainMixerLeft, 0);
AudioConnection          patchCord186(mixerRight3, 0, mainMixerRight, 2);
AudioConnection          patchCord187(mixerLeft3, 0, mainMixerLeft, 2);
AudioConnection          patchCord188(mixerRight1, 0, mainMixerRight, 0);
AudioConnection          patchCord189(mainMixerLeft, 0, OutputMixerLeft, 0);
AudioConnection          patchCord190(mainMixerRight, 0, OutputMixerRight, 0);
AudioConnection          patchCord191(i2s2, 0, inputMixerLeft, 0);
AudioConnection          patchCord192(i2s2, 0, peak_left, 0);
AudioConnection          patchCord193(i2s2, 1, inputMixerRight, 0);
AudioConnection          patchCord194(i2s2, 1, peak_right, 0);
AudioConnection          patchCord195(inputMixerLeft, 0, OutputMixerLeft, 1);
AudioConnection          patchCord196(inputMixerRight, 0, OutputMixerRight, 1);
AudioConnection          patchCord197(OutputMixerLeft, 0, i2s1, 0);
AudioConnection          patchCord198(OutputMixerRight, 0, i2s1, 1);

AudioControlSGTL5000     sgtl5000_1;     //xy=3069.094924926758,2246.0713777542114
// GUItool: end automatically generated code


newdigate::audiosample *samples[MAX_PROJECT_RAW_SAMPLES];

class ComboVoice
{
  public:
    AudioPlayWAVstereo          &wSample;
    AudioPlayArrayResmp         &rSample;
    AudioSynthWaveform          &osca;
    AudioSynthWaveform          &oscb;
    AudioSynthNoiseWhite        &noise;
    AudioMixer4                 &oscMix;
    AudioSynthWaveformDc        &dc;
    AudioFilterLadder           &lfilter;
    AudioEffectEnvelope         &filterEnv;
    AudioMixer4                 &mix;
    AudioEffectEnvelope         &ampEnv;
    AudioAmplifier              &leftCtrl;
    AudioAmplifier              &rightCtrl;
    AudioMixer4                 &leftSubMix;
    AudioMixer4                 &rightSubMix;
 
    ComboVoice(
      AudioPlayWAVstereo          &wSample,
      AudioPlayArrayResmp         &rSample,
      AudioSynthWaveform          &osca,
      AudioSynthWaveform          &oscb,
      AudioSynthNoiseWhite        &noise,
      AudioMixer4                 &oscMix,
      AudioSynthWaveformDc        &dc,
      AudioFilterLadder           &lfilter,
      AudioEffectEnvelope         &filterEnv,
      AudioMixer4                 &mix,
      AudioEffectEnvelope         &ampEnv,
      AudioAmplifier              &leftCtrl,
      AudioAmplifier              &rightCtrl,
      AudioMixer4                 &leftSubMix,
      AudioMixer4                 &rightSubMix
    ) : wSample {wSample},
        rSample {rSample},
        osca {osca},
        oscb {oscb},
        noise {noise},
        oscMix {oscMix},
        dc {dc},
        lfilter {lfilter},
        filterEnv {filterEnv},
        mix {mix},
        ampEnv {ampEnv},
        leftCtrl {leftCtrl},
        rightCtrl {rightCtrl},
        leftSubMix {leftSubMix},
        rightSubMix {rightSubMix}
    {
      //
    }
};

class SampleVoice
{
  public:
    AudioPlayWAVstereo          &wSample;
    AudioPlayArrayResmp         &rSample;
    AudioEffectEnvelope         &ampEnv;
    AudioAmplifier              &leftCtrl;
    AudioAmplifier              &rightCtrl;
    AudioMixer4                 &leftSubMix;
    AudioMixer4                 &rightSubMix;
 
    SampleVoice(
      AudioPlayWAVstereo          &wSample,
      AudioPlayArrayResmp         &rSample,
      AudioEffectEnvelope         &ampEnv,
      AudioAmplifier              &leftCtrl,
      AudioAmplifier              &rightCtrl,
      AudioMixer4                 &leftSubMix,
      AudioMixer4                 &rightSubMix
    ) : wSample {wSample},
        rSample {rSample},
        ampEnv {ampEnv},
        leftCtrl {leftCtrl},
        rightCtrl {rightCtrl},
        leftSubMix {leftSubMix},
        rightSubMix {rightSubMix}
    {
      //
    }
};

ComboVoice comboVoices[COMBO_VOICE_COUNT] = {
  ComboVoice(
    vssample1,vmsample1,vosca1,voscb1,vnoise1,voscmix1,vdc1,vlfilter1,vfilterenv1,vmix1,venv1,vleft1,vright1,vsubmixl1,vsubmixr1
  ),
  ComboVoice(
    vssample2,vmsample2,vosca2,voscb2,vnoise2,voscmix2,vdc2,vlfilter2,vfilterenv2,vmix2,venv2,vleft2,vright2,vsubmixl2,vsubmixr2
  ),
  ComboVoice(
    vssample3,vmsample3,vosca3,voscb3,vnoise3,voscmix3,vdc3,vlfilter3,vfilterenv3,vmix3,venv3,vleft3,vright3,vsubmixl3,vsubmixr3
  ),
  ComboVoice(
    vssample4,vmsample4,vosca4,voscb4,vnoise4,voscmix4,vdc4,vlfilter4,vfilterenv4,vmix4,venv4,vleft4,vright4,vsubmixl4,vsubmixr4
  ),
};

SampleVoice sampleVoices[SAMPLE_VOICE_COUNT] = {
  SampleVoice(
    vssample5,vmsample5,venv5,vleft5,vright5,vsubmixl5,vsubmixr5
  ),
  SampleVoice(
    vssample6,vmsample6,venv6,vleft6,vright6,vsubmixl6,vsubmixr6
  ),
  SampleVoice(
    vssample7,vmsample7,venv7,vleft7,vright7,vsubmixl7,vsubmixr7
  ),
  SampleVoice(
    vssample8,vmsample8,venv8,vleft8,vright8,vsubmixl8,vsubmixr8
  ),
  SampleVoice(
    vssample9,vmsample9,venv9,vleft9,vright9,vsubmixl9,vsubmixr9
  ),
  SampleVoice(
    vssample10,vmsample10,venv10,vleft10,vright10,vsubmixl10,vsubmixr10
  ),
  SampleVoice(
    vssample11,vmsample11,venv11,vleft11,vright11,vsubmixl11,vsubmixr11
  ),
  SampleVoice(
    vssample12,vmsample12,venv12,vleft12,vright12,vsubmixl12,vsubmixr12
  ),
  SampleVoice(
    vssample13,vmsample13,venv13,vleft13,vright13,vsubmixl13,vsubmixr13
  ),
  SampleVoice(
    vssample14,vmsample14,venv14,vleft14,vright14,vsubmixl14,vsubmixr14
  ),
  SampleVoice(
    vssample15,vmsample15,venv15,vleft15,vright15,vsubmixl15,vsubmixr15
  ),
  SampleVoice(
    vssample16,vmsample16,venv16,vleft16,vright16,vsubmixl16,vsubmixr16
  ),
};

#endif /* AudioConfig_h */