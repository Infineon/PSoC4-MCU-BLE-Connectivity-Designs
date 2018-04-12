/*******************************************************************************
* File Name: main.h
*
* Version 1.0
*
* Description:
*  Contains the function prototypes and constants available to the example
*  project.
*
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>
#include <stdio.h>

#define ENABLED                     (1u)
#define DISABLED                    (0u)

/***************************************
* Conditional Compilation Parameters
***************************************/
#define DEBUG_UART_ENABLED          ENABLED
#define DEBUG_UART_FULL             DISABLED


/***************************************
*           API Constants
***************************************/
#define CYBLE_MAX_ADV_DEVICES       10u

#define LED_ON                      (1u)
#define LED_OFF                     (0u)

#define LED_TIMEOUT                 (10u)              /* Сounts in hundreds of ms */

#define STATE_INIT                   0u
#define STATE_CONNECTING             1u
#define STATE_DISCONNECTED           2u
#define STATE_CONNECTED              3u

/* IPSP defines */
#define LE_DATA_CREDITS_IPSP         (1000u)
/* Set the watermark to half of the total credits to be used */
#define LE_WATER_MARK_IPSP           (LE_DATA_CREDITS_IPSP/2u)

#define L2CAP_MAX_LEN                (CYBLE_L2CAP_MTU - 2u)


/***************************************
*        External Function Prototypes
***************************************/
void ShowValue(CYBLE_GATT_VALUE_T *value);
void Set32ByPtr(uint8 ptr[], uint32 value);
void ShowError(void);
void UpdateLedState(void);


/***************************************
*        Macros
***************************************/
#if (DEBUG_UART_ENABLED == ENABLED)
    #define DBG_PRINTF(...)          (printf(__VA_ARGS__))
#else
    #define DBG_PRINTF(...)
#endif /* (DEBUG_UART_ENABLED == ENABLED) */


/* [] END OF FILE */
