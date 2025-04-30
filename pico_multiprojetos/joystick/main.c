#include "pico/stdlib.h"
#include "hardware/adc.h"

// Definições de pinos
#define LED_RED_PIN 13
#define LED_GREEN_PIN 11
#define LED_BLUE_PIN 12
#define JOYSTICK_X_PIN 27
#define JOYSTICK_Y_PIN 26
#define JOYSTICK_SW_PIN 22

// Função para inicializar o LED RGB
void iniciar_led_rgb() {
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
}

// Função para inicializar o joystick
void iniciar_joystick() {
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
    gpio_init(JOYSTICK_SW_PIN);
    gpio_set_dir(JOYSTICK_SW_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_SW_PIN);
}

// Função para controlar o LED RGB
void controlar_led_rgb(uint16_t x, uint16_t y) {
    // Define os limites para as direções
    if (x < 1000) {
        gpio_put(LED_RED_PIN, 1);  // Vermelho
        gpio_put(LED_GREEN_PIN, 0);
        gpio_put(LED_BLUE_PIN, 0);
    } else if (x > 3000) {
        gpio_put(LED_RED_PIN, 0);
        gpio_put(LED_GREEN_PIN, 1);  // Verde
        gpio_put(LED_BLUE_PIN, 0);
    } else if (y < 1000) {
        gpio_put(LED_RED_PIN, 0);
        gpio_put(LED_GREEN_PIN, 0);
        gpio_put(LED_BLUE_PIN, 1);  // Azul
    } else {
        gpio_put(LED_RED_PIN, 0);
        gpio_put(LED_GREEN_PIN, 0);
        gpio_put(LED_BLUE_PIN, 0);  // Desliga
    }
}

int main() {
    stdio_init_all();

    // Inicializa o LED RGB e o joystick
    iniciar_led_rgb();
    iniciar_joystick();

    while (true) {
        // Lê os valores do joystick
        adc_select_input(0); // Seleciona o canal do eixo X
        uint16_t x = adc_read();
        adc_select_input(1); // Seleciona o canal do eixo Y
        uint16_t y = adc_read();

        // Controla o LED RGB com base nos valores do joystick
        controlar_led_rgb(x, y);

        // Verifica se o botão do joystick foi pressionado
        if (gpio_get(JOYSTICK_SW_PIN) == 0) {
            gpio_put(LED_RED_PIN, 0);
            gpio_put(LED_GREEN_PIN, 0);
            gpio_put(LED_BLUE_PIN, 0); // Desliga o LED
        }

        sleep_ms(100); // Pequena pausa
    }

    return 0;
}