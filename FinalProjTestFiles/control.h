#include "pico/stdlib.h"
#include "timer.h"
#include "stdint.h"
#include "stdbool.h"

uint16_t output[6];
uint16_t guiderailOutput[6];
unit32_t timeSinceLastTransition;
int32_t accelVals[6]; //[Vx,Vy,Vz,Dx,Dy,Dz]
int32_t angleVals[3];

uint16_t *translate(uint32_t input[]);
uint16_t *guiderail(uint16_t controls[], int32_t *gyro[],int32_t vPos[],uint8_t *recon);
uint16_t *smoothTransition(uint16_t currentState[], uint16_t desiredPoint[]);
int32_t *updateGyroVals(unt32_t lastTime,int32_t lastVals[]);
int32_t *updateAccelVals(uint32_t lastTime,int32_t lastVals[],int32_t angles[]);
void recon(int32_t *gyro[], int32_t *vPos[]);
void setAllPWM(uint16_t writeVals[],uint16_t outPins[]);
