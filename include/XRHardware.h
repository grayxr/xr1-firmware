#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <SdFat.h>
#include <Audio.h>
#include <ResponsiveAnalogRead.h>
#include <TeensyVariablePlayback.h>
#include <synth_dexed.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_TLC5947.h>
#include <Adafruit_MPR121.h>
#include <FastTouch.h>
#include <Keypad.h>
// #include <MIDI.h>
#include <U8g2lib.h>
#include <string>
#include <vector>
#include <uClock.h>
#include <map>

#include <XRFonts.h>

namespace XRHardware {

class DeltaTracker
{
public:
  int lastVal = 0;
  int delta = 0;
  void Process(int curVal)
  {
    delta = curVal - lastVal;
    lastVal = curVal;
  }
  int GetDelta()
  {
    return delta;
  }
};

class Encoders
{
  int addresses[5] = {
      0x36, 0x37, 0x38, 0x39, 0x40};

public:
  char *_ENC_NAME_STRINGS[5] = {
      "MAIN",
      "MOD1",
      "MOD2",
      "MOD3",
      "MOD4"};
  typedef enum
  {
    ENC_MAIN_ADDRESSS,
    ENC_MOD1_ADDRESS,
    ENC_MOD2_ADDRESS,
    ENC_MOD3_ADDRESS,
    ENC_MOD4_ADDRESS,
    ENC_NAME_LAST
  } _ENC_NAMES;
  void config(int addr, int16_t rmin, int16_t rmax, int16_t rstep, int16_t rval, uint8_t rloop)
  {
    Wire1.beginTransmission(addr);
    Wire1.write((uint8_t)(rval & 0xff));
    Wire1.write((uint8_t)(rval >> 8));
    Wire1.write(0);
    Wire1.write(rloop);
    Wire1.write((uint8_t)(rmin & 0xff));
    Wire1.write((uint8_t)(rmin >> 8));
    Wire1.write((uint8_t)(rmax & 0xff));
    Wire1.write((uint8_t)(rmax >> 8));
    Wire1.write((uint8_t)(rstep & 0xff));
    Wire1.write((uint8_t)(rstep >> 8));
    Wire1.endTransmission();
  }
  void Set(int addressIndex, int16_t rval)
  {
    Wire1.beginTransmission(addresses[addressIndex]);
    Wire1.write((uint8_t)(rval & 0xff));
    Wire1.write((uint8_t)(rval >> 8));
    Wire1.endTransmission();
  }
  int16_t Get(int addressIndex)
  {
    Wire1.requestFrom(addresses[addressIndex], 2);
    return ((uint16_t)Wire1.read() | ((uint16_t)Wire1.read() << 8));
  }

public:
  DeltaTracker deltas[5];
  void Init()
  {
    Wire1.begin();
    for (int i = 0; i < 5; i++)
    {
      config(addresses[i], -3000, 3000, 1, 0, 0);
    }
  }
  void Update()
  {
    for (int i = 0; i < 5; i++)
    {
      deltas[i].Process(Get(i));
    }
  }
};

class LEDs
{
public:
  uint16_t pins[22] = {0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15, 16, 22, 23, 17, 18, 19, 20};
  typedef enum
  {
    LED_GRID_1,
    LED_GRID_2,
    LED_GRID_3,
    LED_GRID_4,
    LED_GRID_5,
    LED_GRID_6,
    LED_GRID_7,
    LED_GRID_8,
    LED_GRID_9,
    LED_GRID_10,
    LED_GRID_11,
    LED_GRID_12,
    LED_GRID_13,
    LED_GRID_14,
    LED_GRID_15,
    LED_GRID_16,
    LED_STOP,
    LED_START,
    LED_PAGE_1,
    LED_PAGE_2,
    LED_PAGE_3,
    LED_PAGE_4
  } LEDNames;
  Adafruit_TLC5947 tlc;
  LEDs() : tlc(1 /* NUM_TLC5947 */, 6 /* tlc5947_data */, 5 /* tlc5947_clock */, 4 /* tlc5947_latch */) {}
  void Init()
  {
    tlc.begin();
    if (3 /* tlc5947_oe */ >= 0)
    {
      pinMode(3 /* tlc5947_oe */, OUTPUT);
      digitalWrite(3 /* tlc5947_oe */, LOW);
    }
    for (int i = 0; i < 25; i++)
    {
      setPWM(i, 0); // sets all 24 outputs to no brightness
    }
  }
  void setPWM(int LED, uint16_t PWM)
  {
    tlc.setPWM(pins[LED], PWM);
    tlc.write();
  }
};

class Buttons
{
  Keypad kpd;

public:
  uint8_t _BUTTONS_ROW_PINS[6] = {38, 37, 36, 35, 34, 33}; // connect to the row pinouts of the keypad
  uint8_t _BUTTONS_COL_PINS[6] = {2, 9, 12, 41, 40, 39};   // connect to the column pinouts of the keypad

