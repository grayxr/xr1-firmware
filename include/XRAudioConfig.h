#ifndef XRAudioConfig_h
#define XRAudioConfig_h

#include <Arduino.h>
#include <Audio.h>
#include <TeensyVariablePlayback.h>
#ifndef NO_DEXED
#include <synth_dexed.h>
#endif
#ifndef NO_FMDRUM
#include <synth_fm_drum.h>
#endif
// #include <synth_braids.h>
#ifdef BUILD_FOR_LINUX
#include <output_soundio.h>
#include <input_soundio.h>
#endif

// GUItool: begin automatically generated code
extern AudioSynthWaveformDc     monoSynthDc2; //xy=128.33333206176758,474.28567600250244
extern AudioSynthWaveformDc     monoSynthDc1;           //xy=130.49998092651367,278.4048204421997
extern AudioSynthWaveformDc     monoSynthDc3; //xy=130.33338928222656,672.6190195083618
extern AudioSynthWaveformDc     monoSynthDc4; //xy=130.33338928222656,869.2856454849243
extern AudioSynthNoiseWhite     monoSynthNoise2; //xy=136.33327102661133,434.9999694824219
extern AudioSynthNoiseWhite     monoSynthNoise1;        //xy=138.49991989135742,239.11911392211914
extern AudioSynthWaveform monoSynthOscA2; //xy=138.9047622680664,356.2856664657593
extern AudioSynthNoiseWhite     monoSynthNoise3; //xy=138.3333282470703,633.3333129882812
extern AudioSynthWaveform monoSynthOscB2; //xy=139.33329391479492,395.5713882446289
extern AudioSynthNoiseWhite     monoSynthNoise4; //xy=138.3333282470703,829.9999389648438
extern AudioSynthWaveform monoSynthOscA1;         //xy=141.0714111328125,160.40481090545654
extern AudioSynthWaveform monoSynthOscB1;         //xy=141.49994277954102,199.69053268432617
extern AudioSynthWaveform monoSynthOscA3; //xy=140.9048194885254,554.6190099716187
extern AudioSynthWaveform monoSynthOscB3; //xy=141.3333511352539,593.9047317504883
extern AudioSynthWaveform monoSynthOscA4; //xy=140.9048194885254,751.2856359481812
extern AudioSynthWaveform monoSynthOscB4; //xy=141.3333511352539,790.5713577270508
extern AudioMixer4              monoSynthMix2; //xy=400.1904525756836,373.19044303894043
extern AudioMixer4              monoSynthMix1;       //xy=402.3571014404297,177.3095874786377
extern AudioMixer4              monoSynthMix3; //xy=402.1905097961426,571.5237865447998
extern AudioMixer4              monoSynthMix4; //xy=402.1905097961426,768.1904125213623
extern AudioEffectEnvelope      monoSynthFilterEnv2; //xy=418.0000228881836,428.142822265625
extern AudioEffectEnvelope      monoSynthFilterEnv1;    //xy=420.1666717529297,232.26196670532227
extern AudioEffectEnvelope      monoSynthFilterEnv3; //xy=420.0000801086426,626.4761657714844
extern AudioEffectEnvelope      monoSynthFilterEnv4; //xy=420.0000801086426,823.1427917480469
extern AudioAmplifier           monoSynthFilterAccent2; //xy=427.83338928222656,467.5474967956543
extern AudioAmplifier           monoSynthFilterAccent1; //xy=430.00003814697266,271.66664123535156
extern AudioAmplifier           monoSynthFilterAccent3; //xy=429.83344650268555,665.8808403015137
extern AudioAmplifier           monoSynthFilterAccent4; //xy=429.83344650268555,862.5474662780762
extern AudioFilterLadder        monoSynthFilter2; //xy=680.4763717651367,425.80951976776123
extern AudioFilterLadder        monoSynthFilter1;      //xy=682.6430206298828,229.9286642074585
extern AudioFilterLadder        monoSynthFilter3; //xy=682.4764289855957,624.1428632736206
extern AudioFilterLadder        monoSynthFilter4; //xy=682.4764289855957,820.8094892501831
extern AudioPlayArrayResmp  monoSample4; //xy=806.6666412353516,2568.3332948684692
extern AudioPlayArrayResmp  monoSample3; //xy=808.3333282470703,2408.3332948684692
extern AudioPlayArrayResmp  monoSample2; //xy=809.9999542236328,2245.0000429153442
extern AudioPlayArrayResmp  monoSample1; //xy=811.1906280517578,2080.095507621765
extern AudioPlayArrayResmp  monoSample16; //xy=807.1427230834961,4659.904113292694
extern AudioPlayArrayResmp  monoSample15; //xy=808.8094100952148,4499.904113292694
extern AudioPlayArrayResmp  monoSample14; //xy=810.4760360717773,4336.570861339569
extern AudioPlayArrayResmp  monoSample13; //xy=811.6667098999023,4171.66632604599
extern AudioPlayArrayResmp  monoSample8; //xy=815.4759521484375,3263.237787246704
extern AudioPlayArrayResmp  monoSample7; //xy=817.1426391601562,3103.237787246704
extern AudioPlayArrayResmp  monoSample12; //xy=813.809455871582,3951.5711240768433
extern AudioPlayArrayResmp  monoSample6; //xy=818.8092651367188,2939.904535293579
extern AudioPlayArrayResmp  monoSample11; //xy=815.4761428833008,3791.5711240768433
extern AudioPlayArrayResmp  monoSample5; //xy=819.9999389648438,2775
extern AudioPlayArrayResmp  monoSample10; //xy=817.1427688598633,3628.2378721237183
extern AudioPlayArrayResmp  monoSample9; //xy=818.3334426879883,3463.333336830139
#ifndef NO_DEXED
extern AudioSynthDexed          dexed3; //xy=846.6665954589844,1231.666669845581
extern AudioSynthDexed          dexed2; //xy=849.9999351501465,1111.6667022705078
extern AudioSynthDexed          dexed1; //xy=850.9008483886719,991.0756568908691
extern AudioSynthDexed          dexed4; //xy=849.9999351501465,1356.6667022705078
#endif
#ifndef NO_FMDRUM
extern AudioSynthFMDrum         fmDrum3; //xy=865.0000228881836,1789.9999504089355
extern AudioSynthFMDrum         fmDrum2; //xy=870.0000228881836,1659.9999504089355
extern AudioSynthFMDrum         fmDrum1; //xy=874.8809852600098,1534.5636081695557
#endif
// extern AudioSynthBraids         braids1;        //xy=874.9999732971191,1918.3333282470703
extern AudioAmplifier           monoSynthAmp2; //xy=929.4998931884766,450.8808708190918
extern AudioAmplifier           monoSynthAmp1; //xy=931.6665420532227,255.00001525878906
extern AudioAmplifier           monoSynthAmp3; //xy=931.4999504089355,649.2142143249512
extern AudioAmplifier           monoSynthAmp4; //xy=931.4999504089355,845.8808403015137
extern AudioEffectEnvelope      monoSynthAmpEnv2; //xy=942.2378921508789,374.5237913131714
extern AudioEffectEnvelope      monoSynthAmpEnv1;          //xy=944.404541015625,178.64293575286865
extern AudioEffectEnvelope      monoSynthAmpEnv3; //xy=944.2379493713379,572.8571348190308
extern AudioEffectEnvelope      monoSynthAmpEnv4; //xy=944.2379493713379,769.5237607955933
extern AudioAmplifier           monoSynthAmpAccent2; //xy=951.1667022705078,412.54753589630127
extern AudioAmplifier           monoSynthAmpAccent1; //xy=953.3333511352539,216.66668033599854
extern AudioAmplifier           monoSynthAmpAccent3; //xy=953.1667594909668,610.8808794021606
extern AudioAmplifier           monoSynthAmpAccent4; //xy=953.1667594909668,807.5475053787231
#ifndef NO_DEXED
extern AudioAmplifier           dexedAmp3; //xy=989.0989875793457,1273.9244232177734
extern AudioAmplifier           dexedAmp2; //xy=992.4323272705078,1153.9244556427002
extern AudioAmplifier           dexedAmp1; //xy=993.3332405090332,1033.3334102630615
extern AudioAmplifier           dexedAmp4; //xy=992.4323272705078,1398.9244556427002
extern AudioAmplifier           dexedAmpAccent3; //xy=1010.7658271789551,1230.591007232666
#endif
extern AudioEffectEnvelope      monoSampleAmpEnv4; //xy=1008.0950241088867,2568.476113319397
#ifndef NO_DEXED
extern AudioAmplifier           dexedAmpAccent2; //xy=1014.0991668701172,1110.5910396575928
extern AudioAmplifier           dexedAmpAccent1; //xy=1015.0000801086426,989.9999942779541
#endif
extern AudioEffectEnvelope      monoSampleAmpEnv3; //xy=1009.7617111206055,2408.476113319397
#ifndef NO_DEXED
extern AudioAmplifier           dexedAmpAccent4; //xy=1014.0991668701172,1355.5910396575928
#endif
extern AudioEffectEnvelope      monoSampleAmpEnv2; //xy=1011.428337097168,2245.142861366272
extern AudioEffectEnvelope      monoSampleAmpEnv1; //xy=1012.619010925293,2080.238326072693
extern AudioEffectEnvelope      monoSampleAmpEnv16; //xy=1008.5711059570312,4660.046931743622
extern AudioEffectEnvelope      monoSampleAmpEnv15; //xy=1010.23779296875,4500.046931743622
extern AudioEffectEnvelope      monoSampleAmpEnv14; //xy=1011.9044189453125,4336.713679790497
extern AudioAmplifier           monoSampleAmpAccent4; //xy=1018.8093414306641,2614.9043340682983
extern AudioEffectEnvelope      monoSampleAmpEnv13; //xy=1013.0950927734375,4171.809144496918
extern AudioEffectEnvelope      monoSampleAmpEnv8; //xy=1016.9043350219727,3263.380605697632
extern AudioAmplifier           monoSampleAmpAccent3; //xy=1020.4760284423828,2454.9043340682983
extern AudioEffectEnvelope      monoSampleAmpEnv7; //xy=1018.5710220336914,3103.380605697632
extern AudioEffectEnvelope      monoSampleAmpEnv12; //xy=1015.2378387451172,3951.713942527771
extern AudioAmplifier           monoSampleAmpAccent2; //xy=1022.1426544189453,2291.5710821151733
extern AudioAmplifier           monoSampleAmpAccent1; //xy=1023.3333282470703,2126.6665468215942
extern AudioEffectEnvelope      monoSampleAmpEnv6; //xy=1020.2376480102539,2940.047353744507
extern AudioEffectEnvelope      monoSampleAmpEnv11; //xy=1016.9045257568359,3791.713942527771
extern AudioEffectEnvelope      monoSampleAmpEnv5; //xy=1021.4283218383789,2775.1428184509277
extern AudioEffectEnvelope      monoSampleAmpEnv10; //xy=1018.5711517333984,3628.380690574646
extern AudioEffectEnvelope      monoSampleAmpEnv9; //xy=1019.7618255615234,3463.476155281067
extern AudioAmplifier           monoSampleAmp4; //xy=1023.8093185424805,2661.571249961853
extern AudioAmplifier           monoSampleAmp3; //xy=1025.4760055541992,2501.571249961853
extern AudioAmplifier           monoSampleAmp2; //xy=1027.1426315307617,2338.237998008728
extern AudioAmplifier           monoSampleAmp1; //xy=1028.3333053588867,2173.333462715149
extern AudioAmplifier           monoSampleAmpAccent16; //xy=1019.2854232788086,4706.475152492523
extern AudioAmplifier           monoSampleAmpAccent15; //xy=1020.9521102905273,4546.475152492523
extern AudioAmplifier           monoSampleAmpAccent14; //xy=1022.6187362670898,4383.141900539398
extern AudioAmplifier           monoSampleAmpAccent13; //xy=1023.8094100952148,4218.237365245819
extern AudioAmplifier           monoSampleAmpAccent8; //xy=1027.61865234375,3309.808826446533
extern AudioAmplifier           monoSampleAmpAccent7; //xy=1029.2853393554688,3149.808826446533
extern AudioAmplifier           monoSampleAmpAccent12; //xy=1025.9521560668945,3998.1421632766724
extern AudioAmplifier           monoSampleAmpAccent6; //xy=1030.9519653320312,2986.475574493408
extern AudioAmplifier           monoSampleAmpAccent11; //xy=1027.6188430786133,3838.1421632766724
extern AudioAmplifier           monoSampleAmp16; //xy=1024.285400390625,4753.142068386078
extern AudioAmplifier           monoSampleAmpAccent5; //xy=1032.1426391601562,2821.571039199829
extern AudioAmplifier           monoSampleAmpAccent10; //xy=1029.2854690551758,3674.8089113235474
extern AudioAmplifier           monoSampleAmp15; //xy=1025.9520874023438,4593.142068386078
extern AudioAmplifier           monoSampleAmpAccent9; //xy=1030.4761428833008,3509.9043760299683
extern AudioAmplifier           monoSampleAmp14; //xy=1027.6187133789062,4429.808816432953
extern AudioAmplifier           monoSampleAmp13; //xy=1028.8093872070312,4264.904281139374
extern AudioAmplifier           monoSampleAmp8; //xy=1032.6186294555664,3356.475742340088
extern AudioAmplifier           braidsAmpAccent1; //xy=1038.4523735046387,1918.769666671753
extern AudioAmplifier           monoSampleAmp7; //xy=1034.2853164672852,3196.475742340088
extern AudioAmplifier           monoSampleAmp12; //xy=1030.952133178711,4044.809079170227
extern AudioAmplifier           braidsAmp1; //xy=1040.1191215515137,1967.1030406951904
extern AudioAmplifier           monoSampleAmp6; //xy=1035.9519424438477,3033.142490386963
extern AudioAmplifier           monoSampleAmp11; //xy=1032.6188201904297,3884.809079170227
extern AudioAmplifier           monoSampleAmp5; //xy=1037.1426162719727,2868.237955093384
extern AudioAmplifier           monoSampleAmp10; //xy=1034.2854461669922,3721.475827217102
extern AudioAmplifier           monoSampleAmp9; //xy=1035.4761199951172,3556.571291923523
extern AudioAmplifier           fmDrumAmpAccent3; //xy=1043.4523735046387,1790.4362926483154
extern AudioAmplifier           fmDrumAmp3; //xy=1045.1191215515137,1838.769666671753
extern AudioAmplifier           fmDrumAmpAccent2; //xy=1048.4523735046387,1660.4362926483154
extern AudioAmplifier           fmDrumAmp2; //xy=1050.1191215515137,1708.769666671753
extern AudioAmplifier           fmDrumAmpAccent1; //xy=1053.3333358764648,1534.9999504089355
extern AudioAmplifier           fmDrumAmp1; //xy=1055.0000839233398,1583.333324432373
extern AudioAmplifier           braidsLeft1; //xy=1235.3093223571777,1948.7936477661133
extern AudioAmplifier           dexedLeft3; //xy=1238.765796661377,1257.4523181915283
extern AudioAmplifier           dexedLeft2; //xy=1242.099136352539,1137.452350616455
extern AudioAmplifier           dexedLeft1; //xy=1243.0000495910645,1016.8613052368164
extern AudioAmplifier           fmDrumLeft3; //xy=1240.3093223571777,1820.4602737426758
extern AudioAmplifier           dexedLeft4; //xy=1242.099136352539,1382.452350616455
extern AudioAmplifier           braidsRight1; //xy=1240.2498359680176,1987.662621498108
extern AudioAmplifier           dexedRight3; //xy=1244.0237159729004,1294.8928680419922
extern AudioAmplifier           monoSynthLeft2; //xy=1247.6190147399902,410.09516048431396
extern AudioAmplifier           monoSynthLeft1;         //xy=1249.7856636047363,214.21430492401123
extern AudioAmplifier           monoSynthLeft3; //xy=1249.6190719604492,608.4285039901733
extern AudioAmplifier           dexedRight2; //xy=1247.3570556640625,1174.892900466919
extern AudioAmplifier           fmDrumLeft2; //xy=1245.3093223571777,1690.4602737426758
extern AudioAmplifier           dexedRight1; //xy=1248.257968902588,1054.3018550872803
extern AudioAmplifier           fmDrumRight3; //xy=1245.2498359680176,1859.3292474746704
extern AudioAmplifier           monoSynthLeft4; //xy=1249.6190719604492,805.0951299667358
extern AudioAmplifier           dexedRight4; //xy=1247.3570556640625,1419.892900466919
extern AudioAmplifier           monoSynthRight2; //xy=1251.3650207519531,447.507869720459
extern AudioAmplifier           monoSynthRight1;        //xy=1253.5316696166992,251.62701416015625
extern AudioAmplifier           monoSynthRight3; //xy=1253.365077972412,645.8412132263184
extern AudioAmplifier           fmDrumLeft1; //xy=1250.190284729004,1565.023931503296
extern AudioAmplifier           monoSynthRight4; //xy=1253.365077972412,842.5078392028809
extern AudioAmplifier           fmDrumRight2; //xy=1250.2498359680176,1729.3292474746704
extern AudioAmplifier           fmDrumRight1; //xy=1255.1307983398438,1603.8929052352905
extern AudioAmplifier           monoSampleLeft4; //xy=1251.5712585449219,2638.809417247772
extern AudioAmplifier           monoSampleLeft3; //xy=1253.2379455566406,2478.809417247772
extern AudioAmplifier           monoSampleLeft2; //xy=1254.9045715332031,2315.476165294647
extern AudioAmplifier           monoSampleLeft1; //xy=1256.0952453613281,2150.571630001068
extern AudioAmplifier           monoSampleRight4; //xy=1255.3173522949219,2677.650682926178
extern AudioAmplifier           monoSampleRight3; //xy=1256.9840393066406,2517.650682926178
extern AudioAmplifier           monoSampleRight2; //xy=1258.6506652832031,2354.317430973053
extern AudioAmplifier           monoSampleRight1; //xy=1259.8413391113281,2189.412895679474
extern AudioAmplifier           monoSampleLeft16; //xy=1252.0473403930664,4730.380235671997
extern AudioAmplifier           monoSampleLeft15; //xy=1253.7140274047852,4570.380235671997
extern AudioAmplifier           monoSampleLeft14; //xy=1255.3806533813477,4407.046983718872
extern AudioAmplifier           monoSampleLeft13; //xy=1256.5713272094727,4242.142448425293
extern AudioAmplifier           monoSampleLeft8; //xy=1260.3805694580078,3333.713909626007
extern AudioAmplifier           monoSampleLeft7; //xy=1262.0472564697266,3173.713909626007
extern AudioAmplifier           monoSampleLeft12; //xy=1258.7140731811523,4022.0472464561462
extern AudioAmplifier           monoSampleRight16; //xy=1255.7934341430664,4769.221501350403
extern AudioAmplifier           monoSampleLeft6; //xy=1263.713882446289,3010.380657672882
extern AudioAmplifier           monoSampleLeft11; //xy=1260.380760192871,3862.0472464561462
extern AudioAmplifier           monoSampleRight15; //xy=1257.4601211547852,4609.221501350403
extern AudioAmplifier           monoSampleLeft5;  //xy=1264.904556274414,2845.476122379303
extern AudioAmplifier           monoSampleLeft10; //xy=1262.0473861694336,3698.7139945030212
extern AudioAmplifier           monoSampleRight14; //xy=1259.1267471313477,4445.888249397278
extern AudioAmplifier           monoSampleLeft9; //xy=1263.2380599975586,3533.809459209442
extern AudioAmplifier           monoSampleRight13; //xy=1260.3174209594727,4280.983714103699
extern AudioAmplifier           monoSampleRight8;  //xy=1264.1266632080078,3372.555175304413
extern AudioAmplifier           monoSampleRight7; //xy=1265.7933502197266,3212.555175304413
extern AudioAmplifier           monoSampleRight12; //xy=1262.4601669311523,4060.888512134552
extern AudioAmplifier           monoSampleRight6; //xy=1267.459976196289,3049.221923351288
extern AudioAmplifier           monoSampleRight11; //xy=1264.126853942871,3900.888512134552
extern AudioAmplifier           monoSampleRight5; //xy=1268.650650024414,2884.3173880577087
extern AudioAmplifier           monoSampleRight10; //xy=1265.7934799194336,3737.555260181427
extern AudioAmplifier           monoSampleRight9; //xy=1266.9841537475586,3572.650724887848
extern AudioMixer4              voiceSubMixLeft3; //xy=1547.3808822631836,1736.4286136627197
extern AudioMixer4              voiceSubMixRight3; //xy=1551.666732788086,1804.3333148956299
extern AudioMixer4              voiceSubMixLeft7; //xy=1544.5237045288086,4451.0947914123535
extern AudioMixer4              voiceSubMixLeft5; //xy=1551.1902465820312,3072.7619614601135
extern AudioMixer4              voiceSubMixRight7; //xy=1548.809555053711,4518.999492645264
extern AudioMixer4              voiceSubMixRight5; //xy=1555.4760971069336,3140.6666626930237
extern AudioMixer4              voiceSubMixLeft2; //xy=1565.4758071899414,1175.2380847930908
extern AudioMixer4              voiceSubMixLeft4; //xy=1562.3809204101562,2366.190683364868
extern AudioMixer4              voiceSubMixLeft6; //xy=1559.5237503051758,3752.7618021965027
extern AudioMixer4              voiceSubMixRight2; //xy=1569.7616577148438,1243.142786026001
extern AudioMixer4              voiceSubMixRight4; //xy=1566.6667709350586,2434.0953845977783
extern AudioMixer4              voiceSubMixRight6; //xy=1563.8096008300781,3820.666503429413
extern AudioMixer4              voiceSubMixLeft1;      //xy=1583.4525337219238,512.7858734130859
extern AudioMixer4              voiceSubMixRight1;      //xy=1587.7383842468262,580.6905746459961
extern AudioMixer4              voiceMixLeft1;  //xy=2002.8574600219727,1486.428565979004
extern AudioMixer4              voiceMixRight1; //xy=2006.1668281555176,1553.23805809021
extern AudioMixer4              voiceMixLeft2; //xy=2004.2142715454102,3393.6905517578125
extern AudioMixer4              voiceMixRight2; //xy=2011.6902236938477,3460.4998817443848
extern AudioMixer4              mainMixerLeft;         //xy=2447.500347137451,2413.3336753845215
extern AudioMixer4              mainMixerRight;         //xy=2450.8332710266113,2485.0003452301025
extern AudioAnalyzePeak         peak_left;          //xy=2476.666847229004,2618.333688735962
extern AudioAnalyzePeak         peak_right; //xy=2480.0001831054688,2718.333366394043
extern AudioMixer4              inputMixerLeft;         //xy=2676.666534423828,2636.6666107177734
extern AudioMixer4              inputMixerRight; //xy=2678.3335189819336,2711.6668577194214
extern AudioMixer4              OutputMixerLeft; //xy=2936.666618347168,2585.000104904175
extern AudioMixer4              OutputMixerRight; //xy=2941.666778564453,2656.6668548583984
#ifdef BUILD_FOR_LINUX
extern AudioOutputSoundIO       i2s1;            //xy=3132.88089752197
extern AudioInputSoundIO        i2s2;           //xy=2470.000587463379,2666.666856765747
#else
AudioOutputI2S           i2s1;           //xy=3132.8808975219727,2627.500024795532
AudioInputI2S            i2s2;           //xy=2470.000587463379,2666.666856765747
#endif
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

extern AudioControlSGTL5000     sgtl5000_1;     //xy=3139.0950927734375,2579.4048891067505
// GUItool: end automatically generated code




#endif /* XRAudioConfig_h */