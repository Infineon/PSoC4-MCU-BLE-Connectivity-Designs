/*******************************************************************************
* File Name: basc.c
*
* Version 1.0
*
* Description:
*  This file contains BAS callback handler function.
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

#include "main.h"


uint8 basNotification = 3u;

/*******************************************************************************
* Function Name: BasInit()
********************************************************************************
*
* Summary:
*   Initializes the battery service.
*
*******************************************************************************/
void BasInit(void)
{
    CyBle_BasRegisterAttrCallback(BasCallBack);
}


/*******************************************************************************
* Function Name: BasCallBack()
********************************************************************************
*
* Summary:
*   This is an event callback function to receive service specific events from 
*   Battery Service.
*
* Parameters:
*  event - the event code
*  *eventParam - the event parameters
*
* Return:
*  None.
*
*******************************************************************************/
void BasCallBack(uint32 event, void* eventParam)
{
    uint8 batteryLevel;
    
    DBG_PRINTF("BAS event: %lx, ", event);

    switch(event)
    {
        case CYBLE_EVT_BASC_WRITE_DESCR_RESPONSE:
            if(0u == basNotification)
            {
                DBG_PRINTF("Battery Level Notification is Disabled  \r\n");
            }
            else
            {
                DBG_PRINTF("Battery Level Notification is Enabled  \r\n");
            }
            break;

        case CYBLE_EVT_BASC_NOTIFICATION:
            batteryLevel = *((CYBLE_BAS_CHAR_VALUE_T*)eventParam)->value->val;
            (void)batteryLevel;
            DBG_PRINTF("Battery Level Notification: %d  \r\n", batteryLevel);

            if(2u == basNotification)
            {
                basNotification = 1u;
                apiResult = CyBle_BascGetCharacteristicDescriptor(cyBle_connHandle, 0, CYBLE_BAS_BATTERY_LEVEL,
                    CYBLE_BAS_BATTERY_LEVEL_CCCD);
                if(apiResult != CYBLE_ERROR_OK)
                {
                    DBG_PRINTF("CyBle_BascGetCharacteristicDescriptor API Error: ");
                    PrintApiResult();
                }
                else
                {
                    DBG_PRINTF("BL CCCD Read Request is sent \r\n");
                }
            }
            
            CySysWdtResetCounters(CY_SYS_WDT_COUNTER2_MASK);
            Notification_LED_Write(LED_ON);
            break;

        case CYBLE_EVT_BASC_READ_CHAR_RESPONSE:
            DBG_PRINTF("BAS CHAR Read Response: %d  \r\n", *((CYBLE_BAS_CHAR_VALUE_T*)eventParam)->value->val);
            break;

        case CYBLE_EVT_BASC_READ_DESCR_RESPONSE:
            DBG_PRINTF("BAS descriptor read rsp: %4.4x  \r\n", CyBle_Get16ByPtr(((CYBLE_BAS_DESCR_VALUE_T*)eventParam)->value->val));
            break;

        default:
            DBG_PRINTF("Not supported event\r\n");
            break;
    }
}


/* [] END OF FILE */
