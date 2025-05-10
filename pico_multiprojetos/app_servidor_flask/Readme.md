# Documentação do projeto app_servidor_flask
### 📂 Estrutura geral
Arquivos principais:

`picow_cliente_http.c` e `picow_verify_http.c`, códigos para o cliente Pico W.

`CMakeLists.txt`: configuração de build do projeto (raiz e em subpastas)

### 📁 lib_server/

Código C auxiliar para requisições HTTP e configurações de rede

### 📁 web/

`server.py`: servidor Flask (será a base da comunicação com o Pico W)

`templates/index.html`: página web entregue pelo servidor Flask



## 📄 Documentação do Arquivo `picow_client_http.c`

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
* A rede e senha são definidos em outros arquivos do projeto (no caso atual, no arquivo `example_http_cliente.c`).

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

📄 pico_verify_http.c
✅ O que este código faz?
Este é um programa de teste de verificação de certificados TLS (HTTPS) no Raspberry Pi Pico W. Ele testa se o cliente HTTP embutido consegue:

Realizar uma requisição HTTPS válida, usando um certificado confiável (root cert correto),

E depois, tenta a mesma requisição com um certificado inválido (root cert errado), esperando que falhe.

Esse teste é fundamental para garantir a segurança das conexões HTTPS em aplicações que precisam de confiança em servidores remotos (como downloads de firmware, atualizações ou comandos autenticados).

🧩 Como se encaixa no projeto?
Este arquivo atua como um exemplo prático e teste funcional da infraestrutura de HTTPS montada com:

Wi-Fi via cyw43_arch,

Cliente HTTP baseado em LWIP,

Conexões TLS via altcp_tls.

É útil para garantir que os componentes de rede segura estejam funcionando corretamente antes de usá-los em produção no restante do projeto.

🧾 Estrutura do Código
1. 📦 Inclusões
c
Copiar
Editar
#include "example_http_client_util.h"
#include "lwip/altcp_tls.h"
Importa o cliente HTTP e a pilha TLS usada.

2. 🌐 Definições de Host e URL
c
Copiar
Editar
#define HOST "fw-download-alias1.raspberrypi.com"
#define URL_REQUEST "/net_install/boot.sig"
Define o servidor e o arquivo a ser requisitado. É um domínio da própria Raspberry Pi, com certificado estável.

3. 🔐 Certificados TLS
Certificado correto (TLS_ROOT_CERT_OK)
Este é um certificado raiz confiável, exportado de um navegador. Ele é usado para validar a cadeia de segurança da conexão.

Certificado incorreto (TLS_ROOT_CERT_BAD)
Um certificado "falso", usado para simular erro de verificação.

4. 🧠 Lógica do main()
Inicialização:
c
Copiar
Editar
stdio_init_all();
cyw43_arch_init();
cyw43_arch_enable_sta_mode();
cyw43_arch_wifi_connect_timeout_ms(...);
Inicializa o sistema, Wi-Fi e conecta-se à rede com WIFI_SSID e WIFI_PASSWORD.

Teste com certificado correto:
c
Copiar
Editar
req.tls_config = altcp_tls_create_config_client(cert_ok, sizeof(cert_ok));
int pass = http_client_request_sync(...);
Cria a configuração TLS usando o certificado correto e faz uma requisição HTTP segura. Espera-se que funcione.

Teste com certificado errado:
c
Copiar
Editar
req.tls_config = altcp_tls_create_config_client(cert_bad, sizeof(cert_bad));
int fail = http_client_request_sync(...);
Repete o teste com o certificado inválido. Espera-se que a requisição falhe por erro de verificação de certificado.

Validação final:
c
Copiar
Editar
if (pass != 0 || fail == 0) {
    panic("test failed");
}
Confirma se os comportamentos esperados ocorreram (ok com cert válido, erro com inválido). Caso contrário, dispara panic.

🧪 Resultado Esperado
Com certificado correto: Test passed

Com certificado inválido: falha na conexão TLS (detectado corretamente)


