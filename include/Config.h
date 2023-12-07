#ifndef Config_h
#define Config_h

#include <MIDI.h>
#include <Arduino.h>
#include <Audio.h>
#include <Adafruit_TLC5947.h>
#include <Adafruit_MPR121.h>
#include <FastTouch.h>
#include <Keypad.h>
#include <uClock.h>
#include <U8g2lib.h>
#include <ResponsiveAnalogRead.h>
#include <TeensyVariablePlayback.h>
#include "flashloader.h"
#include <map>
#include <string>
#include <TimeLib.h>

#define FIRMWARE_VERSION "0.1.0"

#define LOG_METRICS_ENABLED true

#define DISPLAY_MAX_WIDTH 128
#define DISPLAY_MAX_HEIGHT 64

#define MAX_PROJECT_RAW_SAMPLES 40

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

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// How many boards do you have chained?
#define NUM_TLC5947 1

#define tlc5947_data 5
#define tlc5947_clock 6
#define tlc5947_latch 4
#define tlc5947_oe 3  // set to -1 to not use the enable pin (its optional)

#define MAX_USABLE_SAMPLE_IDS 256
#define MAX_USABLE_WAV_SAMPLE_IDS 512
#define MAX_SAMPLE_NAME_LENGTH 32

// Sequencer data
#define MAXIMUM_SEQUENCER_STEPS 64
#define MAXIMUM_SEQUENCER_TRACKS 16
#define MAXIMUM_SEQUENCER_PATTERNS 16
#define MAXIMUM_SEQUENCER_BANKS 1
#define DEFAULT_LAST_STEP 16

// Buttons
#define PERFORM_BTN_CHAR 'a'
#define PATTERN_BTN_CHAR 'b'
#define TRACK_BTN_CHAR 'c'
#define FUNCTION_BTN_CHAR 'r'
#define COPY_BTN_CHAR 'x'
#define TEMPO_BTN_CHAR '4'
#define SOUND_BTN_CHAR '0'
#define SELECT_BTN_CHAR 'i' // reverse when silkscreen is corrected
#define ESCAPE_BTN_CHAR 'j' // reverse when silkscreen is corrected

#define COMBO_VOICE_COUNT 4
#define SAMPLE_VOICE_COUNT 12

#define STARS 500

float star_x[STARS], star_y[STARS], star_z[STARS]; 

const byte chan_a_low_gain = 0b00000000;
const byte chan_b_low_gain = 0b10000000;
const byte chan_a_high_gain = 0b00010000;
const byte chan_b_high_gain = 0b10010000;

time_t RTCTime;

unsigned long lastSamplePlayed = 0;
uint16_t wavSamplesAvailable = 0;
uint16_t rawSamplesAvailable = 0;

U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 14, /* reset=*/ 15);

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
  { 'i', "ESC" },
  { 'j', "SEL" },
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

char usableSampleNames[MAX_USABLE_SAMPLE_IDS][MAX_SAMPLE_NAME_LENGTH];
char usableWavSampleNames[MAX_USABLE_SAMPLE_IDS][MAX_SAMPLE_NAME_LENGTH];

ResponsiveAnalogRead analog(A8, true); // headphone pot

float hp_vol_cur = 0.5;

enum WAVEFORM_TYPE {
  SAW = WAVEFORM_SAWTOOTH,
  RSAW = WAVEFORM_SAWTOOTH_REVERSE,
  TRI = WAVEFORM_TRIANGLE,
  SQUARE = WAVEFORM_SQUARE,
  PULSE = WAVEFORM_PULSE,
  SINE = WAVEFORM_SINE,
};

// sequencer

enum TRACK_TYPE {
  MIDI_OUT = 0,
  CV_TRIG = 1,
  CV_GATE = 2,
  SUBTRACTIVE_SYNTH = 3,
  RAW_SAMPLE = 4,
  WAV_SAMPLE = 5,
  DEXED = 6,
};

