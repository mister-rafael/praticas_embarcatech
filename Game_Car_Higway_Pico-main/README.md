# Firmware para Raspberry Pi Pico W - Controle de Jogo via Botões

Este firmware foi desenvolvido para controlar um jogo hospedado em um servidor Flask utilizando uma placa **Raspberry Pi Pico W**. Dois botões físicos conectados à plaquinha enviam comandos via HTTP para o servidor, permitindo controlar as ações do jogo (por exemplo, mover para a esquerda ou direita).

## 🔧 Funcionalidades

- Conexão automática à rede Wi-Fi.
- Envio de comandos `LEFT` ou `RIGHT` através de requisições HTTP GET.
- LEDs azuis e vermelhos acendem ao pressionar os botões para feedback visual.
- Intervalo configurável entre leituras dos botões.

## 📦 Estrutura

O código principal está no arquivo:

- `picow_http_client.c` – contém a lógica de conexão Wi-Fi e envio dos comandos.

O projeto é configurado e compilado usando CMake.

## 🧪 Dependências

- [Pico SDK](https://github.com/raspberrypi/pico-sdk)
- Biblioteca `lwIP` e `mbedTLS` (já integradas ao SDK com suporte ao Pico W).
- `example_http_client_util.c` – utilitário de requisição HTTP.

## 📡 Conexão com o Jogo

O jogo deve estar hospedado em um servidor Flask que escute os endpoints `/left` e `/right`. A plaquinha envia os comandos via HTTP para o IP e porta definidos nas macros:

```c
#define HOST "192.168.xx.xx"
#define PORT 5000
```

## 📥 Clonagem e Compilação

```bash
git clone <https://github.com/davissontiago/Game_Car_Higway.git>
cd <diretorio>
mkdir build
cd build
cmake ..
make
```

## 🙏 Créditos

Este projeto foi **baseado no código original de** [@igordev23](https://github.com/igordev23), disponível em:

📎 https://github.com/igordev23/picoHttp_local_pyserver/blob/main/picow_http_client.c

Adaptei esse código para uso com um mini game controlado por botões físicos, integrando com um servidor Flask via HTTP.