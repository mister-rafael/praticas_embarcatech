# Cria a biblioteca chamada matriz_utils do tipo STATIC
# (ou seja, não é compartilhada entre os executáveis) 
# e que será compilada a partir dos arquivos .c listados
add_library(matriz_utils STATIC 
    matriz_utils.c
)

# Generate PIO header  
pico_generate_pio_header(matriz_utils ${CMAKE_CURRENT_LIST_DIR}/ws2818b.pio)

# Linka com bibliotecas externas que a matriz_utils precisa para funcionar
target_link_libraries(matriz_utils
    pico_stdlib
    hardware_pio # Necessário para PIO
    hardware_clocks # Necessário para clocks
)
# Define onde estão os headers (arquivos .h) desta biblioteca
target_include_directories(matriz_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})