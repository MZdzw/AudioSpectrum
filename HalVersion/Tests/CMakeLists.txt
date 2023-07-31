cmake_minimum_required(VERSION 3.22)

project(AudioSpectrum_Tests)

set(TESTED_MODULES ../Core/Src/TestTemplate.c)

set(TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/TestTemplate.c)

set(UNITY_INCLUDES
        unity/core/
        unity/fixture/src/
        )

add_subdirectory(unity)

add_executable(AudioSpectrum_Tests main.c ${TEST_FILES})
target_link_libraries(AudioSpectrum_Tests PRIVATE unity)
target_include_directories(AudioSpectrum_Tests PRIVATE ../Core/Inc/)

add_test(NAME AudioSpectrum COMMAND AudioSpectrum_Tests)
