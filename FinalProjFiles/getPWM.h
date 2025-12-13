#include "pico/stdlib.h"
#include "timer.h"
#include "stdint.h"
#include "stdbool.h"

struct duties{
    unt32_t on[6];
}
uint32_t * get_duty(uint16_t pin[]);
uint32_t * get_duty2(uint16_t pin[]);