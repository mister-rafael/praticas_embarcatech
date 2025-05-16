# Cria a biblioteca compartilhada, chamada de "exemplo_lwip_http_util" como "STATIC"
# Essa biblioteca é usada para compartilhar código entre diferentes partes do projeto
# ===================================================================================
#add_library(exemplo_lwip_http_util STATIC
#    exemplo_http_client_util.c
#)
# Ou, cria a biblioteca como INTERFACE:
pico_add_library(exemplo_lwip_http_util NOFLAG)
# ===================================================================================
# OBS.: "pico_add_library" é uma função ideal para criar bibliotecas:
# leves e que consistem principalmente em cabeçalhos ou funções auxiliares
# ====================================================================================

# Linka com bibliotecas externas que a io_utils precisa para funcionar
#target_link_libraries(exemplo_lwip_http_util
#    pico_lwip_http
#    pico_lwip_mbedtls
#    pico_mbedtls
#    hardware_adc
#    hardware_pio # Necessário para PIO
#)
# Define onde estão os headers (arquivos .h) desta biblioteca
#target_include_directories(exemplo_lwip_http_util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# ===================================================================================
target_sources(exemplo_lwip_http_util INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/exemplo_http_client_util.c
        )
# ===================================================================================

# Inclui os diretórios de cabeçalhos:
# Isso é importante para que o compilador saiba onde encontrar os arquivos de cabeçalho
# que estão na mesma pasta que o CMakeLists.txt
#target_include_directories(exemplo_lwip_http_util INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Vincula bibliotecas necessárias à biblioteca compartilhada
# Aqui estamos dizendo que a biblioteca "exemplo_lwip_http_utils" depende de outras bibliotecas
# como pico_stdlib, pico_cyw43_arch_none e lwip.

pico_mirrored_target_link_libraries(exemplo_lwip_http_util INTERFACE
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