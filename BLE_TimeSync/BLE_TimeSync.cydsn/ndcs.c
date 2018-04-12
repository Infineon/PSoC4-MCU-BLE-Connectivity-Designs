/*******************************************************************************
* File Name: ndcs.c
*
* Version: 1.0
*
* Description:
*  This file contains NDCS callback handler function.
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
#include "ndcs.h"


/***************************************
*        Global Variables
***************************************/

/*******************************************************************************
* Function Name: NdcsAppEventHandler
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the BLE Component,
*  which are specific to Next DST Change Service.
*
* Parameters:  
*  uint8 event:       Reference Time Update Service event.
*  void* eventParams: Data structure specific to event received.
*
* Return: 
*  None
*
*******************************************************************************/
void NdcsAppEventHandler(uint32 event, void *eventParam)
{
    uint8 i;
    CYBLE_NDCS_CHAR_VALUE_T *charValue;
    
    switch(event)
    {
    case CYBLE_EVT_NDCSC_READ_CHAR_RESPONSE:
        charValue = (CYBLE_NDCS_CHAR_VALUE_T *) eventParam;
        
        DBG_PRINTF("Read Characteristic response is received!\r\n");
        DBG_PRINTF("Data length: %x, ", charValue->value->len);
        DBG_PRINTF("Data: ");

        for(i = 0u; i < charValue->value->len; i++)
        {
            DBG_PRINTF("%d ", charValue->value->val[i]);
        }
        DBG_PRINTF("\r\n");
        break;
        
	default:
        DBG_PRINTF("Unrecognized RTUS event.\r\n");
	    break;
    }
}


/* [] END OF FILE */
