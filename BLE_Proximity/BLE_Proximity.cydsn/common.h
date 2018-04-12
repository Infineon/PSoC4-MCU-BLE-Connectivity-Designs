/*******************************************************************************
* File Name: common.h
*
* Description:
*  Common BLE application header.
*
*******************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>
#include <stdio.h>
#include <math.h>
#include "lls.h"
#include "tps.h"

#define ENABLED                             (1u)
#define DISABLED                            (0u)

/***************************************
* Conditional Compilation Parameters
***************************************/
#define DEBUG_UART_ENABLED                  ENABLED


/***************************************
*           API Constants
***************************************/
#define LED_ON                              (0u)
#define LED_OFF                             (1u)

#define YES                                 (1u)
#define NO                                  (0u)

/* Constants for buttonState */
#define BUTTON_IS_PRESSED                   (1u)
#define BUTTON_IS_NOT_PRESSED               (0u)

/* WDT related constants */
#define WDT_COUNTER                 (CY_SYS_WDT_COUNTER1)
#define WDT_COUNTER_MASK            (CY_SYS_WDT_COUNTER1_MASK)
#define WDT_INTERRUPT_SOURCE        (CY_SYS_WDT_COUNTER1_INT)
#define WDT_COUNTER_ENABLE          (1u)
#define WDT_COUNT_PERIOD            (32767ul)


/***************************************
*        Function Prototypes
***************************************/
void AppCallBack(uint32 event, void * eventParam);
void HandleAlertIndication(void);
CY_ISR_PROTO(ButtonPressInt);
CY_ISR_PROTO(Timer_Interrupt);


/***************************************
*        Macros
***************************************/
#if (DEBUG_UART_ENABLED == ENABLED)
    #define DBG_PRINTF(...)          (printf(__VA_ARGS__))
#else
    #define DBG_PRINTF(...)
#endif /* (DEBUG_UART_ENABLED == ENABLED) */


/* [] END OF FILE */
