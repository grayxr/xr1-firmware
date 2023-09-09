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
#include <AudioConfig.h>
#include <SD.h>
#include <SerialFlash.h>
#include <TeensyVariablePlayback.h>
#include "flashloader.h"
#include <ResponsiveAnalogRead.h>
#include <LittleFS.h>
#include <TimeLib.h>
#include <Fonts.h>

time_t RTCTime;

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
  { 'a', "PERFORM" },
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
  uint8_t raw_sample_id = 0;
  uint8_t wav_sample_id = 0;
  uint8_t waveform = WAVEFORM_TYPE::SAW;
  uint8_t last_step = DEFAULT_LAST_STEP;
  uint8_t note = 0; // 0 - C1
  uint8_t octave = 4; // 4 - middle C (C4)
  int8_t detune = -7;
  int8_t fine = 0;
  int8_t microtiming = 0;
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
  bool muted = false;
  bool soloing = false;
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

EXTMEM SEQUENCER_EXTERNAL _seq_external;

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
  int8_t current_step = 1;
  int8_t current_bar = 1;
} TRACK_CURRENT_STEP_STATE;

typedef struct
{
  SEQUENCER_PLAYBACK_STATE playback_state = STOPPED;
  TRACK_CURRENT_STEP_STATE current_track_steps[MAXIMUM_SEQUENCER_TRACKS];
  int8_t current_step = 1;
  int8_t current_bar = 1;
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

// Project structure

bool project_initialized = false;

typedef struct
{
  char directory[MAX_SAMPLE_NAME_LENGTH];
  char filename[MAX_SAMPLE_NAME_LENGTH];
} PROJECT_RAW_SAMPLE;

typedef struct
{
  char directory[MAX_SAMPLE_NAME_LENGTH];
  char filename[MAX_SAMPLE_NAME_LENGTH];
} PROJECT_WAV_SAMPLE;

enum CLOCK_TYPE {
  DIN_MIDI,
  USB_MIDI,
};

typedef struct
{
  bool receive = false;
  bool send = false;
  CLOCK_TYPE receive_port;
  CLOCK_TYPE send_port;
} PROJECT_CLOCK_SETTINGS;

typedef struct
{
  // PROJECT_RAW_SAMPLE raw_sample_pool[MAX_USABLE_SAMPLE_IDS];
  // PROJECT_WAV_SAMPLE wav_sample_pool[MAX_USABLE_WAV_SAMPLE_IDS];
  PROJECT_CLOCK_SETTINGS clock_settings;
  char name[22];
  float tempo = 120.0;
} PROJECT;

PROJECT current_project;

const int numChannels = 1; // 1 for mono, 2 for stereo...

enum UI_MODE {
  BANK_WRITE,
  BANK_SEL,
  PATTERN_WRITE, // The default mode
  PATTERN_SEL,
  TRACK_WRITE,
  TRACK_SEL,
  SET_TEMPO,
  SUBMITTING_STEP_VALUE,
  PROJECT_INITIALIZE,
  PROJECT_BUSY,
  CHANGE_SETUP,
  PERFORM_SEL,
  PERFORM_TAP,
  PERFORM_MUTE,
  PERFORM_SOLO,
  PERFORM_RATCHET,
};

UI_MODE previous_UI_mode = PATTERN_WRITE; // the default mode
UI_MODE current_UI_mode = PATTERN_WRITE; // the default mode

int8_t current_selected_bank = 0; // default to 0 (first)
int8_t current_selected_pattern = 0; // default to 0 (first)
int8_t current_selected_track = 0; // default to 0 (first)
int8_t current_selected_step = -1; // default to -1 (none)

int8_t current_step_page = 1;

typedef struct 
{
  int bank = -1;
  int number = -1;
} QUEUED_PATTERN;

QUEUED_PATTERN _queued_pattern;

int draw_queue_blink = -1;
bool dequeue_pattern = false;

#define FUNCTION_BTN_CHAR 'r'
#define SOUND_SETUP_BTN_CHAR '0'

bool function_started = false;
bool track_sel_btn_held = false;

int8_t patt_held_for_selection = -1;
int8_t track_held_for_selection = -1; // default to -1 (none)
int8_t perf_held_for_selection = -1;

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

typedef struct
{
  float left;
  float right;
} PANNED_AMOUNTS;

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
int8_t getKeyLED(char idx);
void toggleSequencerPlayback(char btn);
void handle_bpm_step(uint32_t tick);
void triggerAllStepsForAllTracks(uint32_t tick);
void initMain(void);
void drawSequencerScreen(bool queueBlink = false);
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
void swapSequencerMemoryForPattern(int newBank, int newPattern);
void drawHatchedBackground();
void drawGenericOverlayFrame(void);
void drawErrorMessage(std::string message);
void drawSetupScreen();
void saveProject();
void displayPageLEDs(int currentBar = -1);
void displayCurrentOctaveLEDs();
void displayPerformModeLEDs(void);
void displayMuteLEDs(void);
void handleSoloForTrack(uint8_t track, bool undoSoloing);
void clearPageLEDs();
void changeSampleTrackSoundType(uint8_t t, TRACK_TYPE newType);
void configureVoiceSettingsOnLoad(void);
void configureSampleVoiceSettingsOnLoad(int t);
std::string strldz(std::string inputStr, const int zeroNum);
std::string getNewProjectName();
time_t getTeensy3Time();
PANNED_AMOUNTS getStereoPanValues(float pan);
TRACK getHeapTrack(int track);
TRACK_STEP getHeapStep(int track, int step);
PATTERN getHeapCurrentSelectedPattern(void);
TRACK getHeapCurrentSelectedTrack(void);
TRACK_STEP getHeapCurrentSelectedTrackStep(void);

time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

std::string getNewProjectName()
{
  std::string newProjectName = "project_";
  
  newProjectName += std::to_string(year());

  if (month() < 10) {
    newProjectName += strldz(std::to_string(month()),2);
  } else {
    newProjectName += std::to_string(month());
  }

  if (day() < 10) {
    newProjectName += strldz(std::to_string(day()),2);
  } else {
    newProjectName += std::to_string(day());
  }

  if (hour() < 10) {
    newProjectName += strldz(std::to_string(hour()),2);
  } else {
    newProjectName += std::to_string(hour());
  }

  if (minute() < 10) {
    newProjectName += strldz(std::to_string(minute()),2);
  } else {
    newProjectName += std::to_string(minute());
  }

  if (second() < 10) {
    newProjectName += strldz(std::to_string(second()),2);
  } else {
    newProjectName += std::to_string(second());
  }

  return newProjectName;
}

std::string new_project_name;

void drawErrorMessage(std::string message)
{
  u8g2.clearBuffer();

  drawGenericOverlayFrame();

  u8g2.drawStr(37, 6, "ERROR!");
  u8g2.drawStr(10, 22, message.c_str());

  u8g2.sendBuffer();
}

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

  initTrackSounds();
  configureVoiceSettingsOnLoad();
}

void queuePatternPlay(int selPattern)
{

}

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
  if (!project_initialized) {
    return;
  }

  // Send MIDI_CLOCK to external gears
  //usbMIDI.sendRealTime(usbMIDI.Clock);

  handle_bpm_step(tick);
}

void onClockStart() {
  if (!project_initialized) {
    return;
  }

  //usbMIDI.sendRealTime(usbMIDI.Start);
}

