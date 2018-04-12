/*******************************************************************************
* File Name: common.h
*
* Description:
*  Common BLE application header.
*
*******************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


/***************************************
*        API Constants
***************************************/

#define LED_ON             (0u)
#define LED_OFF            (1u)


/***************************************
*        Function Prototypes
***************************************/
void StackEventHandler(uint32 event, void* eventParam);


/* [] END OF FILE */
