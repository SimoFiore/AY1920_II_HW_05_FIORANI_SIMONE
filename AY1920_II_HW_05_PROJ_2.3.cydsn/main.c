/**
* \brief Main source file for the I2C-Master project.
*
* In this project we set up a I2C master device with
* to understand the I2C protocol and communicate with a
* a I2C Slave device (LIS3DH Accelerometer).
*
* \author Gabriele Belotti
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
*   \brief Address of the Xaxis output LSB register. It will be the first register to be read for accelerometer data
*/
#define LIS3DH_X_AXIS_L 0x28

/**
*   \brief Hex value to set normal mode, 100 Hz to the accelerator in CTRL_REG_1
*/
#define LIS3DH_NORMAL_MODE_100HZ_CTRL_REG_1 0x57

/**
*   \brief Hex value to set high resolution mode and +-4.0g FSR to the accelerator in CTRL_REG_4
*/
#define LIS3DH_HIGH_RES_MODE_4G_CTRL_REG_4 0x98

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    I2C_Peripheral_Start();
    UART_Debug_Start();
    
    
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
    
    /*      I2C Reading Status Register       */
    
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
    /*        Read Control Register 1         */
    /******************************************/
    uint8_t ctrl_reg1; 
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
    /*            I2C Writing                 */
    /******************************************/
    
        
    UART_Debug_PutString("\r\nWriting new values..\r\n");
    
    if (ctrl_reg1 != LIS3DH_NORMAL_MODE_100HZ_CTRL_REG_1)
    {
        ctrl_reg1 = LIS3DH_NORMAL_MODE_100HZ_CTRL_REG_1;
    
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
    /*     Read Control Register 1 again      */
    /******************************************/

    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG1,
                                        &ctrl_reg1);
    
    if (error == NO_ERROR)
    {
        sprintf(message, "CONTROL REGISTER 1 after overwrite operation: 0x%02X\r\n", ctrl_reg1);
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

    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG4,
                                        &ctrl_reg4);
    
    if (error == NO_ERROR)
    {
        sprintf(message, "CONTROL REGISTER 4: 0x%02X\r\n", ctrl_reg4);
        UART_Debug_PutString(message); 
    }
    else
    {
        UART_Debug_PutString("Error occurred during I2C comm to read control register4\r\n");   
    }
    
    
    ctrl_reg4 = LIS3DH_HIGH_RES_MODE_4G_CTRL_REG_4; // must be changed to the appropriate value
    
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
    
    uint8_t StatusReg;
    int16_t OutTemp;
    uint8_t header = 0xA0;
    uint8_t footer = 0xC0;
    uint8_t OutArray[14]; 
    uint8_t AccData[6];
    
    float Xaxis_Acc;
    float Yaxis_Acc;
    float Zaxis_Acc;
    
    int32 Xaxis_OUT;
    int32 Yaxis_OUT;
    int32 Zaxis_OUT;
    
    OutArray[0] = header;
    OutArray[13] = footer;
    
    Timer_ACC_Start();
    isr_READ_StartEx(Custom_ISR_READ);
    
    for(;;)
    {
        if (FlagREAD == 1)
        {
            error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                LIS3DH_STATUS_REG,
                                                &StatusReg);
            if ( (StatusReg & 8) == 8 )
            {
                error = I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS,
                                                         LIS3DH_X_AXIS_L,
                                                         6,
                                                         &AccData[0]);
                if(error == NO_ERROR)
                {
                                       
                    OutTemp = (int16)((AccData[0] | (AccData[1]<<8)))>>4;
                    Xaxis_Acc = OutTemp * 9.806 * 0.001 * 2;
                    Xaxis_OUT = Xaxis_Acc * 1000;
                    
                    OutArray[1] = (uint8_t)(Xaxis_OUT & 0xFF);
                    OutArray[2] = (uint8_t)(Xaxis_OUT >> 8);
                    OutArray[3] = (uint8_t)(Xaxis_OUT >> 16);
                    OutArray[4] = (uint8_t)(Xaxis_OUT >> 24);
                    
                    OutTemp = (int16)((AccData[2] | (AccData[3]<<8)))>>4;
                    Yaxis_Acc = OutTemp * 9.806 * 0.001 * 2;
                    Yaxis_OUT = Yaxis_Acc * 1000;
                    
                    OutArray[5] = (uint8_t)(Yaxis_OUT & 0xFF);
                    OutArray[6] = (uint8_t)(Yaxis_OUT >> 8);
                    OutArray[7] = (uint8_t)(Yaxis_OUT >> 16);
                    OutArray[8] = (uint8_t)(Yaxis_OUT >> 24);
                    
                    OutTemp = (int16)((AccData[4] | (AccData[5]<<8)))>>4;
                    Zaxis_Acc = OutTemp * 9.806 * 0.001 * 2;
                    Zaxis_OUT = Zaxis_Acc * 1000;
                    
                    OutArray[9] = (uint8_t) (Zaxis_OUT & 0xFF);
                    OutArray[10] = (uint8_t)(Zaxis_OUT >> 8);
                    OutArray[11] = (uint8_t)(Zaxis_OUT >> 16);
                    OutArray[12] = (uint8_t)(Zaxis_OUT >> 24);
                    
                    UART_Debug_PutArray(OutArray, 14);
                    
                    FlagREAD = 0;
                }
            }
        }
    }
}

/* [] END OF FILE */
