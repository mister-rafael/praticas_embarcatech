# Cria o executável pisca_led
add_executable(pisca_led 
    main.c
)

# Nome e versão do programa
pico_set_program_name(pisca_led "pisca_led")
pico_set_program_version(pisca_led "1.0")

# Configura USB/UART
pico_enable_stdio_usb(pisca_led 0)
pico_enable_stdio_uart(pisca_led 0)

# Adiciona caminhos para headers .h	Faz o compilador "enxergar" arquivos
target_include_directories(pisca_led PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../lib/io_utils
    ${CMAKE_CURRENT_LIST_DIR}/../lib/setup_utils
)
# Liga o executável com as bibliotecas necessárias
target_link_libraries(pisca_led
    pico_stdlib
    io_utils
    setup_utils
)

# Gera arquivos .uf2, .bin, etc.
pico_add_extra_outputs(pisca_led)