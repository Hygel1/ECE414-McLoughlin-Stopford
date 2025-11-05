#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdio.h>
#include "pwm_pin.h"
#include "getPWM.h"

#define UART_ID uart0
#define BAUD_RATE 115200

// UART pin definitions
#define UART_TX_PIN 0
#define UART_RX_PIN 1


//currently the default pwm pin is GPIO 14
int main() {
    stdio_init_all();
    printf("Starting");
    pwm_pin_init(0x8000);
    uint16_t inPins[] = {19, 20, 21, 22, 26, 27};
    uint32_t* duties = get_duty(pin);
    while(1) {
        duties = get_duty(inPins);
        //PWM for 0-180 servoes us 50Hz
        // duty19 = duty19*(185)+550;
        // duty19 = (duty19*100/20000)*(0xffff/100);
        //duty19 = (0xffff/100)*duty19; //0xffff/100;
        //pwm_pin_set_level(duty19);
        for(int i = 0; i < 6; i++) {
            printf("%d, ", duties[i]);
        }
        printf("\n");


    }
}
