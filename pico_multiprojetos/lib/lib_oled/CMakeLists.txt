# Cria a biblioteca com o .c correspondente
add_library(oled STATIC ssd1306_i2c.c)

# Linka com bibliotecas externas que a io_utils precisa para funcionar
target_link_libraries(oled
    pico_stdlib
    hardware_i2c
)
# Define onde estão os headers (arquivos .h) desta biblioteca
target_include_directories(oled PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})