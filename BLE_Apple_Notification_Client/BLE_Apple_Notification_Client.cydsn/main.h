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

#if !defined(MAIN_H)
#define MAIN_H
    
#include <project.h>
#include <stdio.h>
    
#include "debug.h"
    
/* Profile specific includes */
#include "ancsc.h"


#define LED_BLINK_RATE (12u)
#define BTN_TMR_MASK   (0x01u)
    
#define LED_ON                      (0u)
#define LED_OFF                     (1u)
    
#define TIMER_SW2                   (0x01u)
#define PASSKEY                     (0x02u)
    
#define SW2_ZERO_PRESSING           (0u)    
#define SW2_ONE_PRESSING            (1u)
#define SW2_TWO_PRESSING            (2u)
    
#define TIMER_STOP                  (0u)    
#define TIMER_START                 (1u)
#define SW2_DEBOUNCING_TIME         (2u)
#define SW2_POLLING_TIME            (32u)
    
/***************************************
*      API Function Prototypes
***************************************/
void StartDiscovery(void);
void StartAdvertisement(void);

/***************************************
* External data references
***************************************/
extern CYBLE_API_RESULT_T apiResult;
extern uint16 i;
extern uint8 flag;
extern uint16 cccd;
extern uint8 button;
    
#endif /* MAIN_H */

/* [] END OF FILE */
