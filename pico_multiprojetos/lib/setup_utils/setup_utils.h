#ifndef SETUP_UTILS_H
#define SETUP_UTILS_H

// Include necessary headers
#include <stdio.h> 
#include "hardware/i2c.h"

// Definições de pinagem
static const uint PIN_SDA = 14; // Pino de dados do I2C
static const uint PIN_SCL = 15; // Pino de clock do I2C

// Definições de pinos para botões
static const uint BUTTON_A_PIN = 5; // Botão A
static const uint BUTTON_B_PIN = 6; // Botão B

// Definições de pinos para LEDs RGB
static const uint BLUE_LED_PIN = 12;  // LED azul
static const uint RED_LED_PIN = 13;   // LED vermelho
static const uint GREEN_LED_PIN = 11; // LED verde

// Definições de pinos para o buzzer
static const uint BUZZER_PIN_1 = 21;   // Buzzer 1
static const uint BUZZER_PIN_2 = 22;   // Buzzer 2
static const uint BUZZER_FREQ = 100;   // Frequência do buzzer

// ============ CONFIGURAÇÕES DO JOYSTICK ============ //
// Define os pinos conectados ao joystick analógico.
static const uint JOYSTICK_X_PIN = 27;   // Canal ADC para eixo X (GPIO 27)
static const uint JOYSTICK_Y_PIN = 26;   // Canal ADC para eixo Y (GPIO 26)
static const uint JOYSTICK_SW_PIN = 22;  // Pino digital do botão do joystick (GPIO 22)

// Declaração de funções para configuração do sistema e periféricos
// -----------------------------------------------------------------------------------
void iniciar_botao(uint pin); // Inicializa o botão na porta especificada
void iniciar_led(uint pin); // Inicializa o LED na porta especificada
void iniciar_i2c(i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint baudrate_khz); // Inicializa a interface I2C
void iniciar_buzzer(uint pin); // Inicializa o buzzer na porta especificada
void iniciar_joystick(uint pinx, uint piny, uint pinw);
#endif // SETUP_UTILS_H