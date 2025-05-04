// Inclusão das bibliotecas necessárias
#include <stdio.h>            //Entrada e saída padrão.
#include <string.h>           //Manipulação de strings.
#include <stdlib.h>           //Biblioteca do Raspberry Pi Pico.
#include <ctype.h>            //Funções para manipulação de caracteres.
#include "pico/binary_info.h" //Informações binárias para depuração.
#include "pico/stdlib.h"      //Funções de alocação de memória e conversão.
#include "hardware/i2c.h"     //Biblioteca para comunicação I2C.
#include "ssd1306.h"          //Biblioteca específica para controlar o display OLED SSD1306.

#include "setup_utils.h" //Biblioteca de configuração do sistema e periféricos.
#include "actions_io.h"  //Biblioteca de ações de entrada e saída.

int main()
{
    stdio_init_all();
    iniciar_led(RED_LED_PIN);    // Inicializa o LED vermelho
    iniciar_led(GREEN_LED_PIN);  // Inicializa o LED verde
    iniciar_botao(BUTTON_A_PIN); // Inicializa o botão A
    iniciar_botao(BUTTON_B_PIN); // Inicializa o botão B

    while (true)
    {
        int opcao = -1;                       // opcao inicial: display apagado
        if (gpio_get(BUTTON_A_PIN) == 0) // Verifica se o botão A foi pressionado
        {
            opcao = 0; // Se o botão A foi pressionado, define a opção como 0
        }
        else if (gpio_get(BUTTON_B_PIN) == 0) // Verifica se o botão B foi pressionado
        {
            opcao = 1; // Se o botão B foi pressionado, define a opção como 1
        }

        switch (opcao) // Verifica a opção selecionada
        {
        case 0: // Se a opção for 0, exibe a mensagem inicial
            // exibir_messagem(ssd, &frame_area, "Caso 0"); // Exibe a mensagem inicial para digitar a senha
            // desligar_led(RED_LED_PIN); // Desliga o LED vermelho
            ligar_led(GREEN_LED_PIN);    // Liga o LED verde para indicar que o sistema está pronto
            sleep_ms(5000);              // Aguarda 1 segundo
            desligar_led(GREEN_LED_PIN); // Desliga o LED verde
            break;
        case 1: // Se a opção for 1, exibe a mensagem de erro
            // Exibe a mensagem de erro
            // desligar_led(GREEN_LED_PIN); // Desliga o LED verde
            ligar_led(RED_LED_PIN);    // Liga o LED vermelho para indicar erro
            sleep_ms(5000);            // Aguarda 1 segundo
            desligar_led(RED_LED_PIN); // Desliga o LED vermelho
            break;
        default:
            break;
        }
        sleep_ms(100); // Aguarda 100 ms antes de verificar novamente
    }
}