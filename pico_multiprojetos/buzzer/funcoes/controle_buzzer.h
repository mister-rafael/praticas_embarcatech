#ifndef CONTROLE_BUZZER_H
#define CONTROLE_BUZZER_H

#include "pico/stdlib.h"


// Funções para controle do buzzer
void iniciar_buzzer(uint pin);
void tocar_buzzer(uint pin, uint freq_hz);
void parar_buzzer(uint pin);

#endif // CONTROLE_BUZZER_H