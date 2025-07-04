#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <string.h>
#include "ssd1306.h"

// --- Constantes e Definições ---
// Configuração I2C para Display OLED (integrado)
const uint I2C_SDA_DISPLAY = 14;
const uint I2C_SCL_DISPLAY = 15;

// Define os pinos GPIO que serão usados para SDA e SCL do sensor AHT10
// Conforme a minha montagem
static const uint I2C_SDA_AHT10 = 0;
static const uint I2C_SCL_AHT10 = 1;
static const uint I2C_BAUDRATE = 100 * 1000; // 100kHz

// Endereço I2C do sensor AHT10
const uint8_t AHT10_ADDR = 0x38;

// Comandos do AHT10 (retirados do datasheet do sensor)
const uint8_t AHT10_CMD_INIT[] = {0xE1, 0x08, 0x00};    // Comando para inicializar
const uint8_t AHT10_CMD_TRIGGER[] = {0xAC, 0x33, 0x00}; // Comando para pedir uma medição
const uint8_t AHT10_CMD_SOFT_RESET[] = {0xBA};          // Comando para resetar

// Função para inicializar o sensor AHT10
void aht10_init()
{
    // Primeiro, faz um reset suave para garantir que o sensor esteja em um estado conhecido
    i2c_write_blocking(i2c0, AHT10_ADDR, AHT10_CMD_SOFT_RESET, 1, false);
    sleep_ms(20); // Espera 20ms após o reset

    // Envia o comando de inicialização
    i2c_write_blocking(i2c0, AHT10_ADDR, AHT10_CMD_INIT, 3, false);
    sleep_ms(300); // O datasheet recomenda esperar um pouco após a inicialização
}

int main()
{
    // Inicializa a comunicação serial via USB para que possamos ver os dados no computador
    stdio_init_all();

    // Dá um tempo para você abrir o monitor serial
    sleep_ms(2000);
    printf("Iniciando o programa do sensor AHT10...\n");

    // --- Configuração do I2C ---
    // 1. Inicializa a interface I2C com uma velocidade de 100kHz (padrão seguro)
    i2c_init(i2c0, I2C_BAUDRATE);
    // 2. Inicializa a interface I2C para o display OLED
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    // 3. Define a função dos pinos GP0 e GP1 para I2C do Sensor AHT10
    gpio_set_function(I2C_SDA_AHT10, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_AHT10, GPIO_FUNC_I2C);
    // 4. Define a função dos pinos GP14 e GP15 para I2C do display OLED
    gpio_set_function(I2C_SDA_DISPLAY, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_DISPLAY, GPIO_FUNC_I2C);
    // 5. Ativa os resistores de pull-up internos da Pico para os pinos I2C do sensor e do OLED.
    // O I2C precisa desses resistores para funcionar corretamente.
    gpio_pull_up(I2C_SDA_AHT10);
    gpio_pull_up(I2C_SCL_AHT10);
    gpio_pull_up(I2C_SDA_DISPLAY);
    gpio_pull_up(I2C_SCL_DISPLAY);

    // Inicializa o nosso sensor
    aht10_init();
    printf("Sensor AHT10 inicializado!\n");
    // Inicializa o display OLED
    ssd1306_init();
    // Buffer para armazenar os dados lidos do sensor
    uint8_t data[6];

    // Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1};
        
    calculate_render_area_buffer_length(&frame_area); // calcula o tamanho do buffer necessário para armazenar os dados da área de renderização.
    uint8_t buffer[ssd1306_buffer_length];            // É um buffer que armazena a imagem/tela antes de ser enviada para o OLED.

    char temp_str[20], hum_str[20];

    // Loop infinito para medir e imprimir os dados continuamente
    while (true)
    {
        // Limpa o buffer do Display
        memset(buffer, 0, sizeof(buffer));

        // Envia o comando para o AHT10 começar uma nova medição
        i2c_write_blocking(i2c0, AHT10_ADDR, AHT10_CMD_TRIGGER, 3, false);

        // O sensor precisa de um tempo para realizar a medição (datasheet diz ~75ms)
        sleep_ms(80);

        // Agora, lê os 6 bytes de dados do sensor
        i2c_read_blocking(i2c0, AHT10_ADDR, data, 6, false);

        // --- Cálculos para converter os dados brutos ---
        // (Baseado nas fórmulas do datasheet do AHT10)

        // 1. Cálculo da Umidade
        // Combina os bytes de umidade usando operações de bits
        uint32_t raw_humidity = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
        // Converte o valor bruto para porcentagem
        float humidity = ((float)raw_humidity * 100) / (1 << 20);

        // 2. Cálculo da Temperatura
        // Combina os bytes de temperatura
        uint32_t raw_temp = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];
        // Converte o valor bruto para graus Celsius
        float temperature = ((float)raw_temp / (1 << 20)) * 200 - 50;

        // Prepara os valores para serem gravados no buffer
        snprintf(temp_str, sizeof(temp_str), "Temp: %.2f C", temperature);
        snprintf(hum_str, sizeof(hum_str), "Umidade: %.2f ", humidity);

        // Desenha os valores no buffer do display
        ssd1306_draw_string(buffer, 0, 0, temp_str);
        ssd1306_draw_string(buffer, 0, 16, hum_str);

        // Verifica condições de alerta
        if (humidity > 50.0 || temperature < 20.0)
        {
            ssd1306_draw_string(buffer, 0, 32, "ALERTA!");
            ssd1306_draw_string(buffer, 0, 40, "Cond adversas!");
        }
        // Renderiza o buffer no display OLED
        render_on_display(buffer, &frame_area);
        // Imprime os valores no console
        printf("Umidade: %.2f %%, Temperatura: %.2f C\n", humidity, temperature);

        // Espera 2 segundos antes da próxima leitura
        sleep_ms(2000);
    }

    return 0;
}