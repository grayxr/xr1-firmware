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

#if F_CPU == (24000000L) //24mhz
#define UNLOADED_IDLE_COUNTS 152000
#elif F_CPU == (48000000L) //48mhz
#define UNLOADED_IDLE_COUNTS 280000
#elif F_CPU == (72000000L) //72mhz
#define UNLOADED_IDLE_COUNTS 360000
#elif F_CPU == (600000000L) //600mhz
#define UNLOADED_IDLE_COUNTS 13600000
#else //96mhz
#define UNLOADED_IDLE_COUNTS 420000
#endif

//set cpu percent scale to a linear one
#define LOG 0
#define LIN 1
#define SCALE LIN

static IntervalTimer cpu;

uint32_t Idle_Counter;
uint8_t CPU_Utilization_Info_Read_To_Compute;
uint32_t Prev_Idle_Counter;
uint32_t Idle_Counts;
uint32_t Calculate_Idle_Counts (void);
bool One_MS_Task_Ready;
bool Ten_MS_Task_Ready;
bool One_Hundred_MS_Task_Ready;
bool One_S_Task_Ready;

extern unsigned long _heap_start;
extern unsigned long _heap_end;
extern char *__brkval;

extern "C" char* sbrk(int incr);
int freeram() {
  return (char *)&_heap_end - __brkval;
}

