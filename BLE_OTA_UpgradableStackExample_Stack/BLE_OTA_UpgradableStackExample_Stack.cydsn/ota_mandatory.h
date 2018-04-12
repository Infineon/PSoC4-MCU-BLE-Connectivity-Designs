/*******************************************************************************
* File Name: ota_mandatory.h
* Version 1.20
*
* Description:
*  Contains the function prototypes and constants available to the example
*  project. They are mandatory for OTA functionality.
*
********************************************************************************
* Copyright 2014-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#ifndef BLE_OTA_EP_OTA_MANDAROTY_H_
#define BLE_OTA_EP_OTA_MANDAROTY_H_

#include <cytypes.h>


#if CYDEV_FLASH_SIZE != 0x00040000u
 #error "This design is specifically targeted to parts with 256k of flash. Please change project device to BLE\
 silicon that has 256K Flash array. For example CY8C4248LQI-BL483."
#endif

#if defined(__ARMCC_VERSION)

extern unsigned long Image$$DATA$$ZI$$Limit;
__attribute__ ((section(".bootloaderruntype"), zero_init))
extern volatile uint32 CyReturnToBootloaddableAddress;

#endif /* __ARMCC_VERSION */

#endif /* BLE_OTA_EP_OTA_MANDATORY_H_ */

/* [] END OF FILE */
