# Cria a biblioteca com o .c correspondente
add_library(io_utils STATIC io_utils.c)

# Linka com bibliotecas externas que a io_utils precisa para funcionar
target_link_libraries(io_utils
    pico_stdlib
)
# Define onde estão os headers (arquivos .h) desta biblioteca
target_include_directories(io_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})