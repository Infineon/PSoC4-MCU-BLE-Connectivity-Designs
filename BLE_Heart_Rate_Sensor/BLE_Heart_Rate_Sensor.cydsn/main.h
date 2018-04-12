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
#include "bass.h"
#include "hrss.h"


#define LED_ON                      (0u)
#define LED_OFF                     (1u)
    
#define PASSKEY                     (0x04u)


/***************************************
*      API Function Prototypes
***************************************/
void StartAdvertisement(void);


/***************************************
*      External data references
***************************************/
extern CYBLE_API_RESULT_T apiResult;
extern uint16 i;
extern uint8 flag;
    
#endif /* MAIN_H */

/* [] END OF FILE */