enum TRACK_STEP_STATE {
  STATE_OFF = 0,
  STATE_ON = 1,
  STATE_ACCENTED = 2
};

typedef struct
{
  TRACK_STEP_STATE state = TRACK_STEP_STATE::STATE_OFF;
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
  bool initialized = false;
} TRACK;

typedef struct
{
  TRACK tracks[MAXIMUM_SEQUENCER_TRACKS];
  uint8_t last_step = DEFAULT_LAST_STEP;
  uint8_t groove_amount = 0;
  int8_t groove_id = -1;
  bool initialized = false;
} PATTERN;

typedef struct
{
  PATTERN patterns[MAXIMUM_SEQUENCER_PATTERNS];
} BANK;

typedef struct
{
  BANK banks[MAXIMUM_SEQUENCER_BANKS];
} SEQUENCER_EXTERNAL;

DMAMEM SEQUENCER_EXTERNAL _seq_external;
DMAMEM PATTERN _pattern_copy_buf;
DMAMEM TRACK _track_copy_buf;
DMAMEM TRACK_STEP _step_copy_buf;

DMAMEM PATTERN _rec_pattern_buf;

typedef struct
{
  PATTERN pattern;
} SEQUENCER_HEAP;

SEQUENCER_HEAP _seq_heap;

// pattern mods

typedef struct
{
  uint8_t raw_sample_id = 0;
  uint8_t wav_sample_id = 0;
  uint8_t waveform = WAVEFORM_TYPE::SAW;
  uint8_t note = 0; // 0 - C
  uint8_t octave = 4; // 4 - middle C (C4)
  uint8_t length = 4; // 4 = 1/16
  uint8_t velocity = 50; // 1 - 100%
  uint8_t probability = 0; // 1 = 100ms
  int8_t microtiming = 0; // 1 = 100ms
  int8_t detune = 0;
  int8_t fine = 0;
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
} TRACK_STEP_MODS;

#define MAX_STEP_MOD_ATTRS 33

enum MOD_ATTRS {
  RAW_SAMPLE_ID = 0,
  WAV_SAMPLE_ID,
  WAVEFORM,
  NOTE,
  OCTAVE,
  LENGTH,
  VELOCITY,
  PROBABILITY,
  MICROTIMING,
  DETUNE,
  FINE,
  LOOPTYPE,
  LOOPSTART,
  LOOPFINISH,
  PLAYSTART,
  LEVEL,
  PAN,
  SAMPLE_PLAY_RATE,
  WIDTH,
  OSCALEVEL,
  OSCBLEVEL,
  CUTOFF,
  RES,
  F_ATTACK,
  F_SUSTAIN,
  F_DECAY,
  F_RELEASE,
  A_ATTACK,
  A_SUSTAIN,
  A_DECAY,
  A_RELEASE,
  NOISE,
};

typedef struct
{
  bool flags[MAX_STEP_MOD_ATTRS];
} TRACK_STEP_MOD_FLAGS;

typedef struct
{
  TRACK_STEP_MODS steps[MAXIMUM_SEQUENCER_STEPS];
  TRACK_STEP_MOD_FLAGS step_mod_flags[MAXIMUM_SEQUENCER_STEPS];
} TRACK_MODS;

typedef struct
{
  TRACK_MODS tracks[MAXIMUM_SEQUENCER_TRACKS];
} PATTERN_MODS;

DMAMEM PATTERN_MODS _pattern_mods_mem;

// seq state

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

bool _recording = false; // add to seq state struct eventually

typedef struct
{
  int8_t trackNum = -1;
  int8_t stepNum = -1;
  int8_t length = -1; // need -1 to know if active/not active 
  // int8_t microtiming;
} STACK_STEP_DATA;

typedef struct
{
  int8_t trackNum = -1;
  int8_t length = -1;
} STACK_RATCHET_DATA;

// Represents the maximum allowed steps triggered in a running pattern,
// this is tracked by the sequencer to know when to trigger noteOn/noteOff messages, etc
// 1 pattern * 16 tracks * 64 step max = 1024 possible steps while the sequencer is running
#define STEP_STACK_SIZE 1024
STACK_STEP_DATA _step_stack[STEP_STACK_SIZE];

