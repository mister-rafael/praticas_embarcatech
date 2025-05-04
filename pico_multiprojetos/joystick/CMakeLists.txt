# Cria o executável para o projeto joystick
add_executable(joystick
    main.c
)

# Configurações do projeto
pico_set_program_name(joystick "Joystick LED Control")
pico_set_program_version(joystick "1.0")

# Configura USB/UART
pico_enable_stdio_usb(joystick 1)

# Adiciona caminhos para headers .h	
# Faz o compilador "enxergar" arquivos .h
target_include_directories(joystick PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../lib/setup_utils
    ${CMAKE_CURRENT_LIST_DIR}/../lib/matriz_utils
)

# Inclui bibliotecas padrão do Pico
target_link_libraries(joystick 
    pico_stdlib 
    hardware_adc
    hardware_pio # Necessário para PIO
    hardware_clocks # Necessário para clocks
    hardware_pwm # Necessário para PWM
    hardware_gpio # Necessário para GPIO
    setup_utils # Biblioteca de setup_utils
    matriz_utils # Biblioteca de matriz_utils
)



# Gera arquivos binários adicionais
pico_add_extra_outputs(joystick)