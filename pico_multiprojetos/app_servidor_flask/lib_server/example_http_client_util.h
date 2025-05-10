/**
 * Copyright (c) 2024 Raspberry Pi (Trading) Ltd.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef EXAMPLE_HTTP_CLIENT_UTIL_H
#define EXAMPLE_HTTP_CLIENT_UTIL_H

#include "lwip/apps/http_client.h"

// ==================== DEFINIÇÕES DE PINOS ==================== //
// Define os pinos utilizados para os botões e LEDs no projeto.
#define BUTTON_A 5          // Pino do botão A (GPIO 5)
#define BUTTON_B 6          // Pino do botão B (GPIO 6)
#define LED_RED 13          // Pino do LED vermelho (GPIO 13)
#define LED_GREEN 12        // Pino do LED verde (GPIO 12)
#define LED_BLUE 11         // Pino do LED azul (GPIO 11)

// ============ CONFIGURAÇÕES DO JOYSTICK ============ //
// Define os pinos conectados ao joystick analógico.
#define JOYSTICK_X_PIN 27   // Canal ADC para eixo X (GPIO 27)
#define JOYSTICK_Y_PIN 26   // Canal ADC para eixo Y (GPIO 26)
#define JOYSTICK_SW_PIN 22  // Pino digital do botão do joystick (GPIO 22)

// ==================== FUNÇÕES AUXILIARES ==================== //
/**
 * @brief Conecta à rede Wi-Fi especificada.
 * 
 * @param ssid Nome da rede Wi-Fi.
 * @param password Senha da rede Wi-Fi.
 * @return 0 em caso de sucesso, ou outro valor em caso de erro.
 */
int wifi_connect(const char *ssid, const char *password);

/**
 * @brief Inicializa um botão em um pino GPIO.
 * 
 * @param pin Número do pino GPIO.
 */
void iniciar_botao(uint pin);

/**
 * @brief Inicializa um LED em um pino GPIO.
 * 
 * @param pin Número do pino GPIO.
 */
void iniciar_led(uint pin);

/**
 * @brief Inicializa o joystick com os pinos especificados.
 * 
 * @param pinx Pino do eixo X (ADC).
 * @param piny Pino do eixo Y (ADC).
 * @param pinw Pino do botão do joystick (digital).
 */
void iniciar_joystick(uint pinx, uint piny, uint pinw);

// ==================== ESTRUTURA DE REQUISIÇÃO HTTP ==================== //

/**
 * @brief Estrutura para armazenar parâmetros de uma requisição HTTP.
 * 
 * Utilizada com a biblioteca LWIP para fazer requisições HTTP (com ou sem TLS).
 */
typedef struct EXAMPLE_HTTP_REQUEST {
    const char *hostname;                  // Nome do host (ex: "meuservidor.com")
    const char *url;                       // Caminho da requisição (ex: "/comando")
    httpc_headers_done_fn headers_fn;      // Callback chamado quando os cabeçalhos são recebidos
    altcp_recv_fn recv_fn;                 // Callback chamado quando o corpo da resposta é recebido
    httpc_result_fn result_fn;             // Callback chamado ao final da requisição
    void *callback_arg;                    // Argumento opcional passado aos callbacks
    uint16_t port;                         // Porta da conexão (ex: 80 ou 443)

#if LWIP_ALTCP && LWIP_ALTCP_TLS
    struct altcp_tls_config *tls_config;   // Configuração TLS para conexões HTTPS
    altcp_allocator_t tls_allocator;       // Alocador TLS interno
#endif

    httpc_connection_t settings;           // Configurações da conexão HTTP
    int complete;                          // Flag: 1 quando a requisição for concluída
    httpc_result_t result;                 // Resultado final da requisição (0 = sucesso)
} EXAMPLE_HTTP_REQUEST_T;

struct async_context; // Contexto para operações assíncronas

// ==================== FUNÇÕES DE REQUISIÇÃO HTTP ==================== //

/**
 * @brief Executa uma requisição HTTP de forma assíncrona (não bloqueante).
 * 
 * @param context Ponteiro para o contexto assíncrono.
 * @param req Estrutura de requisição preenchida.
 * @return 0 se a requisição foi iniciada com sucesso, ou outro valor em caso de erro.
 * 
 * Observação: A função retorna imediatamente. O resultado será tratado nos callbacks.
 */
int http_client_request_async(struct async_context *context, EXAMPLE_HTTP_REQUEST_T *req);

/**
 * @brief Executa uma requisição HTTP de forma síncrona (bloqueante).
 * 
 * @param context Contexto assíncrono.
 * @param req Estrutura com os parâmetros da requisição.
 * @param result Retorna o resultado da requisição (0 = sucesso).
 * @return 0 em caso de sucesso, ou outro valor se falhar.
 * 
 * Observação: A função só retorna após a conclusão da requisição.
 */
int http_client_request_sync(struct async_context *context, EXAMPLE_HTTP_REQUEST_T *req);

// ==================== CALLBACKS ÚTEIS PARA DEBUG ==================== //

/**
 * @brief Callback que imprime os cabeçalhos HTTP no terminal.
 * 
 * Pode ser usado em headers_fn da requisição.
 * 
 * @param arg Argumento passado durante a inicialização.
 * @param hdr Cabeçalhos recebidos.
 * @param hdr_len Tamanho dos cabeçalhos.
 * @param content_len Tamanho do corpo (se informado pelo servidor).
 * @return Retornar diferente de zero cancela a conexão.
 */
err_t http_client_header_print_fn(httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, u32_t content_len);

/**
 * @brief Callback que imprime o corpo da resposta HTTP no terminal.
 * 
 * Pode ser usado em recv_fn da requisição.
 * 
 * @param arg Argumento passado durante a inicialização.
 * @param conn Estrutura da conexão.
 * @param p Dados do corpo da resposta.
 * @param err Código de erro (se houver).
 * @return Retornar diferente de zero cancela a conexão.
 */
err_t http_client_receive_print_fn(void *arg, struct altcp_pcb *conn, struct pbuf *p, err_t err);

#endif // EXAMPLE_HTTP_CLIENT_UTIL_H