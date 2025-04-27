# Cria a biblioteca chamada io_utils do tipo STATIC
# (ou seja, não é compartilhada entre os executáveis) 
# e que será compilada a partir dos arquivos .c listados
add_library(io_utils STATIC 
    actions_io.c
)

# Linka com bibliotecas externas que a io_utils precisa para funcionar
target_link_libraries(io_utils
    pico_stdlib
)
# Define onde estão os headers (arquivos .h) desta biblioteca
target_include_directories(io_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})