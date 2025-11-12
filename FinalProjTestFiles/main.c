#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/pwm.h"
#include "stdio.h"
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
    for(int i = 10; i <16; i++) {
        gpio_set_function(i, GPIO_FUNC_PWM);
    }
    uint16_t outPins[] = {10, 11, 12, 13, 14, 15};
    uint16_t inPins[] = {19, 20, 21, 22, 26, 27};
    uint16_t inPins2[] = {4, 5, 6, 7, 8, 9};
    for(int i =0; i <6; i++) {
        pwm_pin_init(0x8000, outPins[i]);
        pwm_init(pwm_gpio_to_slice_num(outPins[i]),0x0000, 1); //this is probably not right https://www.raspberrypi.com/documentation/pico-sdk/hardware.html#group_hardware_pwm
    }
    uint32_t *duties = get_duty(inPins);
    uint32_t *duties2 = get_duty2(inPins2);
    uint16_t input[6];
    uint16_t input2[6];
    while(1) {
        duties = get_duty(inPins);
        duties2=get_duty2(inPins2); //inpins2 connected to output pins, should read output from outpins which would match inpins[]
        //PWM for 0-180 servoes us 50Hz
        //out[0] = duties[0]*(185)+550;
        //out[0] = (out[0]*100/20000)*(0xffff/100);
        //out[0] = (0xffff/100)*out[0]; //0xffff/100;
        //for(i=0;i<6;i++) input[i]=(uint16_t)(duties[i]*163.83); //set input values from read pwm signal
        for(int i=0;i<6;i++) input[i]=(uint16_t)(duties[i]*.002*0xFFFF); //set input values from read pwm signal
        for(int i=0;i<6;i++) input2[i]=(uint16_t)(duties2[i]*.002*0xFFFF); //set input values from read pwm signal
        for(int i = 0; i < 6; i++) {
            pwm_pin_set_level(input[0], outPins[i]);
        }
        //pwm_pin_set_level(input[0], outPins[4]);
        for(int i = 0; i < 6; i++) {
            printf("%d, ", input[i]);
        }
        for(int i = 0; i < 6; i++) {
            printf("%d, ", input2[i]);
        }
        printf("\n");


    }
}
