cmake_minimum_required(VERSION 3.22)

#project(Template)

enable_language(C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(PROJ_PATH                       ${CMAKE_CURRENT_SOURCE_DIR}/..)

message("Build type: "              ${CMAKE_BUILD_TYPE})

set(CPU_PARAMETERS
    -mthumb
    -mcpu=cortex-m4
)

set(LINKER_SCRIPT ${PROJ_PATH}/STM32F401CCUX_FLASH.ld)

set(EXECUTABLE ${CMAKE_PROJECT_NAME})

set(HAL_DRIVERS 
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_adc.c
)

set(INCLUDES
        ${PROJ_PATH}/Drivers/CMSIS/Device/ST/STM32F4xx/Include/
        ${PROJ_PATH}/Drivers/CMSIS/Include/

        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Inc/
        ${PROJ_PATH}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy/

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

        ${CMAKE_CURRENT_SOURCE_DIR}/Src/spiBuffer.c

        ${HAL_DRIVERS}

        ${CMAKE_CURRENT_SOURCE_DIR}/Startup/startup_stm32f401ccux.s
        )

set(SYMBOLS
        -DSTM32
        -DSTM32F4
        -DSTM32F401xC
        -DDEBUG
        -DARM_MATH_CM4
        )

include(compiler_flags.cmake)

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
    # # ${CPU_PARAMETERS}
)

target_link_options(${EXECUTABLE} PRIVATE
    # -T${LINKER_SCRIPT}
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
