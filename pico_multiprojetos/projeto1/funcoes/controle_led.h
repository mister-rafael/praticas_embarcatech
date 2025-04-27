#ifndef CONTROLE_LED_H
#define CONTROLE_LED_H

#include "pico/stdlib.h"

// Definições de pinos para o LED RGB
#define RED_LED_PIN 11
#define GREEN_LED_PIN 12
#define BLUE_LED_PIN 13

// Funções para controle do LED RGB
void iniciar_led(uint pin);
void ligar_led(uint pin);
void desligar_led(uint pin);

#endif // CONTROLE_LED_H