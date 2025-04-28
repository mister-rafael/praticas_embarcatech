# Cria o executável para o projeto buzzer
add_executable(buzzer
    main.c
    funcoes/controle_buzzer.c
)

# Configurações do projeto
pico_set_program_name(buzzer "Projeto Buzzer")
pico_set_program_version(buzzer "1.0")

# Inclui diretórios de cabeçalhos
# Faz o compilador "enxergar" os arquivos .h
target_include_directories(buzzer PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../lib/lib_oled
    ${CMAKE_CURRENT_LIST_DIR}/../lib/io_utils
    ${CMAKE_CURRENT_LIST_DIR}/../lib/setup_utils
    ${CMAKE_CURRENT_LIST_DIR}/funcoes # Adiciona o subdiretório funcoes
)

# Vincula bibliotecas padrão do Pico
# Necessário para GPIO e outras funcionalidades
target_link_libraries(buzzer 
    pico_stdlib 
    hardware_pwm 
    hardware_gpio
)

# Gera arquivos binários adicionais
pico_add_extra_outputs(buzzer)