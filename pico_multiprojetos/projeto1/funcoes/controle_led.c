#include "controle_led.h"

void iniciar_led(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
}

void ligar_led(uint pin) {
    gpio_put(pin, 1);
}

void desligar_led(uint pin) {
    gpio_put(pin, 0);
}