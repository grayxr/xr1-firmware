#ifndef XRConfig_h
#define XRConfig_h

#include <Arduino.h>

#define FIRMWARE_VERSION "0.1.0"

#define COMPANY_NAME "audio enjoyer"
#define DEVICE_NAME "xr-1"

#define DATA_FOLDER_NAME ".data"
#define MACHINE_STATE_FILENAME_PREFIX "machine_state"
#define PROJECTS_FOLDER_NAME "projects"

#define LOG_METRICS_ENABLED true
#define DISPLAY_MAX_WIDTH 128
#define DISPLAY_MAX_HEIGHT 64
#define MAX_PROJECT_RAW_SAMPLES 40
#define sd1 SD
#define SDCARD_CS_PIN BUILTIN_SDCARD
#define SDCARD_MOSI_PIN 43 // not actually used
#define SDCARD_SCK_PIN 45  // not actually used
#define DAC_SCK 27
#define DAC_MOSI 26
#define CS1 28
#define CS2 29
#define CS3 30
#define CS4 31
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define NUM_TLC5947 1    // How many boards do you have chained?
#define tlc5947_data 5
#define tlc5947_clock 6
#define tlc5947_latch 4
#define tlc5947_oe 3 // set to -1 to not use the enable pin (its optional)
#define HEADPHONE_PIN A8
#define FAST_TOUCH_PIN 32
#define MAX_USABLE_SAMPLE_IDS 256
#define MAX_USABLE_WAV_SAMPLE_IDS 512
#define MAX_SOUND_NAME_LENGTH 32
#define MAX_SAMPLE_NAME_LENGTH 32
#define MAX_PATTERN_SAMPLES 16 // there are only 16 tracks per patter, so a max of 16 samples per pattern, for now
// Sequencer data
#define MAXIMUM_SEQUENCER_STEPS 64
#define MAXIMUM_SEQUENCER_TRACKS 16
#define MAXIMUM_SEQUENCER_PATTERNS 16
#define MAXIMUM_SEQUENCER_BANKS 1
#define MAXIMUM_SOUND_PARAMS 30
#define MAXIMUM_DEXED_SOUND_PARAMS 156
#define DEFAULT_LAST_STEP 16
#define MAX_STEP_MOD_ATTRS 32
#define COMBO_VOICE_COUNT 4
#define SAMPLE_VOICE_COUNT 12
// Represents the maximum allowed steps triggered in a running pattern,
// this is tracked by the sequencer to know when to trigger noteOn/noteOff messages, etc
// 1 pattern * 16 tracks * 64 step max = 1024 possible steps while the sequencer is running
#define STEP_STACK_SIZE 1024
// 1 track * 16 steps * 96ppqn max = 1536
#define RATCHET_STACK_SIZE 1536
// Clock
#define MAXIMUM_GROOVE_CONFIGS 1
#define MAXIMUM_GROOVE_OPTIONS 5
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
#define START_BTN_CHAR 'q'
#define MOD_D_BTN_CHAR 'h'
// Encoders
#define MAIN_ENCODER_ADDRESS 0
#define MOD1_ENCODER_ADDRESS 1
#define MOD2_ENCODER_ADDRESS 2
#define MOD3_ENCODER_ADDRESS 3
#define MOD4_ENCODER_ADDRESS 4

#define NOTE_FREQ_C4 261.63

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

#define DEXED_SOUND_MENU_ITEM_MAX 4
#define SOUND_MENU_ITEM_MAX 3
#define SETUP_MENU_ITEM_MAX 3

#define MAXIMUM_TRACK_MODS 10
#define DEFAULT_GLOBAL_ACCENT 80

#endif /* XRConfig_h */