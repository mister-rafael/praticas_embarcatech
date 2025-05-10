# Documentação do projeto app_servidor_flask
### 📂 Estrutura geral
Arquivos principais:

`picow_cliente_http.c` e `picow_verify_http.c`, códigos para o cliente Pico W.

`CMakeLists.txt`: configuração de build do projeto (raiz e em subpastas)

### 📁 lib_server/

Código C auxiliar para requisições HTTP e configurações de rede

### 📁 web/

`server.py`: servidor Flask (é a base da comunicação com o Pico W)

`templates/index.html`: página web entregue pelo servidor Flask



## 📄 Arquivo `picow_client_http.c`

### 🔹 Visão Geral

Este arquivo implementa um **cliente HTTP** rodando no **Raspberry Pi Pico W**, com capacidade de:

* Detectar o **estado de dois botões físicos** (A e B). (Tarefa 01)
* Ler valores de um **joystick analógico (eixos X e Y)**. (Tarefa 02)
* Controlar LEDs locais. (Adicional)
* Enviar comandos para um **servidor Flask** via requisições HTTP GET.

---

### 🔹 Importações de Bibliotecas

```c
#include <stdio.h>
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"
#include "pico/async_context.h"
#include "lwip/altcp_tls.h"
#include "example_http_client_util.h"
#include "hardware/adc.h"
```

* `stdio.h`: entrada e saída padrão (`printf`).
* `pico/stdio.h`, `cyw43_arch.h`: funções específicas para Wi-Fi e GPIO no Pico W.
* `hardware/adc.h`: leitura analógica (ADC) dos eixos do joystick.
* `example_http_client_util.h`: abstrações para fazer requisições HTTP.

---

### 🔹 Definições de Configuração

```c
#define HOST "10.3.10.217"
#define PORT 5000
#define INTERVALO_MS 500
```

* **HOST**: IP do servidor Flask.
* **PORT**: porta do servidor.
* **INTERVALO\_MS**: intervalo entre verificações no loop principal (1000ms).

---

### 🔹 Funções Auxiliares

#### `posicao_x()` e `posicao_y()`

```c
uint16_t posicao_x()
{
    adc_select_input(1);
    return adc_read();
}
```
```c
uint16_t posicao_y()
{
    adc_select_input(0);
    return adc_read();
}
```
* Selecionam o canal ADC e retornam a leitura analógica do eixo X ou Y do joystick.
* Valores variam entre **0 (mínimo)** e **4095 (máximo)**.

---

#### `enviar_dados_joystick(x, y)`

```c
void enviar_dados_joystick(uint16_t x, uint16_t y)
{
    char url[64];
    snprintf(url, sizeof(url), "/joystick?x=%d&y=%d", x, y);
```

* Constrói uma URL GET como `/joystick?x=1234&y=5678`.
* Usa `http_client_request_sync()` para enviar os dados ao servidor Flask.

---

#### `enviar_dados_botao(caminho)`

```c
void enviar_dados_botao(const char *caminho)
```

* Recebe uma string como `/CLICK_A`, `/SOLTO_B`, etc.
* Envia ao servidor Flask indicando o estado do botão pressionado ou solto.
* Essa abordagem torna o fluxo mais claro e evita redundância na verificação de uma variável que já foi tratada na mesma condição.

---

### 🔹 Função `main()`

#### 1. Inicialização dos periféricos

```c
stdio_init_all();
iniciar_botao(BUTTON_A);
iniciar_botao(BUTTON_B);
iniciar_joystick(...);
iniciar_led(...);
```

* Inicializa comunicação serial, botões, joystick e LEDs.

---

#### 2. Conexão Wi-Fi

```c
if (wifi_connect(WIFI_SSID, WIFI_PASSWORD)) { return 1; }
```

* Conecta à rede Wi-Fi.
* A rede e senha são definidos no arquivo `CMakeLists.txt` principal.

---

#### 3. Loop Principal

```c
while (1)
```

* Executa continuamente verificações e ações.
* Verifica mudanças de estado dos botões A e B.
* Lê posições do joystick.
* Liga/desliga LEDs baseados nos comandos.
* Envia dados HTTP para o servidor em cada evento detectado.

---

#### Exemplo de verificação do botão A:

