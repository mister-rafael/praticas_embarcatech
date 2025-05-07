/**
 * Copyright (c) 2024 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

 #ifndef EXAMPLE_HTTP_CLIENT_UTIL_H
 #define EXAMPLE_HTTP_CLIENT_UTIL_H
 
 #include "lwip/apps/http_client.h"

 // ======= CONFIGURAÇÕES ======= //
#define BUTTON_A 5          // Pino do botão A
#define BUTTON_B 6          // Pino do botão B
#define LED_RED 13          // Pino do LED vermelho
#define LED_GREEN 12        // Pino do LED verde
#define LED_BLUE 11         // Pino do LED azul
// ============================= //
// ======== CONFIGURAÇÕES DO JOYSTIK ================ //
#define JOYSTICK_X_PIN 27
#define JOYSTICK_Y_PIN 26
#define JOYSTICK_SW_PIN 22
// =================================================== //
// ======= FUNÇÕES AUTORAIS ======= //
int wifi_connect(const char *ssid, const char *password);
void iniciar_botao(uint pin); // Inicializa o botão na porta especificada
void iniciar_led(uint pin);   // Inicializa o LED na porta especificada
void iniciar_joystick(uint pinx, uint piny, uint pinw); // Inicializa o joystick na porta especificada
// ============================= //
 /*! \brief Parameters used to make HTTP request
  *  \ingroup pico_lwip
  */
 typedef struct EXAMPLE_HTTP_REQUEST {
     //Diz pra quem é o pedido (hostname e url);
     const char *hostname;
     const char *url;
    httpc_headers_done_fn headers_fn; //Função de callback que é chamada quando os cabeçalhos HTTP chegam
    altcp_recv_fn recv_fn; //Função de callback que será chamada quando o conteúdo da resposta chegar.
    httpc_result_fn result_fn; //Função que é chamada quando o pedido termina, com sucesso ou erro.
    void *callback_arg; //Informação extra que você pode passar para usar dentro das funções de callback.
    uint16_t port; //A porta a ser usada. Uma porta padrão é escolhida se este valor for definido como zero

 #if LWIP_ALTCP && LWIP_ALTCP_TLS
     /*!
      * TLS configuration, can be null or set to a correctly configured tls configuration.
      * e.g altcp_tls_create_config_client(NULL, 0) would use https without a certificate
      */
     struct altcp_tls_config *tls_config;
     /*!
      * TLS allocator, used internall for setting TLS server name indication
      */
     altcp_allocator_t tls_allocator;
 #endif
     /*!
      * LwIP HTTP client settings
      */
     httpc_connection_t settings;
     /*!
      * Flag to indicate when the request is complete
      */
     int complete;
     /*!
      * Overall result of http request, only valid when complete is set
      */
     httpc_result_t result;
 
 } EXAMPLE_HTTP_REQUEST_T; // Aqui terminamos a declaração da struct. Ela é usada como tipo de dado.
 
 struct async_context;
 
/*! \brief Executa uma requisição HTTP de forma assíncrona
 *  \ingroup pico_lwip
 *
 * Executa a requisição HTTP de forma assíncrona
 * Assíncrona (funciona como pedir uma pizza e continuar jogando videogame)
 *
 * @param context contexto assíncrono
 * @param req Parâmetros da requisição HTTP. No mínimo, deve ser inicializado com zero e com hostname e url definidos com valores válidos.
 * @return Se retornar zero, a requisição foi feita e estará completa quando \em req->complete for verdadeiro ou o callback de resultado tiver sido chamado.
 *  Retornar um valor diferente de zero indica um erro.
 *
 * @see async_context
 */
 int http_client_request_async(struct async_context *context, EXAMPLE_HTTP_REQUEST_T *req);
 
 /*! \brief Perform a http request synchronously
  *  \ingroup pico_lwip
  *
  * Perform the http request synchronously
  * Síncrona (você espera a pizza chegar antes de jogar)
  *
  * @param context async context
  * @param req HTTP request parameters. As a minimum this should be initialised to zero with hostname and url set to valid values
  * @param result Returns the overall result of the http request when complete. Zero indicates success.
  */
 int http_client_request_sync(struct async_context *context, EXAMPLE_HTTP_REQUEST_T *req);
 
 /*! \brief A http header callback that can be passed to \em http_client_init or \em http_client_init_secure
  *  \ingroup pico_http_client
  *
  * An implementation of the http header callback which just prints headers to stdout
  *
  * @param arg argument specified on initialisation
  * @param hdr header pbuf(s) (may contain data also)
  * @param hdr_len length of the headers in 'hdr'
  * @param content_len content length as received in the headers (-1 if not received)
  * @return if != zero is returned, the connection is aborted
  */
 err_t http_client_header_print_fn(httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, u32_t content_len);
 
 /*! \brief A http recv callback that can be passed to http_client_init or http_client_init_secure
  *  \ingroup pico_http_client
  *
  * An implementation of the http recv callback which just prints the http body to stdout
  *
  * @param arg argument specified on initialisation
  * @param conn http client connection
  * @param p body pbuf(s)
  * @param err Error code in the case of an error
  * @return if != zero is returned, the connection is aborted
  */
 err_t http_client_receive_print_fn(void *arg, struct altcp_pcb *conn, struct pbuf *p, err_t err);
 
 #endif 