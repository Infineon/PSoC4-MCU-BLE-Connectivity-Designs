/*******************************************************************************
* File Name: ota_optional.h
*
* Version 1.40
*
* Description:
*  Contains the function prototypes and constants available to the example
*  project. They are optional for OTA functionality.
*
********************************************************************************
* Copyright 2014-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#ifndef BLE_OTA_EP_OTA_OPTIONAL_H_
#define BLE_OTA_EP_OTA_OPTIONAL_H_

#include "common.h"
#include "ota_mandatory.h"


extern void InitializeBootloaderSRAM(void);

extern volatile uint32 mainTimer;

CY_ISR(Timer_Interrupt);
void WDT_Start(void);
void WDT_Stop(void);
void PrintProjectHeader(void);

#endif /* BLE_OTA_EP_OPTIONAL_H_ */

/* [] END OF FILE */
