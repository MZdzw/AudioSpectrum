cmake_minimum_required(VERSION 3.22)

enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CXX_WARNINGS "-Wall -Wextra")
set(C_WARNINGS "-Wall -Wextra")
set(SYMBOLS
    -DSTM32
    -DSTM32F4
    -DSTM32F401xC
    -DDEBUG
    -DARM_MATH_CM4
    -DUSE_HAL_DRIVER
)
set(CORE_FLAGS -mcpu=cortex-m4 -mthumb)
set(CORE_FLAGS ${CORE_FLAGS} -mfloat-abi=hard -mfpu=fpv4-sp-d16)
set(CMAKE_CXX_FLAGS "-fno-rtti -fno-exceptions -fverbose-asm -MMD ${CXX_WARNINGS}")
set(CMAKE_C_FLAGS "-ffunction-sections -fdata-sections -fstack-usage -MMD -MP ${C_WARNINGS}")
set(CMAKE_ASM_FLAGS "-x assembler-with-cpp")
set(LINKER_FLAGS --specs=nosys.specs 
    -Wl,-Map=${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map 
    -Wl,--gc-sections -static  
    -Wl,--start-group -lc -lm -Wl,--end-group -Wl,--print-memory-usage
)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F401CCUX_FLASH.ld)
set(EXECUTABLE ${CMAKE_PROJECT_NAME})

set(INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc/ 
)

set(STM32_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/sysmem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/system_stm32f4xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/stm32f4xx_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/stm32f4xx_hal_msp.c

    ${CMAKE_CURRENT_SOURCE_DIR}/Src/gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/adc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/dma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/tim.c

    ${CMAKE_CURRENT_SOURCE_DIR}/Src/spiInterface.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/usbInterface.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/spiBuffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/ws2812bDriver.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/animations.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/communication.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/activity.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/fft.c

    ${CMAKE_CURRENT_SOURCE_DIR}/Startup/startup_stm32f401ccux.s
)

set(SYMBOLS
    -DSTM32
    -DSTM32F4
    -DSTM32F401xC
    -DDEBUG
    -DARM_MATH_CM4
    -DUSE_HAL_DRIVER
)

add_subdirectory(CMSIS_HAL)

add_executable(${EXECUTABLE} ${STM32_SRC})

# to disable meaningless warnings for specific files
set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/syscalls.c
    PROPERTIES
    COMPILE_FLAGS "-Wno-unused-parameter"
)

# Include paths
target_include_directories(${EXECUTABLE} PRIVATE ${INCLUDES})

# Project symbols
target_compile_definitions(${EXECUTABLE} PRIVATE ${SYMBOLS})

# Compiler options
target_compile_options(${EXECUTABLE} PRIVATE
    ${CORE_FLAGS}
)

target_link_options(${EXECUTABLE} PRIVATE
    -T${LINKER_SCRIPT}
    ${CORE_FLAGS}
    ${LINKER_FLAGS}
)

target_link_libraries(${EXECUTABLE} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/CMSIS_HAL/Drivers/CMSIS/Lib/GCC/libarm_cortexM4lf_math.a
    CMSIS_HAL
)

# Execute post-build to print size
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${EXECUTABLE}>
)

# Convert output to hex and binary
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.hex
)

# Convert to bin file -> add conditional check?
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.bin
)
