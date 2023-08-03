cmake_minimum_required(VERSION 3.22)

# project(AudioSpectrum_Tests)

set(TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Test_SpiBuffer.c)

set(TEST_PRODUCTION_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Src/spiBuffer.c)

set(UNITY_INCLUDES
        unity/core/
        unity/fixture/src/
        )

add_subdirectory(unity)

add_executable(Test_SpiBuffer main.c ${TEST_FILES} ${TEST_PRODUCTION_FILES})

#to add condition macro for testing purposes
set_source_files_properties(
    ${TEST_PRODUCTION_FILES}
    PROPERTIES
    COMPILE_FLAGS "-D TESTING"
)

target_link_libraries(Test_SpiBuffer PRIVATE unity)
target_include_directories(Test_SpiBuffer PRIVATE ../Core/Inc/)

add_test(NAME spiBuffer COMMAND Test_SpiBuffer)
