cmake_minimum_required(VERSION 3.10)
project(xr1_x86_firmware C CXX)
set(CMAKE_CXX_STANDARD 17)

add_definitions(-DNO_DEXED) # NO_DEXED
add_definitions(-DNO_BRAIDS) # NO_BRAIDS
add_definitions(-DNO_FMDRUM) # NO_FMDRUM
add_definitions(-DARDUINO)

find_package(teensy_x86_stubs)
include_directories(${teensy_x86_stubs_INCLUDE_DIR})

find_package(teensy_x86_spi_stubs)
include_directories(${teensy_x86_spi_stubs_INCLUDE_DIR})

find_package(teensy_x86_sd_stubs)
include_directories(${teensy_x86_sd_stubs_INCLUDE_DIR})

find_package(teensy_x86_midi_stubs)
include_directories(${teensy_x86_midi_stubs_INCLUDE_DIR})

find_package(teensy_x86_tlc5947_stubs)
include_directories(${teensy_x86_tlc5947_stubs_INCLUDE_DIR})

find_package(teensy_x86_mpr121_stubs)
include_directories(${teensy_x86_mpr121_stubs_INCLUDE_DIR})

find_package(teensy_x86_twowire_stubs)
include_directories(${teensy_x86_twowire_stubs_INCLUDE_DIR})


find_package(teensy_audio_x86_stubs)
include_directories(${teensy_audio_x86_stubs_INCLUDE_DIR})

find_package(teensy_audio_x86_stubs_soundio)
include_directories(${teensy_audio_x86_stubs_soundio_INCLUDE_DIR})

find_package(teensy_x86_uClock_stubs)
include_directories(${teensy_x86_uClock_stubs_INCLUDE_DIR})

find_package(teensy_x86_u8g2_stubs)
include_directories(${teensy_x86_u8g2_stubs_INCLUDE_DIR})

find_package(teensy_variable_playback)
include_directories(${teensy_variable_playback_INCLUDE_DIR})

add_definitions(-DU8X8_USE_PINS)
find_package(teensy_x86_u8g2_stubs)
include_directories(${teensy_x86_u8g2_stubs_INCLUDE_DIR})

find_package(teensy_x86_keypad_stubs)
include_directories(${teensy_x86_keypad_stubs_INCLUDE_DIR})

find_package(teensy_x86_responsiveanalogread_stubs)
include_directories(${teensy_x86_responsiveanalogread_stubs_INCLUDE_DIR})

find_package(teensy_x86_time_stubs)
include_directories(${teensy_x86_time_stubs_INCLUDE_DIR})

find_package(teensy_x86_midi_stubs)
include_directories(${teensy_x86_midi_stubs_INCLUDE_DIR})

find_package(teensy_sampleflashloader)
include_directories(${teensy_sampleflashloader_INCLUDE_DIR})

find_package(teensy_x86_midi_stubs_rtmidi)
include_directories(${teensy_x86_midi_stubs_rtmidi_INCLUDE_DIR})

find_package(teensy_x86_u8g2_stubs_opengl)
include_directories(${teensy_x86_u8g2_stubs_opengl_INCLUDE_DIR})

find_path(RTMIDI_INCLUDE_DIR RtMidi.h
  PATHS
    /usr/local/include
    /usr/include
    /usr/include/rtmidi)
include_directories(${RTMIDI_INCLUDE_DIR})

include(opengl.cmake.in)

add_subdirectory(lib/FastTouch/src)
include_directories(include)
include_directories(lib/FastTouch/src)

add_definitions(-DBUILD_FOR_LINUX)

add_executable(xr1
        src/main.cpp
        src/XRAsyncPSRAMLoader.cpp
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
target_link_libraries(xr1 ${teensy_audio_x86_stubs_soundio_LIBS} ${teensy_x86_responsiveanalogread_stubs_LIBS} ${teensy_sampleflashloader_LIBS} ${teensy_x86_keypad_stubs_LIBS} ${teensy_x86_midi_stubs_rtmidi_LIBS} ${teensy_x86_time_stubs_LIBS} ${teensy_x86_u8g2_stubs_LIBS} ${teensy_x86_midi_stubs_LIBS} ${teensy_variable_playback_LIBS} ${teensy_x86_u8g2_stubs_LIBS} ${teensy_x86_uClock_stubs_LIBS} ${teensy_x86_IntervalTimer_stubs_LIBS} ${teensy_audio_x86_stubs_LIBS} ${teensy_x86_twowire_stubs_LIBS} ${teensy_x86_mpr121_stubs_LIBS} ${teensy_x86_tlc5947_stubs_LIBS}  ${teensy_x86_midi_stubs_LIBS}  ${teensy_x86_sd_stubs_LIBS} fasttouch ${teensy_x86_spi_stubs_LIBS} ${teensy_x86_stubs_LIBS})
target_link_libraries(xr1 -L/usr/local/lib/ rtmidi)
target_link_libraries(xr1 "-L/usr/local/lib -lsoundio")

target_link_libraries(xr1 ${teensy_x86_u8g2_stubs_LIBS})
target_link_libraries(xr1 -L../lib imgui)

target_link_libraries(xr1 ${LIBS})

if  (APPLE)
    target_link_libraries(xr1
            "-framework CoreServices"
            "-framework CoreAudio"
            "-framework Foundation"
            "-framework AudioUnit")
endif()
#set(CMAKE_VERBOSE_MAKEFILE 1)
