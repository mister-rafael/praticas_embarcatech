# Documentação do projeto app_servidor_flask
## Estrutura do projeto
### 📂 Estrutura geral
Arquivos principais:

`picow_cliente_http.c` e `picow_verify_http.c`, códigos para o cliente Pico W.

`CMakeLists.txt`: configuração de build do projeto (raiz e em subpastas)

### 📁 web/

`server.py`: servidor Flask (é a base da comunicação com o Pico W)

`templates/index.html`: página web entregue pelo servidor Flask

### 📁 lib_server/

Código C auxiliar para requisições HTTP e configurações de rede.

`example_http_client_util.c` e `example_http_client_util.h`, códigos para o cliente Pico W.

`CMakeLists.txt`: configuração de build do projeto.

Outros arquivos de bibliotecas.

---

# 📂 Estrutura geral
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
#### Implementação de cada IF
```c
if (x < 1000) // O Joystick foi movido para a esquerda
        {
            gpio_put(LED_BLUE, 1);
            // Envia os dados para o servidor Flask
            enviar_dados_joystick(x, y);
        }
```

---
#### 🔹 Integração com o servidor Flask

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

### 🚀 Executável 1: `picow_cliente_http`

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

### 🔒 Executável 2: `picow_http_client_verify`

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

### 🛠️ Suprimir Warnings

```cmake
set_source_files_properties(
    ${PICO_LWIP_PATH}/src/apps/altcp_tls/altcp_tls_mbedtls.c
    PROPERTIES
    COMPILE_OPTIONS "-Wno-unused-result"
)
```

Desativa alertas de compilação sobre resultados de funções não utilizados, aplicando a um arquivo específico da biblioteca LWIP.

---

# 📁 web/
## 🌐 Arquivo `server.py` – *Servidor Flask + WebSocket para Controle Remoto*

### 🧩 Bibliotecas Usadas

```python
from flask import Flask, render_template, request 
from flask_socketio import SocketIO, emit, send
```

* `Flask`: cria a aplicação web.
* `SocketIO`: habilita comunicação em tempo real entre o servidor e os clientes via WebSockets.

---

### 🚀 Inicialização do Servidor

```python
app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*")
```

* `cors_allowed_origins="*"` permite que qualquer origem (inclusive o Pico W) se conecte ao servidor.

---

### 📄 Rota Principal: Interface Web

```python
@app.route('/')
def index():
    return render_template('index.html')  # Renderiza o template web/templates/index.html

```

Serve o arquivo `index.html` dentro de `templates/`, que é a interface visual do projeto.

---

### 🔘 Rotas de Comando via HTTP

Cada uma dessas rotas aceita métodos `GET` ou `POST` e emite um evento WebSocket chamado `command`.

### 🔴 Botão A

```python
@app.route('/CLICK_A', methods=['GET', 'POST']) # Define a rota para o comando de clique
# Define uma função para lidar com o evento de clique do botão A
def click_a():
    print("Comando: Botão A, pressionado")
    socketio.emit('command', {'action': 'click_a'})  # Envia comando para ON
    return 'Click command sent', 200 # Retorna resposta HTTP 200

@app.route('/SOLTO_A', methods=['GET', 'POST']) # Define a rota para o comando de solto
def solto_a():
    print("Comando: Botão A, solto")
    socketio.emit('command', {'action': 'solto_a'})  # Envia comando para OFF
    return 'solto command sent', 200
```

### 🔵 Botão B

```python
@app.route('/CLICK_B', methods=['GET', 'POST']) # Define a rota para o comando de clique
# Define uma função para lidar com o evento de clique do botão A
def click_b():
    print("Comando: Botão B, pressionado")
    socketio.emit('command', {'action': 'click_b'})  # Envia comando para ON
    return 'Click command sent', 200 # Retorna resposta HTTP 200

@app.route('/SOLTO_B', methods=['GET', 'POST']) # Define a rota para o comando de solto
def solto_b():
    print("Comando: Botão B, solto")
    socketio.emit('command', {'action': 'solto_b'})  # Envia comando para OFF
    return 'solto command sent', 200
```

---

### 🎮 Controle de Joystick

```python
@app.route('/joystick', methods=['GET'])
def joystick():
    x = request.args.get('x', type=int)
    y = request.args.get('y', type=int)
    print(f"Dados do joystick recebidos: X = {x}, Y = {y}")
    socketio.emit('joystick', {'x': x, 'y': y})  # Envia os dados do joystick para o cliente
    return "Dados recebidos", 200
```

Essa rota espera dois parâmetros de consulta: `x` e `y`, representando a posição do joystick. Os dados são emitidos em tempo real para os clientes conectados.

