#include "controle_buzzer.h"
#include "hardware/pwm.h"

void iniciar_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_enabled(slice_num, true);
}

void tocar_buzzer(uint pin, uint freq_hz) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    uint clock_div = 4; // Divisor de clock para ajustar a frequência
    uint top = 125000000 / (clock_div * freq_hz);
    pwm_set_wrap(slice_num, top);
    pwm_set_gpio_level(pin, top / 2); // 50% de duty cycle
}

void parar_buzzer(uint pin) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_gpio_level(pin, 0);
}

void som_sucesso(uint pin) {
    tocar_buzzer(pin, 1000); // Frequência de 1000 Hz
    sleep_ms(500);          // Duração de 500 ms
    parar_buzzer(pin);
}
void som_erro(uint pin) {
    tocar_buzzer(pin, 500); // Frequência de 500 Hz
    sleep_ms(500);         // Duração de 500 ms
    parar_buzzer(pin);
}
void som_alarme(uint pin) {
    tocar_buzzer(pin, 2000); // Frequência de 2000 Hz
    sleep_ms(1000);         // Duração de 1000 ms
    parar_buzzer(pin);
}
void som_alarme_continuo(uint pin) {
    tocar_buzzer(pin, 2000); // Frequência de 2000 Hz
    sleep_ms(100);          // Duração de 100 ms
}