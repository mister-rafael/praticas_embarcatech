#ifndef SETUP_UTILS_H
#define SETUP_UTILS_H

// Include necessary headers
#include <stdio.h> 
#include "hardware/i2c.h"

// Definições de pinagem
#define PIN_SDA 14 // Pino de dados do I2C
#define PIN_SCL 15 // Pino de clock do I2C

#define BUTTON_A_PIN 5 // Botão A
#define BUTTON_B_PIN 6 // Botão B

#define BLUE_LED_PIN 12  // LED azul
#define RED_LED_PIN 13   // LED vermelho
#define GREEN_LED_PIN 11 // LED verde

#define BUZZER_PIN_1 21   // Buzzer 1
#define BUZZER_PIN_2 22 // Buzzer 2
#define BUZZER_FREQ 100 // Frequência do buzzer

// Declaração de funções para configuração do sistema e periféricos
// -----------------------------------------------------------------------------------
void iniciar_botao(uint pin); // Inicializa o botão na porta especificada
void iniciar_led(uint pin); // Inicializa o LED na porta especificada
void iniciar_i2c(i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint baudrate_khz); // Inicializa a interface I2C
void iniciar_buzzer(uint pin); // Inicializa o buzzer na porta especificada

#endif // SETUP_UTILS_H