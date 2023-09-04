#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_TLC5947.h>
#include <Adafruit_MPR121.h>
#include <FastTouch.h>
#include <Keypad.h>
#include <uClock.h>
#include <U8g2lib.h>
#include <map>
#include <string>
#include <Audio.h>
#include <SD.h>
#include <SerialFlash.h>
#include <TeensyVariablePlayback.h>
#include "flashloader.h"
#include <ResponsiveAnalogRead.h>

/*
  Fontname: -aaron-bitocra7-Medium-R-Normal--7-60-75-75-C-40-ISO8859-1
  Copyright: Aaron Christianson 2011
  Glyphs: 160/192
  BBX Build Mode: 0
*/
const uint8_t bitocra7_c[1268] U8G2_FONT_SECTION("bitocra7_c") = 
  "\240\0\2\3\3\3\2\4\4\4\7\0\377\5\377\5\377\0\361\1\335\4\327 \5\200\316\0!\6\351\310"
  "L\1\42\7\223\313DR\0#\7\253\310D\255\0$\11\263\307E\65@E\0%\10\253\310\304P\66"
  "\24&\12\254\310I\61\24Ia\24'\6\322\313L\0(\7\362\307I\231\1)\10\362\307\204QJ\2"
  "*\7\243\311ES\6+\7\233\311ES\4,\6\322\307H\1-\5\213\312\14.\5\311\310\4/\11"
  "\263\307\246\302(\14\1\60\7\253\310P*\2\61\7\253\310\210Y\3\62\7\253\310\214\345\0\63\7\253\310"
  "\214\221H\64\10\253\310D\22\231\0\65\7\253\310\220#\1\66\7\253\310H!E\67\10\253\310\214Q\230"
  "\2\70\7\253\310I\26\1\71\7\253\310Pd$:\6\331\310D\1;\7\342\307\304\200\24<\11\253\310"
  "Fi \6\2=\7\233\311\314\300\0>\12\253\310\304@\14DI\0\77\12\263\307\214Q\30\203\21\0"
  "@\10\254\310\224Q\64\25A\7\253\310MR\25B\7\253\310H\22\3C\7\253\310\215\311\1D\10\253"
  "\310H\221j\2E\7\253\310\220R\70F\10\253\310\220R\30\2G\7\253\310\215\221DH\10\253\310D"
  "R%\5I\10\253\310Laj\0J\7\253\310\211\241DK\11\253\310D\322\24I\1L\7\253\310\204"
  "\231\3M\7\253\310\34\223\24N\7\253\310D\7\25O\7\253\310M*\2P\7\253\310Pe\10Q\10"
  "\263\307M\252Q\0R\10\253\310PS$\5S\7\253\310\215#\1T\10\253\310La\26\0U\7\253"
  "\310DZ\4V\10\253\310D\252)\4W\7\253\310D\322\61X\10\253\310DR\226\24Y\11\253\310D"
  "\22\25F\0Z\10\253\310\214Qr\0[\7\263\307TZ\3\134\12\263\307\204\61\20\306@\30]\7\263"
  "\307LZ\5^\5\223\313e_\5\214\307\20`\6\322\313H\1a\6\243\310\211\7b\7\253\310\204\224"
  "Dc\7\243\310\215\341\0d\7\253\310V\22\1e\6\243\310\234\3f\7\253\310\215d\10g\7\253\307"
  "P$\1h\10\253\310\204\224*\0i\10\263\310\305\220\230\32j\11\273\307\306\220\30J\4k\10\253\310"
  "\204\321$%l\7\253\310\210\231\2m\7\243\310\34S\0n\7\243\310P\252\0o\7\243\310P\22\1"
  "p\7\253\307Pe\10q\7\253\307Pd\2r\7\243\310\220I\0s\7\243\310\320@\1t\7\352\310"
  "DS$u\7\243\310D*\2v\10\243\310D\22\25\1w\7\243\310D\307\0x\10\243\310DYR"
  "\0y\7\253\307D\22Iz\7\243\310L\324\0{\10\263\307I\221\230\24|\5\351\310\24}\12\263\307"
  "\210\241\24F\22\0~\6\233\311\226\0\177\10\253\310\205Q$\21\300\11\263\310\304@\70U\1\301\10\263"
  "\310F\341T\5\302\10\263\310eMU\0\303\13\264\310ER(\245\246\24\0\304\11\263\310D\321$U"
  "\1\305\7\263\310\345U\5\306\10\254\310Q\251*\22\307\10\263\307\215\311I\0\310\11\263\310\304@T\205"
  "\3\311\10\263\310FQ\25\16\312\10\263\310\245\254p\0\313\10\263\310D\245\24\16\314\12\263\310\304@\64"
  "\205\321\0\315\11\263\310F\321\24F\3\316\10\263\310\245\250\60\32\317\10\263\310DT\230\32\320\11\254\310"
  "\211\321\224%\1\321\11\264\310ER\326*\5\322\11\263\310\304@DI\4\323\10\263\310F\21%\21\324"
  "\10\263\310\245*\211\0\325\12\264\310ER\64\345\232\0\326\10\263\310D\225\212\0\327\6\233\311D\71\330"
  "\10\273\307\36\321\21\2\331\11\263\310\304@\24\251\10\332\10\263\310F)\25\1\333\7\263\310\245\264\10\334"
  "\11\263\310D\61\20\251\10\335\11\263\310F)*\214\0\336\7\253\310\204T\11\337\11\263\307HR$\225"
  "\0\340\10\263\310\304@(\36\341\7\263\310F\241x\342\7\263\310e\211\7\343\13\264\310ER(\3\321"
  "\12\0\344\10\263\310D\61$\36\345\7\263\310\345\6\16\346\7\244\310\215\25\1\347\7\253\307\215\341$\350"
  "\10\263\310\304@t\16\351\7\263\310F\321\71\352\10\263\310\245\216p\0\353\10\263\310D\61p\16\354\11"
  "\263\310\304@$\246\6\355\10\263\310F\221\230\32\356\10\263\310\245\306\324\0\357\11\263\310D\61 \246\6"
  "\360\12\264\310\305\220RJM\0\361\12\264\310ER\64\345\13\0\362\11\263\310\304@DI\4\363\10\263"
  "\310F\21%\21\364\10\263\310\245*\211\0\365\12\264\310ER\64\345\232\0\366\11\263\310D\61@I\4"
  "\367\11\253\310\305\320\14E\0\370\7\263\307\36'\0\371\11\263\310\304@\24\251\10\372\10\263\310F)\25"
  "\1\373\10\263\310e\3\221D\374\11\263\310D\61\20\251\10\375\10\273\307F)\211$\376\10\263\307\204\341"
  "\31\2\377\12\273\307D\61\20I$\1\0\0\0\4\377\377\0";

/*
  Fontname: -aaron-bitocra13full-Medium-R-Normal--13-130-84-84-C-90-ISO8859-1
  Copyright: OLF 1.1 Aaron Christianson, 2011
  Glyphs: 189/277
  BBX Build Mode: 0
*/
const uint8_t bitocra13_c[1988] U8G2_FONT_SECTION("bitocra13_c") = 
  "\275\0\3\3\3\4\3\5\4\7\15\0\376\10\376\11\377\1@\2\245\7\247 \5\0\356\7!\7A\303"
  "GE\0\42\10\233\332GD%\0#\12-\306OJ%\245\222\2$\14=\302W\310\22*EL!"
  "\0%\12\65\302\207h\42\322\62\22&\20F\302\317(\34\214\304B\21Y$\26\232\4'\7\32\333\17"
  "\5\0(\11\323\276W$)o\11)\12\323\276G,)/I\0*\13\65\306WJ\312h\222)\4"
  "+\12-\306W\60T\12\206\0,\7\243\272\307(\3-\6\15\316G\1.\6\22\303\7\1/\16U"
  "\276gZ\60\26\214\5c\301 \0\60\14E\302\207M%\42\211\310d\6\61\11\304\302\207,/)\5"
  "\62\11E\302G\61xL,\63\11E\302G\61\205\230h\64\17E\302G\60\24\11EB\221Z\60\5"
  "\0\65\11E\302\207\61XL\64\66\12E\302\307(\230h\223\31\67\13E\302G\61M\244\26\214\1\70"
  "\13E\302\17%\24\11\335\324\14\71\11E\302\207Mf\314\64:\7\62\303\7\221\0;\11\303\272\207D"
  "\16\30e<\10=\302\237H\243\6=\7\34\306\7\221\0>\11=\302\207P\223\66\0\77\16U\276\207"
  "M\26\14\215\202qP\60\4@\16N\302\307Q\30\215VD\21Q\344\0A\11E\302OM\333M\26"
  "B\14E\302\7%\24\11Elj\6C\10E\302O\61\217\5D\21F\302G)\26\211Eb\221X"
  "$\26\211Y\0E\12E\302\207\61H\11&\26F\12E\302\207\61H\11f\4G\11E\302O\61\323"
  "LfH\12E\302GL\333M-\0I\11E\302G)\230\247\2J\11E\302\327\60G\231\1K\12"
  "E\302GL[%\246\26L\10E\302G\60\37\13M\14E\302Glr\210Dd\332\2N\15E\302"
  "GL\66\242HH\63Y\0O\10E\302\207Mo\6P\17F\302\207%\26\211Eb\21R\64\21\0"
  "Q\13M\276OM\333H\22)\12R\21F\302\207%\26\211Eb\21R,\22\213\304\2S\11E\302"
  "O\61\261\30\64T\12E\302\207%)\230'\0U\10E\302GL\337\14V\14E\302GL\323D\62"
  "\222\5\1W\13E\302GL\227\310!\62\13X\14E\302GL-\62\232\304\324\2Y\14E\302GL"
  "m\42\231\5\223\0Z\12E\302G\61&\322\26,[\24U\276\207%\24\11EB\221P$\24\11E"
  "B\221P\1\134\16U\276G\60\32\214\6\243\301h\60\0]\24U\276G)\22\212\204\42\241H(\22"
  "\212\204\42\241\210\1^\11%\322Wl\42\231\5_\6\16\302\207\1`\7\232\332GD\22a\10\65\302"
  "\17\361&\63b\12M\302G\60\321\246\315\0c\10\65\302O\61c\1d\11M\302g\342M\233\1e"
  "\10\65\302\207Mv,f\12\314\302\17-\255\226\15\0g\11E\272\207M\315\30\64h\12M\302G\60"
  "\321\246[\0i\11\303\302OP\224\313\0j\12T\272_X\226\233\250\0k\13M\302G\60M\255\22"
  "S\13l\10\313\302\207(\77\11m\15\65\302\207%\42\211H\42\62Y\0n\10\65\302\207M\267\0o"
  "\10\65\302\207M\233\1p\11E\272\207M\355\230\10q\11E\272\207M\315\230\1r\11\65\302\207M\230"
  "\21\0s\10\65\302O\221\32\64t\12\314\302G,\255\226\251\0u\10\65\302GL\67\3v\13\65\302"
  "GLm\42\231\205\0w\14\65\302GL%\42\211H\42\6x\14\65\302GL\26\31Mb\262\0y"
  "\11E\272GL\233\61hz\11\65\302G\61\244\24,{\13T\276\227$\226$\313Q\0|\7Q\277"
  "\307a\0}\14T\276\207\60\226&\212e\21\1~\11\35\312\307D\22\221\14\241\7A\303\207\244\0\242"
  "\17E\302_\250\24I\212\204\42\61J\14\0\243\13E\302\327$\230F\12\306\12\244\13\66\302G\60B"
  "\312\211\22\14\245\16E\302Gl\42\231\305f\261Y\10\0\246\7Q\277\307a\0\247\16F\302\27%:"
  "\212\205b\241i\204\4\250\7\214\346G(\0\251\20\77\306\327,%$\221\304D\222\264\330\10\0\252\7"
  ",\316\317\314T\253\10\35\316OH\224\1\254\7\35\302G\61\1\256\16\77\306\327,%\62Q\222\344\26"
  "\33\1\257\6\214\336\7\1\260\7\244\326GIT\261\13=\302W\60T\12\306\1\5\262\7,\322\7\315"
  "F\263\10,\322\7-\62+\264\7\22\343\217\4\0\265\15E\272GLmr\210D\204A\0\266\20M"
  "\276\317\205%\42\11EB\221P$\24\11\267\6\22\313\7\1\270\7\233\302O\214\0\271\11\64\316\207,"
  "KJ\1\272\12\244\326\217$$\212H\0\273\11\35\316G(I\24\1\274\16V\302\207\64\343(\26\211"
  "Eb\323\4\275\15W\302\207\70\353\34@\216\321\302\4\276\17V\302\7\65\66\15Qb\221X$\66M"
  "\277\16U\276W\60\16\12\206F\301\230\314\0\300\13U\302O\64V\323v\223\5\301\12U\302_ZM"
  "\333M\26\302\15]\302W,\22\7\324\264\335d\1\303\14]\302W$;\240\246\355&\13\304\13U\302"
  "O:\240\246\355&\13\305\15]\302W,\22\213\325\264\335d\1\306\20G\302\317)\26\212\205&\241\30"
  "-\24\13\21\307\13U\272O\61\217\265\230\4\0\310\14U\302O\64d\14R\202\211\5\311\13U\302_"
  "\222\61H\11&\26\312\15]\302W,\22\66\6)\301\304\2\313\13U\302O\262\61H\11&\26\314\13"
  "U\302O\64T\12\346\251\0\315\12U\302_R)\230\247\2\316\13]\302W,\22.\5\363T\317\12"
  "U\302Or)\230\247\2\320\17F\302G)\26\211U\322\42\261H\314\2\321\16U\302W$\67\331\210"
  "\42!\315d\1\322\12U\302O\64d\323\233\1\323\11U\302_\222Mo\6\324\13]\302W,\22\266"
  "\351\315\0\325\12]\302W$\263Mo\6\326\11U\302O\262Mo\6\327\7\233\316G$\7\330\15U"
  "\276\347i%\42\211\214FG\0\331\12U\302O\64\24\323\67\3\332\11U\302_RL\337\14\333\13]"
  "\302W,\22\216\351\233\1\334\11U\302OrL\337\14\335\15U\302_RLm\42\231\5\223\0\336\13"
  "M\302G\60hS;\6\1\337\20]\272\317$\24\11EB\221\230N\303 \0\340\13M\302W\64\16"
  " \336d\6\341\12M\302_:\204x\223\31\342\14M\302W,\22\7\20o\62\3\343\13M\302W$"
  ";\200x\223\31\344\12E\302O:\200x\223\31\345\15U\302\227(%\24\22\21o\62\3\346\13\67\302"
  "\217-T\22\225b\7\347\12E\272O\61c-&\1\350\12E\302O\64d\223\35\13\351\11E\302_"
  "\222Mv,\352\13M\302W,\22\266\311\216\5\353\11E\302O\262Mv,\354\12\313\302G,(\312"
  "e\0\355\12\313\302W$(\312e\0\356\11\313\302O\66Q.\3\357\12\303\302G$&\312e\0\360"
  "\13M\302WT$\15\336d\6\361\12M\302W$\263M\267\0\362\11E\302O\64d\323f\363\11E"
  "\302_\222M\233\1\364\12M\302W,\22\266i\63\365\12M\302W$\263M\233\1\366\11E\302O\262"
  "M\233\1\367\12-\306W\34P\7\204\0\370\12E\276\347ib\31\35\1\371\12E\302O\64\24\323\315"
  "\0\372\11E\302_RL\67\3\373\12M\302W,\22\216\351f\374\11E\302OrL\67\3\375\12U"
  "\272_RL\233\61h\376\13M\272G\60hS;\6\1\377\12U\272OrL\233\61h\0\0\0\4"
  "\377\377\0";

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
AudioMixer4              mainMixerLeft;         //xy=2425.833595275879,2078.3334159851074
AudioMixer4              mainMixerRight;         //xy=2429.1665992736816,2153.3335819244385
AudioOutputI2S           i2s1;           //xy=2602.880558013916,2109.166506767273

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
AudioConnection          patchCord189(mainMixerLeft, 0, i2s1, 0);
AudioConnection          patchCord190(mainMixerRight, 0, i2s1, 1);

AudioControlSGTL5000     sgtl5000_1;     //xy=2609.095317840576,2062.7380781173706
// GUItool: end automatically generated code

unsigned long lastSamplePlayed = 0;

newdigate::audiosample *samples[16];

#define sd1 SD
#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  43  // not actually used
#define SDCARD_SCK_PIN   45  // not actually used

#define DAC_SCK  27
#define DAC_MOSI 26
#define CS1 28
#define CS2 29
#define CS3 30
#define CS4 31

const byte chan_a_low_gain = 0b00000000;
const byte chan_b_low_gain = 0b10000000;
const byte chan_a_high_gain = 0b00010000;
const byte chan_b_high_gain = 0b10010000;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 14, /* reset=*/ 15);

// How many boards do you have chained?
#define NUM_TLC5947 1

#define tlc5947_data 5
#define tlc5947_clock 6
#define tlc5947_latch 4
#define tlc5947_oe 3  // set to -1 to not use the enable pin (its optional)

Adafruit_TLC5947 tlc = Adafruit_TLC5947(NUM_TLC5947, tlc5947_clock, tlc5947_data, tlc5947_latch);

const byte ROWS = 6;
const byte COLS = 6;

char keys[ROWS][COLS] = {
  {'a','b','c','d','e','f'},
  {'g','h','i','j','k','l'},
  {'m','n','o','p','q','r'},
  {'s','t','u','v','w','x'},
  {'y','z','1','2','3','4'},
  {'5','6','7','8','9','0'},
};

byte rowPins[ROWS] = {38, 37, 36, 35, 34, 33}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {2, 9, 12, 41, 40, 39}; //connect to the column pinouts of the keypad

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

const char* keyCharsMap[ROWS][COLS] = {
  {"1","2","3","4","PLAY","FN"},
  {"5","6","7","8","STOP","COPY"},
  {"9","10","11","12","UP","SOUND"},
  {"13","14","15","16","DOWN","TEMPO"},
  {"BANK","PATTERN","TRACK","DATA","MODA","MODB"},
  {"MODC","MODD","SEL","ESC","NA","NA"},
};

std::map<char, const char*> charCharsMap = {
  { 'a', "BANK" },
  { 'b', "PATTERN" },
  { 'c', "TRACK" },
  { 'd', "MAIN" },
  { 'e', "MODA" },
  { 'f', "MODB" },
  { 'g', "MODC" },
  { 'h', "MODD" },
  { 'i', "SEL" },
  { 'j', "ESC" },
  { 'k', "STOP" },
  { 'l', "COPY" },
  { 'm', "1" },
  { 'n', "2" },
  { 'o', "3" },
  { 'p', "4" },
  { 'q', "START" },
  { 'r', "FN" },
  { 's', "5" },
  { 't', "6" },
  { 'u', "7" },
  { 'v', "8" },
  { 'w', "STOP" },
  { 'x', "COPY" },
  { 'y', "9" },
  { 'z', "10" },
  { '0', "SOUND" },
  { '1', "11" },
  { '2', "12" },
  { '3', "DOWN" },
  { '4', "TEMPO" },
  { '5', "13" },
  { '6', "14" },
  { '7', "15" },
  { '8', "16" },
  { '9', "UP" },
};

std::map<int8_t, char> stepCharMap = {
  { 1, 'm' },
  { 2, 'n' },
  { 3, 'o' },
  { 4, 'p' },
  { 5, 's' },
  { 6, 't' },
  { 7, 'u' },
  { 8, 'v' },
  { 9, 'y' },
  { 10, 'z' },
  { 11, '1' },
  { 12, '2' },
  { 13, '5' },
  { 14, '6' },
  { 15, '7' },
  { 16, '8' },
};

std::map<char, int8_t> charStepMap = {
  {'m', 1},
  {'n', 2},
  {'o', 3},
  {'p', 4},
  {'s', 5},
  {'t', 6},
  {'u', 7},
  {'v', 8},
  {'y', 9},
  {'z', 10},
  {'1', 11},
  {'2', 12},
  {'5', 13},
  {'6', 14},
  {'7', 15},
  {'8', 16},
};

