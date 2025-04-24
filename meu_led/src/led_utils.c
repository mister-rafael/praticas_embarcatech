#include "pico/stdlib.h"
#include "led_utils.h"

void pisca_led(int pin, int tempo_ms) {
    gpio_put(pin, 1);
    sleep_ms(tempo_ms);
    gpio_put(pin, 0);
    sleep_ms(tempo_ms);
}
