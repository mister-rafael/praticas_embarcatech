#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "prog_ppa.h"

// Pinos
#define UART_ID uart0
#define BAUD_RATE 9600
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define BTN_A 5
#define BTN_B 6
#define LED_PIN 25

void enviar_comando_uart(const char *comando) {
    uart_puts(UART_ID, comando);
    uart_puts(UART_ID, "\r\n");
}

void iniciar_uart(void) {
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
}

void atualizar_display(const char *linha1, const char *linha2) {
    printf("OLED: %s | %s\n", linha1, linha2);
}

void checar_botoes(void) {
    static bool estado_a = true, estado_b = true;
    bool novo_a = gpio_get(BTN_A);
    bool novo_b = gpio_get(BTN_B);

    if (!novo_a && estado_a) {
        enviar_comando_uart("VEL+1");
        atualizar_display("Velocidade +", "");
    }

    if (!novo_b && estado_b) {
        enviar_comando_uart("VEL-1");
        atualizar_display("Velocidade -", "");
    }

    estado_a = novo_a;
    estado_b = novo_b;
}

int main() {
    stdio_init_all();
    iniciar_uart();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);

    gpio_init(BTN_A);
    gpio_set_dir(BTN_A, GPIO_IN);
    gpio_pull_up(BTN_A);

    gpio_init(BTN_B);
    gpio_set_dir(BTN_B, GPIO_IN);
    gpio_pull_up(BTN_B);

    atualizar_display("Prog PPA", "Pronto");

    while (true) {
        checar_botoes();
        sleep_ms(200);
    }

    return 0;
}