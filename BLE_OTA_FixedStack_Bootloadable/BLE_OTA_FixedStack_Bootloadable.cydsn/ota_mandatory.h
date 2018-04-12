/*******************************************************************************
* File Name: ota_mandatory.h
*
* Version 1.50
*
* Description:
*  Contains the function prototypes and constants available to the example
*  project. They are mandatory for OTA functionality.
*
*  Use this header file together with BLE component version 3.30.
*  Request Cypress to send appropriate revision of this file for usage together
*  with either newer or older component versions.
*
********************************************************************************
* Copyright 2014-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#ifndef BLE_OTA_EP_OTA_MANDATORY_H_
#define BLE_OTA_EP_OTA_MANDATORY_H_

/*
* Note: this include files have to be copied from Bootolader project each time
* BLE component gets updated
*/
#include <CyBle.h>
#include <CyBle_bas.h>
#include <CyBle_bless_isr.h>
#include <CyBle_bts.h>
#include <CyBle_dis.h>
#include <CyBle_eventHandler.h>
#include <CyBle_gatt.h>
#include <CyBle_HAL_PVT.h>
#include <CyBle_hids.h>
#include <CyBle_scps.h>
#include <CyBle_Stack.h>
#include <CyBle_STACK_PVT.h>
#include <CyBle_StackGap.h>
#include <CyBle_StackGatt.h>
#include <CyBle_StackGattClient.h>
#include <CyBle_StackGattDb.h>
#include <CyBle_StackGattServer.h>
#include <CyBle_StackHostMain.h>
#include <CyBle_StackL2cap.h>

/* Pin APIs */
extern void Bootloader_Service_Activation_Write(uint8 value);
extern uint8 Bootloader_Service_Activation_ClearInterrupt(void);
extern void Bootloader_Service_Activation_SetDriveMode(uint8 mode);
extern void Advertising_LED_1_Write(uint8 value);
extern void Advertising_LED_2_Write(uint8 value);
extern void Advertising_LED_1_SetDriveMode(uint8 mode);
extern void Advertising_LED_2_SetDriveMode(uint8 mode);
#define Advertising_LED_Write(value) Advertising_LED_1_Write(value)
#define Advertising_LED_SetDriveMode(value) Advertising_LED_1_SetDriveMode(value)
#define Disconnect_LED_Write(value) Advertising_LED_2_Write(value)
#define Disconnect_LED_SetDriveMode(value) Advertising_LED_2_SetDriveMode(value)
extern void Bootloading_LED_SetDriveMode(uint8 value);
extern void Bootloading_LED_Write(uint8 value);
#define CapsLock_LED_SetDriveMode(value) Bootloading_LED_SetDriveMode(value)
#define CapsLock_LED_Write(value) Bootloading_LED_Write(value)

/* Interrupt APIs */
extern uint8 Bootloader_Service_Activation_ClearInterrupt(void);
extern uint8 Bootloader_Service_Activation_Read(void);
extern void Wakeup_Interrupt_ClearPending(void);
extern void Wakeup_Interrupt_Start(void);

/* These API are defined in ota_mandatory.c, they are not code shared */
void ConfigureSharedPins(void);
void BootloaderSwitch(void);
void ConfigureServices(void);

#endif /* BLE_OTA_EP_OTA_MANDATORY_H_ */


/* [] END OF FILE */
