# Cria o executável mensagem_serial
add_executable(mensagem_serial main.c)

# Nome e versão do programa
pico_set_program_name(mensagem_serial "mensagem_serial")
pico_set_program_version(mensagem_serial "1.0")

# Ativa saída via USB, desativa UART
pico_enable_stdio_usb(mensagem_serial 1)
pico_enable_stdio_uart(mensagem_serial 0)

# Adiciona caminhos para headers .h	Faz o compilador "enxergar" arquivos
target_include_directories(mensagem_serial PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../lib/io_utils
)
# Liga o executável com as bibliotecas necessárias
target_link_libraries(mensagem_serial
    pico_stdlib
    io_utils
)
# Gera arquivos .uf2, .bin, etc.
pico_add_extra_outputs(mensagem_serial)