/*******************************************************************************
* File Name: hids.c
*
* Version: 1.0
*
* Description:
*  This file contains HIDS callback handler function.
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
* 
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "common.h"
#include "hids.h"

uint16 mouseSimulation;

uint8 protocol = CYBLE_HIDS_PROTOCOL_MODE_REPORT;   /* Boot or Report protocol mode */
uint8 suspend = CYBLE_HIDS_CP_EXIT_SUSPEND;         /* Suspend to enter into deep sleep mode */

/*******************************************************************************
* Function Name: HidsCallBack()
********************************************************************************
*
* Summary:
*   This is an event callback function to receive service specific events from 
*   HID Service.
*
* Parameters:
*  event - the event code
*  *eventParam - the event parameters
*
* Return:
*  None.
*
********************************************************************************/
void HidsCallBack(uint32 event, void *eventParam)
{
    CYBLE_HIDS_CHAR_VALUE_T *locEventParam = (CYBLE_HIDS_CHAR_VALUE_T *)eventParam;

    DBG_PRINTF("HIDS event: %lx, ", event);

    switch(event)
    {
        case CYBLE_EVT_HIDSS_NOTIFICATION_ENABLED:
            DBG_PRINTF("CYBLE_EVT_HIDSS_NOTIFICATION_ENABLED: serv=%x, char=%x\r\n", 
                locEventParam->serviceIndex,
                locEventParam->charIndex);
            if(CYBLE_HUMAN_INTERFACE_DEVICE_SERVICE_INDEX == locEventParam->serviceIndex)
            {
                mouseSimulation = ENABLED;
            }
            break;
        case CYBLE_EVT_HIDSS_NOTIFICATION_DISABLED:
            DBG_PRINTF("CYBLE_EVT_HIDSS_NOTIFICATION_DISABLED: serv=%x, char=%x\r\n", 
                locEventParam->serviceIndex,
                locEventParam->charIndex);
            if(CYBLE_HUMAN_INTERFACE_DEVICE_SERVICE_INDEX == locEventParam->serviceIndex)
            {
                mouseSimulation = DISABLED;
            }
            break;
        case CYBLE_EVT_HIDSS_BOOT_MODE_ENTER:
            DBG_PRINTF("CYBLE_EVT_HIDSS_BOOT_MODE_ENTER \r\n");
            protocol = CYBLE_HIDS_PROTOCOL_MODE_BOOT;
            break;
        case CYBLE_EVT_HIDSS_REPORT_MODE_ENTER:
            DBG_PRINTF("CYBLE_EVT_HIDSS_REPORT_MODE_ENTER \r\n");
            protocol = CYBLE_HIDS_PROTOCOL_MODE_REPORT;
            break;
        case CYBLE_EVT_HIDSS_SUSPEND:
            DBG_PRINTF("CYBLE_EVT_HIDSS_SUSPEND \r\n");
            suspend = CYBLE_HIDS_CP_SUSPEND;
        #if (DEBUG_UART_ENABLED == ENABLED)
            /* Reduce power consumption, power down logic that is not required to wake up the system */
            UART_DEB_Stop();
        #endif /* (DEBUG_UART_ENABLED == ENABLED) */
            break;
        case CYBLE_EVT_HIDSS_EXIT_SUSPEND:
        #if (DEBUG_UART_ENABLED == ENABLED)
            /* Power up all circuitry previously shut down */
            UART_DEB_Start();
        #endif /* (DEBUG_UART_ENABLED == ENABLED) */
            DBG_PRINTF("CYBLE_EVT_HIDSS_EXIT_SUSPEND \r\n");
            suspend = CYBLE_HIDS_CP_EXIT_SUSPEND;
            break;
        case CYBLE_EVT_HIDSS_REPORT_CHAR_WRITE:
            DBG_PRINTF("CYBLE_EVT_HIDSS_REPORT_CHAR_WRITE: serv=%x, char=%x, value=", 
                locEventParam->serviceIndex,
                locEventParam->charIndex);
                ShowValue(locEventParam->value);
            break;
        case CYBLE_EVT_HIDSC_NOTIFICATION:
            break;
        case CYBLE_EVT_HIDSC_READ_CHAR_RESPONSE:
            break;
        case CYBLE_EVT_HIDSC_WRITE_CHAR_RESPONSE:
            break;
        case CYBLE_EVT_HIDSC_READ_DESCR_RESPONSE:
            break;
        case CYBLE_EVT_HIDSC_WRITE_DESCR_RESPONSE:           
            break;
		default:
            DBG_PRINTF("Not supported event\r\n");
			break;
    }
}


