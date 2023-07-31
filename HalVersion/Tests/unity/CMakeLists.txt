cmake_minimum_required(VERSION 3.22)

set(UNITY_FILES core/unity.c
                fixture/unity_fixture.c
                memory/unity_memory.c
                )

set(UNITY_INCLUDES
        core/
        fixture/
        memory/
        )
            
add_library(unity ${UNITY_FILES})

target_include_directories(unity PUBLIC ${UNITY_INCLUDES})

target_compile_options(unity PRIVATE "-Wall")