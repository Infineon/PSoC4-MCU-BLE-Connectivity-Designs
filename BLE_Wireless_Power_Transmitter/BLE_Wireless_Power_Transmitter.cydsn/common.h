/*******************************************************************************
* File Name: common.h
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  Contains the function prototypes and constants available to the example
*  project.
*
********************************************************************************
* Copyright 2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>
#include <stdio.h>

#if defined (__GNUC__)
    /* Add an explicit reference to the floating point printf library */
    /* to allow the usage of floating point conversion specifiers. */
    /* This is not linked in by default with the newlib-nano library. */
    asm (".global _printf_float");
#endif

#define ENABLED                     (1u)
#define DISABLED                    (0u)

/***************************************
* Conditional Compilation Parameters
***************************************/
#define DEBUG_UART_ENABLED          ENABLED


/***************************************
*           API Constants
***************************************/

#define CYBLE_MAX_ADV_DEVICES       8u

#define LED_ON                      (1u)
#define LED_OFF                     (0u)

#define LED_TIMEOUT                 (1000u / 30u * 2u)    /* Сounts depend on scanning interval parameter */

#define STATE_INIT                   0u
#define STATE_CONNECTING             1u
#define STATE_DISCONNECTED           2u
#define STATE_CONNECTED              3u
#define STATE_CONFIGURED             4u


#define WDT_COUNTER                 (CY_SYS_WDT_COUNTER1)
#define WDT_COUNTER_MASK            (CY_SYS_WDT_COUNTER1_MASK)
#define WDT_INTERRUPT_SOURCE        (CY_SYS_WDT_COUNTER1_INT) 
#define WDT_COUNTER_ENABLE          (1u)
#define WDT_1SEC                    (32767u)


/***************************************
*        External Function Prototypes
***************************************/
void ShowValue(CYBLE_GATT_VALUE_T *value, uint8 valueType);
void Set32ByPtr(uint8 ptr[], uint32 value);
uint32 Get32ByPtr(uint8 ptr[]);
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


/***************************************
* External data references
***************************************/
extern volatile uint32 mainTimer;

/* [] END OF FILE */
