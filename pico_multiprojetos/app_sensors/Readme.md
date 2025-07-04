# SENSOR DE LUMINOSIDADE BH1750
## Programação em C/C++ para Medir Luminosidade
Agora é a hora de escrever o código que fará tudo funcionar!

### Estrutura do Projeto
O projeto terá um arquivo principal `cliente_sensor_bh1750.c` e o `CMakeLists.txt` que configura a compilação.

```cmake
CMakeLists.txt
```
Este arquivo é fundamental para o Pico SDK. Ele informa ao compilador como construir o projeto e quais bibliotecas incluir. Certifique-se de adicionar a linha target_link_libraries(cliente_sensor_bh1750 pico_stdlib hardware_i2c) para incluir as bibliotecas necessárias para I2C.

#### `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.13)

# Inicializa o SDK do Raspberry Pi Pico
include(pico_sdk_init)

project(bh1750_sensor_example C CXX ASM)

# Configura o target para o seu executável
add_executable(bh1750_sensor_example
    main.c
)

# Inclui as bibliotecas necessárias do Pico SDK
target_link_libraries(bh1750_sensor_example
    pico_stdlib
    hardware_i2c # Importante: para comunicação I2C
)

# Adiciona o suporte a USB para stdio (impressão serial)
pico_enable_stdio_usb(bh1750_sensor_example)

# Opcional: Adiciona o suporte a UART para stdio (se quiser usar UART serial)
# pico_enable_stdio_uart(bh1750_sensor_example)

# Permite fazer 'make flash' para gravar diretamente na placa
pico_add_extra_binary_output(bh1750_sensor_example)
```

#### `cliente_sensor_bh1750.c`
Este é o coração do código. Aqui, será iniciado o hardware, para se comunicar com o sensor e imprimir os resultados.

```c
#include <stdio.h> // Para printf
#include "pico/stdlib.h" // Funções padrão do Pico SDK
#include "hardware/i2c.h" // Para comunicação I2C
#include "hardware/gpio.h" // Para configurar GPIOs

// Endereço I2C do sensor BH1750
// Pode ser 0x23 (se ADDR estiver GND ou desconectado) ou 0x5C (se ADDR estiver VCC)
#define BH1750_ADDR 0x23 

// Comandos de operação do BH1750
#define BH1750_POWER_ON 0x01 // Ativar
#define BH1750_RESET    0x07 // Resetar o registrador de dados
#define BH1750_CONT_HIGH_RES_MODE 0x10 // Modo de medição contínua, alta resolução (1 lux)
#define BH1750_CONT_LOW_RES_MODE  0x13 // Modo de medição contínua, baixa resolução (4 lux)
#define BH1750_ONE_TIME_HIGH_RES_MODE 0x20 // Modo de medição única, alta resolução (1 lux)

// Pinos I2C para a BitDogLab
#define I2C_SDA_PIN GPIO2
#define I2C_SCL_PIN GPIO3

// Função para inicializar o sensor BH1750
void bh1750_init() {
    uint8_t buffer[1];
    buffer[0] = BH1750_POWER_ON;
    i2c_write_blocking(i2c1, BH1750_ADDR, buffer, 1, false); // Ligar o sensor
    sleep_ms(10); // Pequeno atraso

    buffer[0] = BH1750_CONT_HIGH_RES_MODE; // Modo de medição contínua, alta resolução
    i2c_write_blocking(i2c1, BH1750_ADDR, buffer, 1, false); // Iniciar medição
    sleep_ms(10); // Pequeno atraso para o sensor se estabilizar
}

// Função para ler o valor de luminosidade do sensor BH1750
float bh1750_read_light_level() {
    uint8_t buffer[2];
    uint16_t lux_raw;
    float lux;

    // A leitura do BH1750 em modo contínuo é automática.
    // Basta ler os 2 bytes de dados (High byte primeiro, Low byte depois).
    i2c_read_blocking(i2c1, BH1750_ADDR, buffer, 2, false);

    lux_raw = (buffer[0] << 8) | buffer[1]; // Combina os dois bytes
    lux = (float)lux_raw / 1.2; // A datasheet indica dividir por 1.2 para obter o valor em lux

    return lux;
}

