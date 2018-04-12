/*******************************************************************************
* File Name: common.h
*
* Version 1.20
*
* Description:
*  Contains the function prototypes and constants available to the example
*  project.
*
********************************************************************************
* Copyright 2014-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#ifndef BLE_OTA_EP_COMMON_H_
#define BLE_OTA_EP_COMMON_H_

#include <project.h>
#include <stdio.h>
#include "debug.h"

#define ENABLED                     (1u)
#define DISABLED                    (0u)



#define LED_TIMEOUT                 (10u)              /* Сounts in hundreds of seconds */

#define WDT_COUNTER                                   (CY_SYS_WDT_COUNTER1)
#define WDT_COUNTER_MASK                              (CY_SYS_WDT_COUNTER1_MASK)
#define WDT_INTERRUPT_SOURCE                          (CY_SYS_WDT_COUNTER1_INT) 
#define WDT_COUNTER_ENABLE                            (1u)
#define WDT_TIMEOUT                                   (32767u/10u) /* 100 ms @ 32.768kHz clock */


/***************************************
*        External Function Prototypes
***************************************/
void ShowValue(CYBLE_GATT_VALUE_T *value);
void Set32ByPtr(uint8 ptr[], uint32 value);
void ShowError(void);

#endif /* BLE_OTA_EP_COMMON_H_ */

/* [] END OF FILE */
