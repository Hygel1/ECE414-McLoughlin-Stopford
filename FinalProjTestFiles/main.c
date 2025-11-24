#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/pwm.h"
#include <stdio.h>
#include "pwm_pin.h"
#include "getPWM.h"
#include "control.h"
#include "baro.h"
#include "gyro.h"

#define UART_ID uart0
#define BAUD_RATE 115200

// UART pin definitions
#define UART_TX_PIN 0
#define UART_RX_PIN 1


int main() {
    stdio_init_all();
    uint16_t outPins[] = {15, 19, 21, 27, 29, 16};// {prop, L_aileron, R_aileron, elevator, nc, nc}
    uint16_t inPins[] = {4, 6, 8, 10, 12, 14}; //{NC, L_Wheel, L_Horiz, R_Vert, L_Vert, R, Horiz, NC}
    uint32_t nothing[]= {0,0,0,0,0,0};
    for(uint8_t i =0; i <6; i++) {
        pwm_pin_init(0x8000, outPins[i],i);
    }
    uint32_t *duties = get_duty(inPins);
    uint16_t *outStage=translate(nothing); //initial set, never used
    uint16_t input[6];
        struct Output outGyro;
    init();
    initBaro();
    while(1) {
        duties = get_duty(inPins);
        outStage = translate(duties);
        for(int i=0;i<6;i++) input[i]=(uint16_t)((outStage[i]/4*.05+5)/100*0xffff); //set input values from read pwm signal
        for(uint8_t i = 0; i < 6; i++) {
            pwm_pin_set_level(input[i], outPins[i],i);
        }
        //pwm_pin_set_level(input[0], outPins[4]);
        for(int i = 0; i < 6; i++) {
            printf("%d, ", outStage[i]);
        }
        printf("\n");
        // for(int i = 0; i < 6; i++) {
        //     printf("%d, ", input2[i]);
        // }
        printf("\n");
        readBaro();
        outGyro = readGyro();
        printf("Accel: X=%.3fg Y=%.3fg Z=%.3fg\n", outGyro.readOut[0], outGyro.readOut[1], outGyro.readOut[2]);
        printf("Gyro: X=%.3f°/s Y=%.3f°/s Z=%.3f°/s\n", outGyro.readOut[3], outGyro.readOut[4], outGyro.readOut[5]);
        //printf("Temperature: %2.2f degrees\n", output.readOut[6]);
    }
}