#include "pico/stdlib.h"
#include "timer.h"
#include "stdint.h"
#include "stdbool.h"
#ifndef CONTROL
#include "control.h"
#endif
#include <math.h>
#include "hardware/pwm.h"
#ifndef GYRO
#include "gyro.h"
#endif
#include "pwm_pin.h"

//takes in values 0-400
uint16_t output[6];
uint16_t guiderailOutput[6];
uint16_t smoothOutput[6];
struct Vals6 accelVals; //[Vx,Vy,Vz,Dx,Dy,Dz]
struct Angles angleVals;
uint64_t timeSinceLastTransition=0;
uint32_t distFromTakeoff;
uint32_t angleFromTakeoff;

uint32_t timeLastINS;

struct Vals6 dist;
struct Vals6 velocity;

void initINS() {
for(int i = 0; i < 6; i++) {
    velocity.vals[i] = 0;
    dist.vals[i] = 0;
}
}
//I guess we're making INS
/**
 * @brief updates values for onboard Inertial Navigation System
 */
struct Vals6 updateINS(struct Output gyroVals) {
    uint32_t timeNow=timer_read();
    uint32_t timeElapsed=timeNow-timeLastINS; //time elapsed since last run used for movement allowance calculation
    timeLastINS=timeNow;
    //updade
    for(int i = 0; i < 6; i++) {
        if(i < 3) { //correct that accel is in g
            if(i = 2) {//correct that z accel is 1g default
            dist.vals[i] = dist.vals[i] + velocity.vals[i]*timeElapsed + .5*(gyroVals.readOut[i]-1)*9.80665*timeElapsed;
            velocity.vals[i] = velocity.vals[i]+ ((gyroVals.readOut[i]-1))*9.80665*timeElapsed*timeElapsed;                
            } else {
            dist.vals[i] = dist.vals[i] + velocity.vals[i]*timeElapsed + .5*gyroVals.readOut[i]*9.80665*timeElapsed;
            velocity.vals[i] = velocity.vals[i]+ (gyroVals.readOut[i])*9.80665*timeElapsed*timeElapsed;
            }
        } else{
            dist.vals[i] = dist.vals[i] + velocity.vals[i]*timeElapsed + .5*gyroVals.readOut[i]*timeElapsed;
            velocity.vals[i] = velocity.vals[i]+ (gyroVals.readOut[i])*timeElapsed*timeElapsed;
        }
    }
    return dist;
}
/**
 * @brief takes input and translates it to output indices
 */
uint16_t *translate(uint32_t input[]){
    if(input[4] < 50) {
        output[0] = 0;
    } else {
    output[0]=(uint16_t)input[4]; //propeller power control
    }
    output[1]=(uint16_t)(input[5]+30); //L-aileron control
    output[2]=(uint16_t)(input[5]-10); // R-Aileron control, inverse of left
    output[3]=(uint16_t)input[3]; //elevator control;
    return output;
}
/**
 * polls barometer and gyroscope data to find all available data points and sets limitations on the plane's maneuverability
 * to prevent the plane from crashing
 * 
 * controls input values represents a fixed value returned from tranlsate function -- this means
 * that the order is [prop, l_aileron, r_aileron, elevator, NC, NC]
 * 
 * @return uint8_t* array of 'corrected' input values
 */
