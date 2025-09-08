#ifndef LED_OUT_H
#define LED_OUT_H

#include "pico/stdlib.h"
#include <stdint.h>

void led_out_init();
void led_OUT_write(uint8_t value);

#endif