```c
int estado_real_a = (gpio_get(BUTTON_A) == 0) ? 0 : 1;
if (button_a_state != estado_real_a)
{
    path_a = (estado_real_a == 0) ? "/CLICK_A" : "/SOLTO_A";
    gpio_put(LED_RED, (estado_real_a == 0) ? 1 : 0);
    enviar_dados_botao(path_a);
}
```

* Detecta se o botão A foi pressionado ou solto.
* Aciona LED correspondente.
* Envia a informação ao servidor Flask.
* O mesmo vale para o botão B

---

#### Exemplo de resposta ao joystick:

```c
if (x < 1000)           -> esquerda  → LED azul
else if (x > 3000)      -> direita   → todos os LEDs
else if (y < 1000)      -> baixo     → LED verde
else if (y > 3000)      -> cima      → LED vermelho
else if (SW pressionado)-> central   → todos os LEDs
else                    -> desligar LEDs
```

---
### Implementação de cada IF
```c
if (x < 1000) // O Joystick foi movido para a esquerda
        {
            gpio_put(LED_BLUE, 1);
            // Envia os dados para o servidor Flask
            enviar_dados_joystick(x, y);
        }
```

---
### 🔹 Integração com o servidor Flask

Todas as requisições são feitas com base nas seguintes URLs:

* **Botões**: `/CLICK_A`, `/SOLTO_B`, etc.
* **Joystick**: `joystick`

No lado do servidor, o Flask interpretará essas rotas para reagir adequadamente (controlar uma interface, registrar dados, etc.).

---

### 🧠 Conclusão

Este código transforma o **Raspberry Pi Pico W** em um **cliente interativo IoT**, enviando comandos físicos (botões/joystick) para um servidor Flask por meio de HTTP. Ele demonstra:

* Leitura de sensores analógicos (joystick).
* Monitoramento de botões digitais.
* Controle de LEDs como feedback local.
* Envio de dados para um servidor remoto via GET.

---

## 📄 Arquivo `pico_verify_http.c`

### ✅ O que este código faz?
Este programa realiza testes de verificação de certificados TLS (HTTPS) no Raspberry Pi Pico W. Ele valida se o cliente HTTP:
- Consegue realizar uma requisição HTTPS com um certificado confiável (root cert correto).
- Detecta falhas ao tentar a mesma requisição com um certificado inválido (root cert errado).

### 🧩 Como se encaixa no projeto?
Este arquivo serve como um exemplo prático e teste funcional da infraestrutura de HTTPS, utilizando:
- Wi-Fi via cyw43_arch.
- Cliente HTTP baseado em LWIP.
- Conexões TLS via altcp_tls.

É essencial para garantir a segurança das conexões em aplicações que dependem de atualizações ou comandos autenticados.

### 🧾 Estrutura do Código

1. **Inclusões**
    ```c
    #include "example_http_client_util.h"
    #include "lwip/altcp_tls.h"
    ```
    Importa o cliente HTTP e a pilha TLS usada.

2. **Definições de Host e URL**
    ```c
    #define HOST "fw-download-alias1.raspberrypi.com"
    #define URL_REQUEST "/net_install/boot.sig"
    ```
    Define o servidor e o recurso a ser requisitado. O domínio é propriedade da Raspberry Pi e possui um certificado estável.

3. **Certificados TLS**
    - **TLS_ROOT_CERT_OK**: Certificado raiz confiável, extraído de um navegador.
    - **TLS_ROOT_CERT_BAD**: Certificado incorreto, utilizado para simular uma falha na verificação.

4. **Lógica do main()**

    a) Inicialização:
    ```c
    stdio_init_all();
    cyw43_arch_init();
    cyw43_arch_enable_sta_mode();
    cyw43_arch_wifi_connect_timeout_ms(...);
    ```
    Inicializa o sistema, configura o Wi-Fi e conecta à rede usando WIFI_SSID e WIFI_PASSWORD.

    b) Teste com certificado correto:
    ```c
    req.tls_config = altcp_tls_create_config_client(cert_ok, sizeof(cert_ok));
    int pass = http_client_request_sync(...);
    ```
    Cria a configuração TLS com o certificado confiável e executa uma requisição HTTPS que deve ser bem-sucedida.

    c) Teste com certificado errado:
    ```c
    req.tls_config = altcp_tls_create_config_client(cert_bad, sizeof(cert_bad));
    int fail = http_client_request_sync(...);
    ```
    Reaplica a lógica com o certificado incorreto, esperando uma falha na verificação.

    d) Validação final:
    ```c
    if (pass != 0 || fail == 0) {
         panic("test failed");
    }
    ```
    Verifica os resultados dos testes: sucesso com certificado válido e falha com o inválido. Caso contrário, aciona um `panic`.

