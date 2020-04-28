/**
* Assignment 5 - Project 2.2
*
* In this project we want to apply the I2C protocol
* to read data from the LIS3DH accelerometer through a multiread.
* In this section we want to read the data in mg units.
*
* \author Simone Fiorani
* \date , 2020
*/

// Include required header files
#include "I2C_Interface.h"
#include "project.h"
#include "stdio.h"
#include "InterruptRoutines.h"

/**
*   \brief 7-bit I2C address of the slave device.
*/
#define LIS3DH_DEVICE_ADDRESS 0x18

/**
*   \brief Address of the WHO AM I register
*/
#define LIS3DH_WHO_AM_I_REG_ADDR 0x0F

/**
*   \brief Address of the Status register
*/
#define LIS3DH_STATUS_REG 0x27

/**
*   \brief Address of the Control register 1
*/
#define LIS3DH_CTRL_REG1 0x20

/**
*   \brief Address of the Control register 4
*/
#define LIS3DH_CTRL_REG4 0x23

/**
*   \brief Address of the Xaxis output LSB register. It will be the first accelerometer register to be read in the multiread
*/
#define LIS3DH_X_AXIS_L 0x28

/**
*   \brief Hex value to set normal mode, 100 Hz to in CTRL_REG_1
*/
#define LIS3DH_NORMAL_MODE_100HZ_CTRL_REG_1 0x57

/**
*   \brief Hex value to set the BDU active and the +-2g FSR mode in CTRL_REG_4
*/
#define LIS3DH_CTRL_REG4_BDU_ACTIVE_2G 0x80


/******************************************/


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    I2C_Peripheral_Start(); // Start of the I2C
    UART_Debug_Start();     // Start of UART
    
    
    CyDelay(5); //"The boot procedure is complete about 5 milliseconds after device power-up."
    
    // String to print out messages on the UART
    char message[50];

    // Check which devices are present on the I2C bus
    for (int i = 0 ; i < 128; i++)
    {
        if (I2C_Peripheral_IsDeviceConnected(i))
        {
            // print out the address is hex format
            sprintf(message, "Device 0x%02X is connected\r\n", i);
            UART_Debug_PutString(message); 
        }
        
    }
    
    /******************************************/
    /*            I2C Reading                 */
    /******************************************/
    
    /* Read WHO AM I REGISTER register */
    uint8_t who_am_i_reg;
    
    ErrorCode error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                  LIS3DH_WHO_AM_I_REG_ADDR, 
                                                  &who_am_i_reg);
    if (error == NO_ERROR)
    {
        sprintf(message, "WHO AM I REG: 0x%02X [Expected: 0x33]\r\n", who_am_i_reg);
        UART_Debug_PutString(message); 
    }
    else
    {
        UART_Debug_PutString("Error occurred during I2C comm\r\n");   
    }
    
    /******************************************/
    /*      I2C Reading Status Register       */
    /******************************************/
    
    uint8_t status_register;
    
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_STATUS_REG,
                                        &status_register);
    
    if (error == NO_ERROR)
    {
        sprintf(message, "STATUS REGISTER: 0x%02X\r\n", status_register);
        UART_Debug_PutString(message); 
    }
    else
    {
        UART_Debug_PutString("Error occurred during I2C comm to read status register\r\n");   
    }
    
    /******************************************/
    /*            I2C Writing                 */
    /******************************************/
    
    uint8_t ctrl_reg1;
    
    if (ctrl_reg1 != LIS3DH_NORMAL_MODE_100HZ_CTRL_REG_1)
    {
        ctrl_reg1 = LIS3DH_NORMAL_MODE_100HZ_CTRL_REG_1; // We set the 100 Hz, normal mode to sample the accelerometer data
    
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             LIS3DH_CTRL_REG1,
                                             ctrl_reg1);
    
        if (error == NO_ERROR)
        {
            sprintf(message, "CONTROL REGISTER 1 successfully written as: 0x%02X\r\n", ctrl_reg1);
            UART_Debug_PutString(message); 
        }
        else
        {
            UART_Debug_PutString("Error occurred during I2C comm to set control register 1\r\n");   
        }
    }
    
    /******************************************/
    /*        Read Control Register 1         */
    /******************************************/
    
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG1,
                                        &ctrl_reg1);
    
    if (error == NO_ERROR)
    {
        sprintf(message, "CONTROL REGISTER 1: 0x%02X\r\n", ctrl_reg1);
        UART_Debug_PutString(message); 
    }
    else
    {
        UART_Debug_PutString("Error occurred during I2C comm to read control register 1\r\n");   
    }
    
    /******************************************/
    /*          Set Control Register 4        */
    /******************************************/
    
    uint8_t ctrl_reg4;
  
    ctrl_reg4 = LIS3DH_CTRL_REG4_BDU_ACTIVE_2G; // We set the +-2g mode and the BDU active, so the data won't be uploaded,
                                                // until both LSB and MSB of the registers have been read
    
    error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_CTRL_REG4,
                                         ctrl_reg4);
    
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG4,
                                        &ctrl_reg4);
    
    
    if (error == NO_ERROR)
    {
        sprintf(message, "CONTROL REGISTER 4 after being updated: 0x%02X\r\n", ctrl_reg4);
        UART_Debug_PutString(message); 
    }
    else
    {
        UART_Debug_PutString("Error occurred during I2C comm to read control register4\r\n");   
    }
    
    /******************************************/
    /*   Reading of the 3 Axis Accelerometer  */
    /******************************************/
    
    uint8_t StatusReg;      // Reading of the StatusReg to check if new data is available
    int16_t OutTemp;        // Variable containing the actual, int16, data of the i-Axis accelerometer that will be sent to UART
    uint8_t header = 0xA0;  // Header of the UART string
    uint8_t footer = 0xC0;  // Footer of the UART string
    uint8_t OutArray[8];    // The final packet sent by UART
    uint8_t AccData[6];     // Array containig the accelerometer data in this order: LSB and MSB of the X,Y and then Z axis
    
    OutArray[0] = header;   // First byte of the string is the header
    OutArray[7] = footer;   // Last byte of the string is the footer
    
