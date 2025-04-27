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
project(controle_acesso_oled C CXX ASM)

# Inicializa o SDK do Raspberry Pi Pico
# Isso configura tudo o que o SDK precisa para funcionar
pico_sdk_init()

# Define o nome do executável final que será criado e o arquivo principal do código-fonte
# Aqui o executável será "controle_acesso_oled " e o arquivo é "controle_acesso_oled .c"
add_executable(controle_acesso_oled 
    controle_acesso_oled.c 
    inc/ssd1306_i2c.c)

# Define o nome que o programa terá (aparece no USB, por exemplo)
pico_set_program_name(controle_acesso_oled "controle_acesso_oled")
# Define a versão do programa
pico_set_program_version(controle_acesso_oled "0.1")

# Aqui você pode habilitar/desabilitar o uso de USB ou UART para comunicação
# 0 = desabilita, 1 = habilita
# Nesse exemplo, estamos desabilitando UART e habilitando USB
pico_enable_stdio_uart(controle_acesso_oled 0)
pico_enable_stdio_usb(controle_acesso_oled 1)

# Liga (linka) o programa com a biblioteca padrão do Pico (pico_stdlib)
target_link_libraries(controle_acesso_oled
        pico_stdlib)

# Adiciona diretórios de inclusão (headers .h) ao projeto, ou seja,
# Diz ao CMake onde estão os arquivos .h para o compilador achar.
# ${CMAKE_CURRENT_LIST_DIR} significa "a pasta onde está o CMakeLists.txt"
target_include_directories(controle_acesso_oled PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

# Aqui você pode adicionar outras bibliotecas, se você quiser
# ligar o seu programa à bibliotecas externas, 
# não a código-fonte .c que você mesmo escreveu.
target_link_libraries(controle_acesso_oled 
        hardware_i2c
        )

# Cria arquivos extras de saída: .uf2, .bin, .hex
# Isso é útil para gravar na placa depois da compilação
pico_add_extra_outputs(controle_acesso_oled)

# Adiciona o novo arquivo .c com a função
# Essa "target_sources" Diz ao CMake que o novo .c deve ser compilado junto
# Vázia por enquanto
target_sources(meu_led PRIVATE
    
)

