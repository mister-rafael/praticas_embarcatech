/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

 #include <stdio.h>
 #include "btstack.h"
 #include "pico/cyw43_arch.h"
 #include "pico/stdlib.h"
 
 #if 0
 #define DEBUG_LOG(...) printf(__VA_ARGS__)
 #else
 #define DEBUG_LOG(...)
 #endif
 
 // Definição dos tempos de delay para o LED piscar
 #define LED_QUICK_FLASH_DELAY_MS 100
 #define LED_SLOW_FLASH_DELAY_MS 1000
 
 // Enumeração dos estados do cliente Bluetooth
 typedef enum {
     TC_OFF, // Desligado
     TC_IDLE, // Inativo
     TC_W4_SCAN_RESULT, // Aguardando resultado do scan
     TC_W4_CONNECT, // Aguardando conexão
     TC_W4_SERVICE_RESULT, // Aguardando resultado do serviço
     TC_W4_CHARACTERISTIC_RESULT, // Aguardando resultado da característica
     TC_W4_ENABLE_NOTIFICATIONS_COMPLETE, // Aguardando ativação de notificações
     TC_W4_READY // Pronto
 } gc_state_t;
 
 // Variáveis globais
 static btstack_packet_callback_registration_t hci_event_callback_registration;
 static gc_state_t state = TC_OFF;
 static bd_addr_t server_addr; // Endereço do servidor
 static bd_addr_type_t server_addr_type; // Tipo de endereço do servidor
 static hci_con_handle_t connection_handle; // Handle da conexão
 static gatt_client_service_t server_service; // Serviço do servidor
 static gatt_client_characteristic_t server_characteristic; // Característica do servidor
 static bool listener_registered; // Listener de notificações registrado
 static gatt_client_notification_t notification_listener; // Listener de notificações
 static btstack_timer_source_t heartbeat; // Timer para o LED piscar
 
 // Função para iniciar o cliente Bluetooth
 static void client_start(void) {
     DEBUG_LOG("Iniciando scan!\n");
     state = TC_W4_SCAN_RESULT;
     gap_set_scan_parameters(0, 0x0030, 0x0030);
     gap_start_scan();
 }
 
 // Função para verificar se o relatório de anúncio contém um serviço específico
 static bool advertisement_report_contains_service(uint16_t service, uint8_t *advertisement_report) {
     // Obtém os dados do relatório de anúncio
     const uint8_t *adv_data = gap_event_advertising_report_get_data(advertisement_report);
     uint8_t adv_len = gap_event_advertising_report_get_data_length(advertisement_report);
 
     // Itera sobre os dados de anúncio
     ad_context_t context;
     for (ad_iterator_init(&context, adv_len, adv_data); ad_iterator_has_more(&context); ad_iterator_next(&context)) {
         uint8_t data_type = ad_iterator_get_data_type(&context);
         uint8_t data_size = ad_iterator_get_data_len(&context);
         const uint8_t *data = ad_iterator_get_data(&context);
         switch (data_type) {
             case BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS:
                 for (int i = 0; i < data_size; i += 2) {
                     uint16_t type = little_endian_read_16(data, i);
                     if (type == service) return true;
                 }
             default:
                 break;
         }
     }
     return false;
 }
 
 // Função para lidar com eventos do cliente GATT
 static void handle_gatt_client_event(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
     UNUSED(packet_type);
     UNUSED(channel);
     UNUSED(size);
 
     uint8_t att_status;
     switch (state) {
         case TC_W4_SERVICE_RESULT:
             switch (hci_event_packet_get_type(packet)) {
                 case GATT_EVENT_SERVICE_QUERY_RESULT:
                     // Armazena o serviço (esperamos apenas um)
                     DEBUG_LOG("Armazenando serviço\n");
                     gatt_event_service_query_result_get_service(packet, &server_service);
                     break;
                 case GATT_EVENT_QUERY_COMPLETE:
                     att_status = gatt_event_query_complete_get_att_status(packet);
                     if (att_status != ATT_ERROR_SUCCESS) {
                         printf("ERRO SERVICE_QUERY_RESULT, Código ATT 0x%02x.\n", att_status);
                         gap_disconnect(connection_handle);
                         break;
                     }
                     // Consulta de serviço concluída, busca pela característica
                     state = TC_W4_CHARACTERISTIC_RESULT;
                     DEBUG_LOG("Procurando pela característica de sensoriamento ambiental.\n");
                     gatt_client_discover_characteristics_for_service_by_uuid16(handle_gatt_client_event, connection_handle, &server_service, ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE);
                     break;
                 default:
                     break;
             }
             break;
         case TC_W4_CHARACTERISTIC_RESULT:
             switch (hci_event_packet_get_type(packet)) {
                 case GATT_EVENT_CHARACTERISTIC_QUERY_RESULT:
                     DEBUG_LOG("Armazenando característica\n");
                     gatt_event_characteristic_query_result_get_characteristic(packet, &server_characteristic);
                     break;
                 case GATT_EVENT_QUERY_COMPLETE:
                     att_status = gatt_event_query_complete_get_att_status(packet);
                     if (att_status != ATT_ERROR_SUCCESS) {
                         printf("ERRO CHARACTERISTIC_QUERY_RESULT, Código ATT 0x%02x.\n", att_status);
                         gap_disconnect(connection_handle);
                         break;
                     }
                     // Registra o handler para notificações
                     listener_registered = true;
                     gatt_client_listen_for_characteristic_value_updates(&notification_listener, handle_gatt_client_event, connection_handle, &server_characteristic);
                     // Ativa notificações
                     DEBUG_LOG("Ativando notificações na característica.\n");
                     state = TC_W4_ENABLE_NOTIFICATIONS_COMPLETE;
                     gatt_client_write_client_characteristic_configuration(handle_gatt_client_event, connection_handle,
                                                                          &server_characteristic, GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION);
                     break;
                 default:
                     break;
             }
             break;
         case TC_W4_ENABLE_NOTIFICATIONS_COMPLETE:
             switch (hci_event_packet_get_type(packet)) {
                 case GATT_EVENT_QUERY_COMPLETE:
                     DEBUG_LOG("Notificações ativadas, Status ATT 0x%02x\n", gatt_event_query_complete_get_att_status(packet));
                     if (gatt_event_query_complete_get_att_status(packet) != ATT_ERROR_SUCCESS) break;
                     state = TC_W4_READY;
                     break;
                 default:
                     break;
             }
             break;
         case TC_W4_READY:
             switch (hci_event_packet_get_type(packet)) {
                 case GATT_EVENT_NOTIFICATION: {
                     uint16_t value_length = gatt_event_notification_get_value_length(packet);
                     const uint8_t *value = gatt_event_notification_get_value(packet);
                     DEBUG_LOG("Valor de indicação len %d\n", value_length);
                     if (value_length == 2) {
                         float temp = little_endian_read_16(value, 0);
                         printf("Temperatura lida: %.2f °C\n", temp / 100);
                     } else {
                         printf("Comprimento inesperado %d\n", value_length);
                     }
                     break;
                 }
                 default:
                     printf("Tipo de pacote desconhecido 0x%02x\n", hci_event_packet_get_type(packet));
                     break;
             }
             break;
         default:
             printf("Erro\n");
             break;
     }
 }
 
 // Função para lidar com eventos HCI
 static void hci_event_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
     UNUSED(size);
     UNUSED(channel);
     bd_addr_t local_addr;
     if (packet_type != HCI_EVENT_PACKET) return;
 
     uint8_t event_type = hci_event_packet_get_type(packet);
     switch (event_type) {
         case BTSTACK_EVENT_STATE:
             if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
                 gap_local_bd_addr(local_addr);
                 printf("BTstack funcionando em %s.\n", bd_addr_to_str(local_addr));
                 client_start();
             } else {
                 state = TC_OFF;
             }
             break;
         case GAP_EVENT_ADVERTISING_REPORT:
             if (state != TC_W4_SCAN_RESULT) return;
             // Verifica o nome no anúncio
             if (!advertisement_report_contains_service(ORG_BLUETOOTH_SERVICE_ENVIRONMENTAL_SENSING, packet)) return;
             // Armazena endereço e tipo
             gap_event_advertising_report_get_address(packet, server_addr);
             server_addr_type = gap_event_advertising_report_get_address_type(packet);
             // Para o scan e conecta ao dispositivo
             state = TC_W4_CONNECT;
             gap_stop_scan();
             printf("Conectando ao dispositivo com endereço %s.\n", bd_addr_to_str(server_addr));
             gap_connect(server_addr, server_addr_type);
             break;
         case HCI_EVENT_LE_META:
             // Aguarda a conclusão da conexão
             switch (hci_event_le_meta_get_subevent_code(packet)) {
                 case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
                     if (state != TC_W4_CONNECT) return;
                     connection_handle = hci_subevent_le_connection_complete_get_connection_handle(packet);
                     // Inicializa o contexto do cliente GATT com o handle e adiciona à lista de clientes ativos
                     // Consulta os serviços primários
                     DEBUG_LOG("Procurando pelo serviço de sensoriamento ambiental.\n");
                     state = TC_W4_SERVICE_RESULT;
                     gatt_client_discover_primary_services_by_uuid16(handle_gatt_client_event, connection_handle, ORG_BLUETOOTH_SERVICE_ENVIRONMENTAL_SENSING);
                     break;
                 default:
                     break;
             }
             break;
         case HCI_EVENT_DISCONNECTION_COMPLETE:
             // Desregistra o listener
             connection_handle = HCI_CON_HANDLE_INVALID;
             if (listener_registered) {
                 listener_registered = false;
                 gatt_client_stop_listening_for_characteristic_value_updates(&notification_listener);
             }
             printf("Desconectado %s\n", bd_addr_to_str(server_addr));
             if (state == TC_OFF) break;
             client_start();
             break;
         default:
             break;
     }
 }
 
 // Função para lidar com o timer do LED
 static void heartbeat_handler(struct btstack_timer_source *ts) {
     // Inverte o estado do LED
     static bool quick_flash;
     static bool led_on = true;
 
     led_on = !led_on;
     cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
     if (listener_registered && led_on) {
         quick_flash = !quick_flash;
     } else if (!listener_registered) {
         quick_flash = false;
     }
 
     // Reinicia o timer
     btstack_run_loop_set_timer(ts, (led_on || quick_flash) ? LED_QUICK_FLASH_DELAY_MS : LED_SLOW_FLASH_DELAY_MS);
     btstack_run_loop_add_timer(ts);
 }
 
 // Função principal
 int main() {
     stdio_init_all();
 
     // Inicializa a arquitetura do driver CYW43 (habilitará BT se CYW43_ENABLE_BLUETOOTH == 1)
     if (cyw43_arch_init()) {
         printf("Falha ao inicializar cyw43_arch\n");
         return -1;
     }
 
     l2cap_init();
     sm_init();
     sm_set_io_capabilities(IO_CAPABILITY_NO_INPUT_NO_OUTPUT);
 
     // Configura o servidor ATT vazio - necessário apenas se o periférico LE fizer consultas ATT por conta própria
     att_server_init(NULL, NULL, NULL);
 
     gatt_client_init();
 
     hci_event_callback_registration.callback = &hci_event_handler;
     hci_add_event_handler(&hci_event_callback_registration);
 
     // Configura o timer do BTstack
     heartbeat.process = &heartbeat_handler;
     btstack_run_loop_set_timer(&heartbeat, LED_SLOW_FLASH_DELAY_MS);
     btstack_run_loop_add_timer(&heartbeat);
 
     // Liga o Bluetooth
     hci_power_control(HCI_POWER_ON);
 
     // btstack_run_loop_execute é necessário apenas ao usar o método 'polling'
     // Este exemplo usa o método 'threadsafe background', onde o trabalho do BT é tratado em uma IRQ de baixa prioridade
     // Portanto, é seguro chamar bt_stack_run_loop_execute(), mas você também pode continuar executando código do usuário.
 
 #if 1 // Isso é necessário apenas ao usar polling (o que não estamos, mas mostramos que é seguro chamar neste caso)
     btstack_run_loop_execute();
 #else
     // Este núcleo está livre para fazer suas próprias coisas, exceto ao usar o método 'polling'
     // (nesse caso, você deve usar os métodos btstacK_run_loop_ para adicionar trabalho ao run loop).
 
     // Este é um loop infinito no lugar onde o código do usuário iria.
     while (true) {
         sleep_ms(1000);
     }
 #endif
     return 0;
 }