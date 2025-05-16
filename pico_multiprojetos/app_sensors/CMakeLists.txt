# Adiciona a biblioteca compartilhada interna
#add_subdirectory(lib_sensors)
# ===================================================================================

# Cria o primeiro executável, que é o cliente Flask
# ===================================================================================
add_executable(cliente_http
        cliente_http.c
        )
# ======================================================================================================        
# Seta as configurações da rede Wi-Fi
set(WIFI_SSID "SR-OLIVER") # Nome da sua rede wifi
set(WIFI_PASSWORD "Jw5|0390") # Senha da rede wifi
# Cria macros para definir as variáveis de ambiente
# Isso é útil para que o código possa acessar essas variáveis em tempo de execução
target_compile_definitions(cliente_http PRIVATE   
        WIFI_SSID=\"${WIFI_SSID}\" # Nome da sua rede wifi
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\" # Senha da rede wifi
        )
# ======================================================================================================
# O comando "target_compile_definitions" define macros de pré-processador para o alvo cliente_http. 
# Essas macros (WIFI_SSID e WIFI_PASSWORD) são usadas no código C/C++ como constantes para armazenar 
#as credenciais Wi-Fi.
# ======================================================================================================

# Seta o nome do executável e a versão
pico_set_program_name(cliente_http "cliente_http")
pico_set_program_version(cliente_http "1.0")

# # Configura USB/UART
pico_enable_stdio_uart(cliente_http 0)
pico_enable_stdio_usb(cliente_http 1)

# Adiciona caminhos para headers .h
target_link_libraries(cliente_http
    pico_stdlib
    hardware_adc
    pico_cyw43_arch_lwip_threadsafe_background
    exemplo_lwip_http_util # Vincula a biblioteca compartilhada
)

# Inclui os diretórios de cabeçalhos 
target_include_directories(cliente_http PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/lib_sensors # Adiciona o subdiretório lib_sensors
    ${CMAKE_CURRENT_LIST_DIR}/../lib/base_server # Adiciona o subdiretório base_server
)
# ======================================================================================================
pico_add_extra_outputs(cliente_http)
# ===================================================================================

# Cria o segundo executável
# ===================================================================================
add_executable(verify_http 
        verify_http.c)

target_compile_definitions(verify_http PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        # By default verification is optional (MBEDTLS_SSL_VERIFY_OPTIONAL)
        # Make it required for this test
        ALTCP_MBEDTLS_AUTHMODE=MBEDTLS_SSL_VERIFY_REQUIRED
        )
target_include_directories(verify_http PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/lib_sensors # for our common lwipopts and mbedtls_config
        ${CMAKE_CURRENT_LIST_DIR}/../lib/base_server # for our common lwipopts and mbedtls_config
        )

target_link_libraries(verify_http
        pico_stdlib
        pico_cyw43_arch_lwip_threadsafe_background
        exemplo_lwip_http_util
)
pico_add_extra_outputs(verify_http)

# ===================================================================================
set_source_files_properties(
        ${PICO_LWIP_PATH}/src/apps/altcp_tls/altcp_tls_mbedtls.c
        PROPERTIES
        COMPILE_OPTIONS "-Wno-unused-result"
        )
# ===================================================================================