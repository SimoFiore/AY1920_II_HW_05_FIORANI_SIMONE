/*******************************************************************************
* File Name: Timer_ACC_PM.c
* Version 2.80
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "Timer_ACC.h"

static Timer_ACC_backupStruct Timer_ACC_backup;


/*******************************************************************************
* Function Name: Timer_ACC_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_ACC_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void Timer_ACC_SaveConfig(void) 
{
    #if (!Timer_ACC_UsingFixedFunction)
        Timer_ACC_backup.TimerUdb = Timer_ACC_ReadCounter();
        Timer_ACC_backup.InterruptMaskValue = Timer_ACC_STATUS_MASK;
        #if (Timer_ACC_UsingHWCaptureCounter)
            Timer_ACC_backup.TimerCaptureCounter = Timer_ACC_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!Timer_ACC_UDB_CONTROL_REG_REMOVED)
            Timer_ACC_backup.TimerControlRegister = Timer_ACC_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: Timer_ACC_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_ACC_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_ACC_RestoreConfig(void) 
{   
    #if (!Timer_ACC_UsingFixedFunction)

        Timer_ACC_WriteCounter(Timer_ACC_backup.TimerUdb);
        Timer_ACC_STATUS_MASK =Timer_ACC_backup.InterruptMaskValue;
        #if (Timer_ACC_UsingHWCaptureCounter)
            Timer_ACC_SetCaptureCount(Timer_ACC_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!Timer_ACC_UDB_CONTROL_REG_REMOVED)
            Timer_ACC_WriteControlRegister(Timer_ACC_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: Timer_ACC_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_ACC_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer_ACC_Sleep(void) 
{
    #if(!Timer_ACC_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(Timer_ACC_CTRL_ENABLE == (Timer_ACC_CONTROL & Timer_ACC_CTRL_ENABLE))
        {
            /* Timer is enabled */
            Timer_ACC_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            Timer_ACC_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    Timer_ACC_Stop();
    Timer_ACC_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer_ACC_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Timer_ACC_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer_ACC_Wakeup(void) 
{
    Timer_ACC_RestoreConfig();
    #if(!Timer_ACC_UDB_CONTROL_REG_REMOVED)
        if(Timer_ACC_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                Timer_ACC_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