std::map<char, uint8_t> charLEDMap = {
  { 'm', 0 },
  { 'n', 1 },
  { 'o', 2 },
  { 'p', 3 },
  { 'q', 23 },
  { 's', 4 },
  { 't', 5 },
  { 'u', 6 },
  { 'v', 7 },
  { 'w', 22 },
  { 'y', 9 },
  { 'z', 10 },
  { '1', 11 },
  { '2', 12 },
  { '5', 13 },
  { '6', 14 },
  { '7', 15 },
  { '8', 16 },
};

PROGMEM int8_t stepLEDPins[16] = {
  0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15, 16
};

PROGMEM int encoder_addrs[5] = {
  0x36, 0x37, 0x38, 0x39, 0x40
};

int16_t encoder_currValues[5] = {
  0, 0, 0, 0, 0
};

int16_t encoder_lastValues[5] = {
  0, 0, 0, 0, 0
};

uint8_t bpm_blink_timer = 1;

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

bool fastBtnPressed = false;

uint8_t note_on_keyboard = 0;
bool playing_note_on_keyboard = false;

float noteToFreqArr[13] = {
  16.35, 17.32, 18.35, 19.45, 20.60, 21.83, 23.12, 24.50, 25.96, 27.50, 29.14, 30.87, 32.70
};

#define MAX_USABLE_SAMPLE_IDS 256
#define MAX_USABLE_WAV_SAMPLE_IDS 512
#define MAX_SAMPLE_NAME_LENGTH 32
char usableSampleNames[MAX_USABLE_SAMPLE_IDS][MAX_SAMPLE_NAME_LENGTH];
char usableWavSampleNames[MAX_USABLE_SAMPLE_IDS][MAX_SAMPLE_NAME_LENGTH];

ResponsiveAnalogRead analog(A8, true); // headphone pot

float hp_vol_cur = 0.5;

// Sequencer data

#define MAXIMUM_SEQUENCER_STEPS 64
#define MAXIMUM_SEQUENCER_TRACKS 16
#define MAXIMUM_SEQUENCER_PATTERNS 16
#define MAXIMUM_SEQUENCER_BANKS 1

#define DEFAULT_LAST_STEP 16

enum WAVEFORM_TYPE {
  SAW = WAVEFORM_SAWTOOTH,
  RSAW = WAVEFORM_SAWTOOTH_REVERSE,
  TRI = WAVEFORM_TRIANGLE,
  SQUARE = WAVEFORM_SQUARE,
  PULSE = WAVEFORM_PULSE,
  SINE = WAVEFORM_SINE,
};

enum TRACK_TYPE {
  MIDI = 0,
  CV_TRIG = 1,
  CV_GATE = 2,
  SUBTRACTIVE_SYNTH = 3,
  RAW_SAMPLE = 4,
  WAV_SAMPLE = 5,
};

enum TRACK_STEP_STATE {
  OFF = 0,
  ON = 1,
  ACCENTED = 2
};

typedef struct
{
  TRACK_STEP_STATE state = OFF;
  uint8_t note = 0; // 0 - C
  uint8_t octave = 4; // 4 - middle C (C4)
  uint8_t length = 4; // 4 = 1/16
  uint8_t velocity = 50; // 1 - 100%
  uint8_t probability = 0; // 1 = 100ms
  int8_t microtiming = 0; // 1 = 100ms
} TRACK_STEP;

typedef struct
{
  TRACK_TYPE track_type = RAW_SAMPLE;
  TRACK_STEP steps[MAXIMUM_SEQUENCER_STEPS];
  uint16_t samplerate = 44100;
  uint8_t sample_dir = 0;
  uint8_t raw_sample_id = 0;
  uint8_t wav_sample_id = 0;
  uint8_t waveform = WAVEFORM_TYPE::SAW;
  uint8_t last_step = DEFAULT_LAST_STEP;
  uint8_t note = 0; // 0 - C1
  uint8_t octave = 4; // 4 - middle C (C4)
  int8_t detune = -7;
  int8_t fine = 0;
  int8_t microtiming = 0; // 1 = 100ms
  uint8_t length = 4; // 1 = 1/64 step len
  uint8_t bitrate = 16;
  uint8_t velocity = 50; // 1 - 100%
  uint8_t probability = 100;
  uint8_t channel = 1;
  uint8_t looptype = 0;
  uint32_t loopstart = 0;
  uint32_t loopfinish = 3000;
  play_start playstart = play_start_sample;
  float level = 0.7; // TODO: impl real default level based on default mixer settings
  float pan = 0; // -1.0 = panned fully left, 1.0 = panned fully right
  float sample_play_rate = 1.0;
  float width = 0.5; // pulsewidth / waveshaping
  float oscalevel = 1;
  float oscblevel = 0.5;
  float cutoff = 1600;
  float res = 0;
  float filter_attack = 0;
  float filter_decay = 1000;
  float filter_sustain = 1.0;
  float filter_release = 5000;
  float filterenvamt = 1.0;
  float amp_attack = 0;
  float amp_decay = 500;
  float amp_sustain = 1.0;
  float amp_release = 500;
  float noise = 0;
  bool chromatic_enabled = false;
} TRACK;

typedef struct
{
  TRACK tracks[MAXIMUM_SEQUENCER_TRACKS];
  uint8_t last_step = DEFAULT_LAST_STEP;
} PATTERN;

typedef struct
{
  PATTERN patterns[MAXIMUM_SEQUENCER_PATTERNS];
} BANK;

typedef struct
{
  BANK banks[MAXIMUM_SEQUENCER_BANKS];
} SEQUENCER_EXTERNAL;

FLASHMEM SEQUENCER_EXTERNAL _seq_external;

typedef struct
{
  PATTERN pattern;
} SEQUENCER_HEAP;

SEQUENCER_HEAP _seq_heap;

enum SEQUENCER_PLAYBACK_STATE {
  STOPPED = 0,
  RUNNING = 1,
  PAUSED = 2
};

typedef struct
{
  SEQUENCER_PLAYBACK_STATE playback_state = STOPPED;
  int8_t current_step = 1;
} SEQUENCER_STATE;

SEQUENCER_STATE _seq_state;

typedef struct
{
  int8_t trackNum = -1;
  int8_t stepNum = -1;
  int8_t length = -1; // need -1 to know if active/not active 
  // int8_t microtiming;
} STACK_STEP_DATA;

// Represents the maximum allowed steps triggered in a running pattern,
// this is tracked by the sequencer to know when to trigger noteOn/noteOff messages, etc
// 1 pattern * 16 tracks * 64 step max = 1024 possible steps while the sequencer is running
#define STEP_STACK_SIZE 1024
STACK_STEP_DATA _step_stack[STEP_STACK_SIZE];

const int numChannels = 1; // 1 for mono, 2 for stereo...

enum UI_MODE {
  BANK_WRITE,
  BANK_SEL,
  PATTERN_WRITE, // The default mode
  PATTERN_SEL,
  TRACK_WRITE,
  TRACK_SEL,
  SET_TEMPO,
  SUBMITTING_STEP_VALUE
};

UI_MODE previous_UI_mode = PATTERN_WRITE; // the default mode
UI_MODE current_UI_mode = PATTERN_WRITE; // the default mode

int8_t current_selected_bank = 0; // default to 0 (first)
int8_t current_selected_pattern = 0; // default to 0 (first)
int8_t current_selected_track = 0; // default to 0 (first)
int8_t current_selected_step = -1; // default to -1 (none)

#define FUNCTION_BTN_CHAR 'r'
#define SOUND_SETUP_BTN_CHAR '0'

bool function_started = false;
bool track_sel_btn_held = false;

int8_t patt_held_for_selection = -1;
int8_t track_held_for_selection = -1; // default to -1 (none)

const uint8_t backwardsNoteNumbers[13] = {
  12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
};

int keyboardNotesHeld = 0;

int current_page_selected = 0;

int oscWaveforms[6] = {
  WAVEFORM_SAWTOOTH,
  WAVEFORM_SAWTOOTH_REVERSE,
  WAVEFORM_TRIANGLE,
  WAVEFORM_SQUARE,
  WAVEFORM_PULSE,
  WAVEFORM_SINE
};

std::map<int, int> waveformSelMap = {
  {0, WAVEFORM_SAWTOOTH},
  {1, WAVEFORM_SAWTOOTH_REVERSE},
  {2, WAVEFORM_TRIANGLE},
  {3, WAVEFORM_SQUARE},
  {4, WAVEFORM_PULSE},
  {5, WAVEFORM_SINE},
};

std::map<int, int> waveformFindMap = {
  {WAVEFORM_SAWTOOTH, 0},
  {WAVEFORM_SAWTOOTH_REVERSE, 1},
  {WAVEFORM_TRIANGLE, 2},
  {WAVEFORM_SQUARE, 3},
  {WAVEFORM_PULSE, 4},
  {WAVEFORM_SINE, 5},
};

std::map<int, loop_type> loopTypeSelMap = {
  {0, looptype_none},
  {1, looptype_repeat},
  {2, looptype_repeat}, // used for chromatic repeat
};

std::map<loop_type, int> loopTypeFindMap = {
  {looptype_none, 0},
  {looptype_repeat, 1},
};

std::map<int, play_start> playStartSelMap = {
  {0, play_start_sample},
  {1, play_start_loop},
};

std::map<play_start, int> playStartFindMap = {
  {play_start_sample, 0},
  {play_start_loop, 1},
};

const uint8_t *small_font = bitocra7_c; // u8g2_font_trixel_square_tf

elapsedMillis elapsed;

void setLEDPWM(uint8_t lednum, uint16_t pwm);
void setLEDPWMDouble(uint8_t lednum1, uint16_t pwm1, uint8_t lednum2, uint16_t pwm2);
void u8g2_prepare(void);\
void handleSwitchStates(bool discard);
void handleKeyboardStates(void);
void handleEncoderStates(void);
void encoder_set(int addr, int16_t rmin, int16_t rmax, int16_t rstep, int16_t rval, uint8_t rloop);
void encoder_setValue(int addr, int16_t rval);
int16_t encoder_getValue(int addr);
void initEncoders(void);
void writeToDAC(int chip, int chan, int val);
void initDACs(void);
const char* getKeyStr(char idx);
u_int8_t getKeyStepNum(char idx);
uint8_t getKeyLED(char idx);
void toggleSequencerPlayback(char btn);
void handle_bpm_step(uint32_t tick);
void triggerAllStepsForGlobalStep(uint32_t tick);
void initMain(void);
void drawSequencerScreen(void);
void drawSetTempoOverlay(void);
void setDisplayStateForAllStepLEDs(void);
void setDisplayStateForPatternActiveTracksLEDs(bool enable);
bool btnCharIsATrack(char btnChar);
void toggleSelectedStep(uint8_t step);
void clearAllStepLEDs(void);
void displayCurrentlySelectedPattern(void);
void displayCurrentlySelectedTrack(void);
void changeTrackSoundType(uint8_t track, TRACK_TYPE newType);
void initTrackSounds(void);
void initUsableSampleNames(void);
void initUsableWavSampleNames(void);
void triggerTrackManually(uint8_t t, uint8_t note);
std::string getTrackTypeNameStr(TRACK_TYPE type);
void handleHeadphoneAdjustment(void);
void handleEncoderSetTempo();
void handleAddToStepStack(uint32_t tick, int track, int step);
void noteOffForAllSounds(void);

TRACK getHeapTrack(int track);
TRACK_STEP getHeapStep(int track, int step);
PATTERN getHeapCurrentSelectedPattern(void);
TRACK getHeapCurrentSelectedTrack(void);
TRACK_STEP getHeapCurrentSelectedTrackStep(void);

TRACK getHeapTrack(int track)
{
  return _seq_heap.pattern.tracks[track];
}

TRACK_STEP getHeapStep(int track, int step)
{
  return _seq_heap.pattern.tracks[track].steps[step];
}

PATTERN getHeapCurrentSelectedPattern()
{
  return _seq_heap.pattern;
}

TRACK getHeapCurrentSelectedTrack(void)
{
  return _seq_heap.pattern.tracks[current_selected_track];
}

TRACK_STEP getHeapCurrentSelectedTrackStep(void)
{
  return _seq_heap.pattern.tracks[current_selected_track].steps[current_selected_step];
}

void swapSequencerMemoryForPattern(int newBank, int newPattern)
{
  PATTERN currPatternData = _seq_heap.pattern;
  PATTERN newPatternData = _seq_external.banks[newBank].patterns[newPattern];

  // swap memory data
  _seq_external.banks[current_selected_bank].patterns[current_selected_pattern] = currPatternData;
  _seq_heap.pattern = newPatternData;

  // update currently selected vars
  current_selected_bank = newBank;
  current_selected_pattern = newPattern;
}

typedef struct
{
  float left;
  float right;
} PANNED_AMOUNTS;

PANNED_AMOUNTS getStereoPanValues(float pan);
PANNED_AMOUNTS getStereoPanValues(float pan)
{
  PANNED_AMOUNTS amounts;

  amounts.left = 1.0;
  if (pan < 0) {
    amounts.left += pan;
  }

  amounts.right = 1.0;
  if (pan > 0) {
    amounts.right -= pan;
  }

  return amounts;
}

std::string getTrackTypeNameStr(TRACK_TYPE type)
{
  std::string str = "MIDI"; // default

  switch (type)
  {
  case TRACK_TYPE::CV_GATE :
    str = "CV/GATE";

    break;
  
  case TRACK_TYPE::CV_TRIG :
    str = "CV/TRIG";
    
    break;
  
  case TRACK_TYPE::RAW_SAMPLE :
    str = "MONO SAMPLE";
    
    break;
  
  case TRACK_TYPE::WAV_SAMPLE :
    str = "STEREO SAMPLE";
    
    break;
  
  case TRACK_TYPE::SUBTRACTIVE_SYNTH :
    str = "2-OSC SUBTRACTIVE";
    
    break;
  
  default:
    break;
  }

  return str;
}

// Internal clock handlers
void ClockOut96PPQN(uint32_t tick) {
  // Send MIDI_CLOCK to external gears
  //usbMIDI.sendRealTime(usbMIDI.Clock);

  handle_bpm_step(tick);
}

void onClockStart() {
  //usbMIDI.sendRealTime(usbMIDI.Start);
}

void onClockStop() {
  //usbMIDI.sendRealTime(usbMIDI.Stop);

  noteOffForAllSounds();
}

int8_t keyboardOctave = 4; // range 1-7 ?

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

#define COMBO_VOICE_COUNT 4

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

#define SAMPLE_VOICE_COUNT 12

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

void noteOffForAllSounds(void)
{
  for (int t = 0; t < 4; t++)
  {
    TRACK currTrack = getHeapTrack(t);
  
    if (currTrack.track_type == SUBTRACTIVE_SYNTH) {
      comboVoices[t].ampEnv.noteOff();
      comboVoices[t].filterEnv.noteOff();
    } else {
      comboVoices[t].ampEnv.noteOff();
    }
  }

  for (int s = 0; s < 12; s++)
  {
    sampleVoices[s].ampEnv.noteOff();
  }
}

void initTrackSounds()
{
  // configure combo voice audio objects
  for (int v = 0; v < COMBO_VOICE_COUNT; v++) {    
    // TODO: eventually need to restore all sounds for all patterns and their tracks?
    TRACK currTrack = getHeapTrack(v);

    // init mono RAW sample
    comboVoices[v].rSample.setPlaybackRate(currTrack.sample_play_rate);
    comboVoices[v].rSample.enableInterpolation(true);

    // init synth
    comboVoices[v].osca.begin(currTrack.waveform);
    comboVoices[v].osca.amplitude(currTrack.oscalevel);
    comboVoices[v].osca.frequency(261.63); // C4 TODO: use find freq LUT with track note
    comboVoices[v].osca.pulseWidth(currTrack.width);
    comboVoices[v].oscb.begin(currTrack.waveform);
    comboVoices[v].oscb.amplitude(currTrack.oscblevel);
    comboVoices[v].oscb.frequency(261.63); // C3 TODO: use find freq LUT with track note + detune
    comboVoices[v].oscb.pulseWidth(currTrack.width);
    comboVoices[v].noise.amplitude(currTrack.noise);
    comboVoices[v].oscMix.gain(0, 0.33);
    comboVoices[v].oscMix.gain(1, 0.33);
    comboVoices[v].oscMix.gain(2, 0.33);
    comboVoices[v].dc.amplitude(1);
    comboVoices[v].lfilter.frequency(currTrack.cutoff);
    comboVoices[v].lfilter.resonance(currTrack.res);
    comboVoices[v].lfilter.octaveControl(4);
    comboVoices[v].filterEnv.attack(currTrack.filter_attack);
    comboVoices[v].filterEnv.decay(currTrack.filter_decay);
    comboVoices[v].filterEnv.sustain(currTrack.filter_sustain);
    comboVoices[v].filterEnv.release(currTrack.filter_release);
    //comboVoices[v].filterEnv.releaseNoteOn(15);
    comboVoices[v].ampEnv.attack(currTrack.amp_attack * (currTrack.velocity * 0.01));
    comboVoices[v].ampEnv.decay(currTrack.amp_decay * (currTrack.velocity * 0.01));
    comboVoices[v].ampEnv.sustain(currTrack.amp_sustain * (currTrack.velocity * 0.01));
    comboVoices[v].ampEnv.release(currTrack.amp_release * (currTrack.velocity * 0.01));
    //comboVoices[v].ampEnv.releaseNoteOn(15);

    // output
    comboVoices[v].mix.gain(0, 1); // raw sample
    comboVoices[v].mix.gain(1, 1); // synth

    // mono to L&R
    comboVoices[v].leftCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));
    comboVoices[v].rightCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));

    // Sub L&R mixers
    comboVoices[v].leftSubMix.gain(0, currTrack.level); // wav sample left
    comboVoices[v].leftSubMix.gain(1, currTrack.level); // raw sample / synth left
    comboVoices[v].rightSubMix.gain(0, currTrack.level); // wav sample right
    comboVoices[v].rightSubMix.gain(1, currTrack.level); // raw sample / synth right
  }

  // configure sample voice audio objects
  for (int v = 0; v < SAMPLE_VOICE_COUNT; v++) {
    // TODO: eventually need to restore all sounds for all patterns and their tracks?
    TRACK currTrack = getHeapTrack(v);

    // init mono RAW sample
    sampleVoices[v].rSample.setPlaybackRate(currTrack.sample_play_rate);
    sampleVoices[v].rSample.enableInterpolation(true);

    sampleVoices[v].ampEnv.attack(currTrack.amp_attack * (currTrack.velocity * 0.01));
    sampleVoices[v].ampEnv.decay(currTrack.amp_decay * (currTrack.velocity * 0.01));
    sampleVoices[v].ampEnv.sustain(currTrack.amp_sustain * (currTrack.velocity * 0.01));
    sampleVoices[v].ampEnv.release(currTrack.amp_release * (currTrack.velocity * 0.01));
    //sampleVoices[v].ampEnv.releaseNoteOn(15);

    // mono to L&R
    sampleVoices[v].leftCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));
    sampleVoices[v].rightCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));

    // Sub L&R mixers
    sampleVoices[v].leftSubMix.gain(0, currTrack.level); // wav sample left
    sampleVoices[v].leftSubMix.gain(1, currTrack.level); // raw sample / synth left
    sampleVoices[v].rightSubMix.gain(0, currTrack.level); // wav sample right
    sampleVoices[v].rightSubMix.gain(1, currTrack.level); // raw sample / synth right
  }

  mixerLeft1.gain(0, 1);
  mixerRight1.gain(0, 1);
  mixerLeft1.gain(1, 1);
  mixerRight1.gain(1, 1);
  mixerLeft1.gain(2, 1);
  mixerRight1.gain(2, 1);
  mixerLeft1.gain(3, 1);
  mixerRight1.gain(3, 1);

  mixerLeft2.gain(0, 1);
  mixerRight2.gain(0, 1);
  mixerLeft2.gain(1, 1);
  mixerRight2.gain(1, 1);
  mixerLeft2.gain(2, 1);
  mixerRight2.gain(2, 1);
  mixerLeft2.gain(3, 1);
  mixerRight2.gain(3, 1);

  mixerLeft3.gain(0, 1);
  mixerRight3.gain(0, 1);
  mixerLeft3.gain(1, 1);
  mixerRight3.gain(1, 1);
  mixerLeft3.gain(2, 1);
  mixerRight3.gain(2, 1);
  mixerLeft3.gain(3, 1);
  mixerRight3.gain(3, 1);

  mixerLeft4.gain(0, 1);
  mixerRight4.gain(0, 1);
  mixerLeft4.gain(1, 1);
  mixerRight4.gain(1, 1);
  mixerLeft4.gain(2, 1);
  mixerRight4.gain(2, 1);
  mixerLeft4.gain(3, 1);
  mixerRight4.gain(3, 1);

  // Main L&R output mixer
  mainMixerLeft.gain(0, 1);
  mainMixerRight.gain(0, 1);
  mainMixerLeft.gain(1, 1);
  mainMixerRight.gain(1, 1);
  mainMixerLeft.gain(2, 1);
  mainMixerRight.gain(2, 1);
  mainMixerLeft.gain(3, 1);
  mainMixerRight.gain(3, 1);
}

