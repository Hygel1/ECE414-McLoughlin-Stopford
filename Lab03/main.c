/* ECE414 - Lab 3 - main.c file Template.
Ideally, in your Lab3 project folder, you should have the following files to start with:
    - CMakeLists.txt
    - Lab3.c (or main.c)
    - led_out.c
    - led_out.h
    - sw_in.c
    - sw_in.h
    - debounce_sw1.c
    - debounce_sw1.h
    - debounce_sw2.c
    - debounce_sw2.h
    - timer.c
    - timer.h
    - uart.c
    - uart.h
    (The UART functionality can be embedded in the main file (#include "hardware/uart.h"), which is acceptable since it's just initialization and printf statements. Separate uart.c/uart.h files would be good practice for modularity, but isn't strictly necessary for this lab.)
    - gpio.c (to test pico's GPIO functionality if needed)
*/

#include "pico/stdlib.h"
#include "sw_in.h"
#include <stdint.h>
#include <stdlib.h>
#include "led_out.h"
#include <stdbool.h>
#include "timer.h"
#include "debounce_sw1.h"
#include "debounce_sw2.h"
#include "hardware/uart.h"
#include <stdio.h>

#define UART_ID uart0
#define BAUD_RATE 115200

// UART pin definitions
#define UART_TX_PIN 0
#define UART_RX_PIN 1

// FSM States - TODO: Define your game states
enum PONG_States
{
    // TODO: Add your states here (e.g., Init, Serve, Travel, etc.)
    Init,
    Serve,
    Travel,
    Thwack,
    Victory

} PONG_State;

// Player enum - TODO: Define which player is currently active
enum currentPlayer
{
    PlayerL,
    PlayerR
    // TODO: Define PlayerL and PlayerR
} currentPlayer;

// Global variables
uint8_t ledsStates = 0;           // 8-bit value for LED control
uint8_t ledsStatesCounter = 0;    // Counter for multi-tick actions
uint32_t t1, t2, t3, t4;          // Timers for debounce and game timing
uint32_t gameTimer = 300000;         // Game speed timer (starts at 300ms)
uint8_t numberOfRoundsPlayed = 0; // Track rounds for speed increase
uint32_t randValue;
uint8_t switchPlayer = 0;
uint8_t SW1 = 0;
uint8_t SW2 = 0;
uint8_t firstTravel = 0;
uint8_t stillHoldL=0;
uint8_t stillHoldR=0;

// Main game state machine tick function
void tick()
{
    switch (currentPlayer)
    {
    case PlayerL:
        if (switchPlayer)
            currentPlayer = PlayerR;
        break;
    case PlayerR:
        if (switchPlayer)
            currentPlayer = PlayerL;
        break;
    default:
        break;
    }
    switchPlayer = 0;
    switch (PONG_State)
    {
    case Init:
    //add light LED
        printf("init");
        ledsStatesCounter = 0;
        numberOfRoundsPlayed = 0;
        t1, t2, t3, t4 = 0;
        if (rand()*1000 % 2 == 1) //odd
        {
            currentPlayer = PlayerL;
            led_out_write(0x01);
            ledsStates = 0x01;
            printf("L Serve \n");
        }
        else
        {
            currentPlayer = PlayerR;
            led_out_write(0x80);
            ledsStates = 0x80;
            printf("R Serve \n");
        }
        PONG_State = Serve;
        // TODO: Initialize the game
        // Hints:
        // - Use rand() % 2 to randomly choose starting player
        // - Set appropriate LED (0x80 for left, 0x01 for right)
        // - Print serve message to UART
        // - Transition to Serve state
        // - Reset counters

        break;
    case Serve:
    printf("serve \n");
        if((currentPlayer == PlayerR && SW2) || (currentPlayer == PlayerL && SW1)) {
            t3 = timer_read();
            PONG_State = Travel;
            if (currentPlayer == PlayerR)
            {
                printf("travel left\n");
                ledsStates = ledsStates << 1;
                //busy_wait_ms(gameTimer);
            }
            else
            { // PlayerR
                printf("travel right\n");
                ledsStates = ledsStates >> 1;
                //busy_wait_ms(gameTimer);
            }
        }
        if(numberOfRoundsPlayed > 2) gameTimer = 200000;
        if(numberOfRoundsPlayed > 4) gameTimer = 100000;
        switchPlayer = 1;
        firstTravel = 1;
        break;

    case Travel:
    printf("travel \n");
        if (!firstTravel && ((ledsStates == 0x80 && SW1) || (ledsStates == 0x01 && SW2)))
        {
            PONG_State = Thwack;
        }
        else if (SW1)//&&!stillHoldL && timer_read()-300 > t3)
        {
            currentPlayer = PlayerL;
            PONG_State = Victory;
            printf("V1");
        }
        else if (SW2)//&&!stillHoldR&& timer_read()-300 > t3)
        {
            currentPlayer = PlayerR;
            PONG_State = Victory;
            printf("v2");
        }
        else if((ledsStates <= 0x01 && currentPlayer == PlayerL) || (ledsStates >= 0x80 && currentPlayer == PlayerR)) {
            PONG_State = Victory;
            printf("v3");
        }
        else
        {
            if (currentPlayer == PlayerL)
            {
                printf("travel left\n");
                ledsStates = ledsStates << 1;
                //busy_wait_ms(gameTimer);
            }
            else
            { // PlayerR
                printf("travel right\n");
                ledsStates = ledsStates >> 1;
                //busy_wait_ms(gameTimer);
            }
            led_out_write(ledsStates);
        }
        firstTravel = 0;
        if(!SW1) stillHoldL=0;
        if(!SW2) stillHoldR=0;
        break;

    case Thwack:
    printf("thwack \n");
        firstTravel = 1;
        switchPlayer = 1;
        if(currentPlayer==PlayerL) stillHoldL=1;
        if(currentPlayer==PlayerR) stillHoldR=1;
        PONG_State = Travel;
        break;

    case Victory:
    printf("victory\n");
        // Nonactive player wins
        if(PlayerL) {
            printf("right wins");
            led_out_write(0x80);
            busy_wait_ms(100000);
            led_out_write(0x80);
            busy_wait_ms(100000);
            led_out_write(0x80);
        } else {
            printf("left wins");
            led_out_write(0x01);
            busy_wait_ms(100000);
            led_out_write(0x01);
            busy_wait_ms(100000);
            led_out_write(0x01);
        }
        PONG_State = Init;
        break;
        // TODO: Add case for Serve state
        // Hints:
        // - Wait for serving player to press their button
        // - When pressed, switch to other player and go to Travel state

        // TODO: Add case for Travel state
        // Hints:
        // - Move LED toward current player's side
        // - Use bit shifting (<<1 or >>1) to move the LED
        // - Check if ball reached the end (counter >= 7)
        // - If player presses early, they lose (go to Victory)
        // - If ball reaches end, go to Thwack state

        // TODO: Add case for Thwack state
        // Hints:
        // - Player must hit the ball when it's at their end
        // - If they hit it, switch players and go to Travel
        // - If they miss, go to Victory state

        // TODO: Add case for Victory state
        // Hints:
        // - Flash the losing player's LED
        // - Use ledsStatesCounter to control flash timing
        // - Print loss message to UART
        // - After flashing, go back to Init
        // - Update numberOfRoundsPlayed and adjust gameTimer speed

    default:
        break;
    }
}