// 1 track * 16 steps * 96ppqn max = 1536
#define RATCHET_STACK_SIZE 1536
STACK_RATCHET_DATA _ratchet_stack[RATCHET_STACK_SIZE];

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
  char name[22];
  char version[12];
  float tempo = 120.0;
  PROJECT_CLOCK_SETTINGS clock_settings;
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
  COPY_SEL,
  COPY_PATTERN,
  COPY_TRACK,
  COPY_STEP,
  PASTE_SEL,
  PASTE_PATTERN,
  PASTE_TRACK,
  PASTE_STEP,
  UNDO,
  REDO,
};

UI_MODE previous_UI_mode = PATTERN_WRITE; // the default mode
UI_MODE current_UI_mode = PATTERN_WRITE; // the default mode

int8_t current_selected_bank = 0; // default to 0 (first)
int8_t current_selected_pattern = 0; // default to 0 (first)
int8_t current_selected_track = 0; // default to 0 (first)
int8_t current_selected_step = -1; // default to -1 (none)
int8_t current_step_page = 1;

int8_t current_tracks_soloed = 0;

bool pattern_copy_available = false;
bool track_copy_available = false;
bool step_copy_available = false;

typedef struct 
{
  int bank = -1;
  int number = -1;
} QUEUED_PATTERN;

QUEUED_PATTERN _queued_pattern;

int draw_queue_blink = -1;
bool dequeue_pattern = false;

int ratcheting_track = -1;
int ratchet_division = -1;
long ratchetReleaseTime;
int ratchets_held = 0;

int8_t keyboardOctave = 4; // range 1-7 ?

std::map<uint8_t, uint8_t> keyed_ratchet_divisions = {
  {0, 24}, // 1/4 note
  {1, 16}, // 1/6 note
  {2, 12}, // 1/8 note
  {3, 8}, // 1/12 note
  {4, 6}, // 1/16 note
  {5, 3}, // 1/32 note
  {6, 4}, // 1/24 note
  {7, 2}, // 1/48 note
  {8, 24}, // 1/24 note
  {9, 1}, // 1/96 note
  {10, 24}, // 1/4 note
  {11, 24}, // 1/4 note
  {12, 24}, // 1/4 note
};

bool function_started = false;
bool track_sel_btn_held = false;

int8_t bnk_held_for_selection = -1;
int8_t patt_held_for_selection = -1;
int8_t track_held_for_selection = -1; // default to -1 (none)
int8_t perf_held_for_selection = -1;

const uint8_t backwardsNoteNumbers[13] = {
  12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
};

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

int keyboardNotesHeld = 0;

bool checked_remaining_seq_notes = false;

bool toggled_queue_ind_off = false;

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

