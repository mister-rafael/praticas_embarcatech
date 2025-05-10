

# 📄 Documentação do Arquivo `picow_client_http.c`

### 🔹 Visão Geral

Este arquivo implementa um **cliente HTTP** rodando no **Raspberry Pi Pico W**, com capacidade de:

* Ler valores de um **joystick analógico (eixos X e Y)**.
* Detectar o **estado de dois botões físicos** (A e B).
* Controlar LEDs locais.
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
* **INTERVALO\_MS**: intervalo entre verificações no loop principal (500ms).

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
* A rede e senha são definidos em outros arquivos do projeto (provavelmente `lwipopts_examples_common.h` ou algo similar).

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

### 🔹 Integração com o servidor Flask

Todas as requisições são feitas com base nas seguintes URLs:

* **Botões**: `/CLICK_A`, `/SOLTO_B`, etc.
* **Joystick**: `/joystick?x=1234&y=5678`

No lado do servidor, o Flask interpretará essas rotas para reagir adequadamente (controlar uma interface, registrar dados, etc.).

---

### 🧠 Conclusão

Este código transforma o **Raspberry Pi Pico W** em um **cliente interativo IoT**, enviando comandos físicos (botões/joystick) para um servidor Flask por meio de HTTP. Ele demonstra:

* Leitura de sensores analógicos (joystick).
* Monitoramento de botões digitais.
* Controle de LEDs como feedback local.
* Envio de dados para um servidor remoto via GET.

---

Se quiser, posso continuar com a documentação de outro arquivo — só me dizer qual é o próximo!
