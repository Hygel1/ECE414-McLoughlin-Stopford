#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include "hardware/uart.h"
#define GYRO
struct Output {
    float readOut[7];
    float linear[3];
    float angular[3];
    float temp;
};

void i2c_write_reg(uint8_t reg, uint8_t val);
uint8_t i2c_read_reg(uint8_t reg);
void i2c_read_multi(uint8_t start_reg, uint8_t *buffer, uint8_t len);
int16_t combine_bytes(uint8_t low, uint8_t high);
int primary();
bool initGyro();
struct Output readGyro();
uint16_t readTemp();
