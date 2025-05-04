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
    limpar_display(ssd, &frame_area); // Chama a função para limpar a tela

restart: // Rótulo para reiniciar o loop
    // Exibe a mensagem inicial para digitar a senha
    exibir_messagem(ssd, &frame_area, "Digite a senha"); // Exibe a mensagem inicial para digitar a senha
    coletar_senha(ssd, &frame_area);                     // Chama a função para coletar a senha
    while (true)
    {
        loop(ssd, &frame_area);
        sleep_ms(10);
    }

    //============================================================================================================================
    // PARTE EM DESENVOLVIMENTO
    /*while (true)
    {
        int opcao = -1;                  // opcao inicial: display apagado
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
        case 0:                                                  // Se a opção for 0, exibe a mensagem inicial
            exibir_messagem(ssd, &frame_area, "Digite a senha"); // Exibe a mensagem inicial para digitar a senha
            coletar_senha(ssd, &frame_area);
            loop(ssd, &frame_area);
            sleep_ms(10);
            break;
        case 1: // Se a opção for 1, exibe a mensagem de erro
            limpar_display(ssd, &frame_area); // Limpa o display
            break;
        default:
            break;
        }
        sleep_ms(100); // Aguarda 100 ms antes de verificar novamente
    }*/
    //============================================================================================================================
    return 0; // Retorna 0 para indicar que o programa terminou corretamente
}