uint16_t *guiderail(uint16_t controls[], struct Angles gyro, struct Vals6 vPos,uint8_t *recon){
    for(int i = 0; i < 5; i++) {
        guiderailOutput[i]=controls[i];
    }
    uint32_t maxPitch = 45;//can correct with some equation involving speed and atmospheric pressure
    uint32_t maxRoll = 80; //assuming 80 degrees is the max intended roll for turning, mostly just used to ensure that the plane doesn't end up upside down
    uint16_t aileron_fromCenter; //max value of 200, indicates how far the ailerons should move from center to fix roll angle
    //no yaw control because not an issue and also not controllable
    //also assuming that 0 values are 0 and negative values go negative

    //GYRO.VALS IS ANGULAR ACCEL
    if(gyro.vals[2]>maxPitch){ //if nose too far up
        //move elevator to push plane down, put value to 200 (centerpoint)
        guiderailOutput[3]=200;
    }
    else if(gyro.vals[2]<(-1*maxPitch)){ //if nose too far down
        guiderailOutput[3]= 400; //want to bring tail up so put elevetor in upward position -- drastice move to avoid freefall
        /*the smoothTransition function will likely prevent the elevetor from actually making it to max from this in any real scenario 
        other than freefall but will ensure a natural feeling guiderail that doesn't just throw the plane back up every time you hit 45
        rechecks angle every poll */
    }
    if(gyro.vals[1]>80){ ///rolling too far right
        if(controls[1]>100) guiderailOutput[1]=100;//left aileron goes down
        if(controls[2]<300) guiderailOutput[2]=300;//right aileron goes up
    }
    else if(gyro.vals[1]<-80){ //rolling too far left
        if(controls[1]<300) guiderailOutput[1]=300; //left aileron goes up
        if(controls[2]>100) guiderailOutput[2]=100; //right aileron goes down
    }
    //minspeed stall avoidance, not too worried about overspeed by prop
    if(vPos.vals[0]<8){ //assuming x axis is the direction of flight, measured in m/s
        if(controls[0]<300) guiderailOutput[0]=300;
    }
    //range control
    uint32_t totalDistance = sqrt((vPos.vals[3])*(vPos.vals[3])+(vPos.vals[4])*(vPos.vals[4])+(vPos.vals[5])*(vPos.vals[5]));
    if(totalDistance>150&&!recon){ //assuming range of controller is around 150m -- also assuming that the initial point of collection is around the same as where the controller will remain
        //TODO: MAKE RECON FUNCTION TO RETURN BACK IN DIRECTION OF CONTROLLER WHEN FLYING OUT OF RANGE, WILL LIKELY REQUIRE PQM WRITING FROM INSIDE FUNCTION
        
    }
    return guiderailOutput;
}

/**
 * @brief in case of out of range flight, recon function takes over controls and guides plane in the direction of position 0
 * (where the plane initially started taking data points) in hopes of avoiding a controller detection issue
 * 
 */
void recon(int32_t *gyro[], int32_t *vPos[]){

    // //TODO: WRITE THIS WHOLE THING BC I DONT WANNA
    // //This is going to suck

    // //define out of range --> what's this value?
    // //its 150, see totalDistance above
    // //So first we need to find the angle from the zero point using the I HAVE NO CLUE
    // angleFromTakeoff = atan(dist.vals[1]/dist.vals[0]); //angle = arctan(y/x)
    // //turn around. how? not sure
    // //set ailerons to half deflection to turn right until Z angle has changed by 180 degrees?
    // uint32_t turning[] = {250, 400, 200, 200}; //~half power, full deflection on ailerons, 0 on elevator
    // translate()
}
/**
 * ensures that plane doesn't attempt to move too quickly and break out of the envelope of control
*/
uint16_t *smoothTransition(uint16_t currentState[], uint16_t desiredPoint[]){
    uint64_t timeNow=timer_read();
    uint64_t timeElapsed=timeNow-timeSinceLastTransition; //time elapsed since last run used for movement allowance calculation
    timeSinceLastTransition=timeNow; //set time value for next operation
    //setting a full 0-400 to 1 second, that means that every single digit move should take 1/400 of a second, will let the error work in the favor of the faster move
    uint16_t maxMove=(timeElapsed/250); //10^6 (one second) / 400 (the alloted movement for one second) = 2500
    smoothOutput[0]=desiredPoint[0]; //propeller power can change instantly
    for(int i=1;i<6;i++){ //for all input points
        if(desiredPoint[i]>395) desiredPoint[i]=395; //clamp desired point to max
        else if(desiredPoint[i]<5) desiredPoint[i]=5; //clamp desired point to min

        else if(desiredPoint[i]>currentState[i]){ //if currentState should move in the positive direction
            if(desiredPoint[i]-currentState[i]>maxMove) smoothOutput[i]=currentState[i]+maxMove; //if asking to move beyond the max, limit
            else smoothOutput[i]=desiredPoint[i]; //otherwise, just put to desired point
        }
        else{ //if currentState should move in the negative direction
            if(currentState[i]-desiredPoint[i]>maxMove) smoothOutput[i]=currentState[i]-maxMove;
            else smoothOutput[i]=desiredPoint[i];
        }
        if(smoothOutput[i]>395) smoothOutput[i]=395; //clamp output to max
        else if(smoothOutput[i]<5) smoothOutput[i]=5; //clamp output to min
    }
    return smoothOutput;
}
// [roll, pitch, yaw]
/**
 * Returns the angular distance travelled
 */
