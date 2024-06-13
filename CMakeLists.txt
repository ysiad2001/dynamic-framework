cmake_minimum_required(VERSION 3.12)

# build functions from pico sdk
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(framework C CXX ASM)
set(CMAKE_C_STANDARD 11)
pico_sdk_init()

add_executable(${PROJECT_NAME})

# Disable SDK alarm support for this framework
target_compile_definitions(${PROJECT_NAME} PRIVATE
        PICO_DISABLE_SHARED_IRQ_HANDLERS=1
        PICO_TIME_DEFAULT_ALARM_POOL_DISABLED=1)


# pull in common dependencies = library
target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_structs hardware_regs pico_multicore)

# LINK with a custom linker script
set_target_properties(${PROJECT_NAME} PROPERTIES PICO_TARGET_LINKER_SCRIPT 
    ${CMAKE_SOURCE_DIR}/linker.ld)

if (NOT PICO_NO_HARDWARE)
    add_subdirectory(cont_free_framework)
    add_subdirectory(compare_framework)
    add_subdirectory(dynamic_framework)
    add_subdirectory(measure_dynamic_framework)
    add_subdirectory(measure_cont_free_framework)
    add_subdirectory(measure_compare_framework)
endif ()


