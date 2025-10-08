#include "TouchDisplay.h"
#include "TFTMaster.h"
#include "ts_lcd.h"
#include <stdio.h>
// === TODO: === Define button positions (4x4 grid). Below is an example.
#define BUTTON_WIDTH 40
#define BUTTON_HEIGHT 50
#define COL1_X 20
#define COL2_X 70
#define COL3_X 120
#define COL4_X 170
#define ROW1_Y 60
#define ROW2_Y 120
#define ROW3_Y 180
#define ROW4_Y 240 
static char displayBuffer[20];
void drawCalcScreen()
{
    tft_fillScreen(ILI9340_BLACK);
    tft_setTextSize(2);

    // Display area
    tft_fillRect(10, 10, 300, 40, ILI9340_BLUE);

    // === TODO: === Draw number buttons 1-9 in 3x3 grid
    // Button 1
    tft_fillRect(COL1_X, ROW1_Y, BUTTON_WIDTH, BUTTON_HEIGHT, ILI9340_WHITE);
    tft_setCursor(COL1_X + 20, ROW1_Y + 15);
    tft_setTextColor(ILI9340_BLACK);
    tft_writeString("1");

    // === TODO: === Add buttons 2-9 following same pattern
    tft_fillRect(COL2_X, ROW1_Y, BUTTON_WIDTH, BUTTON_HEIGHT, ILI9340_WHITE);
    tft_setCursor(COL2_X + 20, ROW1_Y + 15);
    tft_setTextColor(ILI9340_BLACK);
    tft_writeString("2");

    tft_fillRect(COL3_X, ROW1_Y, BUTTON_WIDTH, BUTTON_HEIGHT, ILI9340_WHITE);
    tft_setCursor(COL3_X + 20, ROW1_Y + 15);
    tft_setTextColor(ILI9340_BLACK);
    tft_writeString("3");

    tft_fillRect(COL1_X, ROW2_Y, BUTTON_WIDTH, BUTTON_HEIGHT, ILI9340_WHITE);
    tft_setCursor(COL1_X + 20, ROW2_Y + 15);
    tft_setTextColor(ILI9340_BLACK);
    tft_writeString("4");

    tft_fillRect(COL2_X, ROW2_Y, BUTTON_WIDTH, BUTTON_HEIGHT, ILI9340_WHITE);
    tft_setCursor(COL2_X + 20, ROW2_Y + 15);
    tft_setTextColor(ILI9340_BLACK);
    tft_writeString("5");

    tft_fillRect(COL3_X, ROW2_Y, BUTTON_WIDTH, BUTTON_HEIGHT, ILI9340_WHITE);
    tft_setCursor(COL3_X + 20, ROW2_Y + 15);
    tft_setTextColor(ILI9340_BLACK);
    tft_writeString("6");

    tft_fillRect(COL1_X, ROW3_Y, BUTTON_WIDTH, BUTTON_HEIGHT, ILI9340_WHITE);
    tft_setCursor(COL1_X + 20, ROW3_Y + 15);
    tft_setTextColor(ILI9340_BLACK);
    tft_writeString("7");

    tft_fillRect(COL2_X, ROW3_Y, BUTTON_WIDTH, BUTTON_HEIGHT, ILI9340_WHITE);
    tft_setCursor(COL2_X + 20, ROW3_Y + 15);
    tft_setTextColor(ILI9340_BLACK);
    tft_writeString("8");

    tft_fillRect(COL3_X, ROW3_Y, BUTTON_WIDTH, BUTTON_HEIGHT, ILI9340_WHITE);
    tft_setCursor(COL3_X + 20, ROW3_Y + 15);
    tft_setTextColor(ILI9340_BLACK);
    tft_writeString("9");
    // Button 0, C, = in bottom row
    tft_fillRect(COL1_X, ROW4_Y, BUTTON_WIDTH, BUTTON_HEIGHT, ILI9340_WHITE);
    tft_setCursor(COL1_X + 20, ROW4_Y + 15);
    tft_setTextColor(ILI9340_BLACK);//tft_setTextColor(ILI9340_WHITE);
    tft_writeString("0");

    // === TODO: === Add C, =, and operator buttons
    tft_fillRect(COL2_X, ROW4_Y, BUTTON_WIDTH, BUTTON_HEIGHT, ILI9340_RED);
    tft_setCursor(COL2_X + 20, ROW4_Y + 15);
    tft_setTextColor(ILI9340_BLACK);
    tft_writeString("C");

    tft_fillRect(COL3_X, ROW4_Y, BUTTON_WIDTH, BUTTON_HEIGHT, ILI9340_BLUE);
    tft_setCursor(COL3_X + 20, ROW4_Y + 15);
    tft_setTextColor(ILI9340_BLACK);
    tft_writeString("=");

    tft_fillRect(COL4_X, ROW4_Y, BUTTON_WIDTH, BUTTON_HEIGHT, ILI9340_YELLOW);
    tft_setCursor(COL4_X + 20, ROW4_Y + 15);
    tft_setTextColor(ILI9340_BLACK);
    tft_writeString("/");

    tft_fillRect(COL4_X, ROW3_Y, BUTTON_WIDTH, BUTTON_HEIGHT, ILI9340_YELLOW);
    tft_setCursor(COL4_X + 20, ROW3_Y + 15);
    tft_setTextColor(ILI9340_BLACK);
    tft_writeString("x");

    tft_fillRect(COL4_X, ROW2_Y, BUTTON_WIDTH, BUTTON_HEIGHT, ILI9340_YELLOW);
    tft_setCursor(COL4_X + 20, ROW2_Y + 15);
    tft_setTextColor(ILI9340_BLACK);
    tft_writeString("-");

    tft_fillRect(COL4_X, ROW1_Y, BUTTON_WIDTH, BUTTON_HEIGHT, ILI9340_YELLOW);
    tft_setCursor(COL4_X + 20, ROW1_Y + 15);
    tft_setTextColor(ILI9340_BLACK);
    tft_writeString("+");
}
Buttons getButton(uint16_t x, uint16_t y)
{
    // === TODO: === Simple button detection
    // Check if within button 1 area
    if (x >= COL1_X && x <= COL1_X + BUTTON_WIDTH &&
        y >= ROW1_Y && y <= ROW1_Y + BUTTON_HEIGHT)
    {
        return B1;
    }

    else if (x >= COL2_X && x <= COL2_X + BUTTON_WIDTH &&
        y >= ROW1_Y && y <= ROW1_Y + BUTTON_HEIGHT)
    {
        return B2;
    }

    else if (x >= COL3_X && x <= COL3_X + BUTTON_WIDTH &&
        y >= ROW1_Y && y <= ROW1_Y + BUTTON_HEIGHT)
    {
        return B3;
    }

    else if (x >= COL1_X && x <= COL1_X + BUTTON_WIDTH &&
        y >= ROW2_Y && y <= ROW2_Y + BUTTON_HEIGHT)
    {
        return B4;
    }

    else if (x >= COL2_X && x <= COL2_X + BUTTON_WIDTH &&
        y >= ROW2_Y && y <= ROW2_Y + BUTTON_HEIGHT)
    {
        return B5;
    }

    else if (x >= COL3_X && x <= COL3_X + BUTTON_WIDTH &&
        y >= ROW2_Y && y <= ROW2_Y + BUTTON_HEIGHT)
    {
        return B6;
    }

    else if (x >= COL1_X && x <= COL1_X + BUTTON_WIDTH &&
        y >= ROW3_Y && y <= ROW3_Y + BUTTON_HEIGHT)
    {
        return B7;
    }

    else if (x >= COL2_X && x <= COL2_X + BUTTON_WIDTH &&
        y >= ROW3_Y && y <= ROW3_Y + BUTTON_HEIGHT)
    {
        return B8;
    }

    else if (x >= COL3_X && x <= COL3_X + BUTTON_WIDTH &&
        y >= ROW3_Y && y <= ROW3_Y + BUTTON_HEIGHT)
    {
        return B9;
    }
    // === TODO: === Add detection for other buttons
    //row 4
    else if (x >= COL1_X && x <= COL1_X + BUTTON_WIDTH &&
        y >= ROW4_Y && y <= ROW4_Y + BUTTON_HEIGHT)
    {
        return B0;
    }

    else if (x >= COL2_X && x <= COL2_X + BUTTON_WIDTH &&
        y >= ROW4_Y && y <= ROW4_Y + BUTTON_HEIGHT)
    {
        return BCL; //clear
    }

    else if (x >= COL3_X && x <= COL3_X + BUTTON_WIDTH &&
        y >= ROW4_Y && y <= ROW4_Y + BUTTON_HEIGHT)
    {
        return BEQ; //equals
    }

    //math operators
    else if (x >= COL4_X && x <= COL4_X + BUTTON_WIDTH &&
        y >= ROW1_Y && y <= ROW1_Y + BUTTON_HEIGHT)
    {
        return BPLUS; //+
    }

    else if (x >= COL4_X && x <= COL4_X + BUTTON_WIDTH &&
        y >= ROW2_Y && y <= ROW2_Y + BUTTON_HEIGHT)
    {
        return BMINUS; //-
    }

    else if (x >= COL4_X && x <= COL4_X + BUTTON_WIDTH &&
        y >= ROW3_Y && y <= ROW3_Y + BUTTON_HEIGHT)
    {
        return BMULT; //*
    }

    else if (x >= COL4_X && x <= COL4_X + BUTTON_WIDTH &&
        y >= ROW4_Y && y <= ROW4_Y + BUTTON_HEIGHT)
    {
        return BDIV; // /
    }
    
    return BERR;
}

void displayResult(int32_t num)
{
    tft_fillRect(10, 10, 300, 40, ILI9340_BLACK);
    tft_setCursor(15, 20);
    tft_setTextColor(ILI9340_WHITE);
    sprintf(displayBuffer, "%ld", num);
    tft_writeString(displayBuffer);
}
void displayError()
{
    tft_fillRect(10, 10, 300, 40, ILI9340_BLACK);
    tft_setCursor(15, 20);
    tft_setTextColor(ILI9340_RED);
    tft_writeString("ERROR");
}