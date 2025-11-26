#include "pico/stdlib.h"
#include "timer.h"
#include "stdint.h"
#include "stdbool.h"
#include "control.h"
#include <math.h>

//takes in values 0-400
uint16_t output[6];
uint16_t guiderailOutput[6];
unit32_t timeSinceLastTransition;
int32_t accelVals[6]; //[Vx,Vy,Vz,Dx,Dy,Dz]
int32_t angleVals[3];
/**
 * @brief takes input and translates it to output indices
 */
uint16_t *translate(uint32_t input[]){
    output[0]=(uint16_t)input[4]; //propeller power control
    output[1]=(uint16_t)input[5]; //L-aileron control
    output[2]=(uint16_t)(400-input[5]); // R-Aileron control, inverse of left
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
uint16_t *guiderail(uint16_t controls[], int32_t *gyro[],int32_t *vPos[],uint8_t recon){
    guiderailOutput=controls;
    uint32_t maxPitch = 45;//can correct with some equation involving speed and atmospheric pressure
    uint32_t maxRoll = 80; //assuming 80 degrees is the max intended roll for turning, mostly just used to ensure that the plane doesn't end up upside down
    uint16_t aileron_fromCenter; //max value of 200, indicates how far the ailerons should move from center to fix roll angle
    //no yaw control because not an issue and also not controllable
    //also assuming that 0 values are 0 and negative values go negative
    if(gyro[2]>maxPitch){ //if nose too far up
        //move elevator to push plane down, put value to 200 (centerpoint)
        guiderailOutput[3]=200;
    }
    else if(gyro[2]<(-1*maxPitch)){ //if nose too far down
        guiderailOutput[3]= 400; //want to bring tail up so put elevetor in upward position -- drastice move to avoid freefall
        /*the smoothTransition function will likely prevent the elevetor from actually making it to max from this in any real scenario 
        other than freefall but will ensure a natural feeling guiderail that doesn't just throw the plane back up every time you hit 45
        rechecks angle every poll */
    }
    if(gyro[1]>80){ ///rolling too far right
        if(controls[1]>100) guiderailOutput[1]=100;//left aileron goes down
        if(controls[2]<300) guiderailOutput[2]=300;//right aileron goes up
    }
    else if(gyro[1]<-80){ //rolling too far left
        if(controls[1]<300) guiderailOutput[1]=300; //left aileron goes up
        if(controls[2]>100) guiderailOutput[2]=100; //right aileron goes down
    }
    //minspeed stall avoidance, not too worried about overspeed by prop
    if(vPos[0]<8){ //assuming x axis is the direction of flight, measured in m/s
        if(controls[0]<300) guiderailOutput[0]=300;
    }
    //range control
    uint32_t totalDistance = sqrt(vPos[3]*vPos[3]+vPos[4]*vPos[4]+vPos[5]*vPos[5]);
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
    //TODO: WRITE THIS WHOLE THING BC I DONT WANNA
}
/**
 * ensures that plane doesn't attempt to move too quickly and break out of the envelope of control
*/
uint16_t *smoothTransition(uint16_t currentState[], uint16_t desiredPoint[]){
    uint32_t timeNow=timer_read();
    uint32_t timeElapsed=timeNow-timeSinceLastTransition; //time elapsed since last run used for movement allowance calculation
    timeSinceLastTransition=timeNow; //set time value for next operation
    //setting a full 0-400 to 1 second, that means that every single digit move should take 1/400 of a second, will let the error work in the favor of the faster move
    uint16_t maxMove=(timeElapsed/2500); //10^6 (one second) / 400 (the alloted movement for one second) = 2500
    for(int i=0;i<6;i++){ //for all input points
        if(desiredPoint[i]>currentState[i]){ //if currentState should move in the positive direction
            if(desiredPoint[i]-currentState[i]>maxMove) guiderailOutput[i]=currentState[i]+maxMove; //if asking to move beyond the max, limit
            else guiderailOutput[i]=desiredPoint[i]; //otherwise, just put to desired point
        }
        else{ //if currentState should move in the negative direction
            if(currentState[i]-desiredPoint[i]>maxMove) guiderailOutput[i]=currentState[i]-maxMove;
            else guiderailOutput[i]=desiredPoint[i];
        }
    }
    return guiderailOutput; //using same guiderail output for this method since no memory between function is required for either method
}
// [roll, pitch, yaw]
int32_t *updateGyroVals(unt32_t lastTime,int32_t lastVals[]){
    float gyroHold=readGyroVals();
    for(int i=0;i<3;i++){
        uint32_t timeDiff=(timerRead()-lastTime)*1000000;
        angleVals[i]=lastVals[i]+gyroHold[i]/9.81*(timeDiff*timeDiff);
    }
    return angleVals;
}
// [Vx,Vy,Vz,Dx,Dy,Dz]
int32_t *updateAccelVals(uint32_t lastTime,int32_t lastVals[],int32_t angles[]){
    //take accel value and time interval since last read and use to estimate speed change since last read
    float accelHold[]=readAccel(); //accelerometer reads in g
    uint32_t timeInterval=(timerRead()-lastTime)*1000000;
    int32_t interm[6];
    for(int i=0;i<3;i++){ //0-2 speed values, 3-5 position values
        //these equations should be updated to consider the roll and pitch angles of the plane
        interm[i]=lastVals[i]+accelHold[i]/9.81*(timeInterval); //can convert from m/s to mph or something
        interm[i+3]=lastVals[i+3]+accelHold[i]/9.81*(timeInterval*timeInterval); //percieved distance calc
    } //theta=roll=angles[0], alpha=pitch=angles[1], beta=yaw=angles[2]  - accelHold=deltaVals
    //modify 
    accelVals[0]=lastVals[0]+interm[0]*(cos(angles[1])+cos(angles[2]));
    accelVals[3]=lastVals[3]+interm[3]*(cos(angles[1])+cos(angles[2]));

    accelVals[1]=lastVals[1]+interm[0]*sin(angles[2])+interm[1]*cos(angles[0])+interm[2]*cos(3.14159/2-angles[0]);
    accelVals[4]=lastVals[4]+interm[3]*sin(angles[2])+interm[4]*cos(angles[0])+interm[5]*cos(3.14159/2-angles[0]);
    
    accelVals[2]=lastVals[2]+interm[0]*sin(angles[1])+interm[1]*sin(angles[0])+interm[2]*sin(angles(3.14159/2-angles[0]));
    accelVals[5]=lastVals[5]+interm[3]*sin(angles[1])+interm[4]*sin(angles[0])+interm[5]*sin(angles(3.14159/2-angles[0]));
    return accelVals;
}
/**
 * @brief takes pwm output values in 0-400 range and translates to larger PWM duty cycle range, then outputs to all output pins
 */
void setAllPWM(uint16_t writeVals[],uint16_t outPins[]){
    //translate from 0-400 easy math values to pwm write range
    for(int i=0;i<6;i++) input[i]=(uint16_t)((writeVals[i]/4*.05+5)/100*0xffff);
    for(uint8_t i = 0; i < 6; i++) { //output pwm values
        pwm_pin_set_level(input[i], outPins[i],i);
    }
}