// Initialization function - TODO: Complete the initialization
void initializeStuff()
{
    stdio_init_all();
    uart_init(uart0, 115200);
    led_out_init();
    sw_in_init();
    debounce_sw1_init();
    debounce_sw2_init();
    timer_read();
    // TODO: Initialize all required modules
    // Hints:
    // - stdio_init_all() for UART
    // - uart_init() with proper parameters
    // - Set GPIO functions for UART pins
    // - Initialize LED and switch modules
    // - Initialize debounce modules
    // - Read initial timer values
}

int main()
{
    initializeStuff();

    // TODO: Set initial state for the state machine
    PONG_State = Init;
    uint8_t t1 = timer_read();
    uint8_t t4 = timer_read();
    while (1)
    {
        t2 = timer_read();
        if(t1-t2 >= DEBOUNCE_PD_MS) {
            debounce_sw1_tick();
            debounce_sw2_tick();
            SW2 = debounce_sw2_pressed();
            SW1 = debounce_sw1_pressed();
            t1=t2;
        }
        // if(sw_in_read1()){ //if button pressed, set true for 50ms
        //     SW1=0x01;
        //     uint32_t startTime=timer_read();
        //     while(1){ //if pushed, stall for 50ms
        //         if(timer_read()-startTime>50) break;
        //     }
        // }
        // if(sw_in_read2()){ //if button pressed, set true for 50ms
        //     SW2=0x01;
        //     uint32_t startTime=timer_read();
        //     while(1){ //if pushed, stall for 50ms
        //         if(timer_read()-startTime>50) break;
        //     }
        if(t4-t2 >= gameTimer) {
            tick();
            t4=t2;
            printf("LedsStates %d", ledsStates);
        }
        // printf("SW1 ");
        // printf(SW1);
        // printf("\n SW2 ");
        // printf(SW2);
        // printf("\n");
        
        }
        
        //SW1=0x00;SW2=0x00;

        // Button Debounce Code - TODO: Implement debounce timing
        // Hints:
        // - Read current time with timer_read()
        // - Check if DEBOUNCE_PD_MS has elapsed
        // - If so, call debounce tick functions

        // Game State Machine Timing - TODO: Implement game timing
        // Hints:
        // - Read current time
        // - Check if gameTimer milliseconds have elapsed
        // - If so, call tick() function
        // - Update timer reference
    }

/*
 * IMPLEMENTATION HINTS AND REQUIREMENTS:
 *
 * 1. State Machine Design:
 *    - Init: Random player assignment and setup
 *    - Serve: Wait for serving player button press
 *    - Travel: Move ball across LEDs
 *    - Thwack: Handle ball hitting at player's end
 *    - Victory: Flash winner's LED and restart
 *
 * 2. LED Control:
 *    - Use bit patterns: 0x80 = leftmost, 0x01 = rightmost
 *    - Use bit shifting to move the ball: <<1 (left), >>1 (right)
 *    - led_out_write() outputs the pattern to LEDs
 *
 * 3. Timing Requirements:
 *    - Initial game speed: 300ms
 *    - After 3 rounds: 200ms
 *    - After 5 rounds: 100ms
 *    - Debounce timing: DEBOUNCE_PD_MS (25ms)
 *
 * 4. Player Logic:
 *    - PlayerL uses debounce_sw1_pressed()
 *    - PlayerR uses debounce_sw2_pressed()
 *    - Early button press = lose point
 *    - Missing the ball at end = lose point
 *
 * 5. UART Messages:
 *    - "Left Player Serves\n" or "Right Player Serves\n"
 *    - "Left player lost\n" or "Right player lost\n"
 *    - Remove all debug printf statements in final code
 *
 * 6. Victory Animation:
 *    - Flash the WINNING side's LED at least 3 times (per requirement G5)
 *    - Use ledsStatesCounter to control timing
 *    - Alternate between LED on and off states
 */

// Template prepared by Prof. Priyank Kalgaonkar.