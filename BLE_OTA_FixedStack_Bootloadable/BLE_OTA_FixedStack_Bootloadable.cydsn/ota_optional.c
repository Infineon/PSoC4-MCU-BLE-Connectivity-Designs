/*******************************************************************************
* File Name: ota_optional.c
*
* Version: 1.40
*
* Description:
*  Provides an API that implement optional functionality of OTA.
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
#include "ota_optional.h"
#include "common.h"

/*******************************************************************************
* Function Name: Timer_Interrupt
********************************************************************************
*
* Summary:
*  Handles the Interrupt Service Routine for the WDT timer.
*
*******************************************************************************/
CY_ISR(Timer_Interrupt)
{
    if(CySysWdtGetInterruptSource() & WDT_INTERRUPT_SOURCE)
    {
        static uint8 led = LED_OFF;
        static uint32 ledTimer = LED_TIMEOUT;
    
        /* Blink green LED to indicate that device advertises */
        if(CyBle_GetState() == CYBLE_STATE_ADVERTISING)
        {
            if(--ledTimer == 0u) 
            {
                ledTimer = LED_TIMEOUT;
                led ^= LED_OFF;
                Advertising_LED_Write(led);
            }
        }
        
        /* Indicate that timer is raised to the main loop */
        mainTimer++;
        
        /* Clears interrupt request  */
        CySysWdtClearInterrupt(WDT_INTERRUPT_SOURCE);
    }
}

/*******************************************************************************
* Function Name: WDT_Start
********************************************************************************
*
* Summary:
*  Configures WDT to trigger an interrupt with period 100ms for keyboard
*  activity simulation.
*
*******************************************************************************/

void WDT_Start(void)
{
    /* Unlock the WDT registers for modification */
    CySysWdtUnlock(); 
    /* Setup ISR */
    WDT_Interrupt_StartEx(&Timer_Interrupt);
    /* Write the mode to generate interrupt on match */
    CySysWdtWriteMode(WDT_COUNTER, CY_SYS_WDT_MODE_INT);
    /* Configure the WDT counter clear on a match setting */
    CySysWdtWriteClearOnMatch(WDT_COUNTER, WDT_COUNTER_ENABLE);
    /* Configure the WDT counter match comparison value */
    CySysWdtWriteMatch(WDT_COUNTER, WDT_TIMEOUT);
    /* Reset WDT counter */
    CySysWdtResetCounters(WDT_COUNTER);
    /* Enable the specified WDT counter */
    CySysWdtEnable(WDT_COUNTER_MASK);
    /* Lock out configuration changes to the Watchdog timer registers */
    CySysWdtLock();    
}


/*******************************************************************************
* Function Name: WDT_Stop
********************************************************************************
*
* Summary:
*  This API stops the WDT timer.
*
*******************************************************************************/
void WDT_Stop(void)
{
    /* Unlock the WDT registers for modification */
    CySysWdtUnlock(); 
    /* Disable the specified WDT counter */
    CySysWdtDisable(WDT_COUNTER_MASK);
    /* Locks out configuration changes to the Watchdog timer registers */
    CySysWdtLock();    
}


/*******************************************************************************
* Function Name: PrintProjectHeader
********************************************************************************
*
* Summary:
*  Prints project header to UART.
*
*******************************************************************************/
void PrintProjectHeader()
{
    DBG_PRINT_TEXT("\r\n");
    DBG_PRINT_TEXT("\r\n");
    DBG_PRINT_TEXT("> BLE OTA Fixed Stack Bootloadable   Application Started\r\n");
    DBG_PRINT_TEXT("> Based on BLE HID Keyboard Example Project\r\n");
    DBG_PRINT_TEXT("> Version: 1.30\r\n");
    DBG_PRINT_TEXT("> Compile Date and Time : " __DATE__ " " __TIME__ "\r\n");
    DBG_PRINT_TEXT("\r\n"); 
    DBG_PRINT_TEXT("\r\n");
}

/* [] END OF FILE */
