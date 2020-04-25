/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "InterruptRoutines.h"
#include "project.h"

int FlagREAD = 0;

CY_ISR (Custom_ISR_READ)
{
    Timer_ACC_ReadStatusRegister();
    
    FlagREAD = 1;
    
}
/* [] END OF FILE */
