#include <Arduino.h>
#include <U8g2lib.h>

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