// Inclusão das bibliotecas necessárias
#include <stdio.h>            //Entrada e saída padrão.
#include <string.h>           //Manipulação de strings.
#include <stdlib.h>           //Biblioteca do Raspberry Pi Pico.
#include <ctype.h>            //Funções para manipulação de caracteres.
#include "pico/binary_info.h" //Informações binárias para depuração.
#include "pico/stdlib.h"      //Funções de alocação de memória e conversão.
#include "hardware/i2c.h"     //Biblioteca para comunicação I2C.
#include "ssd1306.h"          //Biblioteca específica para controlar o display OLED SSD1306.

#include "setup_utils.h"             //Biblioteca de configuração do sistema e periféricos.
#include "actions_io.h"              //Biblioteca de ações de entrada e saída.
#include "funcoes/controle_acesso.h" //Biblioteca de controle de acesso.

// Função principal do programa
// Esta função é chamada quando o programa é iniciado
int main()
{
    setup_program(); // Primeiro, chama a função que inicializa os pinos.

    // Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area); // calcula o tamanho do buffer necessário para armazenar os dados da área de renderização.
    uint8_t ssd[ssd1306_buffer_length];               // É um buffer que armazena a imagem/tela antes de ser enviada para o OLED.

    // zera o display inteiro
    //limpar_display(ssd, &frame_area); // Chama a função para limpar a tela

restart:            // Rótulo para reiniciar o loop
    int opcao = 0; // opcao inicial: display apagado
    while (true)
    {
        switch (opcao)
        {
        case 0: // Display apagado, aguardando interação
            // Desliga os LEDs e o display
            limpar_display(ssd, &frame_area); // Chama a função para limpar a tela
            if (gpio_get(BUTTON_A_PIN) == 0)
            {
                sleep_ms(300); // Debounce
                opcao = 1;    // Ir para a tela de inserir senha
                break; // Sai do switch para evitar que o opcao mude novamente
            }
        case 1: // Display ligado, aguardando interação
            // Parte do código para exibir a mensagem no display (opcional: mudar ssd1306_height para 32 em ssd1306_i2c.h)
            exibir_messagem(ssd, &frame_area, "Digite a senha"); // Exibir tela inicial para digitação da senha
            // Função para coletar a senha digitada pelo usuário
            coletar_senha(ssd, &frame_area); // Coletar a senha digitada pelo usuário
            //Loop para lidar com a entrada do usuário
            loop(ssd, &frame_area);
            // Aguarda 10ms para evitar sobrecarga do processador
            sleep_ms(10);
            opcao = 0; // Retorna ao opcao inicial
            break;

        case 2: // Mensagem de deslogado
            if (gpio_get(BUTTON_B_PIN) == 0 && opcao == 1)
            {
                sleep_ms(300); // Debounce
                opcao = 2;    // Ir para a tela de inserir senha
            }
            else if (gpio_get(BUTTON_B_PIN) == 0)
            {
                sleep_ms(300); // Debounce
                opcao = 0;    // Exibir mensagem de deslogado
            }
            break;
        default: // Garante que o opcao volte ao inicial
            printf("Opção inválida.\n");
        }
    }

    return 0; // Retorna 0 para indicar que o programa terminou corretamente
}