void changeSampleTrackSoundType(uint8_t t, TRACK_TYPE newType);
void changeSampleTrackSoundType(uint8_t t, TRACK_TYPE newType)
{
  TRACK currTrack = getHeapTrack(t);
  TRACK_TYPE currType = currTrack.track_type;

  if (currType == newType) return;

  if (currType == WAV_SAMPLE) {
    sampleVoices[t-4].wSample.disposeBuffer();
  }

  if (newType == RAW_SAMPLE) {
    _seq_heap.pattern.tracks[t].track_type = RAW_SAMPLE;
  } else if (newType == WAV_SAMPLE) {
    // only create buffers for stereo samples when needed
    sampleVoices[t-4].wSample.createBuffer(2048, AudioBuffer::inExt);

    _seq_heap.pattern.tracks[t].track_type = WAV_SAMPLE;
  } else if (newType == MIDI) {
    _seq_heap.pattern.tracks[t].track_type = MIDI;
  } else if (newType == CV_GATE) {
    _seq_heap.pattern.tracks[t].track_type = CV_GATE;
  }else if (newType == CV_TRIG) {
    _seq_heap.pattern.tracks[t].track_type = CV_TRIG;
  }
}

void changeTrackSoundType(uint8_t t, TRACK_TYPE newType)
{
  if (t > 3) {
    changeSampleTrackSoundType(t, newType);
    return;
  }

  ComboVoice trackVoice = comboVoices[t];
  TRACK currTrack = getHeapTrack(t);
  TRACK_TYPE currType = currTrack.track_type;

  if (currType == newType) return;

  if (currType == WAV_SAMPLE) {
    trackVoice.wSample.disposeBuffer();
  }

  if (newType == RAW_SAMPLE) {
   _seq_heap.pattern.tracks[t].track_type = RAW_SAMPLE;

    // turn sample volume all the way up
    trackVoice.mix.gain(0, 1);
    // turn synth volume all the way down
    trackVoice.mix.gain(1, 0); // synth
  } else if (newType == WAV_SAMPLE) {
   _seq_heap.pattern.tracks[t].track_type = WAV_SAMPLE;

    // only create buffers for stereo samples when needed
    trackVoice.wSample.createBuffer(2048, AudioBuffer::inExt);

    // turn sample volume all the way up
    trackVoice.mix.gain(0, 1);
    // turn synth volumes all the way down
    trackVoice.mix.gain(1, 0); // synth
  } else if (newType == SUBTRACTIVE_SYNTH) {
   _seq_heap.pattern.tracks[t].track_type = SUBTRACTIVE_SYNTH;

    TRACK currTrack = getHeapTrack(t);

    // turn sample volume all the way down
    trackVoice.mix.gain(0, 0);
    // turn synth volumes all the way up
    trackVoice.mix.gain(1, 1); // ladder

    // TESTING: revert amp env to normal synth setting
    trackVoice.ampEnv.attack(currTrack.amp_attack);
    trackVoice.ampEnv.decay(currTrack.amp_decay);
    trackVoice.ampEnv.sustain(currTrack.amp_sustain);
    trackVoice.ampEnv.release(currTrack.amp_release);
  } else if (newType == MIDI) {
   _seq_heap.pattern.tracks[t].track_type = MIDI;

    // turn all audio for this track voice down
    trackVoice.mix.gain(0, 0); // mono sample
    trackVoice.mix.gain(1, 0); // ladder
  } else if (newType == CV_GATE) {
   _seq_heap.pattern.tracks[t].track_type = CV_GATE;

    // turn all audio for this track voice down
    trackVoice.mix.gain(0, 0); // mono sample
    trackVoice.mix.gain(1, 0); // ladder
  }else if (newType == CV_TRIG) {
   _seq_heap.pattern.tracks[t].track_type = CV_TRIG;

    // turn all audio for this track voice down
    trackVoice.mix.gain(0, 0); // mono sample
    trackVoice.mix.gain(1, 0); // ladder
  }
}

void initUsableSampleNames(void)
{
  // TODO: implement a proper project sample pool so that
  // sample IDs adhere to their respective sample names
  // when loading project data from the SD card
  for (int n=0; n<MAX_USABLE_SAMPLE_IDS; n++) {
    strcpy(usableSampleNames[n], "N/A");
  }
}

void initUsableWavSampleNames(void)
{
  // TODO: implement a proper project sample pool so that
  // sample IDs adhere to their respective sample names
  // when loading project data from the SD card
  for (int n=0; n<MAX_USABLE_SAMPLE_IDS; n++) {
    strcpy(usableWavSampleNames[n], "N/A");
  }
}

bool isWavFile(const char* filename);
bool isWavFile(const char* filename) {
  int8_t len = strlen(filename);

  Serial.print("strlen: ");
  Serial.println(len);

  bool result = false;
  String fStr = filename;

  if (fStr.toLowerCase().substring((len - 4), len) == ".wav") {
    result = true;
  }

  return result;
}

uint16_t wavSamplesAvailable = 0;

void parseRootForWavSamples(void);
void parseRootForWavSamples(void)
{
  File dir = SD.open("/");

  uint8_t currentFileIndex = 0;

  while(true) {
    File entry = dir.openNextFile();
    if (! entry) {
      Serial.println("** no files found **");
      break;
    }

    if (!entry.isDirectory()) {
      if (strlen(entry.name()) > MAX_SAMPLE_NAME_LENGTH) {
        Serial.print("Sample names can only be 32 characters long, not loading: ");
        Serial.println(entry.name());
        continue;
      }

      if ((currentFileIndex + 1) > MAX_USABLE_WAV_SAMPLE_IDS) {
        Serial.println("Reached the 512 WAV sample limit!");
        break;
      }

      if (isWavFile(entry.name())) {
        // files have sizes, directories do not
        strcpy(usableWavSampleNames[currentFileIndex], entry.name());

        ++wavSamplesAvailable;
        ++currentFileIndex;
      }
    }
    
    entry.close();
  }
}

bool isRawFile(const char* filename);
bool isRawFile(const char* filename) {
  int8_t len = strlen(filename);

  Serial.print("strlen: ");
  Serial.println(len);

  bool result = false;
  String fStr = filename;

  if (fStr.toLowerCase().substring((len - 4), len) == ".raw") {
    result = true;
  }

  return result;
}

uint16_t rawSamplesAvailable = 0;

void parseRootForRawSamples(void);
void parseRootForRawSamples(void)
{
  File dir = SD.open("/");

  uint32_t totalSizeLimit = 8000000; // 8 MB
  uint32_t totalSizeCollected = 0;
  uint8_t currentFileIndex = 0;

  while(true) {
    File entry = dir.openNextFile();
    if (! entry) {
      Serial.println("** no files found **");
      break;
    }

    if (!entry.isDirectory()) {
      if (strlen(entry.name()) > MAX_SAMPLE_NAME_LENGTH) {
        Serial.print("Sample names can only be 32 characters long, not loading: ");
        Serial.println(entry.name());
        continue;
      }

      if (isRawFile(entry.name())) {
        // files have sizes, directories do not
        totalSizeCollected += entry.size();
        if (totalSizeCollected >= totalSizeLimit) {
          Serial.println("size limit reached, not loading any more samples!");
          break;
        }

        if ((currentFileIndex + 1) > MAX_USABLE_SAMPLE_IDS) {
          Serial.println("Reached the 255 RAW sample limit!");
          break;
        }

        strcpy(usableSampleNames[currentFileIndex], entry.name());

        ++rawSamplesAvailable;
        ++currentFileIndex;
      }
    }
    
    entry.close();
  }
}

void loadRawSamplesFromSdCard(void);
void loadRawSamplesFromSdCard(void)
{  
  parseRootForRawSamples();

  newdigate::flashloader loader;

  char naChar[32];
  strcpy(naChar, "N/A");
  
  // only load 16 samples in PSRAM at a time, for now
  for (int s = 0; s < 16; s++) {
    if (usableSampleNames[s] != naChar) {
      samples[s] = loader.loadSample(usableSampleNames[s]);
    }
  }
}

void setup() {
  Serial.begin(9600);

  if (CrashReport) {
    Serial.print(CrashReport);
  }

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(50);

  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  // pullup resistors
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.8);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  initTrackSounds();

  // prepare sample names to be occupied
  initUsableSampleNames();

  // IMPORTANT: DO THIS AFTER SD IS INITIALIZED ABOVE
  // load short project mono samples into PSRAM
  loadRawSamplesFromSdCard();

  parseRootForWavSamples();
  
  delay(25);

  Wire1.begin();

  SPI1.begin();
  SPI1.setMOSI(DAC_MOSI);
  SPI1.setSCK(DAC_SCK);

  pinMode(CS1, OUTPUT); // CS
  digitalWrite(CS1, HIGH);
  pinMode(CS2, OUTPUT); // CS
  digitalWrite(CS2, HIGH);
  pinMode(CS3, OUTPUT); // CS
  digitalWrite(CS3, HIGH);
  pinMode(CS4, OUTPUT); // CS
  digitalWrite(CS4, HIGH);

  initDACs();

  tlc.begin();
  if (tlc5947_oe >= 0) {
    pinMode(tlc5947_oe, OUTPUT);
    digitalWrite(tlc5947_oe, LOW);
  }

  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5B, &Wire2)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }

  // init display
  u8g2.begin();

  // initialize encoders
  initEncoders();

  kpd.setHoldTime(150);

  for (int i=0; i<25; i++) {
    setLEDPWM(i, 0); // sets all 24 outputs to no brightness
  }

  delay(100);

  // discard any dirty reads
  handleSwitchStates(true);

  u8g2_prepare();

  initMain();

  // Setup our clock system
  // Inits the clock
  uClock.init();
  // Set the callback function for the clock output to send MIDI Sync message.
  uClock.setClock96PPQNOutput(ClockOut96PPQN);
  // Set the callback function for MIDI Start and Stop messages.
  uClock.setOnClockStartOutput(onClockStart);  
  uClock.setOnClockStopOutput(onClockStop);
  // Set the clock BPM to 120 BPM
  uClock.setTempo(120);
  //uClock.shuffle();

  Serial.println("Now fill sequencer in-heap with some example data");

  _seq_heap.pattern.tracks[0].track_type = TRACK_TYPE::SUBTRACTIVE_SYNTH;
  _seq_heap.pattern.tracks[0].waveform = SAW;
  _seq_heap.pattern.tracks[0].raw_sample_id = 0;
  _seq_heap.pattern.tracks[0].wav_sample_id = 0;
  _seq_heap.pattern.tracks[0].steps[0].state = TRACK_STEP_STATE::ON;
  _seq_heap.pattern.tracks[0].steps[0].note = 0;
  _seq_heap.pattern.tracks[0].steps[0].octave = 4;
  _seq_heap.pattern.tracks[0].steps[0].length = 4;
  _seq_heap.pattern.tracks[0].steps[1].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[0].steps[2].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[0].steps[3].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[0].steps[4].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[0].steps[5].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[0].steps[6].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[0].steps[7].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[0].steps[8].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[0].steps[9].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[0].steps[10].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[0].steps[11].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[0].steps[12].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[0].steps[13].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[0].steps[14].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[0].steps[15].state = TRACK_STEP_STATE::OFF;

  _seq_heap.pattern.tracks[1].track_type = TRACK_TYPE::SUBTRACTIVE_SYNTH;
  _seq_heap.pattern.tracks[1].waveform = SAW;
  _seq_heap.pattern.tracks[1].raw_sample_id = 0;
  _seq_heap.pattern.tracks[1].wav_sample_id = 0;
  _seq_heap.pattern.tracks[1].steps[0].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[1].steps[1].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[1].steps[2].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[1].steps[3].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[1].steps[4].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[1].steps[5].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[1].steps[6].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[1].steps[7].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[1].steps[8].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[1].steps[9].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[1].steps[10].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[1].steps[11].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[1].steps[12].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[1].steps[13].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[1].steps[14].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[1].steps[15].state = TRACK_STEP_STATE::OFF;

  _seq_heap.pattern.tracks[2].track_type = TRACK_TYPE::SUBTRACTIVE_SYNTH;
  _seq_heap.pattern.tracks[2].waveform = SAW;
  _seq_heap.pattern.tracks[2].raw_sample_id = 0;
  _seq_heap.pattern.tracks[2].wav_sample_id = 0;
  _seq_heap.pattern.tracks[2].steps[0].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[2].steps[1].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[2].steps[2].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[2].steps[3].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[2].steps[4].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[2].steps[5].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[2].steps[6].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[2].steps[7].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[2].steps[8].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[2].steps[9].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[2].steps[10].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[2].steps[11].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[2].steps[12].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[2].steps[13].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[2].steps[14].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[2].steps[15].state = TRACK_STEP_STATE::OFF;

  _seq_heap.pattern.tracks[3].track_type = TRACK_TYPE::SUBTRACTIVE_SYNTH;
  _seq_heap.pattern.tracks[3].waveform = SAW;
  _seq_heap.pattern.tracks[3].raw_sample_id = 0;
  _seq_heap.pattern.tracks[3].wav_sample_id = 0;
  _seq_heap.pattern.tracks[3].steps[0].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[3].steps[1].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[3].steps[2].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[3].steps[3].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[3].steps[4].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[3].steps[5].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[3].steps[6].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[3].steps[7].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[3].steps[8].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[3].steps[9].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[3].steps[10].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[3].steps[11].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[3].steps[12].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[3].steps[13].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[3].steps[14].state = TRACK_STEP_STATE::OFF;
  _seq_heap.pattern.tracks[3].steps[15].state = TRACK_STEP_STATE::OFF;

  current_selected_pattern = 0;
  current_selected_track = 0;

  Serial.println("Done filling test sequencer out");

  Serial.print("sizeof sequencer heap: ");
  Serial.print(sizeof(_seq_heap));

  Serial.print(" sizeof sequencer ext: ");
  Serial.print(sizeof(_seq_external));

  Serial.print(" sizeof step stack: ");
  Serial.print(sizeof(_step_stack));

  Serial.print(" comboVoices: ");
  Serial.println(sizeof(comboVoices));

  Serial.print(" sampleVoices: ");
  Serial.println(sizeof(sampleVoices));

  if (current_UI_mode == TRACK_WRITE) {
    setDisplayStateForAllStepLEDs();
  }
}

void loop(void)
{
  analog.update();
  handleHeadphoneAdjustment();

  // handle hardware input states
  handleSwitchStates(false);
  handleKeyboardStates();
  handleEncoderStates();
}

void initMain()
{
  u8g2.clearBuffer();

  u8g2.setFont(bitocra13_c); // u8g2_font_8x13_mr
  u8g2.drawStr( 48, 24, "xr-1");

  u8g2.setFont(small_font); // u8g2_font_6x10_tf
  u8g2.setFontRefHeightExtendedText();

  u8g2.sendBuffer();

  delay(1000);

  drawSequencerScreen();
}

std::string strldz(std::string inputStr, const int zeroNum);
std::string strldz(std::string inputStr, const int zeroNum)
{
  std::string outputStr;

  unsigned int number_of_zeros = zeroNum - inputStr.length();

  outputStr.insert(0, number_of_zeros, '0');
  outputStr.append(inputStr);

  return outputStr;
}

void drawMenuHeader(std::string inputStr, int8_t value, int8_t startX);
void drawMenuHeader(std::string inputStr, int8_t value, int8_t startX)
{
  inputStr += strldz(std::to_string(value), 2);
  u8g2.drawStr(startX, 0, inputStr.c_str());
}

std::string getTrackMetaStr(TRACK_TYPE type);
std::string getTrackMetaStr(TRACK_TYPE type)
{
  std::string outputStr;

  switch (type)
  {
  case SUBTRACTIVE_SYNTH:
    outputStr = "SYNTH";
    break;
  
  case RAW_SAMPLE:
    outputStr = "RSAMPL";
    break;
  
  case WAV_SAMPLE:
    outputStr = "WSAMPL";
    break;
  
  case MIDI:
    outputStr = "MIDI";
    break;
  
  case CV_GATE:
    outputStr = "CV/GAT";
    break;
  
  case CV_TRIG:
    outputStr = "CV/TRG";
    break;
  
  default:
    break;
  }

  return outputStr;
}

std::string getWaveformName(uint8_t waveform);
std::string getWaveformName(uint8_t waveform)
{
  std::string outputStr;

  switch (waveform)
  {
  case WAVEFORM_SAWTOOTH:
    outputStr = "SAW";
    break;
  
  case WAVEFORM_SAWTOOTH_REVERSE:
    outputStr = "RSAW";
    break;
  
  case WAVEFORM_TRIANGLE:
    outputStr = "TRI";
    break;
  
  case WAVEFORM_SQUARE:
    outputStr = "SQR";
    break;
  
  case WAVEFORM_PULSE:
    outputStr = "PUL";
    break;
  
  case WAVEFORM_SINE:
    outputStr = "SIN";
    break;
  
  default:
    break;
  }

  return outputStr;
}

std::map<uint8_t, std::string> baseNoteToStr = {
  { 0, "C" },
  { 1, "C#" },
  { 2, "D" },
  { 3, "D#" },
  { 4, "E" },
  { 5, "F" },
  { 6, "F#" },
  { 7, "G" },
  { 8, "G#" },
  { 9, "A" },
  { 10, "A#" },
  { 11, "B" },
  { 12, "C" },
};

std::string getDisplayNote(void);
std::string getDisplayNote(void)
{
  std::string outputStr;

  if (current_UI_mode == SUBMITTING_STEP_VALUE) {
    //TRACK_STEP currTrackStep = _seq_state.seq.banks[current_selected_bank].patterns[current_selected_pattern].tracks[current_selected_track].steps[current_selected_step];
    TRACK_STEP currTrackStep = getHeapCurrentSelectedTrackStep();
    outputStr += baseNoteToStr[currTrackStep.note];
    outputStr += std::to_string(currTrackStep.octave);
  } else {
    TRACK currTrack = getHeapCurrentSelectedTrack();
    outputStr += baseNoteToStr[currTrack.note];
    outputStr += std::to_string(currTrack.octave);
  }

  return outputStr;
}

std::string getPercentageStr(float rate);
std::string getPercentageStr(float rate)
{
  std::string outputStr;

  outputStr += std::to_string((float)round(rate * 100) / 100);
  outputStr += "%";
  
  return outputStr;
}

std::string getPlaybackSpeedStr(float rate);
std::string getPlaybackSpeedStr(float rate)
{
  std::string rateStr = std::to_string((float)round(rate * 100) / 100);

  int strLen = (int)rate >= 10 ? 4 : 3;
  
  std::string outputStr = rateStr.substr(0,strLen);

  bool t = rate < 0.1;
  if (t) {
    outputStr = "REV";
  }
  
  return outputStr;
}

