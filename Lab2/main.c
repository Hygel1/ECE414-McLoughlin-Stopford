#include "pico/stdlib.h"
#include "sw_in.h"
#include "led_out.h"

int main() {
    uint8_t led_pattern;
    sw_in_init();
    led_out_init();

    while(true) {

        if(sw_in_read1() && sw_in_read2()) {
            led_pattern = 0xFF;
        } else if(sw_in_read1()) {
            led_pattern = 0x80;
        } else if(sw_in_read2()) {
            led_pattern = 0x01;
        } else {
            led_pattern = 0x00;
        }

        led_out_write(led_pattern);

        sleep_ms(10);
    }
}