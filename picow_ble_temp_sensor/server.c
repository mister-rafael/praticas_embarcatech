/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

 #include <stdio.h>
 #include "btstack.h"
 #include "pico/cyw43_arch.h"
 #include "pico/btstack_cyw43.h"
 #include "hardware/adc.h"
 #include "pico/stdlib.h"
 
 #include "server_common.h"
 
 #define HEARTBEAT_PERIOD_MS 1000  // Intervalo do timer (1 segundo)
 
 // Variáveis globais
 static btstack_timer_source_t heartbeat;
 static btstack_packet_callback_registration_t hci_event_callback_registration;
 
 // Função chamada periodicamente para atualizar estado do dispositivo
 static void heartbeat_handler(struct btstack_timer_source *ts) {
     static uint32_t counter = 0;
     counter++;
 
     // Atualizar temperatura a cada 10 segundos
     if (counter % 10 == 0) {
         poll_temp();
         if (le_notification_enabled) {
             att_server_request_can_send_now_event(con_handle);
         }
     }
 
     // Alternar o LED da Pico W
     static int led_on = true;
     led_on = !led_on;
     cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
 
     // Reiniciar o timer
     btstack_run_loop_set_timer(ts, HEARTBEAT_PERIOD_MS);
     btstack_run_loop_add_timer(ts);
 }
 
 int main() {
     stdio_init_all();
 
     // Inicializar a arquitetura do driver CYW43 (habilita o Bluetooth)
     if (cyw43_arch_init()) {
         printf("Falha ao inicializar cyw43_arch\n");
         return -1;
     }
 
     // Inicializar o ADC para leitura da temperatura
     adc_init();
     adc_select_input(ADC_CHANNEL_TEMPSENSOR);
     adc_set_temp_sensor_enabled(true);
 
     // Inicializar o Bluetooth
     l2cap_init();
     sm_init();
     att_server_init(profile_data, att_read_callback, att_write_callback);
 
     // Registrar callback para eventos do Bluetooth
     hci_event_callback_registration.callback = &packet_handler;
     hci_add_event_handler(&hci_event_callback_registration);
     
     // Registrar callback para eventos ATT
     att_server_register_packet_handler(packet_handler);
 
     // Iniciar timer para execução periódica
     heartbeat.process = &heartbeat_handler;
     btstack_run_loop_set_timer(&heartbeat, HEARTBEAT_PERIOD_MS);
     btstack_run_loop_add_timer(&heartbeat);
 
     // Ligar o Bluetooth
     hci_power_control(HCI_POWER_ON);
 
     // Loop infinito para manter o servidor ativo
     while (true) {      
         sleep_ms(1000);
     }
 
     return 0;
 }
 