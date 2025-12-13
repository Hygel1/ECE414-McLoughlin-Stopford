#include "pico/stdlib.h"
#include "timer.h"
#include "stdint.h"
#include "stdbool.h"
#define CONTROL
#ifndef GYRO
#include "gyro.h"
#endif

// uint16_t output[6];
// uint16_t guiderailOutput[6];
// uint32_t timeSinceLastTransition;
// int32_t accelVals[6]; //[Vx,Vy,Vz,Dx,Dy,Dz]
// int32_t angleVals[3];

struct Angles {
    int32_t vals[3];
};

struct Vals6 {
    int32_t vals[6];
};



uint16_t *translate(uint32_t input[]);
uint16_t *guiderail(uint16_t controls[], struct Angles gyro,struct Vals6 vPos,uint8_t *recon);
uint16_t *smoothTransition(uint16_t currentState[], uint16_t desiredPoint[]);
struct Angles updateGyroVals(uint32_t lastTime, struct Angles lastVals);
struct Vals6 updateAccelVals(uint32_t lastTime,struct Vals6 lastVals, struct Angles angles);
void recon(int32_t *gyro[], int32_t *vPos[]);
void setAllPWM(uint16_t writeVals[],uint16_t outPins[]);
struct Vals6 updateINS(struct Output gyroVals);
void initINS();