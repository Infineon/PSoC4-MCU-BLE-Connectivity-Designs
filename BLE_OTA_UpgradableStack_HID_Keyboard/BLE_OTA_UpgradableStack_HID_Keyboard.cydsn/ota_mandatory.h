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

#ifndef BLE_OTA_EP_OTA_MANDATORY_H_
#define BLE_OTA_EP_OTA_MANDATORY_H_

#include <cytypes.h>
#include <project.h>
#include "debug.h"


#define Loader_MD_SIZEOF                (64u)
#define LENGHT_OF_UART_ROW              (20u)
#define APP_FLAG_OFFSET                 (20u)

#define APP_PRJ_MD_BASE_ADDR            (CYDEV_FLASH_BASE + \
                                            (CYDEV_FLASH_SIZE - ((uint32)CYDEV_FLS_ROW_SIZE * 2u)))

#define STACK_MD_BASE_ADDR              (CYDEV_FLASH_BASE + \
                                            (CYDEV_FLASH_SIZE - ((uint32)CYDEV_FLS_ROW_SIZE)))

#define STACK_UPDATE_FLAG_OFFSET        (STACK_MD_BASE_ADDR + APP_FLAG_OFFSET)
#define UPDATE_FLAG_OFFSET              (APP_PRJ_MD_BASE_ADDR + APP_FLAG_OFFSET)

#define LED_ON                          (0u)
#define LED_OFF                         (1u)

#define Bootloadable_MD_BTLDB_ACTIVE_1      (0x01u)
#define Bootloadable_MD_SIZEOF              (64u)
#define Bootloadable_MD_BASE_ADDR(appId)    (CYDEV_FLASH_BASE + (CYDEV_FLASH_SIZE - ((uint32)(appId) * CYDEV_FLS_ROW_SIZE) - \
                                                                        Bootloadable_MD_SIZEOF))
#define Bootloadable_MD_BTLDB_ACTIVE_OFFSET(appId) (Bootloadable_MD_BASE_ADDR(appId) + 16u)
                                                                        

cystatus Bootloadable_WriteFlashByte(const uint32 address, const uint8 inputValue);
cystatus Bootloadable_SetActiveApplication(uint8 appId);

void AfterImageUpdate(void);
#if ((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
    cystatus Clear_ROM_Array(const uint8 eepromPtr[], uint32 byteCount);
#endif /* ((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES)) */

#endif /* BLE_OTA_EP_OTA_MANDATORY_H_ */

/* [] END OF FILE */
