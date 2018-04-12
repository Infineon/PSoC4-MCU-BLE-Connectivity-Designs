/*******************************************************************************
* File Name: rtus.c
*
* Version: 1.0
*
* Description:
*  This file contains RTUS callback handler function.
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

#include <project.h>
#include <stdio.h>
#include "common.h"
#include "rtus.h"


/***************************************
*        Global Variables
***************************************/

/*******************************************************************************
* Function Name: RtusAppEventHandler
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the BLE Component,
*  which are specific to Reference Time Update Service.
*
* Parameters:  
*  uint8 event:       Reference Time Update Service event.
*  void* eventParams: Data structure specific to event received.
*
* Return: 
*  None
*
*******************************************************************************/
void RtusAppEventHandler(uint32 event, void *eventParam)
{
    uint8 i;
    CYBLE_RTUS_CHAR_VALUE_T *charValue;
    
    switch(event)
    {
    /***************************************
    *        RTUS Client events
    ***************************************/
    case CYBLE_EVT_RTUSC_READ_CHAR_RESPONSE:
        charValue = (CYBLE_RTUS_CHAR_VALUE_T *) eventParam;
        
        DBG_PRINTF("Read Characteristic response is received!\r\n");
        DBG_PRINTF("Data length: %x, ", charValue->value->len);
        DBG_PRINTF("Data: ");

        for(i = 0u; i < charValue->value->len; i++)
        {
            DBG_PRINTF("%x ", charValue->value->val[i]);
        }
        DBG_PRINTF("\r\n");
        break;

    /***************************************
    *        RTUS Server events
    ***************************************/
    case CYBLE_EVT_RTUSS_WRITE_CHAR_CMD:
        break;
    
	default:
        DBG_PRINTF("Unrecognized RTUS event.\r\n");
	    break;
    }
}


/* [] END OF FILE */
