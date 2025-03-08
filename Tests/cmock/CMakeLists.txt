cmake_minimum_required(VERSION 3.22)

set(CMOCK_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/src
                   ${CMAKE_CURRENT_SOURCE_DIR}/../unity/core
                   )

add_library(cmock src/cmock.c)

target_include_directories(cmock PUBLIC ${CMOCK_INCLUDES})