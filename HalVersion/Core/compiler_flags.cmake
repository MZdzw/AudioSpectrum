# Compiler flags used for C and ASM files
set(CORE_FLAGS "-mcpu=cortex-m4 -mthumb")

# Debug flag
set(CORE_FLAGS "${CORE_FLAGS} -g3")

# Hardware float support
# set(CORE_FLAGS "${CORE_FLAGS} -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffast-math")
set(CORE_FLAGS "${CORE_FLAGS} -mfloat-abi=hard -mfpu=fpv4-sp-d16")

# Compiler flags specific for C++ files
# -std - C++ standard: c++98, gnu++98, c++11, gnu++11, c++14, gnu++14
# -fno-rtti - disable virtual class information used by dynamic_cast and typeid
# -fno-exceptions - disable exception handling
# fverbose-asm - additional comments for generated assembler code
# -MMD - create dependency files
set(CXX_FLAGS "-std=gnu++17 -O0 -g -fno-rtti -fno-exceptions -fverbose-asm -MMD")

# Compiler flags specific for C files
# -std - C standard: c89, c99, gnu89,gnu99, iso9899:119409
# -O0 - optimization level: -O0, -O1, -O2, -O3, -Os
# fverbose-asm - additional comments for generated assembler code
# -MMD - create dependency files
# set(C_FLAGS "-std=gnu89 -O0 -ffunction-sections -fdata-sections -fverbose-asm -MMD")
set(C_FLAGS "-O0 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP")

set(ASM_FLAGS "-x assembler-with-cpp")

# Warning flags for C++
# -Wall - standard warnings
# -Wextra - extended warnings
set(CXX_WARNINGS "-Wall -Wextra")

# Warning flags for C
# -Wall - standard warnings
# -Wextra - extended warnings
# -Wstrict-prototypes - additional warnings for function prototypes
set(C_WARNINGS "-Wall -Wextra")

set(LD_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F401CCUX_FLASH.ld)
# Linker flags
# -Wl, -Map - map file to be created
# -T - file with linker script
# -g - debug flag
# -Wl,--gc-sections - unused function removal
set(LD_FLAGS "-T\"${LD_SCRIPT}\" --specs=nosys.specs \
-Wl,-Map=\"${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map\" \
-Wl,--gc-sections -static -L\"${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Lib/GCC\" \
--specs=nano.specs -Wl,--start-group -lc -lm -Wl,--end-group -Wl,--print-memory-usage"
)

set(CMAKE_LINKER "/usr/bin/arm-none-eabi-gcc")

set(CMAKE_CXX_FLAGS "${CORE_FLAGS} ${CXX_FLAGS} ${CXX_WARNINGS}")
set(CMAKE_C_FLAGS "${CORE_FLAGS} ${C_FLAGS} ${C_WARNINGS}")
set(CMAKE_ASM_FLAGS "${ASM_FLAGS} ${CORE_FLAGS} ${CXX_WARNINGS}")
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> -larm_cortexM4lf_math ${CORE_FLAGS} ${LD_FLAGS} <OBJECTS> -o <TARGET>")
