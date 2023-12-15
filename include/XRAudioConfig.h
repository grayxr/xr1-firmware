#ifndef XRAudioConfig_h
#define XRAudioConfig_h

#include <Arduino.h>
#include <Audio.h>
#include <TeensyVariablePlayback.h>
#include <synth_dexed.h>
#include <synth_fm_drum.h>

// GUItool: begin automatically generated code
extern AudioSynthNoiseWhite     vnoise1;        //xy=395.999942779541,163.99999713897705
extern AudioSynthWaveformDc     vdc1;           //xy=397.999942779541,208.99999713897705
extern AudioSynthWaveform voscb1;         //xy=398.999942779541,115.99999713897705
extern AudioSynthWaveform vosca1;         //xy=399.999942779541,70.99999713897705
extern AudioSynthNoiseWhite     vnoise2;        //xy=401.28577041625977,507.85716342926025
extern AudioSynthWaveformDc     vdc2;           //xy=402.28577041625977,552.8571634292603
extern AudioSynthWaveform voscb2;         //xy=403.28577041625977,459.85716342926025
extern AudioSynthWaveform vosca2;         //xy=404.28577041625977,414.85716342926025
extern AudioSynthNoiseWhite     vnoise3;        //xy=404.99996185302734,875.0000052452087
extern AudioSynthWaveformDc     vdc3;           //xy=406.99996185302734,920.0000052452087
extern AudioSynthWaveform voscb3;         //xy=407.99996185302734,827.0000052452087
extern AudioSynthWaveform vosca3;         //xy=408.99996185302734,782.0000052452087
extern AudioSynthNoiseWhite     vnoise4;        //xy=409.99992752075195,1244.0000553131104
extern AudioSynthWaveformDc     vdc4;           //xy=411.99992752075195,1290.0000553131104
extern AudioSynthWaveform voscb4;         //xy=412.99992752075195,1197.0000553131104
extern AudioSynthWaveform vosca4;         //xy=413.99992752075195,1152.0000553131104
extern AudioEffectEnvelope      vfilterenv1;    //xy=560.999942779541,149.99999713897705
extern AudioMixer4              voscmix1;       //xy=561.999942779541,90.99999713897705
extern AudioEffectEnvelope      vfilterenv2;    //xy=565.2857704162598,490.85716342926025
extern AudioMixer4              voscmix2;       //xy=566.2857704162598,434.85716342926025
extern AudioMixer4              voscmix3;       //xy=570.9999618530273,801.0000052452087
extern AudioEffectEnvelope      vfilterenv3;    //xy=571.8570671081543,858.9999899864197
extern AudioMixer4              voscmix4;       //xy=575.999927520752,1171.0000553131104
extern AudioEffectEnvelope      vfilterenv4;    //xy=578.999927520752,1227.0000553131104
extern AudioPlayArrayResmp  vmsample1;  //xy=727.9999732971191,51.66665744781494
extern AudioFilterLadder        vlfilter1;      //xy=738.0000343322754,104.33335208892822
extern AudioPlayArrayResmp  vmsample2; //xy=739.2858009338379,394.8571729660034
extern AudioPlayArrayResmp  vmsample3; //xy=744.9999923706055,759.0000138282776
extern AudioFilterLadder        vlfilter2; //xy=749.2856979370117,447.0238924026489
extern AudioPlayArrayResmp  vmsample4; //xy=749.9999351501465,1130.6667203903198
extern AudioFilterLadder        vlfilter3;      //xy=755.9999923706055,813.666684627533
extern AudioFilterLadder        vlfilter4;      //xy=760.9999160766602,1183.6666841506958
extern AudioPlayArrayResmp  vmsample12; //xy=783.1667289733887,3182.666790485382
extern AudioPlayArrayResmp  vmsample5; //xy=791.1666717529297,1650.9999718666077
extern AudioPlayArrayResmp  vmsample7; //xy=789.6666679382324,2069.333248615265
extern AudioPlayArrayResmp  vmsample9; //xy=789.6667900085449,2616.0001645088196
extern AudioPlayArrayResmp  vmsample6; //xy=793.0000038146973,1859.333323955536
extern AudioPlayArrayResmp  vmsample8; //xy=791.3332939147949,2279.333134174347
extern AudioPlayArrayResmp  vmsample11; //xy=789.8334159851074,2982.666790485382
extern AudioPlayArrayResmp  vmsample10; //xy=793.1667900085449,2806.0001645088196
extern AudioPlayArrayResmp  vmsample16; //xy=820.6667747497559,4132.666794300079
extern AudioPlayArrayResmp  vmsample13; //xy=827.1668357849121,3566.000168323517
extern AudioPlayArrayResmp  vmsample15; //xy=827.3334617614746,3932.6667943000793
extern AudioPlayArrayResmp  vmsample14; //xy=830.6668357849121,3756.000168323517
extern AudioMixer4              vmix1;          //xy=891.999942779541,89.99999713897705
extern AudioMixer4              vmix2;          //xy=896.2857704162598,433.85716342926025
extern AudioMixer4              vmix3;          //xy=900.9999618530273,801.0000052452087
extern AudioMixer4              vmix4;          //xy=905.999927520752,1171.0000553131104
extern AudioEffectEnvelope      venv12; //xy=938.5000534057617,3181.666862487793
extern AudioEffectEnvelope      venv5; //xy=946.4999961853027,1650.0000438690186
extern AudioEffectEnvelope      venv7; //xy=944.9999923706055,2068.333320617676
extern AudioEffectEnvelope      venv9; //xy=945.000114440918,2615.0002365112305
extern AudioEffectEnvelope      venv6; //xy=948.3333282470703,1858.3333959579468
extern AudioEffectEnvelope      venv8; //xy=946.666618347168,2278.333206176758
extern AudioEffectEnvelope      venv11; //xy=945.1667404174805,2981.666862487793
extern AudioEffectEnvelope      venv10; //xy=948.500114440918,2805.0002365112305
extern AudioEffectEnvelope      venv16; //xy=976.0000991821289,4131.66686630249
extern AudioEffectEnvelope      venv13; //xy=982.5001602172852,3565.0002403259277
extern AudioEffectEnvelope      venv15; //xy=982.6667861938477,3931.6668663024902
extern AudioEffectEnvelope      venv14; //xy=986.0001602172852,3755.0002403259277
extern AudioEffectEnvelope      venv1;          //xy=1029.999942779541,88.99999713897705
extern AudioSynthDexed          dexed1;         //xy=1031.74995803833,199.50000286102295
extern AudioEffectEnvelope      venv2;          //xy=1034.2857704162598,432.85716342926025
extern AudioSynthFMDrum         fmdrum1;        //xy=1039.5,290
extern AudioEffectEnvelope      venv3;          //xy=1037.9999618530273,800.0000052452087
extern AudioSynthFMDrum         fmdrum2; //xy=1042.1427841186523,637.14288854599
extern AudioSynthDexed          dexed2; //xy=1043.5358047485352,547.6071829795837
extern AudioEffectEnvelope      venv4;          //xy=1042.999927520752,1170.0000553131104
extern AudioSynthDexed          dexed3; //xy=1044.4999771118164,914.2500185966492
extern AudioSynthFMDrum         fmdrum3; //xy=1047.5,1007.5
extern AudioSynthFMDrum         fmdrum4; //xy=1051.6666259765625,1376.6666259765625
extern AudioSynthDexed          dexed4; //xy=1052.7499618530273,1282.5000896453857
extern AudioAmplifier           vleft12; //xy=1091.5000534057617,3163.666862487793
extern AudioAmplifier           vleft5; //xy=1099.4999961853027,1632.0000438690186
extern AudioAmplifier           vleft7; //xy=1097.9999923706055,2050.333320617676
extern AudioAmplifier           vright12; //xy=1093.5000534057617,3202.666862487793
extern AudioAmplifier           vright5; //xy=1101.4999961853027,1671.0000438690186
extern AudioAmplifier           vright7; //xy=1099.9999923706055,2089.333320617676
extern AudioAmplifier           vleft9; //xy=1098.000114440918,2597.0002365112305
extern AudioAmplifier           vleft6; //xy=1101.3333282470703,1840.3333959579468
extern AudioAmplifier           vleft8; //xy=1099.666618347168,2260.333206176758
extern AudioAmplifier           vleft11; //xy=1098.1667404174805,2963.666862487793
extern AudioAmplifier           vright9; //xy=1100.000114440918,2636.0002365112305
extern AudioAmplifier           vright6; //xy=1103.3333282470703,1879.3333959579468
extern AudioAmplifier           vright8; //xy=1101.666618347168,2299.333206176758
extern AudioAmplifier           vright11; //xy=1100.1667404174805,3002.666862487793
extern AudioAmplifier           vleft10; //xy=1101.500114440918,2787.0002365112305
extern AudioAmplifier           vright10; //xy=1103.500114440918,2826.0002365112305
extern AudioAmplifier           vleft16; //xy=1129.000099182129,4113.66686630249
extern AudioAmplifier           vright16; //xy=1131.000099182129,4152.66686630249
extern AudioAmplifier           vleft13; //xy=1135.5001602172852,3547.0002403259277
extern AudioAmplifier           vleft15; //xy=1135.6667861938477,3913.6668663024902
extern AudioAmplifier           vright13; //xy=1137.5001602172852,3586.0002403259277
extern AudioAmplifier           vright15; //xy=1137.6667861938477,3952.6668663024902
extern AudioAmplifier           vleft14; //xy=1139.0001602172852,3737.0002403259277
extern AudioAmplifier           vright14; //xy=1141.0001602172852,3776.0002403259277
extern AudioAmplifier           vleft1;         //xy=1182.999942779541,70.99999713897705
extern AudioAmplifier           vright1;        //xy=1184.999942779541,109.99999713897705
extern AudioAmplifier           fdright1; //xy=1184.75,319.25000047683716
extern AudioAmplifier           fdleft1; //xy=1186,278
extern AudioAmplifier           dright1;           //xy=1186.9999618530273,214.25000286102295
extern AudioAmplifier           dleft1;           //xy=1188.2499618530273,173.0000023841858
extern AudioAmplifier           vleft2;         //xy=1188.2857704162598,414.85716342926025
extern AudioAmplifier           fdright2; //xy=1187.3927841186523,666.3928890228271
extern AudioAmplifier           dright2; //xy=1188.035789489746,560.3571739196777
extern AudioAmplifier           vright2;        //xy=1189.2857704162598,453.85716342926025
extern AudioAmplifier           fdleft2; //xy=1188.6427841186523,625.14288854599
extern AudioAmplifier           dleft2; //xy=1189.285789489746,519.1071734428406
extern AudioAmplifier           dright3; //xy=1191.2499809265137,930.0000185966492
extern AudioAmplifier           vleft3;         //xy=1191.9999618530273,782.0000052452087
extern AudioAmplifier           dleft3; //xy=1192.4999809265137,888.750018119812
extern AudioAmplifier           vright3;        //xy=1192.9999618530273,821.0000052452087
extern AudioAmplifier           fdright3; //xy=1192.75,1036.7500004768372
extern AudioAmplifier           fdleft3; //xy=1194,995.5
extern AudioAmplifier           vleft4;         //xy=1196.999927520752,1152.0000553131104
extern AudioAmplifier           fdright4; //xy=1196.9166259765625,1405.9166264533997
extern AudioAmplifier           dright4; //xy=1197.4999465942383,1297.5000729560852
extern AudioAmplifier           vright4;        //xy=1197.999927520752,1190.0000553131104
extern AudioAmplifier           fdleft4; //xy=1198.1666259765625,1364.6666259765625
extern AudioAmplifier           dleft4; //xy=1198.7499465942383,1256.250072479248
extern AudioMixer4              vsubmixl12; //xy=1302.0715217590332,3166.2382793426514
extern AudioMixer4              vsubmixr12; //xy=1302.7381477355957,3238.2382764816284
extern AudioMixer4              vsubmixl5; //xy=1310.0714645385742,1634.571460723877
extern AudioMixer4              vsubmixl7; //xy=1308.571460723877,2052.904737472534
extern AudioMixer4              vsubmixr5; //xy=1310.7380905151367,1706.571457862854
extern AudioMixer4              vsubmixr7; //xy=1309.2380867004395,2124.9047346115112
extern AudioMixer4              vsubmixl6; //xy=1311.9047966003418,1842.9048128128052
extern AudioMixer4              vsubmixl8; //xy=1310.2380867004395,2262.904623031616
extern AudioMixer4              vsubmixr9; //xy=1309.238208770752,2671.571650505066
extern AudioMixer4              vsubmixr6; //xy=1312.5714225769043,1914.9048099517822
extern AudioMixer4              vsubmixr8; //xy=1310.904712677002,2334.9046201705933
extern AudioMixer4              vsubmixl11; //xy=1308.738208770752,2966.2382793426514
extern AudioMixer4              vsubmixr11; //xy=1309.4048347473145,3038.2382764816284
extern AudioMixer4              vsubmixl10; //xy=1312.0715827941895,2789.571653366089
extern AudioMixer4              vsubmixl9; //xy=1312.857177734375,2596.714553833008
extern AudioMixer4              vsubmixr10; //xy=1312.738208770752,2861.571650505066
extern AudioMixer4              vsubmixl16; //xy=1339.5715675354004,4116.238283157349
extern AudioMixer4              vsubmixr16; //xy=1340.238193511963,4188.238280296326
extern AudioMixer4              vsubmixl13; //xy=1346.0716285705566,3549.571657180786
extern AudioMixer4              vsubmixr13; //xy=1346.7382545471191,3621.571654319763
extern AudioMixer4              vsubmixl15; //xy=1346.2382545471191,3916.2382831573486
extern AudioMixer4              vsubmixr15; //xy=1346.9048805236816,3988.2382802963257
extern AudioMixer4              vsubmixl14; //xy=1349.5716285705566,3739.571657180786
extern AudioMixer4              vsubmixr14; //xy=1350.2382545471191,3811.571654319763
extern AudioMixer4              vsubmixl1;      //xy=1393.5713661313057,73.57140827178955
extern AudioMixer4              vsubmixr1;      //xy=1394.238037109375,145.5714111328125
extern AudioMixer4              vsubmixl3;      //xy=1398.000099182129,774.3333487510681
extern AudioMixer4              vsubmixr3;      //xy=1399.6667251586914,843.6666235923767
extern AudioMixer4              vsubmixr4;      //xy=1399.666648864746,1202.000015258789
extern AudioMixer4              vsubmixl4;      //xy=1401.3332786560059,1131.6667213439941
extern AudioMixer4              vsubmixl2;      //xy=1404.5237426757812,413.14288806915283
extern AudioMixer4              vsubmixr2;      //xy=1404.5239448547363,483.9047317504883
extern AudioMixer4              mixerRight3; //xy=1764.1429748535156,2965.14310836792
extern AudioMixer4              mixerLeft3; //xy=1765.0001678466797,2885.0002250671387
extern AudioMixer4              mixerLeft2; //xy=1779.1666946411133,1930.8334007263184
extern AudioMixer4              mixerRight2; //xy=1781.6427307128906,2007.6428527832031
extern AudioMixer4              mixerRight4; //xy=1791.642967224121,3902.643123626709
extern AudioMixer4              mixerLeft4; //xy=1792.5001602172852,3822.5002403259277
extern AudioMixer4              mixerLeft1;  //xy=1895.0000534057617,614.9999885559082
extern AudioMixer4              mixerRight1; //xy=1899.9761505126953,695.1428909301758
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

extern AudioControlSGTL5000     sgtl5000_1;     //xy=2959.095054626465,2416.0714683532715
// GUItool: end automatically generated code

#endif /* XRAudioConfig_h */