#ifndef IO_UTILS_H
#define IO_UTILS_H

#include "pico/stdlib.h" // Isso define "uint" para o Pico

// DEFINIÇÃO DAS FUNÇÕES DO LED RGB
// ---------------------------------------------------------------
void ligar_led(uint pin);
void desligar_led(uint pin);
void piscar_led(uint pin, int delay_ms);
//---------------------------------------------------------------

void tocar_buzzer(uint pin, uint freq_hz);
void parar_buzzer(uint pin);
void beep(uint pin, uint duration_ms);
#endif