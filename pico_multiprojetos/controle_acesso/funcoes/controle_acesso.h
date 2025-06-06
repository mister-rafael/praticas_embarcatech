#ifndef CONTROLE_ACESSO_H
#define CONTROLE_ACESSO_H

// Includes
#include <stdint.h> // Para uint32_t
#include <stdbool.h> // Para bool
#include "ssd1306.h" // Para manipulação do display OLED
#include "setup_utils.h" // Para inicialização de pinos e periféricos


// Declaração dos protótipos das funções antes do setup()
void setup_program();
void limpar_display(uint8_t *buffer, struct render_area *area);
void exibir_messagem(uint8_t *buffer, struct render_area *area, char *mensagem);
bool get_estado();
void set_estado(bool novo_estado);
void som_sucesso(uint pin);
void som_alarme(uint pin);
void coletar_senha(uint8_t *buffer, struct render_area *area);
void digitar_senha(uint8_t *buffer, struct render_area *area);
void verificar_senha(uint8_t *buffer, struct render_area *area);
void resetar_entrada(uint8_t *buffer, struct render_area *area);
void loop(uint8_t *buffer, struct render_area *area);

#endif // CONTROLE_ACESSO_H