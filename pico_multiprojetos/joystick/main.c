#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "setup_utils.h"  //Biblioteca de configuração do sistema e periféricos.
#include "matriz_utils.h" // Biblioteca de utilitários para matriz de LEDs

// Definições de pinos
#define LED_RED_PIN 13
#define LED_GREEN_PIN 11
#define LED_BLUE_PIN 12
#define JOYSTICK_X_PIN 27
#define JOYSTICK_Y_PIN 26
#define JOYSTICK_SW_PIN 22

// Função para inicializar o LED RGB
void iniciar_led_rgb()
{
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
}

// Função para inicializar o joystick
void iniciar_joystick()
{
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
    gpio_init(JOYSTICK_SW_PIN);
    gpio_set_dir(JOYSTICK_SW_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_SW_PIN);
}


// Função para obter a posição do eixo X
uint16_t posicao_x()
{
    adc_select_input(1); // Seleciona o canal do eixo X
    return adc_read();   // Retorna o valor lido
}

// Função para obter a posição do eixo Y
uint16_t posicao_y()
{
    adc_select_input(0); // Seleciona o canal do eixo Y
    return adc_read();   // Retorna o valor lido
}

// Função para controlar o LED RGB
void controlar_led_rgb(uint16_t x, uint16_t y)
{
    // Define os limites para as direções
    if (x < 500)
    {
        gpio_put(LED_RED_PIN, 1); // Vermelho
        gpio_put(LED_GREEN_PIN, 0);
        gpio_put(LED_BLUE_PIN, 0);
    }
    else if (x > 2500)
    {
        gpio_put(LED_RED_PIN, 0);
        gpio_put(LED_GREEN_PIN, 1); // Verde
        gpio_put(LED_BLUE_PIN, 0);
    }
    else if (y < 500)
    {
        gpio_put(LED_RED_PIN, 0);
        gpio_put(LED_GREEN_PIN, 0);
        gpio_put(LED_BLUE_PIN, 1); // Azul
    }
    else
    {
        gpio_put(LED_RED_PIN, 1);
        gpio_put(LED_GREEN_PIN, 1);
        gpio_put(LED_BLUE_PIN, 1); // Desliga
    }
}

// Função principal para demonstrar o uso das funções criadas
int main()
{
    stdio_init_all();
    iniciar_led_rgb();  // Inicializa o LED RGB
    iniciar_joystick(); // Inicializa o joystick
    iniciar_botao(BUTTON_A_PIN); // Inicializa o botão A
    iniciar_botao(BUTTON_B_PIN); // Inicializa o botão B
    npInit(LED_PIN);
    npClear();

    while (true)
    {
        // Lê a posição do joystick
        uint16_t x = posicao_x(); // Lê o valor do eixo X
        uint16_t y = posicao_y(); // Lê o valor do eixo Y

        // Display the joystick position something like this:
        // X: [            o             ]  Y: [              o         ]
        const uint bar_width = 40;          // Largura da barra de progresso
        const uint adc_max = (1 << 12) - 1; // é equivalente a escrever const uint adc_max = 4095;
        uint bar_x_pos = posicao_x() * bar_width / adc_max;
        uint bar_y_pos = posicao_y() * bar_width / adc_max;

        /*printf("\rX: |");

        for (uint i = 0; i < bar_width; ++i) // Desenha a barra de progresso para o eixo X
            putchar(i == bar_x_pos ? 'o' : ' '); // Coloca um 'o' na posição correspondente ao valor do eixo X

        printf("|\n");*/
        // printf("|  Y: |");

        /*for (uint i = 0; i < bar_width; ++i)
            putchar(i == bar_y_pos ? 'o' : ' ');

        printf("|\n");*/
        // sleep_ms(50); // Pausa para evitar leituras rápidas

        // Controla o buzzer com base na posição do joystick
        /*if (x < 1000)
        {

            npSetColor(LED_PIN, 255, 0, 0); // Define a cor de todos os LEDs como vermelho.
        }
        else if (x > 3000)
        {
            npSetColor(LED_PIN, 0, 255, 0); // Define a cor de todos os LEDs como verde.
        }
        else
        {
            npSetColor(LED_PIN, 0, 0, 0); // Desliga os LEDs
        }*/
        contourMatrix(100, 0, 200, 150); // Contorna a matriz com LEDs azuis
        /*if (gpio_get(BUTTON_A_PIN) == 0) // Verifica se o botão A foi pressionado
        {
            
        }
        else if (gpio_get(BUTTON_B_PIN) == 0) // Verifica se o botão B foi pressionado
        {
            npSetColor(LED_PIN, 0, 0, 0); // Desliga os LEDs
        }*/
        //sleep_ms(10); // Pausa para evitar leituras rápidas
    }

    return 0;
}