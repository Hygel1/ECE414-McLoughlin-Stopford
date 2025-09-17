/* 
 * File:   debounce_sw2.c
 * Author: nestorj
 *
 * Debounce a switch read by the sw_in module
 */

#include "stdio.h"
#include "pico/stdlib.h"
#include "debounce_sw2.h"
#include "sw_in.h"

// flag indicating button pressed and debounced
// cleared when read by debounce1_pressed()
static bool btn_pressed;  

// state variable

static enum DB_States {NOPUSH, MAYBEPUSH, PUSHED, MAYBENOPUSH} DB_State;
uint32_t startTime;
void debounce_sw2_init() {
    DB_State = NOPUSH;
    btn_pressed = false;
}

void debounce_sw2_tick() {
    bool btn = sw_in_read1();
    switch(DB_State) {
        case NOPUSH:
            if (btn) DB_State = MAYBEPUSH;
     
                btn_pressed = true;
                DB_State = PUSHED;
            }
            else DB_State = NOPUSH;
            break;
        case PUSHED:
        btn_pressed=true
            
            startTime=timer_read();
            while(1){ //if pushed, stall for 50ms
                if(timer_read()-startTime>50) break;
            }
            DB_State=MAYBENOPUSH; //sets up for immediate recheck for push
            //if (btn) DB_State = PUSHED;
            //else DB_State = MAYBENOPUSH;
            //break;
        case MAYBENOPUSH:
            if (btn) DB_State = PUSHED;
            else DB_State = NOPUSH;
            break;
        default:
            DB_State = NOPUSH;
            break;
    }

    // note: no other state actions required, 
    // so we don't need a 2nd switch statement

// return TRUE the first time the function is called after the button has 
// been pressed.  Return FALSE until the button is released and pressed again
bool debounce_sw2_pressed() {
    if (btn_pressed) {
        btn_pressed = false; 
        return true;
    } else return false;
}



