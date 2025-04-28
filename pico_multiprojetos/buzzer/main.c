#include "funcoes/controle_buzzer.h"
#include "pico/stdlib.h"

int main() {
    // Inicializa o buzzer
    iniciar_buzzer(21); // Pino 15 como exemplo

    while (true) {
        // Emite som no buzzer
        for (int i = 0; i < 20; i++) {
            tocar_buzzer(21, 2000); // Frequência de 2000 Hz
            sleep_ms(100); // Duração do som
            parar_buzzer(21); // Para o som
            sleep_ms(50); // Pausa entre os toques
        }
        parar_buzzer(21);
        /*tocar_buzzer(21, 1000); // Frequência de 1000 Hz
        sleep_ms(1000);
        parar_buzzer(21);
        sleep_ms(1000);*/
    }

    return 0;
}