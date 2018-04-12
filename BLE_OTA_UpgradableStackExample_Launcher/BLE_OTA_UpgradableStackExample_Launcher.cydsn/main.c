/*******************************************************************************
* File Name: main.c
*
* Version: 1.20
*
* Description:
*  This example shows how to build a launcher project for a BLE design.
*  This project is required for designs that require Over the Air upgrade
*  capability for both Application and the BLE stack.
*
* References:
*  BLUETOOTH SPECIFICATION Version 4.1
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
*
********************************************************************************
* Copyright 2014-2016, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include "project.h"
#include "options.h"
#include "debug.h"


#if CYDEV_FLASH_SIZE != 0x00040000u
 #error "This design is specifically targeted to parts with 256k of flash. Please change project device to BLE\
 silicon that has 256K Flash array. For example CY8C4248LQI-BL483."
#endif


/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  Starts the bootloader component in launcher+copier mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
int main()
{
    L_LED_Write(1);
    L_LED_Write(0);
    
    DBG_NEW_PAGE();
    DBG_PRINT_TEXT("\r\n");
    DBG_PRINT_TEXT("\r\n");
    DBG_PRINT_TEXT("> BLE Upgradable Stack Example Launcher\r\n");
    DBG_PRINT_TEXT("> Version: 1.20\r\n");
    DBG_PRINT_TEXT("> Compile Date and Time: " __DATE__ " " __TIME__ "\r\n");
    DBG_PRINT_TEXT("\r\n"); 
    DBG_PRINT_TEXT("\r\n"); 

    Launcher_Start();
    for(;;)
    {
        /* Should newer get here. */
    }
}


/* [] END OF FILE */
