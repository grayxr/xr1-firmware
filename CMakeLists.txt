cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 14)
set(TEENSY_VERSION 41 CACHE STRING "Set to the Teensy version corresponding to your board (30 or 31 allowed)" FORCE)
set(CPU_CORE_SPEED 600000000 CACHE STRING "Set to 24000000, 48000000, 72000000 or 96000000 to set CPU core speed" FORCE) # Derived variables
set(CMAKE_EXE_LINKER_FLAGS "--specs=nano.specs" CACHE INTERNAL "")

#teensy compiler options - CHANGE THIS TO POINT TO YOUR GCC-ARM-NONE-EABI
set(COMPILERPATH "/opt/gcc-arm-none-eabi-10.3-2021.10/bin/")
#set(COMPILERPATH "/Applications/ARM_10/bin/")

#teensy compiler options - CHANGE THIS TO POINT TO YOUR Dependencies folder
set(DEPSPATH "/home/runner/work/xr1-firmware/xr1-firmware/deps")
set(COREPATH "${DEPSPATH}/cores/teensy4/")

find_package(teensy_cmake_macros)
project(teensys_sample_flashloader)

import_arduino_library(cores ${COREPATH} avr debug util)
import_arduino_library(SPI ${DEPSPATH}/SPI)
import_arduino_library(SdFat ${DEPSPATH}/SdFat/src common DigitalIO ExFatLib FatLib FsLib iostream SdCard SpiDriver)
import_arduino_library(SD ${DEPSPATH}/SD/src)
import_arduino_library(LittleFS ${DEPSPATH}/LittleFS/src)
import_arduino_library(SerialFlash ${DEPSPATH}/SerialFlash util)
import_arduino_library(Wire ${DEPSPATH}/Wire utility)
import_arduino_library(arm_math ${DEPSPATH}/arm_math/src)
import_arduino_library(Audio ${DEPSPATH}/Audio utility)
import_arduino_library(TeensyVariablePlayback ${DEPSPATH}/teensy-variable-playback/src)
import_arduino_library(Adafruit_BusIO ${DEPSPATH}/Adafruit_BusIO)
import_arduino_library(Adafruit_MPR121 ${DEPSPATH}/Adafruit_MPR121)
import_arduino_library(Adafruit_TLC5947 ${DEPSPATH}/Adafruit_TLC5947)
import_arduino_library(Keypad ${DEPSPATH}/Keypad/src)
import_arduino_library(ResponsiveAnalogRead ${DEPSPATH}/ResponsiveAnalogRead/src)
import_arduino_library(TeensySampleFlashLoader ${DEPSPATH}/teensy-sample-flashloader/src)
import_arduino_library(uClock ${DEPSPATH}/uClock/src)
import_arduino_library(arduino_midi_library ${DEPSPATH}/arduino_midi_library/src)
import_arduino_library(FastTouch ${DEPSPATH}/FastTouch/src)
import_arduino_library(U8g2 ${DEPSPATH}/U8g2 cppsrc csrc)
import_arduino_library(Time ${DEPSPATH}/Time)

set(CMAKE_VERBOSE_MAKEFILE)

add_subdirectory(lib/synth_dexed)

teensy_include_directories(include)
teensy_include_directories(lib/synth_dexed/src)

teensy_add_executable(xr1firmware
        src/main.cpp
        src/XRAudio.cpp
        src/XRAudioConfig.cpp
        src/XRClock.cpp
        src/XRCV.cpp
        src/XRDisplay.cpp
        src/XREncoder.cpp
        src/XRHelpers.cpp
        src/XRKeyInput.cpp
        src/XRKeyMatrix.cpp
        src/XRLED.cpp
        src/XRMain.cpp
        src/XRMenu.cpp
        src/XRMIDI.cpp
        src/XRSD.cpp
        src/XRSequencer.cpp
        src/XRSound.cpp
        src/XRUX.cpp
        src/XRVersa.cpp)

teensy_target_link_libraries(xr1firmware synthdexed Audio arm_math Time U8g2 FastTouch arduino_midi_library  Adafruit_MPR121 Adafruit_TLC5947 Adafruit_BusIO Keypad ResponsiveAnalogRead uClock  TeensySampleFlashLoader  TeensyVariablePlayback   SD SdFat  SPI SerialFlash  Wire  cores)
target_link_libraries(xr1firmware.elf stdc++ m)
