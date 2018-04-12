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
#include "cgmss.h"
#include "bmss.h"

/*******************************************************************************
* Enumerations
*******************************************************************************/
typedef enum
{
    AUTHENTICATION_NOT_CONNECTED,
    AUTHENTICATION_WAITING_FOR_PASSKEY,
    AUTHENTICATION_PASSKEY_ENTERED,
    AUTHENTICATION_PASSKEY_YES_NO,    
    AUTHENTICATION_COMPLETE_BONDING_REQD,
    AUTHENTICATION_BONDING_COMPLETE
} AUTHENTICATION_STATE;

#define LED_ON                      (0u)
#define LED_OFF                     (1u)
    
#define PASSKEY                     (0x04u)

#define CYBLE_CONN_INTRV_TO_MS  (5.0 / 4.0)

/***************************************
*      API Function Prototypes
***************************************/
void StartAdvertisement(void);
void AuthenticationHandler(void);

/***************************************
*      External data references
***************************************/
extern CYBLE_API_RESULT_T apiResult;
extern uint16 i;
extern uint8 flag;
    
#endif /* MAIN_H */

/* [] END OF FILE */
