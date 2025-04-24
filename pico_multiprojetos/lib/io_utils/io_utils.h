#ifndef IO_UTILS_H
#define IO_UTILS_H

#include "pico/stdlib.h" // Isso define "uint" para o Pico

void iniciar_led(uint pin);
void piscar_led(uint pin, int delay_ms);

#endif