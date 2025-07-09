#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <string.h> // Biblioteca para funções como memset e strlen

// --- CONFIGURAÇÕES DO TECLADO ---

// Definindo o layout do teclado 4x4
const char keys[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Pinos do Pico. 
const uint PINOS_COLUNAS[] = {17, 18, 19, 20};
const uint PINOS_LINHAS[] = {4, 8, 9, 16};

const uint NUM_LINHAS = 4;
const uint NUM_COLUNAS = 4;

// --- VARIÁVEIS PARA ARMAZENAR A SENHA ---

#define TAMANHO_MAX_SENHA 4 // A senha pode ter no máximo 4 dígitos
char senha_digitada[TAMANHO_MAX_SENHA + 1]; // Buffer para a senha + terminador nulo '\0'
int digitos_cont = 0; // Contador para os dígitos atuais

// Função para escanear e retornar a tecla pressionada
char ler_teclado() {
    for (int col = 0; col < NUM_COLUNAS; col++) {
        gpio_put(PINOS_COLUNAS[col], 0);
        sleep_us(1); 

        for (int row = 0; row < NUM_LINHAS; row++) {
            if (gpio_get(PINOS_LINHAS[row]) == 0) {
                char tecla_pressionada = keys[row][col];
                while (gpio_get(PINOS_LINHAS[row]) == 0) {
                    sleep_ms(20); // Debounce
                }
                gpio_put(PINOS_COLUNAS[col], 1);
                return tecla_pressionada;
            }
        }
        gpio_put(PINOS_COLUNAS[col], 1);
    }
    return '\0'; // Nenhuma tecla pressionada
}

// Função para exibir a senha de forma mascarada (com asteriscos)
void exibir_senha_mascarada() {
    printf("\rSenha: ");
    for (int i = 0; i < digitos_cont; i++) {
        printf("*");
    }
    // Limpa o resto da linha caso a nova senha seja menor que a anterior
    for (int i = digitos_cont; i < TAMANHO_MAX_SENHA; i++) {
        printf(" ");
    }
    fflush(stdout); // Força a atualização imediata do terminal
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

    // Configuração dos pinos GPIO
    for (int col = 0; col < NUM_COLUNAS; col++) {
        gpio_init(PINOS_COLUNAS[col]);
        gpio_set_dir(PINOS_COLUNAS[col], GPIO_OUT);
        gpio_put(PINOS_COLUNAS[col], 1);
    }
    for (int row = 0; row < NUM_LINHAS; row++) {
        gpio_init(PINOS_LINHAS[row]);
        gpio_set_dir(PINOS_LINHAS[row], GPIO_IN);
        gpio_pull_up(PINOS_LINHAS[row]);
    }
    
    // Limpa o buffer e inicia o programa
    memset(senha_digitada, 0, sizeof(senha_digitada));
    printf("Digite a senha e pressione A ou * para confirmar.\n");
    printf("Use B para apagar.\n");
    exibir_senha_mascarada();


    while (true) {
        char tecla = ler_teclado();

        if (tecla != '\0') { // Se uma tecla foi pressionada
            
            // Verifica se a tecla é um dígito (de '0' a '9')
            if (tecla >= '0' && tecla <= '9') {
                if (digitos_cont < TAMANHO_MAX_SENHA) {
                    senha_digitada[digitos_cont] = tecla;
                    digitos_cont++;
                    exibir_senha_mascarada();
                }
            } 
            // Verifica se a tecla é 'A' ou '*' (Confirmar)
            else if (tecla == 'A' || tecla == '*') {
                if (digitos_cont > 0) {
                    senha_digitada[digitos_cont] = '\0'; // Finaliza a string
                    printf("\nSenha final digitada: %s\n", senha_digitada);

                    // --- LÓGICA DE VALIDAÇÃO DA SENHA ---
                    // É aqui que você compara a senha digitada com a senha correta
                    if (strcmp(senha_digitada, "1357") == 0) { 
                        printf("ACESSO PERMITIDO!\n");
                        // Acione um relé, acenda um LED verde, etc.
                    } else {
                        printf("ACESSO NEGADO!\n");
                        // Acenda um LED vermelho, acione um alarme, etc.
                    }

                    // Prepara para a próxima senha
                    digitos_cont = 0;
                    memset(senha_digitada, 0, sizeof(senha_digitada));
                    printf("\nDigite a senha e pressione A ou * para confirmar.\n");
                    exibir_senha_mascarada();
                }
            }
            // Verifica se a tecla é 'B' (Apagar)
            else if (tecla == 'B') {
                if (digitos_cont > 0) {
                    digitos_cont--;
                    senha_digitada[digitos_cont] = '\0';
                    exibir_senha_mascarada();
                }
            }
            // Teclas 'C', 'D', '#' são ignoradas
        }

        sleep_ms(20); // Pequeno delay no loop principal
    }

    return 0;
}