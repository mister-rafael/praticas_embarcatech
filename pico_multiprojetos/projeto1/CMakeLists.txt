# Cria o executável para o projeto1
add_executable(projeto1
    main.c # Arquivo principal do programa
    funcoes/controle_led.c # Adiciona o arquivo de funções de controle de LED
)

# Configurações do projeto
pico_set_program_name(projeto1 "Projeto 1")
pico_set_program_version(projeto1 "1.0")

# Inclui diretórios de cabeçalhos
# Faz o compilador "enxergar" os arquivos .h
include_directories(funcoes)

# Vincula bibliotecas padrão do Pico
# Necessário para GPIO e outras funcionalidades
target_link_libraries(projeto1 pico_stdlib)

# Gera arquivos binários adicionais
pico_add_extra_outputs(projeto1)