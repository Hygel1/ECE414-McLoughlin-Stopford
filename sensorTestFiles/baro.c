#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include "hardware/uart.h"
#include "BMP5_SensorAPI-master/bmp5.h"
#include "BMP5_SensorAPI-master/bmp5_defs.h"
#include "baro.h"

#define BMP5_I2C_ADDR 0x47
#define I2C_PORT i2c0
#define SDA_PIN  16
#define SCL_PIN  17

int8_t rslt;
struct bmp5_sensor_data data;
struct bmp5_osr_odr_press_config config;
struct bmp5_dev dev;

int8_t bmp5_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr)
{
    uint8_t dev_addr = *(uint8_t *)intf_ptr;

    int ret = i2c_write_blocking(i2c0, dev_addr, &reg_addr, 1, true);
    if (ret < 0) return BMP5_E_COM_FAIL;

    ret = i2c_read_blocking(i2c0, dev_addr, data, len, false);
    if (ret < 0) return BMP5_E_COM_FAIL;

    return BMP5_OK;
}


int8_t bmp5_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr)
{
    uint8_t dev_addr = *(uint8_t *)intf_ptr;
    uint8_t buf[len + 1];

    buf[0] = reg_addr;
    memcpy(&buf[1], data, len);

    int ret = i2c_write_blocking(i2c0, dev_addr, buf, len + 1, false);

    return (ret < 0) ? BMP5_E_COM_FAIL : BMP5_OK;
}

void delay_us(uint32_t period, void *intf_ptr) {
    (void)intf_ptr;
    busy_wait_us(period);
}

void initBaro() {
    stdio_init_all();
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
    uint8_t dev_addr = BMP5_I2C_ADDR;
    dev.intf = BMP5_I2C_INTF;
    dev.intf_ptr = &dev_addr;

    dev.read = bmp5_i2c_read;
    dev.write = bmp5_i2c_write;

    dev.delay_us = delay_us;

    // --- Initialize sensor ---
    bmp5_init(&dev);

    // --- Configure oversampling ---
    config.odr = BMP5_ODR_25_HZ;
    config.press_en = BMP5_ENABLE;
    config.osr_p = BMP5_OVERSAMPLING_4X;
    config.osr_t  = BMP5_OVERSAMPLING_4X;

    bmp5_set_osr_odr_press_config(&config, &dev);
    int8_t rslt = bmp5_init(&dev);
    if(rslt != BMP5_OK) {
        printf("BMP5 init failed! Code: %d\n", rslt);
        while(1) {
            sleep_ms(250);
        }

    } else {
        printf("Detected, Chip ID = 0x%02X\n", dev.chip_id);
    }
}

void readBaro(){
        bmp5_get_sensor_data(&data, &config, &dev);

        printf("Temp = %.2f °C  Pressure = %.2f Pa\n",
            data.temperature,
            data.pressure);

        sleep_ms(250);
}