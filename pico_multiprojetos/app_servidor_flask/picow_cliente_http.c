/**
 * Cliente HTTP para Raspberry Pi Pico W
 */
#include <stdio.h>
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"
#include "pico/async_context.h"
#include "lwip/altcp_tls.h"
#include "example_http_client_util.h"
// INLUDES PARA USO DO JOYSTICK
#include "hardware/adc.h"

// ========== CONFIGURAÇÕES ======================== //
#define HOST "192.168.0.20" // IP do servidor de casa
// #define HOST "10.3.10.102" // IP do servidor do IFPI
#define PORT 5000        // Porta do servidor
#define INTERVALO_MS 500 // Intervalo de verificação do botão (em milissegundos)

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
    sleep_ms(20); // Evita múltiplos envios rápidos
}
int main()
{
    // Inicializando entradas padrão
    stdio_init_all();
    // Configuração do Pino do Botão
    iniciar_botao(BUTTON_A); // Inicializa o botão A
    iniciar_botao(BUTTON_B); // Inicializa o botão B
    // Configuração do Pino do Joystick
    iniciar_joystick(JOYSTICK_X_PIN, JOYSTICK_Y_PIN, JOYSTICK_SW_PIN); // Inicializa o joystick no pino X, Y e SW
    // Conifguração do Led
    iniciar_led(LED_RED);   // Inicializa o LED vermelho
    iniciar_led(LED_GREEN); // Inicializa o LED verde
    iniciar_led(LED_BLUE);  // Inicializa o LED azul

    printf("\nIniciando cliente HTTP...\n");

    // Inicializando Wi-Fi
    if (wifi_connect(WIFI_SSID, WIFI_PASSWORD))
    {
        return 1; // Sai do programa em caso de erro
    }
    // Loop Principal
    int button_a_state = 1; // Variável para controlar o estado do botão A (1 = solto, 0 = pressionado)
    int button_b_state = 1; // Variável para controlar o estado do botão B (1 = solto, 0 = pressionado)

    while (1)
    {
        const char *path_a = NULL;
        const char *path_b = NULL;
        const char *path_joystik = NULL;

        // Se o botão A for apertado
        if (gpio_get(BUTTON_A) == 0)
        {
            if (button_a_state == 1)
            {                        // Se o botão estava solto e agora foi pressionado
                path_a = "/CLICK_A"; // Envia a mensagem "CLICK"
                gpio_put(LED_RED, 1);
                button_a_state = 0; // Atualiza o estado para pressionado
            }
        }
        else
        {
            if (button_a_state == 0)
            {                        // Se o botão estava pressionado e agora foi solto
                path_a = "/SOLTO_A"; // Envia a mensagem "SOLTO"
                gpio_put(LED_RED, 0);
                button_a_state = 1; // Atualiza o estado para solto
            }
        }
        // Se o botão B for apertado
        if (gpio_get(BUTTON_B) == 0)
        {
            if (button_b_state == 1)
            {                        // Se o botão estava solto e agora foi pressionado
                path_b = "/CLICK_B"; // Envia a mensagem "CLICK"
                gpio_put(LED_GREEN, 1);
                button_b_state = 0; // Atualiza o estado para pressionado
            }
        }
        else
        {
            if (button_b_state == 0)
            {                        // Se o botão estava pressionado e agora foi solto
                path_b = "/SOLTO_B"; // Envia a mensagem "SOLTO"
                gpio_put(LED_GREEN, 0);
                button_b_state = 1; // Atualiza o estado para solto
            }
        }
        // Envia os comandos para o servidor
        if (path_a != NULL)
        {
            EXAMPLE_HTTP_REQUEST_T req = {0}; // Inicializa a estrutura de requisição
            req.hostname = HOST;
            req.url = path_a;
            req.port = PORT;
            req.headers_fn = http_client_header_print_fn;
            req.recv_fn = http_client_receive_print_fn;

            printf("Enviando comando: %s\n", path_a);
            int result = http_client_request_sync(cyw43_arch_async_context(), &req);

            if (result == 0)
            {
                printf("Comando enviado com sucesso!\n");
            }
            else
            {
                printf("Erro ao enviar comando: %d\n", result);
            }

            sleep_ms(20); // Evita múltiplos envios rápidos
        }
        if (path_b != NULL)
        {
            EXAMPLE_HTTP_REQUEST_T req = {0}; // Inicializa a estrutura de requisição
            req.hostname = HOST;
            req.url = path_b;
            req.port = PORT;
            req.headers_fn = http_client_header_print_fn;
            req.recv_fn = http_client_receive_print_fn;

            printf("Enviando comando: %s\n", path_b);
            int result = http_client_request_sync(cyw43_arch_async_context(), &req);

            if (result == 0)
            {
                printf("Comando enviado com sucesso!\n");
            }
            else
            {
                printf("Erro ao enviar comando: %d\n", result);
            }

            sleep_ms(20); // Evita múltiplos envios rápidos
        }

        // =============LÓGICA PARA O JOYSTICK====================
        // Lê a posição do joystick
        uint16_t x = posicao_x(); // Lê o valor do eixo X
        uint16_t y = posicao_y(); // Lê o valor do eixo Y

        if (x < 1000) // O Joystick foi movido para a esquerda
        {
            // path_joystik = "/x_left"; // Envia a mensagem "CLICK"
            //  printf("X: %d\n", x);
            gpio_put(LED_BLUE, 1);
            // Envia os dados para o servidor Flask
            enviar_dados_joystick(x, y);
        }
        else if (x > 3000) // O Joystick foi movido para a direita
        {
            // path_joystik = "/x_right";
            gpio_put(LED_RED, 1);
            gpio_put(LED_GREEN, 1);
            gpio_put(LED_BLUE, 1);
            enviar_dados_joystick(x, y);
        }
        else if (y < 1000) // O Joystick foi movido para baixo
        {
            // path_joystik = "/y_down"; // Envia a mensagem "CLICK"
            gpio_put(LED_GREEN, 1);
            enviar_dados_joystick(x, y);
        }
        else if (y > 3000) // O Joystick foi movido para cima
        {
            // path_joystik = "/y_up"; // Envia a mensagem "SOLTO"
            gpio_put(LED_RED, 1);
            enviar_dados_joystick(x, y);
        }
        else if (gpio_get(JOYSTICK_SW_PIN) == 0) // O Joystick centralizado
        {
            // path_joystik = "/y_center"; // Envia a mensagem "CLICK"
            gpio_put(LED_RED, 1);
            gpio_put(LED_GREEN, 1);
            gpio_put(LED_BLUE, 1);
            enviar_dados_joystick(x, y);
        }
        else
        {
            gpio_put(LED_RED, 0);
            gpio_put(LED_GREEN, 0);
            gpio_put(LED_BLUE, 0);
            enviar_dados_joystick(x, y);
        }
        sleep_ms(INTERVALO_MS); // Aguarda o intervalo definido antes de verificar novamente
    }
}