std::map<TRACK_TYPE, int> trackPageNumMap = {
  { SUBTRACTIVE_SYNTH, 6},
  { DEXED, 1},
  { RAW_SAMPLE, 4},
  { WAV_SAMPLE, 1},
  { MIDI_OUT, 1},
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
  { DEXED, {
    {0, "MAIN"},
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
  { MIDI_OUT, {
    {0, "MAIN"},
  }},
  { CV_GATE, {
    {0, "MAIN"},
  }},
  { CV_TRIG, {
    {0, "MAIN"},
  }},
};

enum SOUND_CONTROL_MOD_TYPE
{
  DEFAULT = 0,
  RANGE,
  BAR,
  DIAL,
};

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
  float aFloatValue;
  float bFloatValue;
  float cFloatValue;
  float dFloatValue;
  SOUND_CONTROL_MOD_TYPE aType;
  SOUND_CONTROL_MOD_TYPE bType;
  SOUND_CONTROL_MOD_TYPE cType;
  SOUND_CONTROL_MOD_TYPE dType;
} SOUND_CONTROL_MODS;

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

elapsedMillis elapsed;

std::string new_project_name;

  // MPC60 groove signatures?
  uint8_t current_shuffle = 0;
  int8_t shuffle_54[2] = {0, 1};
  int8_t shuffle_58[2] = {0, 2};
  int8_t shuffle_62[2] = {0, 3};
  int8_t shuffle_66[2] = {0, 4};
  int8_t shuffle_71[2] = {0, 5};
  std::string groove_names[1] = {"909"};
  std::string shuffle_name[5] = {"54%", "58%", "62%", "66%", "71%"};
  int8_t* shuffle_templates[5] = {shuffle_54, shuffle_58, shuffle_62, shuffle_66, shuffle_71};


  // off-tempo hiphop style? 
  int8_t shuffle_off_tempo[16] = {0, -1, 0, -2, 0, -1, 0, 2, 0, -1, 0, -2, 0, 3, 0, -1};


#define MAXIMUM_GROOVE_CONFIGS 1
#define MAXIMUM_GROOVE_OPTIONS 5

typedef struct {
  std::string name;
  std::string option_names[MAXIMUM_GROOVE_OPTIONS];
  int8_t* templates[MAXIMUM_GROOVE_OPTIONS];
} GROOVE_CONFIG;

typedef struct {
  GROOVE_CONFIG configs[MAXIMUM_GROOVE_CONFIGS];
} GROOVES;

GROOVE_CONFIG _tr909_groove = {
  "909", 
  {"54%", "58%", "62%", "66%", "71%"},
  {shuffle_54, shuffle_58, shuffle_62, shuffle_66, shuffle_71}
};

GROOVES _grooves = {{_tr909_groove}};

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

int cvLevels[128];

bool _trkNeedsInit[MAXIMUM_SEQUENCER_TRACKS] = {
  false, false, false, false,
  false, false, false, false,
  false, false, false, false,
  false, false, false, false
};

uint8_t fmpiano_sysex[156] = {
  95, 29, 20, 50, 99, 95, 00, 00, 41, 00, 19, 00, 00, 03, 00, 06, 79, 00, 01, 00, 14, // OP6 eg_rate_1-4, level_1-4, kbd_lev_scl_brk_pt, kbd_lev_scl_lft_depth, kbd_lev_scl_rht_depth, kbd_lev_scl_lft_curve, kbd_lev_scl_rht_curve, kbd_rate_scaling, amp_mod_sensitivity, key_vel_sensitivity, operator_output_level, osc_mode, osc_freq_coarse, osc_freq_fine, osc_detune
  95, 20, 20, 50, 99, 95, 00, 00, 00, 00, 00, 00, 00, 03, 00, 00, 99, 00, 01, 00, 00, // OP5
  95, 29, 20, 50, 99, 95, 00, 00, 00, 00, 00, 00, 00, 03, 00, 06, 89, 00, 01, 00, 07, // OP4
  95, 20, 20, 50, 99, 95, 00, 00, 00, 00, 00, 00, 00, 03, 00, 02, 99, 00, 01, 00, 07, // OP3
  95, 50, 35, 78, 99, 75, 00, 00, 00, 00, 00, 00, 00, 03, 00, 07, 58, 00, 14, 00, 07, // OP2
  96, 25, 25, 67, 99, 75, 00, 00, 00, 00, 00, 00, 00, 03, 00, 02, 99, 00, 01, 00, 10, // OP1
  94, 67, 95, 60, 50, 50, 50, 50,                                                     // 4 * pitch EG rates, 4 * pitch EG level
  04, 06, 00,                                                                         // algorithm, feedback, osc sync
  34, 33, 00, 00, 00, 04,                                                             // lfo speed, lfo delay, lfo pitch_mod_depth, lfo_amp_mod_depth, lfo_sync, lfo_waveform
  03, 24,                                                                             // pitch_mod_sensitivity, transpose
  70, 77, 45, 80, 73, 65, 78, 79, 00, 00                                              // 10 * char for name ("DEFAULT   ")
}; // FM-Piano


#endif /* Config_h */