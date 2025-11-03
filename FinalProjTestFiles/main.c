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


uint16_t input16;
uint16_t input17;
uint16_t input18;
uint16_t input19;
uint16_t input20;
uint16_t input21;
uint16_t input22;



//currently the default pwm pin is GPIO 14
int main() {
    stdio_init_all();
    printf("Starting");
    pwm_pin_init(0x8000);
    uint16_t pin[] = {19, 20, 21, 22, 26, 27};
    uint32_t* duties = get_duty(pin);
    while(1) {
        duties = get_duty(pin);
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

// int main() {
//     stdio_init_all();
//     uart_init(UART_ID, BAUD_RATE);
//     gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
//     gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
//     while(1) {
//         input16 = gpio_get(16);
//         input17 = gpio_get(17);
//         input18 = gpio_get(18);
//         input19 = gpio_get(19);
//         input20 = gpio_get(20);
//         input21 = gpio_get(21);
//         input22 = gpio_get(22);
//         printf("Reading input 16: %d \n", input16);
//         printf("Reading input 17: %d \n", input17);
//         printf("Reading input 18: %d \n", input18);
//         printf("Reading input 19: %d \n", input19);
//         printf("Reading input 20: %d \n", input20);
//         printf("Reading input 21: %d \n", input21);
//         printf("Reading input 22: %d \n", input22);
        
//     }
// }