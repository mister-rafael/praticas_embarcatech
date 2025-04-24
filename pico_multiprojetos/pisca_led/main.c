#include "pico/stdlib.h"
#include "io_utils.h"

int main() {
    const uint LED_PIN = 25;
    iniciar_led(LED_PIN);
    while (true) {
        piscar_led(LED_PIN, 500);
    }
}