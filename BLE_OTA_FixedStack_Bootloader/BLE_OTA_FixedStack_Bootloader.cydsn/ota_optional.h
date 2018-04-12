/*******************************************************************************
* File Name: ota_optional.h
*
* Version 1.40
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

/*WDT setup values*/
#define WDT_COUNTER                         (CY_SYS_WDT_COUNTER1)
#define WDT_COUNTER_MASK                    (CY_SYS_WDT_COUNTER1_MASK)
#define WDT_INTERRUPT_SOURCE                (CY_SYS_WDT_COUNTER1_INT) 
#define WDT_COUNTER_ENABLE                  (1u)
#define WDT_TIMEOUT                         (32767u/1000u) /* 1 ms @ 32.768kHz clock */


#define LED_GREEN                           (0u)
#define LED_BLUE                            (1u)
#define LED_ADV_COLOR                       (LED_BLUE)

#if (LED_ADV_COLOR == LED_GREEN)
    #define LED_WRITE_MACRO(a)              (Advertising_LED_1_Write(a))
#else
    #define LED_WRITE_MACRO(a)              (Advertising_LED_2_Write(a))
#endif /* (LED_ADV_COLOR == LED_GREEN) */

#define LED_TIMEOUT                         (1000u)   /* Сounts of 1 millisecond */
#define LED_ON                              (0u)
#define LED_OFF                             (1u)

extern CYBLE_CONN_HANDLE_T connHandle;

CY_ISR(Timer_Interrupt);
void WDT_Start(void);
void WDT_Stop(void);
void HandleLeds(void);

#endif /* BLE_OTA_EP_OTA_OPTIONAL_H_ */

/* [] END OF FILE */
