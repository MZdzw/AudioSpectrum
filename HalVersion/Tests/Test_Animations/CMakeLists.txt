cmake_minimum_required(VERSION 3.22)

SET(CMAKE_EXE_LINKER_FLAGS  "-lm")

set(TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Test_Animations.c)
set(PRODUCTION_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/animations.c
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/ws2812bDriver.c   
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/spiBuffer.c)

set(PRODUCTION_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc
                            ${CMAKE_CURRENT_SOURCE_DIR}/../cmock/src/
)

set(MOCKED_INCLUDES ${CMAKE_SOURCE_DIR}/Tests/cmock_files/)
set(MOCKED_SOURCES ${CMAKE_SOURCE_DIR}/Tests/cmock_files/mock_stm32f4xx_hal_spi.c)

add_executable(Test_Animations main.c ${TEST_FILES} ${PRODUCTION_FILES} ${MOCKED_SOURCES})


target_compile_definitions(Test_Animations PRIVATE TESTING=1)
target_link_libraries(Test_Animations PRIVATE -lm)

target_link_libraries(Test_Animations PRIVATE unity cmock)
target_include_directories(Test_Animations PRIVATE ${PRODUCTION_INCLUDE_PATH} ${MOCKED_INCLUDES})

add_test(NAME Animations COMMAND Test_Animations)
