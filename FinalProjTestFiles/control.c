#include "pico/stdlib.h"
#include "timer.h"
#include "stdint.h"
#include "stdbool.h"
#include "control.h"
//takes in values 0-400
uint16_t output[6];
uint16_t *translate(uint32_t input[]){
    output[0]=(uint16_t)input[4]; //propeller power control
    output[1]=(uint16_t)input[5]; //L-aileron control
    output[2]=(uint16_t)(400-input[5]); // R-Aileron control, inverse of left
    output[3]=(uint16_t)input[3]; //elevator control;
    return output;
}
