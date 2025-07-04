#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "ssd1306.h"
#include "hardware/i2c.h"

// Configuração I2C para Display (integrado)
const uint I2C_SDA_DISPLAY = 14;
const uint I2C_SCL_DISPLAY = 15;

// Configuração I2C para Sensor AHT10
const uint I2C_SDA_SENSOR = 0;
const uint I2C_SCL_SENSOR = 1;
#define AHT10_ADDR 0x38

// Funções para o sensor AHT10
bool aht10_init(i2c_inst_t *i2c) {
    uint8_t init_cmd[3] = {0xE1, 0x08, 0x00};
    return i2c_write_blocking(i2c, AHT10_ADDR, init_cmd, 3, false) == 3;
}
// Função para ler temperatura e umidade do AHT10
bool aht10_read(i2c_inst_t *i2c, float *temp, float *hum) {
    uint8_t cmd[3] = {0xAC, 0x33, 0x00}; // Comando para iniciar a leitura
    uint8_t data[6]; // Buffer para armazenar os dados lidos.
    
    if (i2c_write_blocking(i2c, AHT10_ADDR, cmd, 3, false) != 3) {
        return false;
    }

    absolute_time_t timeout = make_timeout_time_ms(80);
    uint8_t status;
    do {
        sleep_ms(10);
        if (i2c_read_blocking(i2c, AHT10_ADDR, &status, 1, false) != 1) {
            return false;
        }
    } while ((status & 0x80) && !time_reached(timeout));

    if (i2c_read_blocking(i2c, AHT10_ADDR, data, 6, false) != 6) {
        return false;
    }

    uint32_t raw_hum = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
    uint32_t raw_temp = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];
    
    *hum = (raw_hum * 100.0f) / (1 << 20);
    *temp = (raw_temp * 200.0f) / (1 << 20) - 50.0f;
    
    return true;
}

int main() {
    stdio_init_all();
    
    // Inicializa I2C para Display
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA_DISPLAY, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_DISPLAY, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_DISPLAY);
    gpio_pull_up(I2C_SCL_DISPLAY);
    ssd1306_init();

    // Inicializa I2C para Sensor
    i2c_init(i2c0, 100000); // 100 kHz
    gpio_set_function(I2C_SDA_SENSOR, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_SENSOR, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_SENSOR);
    gpio_pull_up(I2C_SCL_SENSOR);

    if (!aht10_init(i2c0)) {
        printf("Erro ao inicializar AHT10!\n");
    }

    // Área de renderização
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);
    uint8_t buffer[ssd1306_buffer_length];
    
    float temperature, humidity;
    char temp_str[20], hum_str[20];
    
    while(true) {
        memset(buffer, 0, sizeof(buffer));
        
        if(aht10_read(i2c0, &temperature, &humidity)) {
            snprintf(temp_str, sizeof(temp_str), "Temp: %.1f C°", temperature);
            snprintf(hum_str, sizeof(hum_str), "Umidade: %.1f%%", humidity);
            
            ssd1306_draw_string(buffer, 0, 0, temp_str);
            ssd1306_draw_string(buffer, 0, 8, hum_str);

            // Verifica condições de alerta
            if(humidity > 70.0 || temperature < 20.0) {
                ssd1306_draw_string(buffer, 0, 16, "ALERTA!");
                ssd1306_draw_string(buffer, 0, 24, "Cond adversas!");
            }
        } else {
            ssd1306_draw_string(buffer, 0, 0, "Erro leitura!");
        }
        
        render_on_display(buffer, &frame_area);
        sleep_ms(2000);
    }
    return 0;
}