std::map<TRACK_TYPE, int> trackPageNumMap = {
  { SUBTRACTIVE_SYNTH, 6},
  { RAW_SAMPLE, 4},
  { WAV_SAMPLE, 1},
  { MIDI, 1},
  { CV_GATE, 1},
  { CV_TRIG, 1},
};

std::map<TRACK_TYPE, std::map<int, std::string>> trackCurrPageNameMap = {
  { SUBTRACTIVE_SYNTH, {
    {0, "MAIN"},
    {1, "OSC"},
    {2, "FILTER"},
    {3, "FILTER ENV"},
    {4, "AMP ENV"},
    {5, "OUTPUT"},
  }},
  { RAW_SAMPLE, {
    {0, "MAIN"},
    {1, "LOOP"},
    {2, "AMP ENV"},
    {3, "OUTPUT"},
  }},
  { WAV_SAMPLE, {
    {0, "MAIN"},
  }},
  { MIDI, {
    {0, "MAIN"},
  }},
  { CV_GATE, {
    {0, "MAIN"},
  }},
  { CV_TRIG, {
    {0, "MAIN"},
  }},
};

std::string getCurrPageNameForTrack(void);
std::string getCurrPageNameForTrack(void)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();
  
  std::string outputStr = trackCurrPageNameMap[currTrack.track_type][current_page_selected];

  return outputStr;
}

std::string getLoopTypeName(void);
std::string getLoopTypeName(void)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  std::string outputStr;

  if (loopTypeSelMap[currTrack.looptype] == loop_type::looptype_none) {
    outputStr += "OFF";
  } else if (loopTypeSelMap[currTrack.looptype] == loop_type::looptype_repeat) {
    if (currTrack.chromatic_enabled) {
      outputStr += "CHR";
    } else {
    outputStr += "REP";
    }
  }

  return outputStr;
}

typedef struct
{
  std::string aName;
  std::string bName;
  std::string cName;
  std::string dName;
  std::string aValue;
  std::string bValue;
  std::string cValue;
  std::string dValue;
} SOUND_CONTROL_MODS;

SOUND_CONTROL_MODS getSubtractiveSynthControlModData();
SOUND_CONTROL_MODS getSubtractiveSynthControlModData()
{
  SOUND_CONTROL_MODS mods;

  TRACK track = getHeapCurrentSelectedTrack();

  switch (current_page_selected)
  {
  case 0: // MAIN
    mods.aName = "LEN";
    mods.bName = "VEL";
    mods.cName = "uTM";
    mods.dName = "PRB";

    if (current_UI_mode == SUBMITTING_STEP_VALUE) {
      TRACK_STEP step = getHeapCurrentSelectedTrackStep();

      mods.aValue = std::to_string(step.length); // TODO: use 1/16 etc display
    } else {
      mods.aValue = std::to_string(track.length); // TODO: use 1/16 etc display
    }

    if (current_UI_mode == SUBMITTING_STEP_VALUE) {
      TRACK_STEP step = getHeapCurrentSelectedTrackStep();

      mods.bValue = std::to_string(step.velocity); // TODO: use 1/16 etc display
    } else {
      mods.bValue = std::to_string(track.velocity); // TODO: use 1/16 etc display
    }

    mods.cValue = "100%"; // TODO: impl
    mods.dValue = "100%"; // TODO: impl
    break;
  
  case 1: // OSC
    mods.aName = "WAV";
    mods.bName = "DET";
    mods.cName = "FIN";
    mods.dName = "WID";

    mods.aValue = getWaveformName(track.waveform);
    mods.bValue = std::to_string(track.detune);
    mods.cValue = std::to_string(track.fine);
    mods.dValue = std::to_string(track.width);
    break;
  
  case 2: // FILTER
    mods.aName = "NOI";
    mods.bName = "FRQ";
    mods.cName = "RES";
    mods.dName = "AMT";

    mods.aValue = std::to_string((float)round(track.noise * 100) / 100);
    mods.aValue = mods.aValue.substr(0,3);

    mods.bValue = std::to_string(track.cutoff);
    mods.bValue = mods.bValue.substr(0,5);

    // mods.cValue = std::to_string((float)round(track.res * 100) / 100);
    // mods.cValue = mods.cValue.substr(0,3);

    mods.cValue = std::to_string(track.res);

    mods.dValue = std::to_string((float)round(track.filterenvamt * 100) / 100);
    mods.dValue = mods.dValue.substr(0,3);
    break;
  
  case 3: // FILTER ENV
    mods.aName = "ATT";
    mods.bName = "DEC";
    mods.cName = "SUS";
    mods.dName = "REL";

    mods.aValue = std::to_string((float)round(track.filter_attack * 100) / 100);
    mods.aValue = mods.aValue.substr(0,3);

    mods.bValue = std::to_string((float)round(track.filter_decay * 100) / 100);
    mods.bValue = mods.bValue.substr(0,5);

    mods.cValue = std::to_string((float)round(track.filter_sustain * 100) / 100);
    mods.cValue = mods.cValue.substr(0,3);

    mods.dValue = std::to_string((float)round(track.filter_release * 100) / 100);
    mods.dValue = mods.dValue.substr(0,5);
    break;
  
  case 4: // AMP ENV
    mods.aName = "ATT";
    mods.bName = "DEC";
    mods.cName = "SUS";
    mods.dName = "REL";

    mods.aValue = std::to_string((float)round(track.amp_attack * 100) / 100);
    mods.aValue = mods.aValue.substr(0,3);

    mods.bValue = std::to_string((float)round(track.amp_decay * 100) / 100);
    mods.bValue = mods.bValue.substr(0,5);

    mods.cValue = std::to_string((float)round(track.amp_sustain * 100) / 100);
    mods.cValue = mods.cValue.substr(0,3);

    mods.dValue = std::to_string((float)round(track.amp_release * 100) / 100);
    mods.dValue = mods.dValue.substr(0,5);
    break;
  
  case 5: // OUTPUT
    mods.aName = "LVL";
    mods.bName = "PAN";
    mods.cName = "--"; // fx send?
    mods.dName = "--"; // fx return?


    mods.aValue = std::to_string((float)round(track.level * 100) / 100);
    mods.aValue = mods.aValue.substr(0,3);

    mods.bValue = std::to_string((float)round(track.pan * 100) / 100);
    mods.bValue = mods.bValue.substr(0,3);

    mods.cValue = "--";
    mods.dValue = "--";
    break;
  
  default:
    break;
  }

  return mods;
}

SOUND_CONTROL_MODS getRawSampleControlModData();
SOUND_CONTROL_MODS getRawSampleControlModData()
{
  SOUND_CONTROL_MODS mods;

  TRACK track = getHeapCurrentSelectedTrack();

  switch (current_page_selected)
  {
  case 0: // MAIN
    mods.aName = "FIL";
    mods.bName = "SPD";
    mods.cName = "--";
    mods.dName = "--";

    mods.aValue = std::to_string(track.raw_sample_id+1);
    mods.bValue = getPlaybackSpeedStr(track.sample_play_rate);
    mods.cValue = "--";
    mods.dValue = "--";
    break;
  
  case 1: // LOOPING
    mods.aName = "TYP";
    mods.bName = "LST";
    mods.cName = "LFI";
    mods.dName = "PST";

    mods.aValue = getLoopTypeName();
    mods.bValue = std::to_string(track.loopstart);
    mods.cValue = std::to_string(track.loopfinish);
    mods.dValue = track.playstart == play_start::play_start_loop ? "LOP" : "SMP";
    break;
  
  case 2: // AMP ENV
    mods.aName = "ATT";
    mods.bName = "DEC";
    mods.cName = "SUS";
    mods.dName = "REL";

    mods.aValue = std::to_string((float)round(track.amp_attack * 100) / 100);
    mods.aValue = mods.aValue.substr(0,3);

    mods.bValue = std::to_string((float)round(track.amp_decay * 100) / 100);
    mods.bValue = mods.bValue.substr(0,3);

    mods.cValue = std::to_string((float)round(track.amp_sustain * 100) / 100);
    mods.cValue = mods.cValue.substr(0,3);

    mods.dValue = std::to_string((float)round(track.amp_release * 100) / 100);
    mods.dValue = mods.dValue.substr(0,3);

    break;
  
  case 3: // OUTPUT
    mods.aName = "LVL";
    mods.bName = "PAN";
    mods.cName = "--"; // fx send?
    mods.dName = "--"; // fx return?

    mods.aValue = std::to_string((float)round(track.level * 100) / 100);
    mods.aValue = mods.aValue.substr(0,3);

    mods.bValue = std::to_string((float)round(track.pan * 100) / 100);
    mods.bValue = mods.bValue.substr(0,3);

    mods.cValue = "--";
    mods.dValue = "--";
    break;
  
  default:
    break;
  }

  return mods;
}

SOUND_CONTROL_MODS getWavSampleControlModData();
SOUND_CONTROL_MODS getWavSampleControlModData()
{
  SOUND_CONTROL_MODS mods;

  TRACK track = getHeapCurrentSelectedTrack();

  switch (current_page_selected)
  {
  case 0: // MAIN
    mods.aName = "FIL";
    mods.bName = "POS";
    mods.cName = "--";
    mods.dName = "--";

    mods.aValue = std::to_string(track.wav_sample_id+1);
    mods.bValue = "0ms";
    mods.cValue = "--";
    mods.dValue = "--";
    break;
  
  default:
    break;
  }

  return mods;
}

SOUND_CONTROL_MODS getMidiControlModData();
SOUND_CONTROL_MODS getMidiControlModData()
{
  SOUND_CONTROL_MODS mods;

  TRACK track = getHeapCurrentSelectedTrack();

  switch (current_page_selected)
  {
  case 0: // MAIN
    mods.aName = "LEN";
    mods.bName = "VEL";
    mods.cName = "CHN";
    mods.dName = "PRB";

    mods.aValue = std::to_string(track.length); // TODO : impl
    mods.bValue = std::to_string(track.velocity); // TODO: impl
    mods.cValue = std::to_string(track.channel); // TODO: impl
    mods.dValue = std::to_string(track.probability); // TODO: impl
    break;
  
  default:
    break;
  }

  return mods;
}

SOUND_CONTROL_MODS getCvGateControlModData();
SOUND_CONTROL_MODS getCvGateControlModData()
{
  SOUND_CONTROL_MODS mods;

  TRACK track = getHeapCurrentSelectedTrack();

  switch (current_page_selected)
  {
  case 0: // MAIN
    mods.aName = "LEN";
    mods.bName = "OUT";
    mods.cName = "uTM";
    mods.dName = "PRB";

    mods.aValue = std::to_string(track.length); // TODO : impl
    mods.bValue = "1AB"; // TODO: impl
    mods.cValue = "--"; // TODO: impl
    mods.dValue = "100%"; // TODO: impl
    break;
  
  default:
    break;
  }

  return mods;
}

SOUND_CONTROL_MODS getCvTrigControlModData();
SOUND_CONTROL_MODS getCvTrigControlModData()
{
  SOUND_CONTROL_MODS mods;

  TRACK track = getHeapCurrentSelectedTrack();

  switch (current_page_selected)
  {
  case 0: // MAIN
    mods.aName = "--";
    mods.bName = "OUT";
    mods.cName = "uTM";
    mods.dName = "PRB";

    mods.aValue = "--";
    mods.bValue = "1AB"; // TODO: impl
    mods.cValue = "--"; // TODO: impl
    mods.dValue = "100%"; // TODO: impl
    break;
  
  default:
    break;
  }

  return mods;
}

SOUND_CONTROL_MODS getControlModDataForTrack();
SOUND_CONTROL_MODS getControlModDataForTrack()
{
  SOUND_CONTROL_MODS mods;

  TRACK track = getHeapCurrentSelectedTrack();
  
  switch (track.track_type)
  {
  case SUBTRACTIVE_SYNTH:
    mods = getSubtractiveSynthControlModData();
    break;
  
  case RAW_SAMPLE:
    mods = getRawSampleControlModData();
    break;
  
  case WAV_SAMPLE:
    mods = getWavSampleControlModData();
    break;
  
  case MIDI:
    mods = getMidiControlModData();
    break;
  
  case CV_GATE:
    mods = getCvGateControlModData();
    break;
  
  case CV_TRIG:
    mods = getCvTrigControlModData();
    break;
  
  default:
    break;
  }

  return mods;
}

void drawControlMods(void);
void drawControlMods(void)
{
  int ctrlModHeaderY = 20;
  int ctrlModHeaderBoxSize = 9;
  int ctrlModHeaderStartX = 29;
  int ctrlModSpacerMult = 25;

  // draw control mod indicators (a,b,c,d)
  u8g2.drawLine(ctrlModHeaderStartX,30,128,30);

  u8g2.drawLine(21+ctrlModHeaderStartX,20,21+ctrlModHeaderStartX,52);
  u8g2.drawLine(22+ctrlModHeaderStartX+(ctrlModSpacerMult*1),20,22+ctrlModHeaderStartX+(ctrlModSpacerMult*1),52);
  u8g2.drawLine(24+ctrlModHeaderStartX+(ctrlModSpacerMult*2),20,24+ctrlModHeaderStartX+(ctrlModSpacerMult*2),52);
  //u8g2.drawLine(20+ctrlModHeaderStartX+(ctrlModSpacerMult*3),20,20+ctrlModHeaderStartX+(ctrlModSpacerMult*3),52);
  
  // u8g2.setColorIndex((u_int8_t)1);
  // u8g2.drawBox(ctrlModHeaderStartX,ctrlModHeaderY,ctrlModHeaderBoxSize,ctrlModHeaderBoxSize);
  // u8g2.drawBox(ctrlModHeaderStartX+(ctrlModSpacerMult*1),ctrlModHeaderY,ctrlModHeaderBoxSize,ctrlModHeaderBoxSize);
  // u8g2.drawBox(ctrlModHeaderStartX+(ctrlModSpacerMult*2),ctrlModHeaderY,ctrlModHeaderBoxSize,ctrlModHeaderBoxSize);
  // u8g2.drawBox(ctrlModHeaderStartX+(ctrlModSpacerMult*3),ctrlModHeaderY,ctrlModHeaderBoxSize,ctrlModHeaderBoxSize);
  // u8g2.drawLine(ctrlModHeaderStartX,20,29,52);
  // u8g2.drawLine(ctrlModHeaderStartX+(ctrlModSpacerMult*1),20,ctrlModHeaderStartX+(ctrlModSpacerMult*1),52);
  // u8g2.drawLine(ctrlModHeaderStartX+(ctrlModSpacerMult*2),20,ctrlModHeaderStartX+(ctrlModSpacerMult*2),52);
  // u8g2.drawLine(ctrlModHeaderStartX+(ctrlModSpacerMult*3),20,ctrlModHeaderStartX+(ctrlModSpacerMult*3),52);
  // u8g2.setColorIndex((u_int8_t)0);
  // u8g2.drawStr(ctrlModHeaderStartX+3,ctrlModHeaderY+1, "a");
  // u8g2.drawStr(ctrlModHeaderStartX+3+(ctrlModSpacerMult*1),ctrlModHeaderY+1, "b");
  // u8g2.drawStr(ctrlModHeaderStartX+3+(ctrlModSpacerMult*2),ctrlModHeaderY+1, "c");
  // u8g2.drawStr(ctrlModHeaderStartX+3+(ctrlModSpacerMult*3),ctrlModHeaderY+1, "d");
  // u8g2.setColorIndex((u_int8_t)1);

  SOUND_CONTROL_MODS mods = getControlModDataForTrack();

  u8g2.drawStr(1+ctrlModHeaderStartX+2,ctrlModHeaderY+1, mods.aName.c_str());
  u8g2.drawStr(2+ctrlModHeaderStartX+2+(ctrlModSpacerMult*1),ctrlModHeaderY+1, mods.bName.c_str());
  u8g2.drawStr(3+ctrlModHeaderStartX+2+(ctrlModSpacerMult*2),ctrlModHeaderY+1, mods.cName.c_str());
  u8g2.drawStr(4+ctrlModHeaderStartX+2+(ctrlModSpacerMult*3),ctrlModHeaderY+1, mods.dName.c_str());
  
  u8g2.drawStr(36,ctrlModHeaderY+15, mods.aValue.c_str());
  u8g2.drawStr(60,ctrlModHeaderY+15, mods.bValue.c_str());
  u8g2.drawStr(84,ctrlModHeaderY+15, mods.cValue.c_str());
  u8g2.drawStr(112,ctrlModHeaderY+15, mods.dValue.c_str());
}

void drawPageNumIndicators(void);
void drawPageNumIndicators(void)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();
  
  int pageNumBasedStartX = 81 - (3 * trackPageNumMap[currTrack.track_type]);
  int pageTabPosY = 56;
  int pageTabFooterNameStartX = 17;

  if (trackPageNumMap[currTrack.track_type] == 5) {
    pageTabFooterNameStartX -= 10;
  } else if (trackPageNumMap[currTrack.track_type] == 4) {
    pageTabFooterNameStartX -= 1;
  } else if (trackPageNumMap[currTrack.track_type] == 1) {
    pageTabFooterNameStartX += 30;
  } 

  u8g2.drawLine(0,52,128,52);
  u8g2.drawStr(0,pageTabPosY, getCurrPageNameForTrack().c_str());

  int pageBoxStartX = (pageNumBasedStartX + 30) - (trackPageNumMap[currTrack.track_type] * 5);
  pageBoxStartX += 8 / (trackPageNumMap[currTrack.track_type]);

  int pageNumStartX = pageBoxStartX + 3;
  int pageBetweenPaddingtX = 10;

  u8g2.drawBox(pageBoxStartX+(pageBetweenPaddingtX*current_page_selected),55,9,9);

  for (int l = 0; l < trackPageNumMap[currTrack.track_type]; l++)
  {
    if (l == current_page_selected) {
      u8g2.setColorIndex((u_int8_t)0);
      u8g2.drawStr(pageNumStartX+(pageBetweenPaddingtX*l),pageTabPosY, std::to_string(l+1).c_str());
      u8g2.setColorIndex((u_int8_t)1);
    } else {
      u8g2.drawStr(pageNumStartX+(pageBetweenPaddingtX*l),pageTabPosY, std::to_string(l+1).c_str());
    }
  }
}

