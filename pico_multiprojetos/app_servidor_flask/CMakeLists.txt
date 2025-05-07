# Adiciona a biblioteca compartilhada
add_subdirectory(lib_server)
# ===================================================================================

# Cria o primeiro executável, que é o cliente Flask
# ===================================================================================
add_executable(picow_cliente_http
        picow_cliente_http.c
        )
# ======================================================================================================        
# Seta as configurações da rede Wi-Fi
set(WIFI_SSID "SR-OLIVER") # Nome da sua rede wifi
set(WIFI_PASSWORD "Jw5|0390") # Senha da rede wifi
# Cria macros para definir as variáveis de ambiente
# Isso é útil para que o código possa acessar essas variáveis em tempo de execução
target_compile_definitions(picow_cliente_http PRIVATE   
        WIFI_SSID=\"${WIFI_SSID}\" # Nome da sua rede wifi
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\" # Senha da rede wifi
        )
# ======================================================================================================
# O comando "target_compile_definitions" define macros de pré-processador para o alvo picow_cliente_http. 
# Essas macros (WIFI_SSID e WIFI_PASSWORD) são usadas no código C/C++ como constantes para armazenar 
#as credenciais Wi-Fi.
# ======================================================================================================

# Seta o nome do executável e a versão
pico_set_program_name(picow_cliente_http "picow_cliente_http")
pico_set_program_version(picow_cliente_http "0.1")

# # Configura USB/UART
pico_enable_stdio_uart(picow_cliente_http 0)
pico_enable_stdio_usb(picow_cliente_http 1)

# Adiciona caminhos para headers .h
target_link_libraries(picow_cliente_http
    pico_stdlib
    hardware_adc
    pico_cyw43_arch_lwip_threadsafe_background
    example_lwip_http_util # Vincula a biblioteca compartilhada
)

# Inclui os diretórios de cabeçalhos 
target_include_directories(picow_cliente_http PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/lib_server # Adiciona o subdiretório lib_server
)
# ======================================================================================================
pico_add_extra_outputs(picow_cliente_http)
# ===================================================================================

# Cria o segundo executável
# ===================================================================================
add_executable(picow_http_client_verify 
        picow_verify_http.c)

target_compile_definitions(picow_http_client_verify PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        # By default verification is optional (MBEDTLS_SSL_VERIFY_OPTIONAL)
        # Make it required for this test
        ALTCP_MBEDTLS_AUTHMODE=MBEDTLS_SSL_VERIFY_REQUIRED
        )
target_include_directories(picow_http_client_verify PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts and mbedtls_config
        )

target_link_libraries(picow_http_client_verify
        pico_stdlib
        pico_cyw43_arch_lwip_threadsafe_background
        example_lwip_http_util
)
pico_add_extra_outputs(picow_http_client_verify)

# ===================================================================================
set_source_files_properties(
        ${PICO_LWIP_PATH}/src/apps/altcp_tls/altcp_tls_mbedtls.c
        PROPERTIES
        COMPILE_OPTIONS "-Wno-unused-result"
        )
# ===================================================================================