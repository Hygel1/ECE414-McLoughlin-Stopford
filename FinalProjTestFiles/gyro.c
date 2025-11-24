#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include "hardware/uart.h"
#include "gyro.h"
#define I2C_PORT i2c0
#define SDA_PIN  16
#define SCL_PIN  17

#define LSM6DSOX_ADDR     0x6A  // or 0x6B if SA0 pulled high
#define REG_WHO_AM_I      0x0F
#define REG_CTRL1_XL      0x10
#define REG_CTRL2_G       0x11
#define REG_OUTX_L_A      0x28
#define REG_OUTX_L_G      0x22
#define REG_OUT_TEMP     0x20
#define EXPECTED_WHOAMI   0x6C

#define UART_ID uart0
#define BAUD_RATE 115200

// UART pin definitions
#define UART_TX_PIN 0
#define UART_RX_PIN 1

void i2c_write_reg(uint8_t reg, uint8_t val) {
    uint8_t buf[2] = {reg, val};
    i2c_write_blocking(I2C_PORT, LSM6DSOX_ADDR, buf, 2, false);
}

uint8_t i2c_read_reg(uint8_t reg) {
    uint8_t val;
    i2c_write_blocking(I2C_PORT, LSM6DSOX_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, LSM6DSOX_ADDR, &val, 1, false);
    return val;
}

void i2c_read_multi(uint8_t start_reg, uint8_t *buffer, uint8_t len) {
    i2c_write_blocking(I2C_PORT, LSM6DSOX_ADDR, &start_reg, 1, true);
    i2c_read_blocking(I2C_PORT, LSM6DSOX_ADDR, buffer, len, false);
}

int16_t combine_bytes(uint8_t low, uint8_t high) {
    return (int16_t)((high << 8) | low);
}
bool init(){
    
    //stdio_init_all();
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    i2c_write_reg(REG_CTRL1_XL, 0x48);
    //configure gyro: ODR = 104Hz, +/- 1000dps
    i2c_write_reg(REG_CTRL2_G, 0x48);
}
struct Output readGyro(){
        struct Output out;
        uint8_t accel[6];
        uint8_t gyro[6];
        uint8_t temp[2];

        i2c_read_multi(REG_OUTX_L_A, accel, 6);
        i2c_read_multi(REG_OUTX_L_G, gyro, 6);
        i2c_read_multi(REG_OUT_TEMP, temp, 2);

        out.readOut[0] = combine_bytes(accel[0], accel[1]) * 0.000122f;
        out.readOut[1] = combine_bytes(accel[2], accel[3]) * 0.000122f;
        out.readOut[2] = combine_bytes(accel[4], accel[5]) * 0.000122f;

        out.readOut[3]= combine_bytes(gyro[0], gyro[1])  *.030f;
        out.readOut[4] = combine_bytes(gyro[2], gyro[3]) *.030f;
        out.readOut[5]= combine_bytes(gyro[4], gyro[5])  *.030f;

        out.readOut[6] = combine_bytes(temp[0], temp[1]);

        return out;
}


//old function for operation as main file, rewrite to return relevant values in array form
int primary() {
    stdio_init_all();
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    sleep_ms(100);

    uint8_t whoami = i2c_read_reg(REG_WHO_AM_I);
    if (whoami != EXPECTED_WHOAMI) {
        printf("LSM6DSOX not detected! WHO_AM_I = 0x%02X\n", whoami);
        while (1) sleep_ms(1000);
    } else {
        printf("LSM6DSOX detected! WHO_AM_I = 0x%02X\n", whoami);
    }

    // Configure accelerometer: ODR = 104 Hz, ±4g full scale, BW = 400 Hz
    i2c_write_reg(REG_CTRL1_XL, 0x48);
    //configure gyro: ODR = 104Hz, +/- 1000dps
    i2c_write_reg(REG_CTRL2_G, 0x48);
    sleep_ms(100);

    printf("Reading accelerometer data...\n");

    while (true) {
        uint8_t accel[6];
        uint8_t gyro[6];
        uint8_t temp[2];
        i2c_read_multi(REG_OUTX_L_A, accel, 6);
        i2c_read_multi(REG_OUTX_L_G, gyro, 6);
        i2c_read_multi(REG_OUT_TEMP, temp, 2);
        int16_t x = combine_bytes(accel[0], accel[1]);
        int16_t y = combine_bytes(accel[2], accel[3]);
        int16_t z = combine_bytes(accel[4], accel[5]);

        int16_t gX = combine_bytes(gyro[0], gyro[1]);
        int16_t gY = combine_bytes(gyro[2], gyro[3]);
        int16_t gZ = combine_bytes(gyro[4], gyro[5]);

        float t = combine_bytes(temp[0], temp[1]);
        // Convert to g's (for ±4g, sensitivity = 0.122 mg/LSB)

        float accel_x = x * 0.000122f;
        float accel_y = y * 0.000122f;
        float accel_z = z * 0.000122f;

        float gyro_x = gX *.030f;
        float gyro_y = gY *.030f;
        float gyro_z = gZ *.030f;

        printf("Accel: X=%.3fg Y=%.3fg Z=%.3fg\n", accel_x, accel_y, accel_z);
        printf("Gyro: X=%.3f°/s Y=%.3f°/s Z=%.3f°/s\n", gyro_x, gyro_y, gyro_z);
        printf("Temperature: %2.2f degrees\n", t);
        sleep_ms(100);
    }
}