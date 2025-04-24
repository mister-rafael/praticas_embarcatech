// Inclusão das bibliotecas necessárias
#include <stdio.h>            //Entrada e saída padrão.
#include <string.h>           //Manipulação de strings.
#include <stdlib.h>           //Biblioteca do Raspberry Pi Pico.
#include <ctype.h>            //Funções para manipulação de caracteres.
#include "pico/binary_info.h" //Informações binárias para depuração.
#include "pico/stdlib.h"      //Funções de alocação de memória e conversão.
#include "hardware/i2c.h"     //Biblioteca para comunicação I2C.
#include "inc/ssd1306.h"      //Biblioteca específica para controlar o display OLED SSD1306.

// Configuração dos pinos do Display Oled
const uint PIN_SDA = 14; // Define o pino 14 como dados
const uint PIN_SCL = 15; // Define o pino 15 como clock

// Configuração dos pinos dos botões como entrada com pull-up
const uint BUTTON_A_PIN = 5; // Botão A no GPIO 5
const uint BUTTON_B_PIN = 6; // Botão B no GPIO 6

// Configuração dos pinos do LED RGB
const uint BLUE_LED_PIN = 12;  // LED azul no GPIO 12
const uint RED_LED_PIN = 13;   // LED vermelho no GPIO 13
const uint GREEN_LED_PIN = 11; // LED verde no GPIO 11

// Variaveis GLOBAIS
// Senha correta definida no código
const int senha_correta[4] = {1, 2, 3, 4};
int senha_digitada[4] = {0, 0, 0, 0}; // Senha digitada pelo usuário
int posicao_atual = 0;                // Posição atual da senha
bool senhaInserida = false;           // Flag para verificar se a senha foi inserida
// Declaração dos protótipos das funções antes do setup()
void digitar_senha(uint8_t *buffer, struct render_area *area);
void verificar_senha(uint8_t *buffer, struct render_area *area);
void resetar_entrada(uint8_t *buffer, struct render_area *area);
void coletar_senha(uint8_t *buffer, struct render_area *area);