// GUItool: begin automatically generated code
AudioPlayArrayResmp      rraw_a1;  //xy=502,248
AudioPlayArrayResmp      rraw_a2;  //xy=605,248
AudioPlayArrayResmp      rraw_a3;  //xy=705,248
AudioPlayArrayResmp      rraw_a4;  //xy=705,248
AudioMixer4              mixer1;         //xy=638,399
AudioMixer4              mixer2;         //xy=638,399
AudioMixer4              mixer3;         //xy=638,399
AudioOutputI2S           audioOutput;           //xy=787,407
AudioConnection          patchCord3(rraw_a1, 0, mixer1, 0);
AudioConnection          patchCord4(rraw_a1, 0, mixer1, 1);
AudioConnection          patchCord1(rraw_a2, 0, mixer1, 2);
AudioConnection          patchCord2(rraw_a2, 0, mixer1, 3);
AudioConnection          patchCord5(rraw_a3, 0, mixer2, 0);
AudioConnection          patchCord6(rraw_a3, 0, mixer2, 1);
AudioConnection          patchCord13(rraw_a4, 0, mixer2, 2);
AudioConnection          patchCord14(rraw_a4, 0, mixer2, 3);
AudioConnection          patchCord7(mixer1, 0, mixer3, 0);
AudioConnection          patchCord8(mixer1, 0, mixer3, 1);
AudioConnection          patchCord9(mixer2, 0, mixer3, 2);
AudioConnection          patchCord10(mixer2, 0, mixer3, 3);
AudioConnection          patchCord11(mixer3, 0, audioOutput, 0);
AudioConnection          patchCord12(mixer3, 0, audioOutput, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=793,542
// GUItool: end automatically generated code

unsigned long lastSamplePlayed = 0;
newdigate::audiosample *sample;
newdigate::audiosample *sample2;
newdigate::audiosample *sample2a;
newdigate::audiosample *sample3;
newdigate::audiosample *sample4;


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

int8_t stepLEDPins[16] = {
  0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15, 16
};

std::map<uint8_t, bool> activeLEDMap = {
  {0, false},
  {1, false},
  {2, false},
  {3, false},
  {4, false},
  {5, false},
  {6, false},
  {7, false},
  {8, false},
  {9, false},
  {10, false},
  {11, false},
  {12, false},
  {13, false},
  {14, false},
  {15, false},
  {16, false},
  {17, false},
  {18, false},
  {19, false},
  {20, false},
  {21, false},
  {22, false},
};

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int encoder_addrs[5] = {
  0x36, 0x37, 0x38, 0x39, 0x40
};

int16_t encoder_currValues[5] = {
  0, 0, 0, 0, 0
};

int16_t encoder_lastValues[5] = {
  0, 0, 0, 0, 0
};

unsigned long loopCount;
unsigned long startTime;
const char *msg;

uint8_t bpm_blink_timer = 1;

bool _running = false;

bool _testUI = false;
bool _checkTestFn = false;

uint8_t draw_state = 0;

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





// Sequencer data

enum TRACK_TYPE {
  MIDI = 0,
  CV_TRIG = 1,
  CV_GATE = 2,
  SAMPLE = 3,
  SYNTH = 4
};

enum TRACK_STEP_STATE {
  OFF = 0,
  ON = 1,
  ACCENTED = 2
};

typedef struct
{
  TRACK_STEP_STATE state = OFF;
  uint8_t note = 0;
  int8_t length = 1;
  uint8_t micro_timing = 0;
  bool glide = false;
} TRACK_STEP;

typedef struct
{
  TRACK_TYPE track_type = MIDI;
  TRACK_STEP steps[64];
  int8_t last_step = 16;
  int8_t sample_id = 0;
  // scale attr
  // shuffle attr
} TRACK;

typedef struct
{
  TRACK tracks[16];
  // scale attr
  // shuffle attr
} PATTERN;

typedef struct
{
  PATTERN patterns[16];
} BANK;

typedef struct
{
  BANK banks[1];
} SEQUENCER;

enum SEQUENCER_PLAYBACK_STATE {
  STOPPED = 0,
  RUNNING = 1,
  PAUSED = 2
};

typedef struct
{
  SEQUENCER seq;
  SEQUENCER_PLAYBACK_STATE playback_state = STOPPED;
  int8_t current_step = 1;
} SEQUENCER_STATE;

SEQUENCER_STATE _seq_state;

const int numChannels = 1; // 1 for mono, 2 for stereo...

enum UI_MODE {
  BANK_WRITE,
  BANK_SEL,
  PATTERN_WRITE, // The default mode
  PATTERN_SEL,
  TRACK_WRITE,
  TRACK_SEL,
  SET_TEMPO,
};

UI_MODE previous_UI_mode = UI_MODE::PATTERN_WRITE; // the default mode
UI_MODE current_UI_mode = UI_MODE::PATTERN_WRITE; // the default mode

int8_t current_selected_pattern = 0;
int8_t current_selected_track = 0;

const uint8_t *small_font = u8g2_font_trixel_square_tf;

elapsedMillis elapsed;

void setLEDPWM(uint8_t lednum, uint16_t pwm);
void setLEDPWMDouble(uint8_t lednum1, uint16_t pwm1, uint8_t lednum2, uint16_t pwm2);
void testLEDs();
void u8g2_prepare(void);
void drawEvent(std::string event, std::string eventAddl, bool erase);
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
void toggleLED(uint8_t num, bool discard);
void toggleSequencerPlayback(char btn);
void handle_bpm_step(uint32_t tick);
void triggerAllStepsForGlobalStep(void);
void initTest(void);
void initMain(void);
void drawSequencerScreen(void);
void drawSetTempoOverlay(void);
void playFile(uint8_t num, bool accented);
void stopFiles(void);
void setDisplayStateForAllStepLEDs(void);
void setDisplayStateForPatternActiveTracksLEDs(bool enable);
bool selectedBtnCharIsATrack(char btnChar);
void toggleSelectedStep(uint8_t step);
void clearAllStepLEDs(void);
void displayCurrentlySelectedPattern(void);
void displayCurrentlySelectedTrack(void);
void handleDisplayModeLEDs(void);
void Update_Task_Ready_Flags(void);

void playFile(uint8_t num, bool accented)
{
  if (num == 0) {
    rraw_a1.playRaw(sample->sampledata, sample->samplesize/2, numChannels);
  } else if (num == 1) {
    if (accented) {
      rraw_a2.playRaw(sample2a->sampledata, sample2a->samplesize/2, numChannels);
    } else {
      rraw_a2.playRaw(sample2->sampledata, sample2->samplesize/2, numChannels);
    }
  } else if (num == 2) {
    rraw_a3.playRaw(sample3->sampledata, sample3->samplesize/2, numChannels);
  } else if (num == 3) {
    rraw_a4.playRaw(sample4->sampledata, sample4->samplesize/2, numChannels);
  }

  // Serial.print("Memory: ");
  // Serial.print(AudioMemoryUsage());
  // Serial.print(",");
  // Serial.print(AudioMemoryUsageMax());
  // Serial.println();
}

void stopFiles()
{
  rraw_a1.stop();
  rraw_a2.stop();
  rraw_a3.stop();
  rraw_a4.stop();
  delay(10);
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
}

void setup() {
  Serial.begin(9600);

  if (CrashReport) {
    Serial.print(CrashReport);
  }

  cpu.begin(Update_Task_Ready_Flags, 1000);

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(30);

  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  // pullup resistors
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.7);

  // bd & sd
  mixer1.gain(0, 0.5);
  mixer1.gain(1, 0.5);
  mixer1.gain(2, 0.7);
  mixer1.gain(3, 0.7);
  // lt
  mixer2.gain(2, 0.5);
  mixer2.gain(3, 0.5);

  rraw_a1.setPlaybackRate(1);
  rraw_a1.enableInterpolation(true);

  rraw_a2.setPlaybackRate(1);
  rraw_a2.enableInterpolation(true);

  rraw_a3.setPlaybackRate(1);
  rraw_a3.enableInterpolation(true);

  rraw_a4.setPlaybackRate(1);
  rraw_a4.enableInterpolation(true);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  newdigate::flashloader loader;
  sample = loader.loadSample("_BD.RAW");
  sample2 = loader.loadSample("_SD.RAW");
  sample2a = loader.loadSample("_SD_ACC.RAW");
  sample3 = loader.loadSample("_CH.RAW");
  sample4 = loader.loadSample("_LT.RAW");
  
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

  loopCount = 0;
  startTime = millis();
  msg = "";

  // initialize encoders
  initEncoders();

  kpd.setHoldTime(150);

  for (int i=0; i<25; i++) {
    setLEDPWM(i, 0);    //sets all 24 outputs to no brightness
  }

  delay(100);

  _checkTestFn = true;

  // discard any dirty reads
  handleSwitchStates(true);

  u8g2_prepare();

  if (_testUI) {
    initTest();
  } else {
    initMain();
  }

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

  Serial.println("fill test sequencer out");

  // build test sequencer out with basic data

  _seq_state.seq.banks[0].patterns[0].tracks[0].track_type = TRACK_TYPE::SAMPLE;
  _seq_state.seq.banks[0].patterns[0].tracks[0].sample_id = 0;
  _seq_state.seq.banks[0].patterns[0].tracks[0].steps[0].state = TRACK_STEP_STATE::ON;
  _seq_state.seq.banks[0].patterns[0].tracks[0].steps[1].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[0].steps[2].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[0].steps[3].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[0].steps[4].state = TRACK_STEP_STATE::ON;
  _seq_state.seq.banks[0].patterns[0].tracks[0].steps[5].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[0].steps[6].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[0].steps[7].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[0].steps[8].state = TRACK_STEP_STATE::ON;
  _seq_state.seq.banks[0].patterns[0].tracks[0].steps[9].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[0].steps[10].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[0].steps[11].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[0].steps[12].state = TRACK_STEP_STATE::ON;
  _seq_state.seq.banks[0].patterns[0].tracks[0].steps[13].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[0].steps[14].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[0].steps[15].state = TRACK_STEP_STATE::OFF;

  _seq_state.seq.banks[0].patterns[0].tracks[1].track_type = TRACK_TYPE::SAMPLE;
  _seq_state.seq.banks[0].patterns[0].tracks[1].sample_id = 1;
  _seq_state.seq.banks[0].patterns[0].tracks[1].steps[0].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[1].steps[1].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[1].steps[2].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[1].steps[3].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[1].steps[4].state = TRACK_STEP_STATE::ON;
  _seq_state.seq.banks[0].patterns[0].tracks[1].steps[5].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[1].steps[6].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[1].steps[7].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[1].steps[8].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[1].steps[9].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[1].steps[10].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[1].steps[11].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[1].steps[12].state = TRACK_STEP_STATE::ON;
  _seq_state.seq.banks[0].patterns[0].tracks[1].steps[13].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[1].steps[14].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[1].steps[15].state = TRACK_STEP_STATE::OFF;

  _seq_state.seq.banks[0].patterns[0].tracks[2].track_type = TRACK_TYPE::SAMPLE;
  _seq_state.seq.banks[0].patterns[0].tracks[2].sample_id = 2;
  _seq_state.seq.banks[0].patterns[0].tracks[2].steps[0].state = TRACK_STEP_STATE::ON;
  _seq_state.seq.banks[0].patterns[0].tracks[2].steps[1].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[2].steps[2].state = TRACK_STEP_STATE::ON;
  _seq_state.seq.banks[0].patterns[0].tracks[2].steps[3].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[2].steps[4].state = TRACK_STEP_STATE::ON;
  _seq_state.seq.banks[0].patterns[0].tracks[2].steps[5].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[2].steps[6].state = TRACK_STEP_STATE::ON;
  _seq_state.seq.banks[0].patterns[0].tracks[2].steps[7].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[2].steps[8].state = TRACK_STEP_STATE::ON;
  _seq_state.seq.banks[0].patterns[0].tracks[2].steps[9].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[2].steps[10].state = TRACK_STEP_STATE::ON;
  _seq_state.seq.banks[0].patterns[0].tracks[2].steps[11].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[2].steps[12].state = TRACK_STEP_STATE::ON;
  _seq_state.seq.banks[0].patterns[0].tracks[2].steps[13].state = TRACK_STEP_STATE::OFF;
  _seq_state.seq.banks[0].patterns[0].tracks[2].steps[14].state = TRACK_STEP_STATE::ON;
  _seq_state.seq.banks[0].patterns[0].tracks[2].steps[15].state = TRACK_STEP_STATE::OFF;

  _seq_state.seq.banks[0].patterns[0].tracks[3].track_type = TRACK_TYPE::SAMPLE;
  _seq_state.seq.banks[0].patterns[0].tracks[3].sample_id = 3;

  Serial.println("Done filling test sequencer out");

  if (current_UI_mode == UI_MODE::TRACK_WRITE) {
    setDisplayStateForAllStepLEDs(); // TODO: wrap with "if in track view display step LED state" conditional, etc
  } else if (current_UI_mode == UI_MODE::PATTERN_WRITE) {
    // turnOffAllTrackStepLEDs();
  }
}

