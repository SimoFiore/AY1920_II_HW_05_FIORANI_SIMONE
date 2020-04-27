#include "InterruptRoutines.h"
#include "project.h"

int FlagREAD = 0; // Definition of the flag that will be risen from the interrupt

CY_ISR (Custom_ISR_READ)
{
    Timer_ACC_ReadStatusRegister(); // Timer reset to generate new interrupt
    
    FlagREAD = 1;   // Flag that enable the reading of accelerometer in the main
    
}
/* [] END OF FILE */