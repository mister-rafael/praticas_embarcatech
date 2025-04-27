#include "funcoes/controle_led.h"
#include "pico/stdlib.h"

int main() {
    // Inicializa o LED RGB
    iniciar_led(RED_LED_PIN);
    iniciar_led(GREEN_LED_PIN);
    iniciar_led(BLUE_LED_PIN);

    while (true) {
        // Alterna as cores do LED RGB
        ligar_led(RED_LED_PIN);
        sleep_ms(1000);
        desligar_led(RED_LED_PIN);

        ligar_led(GREEN_LED_PIN);
        sleep_ms(1000);
        desligar_led(GREEN_LED_PIN);

        ligar_led(BLUE_LED_PIN);
        sleep_ms(1000);
        desligar_led(BLUE_LED_PIN);
    }

    return 0;
}