---

### 🎯 Execução do Servidor

```python
if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=5000)
```

* `host='0.0.0.0'`: permite acesso de qualquer dispositivo da rede.
* `port=5000`: define a porta do servidor.

---

### ✅ Resumo das Rotas HTTP

| Rota        | Função                       | Evento WebSocket Emitido |
| ----------- | ---------------------------- | ------------------------ |
| `/CLICK_A`  | Pressionar Botão A           | `{'action': 'click_a'}`  |
| `/SOLTO_A`  | Soltar Botão A               | `{'action': 'solto_a'}`  |
| `/CLICK_B`  | Pressionar Botão B           | `{'action': 'click_b'}`  |
| `/SOLTO_B`  | Soltar Botão B               | `{'action': 'solto_b'}`  |
| `/joystick` | Enviar dados X/Y do joystick | `{'x': X, 'y': Y}`       |

---

## 🌐 Arquivo `index.html` – *Página Web*

Funciona como um **painel em tempo real** para visualizar o estado dos botões físicos da BitDogLab e os dados do joystick, tudo via **WebSockets com Socket.IO**.

---

### 🖼️ Layout

* **Dois botões visuais**: `Botão A` e `Botão B`.
```html
<div class="button-container">
    <button id="botaoA" class="btn">Botão A</button>
    <button id="botaoB" class="btn">Botão B</button>
</div>
```
* **Caixa de status**: Mostra se os botões estão pressionados ou soltos.
```html
<div id="status-box">Aguardando...</div>
```
* **Leitura do Joystick**: Mostra os valores `X` e `Y`.
```html
<div class="joystick">
    <h2>Posição do Joystick</h2>
    <p>X: <span id="x-pos">0</span></p>
    <p>Y: <span id="y-pos">0</span></p>
  </div>
```

---

### ⚙️ Lógica JavaScript com Socket.IO

#### ✅ Conexão com o servidor

```js
const socket = io();
```

Conecta automaticamente com o `server.py` via WebSocket.

### Pegando elementos do HTML

Usa o método `document.getElementById`, para acessar os **elementos com um ID específico** no DOM (Document Object Model).

---

#### 🔍 `const statusBox = document.getElementById('status-box');`

* Isso busca o elemento com o `id="status-box"`, que é a **caixa de status central** da tela (o retângulo que mostra "Aguardando...", e "Botão A pressionado", etc.).
* Depois disso, a variável `statusBox` é usada para:

  * **Alterar o texto** com `statusBox.textContent = "Botão A, pressionado!..."`
  * **Mudar cor de fundo** com `statusBox.style.backgroundColor = "green"`

---

#### 🔍 `const botaoA = document.getElementById('botaoA');`

* Busca o botão com `id="botaoA"`.
* Isso permite, por exemplo:

  * Adicionar/remover classes CSS com `botaoA.classList.add('pressionado')`
  * Identificar quando ele foi pressionado/solto

---

#### 🔍 `const botaoB = document.getElementById('botaoB');`

* Mesma lógica que o `botaoA`, mas agora para o **botão B**.
* Usado para controlar sua aparência e estado conforme os eventos.

---

#### 🔍 `const xSpan = document.getElementById('x-pos');`

* Esse seleciona o `<span id="x-pos">`, onde será exibido o valor da posição **X** do joystick.
* Depois, é possível atualizar o valor usando `xSpan.textContent = novoValorX`.

---

#### 🔍 `const ySpan = document.getElementById('y-pos');`

* Igual ao anterior, mas para o valor **Y** do joystick.

---

### 🧠 Controle de estados

```js
let estadoA = 'solto';
let estadoB = 'solto';
```

Essas variáveis controlam se os botões estão pressionados ou não, evitando estados inconsistentes.

---

### 🔄 Recebimento de eventos do servidor
#### 📥 Evento: `command`

