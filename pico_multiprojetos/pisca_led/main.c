#include "pico/stdlib.h"
#include "actions_io.h"
#include "setup_utils.h"

int main() {
    iniciar_led(BLUE_LED_PIN);
    while (true) {
        piscar_led(BLUE_LED_PIN, 500);
    }
}