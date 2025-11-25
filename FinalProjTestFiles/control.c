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
    uint16_t *guiderail(uint16_t controls[], int32_t gyro[],int32_t vPos[],uint8_t recon){
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
        return guiderailOutput
    }
    /**
     * ensures that plane doesn't attempt to move too quickly and break out of the envelope of control
     */
    uint16_t *smoothTransition(uint16_t currentState[], uint16_t desiredPoint[]){
        uint32_t timeNow=timer_read();
        uint32_t timeElapsed=timeNow-timeSinceLastTransition; //time elapsed since last run used for movement allowance calculation
        timeSinceLastTransition=timeNow; //set time value for next operation
        //setting a full 0-400 to 1 second, that means that every single digit move should take 1/400 of a second, will let the error work in the favor of the faster move
        uint16_t maxMove=(timeElapsed/2500) //10^6 (one second) / 400 (the alloted movement for one second) = 2500
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
    // [Ax,Ay,Az]
    int32_t *updateGyroVals(unt32_t lastTime,int32_t lastVals[]){
        float gyroHold=readGyroVals();
        for(int i=0;i<3;i++){
            uint32_t timeDiff=(timerRead()-lastTime)*1000000;
            angleVals[i]=lastVals[i]+gyroHold[i]/9.81*(timeDiff*timeDiff);
        }
        return angleVals;
    }
    // [Vx,Vy,Vz,Dx,Dy,Dz]
    int32_t *updateAccelVals(uint32_t lastTime,int32_t lastVals[]){
        //take accel value and time interval since last read and use to estimate speed change since last read
        float accelHold=readAccel(); //accelerometer reads in g
        for(int i=0;i<3;i++){ //0-2 speed values, 3-5 position values
            accelVals[i]=lastVals[i]+accelHold[i]/9.81*((timer_read()-lastTime)*1000000); //can convert from m/s to mph or something
            accelVals[i+3]=lastVals[i+3]+accelVals[i]*((timer_read()-lastTime)*1000000);
        } //values
        return accelVals;
    }
