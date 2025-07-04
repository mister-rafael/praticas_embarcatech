#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

// Define os pinos GPIO que serão usados para SDA e SCL do sensor MPU_6050
static const uint I2C_SDA_MPU6050 = 0; // Pino SDA
static const uint I2C_SCL_MPU6050 = 1; // Pino SCL
static const uint I2C_BAUDRATE_MPU6050 = 100 * 1000; // 100KHZ

// Endereço I2C do sensor AHT10
const uint8_t MPU6050_ADDR = 0x68;

///#define MPU6050_ADDR 0x68

#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_GYRO_XOUT_H 0x43



// --- Protótipos de Funções ---
void mpu6050_init();
void mpu6050_read_raw_data(int16_t accel[3], int16_t gyro[3]);

// --- Implementação das Funções ---

void mpu6050_init() {
    i2c_init(i2c0, I2C_BAUDRATE_MPU6050);
    gpio_set_function(I2C_SDA_MPU6050, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_MPU6050, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_MPU6050);
    gpio_pull_up(I2C_SCL_MPU6050);

    printf("I2C0 para MPU6050 configurado.\n");
    sleep_ms(100);

    uint8_t buf[2];
    buf[0] = MPU6050_PWR_MGMT_1;
    buf[1] = 0x00;
    
    int ret = i2c_write_blocking(i2c0, MPU6050_ADDR, buf, 2, false);
    if (ret == PICO_ERROR_GENERIC) {
        printf("Erro ao acordar MPU6050! Verifique conexoes e endereco I2C.\n");
    } else {
        printf("MPU6050 acordado e inicializado com sucesso.\n");
    }
    sleep_ms(100);
}

void mpu6050_read_raw_data(int16_t accel[3], int16_t gyro[3]) {
    uint8_t buffer[14];

    uint8_t reg_addr = MPU6050_ACCEL_XOUT_H;
    int ret = i2c_write_blocking(i2c0, MPU6050_ADDR, &reg_addr, 1, true);
    if (ret == PICO_ERROR_GENERIC) {
        printf("Erro ao solicitar leitura de dados do MPU6050.\n");
        return;
    }
    
    ret = i2c_read_blocking(i2c0, MPU6050_ADDR, buffer, 14, false);
    if (ret == PICO_ERROR_GENERIC) {
        printf("Erro ao ler dados do MPU6050.\n");
        return;
    }

    accel[0] = (int16_t)((buffer[0] << 8) | buffer[1]);
    accel[1] = (int16_t)((buffer[2] << 8) | buffer[3]);
    accel[2] = (int16_t)((buffer[4] << 8) | buffer[5]);

    gyro[0] = (int16_t)((buffer[8] << 8) | buffer[9]);
    gyro[1] = (int16_t)((buffer[10] << 8) | buffer[11]);
    gyro[2] = (int16_t)((buffer[12] << 8) | buffer[13]);
}

// --- Função Principal ---
int main() {
    stdio_init_all();

    printf("Iniciando projeto MPU6050 no Pico...\n");

    mpu6050_init();

    int16_t accel_data[3];
    int16_t gyro_data[3];

    while (true) {
        mpu6050_read_raw_data(accel_data, gyro_data);

        printf("Accel: X=%d, Y=%d, Z=%d | Gyro: X=%d, Y=%d, Z=%d\n",
               accel_data[0], accel_data[1], accel_data[2],
               gyro_data[0], gyro_data[1], gyro_data[2]);
        
        sleep_ms(1000);
    }

    return 0;
}