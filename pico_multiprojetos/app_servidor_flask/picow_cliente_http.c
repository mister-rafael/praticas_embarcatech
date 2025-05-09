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
// #define HOST "10.3.10.92" // IP do servidor do IFPI
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
        if (gpio_get(BUTTON_A) == 0)
        {
            if (button_a_state == 1) // Se o botão estava solto e agora foi pressionado
            {                         
                path_a = "/CLICK_A";  // Envia a mensagem "CLICK"
                gpio_put(LED_RED, 1); // Liga o LED vermelho
                button_a_state = 0;   // Atualiza o estado para pressionado
            }
        }
        // Se o botão A for solto
        else
        {
            if (button_a_state == 0) // Se o botão estava pressionado e agora foi solto
            {
                path_a = "/SOLTO_A";  // Envia a mensagem "SOLTO"
                gpio_put(LED_RED, 0); // Desliga o LED vermelho
                button_a_state = 1;   // Atualiza o estado para solto
            }
        }
        // Se o botão B for apertado
        if (gpio_get(BUTTON_B) == 0)
        {
            if (button_b_state == 1)    // Se o botão estava solto e agora foi pressionado
            {
                path_b = "/CLICK_B";    // Envia a mensagem "CLICK"
                gpio_put(LED_GREEN, 1); // Liga o LED verde
                button_b_state = 0;     // Atualiza o estado para pressionado
            }
        }
        // Se o botão B for solto
        else
        {
            if (button_b_state == 0)    // Se o botão estava pressionado e agora foi solto
            {
                path_b = "/SOLTO_B";    // Envia a mensagem "SOLTO"
                gpio_put(LED_GREEN, 0); // Desliga o LED verde
                button_b_state = 1;     // Atualiza o estado para solto
            }
        }
        // Envia o comando para o servidor Flask se o botão A ou B for pressionado
        if (path_a != NULL)
        {
            /*EXAMPLE_HTTP_REQUEST_T req = {0}; // Inicializa a estrutura de requisição
            req.hostname = HOST; // Define o hostname do servidor
            req.url = path_a; // Define o caminho do comando
            req.port = PORT; // Define a porta do servidor
            req.headers_fn = http_client_header_print_fn; // Define a função de callback para os cabeçalhos
            req.recv_fn = http_client_receive_print_fn; // Define a função de callback para o recebimento da resposta

            printf("Enviando comando: %s\n", path_a); // Imprime o comando que está sendo enviado
            int result = http_client_request_sync(cyw43_arch_async_context(), &req); // Envia a requisição HTTP de forma síncrona

            if (result == 0) // Verifica se o envio foi bem-sucedido
            // Se o resultado for 0, significa que o comando foi enviado com sucesso
            {
                printf("Comando enviado com sucesso!\n");
            }
            else
            {
                printf("Erro ao enviar comando: %d\n", result);
            }*/
            enviar_dados_botao(path_a); // Envia os dados do botão A
            sleep_ms(20); // Evita múltiplos envios rápidos
        }
        if (path_b != NULL)
        {
            /*EXAMPLE_HTTP_REQUEST_T req = {0}; // Inicializa a estrutura de requisição
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
            }*/
            enviar_dados_botao(path_b); // Envia os dados do botão B
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

        // Aguarda o intervalo definido antes de verificar novamente
        sleep_ms(INTERVALO_MS);
    }
}