void drawSequencerScreen()
{
  u8g2.clearBuffer();

  drawMenuHeader("BNK:", 1, 0);

  if (current_UI_mode == UI_MODE::PATTERN_WRITE) {
    u8g2.setColorIndex((u_int8_t)1);
    u8g2.drawBox(26,0,29,7);
    u8g2.setColorIndex((u_int8_t)0);
    drawMenuHeader("PTN:", current_selected_pattern+1, 29);
    u8g2.setColorIndex((u_int8_t)1);
  } else {
    drawMenuHeader("PTN:", current_selected_pattern+1, 29);
  }

  if (current_UI_mode == TRACK_WRITE || current_UI_mode == TRACK_SEL || current_UI_mode == SUBMITTING_STEP_VALUE) {
    u8g2.setColorIndex((u_int8_t)1);
    u8g2.drawBox(55,0,29,7);
    u8g2.setColorIndex((u_int8_t)0);
    drawMenuHeader("TRK:", current_selected_track+1, 58);
    u8g2.setColorIndex((u_int8_t)1);
  } else {
    drawMenuHeader("TRK:", current_selected_track+1, 58);
  }

  std::string bpmStr = strldz(std::to_string((uint8_t)uClock.getTempo()), 3);
  u8g2.drawStr(104, 0, bpmStr.c_str());

  // transport indicator (play, pause, stop)
  uint8_t transportIconStartX = 119;
  if (_seq_state.playback_state == STOPPED) {
    u8g2.drawBox(transportIconStartX,1,5,5);
  } else if (_seq_state.playback_state == RUNNING) {
    u8g2.drawTriangle(transportIconStartX,0,transportIconStartX,6,transportIconStartX+6,3);
  } else if (_seq_state.playback_state == PAUSED) {
    u8g2.drawBox(transportIconStartX,1,2,5);
    u8g2.drawBox(transportIconStartX+3,1,2,5);
  }

  if (current_UI_mode == PATTERN_WRITE) {
    // implement pattern main context area
  } else if (current_UI_mode == TRACK_WRITE || current_UI_mode == TRACK_SEL || current_UI_mode == SUBMITTING_STEP_VALUE) {
    TRACK currTrack = getHeapCurrentSelectedTrack();
    TRACK_TYPE currTrackType = currTrack.track_type;

    // draw track meta type box
    int trackMetaStrX = 2;
    if (currTrackType == SUBTRACTIVE_SYNTH || currTrackType == MIDI) {
      trackMetaStrX = 4;
    }
    u8g2.setColorIndex((u_int8_t)1);
    u8g2.drawBox(0,9,27,9);
    u8g2.setColorIndex((u_int8_t)0);
    std::string trackMetaStr = getTrackMetaStr(currTrackType);
    u8g2.drawStr(trackMetaStrX, 10, trackMetaStr.c_str());
    u8g2.setColorIndex((u_int8_t)1);

    // draw track info box
    u8g2.setColorIndex((u_int8_t)1);
    u8g2.drawBox(29,9,100,9);
    u8g2.setColorIndex((u_int8_t)0);

    std::string trackInfoStr;
    if (currTrackType == SUBTRACTIVE_SYNTH) {
      trackInfoStr += getTrackTypeNameStr(currTrackType);
    } else if (currTrackType == RAW_SAMPLE) {
      trackInfoStr += usableSampleNames[currTrack.raw_sample_id];
    } else if (currTrackType == RAW_SAMPLE || currTrackType == WAV_SAMPLE) {
      trackInfoStr += usableWavSampleNames[currTrack.wav_sample_id];
    } else if (currTrackType == MIDI) {
      trackInfoStr += "";
    } else if (currTrackType == CV_GATE || currTrackType == CV_TRIG) {
      trackInfoStr += "";
    }
    u8g2.drawStr(31, 10, trackInfoStr.c_str());
    u8g2.setColorIndex((u_int8_t)1);

    // draw track description / main icon area
    if (currTrackType == RAW_SAMPLE || currTrackType == SUBTRACTIVE_SYNTH || currTrackType == MIDI || currTrackType == CV_GATE) {
      u8g2.drawStr(6, 23, "NOTE");
      u8g2.setFont(bitocra13_c);
      u8g2.drawStr(8, 32, getDisplayNote().c_str());
      u8g2.setFont(bitocra7_c);
    } else if (currTrackType == WAV_SAMPLE) {
      u8g2.drawLine(4,29,4,40);
      u8g2.drawLine(4,40,22,40);
      u8g2.drawLine(22,27,22,40);
      u8g2.drawLine(14,27,22,27);
      u8g2.drawLine(11,29,14,27);
      u8g2.drawLine(4,29,11,29);
      u8g2.drawStr(8,31, "uSD");
    } else if (currTrackType == CV_TRIG) {
      u8g2.drawLine(3,42,14,42);
      u8g2.drawLine(14,42,14,28);
      u8g2.drawLine(14,28,19,28);
      u8g2.drawLine(19,28,19,42);
      u8g2.drawLine(19,42,24,42);
    }

    // draw control mod area
    drawControlMods();

    drawPageNumIndicators();
  }

  u8g2.sendBuffer();
}

void drawGenericOverlayFrame(void);
void drawGenericOverlayFrame(void)
{
  u8g2.setColorIndex((u_int8_t)0);
  u8g2.drawBox(2,2,125,62);
  u8g2.setColorIndex((u_int8_t)1);
  u8g2.drawFrame(2,2,125,62);
  u8g2.drawLine(2,16,125,16);
}

void drawSetTempoOverlay(void)
{
  drawGenericOverlayFrame();

  std::string tempoStr = "SET TEMPO";
  u8g2.drawStr(44, 6, tempoStr.c_str());
  u8g2.setFont(bitocra13_c);

  int tempoVal = (int)uClock.getTempo();
  std::string tempoValStr = std::to_string(tempoVal);
  int tempoValX = 52;

  u8g2.drawStr(tempoValX + (tempoVal > 99 ? 0 : 4), 31, tempoValStr.c_str());
  u8g2.setFont(small_font);

  u8g2.sendBuffer();
}

void triggerSubtractiveSynthNoteOn(uint8_t t, uint8_t note);
void triggerSubtractiveSynthNoteOn(uint8_t t, uint8_t note)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  AudioNoInterrupts();
  float foundBaseFreq = noteToFreqArr[note];
  float octaveFreqA = (foundBaseFreq + (currTrack.fine * 0.01)) * (pow(2, keyboardOctave));
  float octaveFreqB = (foundBaseFreq * pow(2.0, (float)currTrack.detune/12.0)) * (pow(2, keyboardOctave));

  comboVoices[t].osca.frequency(octaveFreqA);
  comboVoices[t].oscb.frequency(octaveFreqB);

  comboVoices[t].leftCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));
  comboVoices[t].rightCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));

  comboVoices[t].ampEnv.attack(currTrack.amp_attack * (currTrack.velocity * 0.01));
  comboVoices[t].ampEnv.decay(currTrack.amp_decay * (currTrack.velocity * 0.01));
  comboVoices[t].ampEnv.sustain(currTrack.amp_sustain * (currTrack.velocity * 0.01));
  comboVoices[t].ampEnv.release(currTrack.amp_release * (currTrack.velocity * 0.01));
  comboVoices[t].filterEnv.attack(currTrack.filter_attack);
  comboVoices[t].filterEnv.decay(currTrack.filter_decay);
  comboVoices[t].filterEnv.sustain(currTrack.filter_sustain);
  comboVoices[t].filterEnv.release(currTrack.filter_release);
  AudioInterrupts();

  // now triggers env
  comboVoices[t].ampEnv.noteOn();
  comboVoices[t].filterEnv.noteOn();
}

void triggerRawSampleNoteOn(uint8_t t, uint8_t note);
void triggerRawSampleNoteOn(uint8_t t, uint8_t note)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();
  int tOffset = t-4;

  if (t < 4) {
  AudioNoInterrupts();
    comboVoices[t].leftCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));
    comboVoices[t].rightCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));

    comboVoices[t].ampEnv.attack(currTrack.amp_attack * (currTrack.velocity * 0.01));
    comboVoices[t].ampEnv.decay(currTrack.amp_decay * (currTrack.velocity * 0.01));
    comboVoices[t].ampEnv.sustain(currTrack.amp_sustain * (currTrack.velocity * 0.01));
    comboVoices[t].ampEnv.release(currTrack.amp_release * (currTrack.velocity * 0.01));
  AudioInterrupts();

    comboVoices[t].ampEnv.noteOn();
    comboVoices[t].rSample.playRaw(samples[currTrack.raw_sample_id]->sampledata, samples[currTrack.raw_sample_id]->samplesize/2, numChannels);

    // always re-initialize loop type
    comboVoices[t].rSample.setLoopType(loopTypeSelMap[currTrack.looptype]);

    if (loopTypeSelMap[currTrack.looptype] == looptype_none) {
      comboVoices[t].rSample.setPlayStart(play_start::play_start_sample);
      comboVoices[t].rSample.setLoopType(loop_type::looptype_none);
    } else if (loopTypeSelMap[currTrack.looptype] == looptype_repeat) {
      float loopFinishToUse = currTrack.loopfinish;

      if (currTrack.chromatic_enabled) {
        float foundBaseFreq = noteToFreqArr[note];
        float octaveFreq = foundBaseFreq * (pow(2, keyboardOctave));
        //float freq = 440.0 * powf(2.0, (12-69) / 12.0);
        uint32_t numSamples = 44100 / octaveFreq;

        loopFinishToUse = numSamples;
      }

      comboVoices[t].rSample.setPlayStart(currTrack.playstart == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
      comboVoices[t].rSample.setLoopStart(currTrack.loopstart);
      comboVoices[t].rSample.setLoopFinish(loopFinishToUse);
    }
  } else {
  AudioNoInterrupts();
    sampleVoices[tOffset].leftCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));
    sampleVoices[tOffset].rightCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));

    sampleVoices[tOffset].ampEnv.attack(currTrack.amp_attack * (currTrack.velocity * 0.01));
    sampleVoices[tOffset].ampEnv.decay(currTrack.amp_decay * (currTrack.velocity * 0.01));
    sampleVoices[tOffset].ampEnv.sustain(currTrack.amp_sustain * (currTrack.velocity * 0.01));
    sampleVoices[tOffset].ampEnv.release(currTrack.amp_release * (currTrack.velocity * 0.01));
  AudioInterrupts();

    sampleVoices[tOffset].ampEnv.noteOn();
    sampleVoices[tOffset].rSample.playRaw(samples[currTrack.raw_sample_id]->sampledata, samples[currTrack.raw_sample_id]->samplesize/2, numChannels);

    // always re-initialize loop type
    sampleVoices[tOffset].rSample.setLoopType(loopTypeSelMap[currTrack.looptype]);

    if (loopTypeSelMap[currTrack.looptype] == looptype_none) {
      sampleVoices[tOffset].rSample.setPlayStart(play_start::play_start_sample);
      sampleVoices[tOffset].rSample.setLoopType(loop_type::looptype_none);
    } else if (loopTypeSelMap[currTrack.looptype] == looptype_repeat) {
      float loopFinishToUse = currTrack.loopfinish;

      if (currTrack.chromatic_enabled) {
        float foundBaseFreq = noteToFreqArr[note];
        float octaveFreq = foundBaseFreq * (pow(2, keyboardOctave));
        //float freq = 440.0 * powf(2.0, (12-69) / 12.0);
        uint32_t numSamples = 44100 / octaveFreq;

        loopFinishToUse = numSamples;
      }

      sampleVoices[tOffset].rSample.setPlayStart(currTrack.playstart == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
      sampleVoices[tOffset].rSample.setLoopStart(currTrack.loopstart);
      sampleVoices[tOffset].rSample.setLoopFinish(loopFinishToUse);
    }

  }
}

void triggerWavSampleNoteOn(uint8_t t, uint8_t note);
void triggerWavSampleNoteOn(uint8_t t, uint8_t note)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();
  int tOffset = t-4;

  if (t < 4) {
    comboVoices[t].ampEnv.noteOn();
    comboVoices[t].wSample.play(usableWavSampleNames[currTrack.wav_sample_id], sd1);
  } else {
    sampleVoices[tOffset].ampEnv.noteOn();
    sampleVoices[tOffset].wSample.play(usableWavSampleNames[currTrack.wav_sample_id], sd1);
  }
}

void triggerTrackManually(uint8_t t, uint8_t note) {
  TRACK currTrack = getHeapCurrentSelectedTrack();

  if (currTrack.track_type == RAW_SAMPLE) {
    triggerRawSampleNoteOn(t, note);
  } else if (currTrack.track_type == WAV_SAMPLE) {
    triggerWavSampleNoteOn(t, note);
  } else if (currTrack.track_type == SUBTRACTIVE_SYNTH) {
    triggerSubtractiveSynthNoteOn(t, note);
  }
}

void triggerAllStepsForGlobalStep(uint32_t tick)
{
  int8_t currGlobalStep = _seq_state.current_step - 1; // get zero-based global step
  PATTERN currentPattern = getHeapCurrentSelectedPattern();

  const int MAX_PATTERN_TRACK_SIZE = 16; // TODO: make this a define later

  for (int t = 0; t < MAX_PATTERN_TRACK_SIZE; t++) {
    TRACK currTrack = currentPattern.tracks[t];
    TRACK_STEP currTrackStep = currTrack.steps[currGlobalStep];

    if ((currTrackStep.state == TRACK_STEP_STATE::ON) || (currTrackStep.state == TRACK_STEP_STATE::ACCENTED)) {
      handleAddToStepStack(tick, t, currGlobalStep);
    }
  }
}

bool checked_remaining_seq_notes = false;

void handleRawSampleNoteOnForTrackStep(int track, int step);
void handleRawSampleNoteOnForTrackStep(int track, int step)
{
  TRACK trackToUse = getHeapTrack(track);
  TRACK_STEP stepToUse = getHeapStep(track, step);

  if (track > 3) {
    int tOffset = track-4;
  AudioNoInterrupts();
    sampleVoices[tOffset].leftCtrl.gain(getStereoPanValues(trackToUse.pan).left * (stepToUse.velocity * 0.01));
    sampleVoices[tOffset].rightCtrl.gain(getStereoPanValues(trackToUse.pan).right * (stepToUse.velocity * 0.01));

    sampleVoices[tOffset].ampEnv.attack(trackToUse.amp_attack * (stepToUse.velocity * 0.01));
    sampleVoices[tOffset].ampEnv.decay(trackToUse.amp_decay * (stepToUse.velocity * 0.01));
    sampleVoices[tOffset].ampEnv.sustain(trackToUse.amp_sustain * (stepToUse.velocity * 0.01));
    sampleVoices[tOffset].ampEnv.release(trackToUse.amp_release * (stepToUse.velocity * 0.01));
  AudioInterrupts();

    sampleVoices[tOffset].ampEnv.noteOn();
    sampleVoices[tOffset].rSample.playRaw(samples[trackToUse.raw_sample_id]->sampledata, samples[trackToUse.raw_sample_id]->samplesize/2, numChannels);

    // always re-initialize loop type
    sampleVoices[tOffset].rSample.setLoopType(loopTypeSelMap[trackToUse.looptype]);

    if (loopTypeSelMap[trackToUse.looptype] == looptype_none) {
      sampleVoices[tOffset].rSample.setPlayStart(play_start::play_start_sample);
      sampleVoices[tOffset].rSample.setLoopType(loop_type::looptype_none);
    } else if (loopTypeSelMap[trackToUse.looptype] == looptype_repeat) {
      float loopFinishToUse = trackToUse.loopfinish;

      if (trackToUse.chromatic_enabled) {
        float foundBaseFreq = noteToFreqArr[stepToUse.note];
        float octaveFreq = foundBaseFreq * (pow(2, keyboardOctave));
        //float freq = 440.0 * powf(2.0, (12-69) / 12.0);
        uint32_t numSamples = 44100 / octaveFreq;

        loopFinishToUse = numSamples;
      }

      sampleVoices[tOffset].rSample.setPlayStart(trackToUse.playstart == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
      sampleVoices[tOffset].rSample.setLoopStart(trackToUse.loopstart);
      sampleVoices[tOffset].rSample.setLoopFinish(loopFinishToUse);
    }
  } else {
  AudioNoInterrupts();
    comboVoices[track].leftCtrl.gain(getStereoPanValues(trackToUse.pan).left * (stepToUse.velocity * 0.01));
    comboVoices[track].rightCtrl.gain(getStereoPanValues(trackToUse.pan).right * (stepToUse.velocity * 0.01));

    comboVoices[track].ampEnv.attack(trackToUse.amp_attack * (stepToUse.velocity * 0.01));
    comboVoices[track].ampEnv.decay(trackToUse.amp_decay * (stepToUse.velocity * 0.01));
    comboVoices[track].ampEnv.sustain(trackToUse.amp_sustain * (stepToUse.velocity * 0.01));
    comboVoices[track].ampEnv.release(trackToUse.amp_release * (stepToUse.velocity * 0.01));
  AudioInterrupts();

    comboVoices[track].ampEnv.noteOn();
    comboVoices[track].rSample.playRaw(samples[trackToUse.raw_sample_id]->sampledata, samples[trackToUse.raw_sample_id]->samplesize/2, numChannels);

    // always re-initialize loop type
    comboVoices[track].rSample.setLoopType(loopTypeSelMap[trackToUse.looptype]);

    if (loopTypeSelMap[trackToUse.looptype] == looptype_none) {
      comboVoices[track].rSample.setPlayStart(play_start::play_start_sample);
      comboVoices[track].rSample.setLoopType(loop_type::looptype_none);
    } else if (loopTypeSelMap[trackToUse.looptype] == looptype_repeat) {
      float loopFinishToUse = trackToUse.loopfinish;

      if (trackToUse.chromatic_enabled) {
        float foundBaseFreq = noteToFreqArr[stepToUse.note];
        float octaveFreq = foundBaseFreq * (pow(2, keyboardOctave));
        //float freq = 440.0 * powf(2.0, (12-69) / 12.0);
        uint32_t numSamples = 44100 / octaveFreq;

        loopFinishToUse = numSamples;
      }

      comboVoices[track].rSample.setPlayStart(trackToUse.playstart == play_start::play_start_loop ? play_start::play_start_loop : play_start::play_start_sample);
      comboVoices[track].rSample.setLoopStart(trackToUse.loopstart);
      comboVoices[track].rSample.setLoopFinish(loopFinishToUse);
    }
  }
}

void handleWavSampleNoteOnForTrackStep(int track, int step);
void handleWavSampleNoteOnForTrackStep(int track, int step)
{
  TRACK trackToUse = getHeapTrack(track);

  if (track > 3) {
    int tOffset = track-4;

    sampleVoices[tOffset].ampEnv.noteOn();
    sampleVoices[tOffset].wSample.play(usableWavSampleNames[trackToUse.wav_sample_id], sd1);
  } else {
    comboVoices[track].ampEnv.noteOn();
    comboVoices[track].wSample.play(usableWavSampleNames[trackToUse.wav_sample_id], sd1);
  }
}

void handleSubtractiveSynthNoteOnForTrackStep(int track, int step);
void handleSubtractiveSynthNoteOnForTrackStep(int track, int step)
{
  TRACK trackToUse = getHeapTrack(track);
  TRACK_STEP stepToUse = getHeapStep(track, step);

  AudioNoInterrupts();
  float foundBaseFreq = noteToFreqArr[stepToUse.note];
  float octaveFreqA = (foundBaseFreq + (trackToUse.fine * 0.01)) * (pow(2, stepToUse.octave));
  float octaveFreqB = (foundBaseFreq * pow(2.0, (float)trackToUse.detune/12.0)) * (pow(2, stepToUse.octave));

  comboVoices[track].osca.frequency(octaveFreqA);
  comboVoices[track].oscb.frequency(octaveFreqB);

  comboVoices[track].leftCtrl.gain(getStereoPanValues(trackToUse.pan).left * (stepToUse.velocity * 0.01));
  comboVoices[track].rightCtrl.gain(getStereoPanValues(trackToUse.pan).right * (stepToUse.velocity * 0.01));

  comboVoices[track].ampEnv.attack(trackToUse.amp_attack * (stepToUse.velocity * 0.01));
  comboVoices[track].ampEnv.decay(trackToUse.amp_decay * (stepToUse.velocity * 0.01));
  comboVoices[track].ampEnv.sustain(trackToUse.amp_sustain * (stepToUse.velocity * 0.01));
  comboVoices[track].ampEnv.release(trackToUse.amp_release * (stepToUse.velocity * 0.01));
  comboVoices[track].filterEnv.attack(trackToUse.filter_attack);
  comboVoices[track].filterEnv.decay(trackToUse.filter_decay);
  comboVoices[track].filterEnv.sustain(trackToUse.filter_sustain);
  comboVoices[track].filterEnv.release(trackToUse.filter_release);
  AudioInterrupts();

  // now triggers envs
  comboVoices[track].ampEnv.noteOn();
  comboVoices[track].filterEnv.noteOn();
}

void handleNoteOnForTrackStep(int track, int step);
void handleNoteOnForTrackStep(int track, int step)
{
  TRACK trackToUse = getHeapTrack(track);

  if (trackToUse.track_type == RAW_SAMPLE) {
    handleRawSampleNoteOnForTrackStep(track, step);
  } else if (trackToUse.track_type == WAV_SAMPLE) {
    handleWavSampleNoteOnForTrackStep(track, step);
  } else if (trackToUse.track_type == SUBTRACTIVE_SYNTH) {
    handleSubtractiveSynthNoteOnForTrackStep(track, step);
  }
}

void handleNoteOffForTrackStep(int track, int step);
void handleNoteOffForTrackStep(int track, int step)
{
  TRACK currTrack = getHeapTrack(track);

  if (currTrack.track_type == SUBTRACTIVE_SYNTH) {
    comboVoices[track].ampEnv.noteOff();
    comboVoices[track].filterEnv.noteOff();
  } else {
    if (track > 3) {
      int tOffset = track-4;
      sampleVoices[tOffset].ampEnv.noteOff();
    } else {
      comboVoices[track].ampEnv.noteOff();
    }
  }
}

void setDisplayStateForAllStepLEDs(void)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  const int MAX_TRACK_LEDS_SIZE = 17;
  for (int l = 1; l < MAX_TRACK_LEDS_SIZE; l++) {
    TRACK_STEP currTrackStepForLED = currTrack.steps[l-1];
    int8_t curr_led_char = stepCharMap[l];
    uint8_t keyLED = getKeyLED(curr_led_char);

    if (currTrackStepForLED.state == TRACK_STEP_STATE::OFF) {
      setLEDPWM(keyLED, 0);
    } else if (currTrackStepForLED.state == TRACK_STEP_STATE::ON) {
      setLEDPWM(keyLED, 512); // 256 might be better
    } else if (currTrackStepForLED.state == TRACK_STEP_STATE::ACCENTED) {
      setLEDPWM(keyLED, 4095);
    }
  }
}

