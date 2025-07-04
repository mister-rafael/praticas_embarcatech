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
#define I2C_SDA_PIN 2 // GPIO2 PARA SDA
#define I2C_SCL_PIN 3 // GPIO3 PARA SCL

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