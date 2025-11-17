#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include "hardware/uart.h"
#define I2C_PORT i2c0
#define SDA_PIN  16
#define SCL_PIN  17

#define id 
#define I2C_Add 0x47
#define reg_id 0x01
#define reg_tempLSB 0x1E
#define reg_tempMSB 0x1F

#define reg_pressLSB    0x21
#define reg_pressMSB    0x22



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

int16_t concatenate(uint8_t low, uint8_t high) {
    return (int16_t)((high << 8) | low);
}

int main() {
    stdio_init_all();
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    sleep_ms(100);


}