```js
socket.on('command', (data) => {
      if (data.action === 'click_a') {
        if (estadoA != 'pressionado') { // Verifica se o botão A não está pressionado
          botaoA.classList.add('pressionado');
          statusBox.textContent = 'Botão A, pressionado!';
          estadoA = 'pressionado';

        }
      } else if (data.action === 'solto_a') {
        if (estadoA == 'pressionado') { // Verifica se o botão A está pressionado
          botaoA.classList.remove('pressionado');
          statusBox.textContent = 'Botão A, solto!';
          estadoA = 'solto';
        }
      }
      //VERIFICAR BOTÃO B
      if (data.action === 'click_b') {
        if (estadoB != 'pressionado') { // Verifica se o botão B não está pressionado
          botaoB.classList.add('pressionado');
          statusBox.textContent = 'Botão B, pressionado!';
          estadoB = 'pressionado';
        }
      } else if (data.action === 'solto_b') {
        if (estadoB == 'pressionado') { // Verifica se o botão B está pressionado
          botaoB.classList.remove('pressionado');
          statusBox.textContent = 'Botão B, solto!';
          estadoB = 'solto';
        }
      }

      // Atualiza mensagem central

      if (estadoA === 'pressionado' || estadoB === 'pressionado') {
        statusBox.style.backgroundColor = 'green';
        statusBox.innerHTML = `Botão A: ${estadoA.toUpperCase()}<br>Botão B: ${estadoB.toUpperCase()}`;
      } else {
        statusBox.style.backgroundColor = 'red';
        statusBox.textContent = `Aguardando...`;
      }
    });
```

* Trata os comandos: `click_a`, `solto_a`, `click_b`, `solto_b`.
* Atualiza classes CSS e o conteúdo do `#status-box` dinamicamente.
* Muda a cor do `status-box`:

  * **Verde** se algum botão estiver pressionado.
  * **Preto** caso contrário.
* Finaliza com a exibição efeita do conteudo na `status-box`.
---
#### 📥 Evento: `joystick`

```js
socket.on('joystick', (data) => {
      // Atualiza os spans dos valores X e Y
      xSpan.textContent = data.x;
      ySpan.textContent = data.y;
      console.log(`Joystick: X = ${data.x}, Y = ${data.y}`);
});
```

Atualiza dinamicamente a posição do joystick em tempo real.

---

# 📁 lib_server/
## Arquivo `example_http_client_util.c`

Este arquivo contém funções utilitárias para realizar requisições HTTP de forma assíncrona, com suporte para conexões TLS quando necessário. Ele faz parte da biblioteca auxiliar do projeto **app_servidor_flask** e integra funcionalidades da stack LWIP, do Pico SDK e do mbedTLS.

### Funcionalidades

- **Conexão Wi-Fi:**  
  Implementa a função `wifi_connect` para inicializar o módulo Wi-Fi via `cyw43_arch` e conectar à rede especificada usando WPA2 AES.

- **Inicialização de Periféricos:**  
  - `iniciar_botao`: Configura um pino como entrada para botões.  
  - `iniciar_led`: Configura um pino como saída para LEDs.  
  - `iniciar_joystick`: Inicializa os pinos dos eixos X e Y do joystick e o botão do joystick.

- **Impressão de Dados HTTP:**  
  - `http_client_header_print_fn`: Imprime os cabeçalhos da resposta HTTP.  
  - `http_client_receive_print_fn`: Imprime o corpo da resposta HTTP e libera o buffer utilizado.

- **Callbacks Internos e Tratamento de Resultados:**  
  Funções internas que processam callbacks para cabeçalhos, recebimento de dados (`internal_recv_fn`) e resultado final (`internal_result_fn`).  
  Estas funções encapsulam os callbacks definidos na estrutura `EXAMPLE_HTTP_REQUEST_T`, permitindo a customização do comportamento após a requisição.

- **Suporte a TLS com SNI:**  
  A função `altcp_tls_alloc_sni` substitui o alocador padrão para configurar o SNI (Server Name Indication), essencial para conexões HTTPS seguras.

- **Requisições HTTP:**  
  - `http_client_request_async`: Realiza uma requisição HTTP de forma assíncrona, integrando o contexto de async e configurando os callbacks necessários.  
  - `http_client_request_sync`: Executa uma requisição HTTP de forma síncrona, bloqueando até que a requisição seja concluída e retornando o resultado.
### Dependências

- **Pico SDK:** Para funcionalidades básicas e gerenciamento do hardware do Raspberry Pi Pico.
- **LWIP (Lightweight IP):** Para o stack TCP/IP e suporte à camada de abstração TLS.
- **mbedTLS:** Para a configuração e verificação dos certificados TLS.
### Licença

Este código está licenciado sob a [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause).

---

## Arquivo `example_http_client_util.h`

Este arquivo de header define as interfaces e estruturas para o auxílio em requisições HTTP no projeto. Ele contém definições dos pinos para botões, LEDs, e joystick, além de declarar funções utilitárias para:
- **Conexão Wi-Fi**
- **Inicialização de periféricos** (botões, LEDs, joystick)
- **Execução de requisições HTTP** (assíncronas e síncronas)
- **Callbacks para depuração** (impressão de cabeçalhos e corpo da resposta)

### Definições de Pinos