  typedef enum
  {
    BTN_GRID_1,
    BTN_GRID_2,
    BTN_GRID_3,
    BTN_GRID_4,
    BTN_GRID_5,
    BTN_GRID_6,
    BTN_GRID_7,
    BTN_GRID_8,
    BTN_GRID_9,
    BTN_GRID_10,
    BTN_GRID_11,
    BTN_GRID_12,
    BTN_GRID_13,
    BTN_GRID_14,
    BTN_GRID_15,
    BTN_GRID_16,
    BTN_STOP,
    BTN_START,
    BTN_PAGE_LEFT,
    BTN_PAGE_RIGHT,
    BTN_SEL,
    BTN_ESC,
    BTN_FN,
    BTN_COPY_PASTE,
    BTN_TEMPO_CLOCK,
    BTN_SOUND_SETUP,
    BTN_PERFORM_SONG,
    BTN_PATTERN_BANK,
    BTN_TRACK_LAYER,
    BTN_MOD_A,
    BTN_MOD_B,
    BTN_MOD_C,
    BTN_MOD_D,
    BTN_DATA,
    BTN_UNKNOWN_1,
    BTN_UNKNOWN_2,
    BTN_LAST
  } ButtonNames;

  KeyState buttonStates[BTN_LAST];

  char _BUTTONS_KEYMAP[6][6] = {
      {BTN_PERFORM_SONG, BTN_PATTERN_BANK, BTN_TRACK_LAYER, BTN_DATA, BTN_MOD_A, BTN_MOD_B},
      {BTN_MOD_C, BTN_MOD_D, BTN_SEL, BTN_ESC, BTN_UNKNOWN_1, BTN_UNKNOWN_2},
      {BTN_GRID_1, BTN_GRID_2, BTN_GRID_3, BTN_GRID_4, BTN_START, BTN_FN},
      {BTN_GRID_5, BTN_GRID_6, BTN_GRID_7, BTN_GRID_8, BTN_STOP, BTN_COPY_PASTE},
      {BTN_GRID_9, BTN_GRID_10, BTN_GRID_11, BTN_GRID_12, BTN_PAGE_RIGHT, BTN_TEMPO_CLOCK},
      {BTN_GRID_13, BTN_GRID_14, BTN_GRID_15, BTN_GRID_16, BTN_PAGE_LEFT, BTN_SOUND_SETUP},
  };
  Buttons() : kpd(makeKeymap(_BUTTONS_KEYMAP), _BUTTONS_ROW_PINS, _BUTTONS_COL_PINS, 6, 6) {}
  void Init()
  {
    kpd.setHoldTime(150);
    for(int i=0; i<BTN_LAST; i++) buttonStates[i] = KeyState::IDLE;
  }
  bool Update()
  {
    bool activeKeys = kpd.getKeys();
    for (size_t i=0; i<LIST_MAX; i++) {
        if(buttonStates[kpd.key[i].kchar] == KeyState::PRESSED) buttonStates[kpd.key[i].kchar] = KeyState::HOLD;
        if(buttonStates[kpd.key[i].kchar] == KeyState::RELEASED) buttonStates[kpd.key[i].kchar] = KeyState::IDLE;
    }
    if(activeKeys) {
        for (size_t i=0; i<LIST_MAX; i++) {
            if (kpd.key[i].stateChanged) {
                buttonStates[kpd.key[i].kchar] = kpd.key[i].kstate;
            }
        }
    }
    return activeKeys;
  }
};

class Keyboard
{
  Adafruit_MPR121 mpr121_a = Adafruit_MPR121();
  uint16_t lastTouched = 0;
  uint16_t currentTouched = 0;

public:
  typedef enum
  {
    HOLD,
    IDLE,
    PRESSED,
    RELEASED
  } KeyState;
  Keyboard() {}
  void Init() { mpr121_a.begin(0x5B, &Wire2); }
  void Update()
  {
    lastTouched = currentTouched;
    currentTouched = mpr121_a.touched() | ((fastTouchRead(32) >= 64) ? 1 << 12 : 0);
  }
  KeyState GetKeyState(int key)
  {
    key = 12 - key;
    KeyState state = IDLE;
    if (key >= 0 && key < 13)
    {
      if ((currentTouched & (1 << key)) && !(lastTouched & (1 << key)))
        state = PRESSED;
      if ((currentTouched & (1 << key)) && (lastTouched & (1 << key)))
        state = HOLD;
      if (!(currentTouched & (1 << key)) && (lastTouched & (1 << key)))
        state = RELEASED;
    }
    return state;
  }
};

class Hardware
{
public:
  U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI display;
  AudioControlSGTL5000 codec;
  Encoders encoders;
  LEDs leds;
  Buttons buttons;
  Keyboard keyboard;
  Hardware() : display(U8G2_R0, /* cs=*/10, /* dc=*/14, /* reset=*/15) {}
  void Init()
  {
    // init display
    display.begin();
    display.setContrast(0);
    display.setFont(small_font);
    display.setFontRefHeightAll();
    display.setDrawColor(1);
    display.setFontPosTop();
    display.setFontDirection(0);

    AudioMemory(400);
    // Comment these out if not using the audio adaptor board.
    // This may wait forever if the SDA & SCL pins lack
    // pullup resistors
    codec.enable();
    codec.inputSelect(AUDIO_INPUT_LINEIN);
    codec.lineInLevel(5);
    codec.lineOutLevel(29);
    codec.dacVolumeRamp();
    codec.volume(0.90);

    encoders.Init();
    leds.Init();
    buttons.Init();
    keyboard.Init();
  }
  void UpdateControls()
  {
    buttons.Update();
    encoders.Update();
    keyboard.Update();
  }
};

}