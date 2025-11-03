#include "pico/stdlib.h"
#include "timer.h"
#include "stdint.h"
#include "stdbool.h"
#include "getPWM.h"

#define UART_ID uart0
#define BAUD_RATE 115200

// UART pin definitions
#define UART_TX_PIN 0
#define UART_RX_PIN 1


uint32_t* get_duty(uint32_t pin[]) {
    uint32_t t1, t2, t3, t4;
    const uint32_t period_us = 20000; //full PWM cycle
    const uint32_t readPeriod = 10; //time between sampling is .01ms
    static uint32_t on[6];
    t1 = t3 = t4 = timer_read();
    while (true) {
        t2 = t4 = timer_read();
        if(timer_elapsed_us(t1, t2) >= readPeriod) {
            if(gpio_get(pin[0]) == 1) on[0]++;
            if(gpio_get(pin[1]) == 1) on[1]++;
            if(gpio_get(pin[2]) == 1) on[2]++;
            if(gpio_get(pin[3]) == 1) on[3]++;
            if(gpio_get(pin[4]) == 1) on[4]++;
            if(gpio_get(pin[5]) == 1) on[5]++;
            t1 = t2 = timer_read(); //reset count
        }
        if (timer_elapsed_us(t3, t4) >= period_us) {
            printf("On = %d \n", on);
            //percent = on-100;
            for(int i=0;i<6;i++) on[i]=on[i]-100; //returns 100-200 value, subtract 100 to normalize to percent value
            return on; //returns a number array of values 0-100 
            }
    }
}

// void assign_pwm(int pin, int percent) {
//     uint32_t pwm_level = percent*0xffff;
    
//     pwm_pin_set_level(pwm_level)
    
    
// }