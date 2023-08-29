cmake_minimum_required(VERSION 3.22)

set(TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Test_Ws2812bDriver.c)
set(PRODUCTION_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/ws2812bDriver.c
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/spiBuffer.c)

set(PRODUCTION_HEADERS_TO_MOCK ${CMAKE_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal_spi.h)

set(MOCKED_SRC ${CMAKE_SOURCE_DIR}/Tests/cmock_files/mock_stm32f4xx_hal_spi.c)
set(MOCKED_INCLUDES ${CMAKE_SOURCE_DIR}/Tests/cmock_files/)

set(PRODUCTION_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc
             ${CMAKE_CURRENT_SOURCE_DIR}/../cmock/src/
)

set(MOCKED_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Tests/cmock_files/)

add_custom_target(generate_mocks)

foreach(MOCKED_HEADER ${PRODUCTION_HEADERS_TO_MOCK})
    add_custom_command(TARGET generate_mocks
                       COMMAND CMOCK_DIR=${CMAKE_CURRENT_SOURCE_DIR}/../cmock ruby ${CMAKE_CURRENT_SOURCE_DIR}/../create_mock.rb ${MOCKED_HEADER} ${MOCKED_OUTPUT_PATH}
                       COMMAND python3 ${CMAKE_SOURCE_DIR}/Scripts/createDepForMocks.py ${CMAKE_SOURCE_DIR}/Tests/cmock_files/mock_stm32f4xx_hal_spi.h
                       COMMENT "CMake - Generating mock file for ${MOCKED_HEADER}"
                       )
endforeach()

add_executable(Test_Ws2812bDriver main.c ${TEST_FILES} ${PRODUCTION_FILES} ${MOCKED_SRC})

# Mocks must be generated before test building
add_dependencies(Test_Ws2812bDriver generate_mocks)

set_source_files_properties(
    ${MOCKED_SRC}
    PROPERTIES GENERATED TRUE
    )

# To add condition macro for testing purposes
# set_source_files_properties(
#     ${PRODUCTION_FILES}
#     PROPERTIES
#     COMPILE_FLAGS "-D TESTING"
# )

target_compile_definitions(Test_Ws2812bDriver PRIVATE TESTING=1)
target_link_libraries(Test_Ws2812bDriver PRIVATE -lm)

target_link_libraries(Test_Ws2812bDriver PRIVATE unity cmock)
target_include_directories(Test_Ws2812bDriver PRIVATE ${PRODUCTION_INCLUDE_PATH} ${MOCKED_INCLUDES})

add_test(NAME Ws2812bDriver COMMAND Test_Ws2812bDriver)