void setup()
{
    stdio_init_all(); // Inicializa os tipos stdio padrão presentes ligados ao binário

    // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);  // inicializa a interface I2C no Raspberry Pi Pico.
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C); // define o pino GPIO 14 como DADOS.
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C); // define o pino GPIO 14 como RELÓGIO (CLOCK).
    gpio_pull_up(PIN_SDA);                     // define um resistor de Pull_up para o pino de dados.
    gpio_pull_up(PIN_SCL);                     // define um resistor de Pull_up para o pino de clock.

    // Inicialização dos botões
    gpio_init(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_pull_up(BUTTON_B_PIN);

    // Inicialização dos LEDs
    gpio_init(RED_LED_PIN);
    gpio_init(GREEN_LED_PIN);
    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);

    // Inicialmente, desligar o LED RGB
    gpio_put(RED_LED_PIN, 0);
    gpio_put(GREEN_LED_PIN, 0);
    gpio_put(BLUE_LED_PIN, 0);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init(); // inicializa o display com o endereço I2C do display, que geralmente é 0x3C ou 0x3D.
    // exibir_messagem(ssd,&frame_area); // Exibir tela inicial para digitação da senha
    gpio_put(BLUE_LED_PIN, 1); // Liga o LED azul
}
// Limpar todo o display.
void limpar_display(uint8_t *buffer, struct render_area *area)
{
    memset(buffer, 0, ssd1306_buffer_length); // Garante que o buffer esteja limpo, deixando a tela preta.
    render_on_display(buffer, area);          // Atualiza o display OLED com os dados do buffer.
}
// Exibir mensagem inicial no display
void exibir_messagem(uint8_t *buffer, struct render_area *area)
{
    limpar_display(buffer, area); // Limpa todo o display, antes de exibir.
    ssd1306_draw_string(buffer, 10, 10, "Digite a senha:");
    render_on_display(buffer, area);
    coletar_senha(buffer, area);
}
// Exibir os dígitos da senha e destacar o atual
void coletar_senha(uint8_t *buffer, struct render_area *area)
{
    ssd1306_draw_string(buffer, 30, 40, "");
    for (int i = 0; i < 4; i++)
    {
        char digit[4];
        if (i == posicao_atual)
        {
            sprintf(digit, "[%d] ", senha_digitada[i]);
        }
        else
        {
            sprintf(digit, "%d ", senha_digitada[i]);
        }
        ssd1306_draw_string(buffer, 30 + (i * 16), 40, digit);
    }
    render_on_display(buffer, area);
}
// Função para capturar entrada do usuário
void digitar_senha(uint8_t *buffer, struct render_area *area)
{
    if (gpio_get(BUTTON_A_PIN) == 0)
    {
        senha_digitada[posicao_atual] = (senha_digitada[posicao_atual] + 1) % 10; // Incrementar o número
        coletar_senha(buffer, area);                                              // Atualizar a exibição
        sleep_ms(300);
    }
    if (gpio_get(BUTTON_B_PIN) == 0)
    {
        if (posicao_atual < 3)
        {
            posicao_atual++; // Passar para o próximo dígito
        }
        else
        {
            senhaInserida = true; // Indicar que a senha foi inserida completamente
        }
        coletar_senha(buffer, area); // Atualizar a exibição
        sleep_ms(300);
    }
}
// Verifica se a senha inserida está correta
void verificar_senha(uint8_t *buffer, struct render_area *area)
{
    bool correct = true;
    for (int i = 0; i < 4; i++)
    {
        if (senha_digitada[i] != senha_correta[i])
        {
            correct = false;
            break;
        }
    }
    limpar_display(buffer, area);
    if (correct)
    {
        ssd1306_draw_string(buffer, 0, 48, "Acesso Permitido");
        gpio_put(BLUE_LED_PIN, 0);  // Desliga o LED azul
        gpio_put(GREEN_LED_PIN, 1); // Liga o LED verde
        gpio_put(RED_LED_PIN, 0);   // Desliga o LED vermelho
    }
    else
    {
        ssd1306_draw_string(buffer, 0, 48, "Acesso negado");
        gpio_put(BLUE_LED_PIN, 0);  // Desliga o LED azul
        gpio_put(GREEN_LED_PIN, 0); // Desliga o LED verde
        gpio_put(RED_LED_PIN, 1);   // Liga o LED vermelho
    }
    render_on_display(buffer, area);
}
// Reinicia a entrada da senha para uma nova tentativa
void resetar_entrada(uint8_t *buffer, struct render_area *area)
{
    gpio_put(RED_LED_PIN, 0);   // Desliga o LED vermelho
    gpio_put(GREEN_LED_PIN, 0); // Desliga o LED verde
    gpio_put(BLUE_LED_PIN, 1);  // Desliga o LED azul
    for (int i = 0; i < 4; i++)
    {
        senha_digitada[i] = 0;
    }
    posicao_atual = 0;
    senhaInserida = false;
    exibir_messagem(buffer, area);
}
// Loop principal
void loop(uint8_t *buffer, struct render_area *area)
{
    if (!senhaInserida)
    {
        digitar_senha(buffer, area); // Lidar com a entrada do usuário
    }
    else
    {
        verificar_senha(buffer, area); // Verificar se a senha está correta
        sleep_ms(2000);
        resetar_entrada(buffer, area); // Reiniciar o processo de digitação
    }
}

int main()
{
    setup(); // Primeiro, chama a função que inicializa os pinos.

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

restart:

    // Parte do código para exibir a mensagem no display (opcional: mudar ssd1306_height para 32 em ssd1306_i2c.h)
    exibir_messagem(ssd, &frame_area); // Exibir tela inicial para digitação da senha
    while (true)
    {

        // Chama a função LOOP
        loop(ssd, &frame_area);
        sleep_ms(10);
    }

    return 0;
}
