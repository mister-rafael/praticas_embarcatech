# Cria o executável controle_acesso
add_executable(controle_acesso 
    main.c # Arquivo principal do programa
    funcoes/controle_acesso.c # Adiciona o arquivo de funções de controle de acesso
)

# Nome e versão do programa
pico_set_program_name(controle_acesso "controle_acesso")
pico_set_program_version(controle_acesso "1.0")

# Configura USB/UART
pico_enable_stdio_usb(controle_acesso 0)
pico_enable_stdio_uart(controle_acesso 0)

# Adiciona caminhos para headers .h	
# Faz o compilador "enxergar" arquivos .h
target_include_directories(controle_acesso PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../lib/lib_oled
    ${CMAKE_CURRENT_LIST_DIR}/../lib/io_utils
    ${CMAKE_CURRENT_LIST_DIR}/../lib/setup_utils
    ${CMAKE_CURRENT_LIST_DIR}/funcoes # Adiciona o subdiretório funcoes
)
# Liga o executável com as bibliotecas necessárias
target_link_libraries(controle_acesso
    pico_stdlib
    setup_utils
    oled
    io_utils
    
)

# Gera arquivos .uf2, .bin, etc.
pico_add_extra_outputs(controle_acesso)