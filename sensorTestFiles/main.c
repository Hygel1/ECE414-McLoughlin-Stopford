#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/uart.h"
#include "baro.h"
#include "gyro.h"

#define UART_ID uart0
#define BAUD_RATE 115200

// UART pin definitions
#define UART_TX_PIN 0
#define UART_RX_PIN 1

int main() {
    printf("Running main");
    printf("Initializing\n");
    struct Output output;
    init();
    initBaro();
    printf("Starting read\n");
    while(1) {
        readBaro();
        output = readGyro();
        printf("Accel: X=%.3fg Y=%.3fg Z=%.3fg\n", output.readOut[0], output.readOut[1], output.readOut[2]);
        printf("Gyro: X=%.3f°/s Y=%.3f°/s Z=%.3f°/s\n", output.readOut[3], output.readOut[4], output.readOut[5]);
        //printf("Temperature: %2.2f degrees\n", output.readOut[6]);
    }
}