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

#include "project.h"
#include <stdio.h>


/***************************************
* Conditional Compilation Parameters
***************************************/
#define YES                                 (1u)
#define NO                                  (0u)

#define ENABLED                             (1u)
#define DISABLED                            (0u)

#define DEBUG_UART_ENABLED                  (ENABLED)


/***************************************
*        API Constants
***************************************/
#define LED_ON                              (0u)
#define LED_OFF                             (1u)

/* Constants for buttonState */
#define BUTTON_IS_PRESSED                   (1u)
#define BUTTON_IS_NOT_PRESSED               (0u)

#define BUTTON_PRESS_DELAY                  (1u)

#define BYTE_SHIFT                          (8u)
#define TWO_BYTES_SHIFT                     (16u)
#define THREE_BYTES_SHIFT                   (24u)

#define BYTE_0                              (0u)
#define BYTE_1                              (1u)
#define BYTE_2                              (2u)
#define BYTE_3                              (3u)

#define SI_WEIGHT_RESOLUTION                (5u)
#define SI_WEIGHT_RESOLUTION_DIVIDER        (1000u)
#define SI_HEIGHT_RESOLUTION                (1u)
#define SI_HEIGHT_RESOLUTION_DIVIDER        (1000u)

/* Maximum weight of example project simulation */
#define SI_MAX_WEIGHT                       (16000u)
#define SI_MIN_WEIGHT                       (14000u)

#define IMP_WEIGHT_RESOLUTION               (1u)
#define IMP_WEIGHT_RESOLUTION_DIVIDER       (1u)
#define IMP_HEIGHT_RESOLUTION               (1u)
#define IMP_HEIGHT_RESOLUTION_DIVIDER       (1u)

#define CM_TO_M_DIVIDER                     (100u)
#define HEIGHT_METER_MODIFIER               (10u)

#define PERCENT_MODIFIER                    (100u)

/* Maximum users for this example project */
#define MAX_USERS                           (4u)


/***************************************
*        Function Prototypes
***************************************/
void WDT_Start(void);
void WDT_Stop(void);
void AppCallBack(uint32 event, void * eventParam);
void HandleAlertIndication(void);
void SimulateWeightMeasurement(void);
void HandleLeds(void);
void WeightScaleProfileHandler(void);
void InitializeWeightScale(void);

/* Interrupt handlers */
CY_ISR_PROTO(ButtonPressInt);
CY_ISR_PROTO(Timer_Interrupt);


/***************************************
*        Macros
***************************************/
#if (DEBUG_UART_ENABLED == YES)
    #define DBG_PRINTF(...)                 (printf(__VA_ARGS__))
#else
    #define DBG_PRINTF(...)
#endif /* (DEBUG_UART_ENABLED == YES) */

#define PACK_U16(loByte, hiByte)            ((uint16) (((uint16)loByte) | (((uint16) hiByte) << BYTE_SHIFT)))

#define CONVERT_TO_KILOGRAMS_INT(weight)    (((weight) * SI_WEIGHT_RESOLUTION)/SI_WEIGHT_RESOLUTION_DIVIDER)
#define CONVERT_TO_KILOGRAMS_REM(weight)    ((((weight) * SI_WEIGHT_RESOLUTION * 100u) /\
                                                SI_WEIGHT_RESOLUTION_DIVIDER) % 100u)


/***************************************
* External data references
***************************************/
extern CYBLE_CONN_HANDLE_T          connectionHandle;
extern volatile uint32              mainTimer;
extern uint32                       prevMainTimer;
extern uint16                       essChangeIndex;
extern uint8                        dataLength;
extern uint8                        userIndex;


/* [] END OF FILE */
