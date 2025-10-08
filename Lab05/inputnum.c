#include "inputnum.h"
#include <stdio.h>
static int32_t currentInput = 0, lastInput=0;
OPERATOR op=ERR;
void clear()
{
    currentInput = 0;
}
int32_t getInput(Buttons button)
{
    // === TODO: === Simple number building (NO overflow check)
    switch (button)
    {
    case B0:
        currentInput = currentInput * 10 + 0;
        break;
    case B1:
        currentInput = currentInput * 10 + 1;
        break;
    case B2:
        currentInput=currentInput*10+2;
        break;
    case B3:
        currentInput=currentInput*10+3;
        break;
    case B4:
        currentInput=currentInput*10+4;
        break;
    case B5:
        currentInput=currentInput*10+5;
        break;
    case B6:
        currentInput=currentInput*10+6;
        break;
    case B7:
        currentInput=currentInput*10+7;
        break;
    case B8:
        currentInput=currentInput*10+8;
        break;
    case B9:
        currentInput=currentInput*10+9;
        break;
    case BCL:
        break;
    case BEQ:
        //equals
        break;
    //in the event of an operator press, move current input to storage and reset current input, store operator type
    case BPLUS:
        //plus
        break;
    case BMINUS:
        //minus
        break;
    case BMULT:
        //mult
        break;
    case BDIV:
        //divide
    break;

    default:
        break;
    }
    return currentInput;
}
int32_t performOperation(int32_t num1, int32_t num2, OPERATOR op)
{
    // === TODO: === Basic operations (NO overflow check)
    switch (op)
    {
    case PLUS:
        return num1 + num2;
        break;
    case MINUS:
        return num1-num2;
        break;
    case MULT:
        return num1*num2;
        break;
    case DIV:
        if(num2==0){ 
            displayError();
            sleep_ms(1000);
            return 0;
        }
        else return num1/num2;
        break;
    // === TODO: === Write remaining basic operations
    // *** HINT: *** Use if condition to handle divide by zero case
    default:
        return 0;
    }
}
// Simple helper functions
bool pressC(Buttons button) { return (button == BCL); }
bool pressEq(Buttons button) { return (button == BEQ); }
bool pressOperator(Buttons button)
{
    return (button == BPLUS || button == BMINUS ||
            button == BMULT || button == BDIV);
}
bool pressNumber(Buttons button)
{
    return (button >= B0 && button <= B9);
    //return button==B0||button==B1||button==B2||button==B3||button==B4||button==B5||button==B6||button==B7||button==B8||button==B9;
}
OPERATOR readOperator(Buttons button)
{
    switch (button)
    {
    case BPLUS:
        return PLUS;
    case BMINUS:
        return MINUS;
    case BMULT:
        return MULT;
    case BDIV:
        return DIV;
    default:
        return ERR;
    }
}