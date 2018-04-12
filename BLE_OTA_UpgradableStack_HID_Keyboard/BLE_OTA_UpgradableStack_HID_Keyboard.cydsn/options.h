/*******************************************************************************
* File Name: options.h
*
* Version: 1.20
*
* Description:
*  Provides project configuration options.
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

#ifndef BLE_OTA_EP_OPTIONS_H_
#define BLE_OTA_EP_OPTIONS_H_

#include "cytypes.h"

#define NO                      (0u)
#define YES                     (1u)

/*******************************************************************************
* Following section contains bootloadable project compile-time options.
*******************************************************************************/
#define DEBUG_UART_ENABLED      (YES)

/* YES - use printf, NO - use UART_PutString
 * e.g. DBG_PRINTF("message %d\r\n", i) will transform to UART_PutString("message %d\r\n") */
#define DEBUG_UART_USE_PRINTF_FORMAT  (YES)
#define PRINT_BOUNDING_DATA     (NO)

#endif /* BLE_OTA_EP_OPTIONS_H_ */

/* [] END OF FILE */
