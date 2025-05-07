# Cria a biblioteca compartilhada, chamada de "example_lwip_http_util" como "STATIC"
# Essa biblioteca é usada para compartilhar código entre diferentes partes do projeto
# ===================================================================================
#add_library(example_lwip_http_util STATIC
#    example_http_client_util.h.c
#)
# Ou, cria a biblioteca como INTERFACE:
pico_add_library(example_lwip_http_util NOFLAG)
# ===================================================================================
# OBS.: "pico_add_library" é uma função ideal para criar bibliotecas:
# leves e que consistem principalmente em cabeçalhos ou funções auxiliares
# ====================================================================================

target_sources(example_lwip_http_util INTERFACE
         ${CMAKE_CURRENT_LIST_DIR}/example_http_client_util.c
         )
# ===================================================================================

# Inclui os diretórios de cabeçalhos:
# Isso é importante para que o compilador saiba onde encontrar os arquivos de cabeçalho
# que estão na mesma pasta que o CMakeLists.txt
target_include_directories(example_lwip_http_util INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Vincula bibliotecas necessárias à biblioteca compartilhada
# Aqui estamos dizendo que a biblioteca "example_lwip_http_utils" depende de outras bibliotecas
# como pico_stdlib, pico_cyw43_arch_none e lwip.

pico_mirrored_target_link_libraries(example_lwip_http_util INTERFACE
    pico_lwip_http
    pico_lwip_mbedtls
    pico_mbedtls
    hardware_adc
    hardware_pio # Necessário para PIO
)
# ===================================================================================
# OBS.: "pico_mirrored_target_link_libraries" é uma função que vincula bibliotecas de forma que
# elas sejam incluídas corretamente no executável final. 
# Exemplo: Vincular bibliotecas como pico_lwip_http ou pico_cyw43_arch_none.
# ===================================================================================