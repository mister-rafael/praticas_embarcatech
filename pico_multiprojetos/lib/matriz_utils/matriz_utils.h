#ifndef MATRIZ_UTILS_H
#define MATRIZ_UTILS_H

// Inclue as bibliotecas necessárias
#include <stdint.h> // Para uint16_t e uint8_t
#include "pico/stdlib.h" // Isso define "uint" para o Pico

// Definição do número de LEDs e pino da matriz de Led.
#define LED_COUNT 25
#define LED_PIN 7

// Definição das funções para controle da matriz de LEDs
void npInit(uint pin); // Inicializa a matriz de LEDs
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b); // Define a cor de um LED específico
void npClear(); // Limpa o buffer de LEDs
void npWrite(); // Envia os dados para a matriz de LEDs
void npSetColor(uint pin, uint8_t r, uint8_t g, uint8_t b); // Define a cor de todos os LEDs
//testes de funções
void rgbFadeEffect(uint delay_ms); // Cria um efeito de fade entre os LEDs
void lightUpOneByOne(uint delay_ms, uint8_t r, uint8_t g, uint8_t b); // Acende os LEDs um por um
void alexaEffect(uint delay_ms); // Simula o efeito de LED da Alexa em uma matriz de LEDs
void contourMatrix(uint delay_ms, uint8_t r, uint8_t g, uint8_t b); // Acende os LEDs em contorno
#endif // MATRIZ_UTILS_H