### 🧪 Resultado Esperado
- **Com certificado correto:** A requisição HTTPS deve ser bem-sucedida, indicando que a verificação do certificado funcionou corretamente.
- **Com certificado inválido:** A conexão TLS deve falhar, demonstrando a detecção apropriada do erro de certificação.

---

## 📄 Arquivo `CMakeLists.txt` (principal)

### ✅ Visão Geral

Este é o arquivo principal de construção do projeto em C para o **Raspberry Pi Pico W**, utilizando a biblioteca **Pico SDK**, conexão Wi-Fi e comunicação via HTTP. Ele define dois executáveis:

1. `picow_cliente_http`: Cliente HTTP que se comunica com um servidor Flask.
2. `picow_http_client_verify`: Cliente HTTP que verifica certificados TLS para conexões seguras.

---

### 📦 Estrutura Modular

```cmake
add_subdirectory(lib_server)
```

Adiciona a subpasta `lib_server`, onde está localizada a biblioteca auxiliar `example_lwip_http_util`.

---

## 🚀 Executável 1: `picow_cliente_http`

```cmake
add_executable(picow_cliente_http picow_cliente_http.c)
```

Cria o primeiro executável a partir do arquivo `picow_cliente_http.c`.

### 📶 Configuração de Wi-Fi

```cmake
set(WIFI_SSID "MINHAREDE")
set(WIFI_PASSWORD "MINHASENHA")
```

Define o nome da rede e a senha como variáveis de CMake.

```cmake
target_compile_definitions(picow_cliente_http PRIVATE   
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\")
```

Passa as credenciais para o código C como macros (`#define`), para serem utilizadas no tempo de execução.

---

### ⚙️ Configurações do Projeto

```cmake
pico_set_program_name(picow_cliente_http "picow_cliente_http")
pico_set_program_version(picow_cliente_http "0.1")
```

### 🖥️ Configuração de Entrada/Saída

```cmake
pico_enable_stdio_uart(picow_cliente_http 0) # Desabilita UART
pico_enable_stdio_usb(picow_cliente_http 1)  # Usa USB para saída do printf
```

---

### 🔗 Bibliotecas e Diretórios

```cmake
target_link_libraries(picow_cliente_http
    pico_stdlib
    hardware_adc
    pico_cyw43_arch_lwip_threadsafe_background
    example_lwip_http_util
)
```

Inclui bibliotecas essenciais para funcionamento do Pico W com rede Wi-Fi e ADC.

```cmake
target_include_directories(picow_cliente_http PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/lib_server
)
```

Define os diretórios onde o compilador irá buscar arquivos `.h`.

```cmake
pico_add_extra_outputs(picow_cliente_http)
```

Gera arquivos auxiliares como `.bin` e `.uf2`.

---

## 🔒 Executável 2: `picow_http_client_verify`

```cmake
add_executable(picow_http_client_verify picow_verify_http.c)
```

Cria o segundo executável para testar conexões HTTPS com verificação de certificado TLS.

### 📌 Macros e Segurança

```cmake
target_compile_definitions(picow_http_client_verify PRIVATE
    WIFI_SSID=\"${WIFI_SSID}\"
    WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
    ALTCP_MBEDTLS_AUTHMODE=MBEDTLS_SSL_VERIFY_REQUIRED
)
```

Além das credenciais, define que a verificação de certificado TLS é obrigatória, aumentando a segurança.

### 📁 Inclusão de diretórios

```cmake
target_include_directories(picow_http_client_verify PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/..
)
```

### 🔗 Bibliotecas

```cmake
target_link_libraries(picow_http_client_verify
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    example_lwip_http_util
)
pico_add_extra_outputs(picow_http_client_verify)
```

---

## 🛠️ Suprimir Warnings

```cmake
set_source_files_properties(
    ${PICO_LWIP_PATH}/src/apps/altcp_tls/altcp_tls_mbedtls.c
    PROPERTIES
    COMPILE_OPTIONS "-Wno-unused-result"
)
```

Desativa alertas de compilação sobre resultados de funções não utilizados, aplicando a um arquivo específico da biblioteca LWIP.

---




