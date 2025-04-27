# Cria a biblioteca com o .c correspondente
add_library(setup_utils STATIC 
    setup_utils.c     
)

# Linka com bibliotecas externas que a io_utils precisa para funcionar
target_link_libraries(setup_utils
    pico_stdlib
    hardware_i2c
)
# Define onde estão os headers (arquivos .h) desta biblioteca
target_include_directories(setup_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})