cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 14)

project(teensy_libsynthdexed)

teensy_add_library(synthdexed
        src/dexed.cpp
        src/dx7note.cpp
        src/EngineMkI.cpp
        src/EngineMsfa.cpp
        src/EngineOpl.cpp
        src/env.cpp
        src/exp2.cpp
        src/fm_core.cpp
        src/fm_op_kernel.cpp
        src/freqlut.cpp
        src/lfo.cpp
        src/pitchenv.cpp
        src/PluginFx.cpp
        src/porta.cpp
        src/sin.cpp
        src/synth_dexed.cpp
)