//    isr_READ_StartEx(Custom_ISR_READ);  // Starting the ISR that rise the flag enabling the accelerometer reading
//    Timer_ACC_Start();                  // Timer that generates the ISR at 100 Hz, in order to have a constant reading rate
    
    for(;;)
    {
//        if (FlagREAD == 1) // If the timer has generated the interrupt
//        {
            error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,  // Read the content of the status reg. We want to control
                                                LIS3DH_STATUS_REG,      // the bit ZYXDA (bit 3), that it's 1 when a new set of data is
                                                &StatusReg);            // available. BDU active ensure that the data of the register
                                                                        // won't be updated until the reading is done
                                                                        
            if ( (StatusReg & 8) == 8 ) // If bit ZYXDA is high (new data available)
            {
                error = I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS, // Read the content of the registers of the accelerometer.
                                                         LIS3DH_X_AXIS_L,       // Starting the reading from the first register (LSB of X axis)
                                                         6,                     // We have 6 register to be read (LSB and MSB for the 3 axis).
                                                         &AccData[0]);          // The content saved in the array AccData in X,Y,Z order
                
                if(error == NO_ERROR) // If reading completed without errors
                {
                    
                    OutTemp = (int16)((AccData[0] | (AccData[1]<<8)))>>6;   // LSB and MSB of the reading form an int16. The 6 bit shift is done
                                                                            //      in order to right-align the 10bit left-align value received from the accelerometer
                    
                    OutTemp *= 4;                                           // We multiply for the sensitivity of the sensor to have real value in mg
                    
                    OutArray[1] = (uint8_t)(OutTemp & 0xFF);                // Separation of the int16 in the 2 byte that will be sent
                    OutArray[2] = (uint8_t)(OutTemp >> 8);                  //      to the UART. The order il LSB - MSB
                    
                    OutTemp = (int16)((AccData[2] | (AccData[3]<<8)))>>6;   // Repetition of the reading procedure for Y axis
                    
                    OutTemp *= 4;
                    
                    OutArray[3] = (uint8_t)(OutTemp & 0xFF);
                    OutArray[4] = (uint8_t)(OutTemp >> 8);
                    
                    OutTemp = (int16)((AccData[4] | (AccData[5]<<8)))>>6;   // Repetition of the reading procedure for Z axis
                    
                    OutTemp *= 4;
                    
                    OutArray[5] = (uint8_t)(OutTemp & 0xFF);
                    OutArray[6] = (uint8_t)(OutTemp >> 8);
                    
                    UART_Debug_PutArray(OutArray, 8); // Sending of the complete string through UART
                    
//                    FlagREAD = 0; // Setting again the flag to zero, waiting a new interrupt from timer
                    
                }
            }
        }
    }
//}

/* [] END OF FILE */
