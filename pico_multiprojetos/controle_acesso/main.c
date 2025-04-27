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

/*void setup_program()
{
    stdio_init_all(); // Inicializa os tipos stdio padrão presentes ligados ao binário

    // Inicialização do i2c usando a função da biblioteca setup_utils
    iniciar_i2c(i2c1, PIN_SDA, PIN_SCL, ssd1306_i2c_clock);
    // Inicialização do display OLED SSD1306
    ssd1306_init(); // inicializa o display com o endereço I2C do display, que geralmente é 0x3C ou 0x3D.

    // Inicialização dos botões A e B.
    iniciar_botao(BUTTON_A_PIN); // Inicializa o botão A
    iniciar_botao(BUTTON_B_PIN); // Inicializa o botão B

    // Inicialização dos LeD RGB
    iniciar_led(RED_LED_PIN);   // Inicializa o LED vermelho
    iniciar_led(GREEN_LED_PIN); // Inicializa o LED verde
    iniciar_led(BLUE_LED_PIN);  // Inicializa o LED azul

    // Inicialmente, desligar o LED RGB
    desligar_led(RED_LED_PIN);   // Desliga o LED vermelho
    desligar_led(GREEN_LED_PIN); // Desliga o LED verde
    desligar_led(BLUE_LED_PIN);  // Desliga o LED azul

    // exibir_messagem(ssd,&frame_area); // Exibir tela inicial para digitação da senha
    ligar_led(BLUE_LED_PIN); // Liga o LED azul para indicar que o sistema está pronto
}*/

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

    // Parte do código para exibir a mensagem no display (opcional: mudar ssd1306_height para 32 em ssd1306_i2c.h)
    exibir_messagem(ssd, &frame_area); // Exibir tela inicial para digitação da senha
    while (true)
    {
        // Chama a função loop para lidar com a entrada do usuário e verificar a senha
        loop(ssd, &frame_area);
        // Aguarda 10ms para evitar sobrecarga do processador
        sleep_ms(10);
    }

    return 0; // Retorna 0 para indicar que o programa terminou corretamente
}
