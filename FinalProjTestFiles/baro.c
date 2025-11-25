#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include "hardware/uart.h"
#include "BMP5_SensorAPI-master/bmp5.h"
#include "BMP5_SensorAPI-master/bmp5_defs.h"
#include "baro.h"
#include <string.h>
#include <math.h>

#define BMP5_I2C_ADDR 0x47  // correct for your board (0x46/0x47 are the two options)
#define I2C_PORT i2c0
#define SDA_PIN  16
#define SCL_PIN  17

int8_t rslt;
struct bmp5_sensor_data data;
struct bmp5_osr_odr_press_config config = {0};
struct bmp5_dev dev;
static uint8_t dev_addrGlobal = BMP5_I2C_ADDR;

/* RP2040 I2C callbacks expected by this BMP5 driver */
int8_t bmp5_i2c_read(uint8_t reg_addr, uint8_t *data_buf, uint32_t len, void *intf_ptr)
{
    (void) intf_ptr;
    uint8_t dev_addr = dev_addrGlobal;

    // Write register address, repeated start (no STOP)
    int wrote = i2c_write_blocking(I2C_PORT, dev_addr, &reg_addr, 1, true);
    if (wrote < 0) return BMP5_E_COM_FAIL;

    int read = i2c_read_blocking(I2C_PORT, dev_addr, data_buf, len, false);
    if (read < 0) return BMP5_E_COM_FAIL;

    return BMP5_INTF_RET_SUCCESS;
}

int8_t bmp5_i2c_write(uint8_t reg_addr, const uint8_t *data_buf, uint32_t len, void *intf_ptr)
{
    (void) intf_ptr;
    uint8_t dev_addr = dev_addrGlobal;

    uint8_t tx[len + 1];
    tx[0] = reg_addr;
    memcpy(&tx[1], data_buf, len);

    int wrote = i2c_write_blocking(I2C_PORT, dev_addr, tx, len + 1, false);
    if (wrote < 0) return BMP5_E_COM_FAIL;

    return BMP5_INTF_RET_SUCCESS;
}

void delay_us(uint32_t period, void *intf_ptr)
{
    (void) intf_ptr;
    busy_wait_us(period);
}

void initBaro() {
    stdio_init_all();

    //Init I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
    sleep_ms(300);

    //initiate device and functions
    dev.intf = BMP5_I2C_INTF;
    dev.intf_ptr = &dev_addrGlobal;
    dev.read = bmp5_i2c_read;
    dev.write = bmp5_i2c_write;
    dev.delay_us = delay_us;

    //uye multiple attempts in case its still turning on
 int attempts = 0;
    const int max_attempts = 3;
    while (attempts < max_attempts) {
        rslt = bmp5_init(&dev);
        if (rslt == BMP5_OK) break;
        printf("bmp5_init attempt %d failed: %d. Retrying...\n", attempts+1, rslt);
        attempts++;
        sleep_ms(10);
    }

    if (rslt != BMP5_OK) {
        printf("BMP5 init failed! Code: %d\n", rslt);
        while (1) {
            sleep_ms(500);
        }
    }

    printf("Detected, Chip ID = 0x%02X\n", dev.chip_id);

    bmp5_soft_reset(&dev);

    dev.delay_us(3000, dev.intf_ptr);

    //config 
    config.odr = BMP5_ODR_25_HZ;
    config.press_en = BMP5_ENABLE;
    config.osr_p = BMP5_OVERSAMPLING_4X;
    config.osr_t = BMP5_OVERSAMPLING_4X;

bmp5_set_osr_odr_press_config(&config, &dev);
bmp5_set_power_mode(BMP5_POWERMODE_NORMAL, &dev);
}

void readBaro() {
    bmp5_get_sensor_data(&data, &config, &dev);
    //This eq might be dependent on temp and not work in a climate controlled room, not too sure though
    //float alt = ((pow((101325/data.pressure), .190284)-1)*(data.temperature+273.15))/.0065;
    float alt = 145366.45*(1-pow((data.pressure/100/1013.25), .190284));
    printf("Temp = %.2f C  Pressure = %.2f Pa Altitude = %.2f\n", data.temperature, data.pressure, alt);
    sleep_ms(250);
}

uint32_t getAltitude(){
    bmp5_get_sensor_data(&data, &config, &dev);
    float alt = 145366.45*(1-pow((data.pressure/100/1013.25), .190284));
    return (uint32_t) alt;
}

uint32_t getPress(){
    bmp5_get_sensor_data(&data, &config, &dev);
    return (uint32_t) data.pressure;
}