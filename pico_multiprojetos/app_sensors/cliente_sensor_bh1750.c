#include <stdio.h>         // Para a função printf
#include "pico/stdlib.h"    // Funções padrão do Pico SDK
#include "hardware/i2c.h"     // Para comunicação I2C
#include "hardware/gpio.h"    // Para configurar GPIOs
#include "hardware/pwm.h"     // Para controle de PWM (NOVO)

// --- CONFIGURAÇÃO DO SENSOR BH1750 ---
// Endereço I2C do sensor BH1750
// Pode ser 0x23 (ADDR em GND) ou 0x5C (ADDR em VCC)
const uint BH1750_ADDR = 0x23; 

// Comandos de operação do BH1750
const uint BH1750_POWER_ON = 0x01;
const uint BH1750_RESET    = 0x07;
const uint BH1750_CONT_HIGH_RES_MODE = 0x10;

// Pinos I2C para a BitDogLab (ou configuração padrão do Pico)
const uint I2C_SDA_PIN = 2; // GPIO2 para SDA
const uint I2C_SCL_PIN = 3; // GPIO3 para SCL

// --- CONFIGURAÇÃO DO LED RGB (NOVO) ---
// Defina os pinos GPIO conectados aos canais do LED RGB
const uint LED_R_PIN = 13;
const uint LED_G_PIN = 11;
const uint LED_B_PIN = 12;

// --- CONFIGURAÇÃO DA RESPOSTA À LUZ (NOVO) ---
// Define a faixa de luminosidade (em lux) que controlará o brilho do LED.
// Abaixo de MIN_LUX, o LED ficará apagado.
// Acima de MAX_LUX, o LED ficará no brilho máximo.
const float MIN_LUX   = 0.0;   // Ambiente escuro
const float MAX_LUX   = 500.0; // Ambiente bem iluminado

// Função para inicializar o sensor BH1750
void bh1750_init() {
    uint8_t buffer[1]; // Buffer para enviar comandos ao sensor
    
    buffer[0] = BH1750_POWER_ON; // Comando para ligar o sensor
    i2c_write_blocking(i2c1, BH1750_ADDR, buffer, 1, false);
    sleep_ms(10);

    buffer[0] = BH1750_CONT_HIGH_RES_MODE; // Comando para iniciar a medição em modo de alta resolução contínua
    i2c_write_blocking(i2c1, BH1750_ADDR, buffer, 1, false);
    sleep_ms(10);
}

// Função para ler o valor de luminosidade do sensor BH1750
float bh1750_read_light_level() {
    uint8_t buffer[2];
    uint16_t lux_raw;

    // Lê os 2 bytes de dados do sensor
    i2c_read_blocking(i2c1, BH1750_ADDR, buffer, 2, false);

    // Combina os dois bytes (High byte e Low byte)
    lux_raw = (buffer[0] << 8) | buffer[1]; //ou, lux_raw = (buffer[0] * 256) + buffer[1];
    // Converte o valor bruto para lux, conforme a datasheet (dividir por 1.2)
    return (float)lux_raw / 1.2;
}

// Função para mapear um valor de uma faixa para outra
// Ex: Mapeia o valor de lux (0-500) para um valor de PWM (0-65535)
long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


int main() {
    // Inicializa a comunicação serial (USB CDC) para imprimir no monitor
    stdio_init_all();
    sleep_ms(2000);

    printf("Inicializando sistema de controle de LED com BH1750...\n");

    // --- INICIALIZAÇÃO DO I2C E SENSOR BH1750 ---
    i2c_init(i2c1, 100 * 1000); // Clock de 100 kHz
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    bh1750_init();
    printf("Sensor BH1750 inicializado.\n");


    // --- INICIALIZAÇÃO DO PWM PARA O LED RGB (NOVO) ---
    // Configura os pinos do LED para a função PWM
    gpio_set_function(LED_R_PIN, GPIO_FUNC_PWM);
    gpio_set_function(LED_G_PIN, GPIO_FUNC_PWM);
    gpio_set_function(LED_B_PIN, GPIO_FUNC_PWM);

    // Encontra o "slice" PWM para cada pino do LED
    uint slice_r = pwm_gpio_to_slice_num(LED_R_PIN);
    uint slice_g = pwm_gpio_to_slice_num(LED_G_PIN);
    uint slice_b = pwm_gpio_to_slice_num(LED_B_PIN);

    // Configura o PWM. O valor de "wrap" define o topo da contagem (resolução).
    // 65535 é o valor máximo para 16 bits, oferecendo alta resolução de brilho.
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, 65535); 
    
    // Inicia o PWM para cada slice
    pwm_init(slice_r, &config, true);
    pwm_init(slice_g, &config, true);
    pwm_init(slice_b, &config, true);
    
    printf("PWM para LED RGB inicializado.\n");
    printf("A intensidade do LED mudará de acordo com a luz ambiente.\n");


    // Loop principal
    while (1) {
        // 1. LÊ O SENSOR
        float lux_value = bh1750_read_light_level();

        // 2. MAPEIA O VALOR DE LUX PARA O NÍVEL DE PWM (NOVO)
        // Usa a função map para converter a faixa de lux (ex: 5-500) para a faixa de PWM (0-65535)
        // O valor 65535 corresponde a 100% de duty cycle (brilho máximo)
        uint16_t pwm_level = map(lux_value, MIN_LUX, MAX_LUX, 0, 65535);

        // Garante que o valor de PWM não ultrapasse os limites
        if (pwm_level < 0) pwm_level = 0;
        if (pwm_level > 65535) pwm_level = 65535;
        
        // Inverte a lógica: mais luz = LED mais fraco (luz de preenchimento)
        // Para fazer o LED ficar mais forte com mais luz, comente a linha abaixo.
        //pwm_level = 65535 - pwm_level;

        // 3. ATUALIZA A INTENSIDADE DO LED (NOVO)
        // Define o mesmo nível de intensidade para R, G e B para criar luz branca.
        // A função pwm_set_gpio_level define o "duty cycle".
        pwm_set_gpio_level(LED_R_PIN, pwm_level); // Vermelho
        pwm_set_gpio_level(LED_G_PIN, pwm_level); // Verde
        pwm_set_gpio_level(LED_B_PIN, pwm_level); // Azul

        // Imprime os valores no monitor serial para depuração
        printf("Luminosidade: %.2f lux -> Nivel PWM: %d\n", lux_value, pwm_level);

        // Espera um curto período antes da próxima leitura para não sobrecarregar
        sleep_ms(100); 
    }

    return 0;
}