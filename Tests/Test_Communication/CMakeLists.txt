cmake_minimum_required(VERSION 3.22)

set(TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Test_Communication.c)
set(PRODUCTION_FILES ${CMAKE_SOURCE_DIR}/Core/Src/communication.c)

set(PRODUCTION_INCLUDE_PATH ${CMAKE_SOURCE_DIR}/Core/Inc
                            ${CMAKE_SOURCE_DIR}/Tests/cmock/src
                            )

set(PRODUCTION_HEADERS_TO_MOCK ${CMAKE_SOURCE_DIR}/Core/Inc/usbInterface.h)

set(MOCKED_SRC ${CMAKE_SOURCE_DIR}/Tests/cmock_files/mock_usbInterface.c)
set(MOCKED_INCLUDES ${CMAKE_SOURCE_DIR}/Tests/cmock_files/)

add_custom_target(GenerateMocks_Test_Communication)

foreach(MOCKED_HEADER ${PRODUCTION_HEADERS_TO_MOCK})
    add_custom_command(PRE_BUILD
                       TARGET GenerateMocks_Test_Communication
                       COMMAND ruby ${CMAKE_SOURCE_DIR}/Tests/create_mock.rb ${MOCKED_HEADER} ${MOCKED_INCLUDES}
                       COMMENT "CMake - Generating mock file for ${MOCKED_HEADER}"
                       )
endforeach()


add_executable(Test_Communication main.c ${TEST_FILES} ${PRODUCTION_FILES} ${MOCKED_SRC})

# Mocks must be generated before test building
add_dependencies(Test_Communication GenerateMocks_Test_Communication)

set_source_files_properties(
    ${MOCKED_SRC}
    PROPERTIES GENERATED TRUE
    )

target_compile_definitions(Test_Communication PRIVATE TESTING=1)
target_compile_options(Test_Communication PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(Test_Communication PRIVATE -lm)

target_link_libraries(Test_Communication PRIVATE unity cmock)
target_include_directories(Test_Communication PRIVATE ${PRODUCTION_INCLUDE_PATH} ${MOCKED_INCLUDES})
