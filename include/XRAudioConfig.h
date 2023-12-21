#ifndef XRAudioConfig_h
#define XRAudioConfig_h

#include <Arduino.h>
#include <Audio.h>
#include <TeensyVariablePlayback.h>
#include <synth_dexed.h>
#include <synth_fm_drum.h>
//#include <synth_braids.h>

// GUItool: begin automatically generated code
extern AudioSynthNoiseWhite     vnoise1;        //xy=395.999942779541,163.99999713897705
extern AudioSynthWaveformDc     vdc1;           //xy=397.999942779541,208.99999713897705
extern AudioSynthWaveform voscb1;         //xy=398.999942779541,115.99999713897705
extern AudioSynthWaveform vosca1;         //xy=399.999942779541,70.99999713897705
extern AudioSynthNoiseWhite     vnoise2;        //xy=402.7143783569336,557.1428909301758
extern AudioSynthWaveformDc     vdc2;           //xy=403.7143783569336,602.1428909301758
extern AudioSynthWaveform voscb2;         //xy=404.7143783569336,509.1428909301758
extern AudioSynthWaveform vosca2;         //xy=405.7143783569336,464.1428909301758
extern AudioSynthNoiseWhite     vnoise3;        //xy=405.0000591278076,929.9999985694885
extern AudioSynthWaveformDc     vdc3;           //xy=407.0000591278076,974.9999985694885
extern AudioSynthWaveform voscb3;         //xy=408.0000591278076,881.9999985694885
extern AudioSynthWaveform vosca3;         //xy=409.0000591278076,836.9999985694885
extern AudioSynthNoiseWhite     vnoise4;        //xy=408.5713996887207,1303.2857675552368
extern AudioSynthWaveformDc     vdc4;           //xy=410.5713996887207,1349.2857675552368
extern AudioSynthWaveform voscb4;         //xy=411.5713996887207,1256.2857675552368
extern AudioSynthWaveform vosca4;         //xy=412.5713996887207,1211.2857675552368
extern AudioEffectEnvelope      vfilterenv1;    //xy=560.999942779541,149.99999713897705
extern AudioMixer4              voscmix1;       //xy=561.999942779541,90.99999713897705
extern AudioEffectEnvelope      vfilterenv2;    //xy=566.7143783569336,540.1428909301758
extern AudioMixer4              voscmix2;       //xy=567.7143783569336,484.1428909301758
extern AudioMixer4              voscmix3;       //xy=571.0000591278076,855.9999985694885
extern AudioEffectEnvelope      vfilterenv3;    //xy=571.8571643829346,913.9999833106995
extern AudioMixer4              voscmix4;       //xy=574.5713996887207,1230.2857675552368
extern AudioEffectEnvelope      vfilterenv4;    //xy=577.5713996887207,1286.2857675552368
extern AudioPlayArrayResmp  vmsample1;  //xy=727.9999732971191,51.66665744781494
extern AudioFilterLadder        vlfilter1;      //xy=738.0000343322754,104.33335208892822
extern AudioPlayArrayResmp  vmsample2; //xy=740.7144088745117,444.14290046691895
extern AudioFilterLadder        vlfilter2; //xy=750.7143058776855,496.30961990356445
extern AudioPlayArrayResmp  vmsample4; //xy=748.5714073181152,1189.9524326324463
extern AudioPlayArrayResmp  vmsample3; //xy=755.000078201294,820.66663646698
extern AudioFilterLadder        vlfilter4;      //xy=759.5713882446289,1242.9523963928223
extern AudioFilterLadder        vlfilter3;      //xy=763.7778377532959,868.6666526794434
extern AudioMixer4              vmix1;          //xy=891.999942779541,89.99999713897705
extern AudioMixer4              vmix2;          //xy=897.7143478393555,478.6984519958496
extern AudioPlayArrayResmp  vmsample5; //xy=897.8333358764648,1670.9999451637268
extern AudioPlayArrayResmp  vmsample7; //xy=896.3333320617676,2089.333221912384
extern AudioPlayArrayResmp  vmsample6; //xy=899.6666679382324,1879.333297252655
extern AudioPlayArrayResmp  vmsample8; //xy=897.9999580383301,2299.333107471466
extern AudioPlayArrayResmp  vmsample12; //xy=896.5000457763672,3179.333426952362
extern AudioMixer4              vmix4;          //xy=904.5713996887207,1230.2857675552368
extern AudioPlayArrayResmp  vmsample16; //xy=894.0000457763672,4132.666792392731
extern AudioMixer4              vmix3;          //xy=907.6667346954346,852.6666526794434
extern AudioPlayArrayResmp  vmsample9; //xy=903.0001068115234,2612.6668009757996
extern AudioPlayArrayResmp  vmsample13; //xy=900.5001068115234,3566.000166416168
extern AudioPlayArrayResmp  vmsample11; //xy=903.1667327880859,2979.333426952362
extern AudioPlayArrayResmp  vmsample15; //xy=900.6667327880859,3932.6667923927307
extern AudioPlayArrayResmp  vmsample10; //xy=906.5001068115234,2802.6668009757996
extern AudioPlayArrayResmp  vmsample14; //xy=904.0001068115234,3756.000166416168
extern AudioEffectEnvelope      venv1;          //xy=1036.6665802001953,88.99999713897705
extern AudioSynthFMDrum         fmdrum1;        //xy=1041.1668548583984,264.999981880188
extern AudioSynthDexed          dexed1;         //xy=1045.0833740234375,182.27777481079102
extern AudioEffectEnvelope      venv4;          //xy=1041.5713996887207,1229.2857675552368
extern AudioEffectEnvelope      venv2;          //xy=1046.8254089355469,478.8095998764038
extern AudioSynthDexed          dexed3; //xy=1045.6111736297607,944.8055458068848
extern AudioSynthFMDrum         fmdrum2; //xy=1046.9048233032227,647.5397396087646
//extern AudioSynthBraids         braids1;        //xy=1050.555648803711,355.55557346343994
extern AudioSynthDexed          dexed2; //xy=1050.520004272461,571.3373847007751
extern AudioEffectEnvelope      venv3;          //xy=1050.2223224639893,852.7777900695801
extern AudioSynthFMDrum         fmdrum3; //xy=1050.8335208892822,1022.5000081062317
extern AudioSynthDexed          dexed4; //xy=1051.3212871551514,1331.785726070404
//extern AudioSynthBraids         braids2; //xy=1055.8729858398438,735.3968105316162
extern AudioEffectEnvelope      venv5; //xy=1053.166660308838,1670.0000171661377
//extern AudioSynthBraids         braids3; //xy=1055.5555515289307,1109.9998607635498
extern AudioEffectEnvelope      venv7; //xy=1051.6666564941406,2088.333293914795
extern AudioSynthFMDrum         fmdrum4; //xy=1054.5237846374512,1420.2381176948547
extern AudioEffectEnvelope      venv6; //xy=1054.9999923706055,1878.333369255066
extern AudioEffectEnvelope      venv8; //xy=1053.3332824707031,2298.333179473877
extern AudioEffectEnvelope      venv12; //xy=1051.8333702087402,3178.333498954773
extern AudioEffectEnvelope      venv16; //xy=1049.3333702087402,4131.666864395142
//extern AudioSynthBraids         braids4; //xy=1060,1507.142822265625
extern AudioEffectEnvelope      venv9; //xy=1058.3334312438965,2611.6668729782104
extern AudioEffectEnvelope      venv13; //xy=1055.8334312438965,3565.000238418579
extern AudioEffectEnvelope      venv11; //xy=1058.500057220459,2978.333498954773
extern AudioEffectEnvelope      venv15; //xy=1056.000057220459,3931.6668643951416
extern AudioEffectEnvelope      venv10; //xy=1061.8334312438965,2801.6668729782104
extern AudioEffectEnvelope      venv14; //xy=1059.3334312438965,3755.000238418579
extern AudioAmplifier           vleft1;         //xy=1182.999942779541,70.99999713897705
extern AudioAmplifier           vright1;        //xy=1183.8889617919922,105.55554962158203
extern AudioAmplifier           fdleft1; //xy=1187.666835784912,248.55554723739624
extern AudioAmplifier           dright1;           //xy=1188.1111221313477,194.80556297302246
extern AudioAmplifier           dleft1;           //xy=1188.2500343322754,160.22222805023193
extern AudioAmplifier           bleft1; //xy=1188.8888397216797,336.66665267944336
extern AudioAmplifier           fdright1; //xy=1189.7501831054688,283.1388530731201
extern AudioAmplifier           bright1; //xy=1189.7777481079102,371.2221965789795
extern AudioAmplifier           vleft2;         //xy=1189.7144088745117,460.80956840515137
extern AudioAmplifier           vright2;        //xy=1189.603271484375,495.3651123046875
extern AudioAmplifier           dleft2; //xy=1190.7144622802734,546.1706581115723
extern AudioAmplifier           dright2; //xy=1191.686622619629,580.7540454864502
extern AudioAmplifier           fdleft2; //xy=1193.4046630859375,631.095329284668
extern AudioAmplifier           dright3; //xy=1192.3612575531006,956.1111011505127
extern AudioAmplifier           dleft3; //xy=1192.5001583099365,921.5277671813965
extern AudioAmplifier           vleft3;         //xy=1193.1111736297607,833.6666851043701
extern AudioAmplifier           vright3;        //xy=1193.0000629425049,869.3332996368408
extern AudioAmplifier           bleft2; //xy=1194.2061767578125,716.5078897476196
extern AudioAmplifier           fdleft3; //xy=1194.0001621246338,1006.0555400848389
extern AudioAmplifier           bright2; //xy=1195.095085144043,751.0634336471558
extern AudioAmplifier           fdright2; //xy=1195.488021850586,665.678596496582
extern AudioAmplifier           bright3; //xy=1194.7775592803955,1126.7776126861572
extern AudioAmplifier           bleft3; //xy=1194.9996318817139,1091.111011505127
extern AudioAmplifier           vleft4;         //xy=1195.5713996887207,1211.2857675552368
extern AudioAmplifier           fdright3; //xy=1197.1944828033447,1041.7500200271606
extern AudioAmplifier           vright4;        //xy=1196.5713729858398,1246.4286365509033
extern AudioAmplifier           dleft4; //xy=1197.3212718963623,1305.5357089042664
extern AudioAmplifier           dright4; //xy=1197.4999446868896,1341.0713710784912
extern AudioAmplifier           fdleft4; //xy=1198.1666717529297,1399.6667757034302
extern AudioAmplifier           bright4;  //xy=1199.2220077514648,1523.9205741882324
extern AudioAmplifier           bleft4; //xy=1199.4440803527832,1488.2539730072021
extern AudioAmplifier           fdright4; //xy=1199.7738876342773,1435.2023859024048
extern AudioAmplifier           vleft5; //xy=1206.166660308838,1652.0000171661377
extern AudioAmplifier           vleft7; //xy=1204.6666564941406,2070.333293914795
extern AudioAmplifier           vright5; //xy=1208.166660308838,1691.0000171661377
extern AudioAmplifier           vright7; //xy=1206.6666564941406,2109.333293914795
extern AudioAmplifier           vleft6; //xy=1207.9999923706055,1860.333369255066
extern AudioAmplifier           vleft8; //xy=1206.3332824707031,2280.333179473877
extern AudioAmplifier           vleft12; //xy=1204.8333702087402,3160.333498954773
extern AudioAmplifier           vright6; //xy=1209.9999923706055,1899.333369255066
extern AudioAmplifier           vright8; //xy=1208.3332824707031,2319.333179473877
extern AudioAmplifier           vleft16; //xy=1202.3333702087402,4113.666864395142
extern AudioAmplifier           vright12; //xy=1206.8333702087402,3199.333498954773
extern AudioAmplifier           vright16; //xy=1204.3333702087402,4152.666864395142
extern AudioAmplifier           vleft9; //xy=1211.3334312438965,2593.6668729782104
extern AudioAmplifier           vleft13; //xy=1208.8334312438965,3547.000238418579
extern AudioAmplifier           vleft11; //xy=1211.500057220459,2960.333498954773
extern AudioAmplifier           vright9; //xy=1213.3334312438965,2632.6668729782104
extern AudioAmplifier           vleft15; //xy=1209.000057220459,3913.6668643951416
extern AudioAmplifier           vright13; //xy=1210.8334312438965,3586.000238418579
extern AudioAmplifier           vright11; //xy=1213.500057220459,2999.333498954773
extern AudioAmplifier           vleft10; //xy=1214.8334312438965,2783.6668729782104
extern AudioAmplifier           vright15; //xy=1211.000057220459,3952.6668643951416
extern AudioAmplifier           vleft14; //xy=1212.3334312438965,3737.000238418579
extern AudioAmplifier           vright10; //xy=1216.8334312438965,2822.6668729782104
extern AudioAmplifier           vright14; //xy=1214.3334312438965,3776.000238418579
extern AudioMixer4              vsubmixl1;      //xy=1393.5713661313057,73.57140827178955
extern AudioMixer4              vsubmixr1;      //xy=1394.238037109375,145.5714111328125
extern AudioMixer4              vsubmixl2;      //xy=1405.952350616455,462.42861557006836
extern AudioMixer4              vsubmixr2;      //xy=1405.9525527954102,533.1904592514038
extern AudioMixer4              vsubmixl3;      //xy=1409.111234664917,849.3333511352539
extern AudioMixer4              vsubmixr3;      //xy=1411.095293045044,915.4920082092285
extern AudioMixer4              vsubmixl5; //xy=1416.7381286621094,1654.571434020996
extern AudioMixer4              vsubmixl7; //xy=1415.238124847412,2072.9047107696533
extern AudioMixer4              vsubmixr5; //xy=1417.4047546386719,1726.5714311599731
extern AudioMixer4              vsubmixr7; //xy=1415.9047508239746,2144.9047079086304
extern AudioMixer4              vsubmixl6; //xy=1418.571460723877,1862.9047861099243
extern AudioMixer4              vsubmixl8; //xy=1416.9047508239746,2282.9045963287354
extern AudioMixer4              vsubmixl4;      //xy=1421.333351135254,1222.3810138702393
extern AudioMixer4              vsubmixr6; //xy=1419.2380867004395,1934.9047832489014
extern AudioMixer4              vsubmixr8; //xy=1417.571376800537,2354.9045934677124
extern AudioMixer4              vsubmixr4;      //xy=1422.5239028930664,1286.9999771118164
extern AudioMixer4              vsubmixl12; //xy=1415.4048385620117,3162.9049158096313
extern AudioMixer4              vsubmixr12; //xy=1416.0714645385742,3234.9049129486084
extern AudioMixer4              vsubmixl16; //xy=1412.9048385620117,4116.23828125
extern AudioMixer4              vsubmixr16; //xy=1413.5714645385742,4188.238278388977
extern AudioMixer4              vsubmixr9; //xy=1422.5715255737305,2668.238286972046
extern AudioMixer4              vsubmixl13; //xy=1419.404899597168,3549.5716552734375
extern AudioMixer4              vsubmixl11; //xy=1422.0715255737305,2962.9049158096313
extern AudioMixer4              vsubmixr13; //xy=1420.0715255737305,3621.5716524124146
extern AudioMixer4              vsubmixr11; //xy=1422.738151550293,3034.9049129486084
extern AudioMixer4              vsubmixl15; //xy=1419.5715255737305,3916.23828125
extern AudioMixer4              vsubmixr15; //xy=1420.238151550293,3988.238278388977
extern AudioMixer4              vsubmixl10; //xy=1425.404899597168,2786.238289833069
extern AudioMixer4              vsubmixl9; //xy=1426.1904945373535,2593.381190299988
extern AudioMixer4              vsubmixr10; //xy=1426.0715255737305,2858.238286972046
extern AudioMixer4              vsubmixl14; //xy=1422.904899597168,3739.5716552734375
extern AudioMixer4              vsubmixr14; //xy=1423.5715255737305,3811.5716524124146
extern AudioMixer4              mixerLeft2; //xy=1884.8808822631836,1933.690444946289
extern AudioMixer4              mixerRight2; //xy=1887.356918334961,2010.4998970031738
extern AudioMixer4              mixerLeft1;  //xy=1895.0000495910645,654.9999694824219
extern AudioMixer4              mixerRight1; //xy=1899.976146697998,735.1428718566895
extern AudioMixer4              mixerRight3; //xy=1905.5715713500977,2926.5715103149414
extern AudioMixer4              mixerLeft3; //xy=1906.4287643432617,2846.42862701416
extern AudioMixer4              mixerRight4; //xy=1915.9286804199219,3904.0715713500977
extern AudioMixer4              mixerLeft4; //xy=1916.785873413086,3823.9286880493164
extern AudioMixer4              mainMixerLeft;         //xy=2267.5003089904785,2250.0002546310425
extern AudioMixer4              mainMixerRight;         //xy=2270.8332328796387,2321.6669244766235
extern AudioInputI2S            i2s2;           //xy=2290.0005493164062,2503.333436012268
extern AudioAnalyzePeak         peak_left;          //xy=2296.6668090820312,2455.000267982483
extern AudioAnalyzePeak         peak_right; //xy=2300.000144958496,2554.999945640564
extern AudioMixer4              inputMixerLeft;         //xy=2496.6664962768555,2473.3331899642944
extern AudioMixer4              inputMixerRight; //xy=2498.333480834961,2548.3334369659424
extern AudioMixer4              OutputMixerLeft; //xy=2756.6665802001953,2421.666684150696
extern AudioMixer4              OutputMixerRight; //xy=2761.6667404174805,2493.3334341049194
extern AudioOutputI2S           i2s1;           //xy=2952.880859375,2464.166604042053

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

extern AudioControlSGTL5000     sgtl5000_1;     //xy=2959.095054626465,2416.0714683532715
// GUItool: end automatically generated code


#endif /* XRAudioConfig_h */