/*******************************************************************************
* File Name: ias.c
*
* Description:
*  This file contains Immediate Alert Service callback handler function.
* 
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>
#include "ias.h"

/* IAS alert level value */
uint8 alertLevel = 0;

/*******************************************************************************
* Function Name: IasEventHandler
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the BLE Component,
*  which are specific to Immediate Alert Service.
*
* Parameters:
*  uint8 event:       Write Command event from the CYBLE component.
*  void* eventParams: A structure instance of CYBLE_GATT_HANDLE_VALUE_PAIR_T
*                     type.
*
* Return:
*  None
*
*******************************************************************************/
void IasEventHandler(uint32 event, void *eventParam)
{
    /* Alert Level Characteristic write event */
    if(event == CYBLE_EVT_IASS_WRITE_CHAR_CMD)
    {
        /* Read the updated Alert Level value from the GATT database */
        CyBle_IassGetCharacteristicValue(CYBLE_IAS_ALERT_LEVEL, 
            sizeof(alertLevel), &alertLevel);
    }
}


/* [] END OF FILE */
