// inclue a biblioteca matriz_utils.h
#include "matriz_utils.h"    // Biblioteca de utilitários para matriz de LEDs
#include <stdio.h>           // Biblioteca padrão de entrada e saída
#include "pico/stdlib.h"     // Biblioteca padrão do Raspberry Pi Pico
#include "hardware/pio.h"    // Biblioteca para controle de PIO
#include "hardware/clocks.h" // Biblioteca para controle de clocks

// Biblioteca gerada pelo arquivo .pio durante compilação.
#include "ws2818b.pio.h"

// Definição de pixel GRB
struct pixel_t
{
    uint8_t G, R, B; // Três valores de 8-bits compõem um pixel.
};
typedef struct pixel_t pixel_t; // Troca o nome de "struct pixel_t" para "pixel_t" por clareza.
typedef pixel_t npLED_t;        // Mudança de nome de "pixel_t" para "npLED_t" por clareza.

// Declaração do buffer de pixels que formam a matriz.
npLED_t leds[LED_COUNT]; // Buffer de pixels, cada pixel é um struct com três componentes (G, R, B).

// Variáveis para uso da máquina PIO.
PIO np_pio;
uint sm;

/**
 * Inicializa a máquina PIO para controle da matriz de LEDs.
 */
void npInit(uint pin)
{
    // Cria programa PIO.
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;

    // Toma posse de uma máquina PIO.
    sm = pio_claim_unused_sm(np_pio, false);
    if (sm < 0)
    {
        np_pio = pio1;
        sm = pio_claim_unused_sm(np_pio, true); // Se nenhuma máquina estiver livre, panic!
    }

    // Inicia programa na máquina PIO obtida.
    ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);

    // Limpa buffer de pixels.
    for (uint i = 0; i < LED_COUNT; ++i)
    {
        leds[i].R = 0;
        leds[i].G = 0;
        leds[i].B = 0;
    }
}
/**
 * Atribui uma cor RGB a um LED.
 */
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b)
{
    leds[index].R = r;
    leds[index].G = g;
    leds[index].B = b;
}
/**
 * Limpa o buffer de pixels.
 */
void npClear()
{
    for (uint i = 0; i < LED_COUNT; ++i)
        npSetLED(i, 0, 0, 0);
}
/**
 * Escreve os dados do buffer nos LEDs.
 */
void npWrite()
{
    // Escreve cada dado de 8-bits dos pixels em sequência no buffer da máquina PIO.
    for (uint i = 0; i < LED_COUNT; ++i)
    {
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].B);
    }
    sleep_us(100); // Espera 100us, sinal de RESET do datasheet.
}
// Função para ligar os LEDs da matriz de LEDs em uma cor específica.
void npSetColor(uint pin, uint8_t r, uint8_t g, uint8_t b)
{
    for (uint i = 0; i < LED_COUNT; ++i)
    {
        npSetLED(i, r, g, b); // Define a cor do LED
    }
    npWrite(); // Atualiza a matriz de LEDs
}
// Função para criar um efeito de fade entre os LEDs
void rgbFadeEffect(uint delay_ms)
{
    for (uint i = 0; i < LED_COUNT - 1; ++i)
    {
        for (uint8_t step = 0; step <= 255; ++step)
        {
            // Calcula a intensidade do LED atual e do próximo
            uint8_t current_intensity = 255 - step;
            uint8_t next_intensity = step;

            // Define a cor do LED atual e do próximo
            npSetLED(i, current_intensity, 0, 0);  // LED atual em vermelho
            npSetLED(i + 1, next_intensity, 0, 0); // Próximo LED em vermelho

            // Atualiza os LEDs
            npWrite();
            sleep_ms(delay_ms);
        }

        // Garante que o LED anterior seja apagado ao final da transição
        npSetLED(i, 0, 0, 0);
    }

    // Apaga o último LED ao final do efeito
    npSetLED(LED_COUNT - 1, 0, 0, 0);
    npWrite();
}
// Função para ligar um LED de cada vez em ordem crescente
void lightUpOneByOne(uint delay_ms, uint8_t r, uint8_t g, uint8_t b)
{
    for (uint i = 0; i < LED_COUNT; ++i)
    {
        // Limpa todos os LEDs
        // npClear();

        // Liga apenas o LED atual com a cor especificada
        npSetLED(i, r, g, b);

        // Atualiza os LEDs
        npWrite();

        // Aguarda o tempo especificado antes de passar para o próximo LED
        sleep_ms(delay_ms);
    }

    // Apaga todos os LEDs ao final
    npClear();
    npWrite();
}
// Função para simular o efeito de LED da Alexa em uma matriz de LEDs
void alexaEffect(uint delay_ms)
{
    const uint8_t max_brightness = 200; // Brilho máximo
    const uint8_t fade_steps = 10;      // Número de passos para o gradiente
    const uint num_leds = LED_COUNT;    // Número total de LEDs na matriz

    for (uint led_atual = 0; led_atual < num_leds; ++led_atual)
    {
        // Limpa todos os LEDs
        npClear();

        // Define o LED atual com brilho máximo
        npSetLED(led_atual, 0, max_brightness, max_brightness); // Cor roxa (mistura de vermelho e azul)

        // Cria o gradiente de brilho para os LEDs seguintes
        for (uint step = 1; step <= fade_steps; ++step)
        {
            uint next_led = (led_atual + step) % num_leds; // Calcula o próximo LED (efeito circular)
            uint8_t brightness = max_brightness - (max_brightness / fade_steps) * step;

            // Define o próximo LED com brilho reduzido
            npSetLED(next_led, 0, brightness, brightness); // Gradiente de roxo
        }
        // Atualiza os LEDs
        npWrite();

        // Aguarda antes de mover para o próximo LED
        sleep_ms(delay_ms);
    }
}
void contourMatrix(uint delay_ms, uint8_t r, uint8_t g, uint8_t b)
{
    // Ordem específica dos LEDs para contornar a matriz
    const uint led_order[] = {0, 1, 2, 3, 4, 5, 14, 15, 24, 23, 22, 21, 20, 19, 10, 9, 0}; // Ordem dos LEDs para contornar a matriz
    const uint num_leds = sizeof(led_order) / sizeof(led_order[0]); // Número de LEDs na ordem

    for (uint i = 0; i < num_leds; ++i)
    {
        // Limpa todos os LEDs
        npClear();

        // Liga o LED atual na ordem especificada
        npSetLED(led_order[i], r, g, b);

        // Atualiza os LEDs
        npWrite();

        // Aguarda o tempo especificado antes de passar para o próximo LED
        sleep_ms(delay_ms);
    }

    // Apaga todos os LEDs ao final
    //npClear();
    //npWrite();
}