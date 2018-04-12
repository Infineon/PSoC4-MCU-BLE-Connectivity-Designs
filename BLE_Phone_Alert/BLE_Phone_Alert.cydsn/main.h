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
#include "passc.h"


#define LED_ON                      (0u)
#define LED_OFF                     (1u)
    
#define LED_BLINK_RATE (3u)
#define BTN_TMR_MASK   (0x01u)


/***************************************
*      API Function Prototypes
***************************************/
void StartAdvertisement(void);
void StartDiscovery(void);


/***************************************
*      External data references
***************************************/
extern volatile uint32 mainTimer;
extern CYBLE_API_RESULT_T apiResult;

    
#endif /* MAIN_H */

/* [] END OF FILE */