- **Botões e LEDs:**
  - `BUTTON_A` e `BUTTON_B`: Pinos dos botões.
  - `LED_RED`, `LED_GREEN`, `LED_BLUE`: Pinos dos LEDs.

- **Joystick:**
  - `JOYSTICK_X_PIN` e `JOYSTICK_Y_PIN`: Canais ADC para os eixos X e Y.
  - `JOYSTICK_SW_PIN`: Pino digital para o botão do joystick.

### Funções Auxiliares

- `int wifi_connect(const char *ssid, const char *password);`  
  Conecta o dispositivo à rede Wi-Fi.

- `void iniciar_botao(uint pin);`  
  Inicializa um botão no pino especificado.

- `void iniciar_led(uint pin);`  
  Inicializa um LED no pino especificado.

- `void iniciar_joystick(uint pinx, uint piny, uint pinw);`  
  Configura os pinos do joystick, incluindo os canais ADC e o botão.

### Estrutura de Requisição HTTP

- `EXAMPLE_HTTP_REQUEST_T`  
  Estrutura que armazena os parâmetros necessários para realizar uma requisição HTTP, incluindo:
  - Endereço do host e URL.
  - Funções de callback para cabeçalhos, corpo da resposta e resultado final.
  - Configurações para TLS, se aplicável.
  - Parâmetros para execução das requisições (assíncronas e síncronas).

### Funções de Requisição HTTP

- `int http_client_request_async(struct async_context *context, EXAMPLE_HTTP_REQUEST_T *req);`  
  Inicia uma requisição HTTP de forma assíncrona, retornando imediatamente após disparar a operação.

- `int http_client_request_sync(struct async_context *context, EXAMPLE_HTTP_REQUEST_T *req);`  
  Executa uma requisição HTTP de forma síncrona, aguardando sua conclusão antes de retornar.

### Callbacks para Depuração

- `err_t http_client_header_print_fn(...)`  
  Callback que imprime os cabeçalhos HTTP recebidos no terminal.

- `err_t http_client_receive_print_fn(...)`  
  Callback que imprime o corpo da resposta HTTP no terminal.

### Uso

Este header é utilizado em conjunto com a biblioteca LWIP para efetuar requisições HTTP, permitindo a personalização do tratamento dos dados recebidos através de callbacks. Além disso, via definições de pinos e funções auxiliares, fornece suporte para a configuração de periféricos comuns em aplicações embarcadas.

### Licença

Este código está licenciado sob a [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause).

---

## Arquivo `CMakeLists.txt`

Este arquivo CMakeLists.txt é responsável pela configuração da biblioteca auxiliar para requisições HTTP utilizando a stack LWIP e funcionalidades do Pico SDK. A biblioteca, nomeada como `example_lwip_http_util`, é criada como uma biblioteca INTERFACE, permitindo a inclusão de código e cabeçalhos compartilhados entre diferentes partes do projeto.

### Funcionalidades

- **Criação da Biblioteca:**  
  Utiliza a função `pico_add_library` para criar a biblioteca como INTERFACE.  
  (Comentado encontra-se a opção de criação como `STATIC`).

- **Adição de Código-Fonte:**  
  A fonte `example_http_client_util.c` é adicionada como parte dos sources da biblioteca.

- **Inclusão de Diretórios de Cabeçalhos:**  
  O diretório atual é incluído para que os cabeçalhos necessários sejam encontrados durante a compilação.

- **Vinculação de Bibliotecas Externas:**  
  A função `pico_mirrored_target_link_libraries` vincula bibliotecas críticas para o funcionamento do HTTP, como:
  - `pico_lwip_http`
  - `pico_lwip_mbedtls`
  - `pico_mbedtls`
  - `hardware_adc`
  - `hardware_pio` (necessário para operações com PIO)

### Uso

Este CMakeLists.txt é utilizado pelo sistema de build CMake do Pico SDK para configurar e compilar a biblioteca auxiliar que será, posteriormente, integrada ao projeto principal. Certifique-se de ter todas as dependências (bibliotecas do Pico SDK e LWIP com suporte a mbedTLS) configuradas corretamente em seu ambiente.

### Observações

- A opção de criar a biblioteca como `STATIC` está comentada, pois neste caso opta-se por uma biblioteca INTERFACE, ideal para funções auxiliares que consistem principalmente em cabeçalhos e pequenas implementações.
- O arquivo faz uso de convenções e funções específicas do Pico SDK (como `pico_add_library` e `pico_mirrored_target_link_libraries`).

### Requisitos

- [Pico SDK](https://www.raspberrypi.com/documentation/microcontrollers/)
- LWIP com suporte a mbedTLS

---

