#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/pwm.h"
#include <stdio.h>
#include "pwm_pin.h"
#include "getPWM.h"
#include "control.h"
#include "baro.h"
#ifndef GYRO
#include "gyro.h"
#endif

#define UART_ID uart0
#define BAUD_RATE 115200

// UART pin definitions
#define UART_TX_PIN 0
#define UART_RX_PIN 1
uint16_t inputsCorrected[6];
int main() {
    stdio_init_all();
    uint16_t outPins[] = {6, 19, 21, 16, 29, 27};// {prop(9), L_aileron(19), R_aileron(21), elevator(16), nc(29), nc(27)}
    uint16_t inPins[] = {4, 6, 8, 10, 12, 14}; //{NC(4), L_Wheel(6), L_Horiz(8), R_Vert(10), L_Vert(12), R_Horiz(14)}
    uint32_t nothing[]= {0,0,0,0,0,0};
    for(uint8_t i =0; i <6; i++) {
        pwm_pin_init(0x8000, outPins[i],i);
    }
    struct duties dutiesVals = get_duty(inPins);
    uint16_t *outStage=translate(nothing); //initial set, never used
    uint16_t *smoothOut={0, 200, 200, 200, 0, 0}; //initial smooth output, starts at neutral positions
    uint16_t input[6];
    struct Output outGyro;
    struct Vals6 accelVals; struct Vals6 accelValsTemp;
    struct Angles angles; struct Angles anglesTemp;
    initGyro();
    initBaro();
    // initINS();
    uint32_t lastTimeRead=timer_read(); //used to track acceleration for speed calculation
    uint32_t speedVal; //initial speed must be 0 on boot
    //int32_t *accelVals;
    //int32_t *angles;
    //int32_t accelValstemp[] = {0,0,0,0,0,0};
    //accelVals=accelValstemp;
    //int32_t anglestemp[] = {0,0,0};
    //angles=anglestemp;
    while(1) {
        anglesTemp=angles;
        accelValsTemp=accelVals;
        angles = updateGyroVals(lastTimeRead, anglesTemp); //[Ax,Ay,Az]
        accelVals=updateAccelVals(lastTimeRead,accelValsTemp,angles); //[Vx,Vy,Vz, Dx,Dy,Dz]
        lastTimeRead=timer_read(); //reads current time in us
        //take input values and translate according to output pin configuration
        dutiesVals = get_duty(inPins);
        

        outStage = translate(duties); //values at this point are still from 0-400
        //outStage = guiderail(outStage,angles,accelVals,0); //use guiderail function to correct values accordingly
        //at this point, outStage is the 'most aggressive' version of the user's intended maneuver
        //can probably prevent the bootup issue by omitting the smoothout on first run
        //smoothOut = smoothTransition(smoothOut,outStage); //slows down output controls to ensure that the user doesn't try to shift things too quickly
        //^^ smoothout array will always hold the latest outputted control array until this point
        setAllPWM(smoothOut,outPins); //method located in controls file

        // for(int i=0;i<6;i++) input[i]=(uint16_t)((duties[i]/4*.05+5)/100*0xffff);
        // for(uint8_t i = 0; i < 6; i++) { //output pwm values
        //     pwm_pin_set_level(input[i], outPins[i],i);
        // }

        //testing...
        //printf("\n");
        readBaro();
        outGyro = readGyro();
        //printf("Accel: X=%.3fg Y=%.3fg Z=%.3fg\n", outGyro.readOut[0], outGyro.readOut[1], outGyro.readOut[2]);
        //printf("Gyro: X=%.3f°/s Y=%.3f°/s Z=%.3f°/s\n", outGyro.readOut[3], outGyro.readOut[4], outGyro.readOut[5]);

        //printf("Roll: %u Pitch: %u yaw:%u\n", angles.vals[0],angles.vals[1],angles.vals[2]);
        //printf("LAil: %u RAil: %u Elevator: %u, Prop: %u\n",outStage[1],outStage[2],outStage[3],outStage[0]); 
        //printf("Temperature: %2.2f degrees\n", outGyro.readOut[6]);

    }
}