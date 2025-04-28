#include "actions_io.h"
#include "pico/stdlib.h" // Biblioteca padrão do Raspberry Pi Pico
#include "hardware/pwm.h" // Biblioteca para controle de PWM
#include "hardware/clocks.h" // Biblioteca para controle de clocks

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

// Função para tocar o buzzer
void tocar_buzzer(uint pin, uint freq_hz) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint clock_div = 4; // Divisor de clock para ajustar a frequência
    uint top = 125000000 / (clock_div * freq_hz); // Cálculo do valor máximo do contador
    pwm_set_wrap(slice_num, top); // Define o valor máximo do contador
    pwm_set_gpio_level(pin, top / 2); // 50% de duty cycle
}
void beep(uint pin, uint duration_ms) {
    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o duty cycle para 50% (ativo)
    pwm_set_gpio_level(pin, 2048);

    // Temporização
    sleep_ms(duration_ms);

    // Desativar o sinal PWM (duty cycle 0)
    pwm_set_gpio_level(pin, 0);

    // Pausa entre os beeps
    sleep_ms(100); // Pausa de 100ms
}
// Função para parar o buzzer
void parar_buzzer(uint pin) {
    uint slice_num = pwm_gpio_to_slice_num(pin); // Obtém o número do slice associado ao pino
    pwm_set_gpio_level(pin, 0); // Para o som definindo o nível do pino como 0
}