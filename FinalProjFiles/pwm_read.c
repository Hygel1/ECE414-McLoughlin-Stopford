#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdio.h>

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

uint16_t rise1=0;
uint16_t rise2=0;
uint16_t rise3;
uint16_t rise4=0;
uint16_t rise5=0;
uint16_t rise6=0;

uint16_t time1=0;
uint16_t time2=0;
uint16_t time30;
uint16_t time4=0;
uint16_t time5=0;
uint16_t time6=0;


int main(){
  /*  while(1) {
        input16 = gpio_get(16);
        input17 = gpio_get(17);
        input18 = gpio_get(18);
        input19 = gpio_get(19);
        input20 = gpio_get(20);
        input21 = gpio_get(21);
        input22 = gpio_get(22);
    } */
    while(1){
        attachInterrupt(digitalPinToInterrupt(14),rise1,RISING);
        attachInterrupt(digitalPinToInterrupt(15),rise2,RISING);
        attachInterrupt(digitalPinToInterrupt(16),rise3,RISING);
        attachInterrupt(digitalPinToInterrupt(17),rise4,RISING);
        attachInterrupt(digitalPinToInterrupt(19),rise5,RISING);
        attachInterrupt(digitalPinToInterrupt(20),rise6,RISING);

        attachInterrupt(digitalPinToInterrupt(14),fall1,FALLING);

    }
void rise1(){

}
void rise2(){
    
}
void rise3(){
    
}
void rise4(){
    
}
void rise5(){
    
}
void rise6(){
    
}

}