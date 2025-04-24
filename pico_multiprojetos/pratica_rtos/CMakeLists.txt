# Cria o executável pratica_rtos
add_executable(pratica_rtos 
    main.c
)

include($ENV{FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake) #incluindo em 26/03/2025

# Nome e versão do programa
pico_set_program_name(pratica_rtos "pratica_rtos")
pico_set_program_version(pratica_rtos "1.0")

# Configura USB/UART
pico_enable_stdio_usb(pratica_rtos 0)
pico_enable_stdio_uart(pratica_rtos 0)

# Adiciona caminhos para headers .h	Faz o compilador "enxergar" arquivos
target_include_directories(pratica_rtos PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../lib/rtos
)
# Liga o executável com as bibliotecas necessárias
target_link_libraries(pratica_rtos
    pico_stdlib
    hardware_gpio
    pico_multicore
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
)

# Gera arquivos .uf2, .bin, etc.
pico_add_extra_outputs(pratica_rtos)