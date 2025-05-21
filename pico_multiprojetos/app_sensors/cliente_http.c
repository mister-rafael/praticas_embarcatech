/**
 * Cliente HTTP para Raspberry Pi Pico W
 */
#include <stdio.h>
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"
#include "pico/async_context.h"
#include "lwip/altcp_tls.h"
#include "exemplo_http_client_util.h"
// INLUDES PARA USO DO JOYSTICK
#include "hardware/adc.h"
#include "setup_utils.h"             //Biblioteca de configuração do sistema e periféricos.
#include "actions_io.h"              //Biblioteca de ações de entrada e saída.

// ========== CONFIGURAÇÕES ======================== //
// #define HOST "192.168.0.20" // IP do servidor de casa
#define HOST "10.3.10.191" // IP do servidor do IFPI
#define PORT 5000          // Porta do servidor
#define INTERVALO_MS 500   // Intervalo de verificação do botão (em milissegundos)

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
// Envia os valores do joystick (x e y) para o servidor via requisição HTTP GET
void enviar_dados_joystick(uint16_t x, uint16_t y)
{
    char url[64];
    // Constrói a URL com query parameters para x e y
    snprintf(url, sizeof(url), "/joystick?x=%d&y=%d", x, y);

    EXAMPLE_HTTP_REQUEST_T req = {0}; // Inicializa a estrutura de requisição
    req.hostname = HOST;
    req.url = url;
    req.port = PORT;
    req.headers_fn = http_client_header_print_fn;
    req.recv_fn = http_client_receive_print_fn;

    printf("Enviando dados do joystick: X = %d, Y = %d\n", x, y);
    int result = http_client_request_sync(cyw43_arch_async_context(), &req);

    if (result == 0)
    {
        printf("Dados enviados com sucesso!\n");
    }
    else
    {
        printf("Erro ao enviar dados: %d\n", result);
    }
    sleep_ms(10); // Evita múltiplos envios rápidos
}
// Função para enviar os dados de um botão pressionado
void enviar_dados_botao(const char *caminho)
{
    EXAMPLE_HTTP_REQUEST_T req = {0}; // Inicializa a estrutura de requisição
    req.hostname = HOST;
    req.url = caminho; // Define o caminho do comando
    req.port = PORT;
    req.headers_fn = http_client_header_print_fn;
    req.recv_fn = http_client_receive_print_fn;

    printf("Enviando comando: %s\n", caminho);
    int result = http_client_request_sync(cyw43_arch_async_context(), &req);

    if (result == 0)
    {
        printf("Dados enviados com sucesso!\n");
    }
    else
    {
        printf("Erro ao enviar dados: %d\n", result);
    }
}
int main()
{
    // Inicializando entradas padrão
    stdio_init_all();
    // Configuração do Pino do Botão
    iniciar_botao(BUTTON_A_PIN); // Inicializa o botão A
    iniciar_botao(BUTTON_B_PIN); // Inicializa o botão B
    // Configuração do Pino do Joystick
    iniciar_joystick(JOYSTICK_X_PIN, JOYSTICK_Y_PIN, JOYSTICK_SW_PIN); // Inicializa o joystick no pino X, Y e SW
    // Conifguração do Led
    iniciar_led(RED_LED_PIN);   // Inicializa o LED vermelho
    iniciar_led(GREEN_LED_PIN); // Inicializa o LED verde
    iniciar_led(BLUE_LED_PIN);  // Inicializa o LED azul

    printf("\nIniciando cliente HTTP...\n");

    // Inicializando Wi-Fi
    if (wifi_connect(WIFI_SSID, WIFI_PASSWORD))
    {
        return 1; // Sai do programa em caso de erro
    }
    // Loop Principal
    // Variável para controlar o estado do botão A (1 = solto, 0 = pressionado)
    int button_a_state = 1;
    // Variável para controlar o estado do botão B (1 = solto, 0 = pressionado)
    int button_b_state = 1;

    // O loop principal do programa, onde as ações são realizadas continuamente
    while (1)
    {
        const char *path_a = NULL;       // Variável para armazenar o caminho do botão A
        const char *path_b = NULL;       // Variável para armazenar o caminho do botão  B
        const char *path_joystik = NULL; // Variável para armazenar o caminho do joystick

        // Se o botão A for apertado
        int estado_real_a = (gpio_get(BUTTON_A_PIN) == 0) ? 0 : 1; // 0: pressionado, 1: solto
        if (button_a_state != estado_real_a)                   // Verifica se o estado do botão A mudou
        {
            // Atualiza o caminho com base no estado do botão A
            path_a = (estado_real_a == 0) ? "/CLICK_A" : "/SOLTO_A";
            gpio_put(RED_LED_PIN, (estado_real_a == 0) ? 1 : 0);
            button_a_state = estado_real_a;

            // Envia o comando para o servidor Flask
            enviar_dados_botao(path_a);
            sleep_ms(20); // Evita múltiplos envios rápidos
        }
        // Se o botão B for apertado
        int estado_real_b = (gpio_get(BUTTON_B_PIN) == 0) ? 0 : 1; // 0: pressionado, 1: solto
        if (button_b_state != estado_real_b)                   // Verifica se o estado do botão A mudou
        {
            // Atualiza o caminho com base no estado do botão A
            path_b = (estado_real_b == 0) ? "/CLICK_B" : "/SOLTO_B";
            gpio_put(GREEN_LED_PIN, (estado_real_b == 0) ? 1 : 0);
            button_b_state = estado_real_b;

            // Função para enviar os dados do botão B
            enviar_dados_botao(path_b); // Envia os dados do botão B
            sleep_ms(20);               // Evita múltiplos envios rápidos
        }

        // =============LÓGICA PARA O JOYSTICK====================
        // Lê a posição do joystick
        uint16_t x = posicao_x(); // Lê o valor do eixo X
        uint16_t y = posicao_y(); // Lê o valor do eixo Y

        if (x < 1000) // O Joystick foi movido para a esquerda
        {
            gpio_put(BLUE_LED_PIN, 1);
            // Envia os dados para o servidor Flask
            enviar_dados_joystick(x, y);
        }
        else if (x > 3000) // O Joystick foi movido para a direita
        {
            gpio_put(RED_LED_PIN, 1);
            gpio_put(GREEN_LED_PIN, 1);
            gpio_put(BLUE_LED_PIN, 1);
            enviar_dados_joystick(x, y);
        }
        else if (y < 1000) // O Joystick foi movido para baixo
        {
            gpio_put(GREEN_LED_PIN, 1);
            enviar_dados_joystick(x, y);
        }
        else if (y > 3000) // O Joystick foi movido para cima
        {
            gpio_put(RED_LED_PIN, 1);
            enviar_dados_joystick(x, y);
        }
        else if (gpio_get(JOYSTICK_SW_PIN) == 0) // O Joystick centralizado
        {
            gpio_put(RED_LED_PIN, 1);
            gpio_put(GREEN_LED_PIN, 1);
            gpio_put(BLUE_LED_PIN, 1);
            enviar_dados_joystick(x, y);
        }
        else
        {
            gpio_put(RED_LED_PIN, 0);
            gpio_put(GREEN_LED_PIN, 0);
            gpio_put(BLUE_LED_PIN, 0);
            //enviar_dados_joystick(x, y);
        }
        enviar_dados_joystick(x, y);
        // Aguarda o intervalo definido antes de verificar novamente
        sleep_ms(INTERVALO_MS);
    }
}
