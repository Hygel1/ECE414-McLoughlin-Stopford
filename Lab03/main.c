// Corrected main.c file by Prof. Kalgaonkar.

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

// FSM States
enum PONG_States
{
    Init,
    Serve,
    Travel,
    Thwack,
    Victory
} PONG_State;

// Player enum
enum currentPlayer
{
    PlayerL,
    PlayerR
} currentPlayer;

// Global variables
uint8_t ledsStates = 0;           // 8-bit value for LED control
uint8_t ledsStatesCounter = 0;    // Counter for multi-tick actions
uint32_t lastDebounceTime = 0;    // Timers for debounce
uint32_t lastGameTime = 0;        // Timer for game timing
uint32_t gameDelay = 300;         // Game speed in ms (starts at 300ms)
uint8_t numberOfRoundsPlayed = 0; // Track rounds for speed increase
bool SW1 = false;
bool SW2 = false;
bool firstTravel = true;
bool buttonPressedThisCycle = false;

// Main game state machine tick function
void tick()
{
    buttonPressedThisCycle = false;
    
    // Handle player switching
    switch (PONG_State)
    {
    case Init:
        printf("Game Initializing\n");
        ledsStatesCounter = 0;
        numberOfRoundsPlayed = 0;
        gameDelay = 300; // Reset to initial speed
        
        // Randomly choose starting player
        if (rand() % 2 == 1) {
            currentPlayer = PlayerL;
            ledsStates = 0x80; // Leftmost LED
            printf("L Serve\n");
        } else {
            currentPlayer = PlayerR;
            ledsStates = 0x01; // Rightmost LED
            printf("R Serve\n");
        }
        led_out_write(ledsStates);
        PONG_State = Serve;
        break;
        
    case Serve:
        printf("Serve State - Waiting for player %s\n", (currentPlayer == PlayerL) ? "L" : "R");
        
        // Check if serving player pressed their button
        if ((currentPlayer == PlayerL && SW1) || (currentPlayer == PlayerR && SW2)) {
            printf("Serve accepted - ball moving toward opponent\n");
            PONG_State = Travel;
            firstTravel = true;
            buttonPressedThisCycle = true;
        }
        break;
        
    case Travel:
        printf("Travel State - Ball moving, LED pattern: 0x%02X\n", ledsStates);
        
        // Check for valid hit when ball reaches opponent's end
        if (!firstTravel) {
            if ((ledsStates == 0x80 && SW1) || (ledsStates == 0x01 && SW2)) {
                // Valid hit - reverse direction
                printf("Valid hit - reversing direction\n");
                PONG_State = Thwack;
                numberOfRoundsPlayed++;
                buttonPressedThisCycle = true;
                
                // Increase speed after certain number of rounds
                if (numberOfRoundsPlayed > 2) gameDelay = 200;
                if (numberOfRoundsPlayed > 4) gameDelay = 100;
                break;
            }
        }
        
        // Check for miss (wrong button press)
        if ((SW1 && ledsStates != 0x80) || (SW2 && ledsStates != 0x01)) {
            // Wrong button press - point goes to other player
            printf("Miss detected - wrong button press\n");
            if(SW1){
                currentPlayer = PlayerR;
            }else{
                currentPlayer = PlayerL;
            }
            //currentPlayer = (currentPlayer == PlayerR) ? PlayerR : PlayerL;
            PONG_State = Victory;
            buttonPressedThisCycle = true;
            break;
        }
        
        // Move the ball
        if (currentPlayer == PlayerL) {
            // Ball moving left to right
            ledsStates = ledsStates >> 1;
            printf("Ball moving right\n");
        } else {
            // Ball moving right to left  
            ledsStates = ledsStates << 1;
            printf("Ball moving left\n");
        }
        
        // Check if ball reached the end without being hit
        if (ledsStates == 0x00) {
            // Ball reached the end without being hit
            printf("Miss detected - ball reached end\n");
            PONG_State = Victory;
            break;
        }
        
        led_out_write(ledsStates);
        firstTravel = false;
        break;
        
    case Thwack:
        printf("Thwack State - Switching players\n");
        // Switch players and continue
        currentPlayer = (currentPlayer == PlayerL) ? PlayerR : PlayerL;
        PONG_State = Travel;
        break;
        
    case Victory:
        printf("Victory State - Player %s wins!\n", (currentPlayer == PlayerL) ? "L" : "R");
        
        // Flash winning player's LED 3 times
        for (int i = 0; i < 3; i++) {
            if (currentPlayer == PlayerL) {
                led_out_write(0x80); // Left LED on
            } else {
                led_out_write(0x01); // Right LED on
            }
            busy_wait_ms(200);
            led_out_write(0x00); // All LEDs off
            busy_wait_ms(200);
        }
        
        // Reset for new game
        PONG_State = Init;
        break;
        
    default:
        PONG_State = Init;
        break;
    }
}

void initializeStuff()
{
    stdio_init_all();
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    led_out_init();
    sw_in_init();
    debounce_sw1_init();
    debounce_sw2_init();
    
    // Seed random number generator
    srand(timer_read());
}

int main()
{
    initializeStuff();
    PONG_State = Init;
    
    lastDebounceTime = timer_read();
    lastGameTime = timer_read();
    
    printf("1D Pong Game Starting...\n");
    
    while (1) {
        uint32_t currentTime = timer_read();
        
        // Handle debouncing every DEBOUNCE_PD_MS
        if (timer_elapsed_ms(lastDebounceTime, currentTime) >= DEBOUNCE_PD_MS) {
            debounce_sw1_tick();
            debounce_sw2_tick();

            // SW1 = !gpio_get(16);
            // SW2 = !gpio_get(17);
            lastDebounceTime = currentTime;
        }
            if(debounce_sw1_pressed()) {
                SW1 = 1;
            } else {
                SW1 = 0;
            }
            if(debounce_sw2_pressed()){
                 SW2 = 1;
            }else {
                SW2 = 0;
            }
          
        // Handle game state machine every gameDelay ms
        if (timer_elapsed_ms(lastGameTime, currentTime) >= gameDelay) {
            tick();
            printf("%d, %d - %d, %d - %d, %d - ", SW1, SW2, !gpio_get(16), !gpio_get(17),debounce_sw1_pressed(),debounce_sw2_pressed());
            lastGameTime = currentTime;
        }
        
    }
}