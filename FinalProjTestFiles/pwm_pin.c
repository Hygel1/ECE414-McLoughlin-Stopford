/*
 * pwm-pin - configure GPIO24 as a PWM output
 * and provide a function to change the
 * duty cycle
 *
 * ECE 414 - Lafayette College
 * J. Nestor - July 2023
 * 
 * */

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "pwm_pin.h"

static uint slice_num[6];

// Initialize pin 25 as a basic PWM output
// By setting the pwm clock divider to divide 125MHz/1.9375
// we will count at 63.23 MHz (15.8ns) 
// By seetting the wrap at 0xfff output waveform will have
// a frequency of 965.65 Hz (1.035ms)
void pwm_pin_init(uint16_t level, uint16_t outPin,uint16_t i) {
    gpio_set_function(outPin, GPIO_FUNC_PWM);
    slice_num[i] = pwm_gpio_to_slice_num(outPin);
    pwm_set_clkdiv(slice_num[i], 1.9375*20);  
    pwm_set_wrap(slice_num[i], 0xffff);
    //Odd GPIO pins are on channel B, evens on on channel A
    if((outPin%2==0)){
    pwm_set_chan_level(slice_num[i], PWM_CHAN_A, level);
    } else {
    pwm_set_chan_level(slice_num[i], PWM_CHAN_B, level);
    }
    pwm_set_enabled(slice_num[i], true);
}

// Set the level where output changes from 1 to 0
void pwm_pin_set_level(uint16_t level, uint16_t outPin,uint16_t i){
    //slice_num = pwm_gpio_to_slice_num(outPin);
    if((outPin%2==0)){
    pwm_set_chan_level(slice_num[i], PWM_CHAN_A, level);
    } else {
    pwm_set_chan_level(slice_num[i], PWM_CHAN_B, level);
    }
}
