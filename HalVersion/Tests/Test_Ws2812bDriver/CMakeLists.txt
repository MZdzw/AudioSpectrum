cmake_minimum_required(VERSION 3.22)

set(TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Test_Ws2812bDriver.c)
set(PRODUCTION_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/ws2812bDriver.c
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/spiBuffer.c)

set(PRODUCTION_HEADERS_TO_MOCK ${CMAKE_SOURCE_DIR}/Core/Inc/spiInterface.h)

set(MOCKED_SRC ${CMAKE_SOURCE_DIR}/Tests/cmock_files/mock_spiInterface.c)
set(MOCKED_INCLUDES ${CMAKE_SOURCE_DIR}/Tests/cmock_files/)

set(PRODUCTION_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/Core/Inc
                            ${CMAKE_SOURCE_DIR}/Tests/cmock/src/
)

add_custom_target(GenerateMocks_Test_Ws2812bDriver)

foreach(MOCKED_HEADER ${PRODUCTION_HEADERS_TO_MOCK})
    add_custom_command(TARGET GenerateMocks_Test_Ws2812bDriver
                       COMMAND CMOCK_DIR=${CMAKE_SOURCE_DIR}/Tests/cmock ruby ${CMAKE_SOURCE_DIR}/Tests/create_mock.rb ${MOCKED_HEADER} ${MOCKED_INCLUDES}
                       COMMENT "CMake - Generating mock file for ${MOCKED_HEADER}"
                       )
endforeach()

add_executable(Test_Ws2812bDriver main.c ${TEST_FILES} ${PRODUCTION_FILES} ${MOCKED_SRC})

# Mocks must be generated before test building
add_dependencies(Test_Ws2812bDriver GenerateMocks_Test_Ws2812bDriver)

set_source_files_properties(
    ${MOCKED_SRC}
    PROPERTIES GENERATED TRUE
    )

target_compile_definitions(Test_Ws2812bDriver PRIVATE TESTING=1)
target_compile_options(Test_Ws2812bDriver PRIVATE -Wall -Wextra -Werror)
target_link_libraries(Test_Ws2812bDriver PRIVATE -lm)

target_link_libraries(Test_Ws2812bDriver PRIVATE unity cmock)
target_include_directories(Test_Ws2812bDriver PRIVATE ${PRODUCTION_INCLUDE_PATH} ${MOCKED_INCLUDES})

add_test(NAME Ws2812bDriver COMMAND Test_Ws2812bDriver)