/*******************************************************************************
* Function Name: HidsInit()
********************************************************************************
*
* Summary:
*   Initializes the HID service.
*
*******************************************************************************/
void HidsInit(void)
{
    CYBLE_API_RESULT_T apiResult;
    uint16 cccdValue;
    
    /* Register service specific callback function */
    CyBle_HidsRegisterAttrCallback(HidsCallBack);
    /* Read CCCD configurations from flash */
    mouseSimulation = DISABLED;
    apiResult = CyBle_HidssGetCharacteristicDescriptor(CYBLE_HUMAN_INTERFACE_DEVICE_SERVICE_INDEX, 
        CYBLE_HUMAN_INTERFACE_DEVICE_REPORT_IN, CYBLE_HIDS_REPORT_CCCD, CYBLE_CCCD_LEN, (uint8 *)&cccdValue);
    if((apiResult == CYBLE_ERROR_OK) && (cccdValue != 0u))
    {
        mouseSimulation |= ENABLED;
    }
    apiResult = CyBle_HidssGetCharacteristicDescriptor(CYBLE_HUMAN_INTERFACE_DEVICE_SERVICE_INDEX, 
        CYBLE_HIDS_BOOT_MOUSE_IN_REP, CYBLE_HIDS_REPORT_CCCD, CYBLE_CCCD_LEN, (uint8 *)&cccdValue);
    if((apiResult == CYBLE_ERROR_OK) && (cccdValue != 0u))
    {
        mouseSimulation |= ENABLED;
    }
}


/*******************************************************************************
* Function Name: SimulateMouse()
********************************************************************************
*
* Summary:
*   The custom function to simulate mouse moving in a square clockwise position
*
*******************************************************************************/
void SimulateMouse(void)
{
    uint8 abMouseData[MOUSE_DATA_LEN] = {0, 0, 0};  /* Mouse packet array */
    static uint8 boxLoop = 0;                       /* Box loop counter */
    static uint8 leftButtonPress = 0;               /* Left Button */
    CYBLE_API_RESULT_T apiResult;
    static uint8 dirState = 0;                      /* Mouse direction state */
    signed char bXInc = 0;                          /* X-Step Size */
    signed char bYInc = 0;                          /* Y-Step Size */
    
    if(CyBle_GattGetBusyStatus() == CYBLE_STACK_STATE_FREE)
    {
        /* Init Left Button state */
        leftButtonPress = (SW2_Read() == 0u) ? 1u : 0u;
        
        Simulation_LED_Write(LED_ON);
    
        boxLoop++;
        if(boxLoop > BOX_SIZE)          /* Change mouse direction every 32 packets */
        {
            boxLoop = 0;
            dirState++;                /* Advance box state */
            dirState &= POSMASK;
        }

        switch(dirState)               /* Determine current direction state */
        {
            case MOUSE_DOWN:                           /* Down */
                bXInc = 0;
                bYInc = CURSOR_STEP;
                break;
                
            case MOUSE_LEFT:                           /* Left */
                bXInc = -CURSOR_STEP;
                bYInc = 0;
                break;
                
            case MOUSE_UP:                             /* Up */
                bXInc = 0;
                bYInc = -CURSOR_STEP;
                break;
                
            case MOUSE_RIGHT:                          /* Right */
                bXInc = CURSOR_STEP;
                bYInc = 0;
                break;
        }

        abMouseData[1u] = bXInc;                            /* Load the packet array */
        abMouseData[2u] = bYInc;
        abMouseData[0u] = (leftButtonPress ? MOUSE_LB : 0u);     /* Set up Left Button state */
        DBG_PRINTF("HID notification %x %x %x \r\n", abMouseData[0],abMouseData[1],abMouseData[2]);
        if(protocol == CYBLE_HIDS_PROTOCOL_MODE_BOOT)
        {
            apiResult = CyBle_HidssSendNotification(cyBle_connHandle, CYBLE_HUMAN_INTERFACE_DEVICE_SERVICE_INDEX, 
                CYBLE_HIDS_BOOT_MOUSE_IN_REP, MOUSE_DATA_LEN, abMouseData);
            if(apiResult != CYBLE_ERROR_OK)
            {
                DBG_PRINTF("HIDS_BOOT_MOUSE_IN_REP notification API Error: %x \r\n", apiResult);
                mouseSimulation = DISABLED;
            }
        }
        else
        {
            apiResult = CyBle_HidssSendNotification(cyBle_connHandle, CYBLE_HUMAN_INTERFACE_DEVICE_SERVICE_INDEX, 
                CYBLE_HUMAN_INTERFACE_DEVICE_REPORT_IN, MOUSE_DATA_LEN, abMouseData);
            if(apiResult != CYBLE_ERROR_OK)
            {
                DBG_PRINTF("HIDS_REPORT notification API Error: %x \r\n", apiResult);
                mouseSimulation = DISABLED;
            }
        }
        Simulation_LED_Write(LED_OFF);
    }
}



/* [] END OF FILE */
