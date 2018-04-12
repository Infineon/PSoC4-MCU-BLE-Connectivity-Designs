/*******************************************************************************
* File Name: common.h
*
* Description:
*  The common BLE application header.
*
*******************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>
#include <stdio.h>
#include "ess.h"


/***************************************
* Conditional Compilation Parameters
***************************************/
#define YES                                 (1u)
#define NO                                  (0u)

#define DEBUG_UART_ENABLED                  (YES)


/***************************************
*        API Constants
***************************************/
#define LED_ON                              (0u)
#define LED_OFF                             (1u)

/* Constants for buttonState */
#define BUTTON_IS_PRESSED                   (1u)
#define BUTTON_IS_NOT_PRESSED               (0u)

/* WDT related constants */
#define WDT_COUNTER                         (CY_SYS_WDT_COUNTER1)
#define WDT_COUNTER_MASK                    (CY_SYS_WDT_COUNTER1_MASK)
#define WDT_INTERRUPT_SOURCE                (CY_SYS_WDT_COUNTER1_INT)
#define WDT_COUNTER_ENABLE                  (1u)
#define WDT_1SEC                            (32767u)
#define WDT_500MSEC                         (16383u)
#define WDT_250MSEC                         (8191u)
#define WDT_100MSEC                         (3277u)


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
#if (DEBUG_UART_ENABLED == YES)
    #define DBG_PRINTF(...)          (printf(__VA_ARGS__))
#else
    #define DBG_PRINTF(...)
#endif /* (DEBUG_UART_ENABLED == YES) */


/***************************************
* External data references
***************************************/
extern CYBLE_CONN_HANDLE_T  connectionHandle;
extern volatile uint32      mainTimer;
extern uint32               prevMainTimer;
extern uint16               essChangeIndex;


/* [] END OF FILE */
