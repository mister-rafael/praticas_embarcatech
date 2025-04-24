#include <stdio.h>
#include "pico/stdlib.h"
#include "led_utils.h"
// Configuração dos pinos do LED RGB
const uint BLUE_LED_PIN = 12;  // LED azul no GPIO 12
const uint RED_LED_PIN = 13;   // LED vermelho no GPIO 13
const uint GREEN_LED_PIN = 11; // LED verde no GPIO 11

int main() {
    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);

    while (true) {
        pisca_led(BLUE_LED_PIN,5000);
    }
}