struct Angles updateGyroVals(uint32_t lastTime, struct Angles lastVals){
   // float gyroHold=readGyroVals(); // Sean WTF is readGyroVals
    struct Output gyroHold;
    gyroHold = readGyro();
    for(int i=3;i<6;i++){ //Sean I changed i so that we're getting the angle values
        uint32_t timeDiff=(timer_read()-lastTime)*1000000; //Sean, changed timerRead to timer_read, is that what you meantt?
        angleVals.vals[i]=lastVals.vals[i]+gyroHold.readOut[i]/9.81*(timeDiff*timeDiff);
    }
    return angleVals;
}
// [Vx,Vy,Vz,Dx,Dy,Dz]
struct Vals6 updateAccelVals(uint32_t lastTime,struct Vals6 lastVals, struct Angles angles){
    //take accel value and time interval since last read and use to estimate speed change since last read
    struct Output accelHold;
    accelHold = readGyro();
    //float accelHold[] = readAccel(); //accelerometer reads in g
    uint32_t timeInterval=(timer_read()-lastTime)*1000000;//Sean, changed timerRead to timer_read, is that what you meantt?
    int32_t interm[6];
    for(int i=0;i<3;i++){ //0-2 speed values, 3-5 position values
        //these equations should be updated to consider the roll and pitch angles of the plane
        interm[i]=lastVals.vals[i]+accelHold.readOut[i]/9.81*(timeInterval); //can convert from m/s to mph or something
        interm[i+3]=lastVals.vals[i+3]+accelHold.readOut[i]/9.81*(timeInterval*timeInterval); //percieved distance calc
    } //theta=roll=angles.vals[0], alpha=pitch=angles.vals[1], beta=yaw=angles.vals[2]  - accelHold=deltaVals
    //modify 

    //SEAN ARE THE COS AND SINE SUPPOSED TO USE DEGREES?
    accelVals.vals[0]=lastVals.vals[0]+interm[0]*(cos(angles.vals[1]*0.01745329)+cos(angles.vals[2]*0.01745329));
    accelVals.vals[3]=lastVals.vals[3]+interm[3]*(cos(angles.vals[1]*0.01745329)+cos(angles.vals[2]*0.01745329));

    accelVals.vals[1]=lastVals.vals[1]+interm[0]*sin(angles.vals[2])+interm[1]*cos(angles.vals[0])+interm[2]*cos(3.14159/2-angles.vals[0]*0.01745329);
    accelVals.vals[4]=lastVals.vals[4]+interm[3]*sin(angles.vals[2])+interm[4]*cos(angles.vals[0])+interm[5]*cos(3.14159/2-angles.vals[0]*0.01745329);
    
    accelVals.vals[2]=lastVals.vals[2]+interm[0]*sin(angles.vals[1])+interm[1]*sin(angles.vals[0])+interm[2]*sin(3.14159/2-angles.vals[0]*0.01745329);
    accelVals.vals[5]=lastVals.vals[5]+interm[3]*sin(angles.vals[1])+interm[4]*sin(angles.vals[0])+interm[5]*sin(3.14159/2-angles.vals[0]*0.01745329);
    return accelVals;
}
/**
 * @brief takes pwm output values in 0-400 range and translates to larger PWM duty cycle range, then outputs to all output pins
 */
void setAllPWM(uint16_t writeVals[],uint16_t outPins[]){
    //translate from 0-400 easy math values to pwm write range
    uint16_t input[6];
    for(int i=0;i<6;i++)  {
        if(writeVals[i]>395) writeVals[i]=395;
     else if(writeVals[i]<0) writeVals[i]=0;
        input[i]=(uint16_t)((writeVals[i]/4*.05+5)/100*0xffff);
    }
    for(uint8_t i = 0; i < 6; i++) { //output pwm values
        pwm_pin_set_level(input[i], outPins[i],i);
    }
}