void setDisplayStateForPatternActiveTracksLEDs(bool enable)
{
  int8_t currGlobalStep = _seq_state.current_step - 1; // get zero-based step

  PATTERN currentPattern = getHeapCurrentSelectedPattern();

  const int MAX_PATTERN_TRACK_SIZE = 17;
  for (int t = 1; t < MAX_PATTERN_TRACK_SIZE; t++) {
    TRACK currTrack = currentPattern.tracks[t-1];
    int8_t curr_led_char = stepCharMap[t];
    uint8_t keyLED = getKeyLED(curr_led_char);
    
    TRACK_STEP currTrackStepForLED = currTrack.steps[currGlobalStep];

    if (currTrackStepForLED.state == TRACK_STEP_STATE::OFF) {
      setLEDPWM(keyLED, 0);
    } else if (currTrackStepForLED.state == TRACK_STEP_STATE::ON || currTrackStepForLED.state == TRACK_STEP_STATE::ACCENTED) {
      setLEDPWM(keyLED, enable ? 4095 : 0);
    }
  }
}

void displayCurrentlySelectedPattern(void)
{
  int8_t curr_led_char = stepCharMap[current_selected_pattern+1];
  uint8_t keyLED = getKeyLED(curr_led_char);

  setLEDPWM(keyLED, 4095);
}

void displayCurrentlySelectedTrack(void)
{
  int8_t curr_led_char = stepCharMap[current_selected_track+1];
  uint8_t keyLED = getKeyLED(curr_led_char);
  
  setLEDPWM(keyLED, 4095);
}

void clearAllStepLEDs(void)
{
  for (int s = 0; s < 16; s++) {
    setLEDPWM(stepLEDPins[s], 0);
  }
}

void handleRemoveFromStepStack(uint32_t tick);
void handleRemoveFromStepStack(uint32_t tick)
{
  for ( int i = 0; i < STEP_STACK_SIZE; i++ ) {
    if (_step_stack[i].length != -1) {
      --_step_stack[i].length;

      if (_step_stack[i].length == 0) {
        handleNoteOffForTrackStep(_step_stack[i].trackNum, _step_stack[i].stepNum);

        // re-initialize stack entry
        _step_stack[i].trackNum = -1;
        _step_stack[i].stepNum = -1;
        _step_stack[i].length = -1;
      }
    }  
  }
}

void handleAddToStepStack(uint32_t tick, int track, int step)
{
  TRACK trackToUse = getHeapTrack(track);
  TRACK_STEP stepToUse = getHeapStep(track, step);

  for ( uint8_t i = 0; i < STEP_STACK_SIZE; i++ ) {
    if ( _step_stack[i].length == -1 ) {
      _step_stack[i].trackNum = track;
      _step_stack[i].stepNum = step;
      _step_stack[i].length = stepToUse.length > 0 ? stepToUse.length : trackToUse.length;

      handleNoteOnForTrackStep(_step_stack[i].trackNum, _step_stack[i].stepNum);

      return;
    }
  }
}

void handle_bpm_step(uint32_t tick)
{
  // This method handles advancing the sequencer
  // and displaying the start btn and step btn BPM LEDs
  int8_t curr_step_char = stepCharMap[_seq_state.current_step];
  uint8_t keyLED = getKeyLED(curr_step_char);

  PATTERN currPattern = getHeapCurrentSelectedPattern();
  int currPatternLastStep = currPattern.last_step;

  // This handles displaying the BPM for the start button led
  // on qtr note. Check for odd step number to make sure not lit on backbeat qtr note.
  if (!(tick % 6)) {
    bool isOnStraightBeat = (_seq_state.current_step == 1 || !((_seq_state.current_step-1) % 4));
    if (isOnStraightBeat) {
      setLEDPWM(23, 4095); // each straight quarter note start button led ON
    }
  } else if ( !(tick % bpm_blink_timer) ) {
    setLEDPWM(23, 0); // turn start button led OFF
  }

  handleRemoveFromStepStack(tick);

  // This handles the sixteenth steps for all tracks
  if ( !(tick % (6)) ) {
    if (current_UI_mode == PATTERN_WRITE) {
      setDisplayStateForPatternActiveTracksLEDs(true);
    }

    // TODO: move out of 16th step !(tick % (6)) condition
    // so we can check for microtiming adjustments at the 96ppqn scale
    triggerAllStepsForGlobalStep(tick);

    if (current_UI_mode == PATTERN_SEL) {
      clearAllStepLEDs();
      displayCurrentlySelectedPattern();
    } else if (current_UI_mode == TRACK_SEL) {
      clearAllStepLEDs();
      displayCurrentlySelectedTrack();
    } else if (current_UI_mode == TRACK_WRITE) {

      // show sequencer running LEDs for all other modes?
      if (_seq_state.current_step > 1) {
        uint8_t prevKeyLED = getKeyLED(stepCharMap[_seq_state.current_step-1]);
        setLEDPWM(prevKeyLED, 0); // turn prev sixteenth led OFF
      } else if (_seq_state.current_step == 1) {
        uint8_t prevKeyLED = getKeyLED(stepCharMap[16]);
        setLEDPWM(prevKeyLED, 0); // turn prev sixteenth led OFF
      }
    
      setLEDPWM(keyLED, 4095); // turn sixteenth led ON
    }

    if (_seq_state.current_step <= currPatternLastStep) {
      if (_seq_state.current_step < currPatternLastStep) {
        ++_seq_state.current_step; // advance current step for sequencer
      } else {
        _seq_state.current_step = 1; // reset current step
      }
    }
  } else if ( !(tick % bpm_blink_timer) ) {
    if (current_UI_mode != PATTERN_SEL || current_UI_mode != TRACK_SEL) {
      setLEDPWM(keyLED, 0); // turn 16th and start OFF
    }

    if (current_UI_mode == PATTERN_WRITE) {
      setDisplayStateForPatternActiveTracksLEDs(false);
    } else if (current_UI_mode == TRACK_WRITE || current_UI_mode == SUBMITTING_STEP_VALUE) {
      setDisplayStateForAllStepLEDs();
    }
  }

  // every 1/4 step log memory usage
  if (!(tick % 24)) {
    Serial.print("Memory: ");
    Serial.print(AudioMemoryUsage());
    Serial.print(",");
    Serial.print(AudioMemoryUsageMax());
    Serial.println();
  }
}

void toggleSequencerPlayback(char btn)
{
  int8_t curr_step_char = stepCharMap[_seq_state.current_step-1];
  uint8_t keyLED = getKeyLED(curr_step_char);

  if (_seq_state.playback_state > STOPPED) {
    if (_seq_state.playback_state == RUNNING && btn == 'q') {
      _seq_state.playback_state = PAUSED;
      uClock.pause();
      
      setLEDPWMDouble(23, 0, keyLED, 0);

      if (current_UI_mode == UI_MODE::TRACK_WRITE) {
        setDisplayStateForAllStepLEDs(); // TODO: wrap with "if in track view display step LED state" conditional, etc
      } else if (current_UI_mode == UI_MODE::PATTERN_WRITE) {
        clearAllStepLEDs();
      }
    } else if (_seq_state.playback_state == PAUSED && btn == 'q') {
      // Unpaused, so advance sequencer from last known step
      _seq_state.playback_state = RUNNING;
      uClock.pause();
    } else if (btn == 'w') {
      // Stopped, so reset sequencer to FIRST step in pattern
      _seq_state.current_step = 1;
      _seq_state.playback_state = STOPPED;
      uClock.stop();

      setLEDPWM(keyLED, 0); // turn off current step LED
      setLEDPWM(23, 0); // turn start button led OFF
      
      if (current_UI_mode == UI_MODE::TRACK_WRITE) {
        setDisplayStateForAllStepLEDs(); // TODO: wrap with "if in track view display step LED state" conditional, etc
      } else if (current_UI_mode == UI_MODE::PATTERN_WRITE) {
        clearAllStepLEDs();
      }
    }
  } else if (btn == 'q') {
    // Started, so start sequencer from FIRST step in pattern
    //_seq_state.current_step = 1;
    _seq_state.playback_state = RUNNING;
    uClock.start();
  } else if (btn == 'w') {
    // Stopped, so reset sequencer to FIRST step in pattern
    _seq_state.current_step = 1;
    _seq_state.playback_state = STOPPED;
    uClock.stop();
    
    //setLEDPWM(keyLED, 0); // turn off current step LED
    setLEDPWM(23, 0); // turn start button led OFF
    
    if (current_UI_mode == UI_MODE::TRACK_WRITE) {
      setDisplayStateForAllStepLEDs(); // TODO: wrap with "if in track view display step LED state" conditional, etc
    } else if (current_UI_mode == UI_MODE::PATTERN_WRITE) {
      clearAllStepLEDs();
    }
  }
}

const char* getKeyStr(char idx) {
  return charCharsMap[idx];
}

uint8_t getKeyLED(char idx) {
  if (charLEDMap.count(idx) != 0) {
    return charLEDMap[idx];
  }

  return 17;
}

bool btnCharIsATrack(char btnChar) {
  char* validTrackChars = "mnopstuvyz125678";
  if (strchr(validTrackChars, btnChar) != NULL)
  {
    return true;
  }

  return false;
}

uint8_t getKeyStepNum(char idx)
{
  if (charStepMap.count(idx) != 0) {
    return charStepMap[idx];
  }

  return 1; // default first step num
}

void toggleSelectedStep(uint8_t step)
{
  uint8_t adjStep = step-1; // get zero based step num

  Serial.print("adjStep: ");
  Serial.println(adjStep);

  TRACK currTrack = getHeapCurrentSelectedTrack();
  TRACK_STEP_STATE currStepState = currTrack.steps[adjStep].state;

  Serial.print("currStepState: ");
  Serial.println(currStepState == TRACK_STEP_STATE::ACCENTED ? "accented" : (currStepState == TRACK_STEP_STATE::ON ? "on" : "off"));

  // TODO: implement accent state for MIDI, CV/Trig, Sample, Synth track types?
  if (currStepState == TRACK_STEP_STATE::OFF) {
    _seq_heap.pattern.tracks[current_selected_track].steps[adjStep].state = TRACK_STEP_STATE::ON;
    // copy track properties to steps
    _seq_heap.pattern.tracks[current_selected_track].steps[adjStep].note = currTrack.note;
    _seq_heap.pattern.tracks[current_selected_track].steps[adjStep].octave = currTrack.octave;
    _seq_heap.pattern.tracks[current_selected_track].steps[adjStep].velocity = currTrack.velocity;
  } else if (currStepState == TRACK_STEP_STATE::ON) {
    _seq_heap.pattern.tracks[current_selected_track].steps[adjStep].state = TRACK_STEP_STATE::ACCENTED;
    _seq_heap.pattern.tracks[current_selected_track].steps[adjStep].velocity = 100; // TODO: use a "global accent" value here
  } else if (currStepState == TRACK_STEP_STATE::ACCENTED) {
    _seq_heap.pattern.tracks[current_selected_track].steps[adjStep].state = TRACK_STEP_STATE::OFF;
  }
}

void handleEncoderSetTempo()
{
  int main_encoder_idx = 0;
  encoder_currValues[main_encoder_idx] = encoder_getValue(encoder_addrs[main_encoder_idx]);

  if(encoder_currValues[main_encoder_idx] != encoder_lastValues[main_encoder_idx]) {
    int diff = encoder_currValues[main_encoder_idx] - encoder_lastValues[main_encoder_idx];

    float currTempo = uClock.getTempo();
    float newTempo = currTempo + diff;

    if (!(newTempo < 30 || newTempo > 300)) {
      if ((newTempo - currTempo >= 1) || (currTempo - newTempo) >= 1) {
        uClock.setTempo(newTempo);
        drawSetTempoOverlay();
      }
    }

    encoder_lastValues[main_encoder_idx] = encoder_currValues[main_encoder_idx];
  }
}

void updateTrackLength(int diff);
void updateTrackLength(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  // length adj
  int currLen = currTrack.length;
  int newLen = currLen + diff;

  if (current_UI_mode == SUBMITTING_STEP_VALUE) {
    TRACK_STEP currStep = getHeapCurrentSelectedTrackStep();

    currLen = currStep.length;
    newLen = currLen + diff;
  }

  if (!(newLen < 0 && newLen > 64) && (newLen != currLen)) {
    if (current_UI_mode == SUBMITTING_STEP_VALUE) {
      _seq_heap.pattern.tracks[current_selected_track].steps[current_selected_step].length = newLen;
    } else {
      // TODO: see if track length is even needed?
      // if it is, just copy current track length to all steps in track 
      _seq_heap.pattern.tracks[current_selected_track].length = newLen;
    }

    drawSequencerScreen();
  }
}

void updateSubtractiveSynthWaveform(int diff);
void updateSubtractiveSynthWaveform(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  int newWaveform = (waveformFindMap[(int)currTrack.waveform]) + diff;

  if (newWaveform < 0) {
    newWaveform = 5;
  } else if (newWaveform > 5) {
    newWaveform = 0;
  }

  int waveformSel = waveformSelMap[newWaveform];

  _seq_heap.pattern.tracks[current_selected_track].waveform = waveformSel;

  comboVoices[current_selected_track].osca.begin(waveformSel);
  comboVoices[current_selected_track].oscb.begin(waveformSel);

  drawSequencerScreen();
}

void updateSubtractiveSynthNoiseAmt(int diff);
void updateSubtractiveSynthNoiseAmt(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();
  float currNoise = currTrack.noise;
  float newNoise = currTrack.noise + (diff * 0.05);

  if (!(newNoise < 0.01 || newNoise > 1.0) && newNoise != currNoise) {
    _seq_heap.pattern.tracks[current_selected_track].noise = newNoise;

    AudioNoInterrupts();
    comboVoices[current_selected_track].noise.amplitude(newNoise);
    AudioInterrupts();

    drawSequencerScreen();
  }
}

void updateSubtractiveSynthFilterEnvAttack(int diff);
void updateSubtractiveSynthFilterEnvAttack(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  float currAtt = currTrack.filter_attack;

  int mult = 1;
  if (abs(diff) > 5) {
    mult = 200;
  } else if (abs(diff) > 1 && abs(diff) < 5) {
    mult = 20;
  }

  float newAtt = currTrack.filter_attack + (diff * mult);

  if (!(newAtt < 0 || newAtt > 11880) && newAtt != currAtt) {
    _seq_heap.pattern.tracks[current_selected_track].filter_attack = newAtt;

    AudioNoInterrupts();
    comboVoices[current_selected_track].filterEnv.attack(newAtt);
    AudioInterrupts();

    drawSequencerScreen();
  }
}

void updateTrackAmpEnvAttack(int diff);
void updateTrackAmpEnvAttack(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  float currAtt = currTrack.amp_attack;

  int mult = 1;
  if (abs(diff) > 5) {
    mult = 100;
  } else if (abs(diff) > 1 && abs(diff) < 5) {
    mult = 10;
  }

  float newAtt = currTrack.amp_attack + (diff * mult);

  if (!(newAtt < 1 || newAtt > 11880) && newAtt != currAtt) {
    _seq_heap.pattern.tracks[current_selected_track].amp_attack = newAtt;

    AudioNoInterrupts();
    comboVoices[current_selected_track].ampEnv.attack(newAtt);
    AudioInterrupts();

    drawSequencerScreen();
  }
}

void updateComboTrackLevel(int diff);
void updateComboTrackLevel(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  float currLvl = currTrack.level;
  float newLvl = currTrack.level + (diff * 0.1);

  if (!(newLvl < -0.1 || newLvl > 1.1) && newLvl != currLvl) {
    _seq_heap.pattern.tracks[current_selected_track].level = newLvl;

    AudioNoInterrupts();
    comboVoices[current_selected_track].leftSubMix.gain(0, newLvl);
    comboVoices[current_selected_track].leftSubMix.gain(1, newLvl);
    comboVoices[current_selected_track].rightSubMix.gain(0, newLvl);
    comboVoices[current_selected_track].rightSubMix.gain(1, newLvl);
    AudioInterrupts();

    drawSequencerScreen();
  }
}

void handleEncoderSubtractiveSynthModA(int diff);
void handleEncoderSubtractiveSynthModA(int diff)
{
  switch (current_page_selected)
  {
  case 0:
    updateTrackLength(diff);
    break;
  case 1:
    updateSubtractiveSynthWaveform(diff);
    break;
  case 2:
    updateSubtractiveSynthNoiseAmt(diff);
    break;
  case 3:
    updateSubtractiveSynthFilterEnvAttack(diff);
    break;
  case 4:
    updateTrackAmpEnvAttack(diff);
    break;
  case 5:
    updateComboTrackLevel(diff);
    break;
  default:
    break;
  }
}

void handleEncoderSubtractiveSynthModB(int diff);
void handleEncoderSubtractiveSynthModB(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  if (current_page_selected == 0) {
    float currVel = currTrack.velocity;
    float newVel = currTrack.velocity + diff;

    if (!(newVel < 1 || newVel > 100) && newVel != currVel) {
      _seq_heap.pattern.tracks[current_selected_track].velocity = newVel;

      // AudioNoInterrupts();
      // voices[current_selected_track].leftCtrl.gain(newVel * 0.01);
      // voices[current_selected_track].rightCtrl.gain(newVel * 0.01);
      // AudioInterrupts();

      drawSequencerScreen();
    }
  } else if (current_page_selected == 1) {
    float currDetune = currTrack.detune;
    float newDetune = currDetune + diff;

    if (!(newDetune < -24 || newDetune > 24) && newDetune != currDetune) {
      uint8_t note_to_use = currTrack.note;
      if (keyboardNotesHeld != 0) {
        note_to_use = note_on_keyboard;
      }

      _seq_heap.pattern.tracks[current_selected_track].detune = newDetune;

      // if seq is running, don't adjust detune in realtime:
      if (_seq_state.playback_state != RUNNING) {          
        AudioNoInterrupts();

        
        float foundBaseFreq = noteToFreqArr[note_to_use];
        float octaveFreqB = (foundBaseFreq * pow(2.0, (float)newDetune/12.0)) * (pow(2, keyboardOctave));
        comboVoices[current_selected_track].oscb.frequency(octaveFreqB);

        AudioInterrupts();
      }

      drawSequencerScreen();
    }
  } else if (current_page_selected == 2) {
    float currCutoff = currTrack.cutoff;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 200;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 10;
    }

    float newCutoff = currTrack.cutoff + (diff * mult);

    if (!(newCutoff < 1 || newCutoff > 3000) && newCutoff != currCutoff) {
      _seq_heap.pattern.tracks[current_selected_track].cutoff = newCutoff;

      AudioNoInterrupts();
      comboVoices[current_selected_track].lfilter.frequency(newCutoff);
      AudioInterrupts();

      drawSequencerScreen();
    }
  } else if (current_page_selected == 3) {
    float currDecay = currTrack.filter_decay;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 100;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 10;
    }

    float newDecay = currTrack.filter_decay + (diff * mult);

    if (!(newDecay < 0 || newDecay > 11880) && newDecay != currDecay) {
      _seq_heap.pattern.tracks[current_selected_track].filter_decay = newDecay;

      AudioNoInterrupts();
      comboVoices[current_selected_track].filterEnv.decay(newDecay);
      AudioInterrupts();

      drawSequencerScreen();
    }
  } else if (current_page_selected == 4) {
    float currDecay = currTrack.amp_decay;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 100;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 10;
    }

    float newDecay = currTrack.amp_decay + (diff * mult);

    if (!(newDecay < 0 || newDecay > 11880) && newDecay != currDecay) {
      _seq_heap.pattern.tracks[current_selected_track].amp_decay = newDecay;

      AudioNoInterrupts();
      comboVoices[current_selected_track].ampEnv.decay(newDecay);
      AudioInterrupts();

      drawSequencerScreen();
    }
  } else if (current_page_selected == 5) {
    float currPan = currTrack.pan;
    float newPan = currTrack.pan + (diff * 0.1);

    if (!(newPan < -1.0 || newPan > 1.0) && newPan != currPan) {
      _seq_heap.pattern.tracks[current_selected_track].pan = newPan;

      float newGainL = 1.0;
      if (newPan < 0) {
        newGainL += newPan;
      }

      float newGainR = 1.0;
      if (newPan > 0) {
        newGainR -= newPan;
      }

      AudioNoInterrupts();
      comboVoices[current_selected_track].leftCtrl.gain(newGainL);
      comboVoices[current_selected_track].rightCtrl.gain(newGainR);
      AudioInterrupts();

      drawSequencerScreen();
    }
  }
}

