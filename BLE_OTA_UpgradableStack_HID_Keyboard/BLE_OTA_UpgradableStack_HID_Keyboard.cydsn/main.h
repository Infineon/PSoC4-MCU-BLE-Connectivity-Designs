/*******************************************************************************
* File Name: main.h
*
* Version: 1.20
*
* Description:
*  Simple BLE example project that demonstrates how to configure and use
*  Cypress's BLE component APIs and application layer callback. Device
*  Information Service is used as an example to demonstrate configuring
*  BLE service characteristics in the BLE component.
*
* References:
*  BLUETOOTH SPECIFICATION Version 4.1
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

#ifndef BLE_OTA_EP_MAIN_H_
#define BLE_OTA_EP_MAIN_H_

#include <project.h>
#include <stdio.h>
#include <string.h>
#include "ota_mandatory.h"
#include "ota_optional.h"
#include "options.h"
#include "cytypes.h"
#include "debug.h"
#include "hids.h"
#include "bas.h"
#include "scps.h"

extern void InitializeBootloaderSRAM(void);
extern volatile uint32 mainTimer;
void AppCallBack(uint32 event, void* eventParam);


#endif /* BLE_OTA_EP_MAIN_H_ */


/* [] END OF FILE */