int main() {
    // Inicializa a comunicação serial (USB CDC) para imprimir no monitor
    stdio_init_all();
    sleep_ms(2000); // Pequeno atraso para a inicialização serial

    printf("Inicializando BH1750 no RP2040 (BitDogLab)...\n");

    // Inicializa a interface I2C1 nos pinos GPIO2 (SDA) e GPIO3 (SCL)
    i2c_init(i2c1, 100 * 1000); // Inicializa I2C1 com clock de 100 kHz

    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C); // Configura GPIO2 como SDA
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C); // Configura GPIO3 como SCL
    gpio_pull_up(I2C_SDA_PIN); // Habilita pull-up interno no SDA
    gpio_pull_up(I2C_SCL_PIN); // Habilita pull-up interno no SCL

    bh1750_init(); // Inicializa o sensor BH1750

    printf("Sensor BH1750 inicializado. Lendo luminosidade...\n");

    while (1) {
        float lux_value = bh1750_read_light_level(); // Lê o valor de luminosidade
        printf("Luminosidade: %.2f lux\n", lux_value); // Imprime no monitor serial

        sleep_ms(1000); // Espera 1 segundo antes da próxima leitura
    }

    return 0;
}
```

### Detalhes do Código
#### 1. Inclusões:

`stdio.h`: Para a função `printf`, que envia dados para o monitor serial.
`pico/stdlib.h`: Contém funções básicas do Pico SDK, como `sleep_ms`.
`hardware/i2c.h`: Fornece as funções para controlar a comunicação I2C.
`hardware/gpio.h`: Para configurar os pinos GPIO.

#### 2. Definições:

`BH1750_ADDR`: O endereço I2C do sensor. `0x23` é o mais comum.
`BH1750_POWER_ON`, `BH1750_CONT_HIGH_RES_MODE`, etc.: Comandos específicos para operar o BH1750.
`I2C_SDA_PIN`, `I2C_SCL_PIN`: Os pinos GPIO que você conectou (GPIO2 e GPIO3).

#### 3. `bh1750_init()`:

Envia o comando `BH1750_POWER_ON` para ligar o sensor.
Envia o comando `BH1750_CONT_HIGH_RES_MODE` para iniciar a medição contínua em alta resolução.

#### 4. `bh1750_read_light_level()`:

Lê dois bytes do sensor via I2C. O BH1750 envia o valor de lux em dois bytes.
Combina esses dois bytes em um `uint16_t` (um número de 16 bits).
Conversão para Lux: De acordo com a datasheet do BH1750, o valor lido precisa ser dividido por `1.2` para obter a leitura em Lux.

#### 5. `main()`:

`stdio_init_all()`: Inicializa a comunicação serial (USB CDC), permitindo que você veja o printf no monitor serial do VS Code.
`i2c_init(i2c1, 100 * 1000)`: Inicializa a interface I2C1 com uma velocidade de 100 kHz (padrão para I2C).
`gpio_set_function(...)`: Configura os pinos GPIO2 e GPIO3 para funcionarem como SDA e SCL para a interface I2C.
`gpio_pull_up(...)`: Habilita os resistores de pull-up internos nesses pinos. Isso é importante para a comunicação I2C, pois os pinos SDA e SCL precisam ser "puxados para cima" (para um estado lógico alto) quando inativos.
`bh1750_init()`: Chama a função para inicializar o sensor.
Loop `while(1)`: Este loop executa continuamente:
Chama `bh1750_read_light_level()` para obter a leitura de lux.
Usa `printf` para enviar o valor para o monitor serial.
`sleep_ms(1000)`: Adiciona um atraso de 1 segundo para que as leituras não sejam muito rápidas e o monitor serial não fique sobrecarregado.

## Compilar, Carregar e Exibir em Tempo Real
Com o código escrito, o próximo passo é transformá-lo em um programa que o RP2040 possa entender e executá-lo.

### Compilar o Código
No VS Code, abra a Paleta de Comandos (Ctrl+Shift+P ou Cmd+Shift+P).
Digite "CMake: Build" e selecione a opção.
O VS Code, usando as ferramentas do Pico SDK, compilará seu código. Se tudo estiver correto, ele criará um arquivo .uf2 na pasta build/ do seu projeto (e.g., build/bh1750_sensor_example.uf2).
5.2. Carregar o Código para a BitDogLab
Coloque a BitDogLab em modo de bootloader:

Desconecte a BitDogLab do USB.
Pressione e segure o botão BOOTSEL na BitDogLab.
Conecte a BitDogLab ao seu computador via USB.
Solte o botão BOOTSEL.
Sua BitDogLab deve aparecer como um disco USB chamado "RPI-RP2" no seu computador.
Arraste e Solte:

Navegue até a pasta build/ do seu projeto no explorador de arquivos.
Arraste o arquivo .uf2 gerado (e.g., bh1750_sensor_example.uf2) para o disco "RPI-RP2".
A BitDogLab se reiniciará automaticamente e começará a executar o seu programa.
5.3. Exibição em Tempo Real (Monitor Serial)
No VS Code, abra a Paleta de Comandos (Ctrl+Shift+P ou Cmd+Shift+P).
Digite "Serial Monitor: Open Port" e selecione a porta serial correspondente à sua BitDogLab. Se você não souber qual é, tente algumas opções ou desconecte e reconecte a placa para ver qual nova porta aparece.
Defina a taxa de baud para 115200 (que é a taxa padrão para stdio_init_all() no Pico SDK).
Você deverá começar a ver as leituras de luminosidade sendo impressas no monitor serial, atualizando a cada segundo:

Luminosidade: 350.78 lux
Luminosidade: 351.52 lux
Luminosidade: 349.95 lux
...