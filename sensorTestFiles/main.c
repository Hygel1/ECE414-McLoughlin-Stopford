#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/uart.h"
#include "baro.h"

#define UART_ID uart0
#define BAUD_RATE 115200

// UART pin definitions
#define UART_TX_PIN 0
#define UART_RX_PIN 1

int main() {
    printf("Running main");
    printf("Initializing\n");
    initBaro();
    printf("Starting read\n");
    while(1) {
        readBaro();
    }
}