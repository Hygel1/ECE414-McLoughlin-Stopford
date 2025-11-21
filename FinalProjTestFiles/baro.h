#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include "hardware/uart.h"
#include "libraries/BMP5_SensorAPI-master/bmp5.h"
#include "libraries/BMP5_SensorAPI-master/bmp5_defs.h"

#define BMP5_I2C_ADDR 0x47

int8_t bmp5_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr);
int8_t bmp5_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr);
void delay_us(uint32_t period, void *intf_ptr);
void initBaro();
void readBaro();