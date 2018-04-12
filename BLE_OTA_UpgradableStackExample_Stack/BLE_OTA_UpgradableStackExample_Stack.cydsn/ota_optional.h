/*******************************************************************************
* File Name: ota_optional.h
* Version 1.20
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
    
#include <project.h>
#include "debug.h"

#define APP_FLAG_OFFSET                 (20u)
#define STACK_FLAG_OFFSET               (10u)
#define LENGHT_OF_UART_ROW              (20u)

#define STACK_MD_BASE_ADDR              (CYDEV_FLASH_BASE + \
                                            (CYDEV_FLASH_SIZE - ((uint32)CYDEV_FLS_ROW_SIZE)))
                                                        
#define UPDATE_FLAG_OFFSET              (STACK_MD_BASE_ADDR + STACK_FLAG_OFFSET)
                                                        
#define APP_PRJ_MD_BASE_ADDR            (CYDEV_FLASH_BASE + \
                                            (CYDEV_FLASH_SIZE - ((uint32)CYDEV_FLS_ROW_SIZE * 2u)))

#define APP_UPDATE_FLAG_OFFSET          (APP_PRJ_MD_BASE_ADDR + APP_FLAG_OFFSET)

#define WARNING_TIMEOUT                 (22000u)
/* 300 [sec] = 28200 [N], timeout [sec] = SWITCHING_TIMEOUT * (0.0106 +- 0.0005) */
#define SWITCHING_TIMEOUT               (28200u)

void AfterImageUpdate(void);

#if ((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
    cystatus Clear_ROM_Array(const uint8 eepromPtr[], uint32 byteCount);
#endif /* ((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES)) */

void TimeoutImplementation(void);

#endif /* BLE_OTA_EP_OTA_OPTIONAL_H_ */

/* [] END OF FILE */