/* WARNING this function called from ISR */
void Update_Task_Ready_Flags(void)
{
  static uint32_t counter;
  One_MS_Task_Ready=1;
  counter++;
  if((counter%10)==0)
  {
    Ten_MS_Task_Ready=1;
  }
  if((counter%100)==0)
  {
    One_Hundred_MS_Task_Ready=1;;
  }
  if(counter == 1000)
  {
    One_S_Task_Ready=1;
    counter=0;
  }  
}

uint32_t Read_Idle_Counts(void)
{
  return Idle_Counts;
}
uint32_t Calculate_CPU_Utilization (uint32_t temp_counts)
{
  return 100 - ((100 * temp_counts) / UNLOADED_IDLE_COUNTS);
}
uint32_t Calculate_Idle_Counts (void)
{
  Idle_Counts = Idle_Counter - Prev_Idle_Counter;
  Prev_Idle_Counter = Idle_Counter;
  return Idle_Counts;
}
inline void One_MS_Task(void)
{

}
inline void Ten_MS_Task(void)
{
  //uncomment line below to put in some cpu load :)
  //delay(8);
}
inline void One_Hundred_MS_Task(void)
{

}
inline void One_S_Task(void)
{
  uint32_t idleCounts = Calculate_Idle_Counts();
#if SCALE == (LOG)
  uint8_t percent1 = (Calculate_CPU_Utilization(idleCounts)*2)/2;
  uint8_t percent2 = (Calculate_CPU_Utilization(idleCounts)*2)/2;
  uint8_t percent = (percent1*percent2)/100;
#elif SCALE == (LIN)
  uint8_t percent = Calculate_CPU_Utilization(idleCounts);
#endif
  //output percent to serial monitor
  Serial.print(F("CPU usage: "));
  Serial.print(percent);
  Serial.println(F("%"));
  Serial.print(F("Free RAM: "));
  Serial.println(freeram());
  // volatile char *p = (char *)malloc(152000);
  // *p = 0;
  // Serial.print("freeram = ");
  // Serial.println(freeram());
}

