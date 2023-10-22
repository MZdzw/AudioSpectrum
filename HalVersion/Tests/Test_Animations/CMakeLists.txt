cmake_minimum_required(VERSION 3.22)

SET(CMAKE_EXE_LINKER_FLAGS  "-lm")

set(TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Test_Animations.c)
set(PRODUCTION_FILES ${CMAKE_SOURCE_DIR}/Core/Src/animations.c
                     ${CMAKE_SOURCE_DIR}/Core/Src/ws2812bDriver.c   
                     ${CMAKE_SOURCE_DIR}/Core/Src/spiBuffer.c)

set(PRODUCTION_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/Core/Inc)

set(PRODUCTION_HEADERS_TO_MOCK ${CMAKE_SOURCE_DIR}/Core/Inc/spiInterface.h
                               ${CMAKE_SOURCE_DIR}/Core/Inc/fft.h)

set(MOCKED_SRC ${CMAKE_SOURCE_DIR}/Tests/cmock_files/mock_spiInterface.c
               ${CMAKE_SOURCE_DIR}/Tests/cmock_files/mock_fft.c)
set(MOCKED_INCLUDES ${CMAKE_SOURCE_DIR}/Tests/cmock_files/)

add_custom_target(GenerateMocks_Test_Animations)

foreach(MOCKED_HEADER ${PRODUCTION_HEADERS_TO_MOCK})
    add_custom_command(TARGET GenerateMocks_Test_Animations
                       COMMAND CMOCK_DIR=${CMAKE_SOURCE_DIR}/Tests/cmock ruby ${CMAKE_SOURCE_DIR}/Tests/create_mock.rb ${MOCKED_HEADER} ${MOCKED_INCLUDES}
                       COMMENT "CMake - Generating mock file for ${MOCKED_HEADER}"
                       )
endforeach()

add_executable(Test_Animations main.c ${TEST_FILES} ${PRODUCTION_FILES} ${MOCKED_SRC})

# Mocks must be generated before test building
add_dependencies(Test_Animations GenerateMocks_Test_Animations)

set_source_files_properties(
    ${MOCKED_SRC}
    PROPERTIES GENERATED TRUE
    )

target_compile_definitions(Test_Animations PRIVATE TESTING=1)
target_link_libraries(Test_Animations PRIVATE -lm)

target_link_libraries(Test_Animations PRIVATE unity cmock)
target_include_directories(Test_Animations PRIVATE ${PRODUCTION_INCLUDE_PATH} ${MOCKED_INCLUDES})

add_test(NAME Animations COMMAND Test_Animations)
