#include "setup_utils.h"
#include "pico/stdlib.h" // Biblioteca padrão do Raspberry Pi Pico
#include "hardware/gpio.h" // Biblioteca para manipulação de GPIO
#include "hardware/pwm.h" // Biblioteca para manipulação de PWM
#include "hardware/clocks.h" // Biblioteca para manipulação de clocks

// Função para configurar o botão
// Esta função inicializa o pino do botão como entrada e ativa o resistor de pull-up interno
void iniciar_botao(uint pin) {
    gpio_init(pin); // Inicializa o pino
    gpio_set_dir(pin, GPIO_IN); // Define o pino como entrada
    gpio_pull_up(pin); // Ativa o resistor de pull-up interno
}

// Função para iniciar o LED
// Esta função inicializa o pino do LED como saída
void iniciar_led(uint pin) {
    gpio_init(pin); // Inicializa o pino
    gpio_set_dir(pin, GPIO_OUT); // Define o pino como saída
}

// Função para iniciar a interface I2C
// Esta função configura os pinos SDA e SCL, e define a taxa de transmissão
// de dados para a interface I2C
void iniciar_i2c(i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint baudrate_khz) {
    i2c_init(i2c, baudrate_khz * 1000); // Inicializa a interface I2C com a taxa de transmissão especificada
    gpio_set_function(sda_pin, GPIO_FUNC_I2C); // Configura o pino SDA para função I2C
    gpio_set_function(scl_pin, GPIO_FUNC_I2C); // Configura o pino SCL para função I2C
    gpio_pull_up(sda_pin); // Ativa o resistor de pull-up no pino SDA
    gpio_pull_up(scl_pin); // Ativa o resistor de pull-up no pino SCL

}
// Função para iniciar o buzzer
void iniciar_buzzer(uint pin) {
    // Configura o pino do buzzer para função PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // Obtém o número do slice associado ao pino 
    uint slice_num = pwm_gpio_to_slice_num(pin); 

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config(); // Obtém a configuração padrão do PWM
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQ * 4096)); // Divisor de clock para ajustar a frequência
    pwm_set_enabled(slice_num, true); // Habilita o PWM no slice

    // Define o nível do pino como 0 (desligado)
    pwm_set_gpio_level(pin, 0); 
}

// FUNÇÃO PARA INICIAR A MATRIZ DE LED