inline void Run_Tasks(void)
{
  if(One_MS_Task_Ready)
  {
    One_MS_Task_Ready=0;
    One_MS_Task();
  }
  if(Ten_MS_Task_Ready)
  {
    Ten_MS_Task_Ready=0;
    Ten_MS_Task();
  }
  if(One_Hundred_MS_Task_Ready)
  {
    One_Hundred_MS_Task_Ready=0;
    One_Hundred_MS_Task();
  }
  if(One_S_Task_Ready)
  {
    One_S_Task_Ready=0;
    One_S_Task();
  }
}

void loop(void)
{
  Idle_Counter++; 
  Run_Tasks();

  // handle hardware input states
  handleSwitchStates(false);
  handleKeyboardStates();
  handleEncoderStates();

  // handle LED display states
  handleDisplayModeLEDs();
}

void handleDisplayModeLEDs(void)
{
  if (current_UI_mode == UI_MODE::TRACK_WRITE) {
    int displayRefreshRateMs = 50; // decreaseing this too much increases digital noise due to signal interference?

    if (!(elapsed % displayRefreshRateMs)) {
      setDisplayStateForAllStepLEDs();
    }
  } else if (current_UI_mode == UI_MODE::PATTERN_WRITE) {
    // int displayRefreshRateMs = 50;

    // if (!(elapsed % displayRefreshRateMs) && _seq_state.playback_state == SEQUENCER_PLAYBACK_STATE::RUNNING) {
    //   setDisplayStateForPatternActiveTracksLEDs();
    // }
  } else if (current_UI_mode == UI_MODE::PATTERN_SEL) {
    int displayRefreshRateMs = 100;

    if (!(elapsed % displayRefreshRateMs)) {
      displayCurrentlySelectedPattern();
    }
  } else if (current_UI_mode == UI_MODE::TRACK_SEL) {
    int displayRefreshRateMs = 100;

    if (!(elapsed % displayRefreshRateMs)) {
      displayCurrentlySelectedTrack();
    }
  }
}

void initTest()
{
  u8g2.clearBuffer();
  u8g2.drawStr( 24, 18, "audio enjoyer");
  u8g2.drawStr( 36, 36, "xr-1 test");
  u8g2.sendBuffer();

  delay(1000);

  testLEDs();

  u8g2.clearBuffer();
  u8g2.drawStr( 16, 18, "press any button");
  u8g2.drawStr( 18, 36, "or twist a knob!");
  u8g2.sendBuffer();
}

void initMain()
{
  u8g2.clearBuffer();
  u8g2.drawStr( 24, 18, "audio enjoyer");
  u8g2.drawStr( 48, 36, "xr-1");
  u8g2.sendBuffer();

  delay(1000);

  drawSequencerScreen();
}

void drawSequencerScreen()
{
  u8g2.clearBuffer();
  u8g2.drawLine(0,10,128,10);

  if (current_UI_mode == UI_MODE::PATTERN_WRITE) {
    u8g2.drawStr( 0, 0, "PATTERN WRITE");
  } else if (current_UI_mode == UI_MODE::TRACK_WRITE) {
    u8g2.drawStr( 0, 0, "TRACK WRITE");
  }

  std::string patStr = "PATTERN: ";
  patStr += std::to_string(current_selected_pattern+1);
  u8g2.drawStr( 0, 16, patStr.c_str());

  std::string trkStr = "TRACK: ";
  trkStr += std::to_string(current_selected_track+1);
  u8g2.drawStr( 0, 28, trkStr.c_str());

  std::string tempoStr = "TEMPO: ";
  tempoStr += std::to_string((int)uClock.getTempo());
  u8g2.drawStr( 0, 40, tempoStr.c_str());

  if (_seq_state.playback_state == STOPPED) {
    u8g2.drawBox(110,2,5,5);
  } else if (_seq_state.playback_state == RUNNING) {
    u8g2.drawTriangle(110,1,110,7,117,4);
  } else if (_seq_state.playback_state == PAUSED) {
    u8g2.drawBox(110,2,2,5);
    u8g2.drawBox(113,2,2,5);
  }

  u8g2.sendBuffer();
}

