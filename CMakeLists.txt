cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME universe_half_bridge)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}
        Core/Inc/MY_OLED.h
        Core/Inc/OLED.h
        Core/Src/MY_OLED.c
        Core/Src/OLED.c
        Core/Src/OLED_Data.c)

# ---------- CMSIS-DSP 集成开始 ----------

# 1. 添加 DSP 头文件包含路径
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        Drivers/CMSIS/DSP/Include
        Drivers/CMSIS/Core/Include
)

# 2. 定义 CMSIS-DSP 所需的核心宏（Cortex-M4）
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
        ARM_MATH_CM4
)

# 3. 导入预编译的 DSP 静态库（Cortex-M4 硬浮点版本）
add_library(cmsis_dsp STATIC IMPORTED)
set_target_properties(cmsis_dsp PROPERTIES
        IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Lib/GCC/libarm_cortexM4lf_math.a
)

# 4. 链接 DSP 库（使用普通签名，与子目录 cubeMX 保持一致）
target_link_libraries(${CMAKE_PROJECT_NAME} cmsis_dsp)

# ---------- CMSIS-DSP 集成结束 ----------

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
        # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries（保持普通签名，与子目录统一）
target_link_libraries(${CMAKE_PROJECT_NAME}
        stm32cubemx
        # Add user defined libraries
)