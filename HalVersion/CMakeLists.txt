cmake_minimum_required(VERSION 3.22)

option(BUILD_TEST "Build tests" OFF)
if(NOT BUILD_TEST)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/CMake/toolchain-arm-gcc.cmake)
endif()

project(AudioSpectrumHAL)

if(BUILD_TEST)
    enable_testing()
    add_subdirectory(Tests)
else()
    add_subdirectory(Core)
endif()