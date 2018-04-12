/*******************************************************************************
* File Name: main.h
*
* Version: 1.40
*
* Description:
*  BLE HID keyboard example project that supports both input and output reports
*  in boot and protocol mode. The example also demonstrates handling suspend 
*  event from the central device and enters low power mode when suspended.
*
* References:
*  BLUETOOTH SPECIFICATION Version 4.1
*  HID Usage Tables spec ver 1.12
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
*
********************************************************************************
* Copyright 2014-2016, Cypress Semiconductor Corporation.  All rights reserved.
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

#include "ota_mandatory.h"
#include "ota_optional.h"
#include "common.h"
#include "hids.h"
#include "bas.h"
#include "scps.h"

void AppCallBack(uint32 event, void* eventParam);

#endif /* BLE_OTA_EP_MAIN_H_ */

/* [] END OF FILE */
