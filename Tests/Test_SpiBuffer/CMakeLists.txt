cmake_minimum_required(VERSION 3.22)

set(TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Test_SpiBuffer.c)
set(PRODUCTION_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/spiBuffer.c)

# set(UNITY_INCLUDES
#         ${CMAKE_CURRENT_SOURCE_DIR}/../unity/core/
#         ${CMAKE_CURRENT_SOURCE_DIR}/../unity/fixture/src/
#         )
set(PRODUCTION_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc/)

add_executable(Test_SpiBuffer main.c ${TEST_FILES} ${PRODUCTION_FILES})

#to add condition macro for testing purposes
set_source_files_properties(
    ${PRODUCTION_FILES}
    PROPERTIES
    COMPILE_FLAGS "-D TESTING"
)

target_link_libraries(Test_SpiBuffer PRIVATE unity)
target_include_directories(Test_SpiBuffer PRIVATE ${PRODUCTION_INCLUDE_PATH})
