/*******************************************************************************
* File Name: ota_optional.c
*
* Version: 1.20
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
#include "debug.h"

volatile uint32 mainTimer = 0u;


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
*  Configures WDT to trigger an interrupt.
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
* Function Name: HandleLeds()
********************************************************************************
*
* Summary:
*   This function handles LEDs operation depending on the project operation
*   mode.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void HandleLeds()
{
    static uint32 ledTimer = LED_TIMEOUT;
    static uint8 led = LED_OFF;
    
   /* Blink LED to indicate that device advertises */
    if(CyBle_GetState() == CYBLE_STATE_ADVERTISING)
    {
        if (Connection_LED_Read() != LED_OFF)
        {
            Connection_LED_Write(LED_OFF);
        }
        if(--ledTimer == 0u) 
        {
            ledTimer = LED_TIMEOUT;
            led ^= LED_OFF;
            Advertising_LED_Write(led);
        }
    }
    else if (CyBle_GetState() == CYBLE_STATE_CONNECTED)
    {
        Advertising_LED_Write(LED_ON);
        Connection_LED_Write(LED_ON);
    }
}


/*******************************************************************************
* Function Name: ShowError()
********************************************************************************
*
* Summary:
*   Shows error condition: Turn On all leds - white color will indicate error.
*
*******************************************************************************/
void ShowError(void)
{
    LowPower_LED_Write(LED_ON);
    Advertising_LED_Write(LED_ON);
    Connection_LED_Write(LED_ON);
    /* TDB */
    /* Halt CPU in Debug mode */
    CYASSERT(0u != 0u);
}

/*******************************************************************************
* Function Name: ShowValue
********************************************************************************
*
* Summary:
*  Prints characteristic value to UART
*
*******************************************************************************/
void ShowValue(CYBLE_GATT_VALUE_T *value)
{
    int16 i;
    
    for(i = 0; i < value->len; i++)
    {
        DBG_PRINTF("%2.2x ", value->val[i]);
    }
    DBG_PRINTF("\r\n");
}

/* [] END OF FILE */
