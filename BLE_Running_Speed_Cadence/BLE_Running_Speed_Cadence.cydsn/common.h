/*******************************************************************************
* File Name: common.h
*
* Version 1.0
*
* Description:
*  Contains the function prototypes and constants used by the example project.
*
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>
#include <stdio.h>

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

#define DISCONNECTED                        (0u)
#define ADVERTISING                         (1u)
#define CONNECTED                           (2u)

/* Delay value to produce blinking LED */
#define BLINK_DELAY                         (2000u)

#define PACE_TIMER_VALUE                    (333u)

#define NOTIFICATION_TIMER_VALUE            (150u)


#define WALKING_PROFILE_TIMER_VALUE         (33u)
#define RUNNING_PROFILE_TIMER_VALUE         (17u)


#define WDT_COUNTER                 (CY_SYS_WDT_COUNTER1)
#define WDT_COUNTER_MASK            (CY_SYS_WDT_COUNTER1_MASK)
#define WDT_INTERRUPT_SOURCE        (CY_SYS_WDT_COUNTER1_INT) 
#define WDT_COUNTER_ENABLE          (1u)
#define WDT_1SEC                    (32767u)
#define WDT_500MSEC                 (16383u)
#define WDT_250MSEC                 (8191u)
#define WDT_100MSEC                 (3277u)

#define ONE_BYTE_SHIFT              (8u)
#define TWO_BYTES_SHIFT             (16u)
#define THREE_BYTES_SHIFT           (24u)


/***************************************
*        Function Prototypes
***************************************/
CY_ISR_PROTO(ButtonPressInt);


/***************************************
*        Macros
***************************************/
#if (DEBUG_UART_ENABLED == ENABLED)
    #define DBG_PRINTF(...)          (printf(__VA_ARGS__))
#else
    #define DBG_PRINTF(...)
#endif /* (DEBUG_UART_ENABLED == ENABLED) */


/* [] END OF FILE */
