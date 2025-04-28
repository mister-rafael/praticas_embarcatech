#include <stdio.h>
#include <string.h>
#include "setup_utils.h"
#include "actions_io.h"      // Biblioteca de ações de entrada e saída.
#include "controle_acesso.h" // Biblioteca de controle de acesso.
#include "ssd1306.h"         //Biblioteca específica para controlar o display OLED SSD1306.

// Definições de variáveis
const int senha_correta[4] = {1, 2, 3, 4}; // Senha correta para acesso
int tentativas_incorretas = 0;             // Contador de tentativas incorretas
int senha_digitada[4] = {0, 0, 0, 0};      // Senha digitada pelo usuário
int posicao_atual = 0;                     // Posição atual da senha
bool senhaInserida = false;                // Flag para verificar se a senha foi inserida

// Função de configuração do programa
void setup_program()
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

    // Inicialização do buzzer
    iniciar_buzzer(BUZZER_PIN_1); // Inicializa o buzzer 1
    iniciar_buzzer(BUZZER_PIN_2); // Inicializa o buzzer 2

    // Inicialmente, desligar o LED RGB
    desligar_led(RED_LED_PIN);   // Desliga o LED vermelho
    desligar_led(GREEN_LED_PIN); // Desliga o LED verde
    desligar_led(BLUE_LED_PIN);  // Desliga o LED azul

    // exibir_messagem(ssd,&frame_area); // Exibir tela inicial para digitação da senha
    ligar_led(BLUE_LED_PIN); // Liga o LED azul para indicar que o sistema está pronto
}
// Função para limpar o display
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
// Função som sucesso
void som_sucesso(uint pin)
{
    // Primeiro beep curto
    beep(pin, 200); // Beep de 200 ms
    sleep_ms(100);  // Pausa de 100 ms

    // Segundo beep curto
    beep(pin, 200); // Beep de 200 ms
    sleep_ms(100);  // Pausa de 100 ms

    // Beep longo
    beep(pin, 500); // Beep de 500 ms
}
// Função som alarme
void som_alarme(uint pin){
    for (int i = 0; i < 10; i++)
    {                   // 10 beeps rápidos
        beep(pin, 100); // Beep de 100 ms
        sleep_ms(100);  // Pausa de 100 ms
    }
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
        beep(BUZZER_PIN_1, 80);                                                   // Emite um beep curto de 100 ms
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
        // Se a senha estiver correta, liga o LED verde e toca o som de sucesso
        gpio_put(BLUE_LED_PIN, 0);  // Desliga o LED azul
        gpio_put(GREEN_LED_PIN, 1); // Liga o LED verde
        gpio_put(RED_LED_PIN, 0);   // Desliga o LED vermelho
        ssd1306_draw_string(buffer, 0, 32, "Acesso Permitido");
        render_on_display(buffer, area); // Atualiza o display OLED com os dados do buffer.
        som_sucesso(BUZZER_PIN_1);       // Toca o som de sucesso

        tentativas_incorretas = 0; // Reseta o contador de tentativas incorretas
    }
    else{
        gpio_put(BLUE_LED_PIN, 0);  // Desliga o LED azul
        gpio_put(GREEN_LED_PIN, 0); // Desliga o LED verde
        gpio_put(RED_LED_PIN, 1);   // Liga o LED vermelho
        beep(BUZZER_PIN_1, 500);    // Toca o som de erro
        ssd1306_draw_string(buffer, 0, 32, "Acesso negado");
        tentativas_incorretas++;         // Incrementa o contador de tentativas incorretas

        // Se o número de tentativas incorretas for maior ou igual a 2, toca o alarme
        // e reseta o contador de tentativas incorretas
        if (tentativas_incorretas >= 2)
        {
            sleep_ms(500); // Pausa de 500 ms
            limpar_display(buffer, area);
            som_alarme(BUZZER_PIN_1);     // Toca o alarme contínuo            
            ssd1306_draw_string(buffer, 0, 32, " Nº Tentativas  ");
            ssd1306_draw_string(buffer, 0, 48, "    excedidas   ");
            tentativas_incorretas = 0;       // Reseta o contador de tentativas incorretas
        }
    }
    render_on_display(buffer, area); // Atualiza o display OLED com os dados do buffer.
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