void handleEncoderSubtractiveSynthModC(int diff);
void handleEncoderSubtractiveSynthModC(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  if (current_page_selected == 0) {
    // probability
  } else if (current_page_selected == 1) {
    float currFine = currTrack.fine;
    float newFine = currFine + diff;

    if (!(newFine < -50.0 || newFine > 50.0) && newFine != currFine) {
      _seq_heap.pattern.tracks[current_selected_track].fine = newFine;
      TRACK_STEP currStep = _seq_heap.pattern.tracks[current_selected_track].steps[current_selected_step];

      // TODO: also use step note
      uint8_t note_to_use = currTrack.note;
      if (keyboardNotesHeld != 0) {
        note_to_use = note_on_keyboard;
      } else {
        //note_to_use = currStep.note;
      }

      // if seq running, don't adjust fine freq in realtime:
      if (_seq_state.playback_state != RUNNING) {
        float foundBaseFreq = noteToFreqArr[note_to_use];
        float octaveFreqA = (foundBaseFreq + (currTrack.fine * 0.01)) * (pow(2, keyboardOctave));
      
        AudioNoInterrupts();

        comboVoices[current_selected_track].osca.frequency(octaveFreqA);

        AudioInterrupts();
      }

      drawSequencerScreen();
    }
  } else if (current_page_selected == 2) {
    float currRes = currTrack.res;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 10;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 5;
    }

    float newRes = currTrack.res + (diff * mult * 0.01);

    if (!(newRes < -0.01 || newRes > 1.9) && newRes != currRes) {
      _seq_heap.pattern.tracks[current_selected_track].res = newRes;

      AudioNoInterrupts();
      comboVoices[current_selected_track].lfilter.resonance(newRes);
      AudioInterrupts();

      drawSequencerScreen();
    }
  } else if (current_page_selected == 3) {
    float curSus = currTrack.filter_sustain;
    float newSus = currTrack.filter_sustain + (diff * 0.01);

    if (!(newSus < 0 || newSus > 1.0) && newSus != curSus) {
      _seq_heap.pattern.tracks[current_selected_track].filter_sustain = newSus;

      AudioNoInterrupts();
      comboVoices[current_selected_track].filterEnv.sustain(newSus);
      AudioInterrupts();

      drawSequencerScreen();
    }
  } else if (current_page_selected == 4) {
    float curSus = currTrack.amp_sustain;
    float newSus = currTrack.amp_sustain + (diff * 0.01);

    if (!(newSus < 0 || newSus > 1.0) && newSus != curSus) {
      _seq_heap.pattern.tracks[current_selected_track].amp_sustain = newSus;

      AudioNoInterrupts();
      comboVoices[current_selected_track].ampEnv.sustain(newSus);
      AudioInterrupts();

      drawSequencerScreen();
    }
  } else if (current_page_selected == 5) {
    // n/a
  }
}

void handleEncoderSubtractiveSynthModD(int diff);
void handleEncoderSubtractiveSynthModD(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  if (current_page_selected == 0) { // first page
    // probability
  } else if (current_page_selected == 1) { // width
    float currWidth = currTrack.width;
    float newWidth = currWidth + (diff * 0.01);

    if (!(newWidth < 0.01 || newWidth > 1.0) && newWidth != currWidth) {
      _seq_heap.pattern.tracks[current_selected_track].width = newWidth;
      comboVoices[current_selected_track].osca.pulseWidth(newWidth);
      comboVoices[current_selected_track].oscb.pulseWidth(newWidth);

      drawSequencerScreen();
    }
  } else if (current_page_selected == 2) {
    float currFilterAmt = currTrack.filterenvamt;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 150;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 10;
    }

    float newFilterAmt = currTrack.filterenvamt + (diff * mult * 0.005);

    if (!(newFilterAmt < -1.0 || newFilterAmt > 1.0) && newFilterAmt != currFilterAmt) {
      _seq_heap.pattern.tracks[current_selected_track].filterenvamt = newFilterAmt;

      AudioNoInterrupts();
      comboVoices[current_selected_track].dc.amplitude(newFilterAmt);
      AudioInterrupts();

      drawSequencerScreen();
    }
  } else if (current_page_selected == 3) {
    float curRel = currTrack.filter_release;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 200;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 20;
    }

    float newRel = currTrack.filter_release + (diff * mult);

    if (!(newRel < 0 || newRel > 11880) && newRel != curRel) {
      _seq_heap.pattern.tracks[current_selected_track].filter_release = newRel;

      AudioNoInterrupts();
      comboVoices[current_selected_track].filterEnv.release(newRel);
      AudioInterrupts();

      drawSequencerScreen();
    }
  } else if (current_page_selected == 4) {
    float curRel = currTrack.amp_release;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 200;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 20;
    }

    float newRel = currTrack.amp_release + (diff * mult);

    if (!(newRel < 0 || newRel > 11880) && newRel != curRel) {
      _seq_heap.pattern.tracks[current_selected_track].amp_release = newRel;

      AudioNoInterrupts();
      comboVoices[current_selected_track].ampEnv.release(newRel);
      AudioInterrupts();

      drawSequencerScreen();
    }
  } else if (current_page_selected == 5) {
      // n/a
  }
}

void handleEncoderRawSampleModA(int diff);
void handleEncoderRawSampleModA(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  if (current_page_selected == 0) {
    int newSampleId = currTrack.raw_sample_id + diff;

    Serial.print("newSampleId: ");
    Serial.println(newSampleId);

    if (newSampleId < 0) {
      newSampleId = (rawSamplesAvailable-1);
    } else if (newSampleId > (rawSamplesAvailable-1)) {
      newSampleId = 0;
    }

    Serial.print("rawSamplesAvailable: ");
    Serial.println(rawSamplesAvailable);

    Serial.print("newSampleId post: ");
    Serial.println(newSampleId);

    _seq_heap.pattern.tracks[current_selected_track].raw_sample_id = newSampleId;

    drawSequencerScreen();
  } else if (current_page_selected == 1) {

    Serial.print("currTrack.looptype pre: ");
    Serial.println(currTrack.looptype);

    int newLoopType = (currTrack.looptype + diff);

    Serial.print("newLoopType pre: ");
    Serial.println(newLoopType);

    if (newLoopType < 0) {
      newLoopType = 2;
    } else if (newLoopType > 2) {
      newLoopType = 0;
    }

    Serial.print("newLoopType post: ");
    Serial.println(newLoopType);

    if (newLoopType == 2) {
      _seq_heap.pattern.tracks[current_selected_track].chromatic_enabled = true;
    } else {
      _seq_heap.pattern.tracks[current_selected_track].chromatic_enabled = false;
    }

    _seq_heap.pattern.tracks[current_selected_track].looptype = newLoopType;

    drawSequencerScreen();
  } else if (current_page_selected == 2) {
    float currAtt = currTrack.amp_attack;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 100;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 10;
    }

    float newAtt = currTrack.amp_attack + (diff * mult);

    if (!(newAtt < 1 || newAtt > 11880) && newAtt != currAtt) {
      _seq_heap.pattern.tracks[current_selected_track].amp_attack = newAtt;

      if (current_selected_track > 3) {
        AudioNoInterrupts();
        sampleVoices[current_selected_track-4].ampEnv.attack(newAtt);
        AudioInterrupts();
      } else {
        AudioNoInterrupts();
        comboVoices[current_selected_track].ampEnv.attack(newAtt);
        AudioInterrupts();
      }

      drawSequencerScreen();
    }
  }else if (current_page_selected == 3) {
    float currLvl = currTrack.level;
    float newLvl = currTrack.level + (diff * 0.1);

    if (!(newLvl < -0.1 || newLvl > 1.1) && newLvl != currLvl) {
      _seq_heap.pattern.tracks[current_selected_track].level = newLvl;

      if (current_selected_track > 3) {
        AudioNoInterrupts();
        sampleVoices[current_selected_track-4].leftSubMix.gain(0, newLvl);
        sampleVoices[current_selected_track-4].leftSubMix.gain(1, newLvl);
        sampleVoices[current_selected_track-4].rightSubMix.gain(0, newLvl);
        sampleVoices[current_selected_track-4].rightSubMix.gain(1, newLvl);
        AudioInterrupts();
      } else {
        AudioNoInterrupts();
        comboVoices[current_selected_track].leftSubMix.gain(0, newLvl);
        comboVoices[current_selected_track].leftSubMix.gain(1, newLvl);
        comboVoices[current_selected_track].rightSubMix.gain(0, newLvl);
        comboVoices[current_selected_track].rightSubMix.gain(1, newLvl);
        AudioInterrupts();
      }

      drawSequencerScreen();
    }
  }
}

void handleEncoderRawSampleModB(int diff);
void handleEncoderRawSampleModB(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  if (current_page_selected == 0) {
    // sample speed adj
    float currSpeed = currTrack.sample_play_rate;
    float newSpeed = currSpeed + (diff * 0.1);

    if (!(newSpeed < -1.1 || newSpeed > 10.1) && newSpeed != currSpeed) {
      if ((currSpeed > 0.0 && newSpeed < 0.1) || (currSpeed == -1.0 && newSpeed < -1.0)) {
        newSpeed = -1.0;
      } else if (currSpeed <= -1.0 && newSpeed > -1.0) {
        newSpeed = 0.1;
      }

      _seq_heap.pattern.tracks[current_selected_track].sample_play_rate = newSpeed;
      
      if (current_selected_track > 3) {
        AudioNoInterrupts();
        sampleVoices[current_selected_track-4].rSample.setPlaybackRate(newSpeed);
        AudioInterrupts();
      } else {
        AudioNoInterrupts();
        comboVoices[current_selected_track].rSample.setPlaybackRate(newSpeed);
        AudioInterrupts();
      }

      drawSequencerScreen();
    }
  } else if (current_page_selected == 1) {
    uint32_t currLoopStart = currTrack.loopstart;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 100;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 10;
    }

    uint32_t newLoopStart = currLoopStart + (diff * mult);

    if (!(newLoopStart < 0) && newLoopStart != currLoopStart) {
      _seq_heap.pattern.tracks[current_selected_track].loopstart = newLoopStart;

      drawSequencerScreen();
    }
  } else if (current_page_selected == 2) {
    float currDecay = currTrack.amp_decay;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 100;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 10;
    }

    float newDecay = currTrack.amp_decay + (diff * mult);

    if (!(newDecay < 0 || newDecay > 11880) && newDecay != currDecay) {
      _seq_heap.pattern.tracks[current_selected_track].amp_decay = newDecay;

      if (current_selected_track > 3) {
        AudioNoInterrupts();
        sampleVoices[current_selected_track].ampEnv.decay(newDecay);
        AudioInterrupts();
      } else {
        AudioNoInterrupts();
        comboVoices[current_selected_track].ampEnv.decay(newDecay);
        AudioInterrupts();
      }

      drawSequencerScreen();
    }
  }else if (current_page_selected == 3) {
    float currPan = currTrack.pan;
    float newPan = currTrack.pan + (diff * 0.1);

    if (!(newPan < -1.0 || newPan > 1.0) && newPan != currPan) {
      _seq_heap.pattern.tracks[current_selected_track].pan = newPan;

      float newGainL = 1.0;
      if (newPan < 0) {
        newGainL += newPan;
      }

      float newGainR = 1.0;
      if (newPan > 0) {
        newGainR -= newPan;
      }

      if (current_selected_track > 3) {
        AudioNoInterrupts();
        sampleVoices[current_selected_track-4].leftCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));
        sampleVoices[current_selected_track-4].leftCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));
        AudioInterrupts();
      } else {
        AudioNoInterrupts();
        comboVoices[current_selected_track].leftCtrl.gain(getStereoPanValues(currTrack.pan).left * (currTrack.velocity * 0.01));
        comboVoices[current_selected_track].leftCtrl.gain(getStereoPanValues(currTrack.pan).right * (currTrack.velocity * 0.01));
        AudioInterrupts();
      }

      drawSequencerScreen();
    }
  }
}

void handleEncoderRawSampleModC(int diff);
void handleEncoderRawSampleModC(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  if (current_page_selected == 0) {
    // n/a
  } else if (current_page_selected == 1) {
    uint32_t currLoopFinish = currTrack.loopfinish;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 100;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 10;
    }

    uint32_t newLoopFinish = currLoopFinish + (diff * mult);
    

    if (!(newLoopFinish < 0) && newLoopFinish != currLoopFinish) {
      _seq_heap.pattern.tracks[current_selected_track].loopfinish = newLoopFinish;

      drawSequencerScreen();
    }
  } else if (current_page_selected == 2) {
    float curSus = currTrack.amp_sustain;
    float newSus = currTrack.amp_sustain + (diff * 0.01);

    if (!(newSus < 0 || newSus > 1.0) && newSus != curSus) {
      _seq_heap.pattern.tracks[current_selected_track].amp_sustain = newSus;

      if (current_selected_track > 3) {
        AudioNoInterrupts();
        sampleVoices[current_selected_track-4].ampEnv.sustain(newSus);
        AudioInterrupts();
      } else {
        AudioNoInterrupts();
        comboVoices[current_selected_track].ampEnv.sustain(newSus);
        AudioInterrupts();
      }

      drawSequencerScreen();
    }
  } else if (current_page_selected == 3) {
    // n/a
  }
}

void handleEncoderRawSampleModD(int diff);
void handleEncoderRawSampleModD(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  if (current_page_selected == 0) {
    uint8_t currBitrate = currTrack.bitrate;
    uint8_t newBitrate = currBitrate + diff;

    if (!(newBitrate < 1 || newBitrate > 16) && newBitrate != currBitrate) {
      _seq_heap.pattern.tracks[current_selected_track].bitrate = newBitrate;

      if (current_selected_track > 3) {
        AudioNoInterrupts();

        // sampleVoices[current_selected_track].leftBitCrush.bits(newBitrate);
        // sampleVoices[current_selected_track].rightBitCrush.bits(newBitrate);

        AudioInterrupts();

      } else {
        AudioNoInterrupts();

        // comboVoices[current_selected_track].leftBitCrush.bits(newBitrate);
        // comboVoices[current_selected_track].rightBitCrush.bits(newBitrate);

        AudioInterrupts();
      }

      drawSequencerScreen();
    }
  } else if (current_page_selected == 1) {
    int newPlayStart = (playStartFindMap[currTrack.playstart]) + diff;

    Serial.print("newPlayStart pre: ");
    Serial.println(newPlayStart);

    if (newPlayStart < 0) {
      newPlayStart = 1;
    } else if (newPlayStart > 1) {
      newPlayStart = 0;
    }

    Serial.print("newPlayStart post: ");
    Serial.println(newPlayStart);

    play_start playStartSel = playStartSelMap[newPlayStart];

    _seq_heap.pattern.tracks[current_selected_track].playstart = playStartSel;

    drawSequencerScreen();
  } else if (current_page_selected == 2) {
    float curRel = currTrack.amp_release;

    int mult = 1;
    if (abs(diff) > 5) {
      mult = 200;
    } else if (abs(diff) > 1 && abs(diff) < 5) {
      mult = 20;
    }

    float newRel = currTrack.amp_release + (diff * mult);

    if (!(newRel < 0 || newRel > 11880) && newRel != curRel) {
      _seq_heap.pattern.tracks[current_selected_track].amp_release = newRel;
    
      if (current_selected_track > 3) {
        AudioNoInterrupts();
        sampleVoices[current_selected_track-4].ampEnv.release(newRel);
        AudioInterrupts();
      } else {
        AudioNoInterrupts();
        comboVoices[current_selected_track].ampEnv.release(newRel);
        AudioInterrupts();
      }

      drawSequencerScreen();
    }
  }else if (current_page_selected == 3) {
    // n/a
  }
  
}

void handleEncoderWavSampleModA(int diff);
void handleEncoderWavSampleModA(int diff)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  int newSampleId = currTrack.wav_sample_id + diff;

  if (newSampleId < 0) {
    newSampleId = (wavSamplesAvailable-1);
  } else if (newSampleId > (wavSamplesAvailable-1)) {
    newSampleId = 0;
  }

  _seq_heap.pattern.tracks[current_selected_track].wav_sample_id = newSampleId;

  drawSequencerScreen();
}

void handleEncoderWavSampleModB(int diff);
void handleEncoderWavSampleModB(int diff)
{
  //
}

void handleEncoderWavSampleModC(int diff);
void handleEncoderWavSampleModC(int diff)
{
  //
}

void handleEncoderWavSampleModD(int diff);
void handleEncoderWavSampleModD(int diff)
{
  //
}

void handleEncoderSetTrackMods(void);
void handleEncoderSetTrackMods(void)
{
  TRACK currTrack = getHeapCurrentSelectedTrack();

  const int modCount = 4;
  for (int m = 0; m < modCount; m++)
  {
    int modEncoderIdx = m+1;

    encoder_currValues[modEncoderIdx] = encoder_getValue(encoder_addrs[modEncoderIdx]);

    if(encoder_currValues[modEncoderIdx] != encoder_lastValues[modEncoderIdx]) {
      int diff = encoder_currValues[modEncoderIdx] - encoder_lastValues[modEncoderIdx];

      if (currTrack.track_type == SUBTRACTIVE_SYNTH) {
        if (m == 0) {
          handleEncoderSubtractiveSynthModA(diff);
        } else if (m == 1) {
          handleEncoderSubtractiveSynthModB(diff);
        } else if (m == 2) {
          handleEncoderSubtractiveSynthModC(diff);
        } else if (m == 3) {
          handleEncoderSubtractiveSynthModD(diff);
        }
      } else if (currTrack.track_type == RAW_SAMPLE) {
        if (m == 0) {
          handleEncoderRawSampleModA(diff);
        } else if (m == 1) {
          handleEncoderRawSampleModB(diff);
        } else if (m == 2) {
          handleEncoderRawSampleModC(diff);
        } else if (m == 3) {
          handleEncoderRawSampleModD(diff);
        }
      } else if (currTrack.track_type == WAV_SAMPLE) {
        if (m == 0) {
          handleEncoderWavSampleModA(diff);
        } else if (m == 1) {
          handleEncoderWavSampleModB(diff);
        } else if (m == 2) {
          handleEncoderWavSampleModC(diff);
        } else if (m == 3) {
          handleEncoderWavSampleModD(diff);
        }
      }

      encoder_lastValues[modEncoderIdx] = encoder_currValues[modEncoderIdx];
    }
  }
}

