# Cria o executável para o projeto joystick
add_executable(joystick
    main.c
)

# Configurações do projeto
pico_set_program_name(joystick "Joystick LED Control")
pico_set_program_version(joystick "1.0")

# Inclui bibliotecas padrão do Pico
target_link_libraries(joystick pico_stdlib hardware_adc)

# Gera arquivos binários adicionais
pico_add_extra_outputs(joystick)