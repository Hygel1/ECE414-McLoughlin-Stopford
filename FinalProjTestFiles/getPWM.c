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

uint32_t *on[6] = malloc(24) //6*32/8=24 bytes to be allocated, won't be deallocated until manually done
//malloc to ensure it doesn't dereference; outside of method so we update instead of recreate the same storage on every run
//calling this function will take just over 20kus=.02s, this should be taken into account when used
uint32_t* get_duty(uint32_t pin[]) {
    uint32_t t1, t2, t3, t4;
    const uint32_t period_us = 20000; //full PWM cycle, takes final, wrapped average - doesn't really matter where in the duty cycle it starts
    const uint32_t readPeriod = 10; //time between sampling is .01ms
    t1 = t3 = t4 = timer_read();
    while (true) {
        t2 = t4 = timer_read();
        if(timer_elapsed_us(t1, t2) >= readPeriod) {
            t1 = t2 = timer_read(); //reset count, log time before runtime
            if(gpio_get(pin[0]) == 1) on[0]++;
            if(gpio_get(pin[1]) == 1) on[1]++;
            if(gpio_get(pin[2]) == 1) on[2]++;
            if(gpio_get(pin[3]) == 1) on[3]++;
            if(gpio_get(pin[4]) == 1) on[4]++;
            if(gpio_get(pin[5]) == 1) on[5]++;
        }
        if (timer_elapsed_us(t3, t4) >= period_us) {
            printf("On = %d \n", on);
            //percent = on-100;
            for(int i=0;i<6;i++) on[i]=on[i]-100; //returns 100-200 value, subtract 100 to normalize to percent value
            return on; //returns a number array of values 0-100 
            //need to ensure all of these values are normalized to 0-100 values
            }
    }
}

// void assign_pwm(int pin, int percent) {
//     uint32_t pwm_level = percent*0xffff;
    
//     pwm_pin_set_level(pwm_level)
    
    
// }