#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include "hardware/uart.h"
#include "baro.h"

#define UART_ID uart0
#define BAUD_RATE 115200

// UART pin definitions
#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define I2C_PORT i2c0
#define SDA_PIN  16
#define SCL_PIN  17

int main() {
    printf("Initiating\n");
    initBaro();
    printf("Starting read\n");
    while(1) {
        readBaro();
    }
}