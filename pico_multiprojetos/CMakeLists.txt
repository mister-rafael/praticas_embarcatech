# ====================================================================================
# Arquivo de configuração do projeto CMake para Raspberry Pi Pico

# Versão mínima do CMake que será usada no projeto
cmake_minimum_required(VERSION 3.13)

# Define o padrão da linguagem C que será usada no projeto (C11)
set(CMAKE_C_STANDARD 11)
# Define o padrão da linguagem C++ (caso seja usada, aqui é C++17)
set(CMAKE_CXX_STANDARD 17)

# Gera um arquivo compile_commands.json com os comandos de compilação 
# — útil para ferramentas de análise.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# =====================================================
# Bloco usado pela extensão do VS Code para Raspberry Pi Pico
# Ele detecta o sistema operacional e define variáveis importantes
# como onde está o SDK, toolchain, etc.
# Você NÃO precisa editar isso para projetos normais.
# =====================================================
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
# Versões usadas pelas ferramentas no VS Code
set(sdkVersion 1.5.1)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.0.0)

# Caminho para o script de integração do VS Code com o Pico SDK
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
# Se o arquivo de integração existir, ele será incluído
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# Informa que estamos usando a placa Raspberry Pi Pico W
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Importa o SDK do Raspberry Pi Pico
# Esse arquivo é responsável por puxar todas as bibliotecas básicas
include(pico_sdk_import.cmake)

# Define o nome do projeto
# Aqui estamos dizendo que o projeto se chama "controle_acesso_oled" e usa C, C++ e ASM
project(pico_multiprojetos C CXX ASM)

# Inicializa o SDK do Raspberry Pi Pico
# Isso configura tudo o que o SDK precisa para funcionar
pico_sdk_init()

# Adiciona a(s) biblioteca(s) compartilhadas
# Basicamente, ler o Cmakelist dentro da pasta
add_subdirectory(lib/io_utils)
add_subdirectory(lib/lib_oled)

# Adiciona os programas (executáveis)
# Basicamente, ler o Cmakelist dentro da pasta
add_subdirectory(pisca_led)
add_subdirectory(mensagem_serial)
add_subdirectory(pratica_rtos)
add_subdirectory(controle_acesso)