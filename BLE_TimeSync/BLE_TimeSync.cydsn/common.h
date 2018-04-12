/*******************************************************************************
* File Name: common.h
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

#define TRUE                                (1u)
#define FALSE                               (0u)

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

void HandleLeds(void);
uint8 ButtonPressed(void);
void WDT_Start(void);
void AppCallBack(uint32 event, void * eventParam);
CY_ISR_PROTO(ButtonPressInt);
CY_ISR_PROTO(RTC_Interrupt);


/***************************************
*        Macros
***************************************/
#if (DEBUG_UART_ENABLED == ENABLED)
    #define DBG_PRINTF(...)          (printf(__VA_ARGS__))
#else
    #define DBG_PRINTF(...)
#endif /* (DEBUG_UART_ENABLED == ENABLED) */


/***************************************
*        External data references
***************************************/
extern CYBLE_CONN_HANDLE_T  connHandle;
extern uint8                state;
extern uint16               advBlinkDelayCount;
extern uint8                advLedState;
extern uint8                buttonState;


/* [] END OF FILE */
