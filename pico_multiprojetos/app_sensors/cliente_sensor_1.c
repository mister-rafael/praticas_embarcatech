#include <stdio.h>
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"
#include "pico/async_context.h"
#include "lwip/altcp_tls.h"
#include "exemplo_http_client_util.h"
// INLUDES PARA USO DO JOYSTICK
#include "hardware/adc.h"
#include "setup_utils.h" //Biblioteca de configuração do sistema e periféricos.
#include "actions_io.h"  //Biblioteca de ações de entrada e saída.

// ========== CONFIGURAÇÕES ======================== //
// #define HOST "192.168.0.20" // IP do servidor de casa
#define HOST "10.3.10.191" // IP do servidor do IFPI
#define PORT 5000          // Porta do servidor
#define INTERVALO_MS 500   // Intervalo de verificação do botão (em milissegundos)

#define LED_R_PIN 12 // LED vermelho
#define LED_G_PIN 13 // LED verde
#define LED_B_PIN 11 // LED azul

#define I2C_PORT i2c0
#define SDA_PIN 4 // GPIO4 para SDA
#define SCL_PIN 5 // GPIO5 para SCL

// Endereço do BH1750
#define BH1750_ADDR 0x23
#define BH1750_CMD_START 0x10 // Medição contínua, alta resolução

// Inicializa os LEDs
void init_leds()
{
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_put(LED_R_PIN, 0);

    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_put(LED_G_PIN, 0);

    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);
    gpio_put(LED_B_PIN, 0);
}

// Envia comando ao BH1750
void bh1750_start_measurement()
{
    uint8_t cmd = BH1750_CMD_START;
    i2c_write_blocking(I2C_PORT, BH1750_ADDR, &cmd, 1, false);
}

// Lê o valor da luminosidade do BH1750
uint16_t bh1750_read_lux()
{
    uint8_t data[2];
    i2c_read_blocking(I2C_PORT, BH1750_ADDR, data, 2, false);
    return (data[0] << 8) | data[1];
}

// Controla os LEDs
void set_leds(bool red, bool green, bool blue)
{
    gpio_put(LED_R_PIN, red);
    gpio_put(LED_G_PIN, green);
    gpio_put(LED_B_PIN, blue);
}
int main()
{
    // Inicializando entradas padrão
    stdio_init_all();
    printf("\nIniciando cliente HTTP...\n");
    // Inicializa UART para debug (opcional)
    stdio_init_all();

    // Inicializa LEDs
    init_leds();

    // Inicializa I2C para o BH1750
    i2c_init(I2C_PORT, 100 * 1000); // Frequência de 100 kHz
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Configura o BH1750 para começar a medir
    bh1750_start_measurement();
    // Inicializando Wi-Fi
    if (wifi_connect(WIFI_SSID, WIFI_PASSWORD))
    {
        return 1; // Sai do programa em caso de erro
    }
    // O loop principal do programa, onde as ações são realizadas continuamente
    while (true)
    {
        uint16_t lux = bh1750_read_lux();
        printf("Luminosidade: %u lux\n", lux);

        // Atualiza os LEDs com base na luminosidade
        if (lux < 100)
        {
            set_leds(1, 0, 0); // LED vermelho aceso
        }
        else if (lux < 500)
        {
            set_leds(0, 1, 0); // LED verde aceso
        }
        else
        {
            set_leds(0, 0, 1); // LED azul aceso
        }

        sleep_ms(1000); // Aguarda 1 segundo para a próxima leitura
    }
    return 0;
}