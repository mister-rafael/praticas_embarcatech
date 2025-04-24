cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(bitdoglab_programador_ppa C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(main
    src/main.c
)

target_include_directories(main PRIVATE include)
target_link_libraries(main pico_stdlib hardware_uart hardware_gpio hardware_i2c)

pico_add_extra_outputs(main)