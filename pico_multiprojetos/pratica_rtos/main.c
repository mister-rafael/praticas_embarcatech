#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Configuração dos pinos do LED RGB
const uint BLUE_LED_PIN = 12; // LED azul no GPIO 12
// Configuração dos pinos dos botões como entrada com pull-up
const uint BUTTON_A_PIN = 5; // Botão A no GPIO 5

// Função de configuração inicial
QueueHandle_t filaBotao;
QueueHandle_t filaLED;

// Função de configuração inicial
void setup()
{
    stdio_init_all(); // Inicializa os tipos stdio padrão presentes ligados ao binário

    // Inicialização dos botões
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    // Inicialização dos LEDs
    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);

    // Inicialmente, desligar o LED RGB
    gpio_put(BLUE_LED_PIN, 0);
}
// Tarefa 1: Leitura do botão
void tarefaLeituraBotao(void *pvParameters)
{
    int estadoBotao;
    while (1)
    {
        // Lê o estado do botão (0 = pressionado, 1 = solto) e converte para 1 quando pressionado
        estadoBotao = gpio_get(BUTTON_A_PIN) == 0 ? 1 : 0;

        // Envia o estado do botão para a fila
        xQueueOverwrite(filaBotao, &estadoBotao);

        // Aguarda 100ms antes de realizar a próxima leitura
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
// Tarefa 2: Processamento do estado do botão
void tarefaProcessamentoBotao(void *pvParameters)
{
    int estadoBotao;
    while (1)
    {
        // Aguarda e recebe o estado do botão da fila
        if (xQueueReceive(filaBotao, &estadoBotao, portMAX_DELAY))
        {
            // Recebe o estado do botão, enviado da tarefa anterior
            int comandoLED = estadoBotao;
            // Passa o estado do botão para a fila do LED
            xQueueOverwrite(filaLED, &comandoLED);
        }
    }
}
// Tarefa 3: Controle do LED
void tarefaControleLED(void *pvParameters)
{
    int estadoLED;
    while (1)
    {
        // Aguarda e recebe o comando do LED da fila
        if (xQueueReceive(filaLED, &estadoLED, portMAX_DELAY))
        {
            // Aciona ou desliga o LED azul conforme o comando recebido.
            gpio_put(BLUE_LED_PIN, estadoLED); // Liga o LED azul
        }
    }
}
// Função principal (entry point do programa)
int main()
{
    setup(); // Configuração inicial dos pinos
    // Criação das filas para comunicação entre as tarefas
    filaBotao = xQueueCreate(1, sizeof(int));
    filaLED = xQueueCreate(1, sizeof(int));

    // Verifica se as filas foram criadas corretamente antes de iniciar as tarefas
    if (filaBotao != NULL && filaLED != NULL)
    {
        // Criação das tarefas do FreeRTOS
        xTaskCreate(tarefaLeituraBotao, "LeituraBotao", 256, NULL, 1, NULL);
        xTaskCreate(tarefaProcessamentoBotao, "Processamento", 256, NULL, 2, NULL);
        xTaskCreate(tarefaControleLED, "ControleLED", 256, NULL, 1, NULL);

        // Inicia o escalonador do FreeRTOS para executar as tarefas
        vTaskStartScheduler();
    }

    // Caso algo dê errado e o escalonador não inicie, entra em um loop infinito
    while (1)
        ;
}