void drawSetTempoOverlay(void)
{
  u8g2.clearBuffer();
  u8g2.drawFrame(1,1,127,63);
  u8g2.drawLine(1,15,127,15);

  std::string tempoStr = "SET TEMPO";
  u8g2.drawStr( 48, 5, tempoStr.c_str());

  u8g2.setFont(u8g2_font_logisoso16_tr); // u8g2_font_6x10_tf
  //u8g2.setFontRefHeightExtendedText();

  std::string tempoValStr = std::to_string((int)uClock.getTempo());
  u8g2.drawStr( 48, 25, tempoValStr.c_str());

  // reset font

  u8g2.setFont(small_font); // u8g2_font_6x10_tf
  u8g2.setFontRefHeightExtendedText();

  u8g2.sendBuffer();
}

void triggerAllStepsForGlobalStep(void)
{

  int8_t currGlobalStep = _seq_state.current_step - 1; // get zero-based setp
  BANK currentBank = _seq_state.seq.banks[0]; // TODO: make curr bank index a globally tracked var
  PATTERN currentPattern = currentBank.patterns[current_selected_pattern];

  const int MAX_PATTERN_TRACK_SIZE = 16;
  for (int t = 0; t < MAX_PATTERN_TRACK_SIZE; t++) {
    TRACK currTrack = currentPattern.tracks[t];
    TRACK_STEP currTrackStep = currTrack.steps[currGlobalStep];

    if ((currTrackStep.state == TRACK_STEP_STATE::ON) || (currTrackStep.state == TRACK_STEP_STATE::ACCENTED)) {
      if (currTrack.track_type == TRACK_TYPE::SAMPLE) {
        playFile(currTrack.sample_id, currTrackStep.state == TRACK_STEP_STATE::ACCENTED);
      }
    }
  }
}

void setDisplayStateForAllStepLEDs(void)
{
  //int8_t currGlobalStep = _seq_state.current_step - 1; // get zero-based setp
  BANK currentBank = _seq_state.seq.banks[0]; // TODO: make curr bank index a globally tracked var
  PATTERN currentPattern = currentBank.patterns[current_selected_pattern]; // TODO: make curr pattern index a globally tracked var
  TRACK currTrack = currentPattern.tracks[current_selected_track]; // TODO: make curr pattern index a globally tracked var

  const int MAX_TRACK_LEDS_SIZE = 17;
  for (int l = 1; l < MAX_TRACK_LEDS_SIZE; l++) {
    TRACK_STEP currTrackStepForLED = currTrack.steps[l-1];
    int8_t curr_led_char = stepCharMap[l];
    uint8_t keyLED = getKeyLED(curr_led_char);

    if (currTrackStepForLED.state == TRACK_STEP_STATE::OFF) {
      setLEDPWM(keyLED, 0);
      //delay(5);
    } else if (currTrackStepForLED.state == TRACK_STEP_STATE::ON) {
      setLEDPWM(keyLED, 512); // 256 might be better
      //delay(5);
    } else if (currTrackStepForLED.state == TRACK_STEP_STATE::ACCENTED) {
      setLEDPWM(keyLED, 4095);
      //delay(5);
    }
  }
}

