#include <stdio.h>
#include "pico/stdlib.h"
#include "setup_utils.h" //Biblioteca de configuração do sistema e periféricos.
// #include "actions_io.h"  //Biblioteca de ações de entrada e saída.
#include "matriz_utils.h" // Biblioteca de utilitários para matriz de LEDs

int main()
{
    // Inicializa entradas e saídas.
    stdio_init_all();
    // Iniciar botões
    iniciar_botao(BUTTON_A_PIN); // Inicializa o botão A
    iniciar_botao(BUTTON_B_PIN); // Inicializa o botão B
    // Inicializa matriz de LEDs NeoPixel.
    npInit(LED_PIN);
    npClear();

    // Aqui, você desenha nos LEDs.
    // Função para desenhar todos os LEDs de uma cor específica.
    // npSetColor(LED_PIN, 255, 255, 255); // Define a cor de todos os LEDs como branco.

    //Loop infinito.
    while (true)
    {
        if (gpio_get(BUTTON_A_PIN) == 0) // Verifica se o botão A foi pressionado
        {
            //npClear();                      // Limpa o buffer de LEDs
            //npWrite();                      // Envia os dados para a matriz de LEDs
            npSetColor(LED_PIN, 255, 0, 0); // Define a cor de todos os LEDs como vermelho.
        }
        else if (gpio_get(BUTTON_B_PIN) == 0) // Verifica se o botão B foi pressionado
        {
            //npClear();                      // Limpa o buffer de LEDs
            //npWrite();                      // Envia os dados para a matriz de LEDs
            npSetColor(LED_PIN, 0, 255, 0); // Define a cor de todos os LEDs como verde.
        }
        else
        {
            npClear(); // Limpa o buffer de LEDs
            npWrite(); // Envia os dados para a matriz de LEDs
            // npSetColor(LED_PIN, 0, 0, 255); // Define a cor de todos os LEDs como azul.
        }
        sleep_ms(10); // Aguarda 1 segundo antes de verificar novamente os botões.
    }
}