void handleEncoderTraversePages(void);
void handleEncoderTraversePages(void)
{
  int main_encoder_idx = 0;
  encoder_currValues[main_encoder_idx] = encoder_getValue(encoder_addrs[main_encoder_idx]);

  if(encoder_currValues[main_encoder_idx] != encoder_lastValues[main_encoder_idx]) {
    int diff = encoder_currValues[main_encoder_idx] - encoder_lastValues[main_encoder_idx];
    int newPage = current_page_selected + diff;

    if (newPage != current_page_selected) {
      TRACK currTrack = getHeapCurrentSelectedTrack();
      int maxPagesForCurrTrack = trackPageNumMap[currTrack.track_type];

      if (maxPagesForCurrTrack == 1) {
        return;
      }

      if (newPage > (maxPagesForCurrTrack-1)) {
        newPage = 0;
      } else if (newPage < 0) {
        newPage = maxPagesForCurrTrack-1;
      }

      current_page_selected = newPage;

      drawSequencerScreen();
    }

    encoder_lastValues[main_encoder_idx] = encoder_currValues[main_encoder_idx];
  }
}

void handleEncoderStates() {
  // slow this down from being called every loop when in set tempo mode
  if (!(elapsed % 50) && current_UI_mode == SET_TEMPO) {
    handleEncoderSetTempo();
  } else if (!(elapsed % 50) && (current_UI_mode == TRACK_WRITE || current_UI_mode == SUBMITTING_STEP_VALUE)) {
    if (current_UI_mode == TRACK_WRITE) {
      handleEncoderTraversePages();
    }

    handleEncoderSetTrackMods();
  }
}

void handleKeyboardStates(void) {
  // Get the currently touched pads
  currtouched = cap.touched();

  int8_t invertedNoteNumber = -1;
  bool released = false;

  for (uint8_t i=0; i<12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      invertedNoteNumber = backwardsNoteNumbers[i];

      note_on_keyboard = invertedNoteNumber;
      if (keyboardNotesHeld < 5) keyboardNotesHeld++;

      Serial.print(i); Serial.println(" touched");

      if (current_UI_mode != UI_MODE::SUBMITTING_STEP_VALUE) {
        triggerTrackManually(current_selected_track, invertedNoteNumber);
      }
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      invertedNoteNumber = backwardsNoteNumbers[i]; // need this?

      if (keyboardNotesHeld > 0) keyboardNotesHeld--;

      Serial.print(i);
      Serial.println(" released");

      released = true;

      if (current_UI_mode != UI_MODE::SUBMITTING_STEP_VALUE) {
        TRACK currSelTrack = getHeapCurrentSelectedTrack();

        if (keyboardNotesHeld == 0) {
          if (current_selected_track > 3) {
            sampleVoices[current_selected_track-4].ampEnv.noteOff();
          } else {
            comboVoices[current_selected_track].ampEnv.noteOff();
            if (currSelTrack.track_type == TRACK_TYPE::SUBTRACTIVE_SYNTH) {
              comboVoices[current_selected_track].filterEnv.noteOff();
            }
          }
        }
      }
    }
  }

  if (!fastBtnPressed && fastTouchRead(32) >= 64) {
    invertedNoteNumber = backwardsNoteNumbers[12];
    fastBtnPressed = true;

    note_on_keyboard = invertedNoteNumber;

    if (keyboardNotesHeld < 5) keyboardNotesHeld++;

    std::string touchedStr = "Touched key: 12";
    Serial.println(touchedStr.c_str());

    if (current_UI_mode != SUBMITTING_STEP_VALUE) {
      triggerTrackManually(current_selected_track, invertedNoteNumber);
    }

  } else if (fastBtnPressed && fastTouchRead(32) < 64) {
    invertedNoteNumber = backwardsNoteNumbers[12];

    fastBtnPressed = false;
    released = true;
  
    note_on_keyboard = invertedNoteNumber;

    if (keyboardNotesHeld > 0) keyboardNotesHeld--;

    std::string releasedStr = "Released key: 12";
    Serial.println(releasedStr.c_str());
  }

  // released a key button, so modify a step's note number if also holding a step button
  if (released && invertedNoteNumber > -1) {
    if (current_UI_mode == SUBMITTING_STEP_VALUE && current_selected_step > -1) {
      _seq_heap.pattern.tracks[current_selected_track].steps[current_selected_step].note = invertedNoteNumber;
      _seq_heap.pattern.tracks[current_selected_track].steps[current_selected_step].octave = keyboardOctave;

      drawSequencerScreen();
    } else if (current_UI_mode == TRACK_SEL) {
      _seq_heap.pattern.tracks[current_selected_track].note = invertedNoteNumber;
      _seq_heap.pattern.tracks[current_selected_track].octave = keyboardOctave;

      drawSequencerScreen();
    } else {
      TRACK currSelTrack = getHeapCurrentSelectedTrack();

      if (keyboardNotesHeld == 0) {
        if (current_selected_track > 3) {
          sampleVoices[current_selected_track-4].ampEnv.noteOff();
        } else {
          comboVoices[current_selected_track].ampEnv.noteOff();
          if (currSelTrack.track_type == TRACK_TYPE::SUBTRACTIVE_SYNTH) {
            comboVoices[current_selected_track].filterEnv.noteOff();
          }
        }
      }
    }
  }

  // reset our state
  lasttouched = currtouched;
}

void handleSwitchStates(bool discard) {
  // Fills kpd.key[ ] array with up-to 10 active keys.
  // Returns true if there are ANY active keys.
  if (kpd.getKeys())
  {
    for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
    {
      if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
      {
        switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
          case PRESSED: 
            {
              Serial.print("button pressed: ");
              Serial.println(kpd.key[i].kchar);

              if (!function_started) {
                // track select
                if (kpd.key[i].kchar == 'c') {
                  Serial.println("enter track select mode!");
                  current_UI_mode = TRACK_SEL;

                  clearAllStepLEDs();
                  displayCurrentlySelectedTrack();
                } else if (current_UI_mode == TRACK_SEL && btnCharIsATrack(kpd.key[i].kchar)) {
                  uint8_t selTrack = getKeyStepNum(kpd.key[i].kchar)-1; // zero-based
                  current_selected_track = selTrack;

                  Serial.print("marking pressed track selection (zero-based): ");
                  Serial.println(selTrack);

                  track_held_for_selection = selTrack;

                  previous_UI_mode = TRACK_SEL;
                  current_UI_mode = TRACK_SEL;

                  clearAllStepLEDs();
                  displayCurrentlySelectedTrack();
                } else if (current_UI_mode == TRACK_SEL && kpd.key[i].kchar == SOUND_SETUP_BTN_CHAR) {
                  TRACK_TYPE currType = _seq_heap.pattern.tracks[current_selected_track].track_type;
                  
                  Serial.print("changing ");
                  Serial.print(current_selected_track+1);
                  Serial.print(" track sound for pattern ");
                  Serial.print(current_selected_pattern+1);
                  Serial.print(" to sound type: ");
                  Serial.println(currType);

                  TRACK_TYPE newType = current_selected_track > 3 ? RAW_SAMPLE : SUBTRACTIVE_SYNTH;
                  if (currType == SUBTRACTIVE_SYNTH) {
                    newType = RAW_SAMPLE;
                  } else if (currType == RAW_SAMPLE) {
                    newType = WAV_SAMPLE;
                  } else if (currType == WAV_SAMPLE) {
                    newType = MIDI;
                  } else if (currType == MIDI) {
                    newType = CV_GATE;
                  } else if (currType == CV_GATE) {
                    newType = CV_TRIG;
                  }

                  // reset page to 0
                  current_page_selected = 0;

                  changeTrackSoundType(current_selected_track, newType);

                  drawSequencerScreen();
                }
                // pattern select
                else if (kpd.key[i].kchar == 'b') {
                  Serial.println("enter pattern select mode!");
                  current_UI_mode = PATTERN_SEL;

                  clearAllStepLEDs();
                  displayCurrentlySelectedPattern();
                } else if (current_UI_mode == PATTERN_SEL && btnCharIsATrack(kpd.key[i].kchar)) {
                  uint8_t selPattern = getKeyStepNum(kpd.key[i].kchar)-1; // zero-based
                  current_selected_pattern = selPattern;

                  Serial.print("marking pressed pattern selection (zero-based): ");
                  Serial.println(selPattern);

                  patt_held_for_selection = selPattern;

                  previous_UI_mode = PATTERN_SEL;
                  current_UI_mode = PATTERN_SEL;

                  clearAllStepLEDs();
                  displayCurrentlySelectedPattern();
                } 
                // start/pause or stop
                else if (kpd.key[i].kchar == 'q' || kpd.key[i].kchar == 'w') {
                  toggleSequencerPlayback(kpd.key[i].kchar);
                  drawSequencerScreen();
                }
                // octave
                else if (kpd.key[i].kchar == '9' || kpd.key[i].kchar == '3') {
                  Serial.print("btn: ");
                  Serial.println(kpd.key[i].kchar);

                  // 1-7 octave range e.g. [1,2,3,4,5,6,7]
                  // LEDs 17-20
                  if (kpd.key[i].kchar == '3') {
                    keyboardOctave = min(7, keyboardOctave+1);

                  } else if (kpd.key[i].kchar == '9') {
                    keyboardOctave = max(1, keyboardOctave-1);
                  }

                  switch (keyboardOctave)
                  {
                  case 1:
                    setLEDPWM(17, 100);
                    setLEDPWM(18, 25);
                    setLEDPWM(19, 0);
                    setLEDPWM(20, 0);
                    break;
                  
                  case 2:
                    setLEDPWM(17, 25);
                    setLEDPWM(18, 25);
                    setLEDPWM(19, 0);
                    setLEDPWM(20, 0);
                    break;
                  
                  case 3:
                    Serial.println("here");
                    setLEDPWM(17, 0);
                    setLEDPWM(18, 25);
                    setLEDPWM(19, 0);
                    setLEDPWM(20, 0);
                    break;
                  
                  case 4:
                    setLEDPWM(17, 0);
                    setLEDPWM(18, 0);
                    setLEDPWM(19, 0);
                    setLEDPWM(20, 0);
                    break;
                  
                  case 5:
                    setLEDPWM(17, 0);
                    setLEDPWM(18, 0);
                    setLEDPWM(19, 25);
                    setLEDPWM(20, 0);
                    break;
                  
                  case 6:
                    setLEDPWM(17, 0);
                    setLEDPWM(18, 0);
                    setLEDPWM(19, 25);
                    setLEDPWM(20, 25);
                    break;
                  
                  case 7:
                    setLEDPWM(17, 0);
                    setLEDPWM(18, 0);
                    setLEDPWM(19, 25);
                    setLEDPWM(20, 100);
                    break;
                  
                  default:
                    break;
                  }

                  Serial.print("Updated octave to: ");
                  Serial.println(keyboardOctave);
                }
              }
              // function handling
              else {
                Serial.println("triggering function!");
                // change track sound type
                if (current_UI_mode == TRACK_WRITE && kpd.key[i].kchar == SOUND_SETUP_BTN_CHAR) {
                  Serial.println("draw setup screen!");
                } else if (current_UI_mode == TRACK_WRITE && kpd.key[i].kchar == 'c') {
                  // TODO: impl layer mechanic
                }
              }

              break;
            }
          case HOLD:
            {
              // param lock step
              if (current_UI_mode == TRACK_WRITE && btnCharIsATrack(kpd.key[i].kchar)) {
                // editing a step value / parameter locking this step
                previous_UI_mode = TRACK_WRITE;
                current_UI_mode = SUBMITTING_STEP_VALUE;  

                current_selected_step = getKeyStepNum(kpd.key[i].kchar)-1;

                TRACK_STEP heldStep = _seq_heap.pattern.tracks[current_selected_track].steps[current_selected_step];

                // only toggle held step ON if initially in the OFF position,
                // so that holding / param locking doesn't turn the step off
                if (heldStep.state == OFF) {
                  Serial.println("toggling step!");    

                  uint8_t stepToToggle = getKeyStepNum(kpd.key[i].kchar);

                  toggleSelectedStep(stepToToggle);
                  setDisplayStateForAllStepLEDs(); 
                }

                drawSequencerScreen();
              }
              // tempo set
              else if ((current_UI_mode != PATTERN_SEL || current_UI_mode != TRACK_SEL) && kpd.key[i].kchar == '4') {
                Serial.println("enter tempo set mode!");
                current_UI_mode = SET_TEMPO;

                drawSetTempoOverlay();
              }
              // function
              else if (kpd.key[i].kchar == FUNCTION_BTN_CHAR) {
                Serial.println("starting function!");
                function_started = true;
              }

              break;
            }
          case RELEASED:
            {
              // track select / write release
              if (current_UI_mode == TRACK_SEL && kpd.key[i].kchar == 'c' && track_held_for_selection == -1) {      
                current_UI_mode = TRACK_WRITE; // force track write mode when leaving track / track select action
                previous_UI_mode = TRACK_WRITE;

                setDisplayStateForAllStepLEDs();
                drawSequencerScreen();
              } else if (current_UI_mode == TRACK_SEL && btnCharIsATrack(kpd.key[i].kchar) && ((getKeyStepNum(kpd.key[i].kchar)-1) == track_held_for_selection)) {   
                Serial.println("unmarking track as held for selection!");   

                current_selected_track = getKeyStepNum(kpd.key[i].kchar)-1;
                track_held_for_selection = -1;
                clearAllStepLEDs();

                current_page_selected = 0;

                current_UI_mode = TRACK_WRITE; // force track write mode when leaving track / track select action
                previous_UI_mode = TRACK_WRITE;

                setDisplayStateForAllStepLEDs();
                drawSequencerScreen();
              } else if (current_UI_mode == TRACK_WRITE && btnCharIsATrack(kpd.key[i].kchar) && track_held_for_selection == -1) {   
                Serial.println("toggling step!");         
                // toggle the step state on release
                uint8_t stepToToggle = getKeyStepNum(kpd.key[i].kchar);

                toggleSelectedStep(stepToToggle);
                setDisplayStateForAllStepLEDs();
              } 

              // pattern select / write release              
              else if (current_UI_mode == PATTERN_SEL && kpd.key[i].kchar == 'b' && patt_held_for_selection == -1) {      
                current_UI_mode = PATTERN_WRITE; // force patt write mode when leaving patt / patt select action
                previous_UI_mode = PATTERN_WRITE;

                clearAllStepLEDs();
                drawSequencerScreen();
              } else if (current_UI_mode == PATTERN_SEL && btnCharIsATrack(kpd.key[i].kchar) && ((getKeyStepNum(kpd.key[i].kchar)-1) == patt_held_for_selection)) {   
                Serial.println("unmarking pattern as held for selection!");   

                current_selected_pattern = getKeyStepNum(kpd.key[i].kchar)-1;
                patt_held_for_selection = -1;
                clearAllStepLEDs();

                current_UI_mode = PATTERN_WRITE; // force patt write mode when leaving patt / patt select action
                previous_UI_mode = PATTERN_WRITE;

                drawSequencerScreen();
              }
              
              // tempo set release
              else if (current_UI_mode == SET_TEMPO) {
                Serial.print(" reverting set tempo mode, entering: ");
                Serial.println(previous_UI_mode);
                // revert back to a prior write mode, not a hold mode
                current_UI_mode = previous_UI_mode;

                drawSequencerScreen();
              } 

              // param lock release
              else if (current_UI_mode == SUBMITTING_STEP_VALUE && btnCharIsATrack(kpd.key[i].kchar)) {
                Serial.print(" reverting submitting step value / param lock mode, re-entering: ");
                Serial.println(previous_UI_mode == TRACK_WRITE ? "track write mode!" : "??? mode!");

                // revert
                current_UI_mode = previous_UI_mode;
                previous_UI_mode = SUBMITTING_STEP_VALUE;
                current_selected_step = -1; // unselect the step

                drawSequencerScreen();
              }

              // function stop
              else if (function_started && kpd.key[i].kchar == FUNCTION_BTN_CHAR) {
                Serial.println("leaving function!");
                function_started = false;
              }

              break;
            }
          case IDLE:
            {
              //
              break;
            }
        }
      }
    }
  }
}

void encoder_set(int addr, int16_t rmin, int16_t rmax, int16_t rstep, int16_t rval, uint8_t rloop) {
  Wire1.beginTransmission(addr);
  Wire1.write((uint8_t)(rval & 0xff)); Wire1.write((uint8_t)(rval >> 8));
  Wire1.write(0); Wire1.write(rloop);
  Wire1.write((uint8_t)(rmin & 0xff)); Wire1.write((uint8_t)(rmin >> 8));
  Wire1.write((uint8_t)(rmax & 0xff)); Wire1.write((uint8_t)(rmax >> 8));
  Wire1.write((uint8_t)(rstep & 0xff)); Wire1.write((uint8_t)(rstep >> 8));
  Wire1.endTransmission();
}

void encoder_setValue(int addr, int16_t rval) {
  Wire1.beginTransmission(addr);
  Wire1.write((uint8_t)(rval & 0xff)); Wire1.write((uint8_t)(rval >> 8));
  Wire1.endTransmission();
}

void initEncoders() {
  for (int i=0; i<5; i++) {
    encoder_set(encoder_addrs[i], -1000, 1000, 1, 0, 0);
  }
}

int16_t encoder_getValue(int addr) {
  Wire1.requestFrom(addr, 2);
  return((uint16_t)Wire1.read() | ((uint16_t)Wire1.read() << 8));
}

void u8g2_prepare(void) {
  u8g2.setFont(small_font);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

void setLEDPWM (uint8_t lednum, uint16_t pwm)
{
    tlc.setPWM(lednum, pwm);
    tlc.write();  
}

void setLEDPWMDouble (uint8_t lednum1, uint16_t pwm1, uint8_t lednum2, uint16_t pwm2)
{
    tlc.setPWM(lednum1, pwm1);
    tlc.setPWM(lednum2, pwm2);
    tlc.write();  
}

void writeToDAC(int chip, int chan, int val){
  byte lVal = val;
  byte hVal = (val >> 8);

	SPI1.setDataMode(SPI_MODE0);
  digitalWrite(chip, LOW);

  if (val == 0x000) {
    // use low gain
    if(chan == 0){ 
      SPI1.transfer(hVal | chan_a_low_gain);
    }else{
      SPI1.transfer(hVal | chan_b_low_gain);
    }
  } else {
    // use high gain
    if(chan == 0){ 
      SPI1.transfer(hVal | chan_a_high_gain);
    }else{
      SPI1.transfer(hVal | chan_b_high_gain);
    }
  }

  SPI1.transfer(lVal);
  digitalWrite(chip, HIGH);
}

void initDACs()
{
  // initialize DAC outputs at 0V
  writeToDAC(CS1, 0, 0x000);
  writeToDAC(CS1, 1, 0x000);

  // initialize DAC outputs at 0V
  writeToDAC(CS2, 0, 0x000);
  writeToDAC(CS2, 1, 0x000);

  // initialize DAC outputs at 0V
  writeToDAC(CS3, 0, 0x000);
  writeToDAC(CS3, 1, 0x000);

  // initialize DAC outputs at 0V
  writeToDAC(CS4, 0, 0x000);
  writeToDAC(CS4, 1, 0x000);
}

void handleHeadphoneAdjustment(void)
{
  if (analog.hasChanged()) {
    float newValue = (float)analog.getValue() / (float)1023;

    if (abs(hp_vol_cur - newValue) >= 0.05) {
      
      // Serial.print("hp_vol_cur: ");
      // Serial.println(hp_vol_cur);

      hp_vol_cur = newValue;

      // Serial.print("newValue: ");
      // Serial.println(newValue);

      sgtl5000_1.volume(hp_vol_cur);
    }
  }
}