void setDisplayStateForPatternActiveTracksLEDs(bool enable)
{
  int8_t currGlobalStep = _seq_state.current_step - 1; // get zero-based step

  BANK currentBank = _seq_state.seq.banks[0]; // TODO: make curr bank index a globally tracked var
  PATTERN currentPattern = currentBank.patterns[current_selected_pattern];

  const int MAX_PATTERN_TRACK_SIZE = 17;
  for (int t = 1; t < MAX_PATTERN_TRACK_SIZE; t++) {
    TRACK currTrack = currentPattern.tracks[t-1];
    int8_t curr_led_char = stepCharMap[t];
    uint8_t keyLED = getKeyLED(curr_led_char);
    
    TRACK_STEP currTrackStepForLED = currTrack.steps[currGlobalStep];

    if (currTrackStepForLED.state == TRACK_STEP_STATE::OFF) {
      setLEDPWM(keyLED, 0);
      //delay(5);
    } else if (currTrackStepForLED.state == TRACK_STEP_STATE::ON || currTrackStepForLED.state == TRACK_STEP_STATE::ACCENTED) {
      setLEDPWM(keyLED, enable ? 4095 : 0);
      //delay(5);
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

void handle_bpm_step(uint32_t tick)
{
  // This method handles advancing the sequencer
  // and displaying the start btn and step btn BPM LEDs

  uint8_t bpm_step_blink_timer = 1;
  int8_t curr_step_char = stepCharMap[_seq_state.current_step];
  uint8_t keyLED = getKeyLED(curr_step_char);

  // This handles displaying the BPM for the start button led
  // on qtr note. Check for odd step number to make sure not lit on backbeat qtr note.
  if (!(tick % 6)) {
    if ((_seq_state.current_step == 1 || _seq_state.current_step == 5 || _seq_state.current_step == 9 || _seq_state.current_step == 13)) {
      setLEDPWM(23, 4095); // each straight quarter note start button led ON
    }
  } else if ( !(tick % bpm_blink_timer) ) {
    setLEDPWM(23, 0); // turn start button led OFF
  }

  // This handles the sixteenth steps for all tracks
  if ( !(tick % (6)) ) {
    if (current_UI_mode == UI_MODE::PATTERN_WRITE || (previous_UI_mode == UI_MODE::PATTERN_WRITE && current_UI_mode == UI_MODE::SET_TEMPO)) {
      setDisplayStateForPatternActiveTracksLEDs(true);
    }

    triggerAllStepsForGlobalStep();

    if (_seq_state.current_step > 1) {
      uint8_t prevKeyLED = getKeyLED(stepCharMap[_seq_state.current_step-1]);
      setLEDPWM(prevKeyLED, 0); // turn prev sixteenth led OFF
    } else if (_seq_state.current_step == 1) {
      uint8_t prevKeyLED = getKeyLED(stepCharMap[16]);
      setLEDPWM(prevKeyLED, 0); // turn prev sixteenth led OFF
    }
    
    setLEDPWM(keyLED, 4095); // turn sixteenth led ON

    if (_seq_state.current_step <= 16) {
      // Serial.print("current step: ");
      // Serial.println(_seq_state.current_step);

      if (_seq_state.current_step < 16) {
        ++_seq_state.current_step; // advance current step for sequencer
      } else {
        _seq_state.current_step = 1; // reset current step
      }
    }
  } else if ( !(tick % bpm_step_blink_timer) ) {
      setLEDPWM(keyLED, 0); // turn 16th and start ON

    if (current_UI_mode == UI_MODE::PATTERN_WRITE || (previous_UI_mode == UI_MODE::PATTERN_WRITE && current_UI_mode == UI_MODE::SET_TEMPO)) {
      setDisplayStateForPatternActiveTracksLEDs(false);
    }
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
      
      // TODO: possibly add logic to ignore turning off LED if
      // current step is also an active state track step
      
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
      stopFiles();

      // TODO: possibly add logic to ignore turning off LED if
      // current step is also an active state track step
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
    stopFiles();

    
    // TODO: possibly add logic to ignore turning off LED if
    // current step is also an active state track step
    setLEDPWM(keyLED, 0); // turn off current step LED
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

bool selectedBtnCharIsATrack(char btnChar) {
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

void toggleLED(uint8_t num, bool discard)
{
  if (discard) {
    return;
  }

  if (activeLEDMap[num]) {
    activeLEDMap[num] = false;
    setLEDPWM(num, 0);
  } else if (!activeLEDMap[num]) {
    activeLEDMap[num] = true;
    if (num > 16 && num < 21) {
      setLEDPWM(num, 100); // max brightness for through-hole LEDs
    } else {
      setLEDPWM(num, 4095); // max brightness for SMT LEDs
    }
  }
}

void toggleSelectedStep(uint8_t step)
{
  uint8_t adjStep = step-1; // get zero based step num

  Serial.print("adjStep: ");
  Serial.println(adjStep);

  TRACK_STEP_STATE currStepState = _seq_state.seq.banks[0].patterns[current_selected_pattern].tracks[current_selected_track].steps[adjStep].state;

  Serial.print("currStepState: ");
  Serial.println(currStepState == TRACK_STEP_STATE::ACCENTED ? "accented" : (currStepState == TRACK_STEP_STATE::ON ? "on" : "off"));

  // TODO: implement accent state for MIDI, CV/Trig, Sample, Synth track types?
  if (currStepState == TRACK_STEP_STATE::OFF) {
    _seq_state.seq.banks[0].patterns[current_selected_pattern].tracks[current_selected_track].steps[adjStep].state = TRACK_STEP_STATE::ON;
  } else if (currStepState == TRACK_STEP_STATE::ON) {
    _seq_state.seq.banks[0].patterns[current_selected_pattern].tracks[current_selected_track].steps[adjStep].state = TRACK_STEP_STATE::ACCENTED;
  } else if (currStepState == TRACK_STEP_STATE::ACCENTED) {
    _seq_state.seq.banks[0].patterns[current_selected_pattern].tracks[current_selected_track].steps[adjStep].state = TRACK_STEP_STATE::OFF;
  }
}

void handleEncoderStates() {
  // slow this down from being called every loop when in set tempo mode

  if (!(elapsed % 100) && current_UI_mode == UI_MODE::SET_TEMPO) {
    int main_encoder_idx = 0;
    encoder_currValues[main_encoder_idx] = encoder_getValue(encoder_addrs[main_encoder_idx]);
    if(encoder_currValues[main_encoder_idx] != encoder_lastValues[main_encoder_idx]) {
      // std::string encoderPos = "Encoder ";
      // encoderPos += std::to_string(main_encoder_idx);
      // encoderPos += " pos: ";
      // encoderPos += std::to_string(encoder_currValues[main_encoder_idx]);
      // Serial.print("main enc: ");
      // Serial.println(encoderPos.c_str());

      int diff = encoder_currValues[main_encoder_idx] - encoder_lastValues[main_encoder_idx];
      float currTempo = uClock.getTempo();
      float newTempo = currTempo + diff;

      if ((newTempo - currTempo >= 1) || (currTempo - newTempo) >= 1) {
        // Serial.print("new tempo: ");
        // Serial.println(newTempo);

        uClock.setTempo(newTempo);
        drawSetTempoOverlay();
      }

      encoder_lastValues[main_encoder_idx] = encoder_currValues[main_encoder_idx];
    }
  }

  return;

  for (int i=0; i<5; i++) {
    encoder_currValues[i] = encoder_getValue(encoder_addrs[i]);
    if(encoder_currValues[i] != encoder_lastValues[i]) {
      std::string encoderPos = "Encoder ";
      encoderPos += std::to_string(i);
      encoderPos += " pos: ";
      encoderPos += std::to_string(encoder_currValues[i]);
      drawEvent(encoderPos, "", false);

      encoder_lastValues[i] = encoder_currValues[i];
    }
  }
}

void handleKeyboardStates(void) {
  return;

  // Get the currently touched pads
  currtouched = cap.touched();

  for (uint8_t i=0; i<12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      //Serial.print(i); Serial.println(" touched");
      std::string touched = "Touched key: ";
      touched += std::to_string(i);
      drawEvent(touched, "", false);
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      //Serial.print(i); Serial.println(" released");
    }
  }

  if (!fastBtnPressed && fastTouchRead(32) >= 64) {
    fastBtnPressed = true;
      std::string touched = "Touched key: 12";
      drawEvent(touched, "", false);
  } else if (fastBtnPressed && fastTouchRead(32) < 64) {
    fastBtnPressed = false;
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
              if (_checkTestFn && kpd.key[i].kchar == 'r') {
                _testUI = true;
              }

              if (_testUI) {
                std::string pressed = "Pressed button:";
                std::string pressedNewLine = getKeyStr(kpd.key[i].kchar);
                drawEvent(pressed, pressedNewLine, discard);
              } else {
                if (current_UI_mode == UI_MODE::TRACK_SEL && selectedBtnCharIsATrack(kpd.key[i].kchar)) {
                  uint8_t selTrack = getKeyStepNum(kpd.key[i].kchar);
                  current_selected_track = selTrack-1; // zero-based
                  previous_UI_mode = UI_MODE::TRACK_WRITE;
                  current_UI_mode = UI_MODE::TRACK_WRITE;
                  setDisplayStateForAllStepLEDs();

                  std::string msg = "Selected track: ";
                  std::string newLine = std::to_string(current_selected_track+1);
                  drawEvent(msg, newLine, false);
                } else if (current_UI_mode == UI_MODE::TRACK_WRITE && selectedBtnCharIsATrack(kpd.key[i].kchar)) {
                // make a duplicate method with a diff name to better reflect logic
                // e.g., selectedBtnCharIsAValidStep()

                  uint8_t stepToToggle = getKeyStepNum(kpd.key[i].kchar);
                  toggleSelectedStep(stepToToggle);
                } else if (current_UI_mode == UI_MODE::PATTERN_SEL && selectedBtnCharIsATrack(kpd.key[i].kchar)) {
                  uint8_t selPattern = getKeyStepNum(kpd.key[i].kchar);
                  current_selected_pattern = selPattern-1; // zero-based
                  previous_UI_mode = UI_MODE::PATTERN_WRITE;
                  current_UI_mode = UI_MODE::PATTERN_WRITE;
                  //setDisplayStateForAllStepLEDs();

                  std::string msg = "Selected pattern: ";
                  std::string newLine = std::to_string(current_selected_pattern+1);
                  drawEvent(msg, newLine, false);
                }
                

                if (kpd.key[i].kchar == 'q' || kpd.key[i].kchar == 'w') { // start or stop
                  toggleSequencerPlayback(kpd.key[i].kchar);
                  drawSequencerScreen();
                }
              }

              break;
            }
          case HOLD:
            {
              if (_testUI) {
                std::string held = "Held button:";
                std::string heldNewLine = getKeyStr(kpd.key[i].kchar);
                drawEvent(held, heldNewLine, discard);
              } else {
                // init track select UI mode if track button (char c) is held
                if (current_UI_mode != UI_MODE::TRACK_SEL && kpd.key[i].kchar == 'c') {
                  Serial.println("enter track select mode!");
                  current_UI_mode = UI_MODE::TRACK_SEL;

                  std::string msg = "SELECT A TRACK!";
                  drawEvent(msg, "", false);

                  clearAllStepLEDs();
                  displayCurrentlySelectedTrack();
                } else if (current_UI_mode != UI_MODE::PATTERN_SEL && kpd.key[i].kchar == 'b') {
                  Serial.println("enter pattern select mode!");
                  current_UI_mode = UI_MODE::PATTERN_SEL;

                  std::string msg = "SELECT A PATTERN!";
                  drawEvent(msg, "", false);

                  // TODO: implement function to show patterns with data by illuminating their step LEDs
                  clearAllStepLEDs();
                  displayCurrentlySelectedPattern();
                } else if (current_UI_mode != UI_MODE::PATTERN_SEL && kpd.key[i].kchar == '4') {
                  Serial.println("enter tempo set mode!");
                  current_UI_mode = UI_MODE::SET_TEMPO;

                  drawSetTempoOverlay();
                }

                if (current_UI_mode == UI_MODE::TRACK_SEL && selectedBtnCharIsATrack(kpd.key[i].kchar)) {
                  uint8_t selTrack = getKeyStepNum(kpd.key[i].kchar);
                  current_selected_track = selTrack;
                  //previous_UI_mode = UI_MODE::TRACK_WRITE;
                  current_UI_mode = UI_MODE::TRACK_WRITE;

                  std::string msg = "SELECTED TRACK: ";
                  std::string newLine = std::to_string(current_selected_track+1);
                  drawEvent(msg, newLine, false);
                } else if (current_UI_mode == UI_MODE::PATTERN_SEL && selectedBtnCharIsATrack(kpd.key[i].kchar)) {
                  uint8_t selPattern = getKeyStepNum(kpd.key[i].kchar);
                  current_selected_pattern = selPattern;
                  //previous_UI_mode = UI_MODE::TRACK_WRITE;
                  current_UI_mode = UI_MODE::PATTERN_WRITE;

                  std::string msg = "SELECTED PATTERN: ";
                  std::string newLine = std::to_string(current_selected_pattern+1);
                  drawEvent(msg, newLine, false);
                }
              }

              break;
            }
          case RELEASED:
            {
              // when held button is released, reset the held mode to prior mode

                  Serial.print("button char: ");
                  Serial.print(kpd.key[i].kchar);
                  Serial.print(" released!");

                  if (current_UI_mode == UI_MODE::TRACK_WRITE) {
                    Serial.print(" selected track: ");
                    Serial.print(current_selected_track);
                    Serial.println(", entered track write mode!");

                    drawSequencerScreen();
                  } else if (current_UI_mode == UI_MODE::PATTERN_WRITE) {
                    Serial.print(" selected pattern: ");
                    Serial.print(current_selected_pattern);
                    Serial.println(", entered pattern write mode!");

                    drawSequencerScreen();
                    clearAllStepLEDs();
                  }

                  if (current_UI_mode == UI_MODE::TRACK_SEL) {
                    Serial.print(" reverting track select mode, entering: ");
                    Serial.print(previous_UI_mode == UI_MODE::PATTERN_WRITE ? "pattern write mode!" : "track write mode!");
                    // revert back to a prior write mode, not a hold mode
                    current_UI_mode = previous_UI_mode;

                    drawSequencerScreen();
                    
                    if (current_UI_mode == UI_MODE::TRACK_WRITE) {
                      setDisplayStateForAllStepLEDs();
                    }
                  } else if (current_UI_mode == UI_MODE::PATTERN_SEL) {
                    Serial.print(" reverting pattern select mode, entering: ");
                    Serial.print(previous_UI_mode == UI_MODE::PATTERN_WRITE ? "pattern write mode!" : "track write mode!");
                    // revert back to a prior write mode, not a hold mode
                    current_UI_mode = previous_UI_mode;

                    drawSequencerScreen();
                    
                    if (current_UI_mode == UI_MODE::TRACK_WRITE) {
                      setDisplayStateForAllStepLEDs();
                    } else if (current_UI_mode == UI_MODE::PATTERN_WRITE) {
                      clearAllStepLEDs();
                    }
                  } else if (current_UI_mode == UI_MODE::SET_TEMPO) {
                    Serial.print(" reverting set tempo mode, entering: ");
                    Serial.print(previous_UI_mode == UI_MODE::PATTERN_WRITE ? "pattern write mode!" : "track write mode!");
                    // revert back to a prior write mode, not a hold mode
                    current_UI_mode = previous_UI_mode;

                    drawSequencerScreen();
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

void drawEvent(std::string event, std::string eventAddl, bool discard) {
  u8g2.clearBuffer();

  if (!discard) {
    u8g2.drawStr( 16, 24, event.c_str());

    if (eventAddl != "") {
      u8g2.drawStr( 16, 36, eventAddl.c_str());
    }

    u8g2.sendBuffer();
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
    encoder_set(encoder_addrs[i], -50, 50, 1, 0, 0);
  }
}

int16_t encoder_getValue(int addr) {
  Wire1.requestFrom(addr, 2);
  return((uint16_t)Wire1.read() | ((uint16_t)Wire1.read() << 8));
}

void u8g2_prepare(void) {
  u8g2.setFont(small_font); // u8g2_font_6x10_tf
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

void testLEDs()
{
  for (int j=0; j<24; j++) {
    if (j > 16 && j < 21) {
      setLEDPWM(j, 100);    //sets all 24 outputs to maximum brightness PWM.
    } else {
      setLEDPWM(j, 4095);    //sets all 24 outputs to maximum brightness PWM.
    }
    //setLEDPWM(j, 50);    //sets all 24 outputs to maximum brightness PWM.
    delay(50);
  }

  for (int j=0; j<24; j++) {
    if (j > 16 && j < 21) {
      setLEDPWM(j, 25);    //sets all 24 outputs to maximum brightness PWM.
    } else {
      setLEDPWM(j, 500);    //sets all 24 outputs to maximum brightness PWM.
    }
    //setLEDPWM(j, 50);    //sets all 24 outputs to maximum brightness PWM.
    delay(50);
  }

  delay(100);

  for (int i=0; i<24; i++) {
    setLEDPWM(i, 0);    //sets all 24 outputs to no brightness PWM.
  }
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