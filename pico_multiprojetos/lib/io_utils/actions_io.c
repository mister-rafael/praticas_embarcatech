#include "actions_io.h"
#include "pico/stdlib.h" // Biblioteca padrão do Raspberry Pi Pico

// Funções para LIGAR o LED RGB
void ligar_led(uint pin) {
    gpio_put(pin, 1);
}
// Função para desligar o LED RGB
void desligar_led(uint pin) {
    gpio_put(pin, 0);
}
// Função para piscar o LED RGB
void piscar_led(uint pin, int delay_ms) {
    gpio_put(pin, 1);
    sleep_ms(delay_ms);
    gpio_put(pin, 0);
    sleep_ms(delay_ms);
}
