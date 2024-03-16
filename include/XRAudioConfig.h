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
//#include <synth_braids.h>
#ifdef BUILD_FOR_LINUX
#include <output_soundio.h>
#include <input_soundio.h>
#endif

// GUItool: begin automatically generated code
extern AudioSynthWaveformDc     monoSynthDc2; //xy=1018.333324432373,439.2857208251953
extern AudioSynthWaveformDc     monoSynthDc1;           //xy=1020.4999732971191,243.40486526489258
extern AudioSynthWaveformDc     monoSynthDc3; //xy=1020.333381652832,637.6190643310547
extern AudioSynthWaveformDc     monoSynthDc4; //xy=1020.333381652832,834.2856903076172
extern AudioSynthWaveform       monoSynthOscA2; //xy=1024.2857666015625,319.99998664855957
extern AudioSynthWaveform       monoSynthOscA4; //xy=1022.857177734375,717.1428833007812
extern AudioSynthWaveform       monoSynthOscB2; //xy=1024.285846710205,360.0000114440918
extern AudioSynthNoiseWhite     monoSynthNoise2; //xy=1026.3332633972168,400.00001430511475
extern AudioSynthWaveform       monoSynthOscB4; //xy=1025.7144241333008,755.7143154144287
extern AudioSynthWaveform       monoSynthOscA1;      //xy=1028.571388244629,124.28570699691772
extern AudioSynthWaveform       monoSynthOscA3; //xy=1027.1428756713867,520.0000171661377
extern AudioSynthWaveform       monoSynthOscB1; //xy=1028.5715255737305,164.28571128845215
extern AudioSynthNoiseWhite     monoSynthNoise1;        //xy=1028.499912261963,204.119158744812
extern AudioSynthWaveform       monoSynthOscB3; //xy=1027.142967224121,559.9999647140503
extern AudioSynthNoiseWhite     monoSynthNoise3; //xy=1028.3333206176758,598.3333578109741
extern AudioSynthNoiseWhite     monoSynthNoise4; //xy=1028.3333206176758,794.9999837875366
extern AudioMixer4              monoSynthMix2; //xy=1290.190444946289,338.1904878616333
extern AudioMixer4              monoSynthMix1;       //xy=1292.3570938110352,142.30963230133057
extern AudioMixer4              monoSynthMix3; //xy=1292.190502166748,536.5238313674927
extern AudioMixer4              monoSynthMix4; //xy=1292.190502166748,733.1904573440552
extern AudioEffectEnvelope      monoSynthFilterEnv2; //xy=1308.000015258789,393.14286708831787
extern AudioEffectEnvelope      monoSynthFilterEnv1;    //xy=1310.1666641235352,197.26201152801514
extern AudioEffectEnvelope      monoSynthFilterEnv3; //xy=1310.000072479248,591.4762105941772
extern AudioEffectEnvelope      monoSynthFilterEnv4; //xy=1310.000072479248,788.1428365707397
extern AudioAmplifier           monoSynthFilterAccent2; //xy=1317.833381652832,432.54754161834717
extern AudioAmplifier           monoSynthFilterAccent1; //xy=1320.0000305175781,236.66668605804443
extern AudioAmplifier           monoSynthFilterAccent3; //xy=1319.833438873291,630.8808851242065
extern AudioAmplifier           monoSynthFilterAccent4; //xy=1319.833438873291,827.547511100769
extern AudioFilterLadder        monoSynthFilter2; //xy=1570.4763641357422,390.8095645904541
extern AudioFilterLadder        monoSynthFilter1;      //xy=1572.6430130004883,194.92870903015137
extern AudioFilterLadder        monoSynthFilter3; //xy=1572.4764213562012,589.1429080963135
extern AudioFilterLadder        monoSynthFilter4; //xy=1572.4764213562012,785.809534072876
extern AudioPlayArrayResmp  monoSample4; //xy=1686.666561126709,2573.3335247039795
extern AudioPlayArrayResmp  monoSample3; //xy=1688.3332481384277,2413.3335247039795
extern AudioPlayArrayResmp  monoSample2; //xy=1689.9998741149902,2250.0002727508545
extern AudioPlayArrayResmp  monoSample1; //xy=1691.1905479431152,2085.0957374572754
extern AudioPlayArrayResmp  monoSample16; //xy=1702.1427192687988,4672.404405832291
extern AudioPlayArrayResmp  monoSample15; //xy=1703.8094062805176,4512.404405832291
extern AudioPlayArrayResmp  monoSample14; //xy=1705.47603225708,4349.071153879166
extern AudioPlayArrayResmp  monoSample13; //xy=1706.666706085205,4184.166618585587
extern AudioPlayArrayResmp  monoSample8; //xy=1710.475902557373,3265.7380497455597
extern AudioPlayArrayResmp  monoSample7; //xy=1712.1425895690918,3105.7380497455597
extern AudioPlayArrayResmp  monoSample12; //xy=1708.8094062805176,3954.071386575699
extern AudioPlayArrayResmp  monoSample6; //xy=1713.8092155456543,2942.4047977924347
extern AudioPlayArrayResmp  monoSample11; //xy=1710.4760932922363,3794.071386575699
extern AudioPlayArrayResmp  monoSample5; //xy=1714.9998893737793,2777.5002624988556
extern AudioPlayArrayResmp  monoSample10; //xy=1712.1427192687988,3630.738134622574
extern AudioPlayArrayResmp  monoSample9; //xy=1713.3333930969238,3465.8335993289948
#ifndef NO_DEXED
extern AudioSynthDexed          dexed3; //xy=1736.666763305664,1188.5714526176453
extern AudioSynthDexed          dexed2; //xy=1740.0000343322754,1061.190508365631
extern AudioSynthDexed          dexed1; //xy=1740.9009475708008,930.3614101409912
extern AudioSynthDexed          dexed4; //xy=1739.999912261963,1318.0953812599182
extern AudioSynthFMDrum         fmDrum3; //xy=1749.9999656677246,1775.0001382827759
extern AudioSynthFMDrum         fmDrum2; //xy=1754.999927520752,1631.6667580604553
extern AudioSynthFMDrum         fmDrum1; //xy=1759.8810081481934,1486.2304241657257
#endif
//extern AudioSynthBraids         braids1;        //xy=1759.9999542236328,1920.0003108978271
extern AudioAmplifier           monoSynthAmp2; //xy=1819.499885559082,415.88091564178467
extern AudioAmplifier           monoSynthAmp1; //xy=1821.6665344238281,220.00006008148193
extern AudioAmplifier           monoSynthAmp3; //xy=1821.499942779541,614.214259147644
extern AudioAmplifier           monoSynthAmp4; //xy=1821.499942779541,810.8808851242065
extern AudioEffectEnvelope      monoSynthAmpEnv2; //xy=1832.2378845214844,339.52383613586426
extern AudioEffectEnvelope      monoSynthAmpEnv1;          //xy=1834.4045333862305,143.64298057556152
extern AudioEffectEnvelope      monoSynthAmpEnv3; //xy=1834.2379417419434,537.8571796417236
extern AudioEffectEnvelope      monoSynthAmpEnv4; //xy=1834.2379417419434,734.5238056182861
extern AudioAmplifier           monoSynthAmpAccent2; //xy=1841.1666946411133,377.54758071899414
extern AudioAmplifier           monoSynthAmpAccent1; //xy=1843.3333435058594,181.6667251586914
extern AudioAmplifier           monoSynthAmpAccent3; //xy=1843.1667518615723,575.8809242248535
extern AudioAmplifier           monoSynthAmpAccent4; //xy=1843.1667518615723,772.547550201416
extern AudioAmplifier           dexedAmp3; //xy=1879.0991554260254,1230.8292059898376
extern AudioAmplifier           dexedAmp2; //xy=1882.4324264526367,1103.4482617378235
extern AudioAmplifier           dexedAmp1; //xy=1883.333339691162,972.6191635131836
extern AudioAmplifier           dexedAmp4; //xy=1882.4323043823242,1360.3531346321106
extern AudioEffectEnvelope      monoSampleAmpEnv4; //xy=1888.0949440002441,2573.476343154907
extern AudioEffectEnvelope      monoSampleAmpEnv3; //xy=1889.761631011963,2413.476343154907
extern AudioEffectEnvelope      monoSampleAmpEnv2; //xy=1891.4282569885254,2250.143091201782
extern AudioEffectEnvelope      monoSampleAmpEnv1; //xy=1892.6189308166504,2085.238555908203
extern AudioAmplifier           dexedAmpAccent3; //xy=1900.7659950256348,1187.4957900047302
extern AudioAmplifier           dexedAmpAccent2; //xy=1904.099266052246,1060.114845752716
extern AudioAmplifier           dexedAmpAccent1; //xy=1905.0001792907715,929.2857475280762
extern AudioAmplifier           monoSampleAmpAccent4; //xy=1898.8092613220215,2619.9045639038086
extern AudioAmplifier           dexedAmpAccent4; //xy=1904.0991439819336,1317.0197186470032
extern AudioAmplifier           monoSampleAmpAccent3; //xy=1900.4759483337402,2459.9045639038086
extern AudioAmplifier           monoSampleAmpAccent2; //xy=1902.1425743103027,2296.5713119506836
extern AudioAmplifier           monoSampleAmpAccent1; //xy=1903.3332481384277,2131.6667766571045
extern AudioAmplifier           monoSampleAmp4; //xy=1903.809238433838,2666.5714797973633
extern AudioAmplifier           monoSampleAmp3; //xy=1905.4759254455566,2506.5714797973633
extern AudioAmplifier           monoSampleAmp2; //xy=1907.1425514221191,2343.2382278442383
extern AudioAmplifier           monoSampleAmp1; //xy=1908.3332252502441,2178.333692550659
extern AudioEffectEnvelope      monoSampleAmpEnv16; //xy=1903.571102142334,4672.547224283218
extern AudioEffectEnvelope      monoSampleAmpEnv15; //xy=1905.2377891540527,4512.547224283218
extern AudioEffectEnvelope      monoSampleAmpEnv14; //xy=1906.9044151306152,4349.213972330093
extern AudioEffectEnvelope      monoSampleAmpEnv13; //xy=1908.0950889587402,4184.309437036514
extern AudioEffectEnvelope      monoSampleAmpEnv8; //xy=1911.9042854309082,3265.8808681964874
extern AudioEffectEnvelope      monoSampleAmpEnv7; //xy=1913.570972442627,3105.8808681964874
extern AudioEffectEnvelope      monoSampleAmpEnv12; //xy=1910.2377891540527,3954.2142050266266
extern AudioEffectEnvelope      monoSampleAmpEnv6; //xy=1915.2375984191895,2942.5476162433624
extern AudioEffectEnvelope      monoSampleAmpEnv11; //xy=1911.9044761657715,3794.2142050266266
extern AudioEffectEnvelope      monoSampleAmpEnv5; //xy=1916.4282722473145,2777.6430809497833
extern AudioEffectEnvelope      monoSampleAmpEnv10; //xy=1913.571102142334,3630.8809530735016
extern AudioEffectEnvelope      monoSampleAmpEnv9; //xy=1914.761775970459,3465.9764177799225
//extern AudioAmplifier           braidsAmpAccent1; //xy=1923.4523544311523,1920.4366493225098
//extern AudioAmplifier           braidsAmp1; //xy=1925.1191024780273,1968.7700233459473
extern AudioAmplifier           monoSampleAmpAccent16; //xy=1914.2854194641113,4718.97544503212
extern AudioAmplifier           monoSampleAmpAccent15; //xy=1915.95210647583,4558.97544503212
extern AudioAmplifier           monoSampleAmpAccent14; //xy=1917.6187324523926,4395.642193078995
extern AudioAmplifier           fmDrumAmpAccent3; //xy=1928.4523162841797,1775.4364805221558
extern AudioAmplifier           monoSampleAmpAccent13; //xy=1918.8094062805176,4230.737657785416
extern AudioAmplifier           monoSampleAmpAccent8; //xy=1922.6186027526855,3312.309088945389
extern AudioAmplifier           monoSampleAmpAccent7; //xy=1924.2852897644043,3152.309088945389
extern AudioAmplifier           monoSampleAmpAccent12; //xy=1920.95210647583,4000.642425775528
extern AudioAmplifier           fmDrumAmp3; //xy=1930.1190643310547,1823.7698545455933
extern AudioAmplifier           monoSampleAmpAccent6; //xy=1925.9519157409668,2988.975836992264
extern AudioAmplifier           monoSampleAmpAccent11; //xy=1922.6187934875488,3840.642425775528
extern AudioAmplifier           monoSampleAmp16; //xy=1919.2853965759277,4765.642360925674
extern AudioAmplifier           monoSampleAmpAccent5; //xy=1927.1425895690918,2824.0713016986847
extern AudioAmplifier           monoSampleAmpAccent10; //xy=1924.2854194641113,3677.309173822403
extern AudioAmplifier           monoSampleAmp15; //xy=1920.9520835876465,4605.642360925674
extern AudioAmplifier           monoSampleAmpAccent9; //xy=1925.4760932922363,3512.404638528824
extern AudioAmplifier           fmDrumAmpAccent2; //xy=1933.452278137207,1632.1031002998352
extern AudioAmplifier           monoSampleAmp14; //xy=1922.618709564209,4442.309108972549
extern AudioAmplifier           monoSampleAmp13; //xy=1923.809383392334,4277.40457367897
extern AudioAmplifier           monoSampleAmp8; //xy=1927.618579864502,3358.9760048389435
extern AudioAmplifier           fmDrumAmp2; //xy=1935.119026184082,1680.4364743232727
extern AudioAmplifier           monoSampleAmp7; //xy=1929.2852668762207,3198.9760048389435
extern AudioAmplifier           monoSampleAmp12; //xy=1925.9520835876465,4047.3093416690826
extern AudioAmplifier           monoSampleAmp6; //xy=1930.9518928527832,3035.6427528858185
extern AudioAmplifier           monoSampleAmp11; //xy=1927.6187705993652,3887.3093416690826
extern AudioAmplifier           monoSampleAmp5; //xy=1932.1425666809082,2870.7382175922394
extern AudioAmplifier           monoSampleAmp10; //xy=1929.2853965759277,3723.9760897159576
extern AudioAmplifier           fmDrumAmpAccent1; //xy=1938.3333587646484,1486.6667664051056
extern AudioAmplifier           monoSampleAmp9; //xy=1930.4760704040527,3559.0715544223785
extern AudioAmplifier           fmDrumAmp1; //xy=1940.0001068115234,1535.000140428543
//extern AudioAmplifier           braidsLeft1; //xy=2120.3093032836914,1950.4606304168701
extern AudioAmplifier           fmDrumLeft3; //xy=2125.3092651367188,1805.460461616516
//extern AudioAmplifier           braidsRight1; //xy=2125.2498168945312,1989.3296041488647
extern AudioAmplifier           dexedLeft3; //xy=2128.7659645080566,1214.3571009635925
extern AudioAmplifier           dexedLeft2; //xy=2132.099235534668,1086.9761567115784
extern AudioAmplifier           dexedLeft1; //xy=2133.0001487731934,956.1470584869385
extern AudioAmplifier           fmDrumLeft2; //xy=2130.309226989746,1662.1270813941956
extern AudioAmplifier           dexedLeft4; //xy=2132.0991134643555,1343.8810296058655
extern AudioAmplifier           fmDrumRight3; //xy=2130.2497787475586,1844.3294353485107
extern AudioAmplifier           dexedRight3; //xy=2134.02388381958,1251.7976508140564
extern AudioAmplifier           monoSynthLeft2; //xy=2137.6190071105957,375.09520530700684
extern AudioAmplifier           monoSynthLeft1;         //xy=2139.785655975342,179.2143497467041
extern AudioAmplifier           fmDrumLeft1; //xy=2135.1903076171875,1516.690747499466
extern AudioAmplifier           dexedRight2; //xy=2137.3571548461914,1124.4167065620422
extern AudioAmplifier           monoSynthLeft3; //xy=2139.6190643310547,573.4285488128662
extern AudioAmplifier           fmDrumRight2; //xy=2135.249740600586,1700.9960551261902
extern AudioAmplifier           monoSampleLeft4; //xy=2131.5711784362793,2643.8096470832825
extern AudioAmplifier           dexedRight1; //xy=2138.258068084717,993.5876083374023
extern AudioAmplifier           monoSynthLeft4; //xy=2139.6190643310547,770.0951747894287
extern AudioAmplifier           dexedRight4; //xy=2137.357032775879,1381.3215794563293
extern AudioAmplifier           monoSynthRight2; //xy=2141.3650131225586,412.50791454315186
extern AudioAmplifier           monoSampleLeft3; //xy=2133.237865447998,2483.8096470832825
extern AudioAmplifier           monoSampleLeft2; //xy=2134.9044914245605,2320.4763951301575
extern AudioAmplifier           monoSynthRight1;        //xy=2143.5316619873047,216.62705898284912
extern AudioAmplifier           monoSampleLeft1; //xy=2136.0951652526855,2155.5718598365784
extern AudioAmplifier           monoSynthRight3; //xy=2143.3650703430176,610.8412580490112
extern AudioAmplifier           monoSampleRight4; //xy=2135.3172721862793,2682.6509127616882
extern AudioAmplifier           fmDrumRight1; //xy=2140.1308212280273,1555.5597212314606
extern AudioAmplifier           monoSynthRight4; //xy=2143.3650703430176,807.5078840255737
extern AudioAmplifier           monoSampleRight3; //xy=2136.983959197998,2522.6509127616882
extern AudioAmplifier           monoSampleRight2; //xy=2138.6505851745605,2359.3176608085632
extern AudioAmplifier           monoSampleRight1; //xy=2139.8412590026855,2194.413125514984
extern AudioAmplifier           monoSampleLeft16; //xy=2147.047336578369,4742.880528211594
extern AudioAmplifier           monoSampleLeft15; //xy=2148.714023590088,4582.880528211594
extern AudioAmplifier           monoSampleLeft14; //xy=2150.3806495666504,4419.547276258469
extern AudioAmplifier           monoSampleLeft13; //xy=2151.5713233947754,4254.6427409648895
extern AudioAmplifier           monoSampleLeft8; //xy=2155.3805198669434,3336.2141721248627
extern AudioAmplifier           monoSampleLeft7; //xy=2157.047206878662,3176.2141721248627
extern AudioAmplifier           monoSampleLeft12; //xy=2153.714023590088,4024.547508955002
extern AudioAmplifier           monoSampleRight16; //xy=2150.793430328369,4781.721793889999
extern AudioAmplifier           monoSampleLeft6; //xy=2158.7138328552246,3012.8809201717377
extern AudioAmplifier           monoSampleLeft11; //xy=2155.3807106018066,3864.547508955002
extern AudioAmplifier           monoSampleRight15; //xy=2152.460117340088,4621.721793889999
extern AudioAmplifier           monoSampleLeft5;  //xy=2159.9045066833496,2847.9763848781586
extern AudioAmplifier           monoSampleLeft10; //xy=2157.047336578369,3701.214257001877
extern AudioAmplifier           monoSampleRight14; //xy=2154.1267433166504,4458.388541936874
extern AudioAmplifier           monoSampleLeft9; //xy=2158.238010406494,3536.3097217082977
extern AudioAmplifier           monoSampleRight13; //xy=2155.3174171447754,4293.484006643295
extern AudioAmplifier           monoSampleRight8;  //xy=2159.1266136169434,3375.0554378032684
extern AudioAmplifier           monoSampleRight7; //xy=2160.793300628662,3215.0554378032684
extern AudioAmplifier           monoSampleRight12; //xy=2157.460117340088,4063.3887746334076
extern AudioAmplifier           monoSampleRight6; //xy=2162.4599266052246,3051.7221858501434
extern AudioAmplifier           monoSampleRight11; //xy=2159.1268043518066,3903.3887746334076
extern AudioAmplifier           monoSampleRight5; //xy=2163.6506004333496,2886.8176505565643
extern AudioAmplifier           monoSampleRight10; //xy=2160.793430328369,3740.0555226802826
extern AudioAmplifier           monoSampleRight9; //xy=2161.984104156494,3575.1509873867035
//extern AudioAmplifier           braidsAmpDelaySend1; //xy=2427.142852783203,1620.000286102295
extern AudioMixer4              voiceSubMixLeft3; //xy=2432.3809356689453,1718.0954494476318
extern AudioAmplifier           fmDrumAmpDelaySend1; //xy=2434.2857208251953,1483.8097286224365
extern AudioAmplifier           fmDrumAmpDelaySend2; //xy=2434.2857818603516,1525.7144451141357
extern AudioAmplifier           fmDrumAmpDelaySend3; //xy=2434.2856826782227,1567.143084526062
extern AudioMixer4              voiceSubMixRight3; //xy=2436.6667861938477,1786.000150680542
extern AudioMixer4              voiceSubMixLeft4; //xy=2442.3808403015137,2371.1909132003784
extern AudioAmplifier           dexedAmpDelaySend4; //xy=2451.4285278320312,1047.380859375
extern AudioAmplifier           dexedAmpDelaySend1; //xy=2451.9048767089844,928.5713844299316
extern AudioAmplifier           dexedAmpDelaySend2; //xy=2451.904739379883,967.6189107894897
extern AudioAmplifier           dexedAmpDelaySend3; //xy=2452.38077545166,1007.8570938110352
extern AudioMixer4              voiceSubMixRight4; //xy=2446.666690826416,2439.0956144332886
extern AudioMixer4              voiceSubMixLeft7; //xy=2439.5237007141113,4463.59508395195
extern AudioMixer4              voiceSubMixLeft5; //xy=2446.190196990967,3075.262223958969
extern AudioMixer4              voiceSubMixLeft2; //xy=2455.475929260254,1167.380958557129
extern AudioMixer4              voiceSubMixRight7; //xy=2443.8095512390137,4531.49978518486
extern AudioMixer4              voiceSubMixRight5; //xy=2450.476047515869,3143.1669251918793
extern AudioAmplifier           monoSampleAmpDelaySend5; //xy=2452.8571968078613,2773.571592092514
extern AudioMixer4              voiceSubMixRight2; //xy=2459.7617797851562,1235.285659790039
extern AudioAmplifier           monoSampleAmpDelaySend1; //xy=2457.143077850342,2082.142946958542
extern AudioAmplifier           monoSampleAmpDelaySend6; //xy=2454.285671234131,2813.571526288986
extern AudioAmplifier           monoSampleAmpDelaySend7; //xy=2454.285671234131,2853.571526288986
extern AudioAmplifier           monoSampleAmpDelaySend2; //xy=2458.5715522766113,2122.142881155014
extern AudioAmplifier           monoSampleAmpDelaySend3; //xy=2458.5715522766113,2162.142881155014
extern AudioAmplifier           monoSampleAmpDelaySend8; //xy=2455.7141456604004,2893.5714604854584
extern AudioAmplifier           monoSampleAmpDelaySend4; //xy=2460.000026702881,2202.142815351486
extern AudioAmplifier           monoSampleAmpDelaySend13; //xy=2452.8575019836426,4167.857057332993
extern AudioMixer4              voiceSubMixLeft6; //xy=2454.5237007141113,3755.2620646953583
extern AudioAmplifier           monoSampleAmpDelaySend14;   //xy=2452.857246398926,4207.857127904892
extern AudioAmplifier           monoSampleAmpDelaySend15; //xy=2452.857246398926,4247.857127904892
extern AudioAmplifier           monoSynthAmpDelaySend1; //xy=2469.761474609375,160.47616577148438
extern AudioAmplifier           monoSynthAmpDelaySend2; //xy=2469.761672973633,199.52379417419434
extern AudioAmplifier           monoSampleAmpDelaySend16; //xy=2454.2857208251953,4287.857062101364
extern AudioAmplifier           monoSynthAmpDelaySend3; //xy=2471.190383911133,239.28570556640625
extern AudioAmplifier           monoSynthAmpDelaySend4; //xy=2471.1903533935547,280.2380561828613
extern AudioMixer4              voiceSubMixRight6; //xy=2458.8095512390137,3823.1667659282684
extern AudioMixer4              voiceSubMixLeft1;      //xy=2473.4525260925293,477.7859182357788
extern AudioMixer4              voiceSubMixRight1;      //xy=2477.7383766174316,545.690619468689
extern AudioAmplifier           monoSampleAmpDelaySend9; //xy=2472.8571548461914,3470.7144572734833
extern AudioAmplifier           monoSampleAmpDelaySend10; //xy=2477.142848968506,3509.285881757736
extern AudioAmplifier           monoSampleAmpDelaySend11; //xy=2477.142848968506,3549.285881757736
extern AudioAmplifier           monoSampleAmpDelaySend12; //xy=2478.5713233947754,3589.2858159542084
extern AudioMixer4              voiceMixLeft1;  //xy=2887.8574256896973,1344.7620639801025
extern AudioMixer4              voiceMixRight1; //xy=2891.166793823242,1411.5715560913086
extern AudioMixer4              voiceMixLeft2; //xy=2899.214282989502,3303.690707921982
extern AudioMixer4              voiceMixRight2; //xy=2906.6902351379395,3370.500037908554
extern AudioMixer4              delaySubMix1;         //xy=3249.333221435547,171.66693305969238
extern AudioMixer4              delaySubMix2; //xy=3251.666633605957,939.9999752044678
extern AudioMixer4              delaySubMix3; //xy=3288.333251953125,1603.333251953125
extern AudioMixer4              delaySubMix7; //xy=3286.6664085388184,4184.999744176865
extern AudioMixer4              delaySubMix6; //xy=3291.6664085388184,3486.6666276454926
extern AudioMixer4              delaySubMix5; //xy=3311.666492462158,2788.3334290981293
extern AudioMixer4              delaySubMix4; //xy=3314.999500274658,2094.999986410141
extern AudioMixer4              delayMix1; //xy=3726,1254
extern AudioMixer4              delayMix2; //xy=3751.666561126709,3095.000047683716
extern AudioInputI2S            i2s2;           //xy=4119.000617980957,2853.6670351028442
extern AudioAnalyzePeak         peak_left;          //xy=4125.666877746582,2805.333867073059
extern AudioAnalyzePeak         peak_right; //xy=4129.000213623047,2905.33354473114
extern AudioEffectDelay         delay1;         //xy=4132.97705078125,2173.4049224853516
extern AudioMixer4              mainMixerLeft;         //xy=4154.16682434082,2428.667158126831
//extern AudioEffectCompressor    compressorLeft1;    //xy=4156.66650390625,2626.666693687439
extern AudioMixer4              mainMixerRight;         //xy=4157.4997482299805,2500.333827972412
extern AudioAmplifier           delayLeftAmp1;           //xy=4161.428573608398,2265.7143201828003
extern AudioAmplifier           delayRightAmp1; //xy=4162.857276916504,2305.7140398025513
//extern AudioEffectCompressor    compressorRight1; //xy=4161.665855407715,2684.999780654907
extern AudioMixer4              delayFeedbackMixer1; //xy=4175.500556945801,2065.833236694336
extern AudioMixer4              inputMixerLeft;         //xy=4325.666564941406,2823.6667890548706
extern AudioMixer4              inputMixerRight; //xy=4327.333549499512,2898.6670360565186
extern AudioMixer4              OutputMixerLeft; //xy=4585.666648864746,2772.000283241272
extern AudioMixer4              OutputMixerRight; //xy=4590.666809082031,2843.6670331954956
extern AudioOutputI2S           i2s1;           //xy=4781.880928039551,2814.5002031326294

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