cmake_minimum_required(VERSION 3.22)

set(TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Test_Activity.c)
set(PRODUCTION_FILES ${CMAKE_SOURCE_DIR}/Core/Src/activity.c)

set(PRODUCTION_HEADERS_TO_MOCK ${CMAKE_SOURCE_DIR}/Core/Inc/communication.h
                               ${CMAKE_SOURCE_DIR}/Core/Inc/ws2812bDriver.h
                               )

set(MOCKED_SRC ${CMAKE_SOURCE_DIR}/Tests/cmock_files/mock_communication.c
               ${CMAKE_SOURCE_DIR}/Tests/cmock_files/mock_ws2812bDriver.c)
set(MOCKED_INCLUDES ${CMAKE_SOURCE_DIR}/Tests/cmock_files/)

set(PRODUCTION_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/Core/Inc/
             ${CMAKE_SOURCE_DIR}/Tests/cmock/src/
)

set(MOCKED_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Tests/cmock_files/)

add_custom_target(generate_mocks3)

foreach(MOCKED_HEADER ${PRODUCTION_HEADERS_TO_MOCK})
    add_custom_command(TARGET generate_mocks3
                       COMMAND CMOCK_DIR=${CMAKE_CURRENT_SOURCE_DIR}/../cmock ruby ${CMAKE_CURRENT_SOURCE_DIR}/../create_mock.rb ${MOCKED_HEADER} ${MOCKED_OUTPUT_PATH}
                       COMMENT "CMake - Generating mock file for ${MOCKED_HEADER}"
                       )
endforeach()

add_executable(Test_Activity main.c ${TEST_FILES} ${PRODUCTION_FILES} ${MOCKED_SRC})

# Mocks must be generated before test building
add_dependencies(Test_Activity generate_mocks3)

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

target_compile_definitions(Test_Activity PRIVATE TESTING=1)
target_link_libraries(Test_Activity PRIVATE -lm)

target_link_libraries(Test_Activity PRIVATE unity cmock)
target_include_directories(Test_Activity PRIVATE ${PRODUCTION_INCLUDE_PATH} ${MOCKED_INCLUDES})

add_test(NAME Activity COMMAND Test_Activity)
