// #include <stdio.h>
// #include "pico/stdlib.h"
// #include "hardware/i2c.h"

// // I2C address
// static const uint8_t ADXL343_ADDR = 0x6A;

// // Registers
// static const uint8_t REG_DEVID = 0x0F;
// static const uint8_t REG_POWER_CTL = 0x10;
// static const uint8_t REG_DATAX0 = 0x28;

// // Other constants
// static const uint8_t DEVID = 0x6C;
// static const float SENSITIVITY_2G = 1.0 / 256;  // (g/LSB)
// static const float EARTH_GRAVITY = 9.80665;     // Earth's gravity in [m/s^2]

// /*******************************************************************************
//  * Function Declarations
//  */
// int reg_write(i2c_inst_t *i2c, 
//                 const uint addr, 
//                 const uint8_t reg, 
//                 uint8_t *buf,
//                 const uint8_t nbytes);

// int reg_read(   i2c_inst_t *i2c,
//                 const uint addr,
//                 const uint8_t reg,
//                 uint8_t *buf,
//                 const uint8_t nbytes);

// /*******************************************************************************
//  * Function Definitions
//  */

// // Write 1 byte to the specified register
// int reg_write(  i2c_inst_t *i2c, 
//                 const uint addr, 
//                 const uint8_t reg, 
//                 uint8_t *buf,
//                 const uint8_t nbytes) {

//     int num_bytes_read = 0;
//     uint8_t msg[nbytes + 1];

//     // Check to make sure caller is sending 1 or more bytes
//     if (nbytes < 1) {
//         return 0;
//     }

//     // Append register address to front of data packet
//     msg[0] = reg;
//     for (int i = 0; i < nbytes; i++) {
//         msg[i + 1] = buf[i];
//     }

//     // Write data to register(s) over I2C
//     i2c_write_blocking(i2c, addr, msg, (nbytes + 1), false);

//     return num_bytes_read;
// }

// // Read byte(s) from specified register. If nbytes > 1, read from consecutive
// // registers.
// int reg_read(  i2c_inst_t *i2c,
//                 const uint addr,
//                 const uint8_t reg,
//                 uint8_t *buf,
//                 const uint8_t nbytes) {

//     int num_bytes_read = 0;

//     // Check to make sure caller is asking for 1 or more bytes
//     if (nbytes < 1) {
//         return 0;
//     }

//     // Read data from register(s) over I2C
//     i2c_write_blocking(i2c, addr, &reg, 1, true);
//     num_bytes_read = i2c_read_blocking(i2c, addr, buf, nbytes, false);

//     return num_bytes_read;
// }

// /*******************************************************************************
//  * Main
//  */
// int main() {

//     int16_t acc_x;
//     int16_t acc_y;
//     int16_t acc_z;
//     float acc_x_f;
//     float acc_y_f;
//     float acc_z_f;

//     // Pins
//     const uint sda_pin = 16;
//     const uint scl_pin = 17;

//     // Ports
//     i2c_inst_t *i2c = i2c0;

//     // Buffer to store raw reads
//     uint8_t data[6];

//     // Initialize chosen serial port
//     stdio_init_all();

//     //Initialize I2C port at 400 kHz
//     i2c_init(i2c, 400 * 1000);

//     // Initialize I2C pins
//     gpio_set_function(sda_pin, GPIO_FUNC_I2C);
//     gpio_set_function(scl_pin, GPIO_FUNC_I2C);

//     // Read device ID to make sure that we can communicate with the ADXL343
//     reg_read(i2c, ADXL343_ADDR, REG_DEVID, data, 1);
//     if (data[0] != DEVID) {
//         printf("ERROR: Could not communicate with ADXL343\r\n");
//         while (true);
//     }

//     // Read Power Control register
//     reg_read(i2c, ADXL343_ADDR, REG_POWER_CTL, data, 1);
//     printf("0xX\r\n", data[0]);

//     // Tell ADXL343 to start taking measurements by setting Measure bit to high
//     data[0] |= (1 << 3);
//     reg_write(i2c, ADXL343_ADDR, REG_POWER_CTL, &data[0], 1);

//     // Test: read Power Control register back to make sure Measure bit was set
//     reg_read(i2c, ADXL343_ADDR, REG_POWER_CTL, data, 1);
//     printf("0xX\r\n", data[0]);

//     // Wait before taking measurements
//     sleep_ms(2000);

//     // Loop forever
//     while (true) {

//         // Read X, Y, and Z values from registers (16 bits each)
//         reg_read(i2c, ADXL343_ADDR, REG_DATAX0, data, 6);

//         // Convert 2 bytes (little-endian) into 16-bit integer (signed)
//         acc_x = (int16_t)((data[1] << 8) | data[0]);
//         acc_y = (int16_t)((data[3] << 8) | data[2]);
//         acc_z = (int16_t)((data[5] << 8) | data[4]);

//         // Convert measurements to [m/s^2]
//         acc_x_f = acc_x * SENSITIVITY_2G * EARTH_GRAVITY;
//         acc_y_f = acc_y * SENSITIVITY_2G * EARTH_GRAVITY;
//         acc_z_f = acc_z * SENSITIVITY_2G * EARTH_GRAVITY;

//         // Print results
//         printf("X: %.2f | Y: %.2f | Z: %.2f\r\n", acc_x_f, acc_y_f, acc_z_f);

//         sleep_ms(100);
//     }
// }

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include "hardware/uart.h"
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
        int16_t x = concatenate(accel[0], accel[1]);
        int16_t y = concatenate(accel[2], accel[3]);
        int16_t z = concatenate(accel[4], accel[5]);

        int16_t gX = concatenate(gyro[0], gyro[1]);
        int16_t gY = concatenate(gyro[2], gyro[3]);
        int16_t gZ = concatenate(gyro[4], gyro[5]);

        int16_t t = concatenate(temp[0], temp[1]);
        // Convert to g's (for ±4g, sensitivity = 0.122 mg/LSB)

        float accel_x = x * 0.000122f;
        float accel_y = y * 0.000122f;
        float accel_z = z * 0.000122f;

        float gyro_x = gX *.030f;
        float gyro_y = gY *.030f;
        float gyro_z = gZ *.030f;

        printf("Accel: X=%.3fg Y=%.3fg Z=%.3fg\n", accel_x, accel_y, accel_z);
        printf("Gyro: X=%.3f°/s Y=%.3f°/s Z=%.3f°/s\n", gyro_x, gyro_y, gyro_z);
        //printf("Temperature: %2.2f degrees");
        sleep_ms(100);
    }
}