void onClockStop() {
  if (!project_initialized) {
    return;
  }

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

void configureSampleVoiceSettingsOnLoad(int t)
{
  TRACK track = getHeapTrack(t);

  if (track.track_type == WAV_SAMPLE) {
    // only create buffers for stereo samples when needed
    sampleVoices[t-4].wSample.createBuffer(2048, AudioBuffer::inExt);

    _seq_heap.pattern.tracks[t].track_type = WAV_SAMPLE;
  }
}

void configureVoiceSettingsOnLoad(void)
{
  for (int t=0; t<MAXIMUM_SEQUENCER_TRACKS; t++) {
    if (t > 3) {
      configureSampleVoiceSettingsOnLoad(t);
      continue;
    }

    ComboVoice trackVoice = comboVoices[t];

    if (_seq_heap.pattern.tracks[t].track_type == RAW_SAMPLE) {
      _seq_heap.pattern.tracks[t].track_type = RAW_SAMPLE;

      // turn sample volume all the way up
      trackVoice.mix.gain(0, 1);
      // turn synth volume all the way down
      trackVoice.mix.gain(1, 0); // synth
    } else if (_seq_heap.pattern.tracks[t].track_type == WAV_SAMPLE) {
      _seq_heap.pattern.tracks[t].track_type = WAV_SAMPLE;

      // only create buffers for stereo samples when needed
      trackVoice.wSample.createBuffer(2048, AudioBuffer::inExt);

      // turn sample volume all the way up
      trackVoice.mix.gain(0, 1);
      // turn synth volumes all the way down
      trackVoice.mix.gain(1, 0); // synth
    } else if (_seq_heap.pattern.tracks[t].track_type == SUBTRACTIVE_SYNTH) {
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
    } else if (_seq_heap.pattern.tracks[t].track_type == MIDI) {
      _seq_heap.pattern.tracks[t].track_type = MIDI;

      // turn all audio for this track voice down
      trackVoice.mix.gain(0, 0); // mono sample
      trackVoice.mix.gain(1, 0); // ladder
    } else if (_seq_heap.pattern.tracks[t].track_type == CV_GATE) {
      _seq_heap.pattern.tracks[t].track_type = CV_GATE;

      // turn all audio for this track voice down
      trackVoice.mix.gain(0, 0); // mono sample
      trackVoice.mix.gain(1, 0); // ladder
    }else if (_seq_heap.pattern.tracks[t].track_type == CV_TRIG) {
      _seq_heap.pattern.tracks[t].track_type = CV_TRIG;

      // turn all audio for this track voice down
      trackVoice.mix.gain(0, 0); // mono sample
      trackVoice.mix.gain(1, 0); // ladder
    }
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

void initExternalSequencer(void);
void initExternalSequencer(void)
{
  for (int b = 0; b < MAXIMUM_SEQUENCER_BANKS; b++)
  {
    for (int p = 0; p < MAXIMUM_SEQUENCER_PATTERNS; p++)
    {
      _seq_external.banks[b].patterns[p].last_step = DEFAULT_LAST_STEP;

      for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
      {
        _seq_external.banks[b].patterns[p].tracks[t].track_type = RAW_SAMPLE;
        _seq_external.banks[b].patterns[p].tracks[t].raw_sample_id = 0;
        _seq_external.banks[b].patterns[p].tracks[t].wav_sample_id = 0;
        _seq_external.banks[b].patterns[p].tracks[t].waveform = WAVEFORM_TYPE::SAW;
        _seq_external.banks[b].patterns[p].tracks[t].last_step = DEFAULT_LAST_STEP;
        _seq_external.banks[b].patterns[p].tracks[t].note = 0;
        _seq_external.banks[b].patterns[p].tracks[t].octave = 4;
        _seq_external.banks[b].patterns[p].tracks[t].detune = 0;
        _seq_external.banks[b].patterns[p].tracks[t].fine = 0;
        _seq_external.banks[b].patterns[p].tracks[t].microtiming = 0;
        _seq_external.banks[b].patterns[p].tracks[t].length = 4;
        _seq_external.banks[b].patterns[p].tracks[t].bitrate = 16;
        _seq_external.banks[b].patterns[p].tracks[t].velocity = 50;
        _seq_external.banks[b].patterns[p].tracks[t].probability = 100;
        _seq_external.banks[b].patterns[p].tracks[t].channel = 1;
        _seq_external.banks[b].patterns[p].tracks[t].looptype = 0;
        _seq_external.banks[b].patterns[p].tracks[t].loopstart = 0;
        _seq_external.banks[b].patterns[p].tracks[t].loopfinish = 5000;
        _seq_external.banks[b].patterns[p].tracks[t].playstart = play_start_sample;
        _seq_external.banks[b].patterns[p].tracks[t].level = 0.7;
        _seq_external.banks[b].patterns[p].tracks[t].pan = 0;
        _seq_external.banks[b].patterns[p].tracks[t].sample_play_rate = 1.0;
        _seq_external.banks[b].patterns[p].tracks[t].width = 0.5;
        _seq_external.banks[b].patterns[p].tracks[t].oscalevel = 1;
        _seq_external.banks[b].patterns[p].tracks[t].oscblevel = 0.5;
        _seq_external.banks[b].patterns[p].tracks[t].cutoff = 1600;
        _seq_external.banks[b].patterns[p].tracks[t].res = 0;
        _seq_external.banks[b].patterns[p].tracks[t].filter_attack = 0;
        _seq_external.banks[b].patterns[p].tracks[t].filter_decay = 1000;
        _seq_external.banks[b].patterns[p].tracks[t].filter_sustain = 1.0;
        _seq_external.banks[b].patterns[p].tracks[t].filter_release = 5000;
        _seq_external.banks[b].patterns[p].tracks[t].amp_attack = 0;
        _seq_external.banks[b].patterns[p].tracks[t].amp_decay = 1000;
        _seq_external.banks[b].patterns[p].tracks[t].amp_sustain = 1.0;
        _seq_external.banks[b].patterns[p].tracks[t].amp_release = 5000;
        _seq_external.banks[b].patterns[p].tracks[t].noise = 0;
        _seq_external.banks[b].patterns[p].tracks[t].chromatic_enabled = false;
        _seq_external.banks[b].patterns[p].tracks[t].muted = false;
        _seq_external.banks[b].patterns[p].tracks[t].soloing = false;

        // now fill in steps
        for (int s = 0; s < MAXIMUM_SEQUENCER_STEPS; s++)
        {
          _seq_external.banks[b].patterns[p].tracks[t].steps[s].length = 4;
          _seq_external.banks[b].patterns[p].tracks[t].steps[s].note = 0;
          _seq_external.banks[b].patterns[p].tracks[t].steps[s].octave = 4;
          _seq_external.banks[b].patterns[p].tracks[t].steps[s].velocity = 50;
          _seq_external.banks[b].patterns[p].tracks[t].steps[s].microtiming = 0;
          _seq_external.banks[b].patterns[p].tracks[t].steps[s].probability = 100;
          _seq_external.banks[b].patterns[p].tracks[t].steps[s].state = TRACK_STEP_STATE::OFF;
        }
      }
    }
  }
}

void prepareSequencer(void);
void prepareSequencer(void)
{
  project_initialized = true;

  current_selected_pattern = 0;
  current_selected_track = 0;

  current_UI_mode = PATTERN_WRITE;

  drawSequencerScreen();
}

void saveProject()
{
  current_UI_mode = PROJECT_BUSY;

  int maxWidth = 128;
  int maxHeight = 64;

  int creatingBoxWidth = 50;
  int creatingBoxHeight = 20;
  int creatingBoxStartX = (maxWidth / 2) - (creatingBoxWidth / 2);
  int creatingBoxStartY = (maxHeight / 2) - (creatingBoxHeight / 2);
  int creatingBoxMsgStartX = creatingBoxStartX+8;
  int creatingBoxMsgStartY = creatingBoxStartY+6;

  drawHatchedBackground();

  u8g2.setColorIndex((u_int8_t)0);
  u8g2.drawBox(creatingBoxStartX-3, creatingBoxStartY-3, creatingBoxWidth+6, creatingBoxHeight+6);
  u8g2.setColorIndex((u_int8_t)1);

  // show project creation status indicator?
  u8g2.drawFrame(creatingBoxStartX, creatingBoxStartY, creatingBoxWidth, creatingBoxHeight);
  u8g2.drawStr(creatingBoxMsgStartX, creatingBoxMsgStartY, "SAVING!");

  delay(100);

  std::string currProjectFilename = "/";
  currProjectFilename += current_project.name;
  currProjectFilename += ".txt";

  Serial.println("Write current project text file to SD card!");

  File currProjectFile = SD.open(currProjectFilename.c_str(), FILE_WRITE);
  currProjectFile.truncate();
  currProjectFile.write((byte *)&current_project, sizeof(current_project));
  currProjectFile.close();

  delay(100);

  // push current heap memory to PSRAM
  _seq_external.banks[current_selected_bank].patterns[current_selected_pattern] = _seq_heap.pattern;
  
  String seqFilename = "/";
  seqFilename += current_project.name;
  seqFilename += "_seq.bin";

  Serial.println("Write current project sequencer data binary file to SD card!");

  File seqFileW = SD.open(seqFilename.c_str(), FILE_WRITE);
  seqFileW.truncate();
  seqFileW.write((byte *)&_seq_external, sizeof(_seq_external));
  seqFileW.close();

  // TODO: can prob remove this, the current pattern in heap should reflect the current pattern in PSRAM
  _seq_heap.pattern = _seq_external.banks[current_selected_bank].patterns[current_selected_pattern];

  Serial.println("done saving project!");
}

void loadLatestProject();
void loadLatestProject()
{
  File projectListFile = SD.open("/project_list.txt", FILE_READ);
  String projectListFileContents = projectListFile.readString();
  projectListFile.close();

  String latestProjectFilename = "/";
  latestProjectFilename += projectListFileContents;
  latestProjectFilename += ".txt";

  File latestProjectFile = SD.open(latestProjectFilename.c_str(), FILE_READ);  
  if (latestProjectFile.available()) {    
    latestProjectFile.read((byte *)&current_project, sizeof(current_project));
    latestProjectFile.close();
  } else {
    // handle project object evolution more gracefully than this?
    File latestProjectFile2 = SD.open(latestProjectFilename.c_str(), FILE_WRITE);  
    latestProjectFile2.truncate();
    strcpy(current_project.name, projectListFileContents.c_str());
    latestProjectFile2.write((byte *)&current_project, sizeof(current_project));
    latestProjectFile2.close();
  }

  String seqFilename = "/";
  seqFilename += current_project.name;
  seqFilename += "_seq.bin";

  int fSize = 0;
  
  File seqFile = SD.open(seqFilename.c_str(), FILE_READ);
  fSize = seqFile.size();

  if (fSize > 0) {
    uClock.setTempo(current_project.tempo);

    // copy sequencer data from SD card to PSRAM
    seqFile.read((byte *)&_seq_external, sizeof(_seq_external));
    seqFile.close();

    delay(100);

    // copy first pattern from PSRAM to Heap
    _seq_heap.pattern = _seq_external.banks[0].patterns[0];

    delay(100);
    
    initTrackSounds();

    delay(50);

    configureVoiceSettingsOnLoad();

    delay(50);

    prepareSequencer();
  } else {
    drawErrorMessage("Sequencer data not found!");
  }
}

void initProject();
void initProject()
{
  current_UI_mode = UI_MODE::PROJECT_BUSY;

  int maxWidth = 128;
  int maxHeight = 64;

  int creatingBoxWidth = 50;
  int creatingBoxHeight = 20;
  int creatingBoxStartX = (maxWidth / 2) - (creatingBoxWidth / 2);
  int creatingBoxStartY = (maxHeight / 2) - (creatingBoxHeight / 2);
  int creatingBoxMsgStartX = creatingBoxStartX+8;
  int creatingBoxMsgStartY = creatingBoxStartY+6;

  drawHatchedBackground();

  u8g2.setColorIndex((u_int8_t)0);
  u8g2.drawBox(creatingBoxStartX-3, creatingBoxStartY-3, creatingBoxWidth+6, creatingBoxHeight+6);
  u8g2.setColorIndex((u_int8_t)1);

  // show project creation status indicator?
  u8g2.drawFrame(creatingBoxStartX, creatingBoxStartY, creatingBoxWidth, creatingBoxHeight);
  u8g2.drawStr(creatingBoxMsgStartX, creatingBoxMsgStartY, "CREATING!");

  u8g2.sendBuffer();

  strcpy(current_project.name, new_project_name.c_str());

  // keep a file in flash memory to keep track of available projects
  // and keep the latest project modified at top if the list
  // so that it always opens first when the unit turns on?
  File newProjectListFile = SD.open("/project_list.txt", FILE_WRITE);
  newProjectListFile.print(new_project_name.c_str());
  newProjectListFile.close();

  delay(100);

  std::string newProjectFilename = "/";
  newProjectFilename += new_project_name;
  newProjectFilename += ".txt";

  Serial.println("Saving new project text file to SD card!");

  File newProjectFile = SD.open(newProjectFilename.c_str(), FILE_WRITE);
  newProjectFile.write((byte *)&current_project, sizeof(current_project));
  newProjectFile.close();

  delay(100);

  initExternalSequencer();

  String seqFilename = "/";
  seqFilename += current_project.name;
  seqFilename += "_seq.bin";

  Serial.println("Saving new project sequencer data binary file to SD card!");

  File seqFileW = SD.open(seqFilename.c_str(), FILE_WRITE);
  seqFileW.write((byte *)&_seq_external, sizeof(_seq_external));
  seqFileW.close();

  _seq_heap.pattern = _seq_external.banks[0].patterns[0];

  delay(100);

  configureVoiceSettingsOnLoad();

  prepareSequencer();
}

void drawInitProject();
void drawInitProject()
{
  u8g2.clearBuffer();

  drawGenericOverlayFrame();

  std::string createProjStr = "CREATE PROJECT";
  u8g2.drawStr(37, 6, createProjStr.c_str());

  std::string nameFieldStr = "NAME: ";

  nameFieldStr += new_project_name;

  u8g2.drawStr(10, 22, nameFieldStr.c_str());

  u8g2.drawFrame(106, 49, 17, 11);
  u8g2.drawStr(109, 51, "SEL");
  u8g2.drawStr(10, 52, "CONFIRM?");

  u8g2.sendBuffer();
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

void wipeProject(void);
void wipeProject(void)
{
  Serial.println("wiping known project data!");

  String seqFilename = "/";
  seqFilename += current_project.name;
  seqFilename += "_seq.bin";

  SD.remove(seqFilename.c_str()); 

  String projFilename = "/";
  projFilename += current_project.name;
  projFilename += ".txt";

  SD.remove(projFilename.c_str()); 

  SD.remove("/project_list.txt");
}

void setup() {
  Serial.begin(9600);

  if (CrashReport) {
    Serial.print(CrashReport);
  }

  setSyncProvider(getTeensy3Time);
  if(timeStatus()!= timeSet) 
     Serial.println("Unable to sync with the RTC");
  else
     Serial.println("RTC has set the system time");    

  new_project_name = getNewProjectName();  
  Serial.println("set project name");

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(50);
  Serial.println("buffered audio mem");

  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  // pullup resistors
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.8);
  Serial.println("enabled sgtl");

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  Serial.println("setup SD card");

  current_UI_mode = PROJECT_BUSY;

  // discard any dirty reads
  handleSwitchStates(true);
  Serial.println("handled switch states during project busy");

  // prepare sample names to be occupied
  initUsableSampleNames();
  Serial.println("init'd usable sample names");

  initTrackSounds();
  Serial.println("init'd track sounds");
  
  delay(25);

  Wire1.begin();
  Serial.println("started Wire1");

  SPI1.begin();
  SPI1.setMOSI(DAC_MOSI);
  SPI1.setSCK(DAC_SCK);
  Serial.println("started DAC SPI");

  pinMode(CS1, OUTPUT); // CS
  digitalWrite(CS1, HIGH);
  pinMode(CS2, OUTPUT); // CS
  digitalWrite(CS2, HIGH);
  pinMode(CS3, OUTPUT); // CS
  digitalWrite(CS3, HIGH);
  pinMode(CS4, OUTPUT); // CS
  digitalWrite(CS4, HIGH);

  initDACs();
  Serial.println("init'd DACs");

  tlc.begin();
  if (tlc5947_oe >= 0) {
    pinMode(tlc5947_oe, OUTPUT);
    digitalWrite(tlc5947_oe, LOW);
  }
  Serial.println("began TLC driver");

  for (int i=0; i<25; i++) {
    setLEDPWM(i, 0); // sets all 24 outputs to no brightness
  }
  Serial.println("init'd LEDs");

  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5B, &Wire2)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("began MPR121");

  // init display
  u8g2.begin();
  Serial.println("began u8g2");

  // initialize encoders
  initEncoders();
  Serial.println("init'd uencoders");

  kpd.setHoldTime(150);

  delay(100);

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
  Serial.println("init'd uClock");

  u8g2_prepare();
  initMain();
  Serial.println("init'd main screen");

  Serial.println("handling project save/load");
  
  File projectListFile = SD.open("/project_list.txt", FILE_READ);
  if (!projectListFile.available()) {
    current_UI_mode = UI_MODE::PROJECT_INITIALIZE;
    drawInitProject();
  } else {
    projectListFile.close();
    // load project here
    loadLatestProject();
    //wipeProject();
  }

  // IMPORTANT: DO THIS AFTER SD IS INITIALIZED ABOVE
  // load short project mono samples into PSRAM
  loadRawSamplesFromSdCard();
  parseRootForWavSamples();
  Serial.println("prepared samples");
}

void handleQueueActions(void);
void handleQueueActions(void)
{
  if (draw_queue_blink > -1) {
    if (draw_queue_blink == 1) {
      drawSequencerScreen(true);
    } else if (draw_queue_blink == 0) {
      drawSequencerScreen(false);
    }
  }

  if (dequeue_pattern) {
    dequeue_pattern = false;
        
    swapSequencerMemoryForPattern(_queued_pattern.bank, _queued_pattern.number);
    
    // reset queue flags
    _queued_pattern.bank = -1;
    _queued_pattern.number = -1;
    draw_queue_blink = -1;

    drawSequencerScreen();
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

  handleQueueActions();
}

#define STARS 500

float star_x[STARS], star_y[STARS], star_z[STARS]; 

void initStar(int i) {
  star_x[i] = random(-100, 100);
  star_y[i] = random(-100, 100);
  star_z[i] = random(100, 500);
}

void showStarfield(bool showLogo, bool showFooter, int shrinkAmt) {
  int x,y;
  int centrex,centrey;
  
  centrex = 128 / 2;
  centrey = 64 / 2; 

  // centrex = (128 / 2) - shrinkAmt;
  // centrey = (64 / 2) - shrinkAmt; 
  
  for (int i = 0; i < STARS; i++) {
    star_z[i] = star_z[i] - 7;

    int mult = 100 - shrinkAmt;

    x = star_x[i] / star_z[i] * (mult < 0 ? 0 : mult) + centrex;
    y = star_y[i] / star_z[i] * (mult < 0 ? 0 : mult) + centrey;

    if(
        (x < 0)  ||        (x > 128) || 
        (y < 0)  ||        (y > 64) ||
        (star_z[i] < 1)      
      )      initStar(i);

    if (mult >= 0) {
      u8g2.drawPixel(x, y);
    }
  }

  if (showLogo) {
    int boxWidth = 60;
    int boxHeight = 30;
    u8g2.setColorIndex((u_int8_t)0);
    u8g2.drawBox((128 / 2) - (boxWidth / 2), (64 / 2) - (boxHeight / 2) - 6, boxWidth, boxHeight);
    u8g2.setColorIndex((u_int8_t)1);
    u8g2.setFont(bitocra13_c); // u8g2_font_8x13_mr
    u8g2.drawStr(52, 18, "xr-1");
  }

  if (showFooter) {
    int fBoxWidth = 60;
    int fBoxHeight = 10;
    u8g2.setColorIndex((u_int8_t)0);
    u8g2.drawBox((128 / 2) - (fBoxWidth / 2), 42, fBoxWidth, fBoxHeight);
    u8g2.setColorIndex((u_int8_t)1);
    u8g2.setFont(small_font); // u8g2_font_6x10_tf
    u8g2.setFontRefHeightExtendedText();
    u8g2.drawStr(38, 43, "audio enjoyer");
  }
} 

void initMain()
{
  u8g2.clearBuffer();

  for (int i = 0; i < STARS; i++) {
    initStar(i);
  }

  uint32_t starfieldElapsed = 0;

  int shrinkAmt = 0;

  while (true)
  {
    u8g2.clearBuffer();

    if (starfieldElapsed > 20) {
      int mult = 1;

      if (starfieldElapsed > 50) mult = 3;

      shrinkAmt = shrinkAmt + (1 * mult);

      // ++shrinkAmt;
    }

    showStarfield((starfieldElapsed > 20 ? true : false), (starfieldElapsed > 40 ? true : false), shrinkAmt);

    u8g2.sendBuffer();

    delay(10);

    ++starfieldElapsed;
    if (starfieldElapsed >= 125) { // 1.25s
      break;
    }
  }

  u8g2.setFont(small_font); // u8g2_font_6x10_tf
  u8g2.setFontRefHeightExtendedText();
}

std::string strldz(std::string inputStr, const int zeroNum)
{
  std::string outputStr;

  unsigned int number_of_zeros = zeroNum - inputStr.length();

  outputStr.insert(0, number_of_zeros, '0');
  outputStr.append(inputStr);

  return outputStr;
}

void drawMenuHeader(std::string inputStr, int8_t value, int8_t startX, bool hideNum);
void drawMenuHeader(std::string inputStr, int8_t value, int8_t startX, bool hideNum)
{
  if (!hideNum) inputStr += strldz(std::to_string(value), 2);

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
    mods.cName = "STP";
    mods.dName = "--"; // fx send?

    mods.aValue = std::to_string((float)round(track.level * 100) / 100);
    mods.aValue = mods.aValue.substr(0,3);

    mods.bValue = std::to_string((float)round(track.pan * 100) / 100);
    mods.bValue = mods.bValue.substr(0,3);

    mods.cValue = std::to_string(track.last_step);

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
    mods.cName = "STP";
    mods.dName = "--"; // fx send?

    mods.aValue = std::to_string((float)round(track.level * 100) / 100);
    mods.aValue = mods.aValue.substr(0,3);

    mods.bValue = std::to_string((float)round(track.pan * 100) / 100);
    mods.bValue = mods.bValue.substr(0,3);

    mods.cValue = std::to_string(track.last_step);
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

SOUND_CONTROL_MODS getControlModDataForPattern();
SOUND_CONTROL_MODS getControlModDataForPattern()
{
  SOUND_CONTROL_MODS mods;

  PATTERN pattern = getHeapCurrentSelectedPattern();

  mods.aName = "STP";
  mods.bName = "--";
  mods.cName = "--";
  mods.dName = "--";

  mods.aValue = std::to_string(pattern.last_step); // TODO: use 1/16 etc display
  mods.bValue = "--";
  mods.cValue = "--";
  mods.dValue = "--";

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

  u8g2.drawLine(ctrlModHeaderStartX,20,ctrlModHeaderStartX,52);
  u8g2.drawLine(25+ctrlModHeaderStartX,20,25+ctrlModHeaderStartX,52);
  u8g2.drawLine(25+ctrlModHeaderStartX+(ctrlModSpacerMult*1),20,25+ctrlModHeaderStartX+(ctrlModSpacerMult*1),52);
  u8g2.drawLine(25+ctrlModHeaderStartX+(ctrlModSpacerMult*2),20,25+ctrlModHeaderStartX+(ctrlModSpacerMult*2),52);

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

void drawPatternControlMods(void);
void drawPatternControlMods(void)
{
  int ctrlModHeaderY = 20;
  int ctrlModHeaderBoxSize = 9;
  int ctrlModHeaderStartX = 29;
  int ctrlModSpacerMult = 25;

  // draw control mod indicators (a,b,c,d)
  u8g2.drawLine(ctrlModHeaderStartX,30,128,30);

  u8g2.drawLine(ctrlModHeaderStartX,20,ctrlModHeaderStartX,52);
  u8g2.drawLine(25+ctrlModHeaderStartX,20,25+ctrlModHeaderStartX,52);
  u8g2.drawLine(25+ctrlModHeaderStartX+(ctrlModSpacerMult*1),20,25+ctrlModHeaderStartX+(ctrlModSpacerMult*1),52);
  u8g2.drawLine(25+ctrlModHeaderStartX+(ctrlModSpacerMult*2),20,25+ctrlModHeaderStartX+(ctrlModSpacerMult*2),52);

  SOUND_CONTROL_MODS mods = getControlModDataForPattern();

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

void drawSequencerScreen(bool queueBlink)
{
  u8g2.clearBuffer();

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

  if (current_UI_mode == PERFORM_TAP) {
    u8g2.drawStr(0, 0, "TAP MODE");
    u8g2.sendBuffer();
    return;
  } else if (current_UI_mode == PERFORM_MUTE) {
    u8g2.drawStr(0, 0, "MUTE MODE");
    u8g2.sendBuffer();
    return;
  } else if (current_UI_mode == PERFORM_SOLO) {
    u8g2.drawStr(0, 0, "SOLO MODE");
    u8g2.sendBuffer();
    return;
  } else if (current_UI_mode == PERFORM_RATCHET) {
    u8g2.drawStr(0, 0, "RATCHET MODE");
    u8g2.sendBuffer();
    return;
  }

  drawMenuHeader("BNK:", 1, 0, false);

  int ptnNumber = current_selected_pattern+1;
  if (_queued_pattern.number > -1) {
    ptnNumber = _queued_pattern.number+1;
  }

  if (current_UI_mode == UI_MODE::PATTERN_WRITE) {
    u8g2.setColorIndex((u_int8_t)1);
    u8g2.drawBox(26,0,29,7);
    u8g2.setColorIndex((u_int8_t)0);
    drawMenuHeader("PTN:", ptnNumber, 29, queueBlink);
    u8g2.setColorIndex((u_int8_t)1);
  } else if (current_UI_mode == TRACK_WRITE || current_UI_mode == TRACK_SEL || current_UI_mode == SUBMITTING_STEP_VALUE) {
    drawMenuHeader("PTN:", ptnNumber, 29, queueBlink);
  }

  if (current_UI_mode == TRACK_WRITE || current_UI_mode == TRACK_SEL || current_UI_mode == SUBMITTING_STEP_VALUE) {
    u8g2.setColorIndex((u_int8_t)1);
    u8g2.drawBox(55,0,29,7);
    u8g2.setColorIndex((u_int8_t)0);
    drawMenuHeader("TRK:", current_selected_track+1, 58, false);
    u8g2.setColorIndex((u_int8_t)1);
  } 
  // else {
  //   drawMenuHeader("TRK:", current_selected_track+1, 58, false);
  // }

  if (current_UI_mode == PATTERN_WRITE) {
    //PATTERN currPattern = getHeapCurrentSelectedPattern();

    // draw pattern header box
    u8g2.setColorIndex((u_int8_t)1);
    u8g2.drawBox(0,9,128,9);
    u8g2.setColorIndex((u_int8_t)0);
    std::string patternHeaderStr = "PATTERN EDIT";
    u8g2.drawStr(2, 10, patternHeaderStr.c_str());
    u8g2.setColorIndex((u_int8_t)1);

    // draw pattern main icon area
    std::string patternNumStr = strldz(std::to_string(current_selected_pattern+1), 2);
    u8g2.setFont(bitocra13_c);
    u8g2.drawStr(8, 26, patternNumStr.c_str());
    u8g2.setFont(bitocra7_c);

    // draw control mod area
    drawPatternControlMods();

    u8g2.drawLine(0,52,128,52);
    u8g2.drawStr(0,56,"MAIN");

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

  // displayPageLEDs();
}

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

void drawSetupScreen()
{
  drawGenericOverlayFrame();

  std::string setupStr = "SAVE PROJECT";
  u8g2.drawStr(37, 6, setupStr.c_str());

  std::string nameFieldStr = "NAME: ";

  nameFieldStr += current_project.name;

  u8g2.drawStr(10, 22, nameFieldStr.c_str());

  u8g2.drawFrame(86, 49, 17, 11);
  u8g2.drawStr(89, 51, "SEL");
  u8g2.drawFrame(106, 49, 17, 11);
  u8g2.drawStr(109, 51, "ESC");
  u8g2.drawStr(10, 52, "CONFIRM?");

  u8g2.sendBuffer();
}

void triggerSubtractiveSynthNoteOn(uint8_t t, uint8_t note);
void triggerSubtractiveSynthNoteOn(uint8_t t, uint8_t note)
{
  TRACK currTrack = getHeapCurrentSelectedPattern().tracks[t];

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
  TRACK currTrack = getHeapCurrentSelectedPattern().tracks[t];
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
  TRACK currTrack = getHeapCurrentSelectedPattern().tracks[t];
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
  Serial.print("provided track: ");
  Serial.println(t);

  TRACK currTrack = getHeapCurrentSelectedPattern().tracks[t];

  if (currTrack.track_type == RAW_SAMPLE) {
    triggerRawSampleNoteOn(t, note);
  } else if (currTrack.track_type == WAV_SAMPLE) {
    triggerWavSampleNoteOn(t, note);
  } else if (currTrack.track_type == SUBTRACTIVE_SYNTH) {
    triggerSubtractiveSynthNoteOn(t, note);
  }
}

void triggerAllStepsForAllTracks(uint32_t tick)
{
  PATTERN currentPattern = getHeapCurrentSelectedPattern();

  for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++) {
    int8_t currTrackStep = _seq_state.current_track_steps[t].current_step - 1; // get zero-based track step number

    TRACK currTrack = currentPattern.tracks[t];
    TRACK_STEP currTrackStepData = currTrack.steps[currTrackStep];

    if (!currTrack.muted && ((currTrackStepData.state == TRACK_STEP_STATE::ON) || (currTrackStepData.state == TRACK_STEP_STATE::ACCENTED))) {
      handleAddToStepStack(tick, t, currTrackStep);
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

  if (trackToUse.muted) {
    return;
  }

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

void handleNoteOffForTrack(int track);
void handleNoteOffForTrack(int track)
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
    int stepToUse = l;

    // todo: check if current track has last_step > 16
    // if so, use proper offset to get correct step state for current page
    if (current_step_page == 2) {
      stepToUse += 16;
    } else if (current_step_page == 3) {
      stepToUse += 32;
    } else if (current_step_page == 4) {
      stepToUse += 48;
    }

    TRACK_STEP currTrackStepForLED = currTrack.steps[stepToUse-1];
    int8_t curr_led_char = stepCharMap[l];
    int8_t keyLED = getKeyLED(curr_led_char);

    if (stepToUse > currTrack.last_step) {
      setLEDPWM(keyLED, 0);
      continue;
    }

    if (currTrackStepForLED.state == TRACK_STEP_STATE::OFF) {
      if (keyLED < 0) {
        Serial.println("could not find key LED!");
      } else {
        setLEDPWM(keyLED, 0);
      }
    } else if (currTrackStepForLED.state == TRACK_STEP_STATE::ON) {
      setLEDPWM(keyLED, 512); // 256 might be better
    } else if (currTrackStepForLED.state == TRACK_STEP_STATE::ACCENTED) {
      setLEDPWM(keyLED, 4095);
    }
  }
}

void setDisplayStateForPatternActiveTracksLEDs(bool enable)
{
  PATTERN currentPattern = getHeapCurrentSelectedPattern();

  const int MAX_PATTERN_TRACK_SIZE = 17;
  for (int t = 1; t < MAX_PATTERN_TRACK_SIZE; t++) {
    TRACK currTrack = currentPattern.tracks[t-1];
    int8_t curr_led_char = stepCharMap[t];
    int8_t keyLED = getKeyLED(curr_led_char);
    int8_t currTrackStep = _seq_state.current_track_steps[t-1].current_step;
    
    TRACK_STEP currTrackStepForLED = currTrack.steps[currTrackStep-1];

    if (currTrackStepForLED.state == TRACK_STEP_STATE::OFF) {
      if (keyLED < 0) {
        Serial.println("could not find key LED!");
      } else {
        setLEDPWM(keyLED, 0);
      }
    } else if (currTrackStepForLED.state == TRACK_STEP_STATE::ON || currTrackStepForLED.state == TRACK_STEP_STATE::ACCENTED) {
      setLEDPWM(keyLED, enable ? 4095 : 0);
    }
  }
}

void displayCurrentlySelectedPattern(void)
{
  int8_t curr_led_char = stepCharMap[current_selected_pattern+1];
  int8_t keyLED = getKeyLED(curr_led_char);

  if (keyLED < 0) {
    Serial.println("could not find key LED - 4!");
  } else {
    setLEDPWM(keyLED, 4095);
  }
}

void displayCurrentlySelectedTrack(void)
{
  int8_t curr_led_char = stepCharMap[current_selected_track+1];
  int8_t keyLED = getKeyLED(curr_led_char);

  if (keyLED < 0) {
    Serial.println("could not find key LED - 5!");
  } else {
    setLEDPWM(keyLED, 4095);
  }
}

void clearAllStepLEDs(void)
{
  for (int s = 0; s < 16; s++) {
    setLEDPWM(stepLEDPins[s], 0);
  }
}

void displayPerformModeLEDs(void)
{
  for (int s = 12; s < 16; s++) {
    setLEDPWM(stepLEDPins[s], 4095);
  }
}

void displayMuteLEDs(void)
{
  PATTERN currPattern = getHeapCurrentSelectedPattern();

  for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
  {
    if (!currPattern.tracks[t].muted) {
      setLEDPWM(stepLEDPins[t], 0);
    } else {
      setLEDPWM(stepLEDPins[t], 4095);
    }
  }
}

void handleSoloForTrack(uint8_t track, bool undoSoloing)
{
  for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
  {
    if (undoSoloing) {
      _seq_heap.pattern.tracks[t].soloing = false;
      _seq_heap.pattern.tracks[t].muted = false;
      setLEDPWM(stepLEDPins[t], 0);

      continue;
    }

    if (t == track) {
      _seq_heap.pattern.tracks[t].soloing = true;
      _seq_heap.pattern.tracks[t].muted = false;
      setLEDPWM(stepLEDPins[t], 4095);
    } else {
      _seq_heap.pattern.tracks[t].soloing = false;
      _seq_heap.pattern.tracks[t].muted = true;
      setLEDPWM(stepLEDPins[t], 0);
    }
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
    if (!trackToUse.muted && _step_stack[i].length == -1 ) {
      _step_stack[i].trackNum = track;
      _step_stack[i].stepNum = step;
      _step_stack[i].length = stepToUse.length > 0 ? stepToUse.length : trackToUse.length;

      handleNoteOnForTrackStep(_step_stack[i].trackNum, _step_stack[i].stepNum);

      return;
    }
  }
}

void clearPageLEDs()
{
  setLEDPWM(17, 0);
  setLEDPWM(18, 0);
  setLEDPWM(19, 0);
  setLEDPWM(20, 0);
}

void displayPageLEDs(int currentBar)
{
  bool blinkCurrentPage = _seq_state.playback_state == RUNNING && currentBar != -1;

  TRACK currTrack = getHeapCurrentSelectedTrack();

  uint16_t pageOneBrightnessMin = (current_step_page == 1) ? 50 : 5;
  uint16_t pageTwoBrightnessMin = (current_step_page == 2) ? 50 : (currTrack.last_step > 16 ? 5 : 0);
  uint16_t pageThreeBrightnessMin = (current_step_page == 3) ? 50 : (currTrack.last_step > 32 ? 5 : 0);
  uint16_t pageFourBrightnessMin = (current_step_page == 4) ? 50 : (currTrack.last_step > 48 ? 5 : 0);

  uint16_t pageOneBrightnessMax = blinkCurrentPage && (currentBar == 1) ? pageOneBrightnessMin + 100 : pageOneBrightnessMin;
  uint16_t pageTwoBrightnessMax = blinkCurrentPage && (currentBar == 2) ? pageTwoBrightnessMin + 100 : pageTwoBrightnessMin;
  uint16_t pageThreeBrightnessMax = blinkCurrentPage && (currentBar == 3) ? pageThreeBrightnessMin + 100 : pageThreeBrightnessMin;
  uint16_t pageFourBrightnessMax = blinkCurrentPage && (currentBar == 4) ? pageFourBrightnessMin + 100 : pageFourBrightnessMin;
  
  setLEDPWM(17, pageOneBrightnessMax);
  setLEDPWM(18, pageTwoBrightnessMax);
  setLEDPWM(19, pageThreeBrightnessMax);
  setLEDPWM(20, pageFourBrightnessMax);
}

void updateCurrentPatternStepState(void);
void updateCurrentPatternStepState(void)
{
  PATTERN currPattern = getHeapCurrentSelectedPattern();

  int currPatternLastStep = currPattern.last_step;

  if (_seq_state.current_step <= currPatternLastStep) {
    if (_seq_state.current_step < currPatternLastStep) {
      if (!((_seq_state.current_step+1) % 16)) { // TODO: make this bar division configurable
        ++_seq_state.current_bar;
      }

      ++_seq_state.current_step; // advance current step for sequencer
    } else {
      _seq_state.current_step = 1; // reset current step
      _seq_state.current_bar = 1; // reset current bar

      if (_queued_pattern.bank > -1 && _queued_pattern.number > -1) {
        // nullify pattern queue
        dequeue_pattern = true;
      }
    }
  }
}

void updateAllTrackStepStates(void);
void updateAllTrackStepStates(void)
{
  for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
  {
    TRACK track = _seq_heap.pattern.tracks[t];
    int trackLastStep = track.last_step;
    int8_t track_current_step = _seq_state.current_track_steps[t].current_step;

    if (track_current_step <= trackLastStep) {
      if (track_current_step < trackLastStep) {
        if (!((track_current_step+1) % 16)) { // TODO: make this bar division configurable
          ++_seq_state.current_track_steps[t].current_bar;
        }

        ++_seq_state.current_track_steps[t].current_step; // advance current step for track
      } else {
        _seq_state.current_track_steps[t].current_step = 1; // reset current step for track
        _seq_state.current_track_steps[t].current_bar = 1; // reset current bar for track
      }
    }
  }
}

bool toggled_queue_ind_off = false;

void handle_bpm_step(uint32_t tick)
{
  int8_t currentSelectedPatternCurrentStep = _seq_state.current_step;
  int8_t currentSelectedTrackCurrentStep = _seq_state.current_track_steps[current_selected_track].current_step;
  int8_t currentSelectedTrackCurrentBar = _seq_state.current_track_steps[current_selected_track].current_bar;

  if (current_UI_mode == TRACK_WRITE && !function_started) {
    if (!(tick % 6)) {
      bool isOnStraightQtrNote = (currentSelectedTrackCurrentStep == 1 || !((currentSelectedTrackCurrentStep-1) % 4));

      if (isOnStraightQtrNote) {
        displayPageLEDs(currentSelectedTrackCurrentBar);
      }
    }
  }
  
  int curr_step_paged = currentSelectedTrackCurrentStep;
  if (current_step_page == 2 && curr_step_paged > 16 && curr_step_paged <= 32) {
    curr_step_paged -= 16;
  } else if (current_step_page == 3 && curr_step_paged > 32 && curr_step_paged <= 48) {
    curr_step_paged -= 32;
  } else if (current_step_page == 4 && curr_step_paged > 48 && curr_step_paged <= 64) {
    curr_step_paged -= 48;
  }

  // This method handles advancing the sequencer
  // and displaying the start btn and step btn BPM LEDs
  int8_t curr_step_char = stepCharMap[curr_step_paged];
  int8_t keyLED = getKeyLED(curr_step_char);

  // This handles displaying the BPM for the start button led
  // on qtr note. Check for odd step number to make sure not lit on backbeat qtr note.
  if (!(tick % 6)) {
    bool isOnStraightBeat = (currentSelectedPatternCurrentStep == 1 || !((currentSelectedPatternCurrentStep-1) % 4));
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

    if (_queued_pattern.bank > -1 && _queued_pattern.number > -1) {
      draw_queue_blink = 0;
    }

    // TODO: move out of 16th step !(tick % (6)) condition
    // so we can check for microtiming adjustments at the 96ppqn scale
    triggerAllStepsForAllTracks(tick);

    if (current_UI_mode == PATTERN_SEL) {
      clearAllStepLEDs();
      displayCurrentlySelectedPattern();
    } else if (current_UI_mode == TRACK_SEL) {
      clearAllStepLEDs();
      displayCurrentlySelectedTrack();
    } else if (current_UI_mode == TRACK_WRITE) {
      bool transitionStepLEDs = (
        (current_step_page == 1 && currentSelectedTrackCurrentStep <= 16) || 
        (current_step_page == 2 && currentSelectedTrackCurrentStep > 16 && currentSelectedTrackCurrentStep <= 32) ||
        (current_step_page == 3 && currentSelectedTrackCurrentStep > 32 && currentSelectedTrackCurrentStep <= 48) ||
        (current_step_page == 4 && currentSelectedTrackCurrentStep > 48 && currentSelectedTrackCurrentStep <= 64)
      );

      bool turnOffLastLED = (
        (current_step_page == 1 && currentSelectedTrackCurrentStep == 1) || 
        (current_step_page == 2 && currentSelectedTrackCurrentStep == 16) ||
        (current_step_page == 3 && currentSelectedTrackCurrentStep == 32) ||
        (current_step_page == 4 && currentSelectedTrackCurrentStep == 48)
      );

      if (currentSelectedTrackCurrentStep > 1) {
        if (transitionStepLEDs) {
            uint8_t prevKeyLED = getKeyLED(stepCharMap[curr_step_paged-1]);
            setLEDPWM(prevKeyLED, 0); // turn prev sixteenth led OFF
        }
      } else {
        if (turnOffLastLED) {
          TRACK currTrack = getHeapCurrentSelectedTrack();
          int currTrackLastStep = currTrack.last_step;
          if (currTrackLastStep > 16) {
            currTrackLastStep -= 16;
          }

          uint8_t prevKeyLED = getKeyLED(stepCharMap[currTrackLastStep]);
          setLEDPWM(prevKeyLED, 0); // turn prev sixteenth led OFF
        }
      }

      if (transitionStepLEDs) {
        setLEDPWM(keyLED, 4095); // turn sixteenth led ON
      }
    }

    updateCurrentPatternStepState();
    updateAllTrackStepStates();
  } else if ( !(tick % bpm_blink_timer) ) {
    // remove this?
    // if (current_UI_mode != PATTERN_SEL || current_UI_mode != TRACK_SEL) {
    //   if (keyLED < 0) {
    //     Serial.println("could not find key LED - 6!");
    //   } else {
    //     setLEDPWM(keyLED, 0); // turn 16th and start OFF
    //   }
    // }

    if (current_UI_mode == PATTERN_WRITE) {
      setDisplayStateForPatternActiveTracksLEDs(false);
    } else if (current_UI_mode == TRACK_WRITE || current_UI_mode == SUBMITTING_STEP_VALUE) {
      setDisplayStateForAllStepLEDs();
      if (!function_started) displayPageLEDs(-1);
    }
  }

  // every 1/4 step log memory usage
  if (!(tick % 24)) {
    // Serial.print("Memory: ");
    // Serial.print(AudioMemoryUsage());
    // Serial.print(",");
    // Serial.print(AudioMemoryUsageMax());
    // Serial.println();

    // blink queued bank / pattern
    if (_queued_pattern.bank > -1 && _queued_pattern.number > -1) {
      draw_queue_blink = 1;
    }
  }
}

void rewindAllCurrentStepsForAllTracks(void);
void rewindAllCurrentStepsForAllTracks(void)
{
  for (int t = 0; t < MAXIMUM_SEQUENCER_TRACKS; t++)
  {
    _seq_state.current_track_steps[t].current_step = 1;
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
      displayPageLEDs(-1);

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
      _seq_state.current_bar = 1;

      rewindAllCurrentStepsForAllTracks();

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
    _seq_state.current_bar = 1;

    rewindAllCurrentStepsForAllTracks();

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

int8_t getKeyLED(char idx) {
  if (charLEDMap.count(idx) != 0) {
    return charLEDMap[idx];
  }

  // Serial.print("key LED not found for idx: ");
  // Serial.println(idx);

  return -1;
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
  int stepToUse = step;

  // todo: check if current track has last_step > 16
  // if so, use proper offset to get correct step state for current page
  if (current_step_page == 2) {
    stepToUse += 16;
  } else if (current_step_page == 3) {
    stepToUse += 32;
  } else if (current_step_page == 4) {
    stepToUse += 48;
  }

  uint8_t adjStep = stepToUse-1; // get zero based step num

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
        current_project.tempo = newTempo;
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
    int currLastStep = currTrack.last_step;
    int newLastStep = currTrack.last_step + diff;

    // make sure track last step doesn't exceed pattern's
    PATTERN currPattern = getHeapCurrentSelectedPattern();

    // TODO: try to re-align current playing track step with pattern step if able

    if (newLastStep < 1) {
      newLastStep = 1;
    } else if (newLastStep > currPattern.last_step) {
      newLastStep = currPattern.last_step;
    }

    if (newLastStep != currLastStep) {
      _seq_heap.pattern.tracks[current_selected_track].last_step = newLastStep;

      displayPageLEDs();
      setDisplayStateForAllStepLEDs();
      drawSequencerScreen();
    }
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
    int currLastStep = currTrack.last_step;
    int newLastStep = currTrack.last_step + diff;

    // make sure track last step doesn't exceed pattern's
    PATTERN currPattern = getHeapCurrentSelectedPattern();

    // TODO: try to re-align current playing track step with pattern step if able

    if (newLastStep < 1) {
      newLastStep = 1;
    } else if (newLastStep > currPattern.last_step) {
      newLastStep = currPattern.last_step;
    }

    if (newLastStep != currLastStep) {
      _seq_heap.pattern.tracks[current_selected_track].last_step = newLastStep;

      displayPageLEDs();
      setDisplayStateForAllStepLEDs();
      drawSequencerScreen();
    }
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
  } else if (current_page_selected == 3) {
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

// Pattern mods

void handleEncoderPatternModA(int diff);
void handleEncoderPatternModA(int diff)
{
  PATTERN currPattern = getHeapCurrentSelectedPattern();

  int currLastStep = currPattern.last_step;
  int newLastStep = currPattern.last_step + diff;

  if (newLastStep < 1) {
    newLastStep = 1;
  } else if (newLastStep > MAXIMUM_SEQUENCER_STEPS) { // use max step as a #define later
    newLastStep = MAXIMUM_SEQUENCER_STEPS;
  }

  if (newLastStep != currLastStep) {
    _seq_heap.pattern.last_step = newLastStep;

    for (int t=0; t<MAXIMUM_SEQUENCER_TRACKS; t++) {
      // set track's last_step to match pattern if track last_step is greater than pattern's
      if (_seq_heap.pattern.tracks[t].last_step > newLastStep) {
        _seq_heap.pattern.tracks[t].last_step = newLastStep;
      }
    }

    drawSequencerScreen();
  }
}

void handleEncoderPatternModB(int diff);
void handleEncoderPatternModB(int diff)
{
  //
}

void handleEncoderPatternModC(int diff);
void handleEncoderPatternModC(int diff)
{
  //
}

void handleEncoderPatternModD(int diff);
void handleEncoderPatternModD(int diff)
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

void handleEncoderSetPatternMods(void);
void handleEncoderSetPatternMods(void)
{
  PATTERN currPattern = getHeapCurrentSelectedPattern();

  const int modCount = 4;
  for (int m = 0; m < modCount; m++)
  {
    int modEncoderIdx = m+1;

    encoder_currValues[modEncoderIdx] = encoder_getValue(encoder_addrs[modEncoderIdx]);

    if(encoder_currValues[modEncoderIdx] != encoder_lastValues[modEncoderIdx]) {
      int diff = encoder_currValues[modEncoderIdx] - encoder_lastValues[modEncoderIdx];

      if (m == 0) {
          handleEncoderPatternModA(diff);
        } else if (m == 1) {
          handleEncoderPatternModB(diff);
        } else if (m == 2) {
          handleEncoderPatternModC(diff);
        } else if (m == 3) {
          handleEncoderPatternModD(diff);
        }

      encoder_lastValues[modEncoderIdx] = encoder_currValues[modEncoderIdx];
    }
  }
}

void handleEncoderStates() {
  if (!project_initialized) {
    Serial.println("Not handling encoder states, project is not initialized!");
    return;
  }

  // slow this down from being called every loop when in set tempo mode
  if (!(elapsed % 25) && current_UI_mode == SET_TEMPO) {
    handleEncoderSetTempo();
  } else if (!(elapsed % 25) && current_UI_mode == PATTERN_WRITE) {
    handleEncoderSetPatternMods();
  } else if (!(elapsed % 25) && (current_UI_mode == TRACK_WRITE || current_UI_mode == SUBMITTING_STEP_VALUE)) {
    if (current_UI_mode == TRACK_WRITE) {
      handleEncoderTraversePages();
    }

    handleEncoderSetTrackMods();
  }
}

void handleKeyboardStates(void) {
  if (!project_initialized) {
    return;
  }

  // TODO: remove
  if (_queued_pattern.bank > -1 || _queued_pattern.number > -1) {
    return;
  }

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

              if (current_UI_mode != PROJECT_INITIALIZE  && current_UI_mode != PROJECT_BUSY && !project_initialized) {
                drawErrorMessage("PROJECT NOT INITIALIZED!");

                return;
              } else if (current_UI_mode == PROJECT_INITIALIZE && !project_initialized) {
                if (kpd.key[i].kchar == 'i') { // select
                  initProject();
                }
              } else if (current_UI_mode == PROJECT_BUSY && !project_initialized) {
                return;
              }

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

                  if (_seq_state.playback_state == RUNNING) {
                    _queued_pattern.bank = current_selected_bank;
                    _queued_pattern.number = selPattern;

                    previous_UI_mode = PATTERN_SEL;
                    current_UI_mode = PATTERN_WRITE;
                  } else {
                    swapSequencerMemoryForPattern(current_selected_bank, selPattern);

                    Serial.print("marking pressed pattern selection (zero-based): ");
                    Serial.println(selPattern);

                    patt_held_for_selection = selPattern;

                    previous_UI_mode = PATTERN_SEL;
                    current_UI_mode = PATTERN_SEL;

                    clearAllStepLEDs();
                    displayCurrentlySelectedPattern();
                  }
                } 
                // start/pause or stop
                else if (kpd.key[i].kchar == 'q' || kpd.key[i].kchar == 'w') {
                  toggleSequencerPlayback(kpd.key[i].kchar);
                  drawSequencerScreen();
                }
                // page
                else if (kpd.key[i].kchar == '9' || kpd.key[i].kchar == '3') {
                  Serial.print("btn: ");
                  Serial.println(kpd.key[i].kchar);

                  if (kpd.key[i].kchar == '3') {
                    current_step_page = min(4, current_step_page+1);

                  } else if (kpd.key[i].kchar == '9') {
                    current_step_page = max(1, current_step_page-1);
                  }

                  displayPageLEDs();
                  setDisplayStateForAllStepLEDs();

                  Serial.print("Updated page to: ");
                  Serial.println(current_step_page);
                }
                // perform
                else if (kpd.key[i].kchar == 'a') {
                  Serial.println("enter perform select mode!");
                  current_UI_mode = PERFORM_SEL;

                  clearAllStepLEDs();
                  displayPerformModeLEDs();
                } else if (current_UI_mode == PERFORM_SEL && btnCharIsATrack(kpd.key[i].kchar) && getKeyStepNum(kpd.key[i].kchar) >= 13) {
                  perf_held_for_selection = getKeyStepNum(kpd.key[i].kchar)-1;

                  Serial.print("perf_held_for_selection: ");
                  Serial.println(perf_held_for_selection);

                  if (getKeyStepNum(kpd.key[i].kchar) == 13) { // enable tap mode
                    previous_UI_mode = PATTERN_WRITE; // TODO: find better way to track UI mode before PERFORM_SEL
                    current_UI_mode = PERFORM_TAP;

                    clearAllStepLEDs();
                    drawSequencerScreen();
                  } else if (getKeyStepNum(kpd.key[i].kchar) == 14) { // enable mute mode
                    previous_UI_mode = PATTERN_WRITE; // TODO: find better way to track UI mode before PERFORM_SEL
                    current_UI_mode = PERFORM_MUTE;

                    displayMuteLEDs();
                    drawSequencerScreen();
                  } else if (getKeyStepNum(kpd.key[i].kchar) == 15) { // enable solo mode
                    previous_UI_mode = PATTERN_WRITE; // TODO: find better way to track UI mode before PERFORM_SEL
                    current_UI_mode = PERFORM_SOLO;

                    //displayMuteLEDs();
                    clearAllStepLEDs();
                    drawSequencerScreen();
                  } else if (getKeyStepNum(kpd.key[i].kchar) == 16) { // enable ratchet mode
                    previous_UI_mode = PATTERN_WRITE; // TODO: find better way to track UI mode before PERFORM_SEL
                    current_UI_mode = PERFORM_RATCHET;

                    //displayMuteLEDs();
                    clearAllStepLEDs();
                    drawSequencerScreen();
                  }
                } else if (current_UI_mode == PERFORM_TAP && btnCharIsATrack(kpd.key[i].kchar)) { // handle taps
                
                  triggerTrackManually(getKeyStepNum(kpd.key[i].kchar)-1, _seq_heap.pattern.tracks[getKeyStepNum(kpd.key[i].kchar)-1].note);

                } else if (current_UI_mode == PERFORM_MUTE && btnCharIsATrack(kpd.key[i].kchar)) { // handle mutes
                  bool currMuteState = _seq_heap.pattern.tracks[getKeyStepNum(kpd.key[i].kchar)-1].muted;

                  _seq_heap.pattern.tracks[getKeyStepNum(kpd.key[i].kchar)-1].muted = (currMuteState ? false : true);

                  displayMuteLEDs();
                } else if (current_UI_mode == PERFORM_SOLO && btnCharIsATrack(kpd.key[i].kchar)) { // handle solos
                  bool undoSoloing = _seq_heap.pattern.tracks[getKeyStepNum(kpd.key[i].kchar)-1].soloing;
                  handleSoloForTrack(getKeyStepNum(kpd.key[i].kchar)-1, undoSoloing);
                }
              }
              // function handling
              else {
                Serial.println("triggering function!");
                // change track sound type
                if ((current_UI_mode == TRACK_WRITE || current_UI_mode == PATTERN_WRITE) && kpd.key[i].kchar == SOUND_SETUP_BTN_CHAR) {
                  Serial.println("draw setup screen!");

                  previous_UI_mode = current_UI_mode;
                  current_UI_mode = CHANGE_SETUP;

                  drawSetupScreen();
                } else if (current_UI_mode == TRACK_WRITE && kpd.key[i].kchar == 'c') {
                  // TODO: impl AB layer switching mechanic
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

                  displayCurrentOctaveLEDs();

                  Serial.print("Updated octave to: ");
                  Serial.println(keyboardOctave);
                }
              }

              break;
            }
          case HOLD:
            {
              if (!project_initialized) {
                return;
              }

              // param lock step
              if (current_UI_mode == TRACK_WRITE && btnCharIsATrack(kpd.key[i].kchar)) {
                // editing a step value / parameter locking this step
                previous_UI_mode = TRACK_WRITE;
                current_UI_mode = SUBMITTING_STEP_VALUE;

                uint8_t stepToUse = getKeyStepNum(kpd.key[i].kchar);
                if (current_step_page == 2) {
                  stepToUse += 16;
                } else if (current_step_page == 3) {
                  stepToUse += 32;
                } else if (current_step_page == 4) {
                  stepToUse += 48;
                }

                current_selected_step = stepToUse-1;

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

                displayCurrentOctaveLEDs();
              }

              break;
            }
          case RELEASED:
            {
              if (!project_initialized) {
                return;
              }

              // track select / write release
              if (current_UI_mode == TRACK_SEL && kpd.key[i].kchar == 'c' && track_held_for_selection == -1) {      
                current_UI_mode = TRACK_WRITE; // force track write mode when leaving track / track select action
                previous_UI_mode = TRACK_WRITE;

                displayPageLEDs();
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

                displayPageLEDs();
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

                clearPageLEDs();
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
                saveProject();

                Serial.print(" reverting set tempo mode, entering: ");
                Serial.println(previous_UI_mode);
                // revert back to a prior write mode, not a hold mode
                current_UI_mode = previous_UI_mode;

                drawSequencerScreen();
              }

              // set octave release
              else if (kpd.key[i].kchar == '9' || kpd.key[i].kchar == '3') {
                // current_UI_mode = previous_UI_mode;
                // function_started = false;
                
                // revert LEDs to page display
                // displayPageLEDs();
              }

              // setup screen release
              else if ( current_UI_mode == CHANGE_SETUP) {
                if (kpd.key[i].kchar == 'i') { // select
                  saveProject();

                  auto leavingUI = current_UI_mode;
                  auto newUI = previous_UI_mode;

                  current_UI_mode = newUI;
                  previous_UI_mode = leavingUI;

                  Serial.println("leaving function!");

                  function_started = false;

                  drawSequencerScreen();
                } else if (kpd.key[i].kchar == 'j') {
                  auto leavingUI = current_UI_mode;
                  auto newUI = previous_UI_mode;

                  current_UI_mode = newUI;
                  previous_UI_mode = leavingUI;

                  Serial.print("current UI: ");
                  Serial.println(current_UI_mode);

                  Serial.println("leaving function!");

                  function_started = false;

                  drawSequencerScreen();
                }
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

              // perform mode select release
              else if (kpd.key[i].kchar == 'a' && perf_held_for_selection == -1) {
                // revert
                current_UI_mode = PATTERN_WRITE;
                previous_UI_mode = PERFORM_SEL;

                clearAllStepLEDs();
                drawSequencerScreen();
              } else if (current_UI_mode == PERFORM_SEL && (kpd.key[i].kchar == 'a' || kpd.key[i].kchar == 'w' || btnCharIsATrack(kpd.key[i].kchar)) && ((getKeyStepNum(kpd.key[i].kchar)-1) == perf_held_for_selection)) {
                Serial.println("reverting perf_held_for_selection");

                perf_held_for_selection = -1;
              } else if (current_UI_mode == PERFORM_TAP && btnCharIsATrack(kpd.key[i].kchar)) {
                Serial.println("handling note Off for tapped track");

                handleNoteOffForTrack(getKeyStepNum(kpd.key[i].kchar)-1);
              }

              // function stop
              else if (function_started && kpd.key[i].kchar == FUNCTION_BTN_CHAR) {
                Serial.println("leaving function!");
                function_started = false;

                if (current_UI_mode == TRACK_WRITE) {
                  displayPageLEDs();
                }
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

void drawHatchedBackground() {    
  int leftBoundX = 0;
  int rightBoundX = 128;
  int topBoundX = 0;
  int bottomBoundX = 64;

  int pixelRows = 64;
  int pixelCols = 128;

  // initialize row template
  int t[128];
  int spacing = 4;

  // fill up template
  for (int i=0; i<128; i++) {
      if (i == 0 || (i % spacing == 0)) {
          t[i] = 1;
      } else {
          t[i] = 0;
      }
  }

  for (int r = 0; r < pixelRows; r++) {
    if (r != 0) {
      //Rotate the given array one time toward right    
      for(int i = 0; i < 1; i++){    
        int j;
        int last;  
          
        //Stores the last element of array    
        last = t[127];
        
        for(j = 127; j > 0; j--){
          //Shift element of array by one    
          t[j] = t[j-1];    
        }    
        //Last element of array will be added to the start of array.    
        t[0] = last;    
      } 
    }
    
    for (int c = 0; c < pixelCols; c++) {
      if (t[c] == 1) {
        u8g2.drawPixel(leftBoundX + c, topBoundX + r);
      }
    }
  }
}

void displayCurrentOctaveLEDs()
{
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
}