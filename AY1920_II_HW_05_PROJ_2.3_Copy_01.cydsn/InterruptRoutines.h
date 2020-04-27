#ifndef __INTERRUPT_ROUTINE_H
    
    #include "project.h"
    #include "cytypes.h"
    #include "stdio.h"
    
    int FlagREAD;   // Declaration of the Flag used in the ISR
    
    CY_ISR_PROTO (Custom_ISR_READ); // Declaration of prototype of the ISR function
    
#endif
/* [] END OF FILE */
