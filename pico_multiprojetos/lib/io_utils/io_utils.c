#include "io_utils.h"
#include "pico/stdlib.h"

void iniciar_led(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
}

void piscar_led(uint pin, int delay_ms) {
    gpio_put(pin, 1);
    sleep_ms(delay_ms);
    gpio_put(pin, 0);
    sleep_ms(delay_ms);
}