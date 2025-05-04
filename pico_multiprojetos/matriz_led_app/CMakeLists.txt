# Cria o executável matriz_led
add_executable(matriz_led_app 
    main.c # Arquivo principal do programa
)

# Nome e versão do programa
pico_set_program_name(matriz_led_app "matriz_led_app")
pico_set_program_version(matriz_led_app "1.0")

# Configura USB/UART
pico_enable_stdio_usb(matriz_led_app 0)
pico_enable_stdio_uart(matriz_led_app 0)

# Adiciona caminhos para headers .h	
# Faz o compilador "enxergar" arquivos .h
target_include_directories(matriz_led_app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../lib/lib_oled
    ${CMAKE_CURRENT_LIST_DIR}/../lib/io_utils
    ${CMAKE_CURRENT_LIST_DIR}/../lib/setup_utils
    ${CMAKE_CURRENT_LIST_DIR}/../lib/matriz_utils
)
# Liga o executável com as bibliotecas necessárias
target_link_libraries(matriz_led_app
    pico_stdlib
    hardware_pio # Necessário para PIO
    hardware_clocks # Necessário para clocks
    hardware_pwm # Necessário para PWM
    hardware_gpio # Necessário para GPIO
    setup_utils # Biblioteca de setup_utils
    oled # Biblioteca de OLED
    io_utils    # Biblioteca de io_utils
    matriz_utils # Biblioteca de matriz_utils
)

# Gera arquivos .uf2, .bin, etc.
pico_add_extra_outputs(matriz_led_app)