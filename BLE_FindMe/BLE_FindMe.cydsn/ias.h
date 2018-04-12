/*******************************************************************************
* File Name: ias.h
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


/***************************************
*          API Constants
***************************************/
#define NO_ALERT           (0u)
#define MILD_ALERT         (1u)
#define HIGH_ALERT         (2u)

#define LED_TOGGLE_TIMEOUT      (100u)

extern uint8 alertLevel;

/***************************************
*       Function Prototypes
***************************************/
void IasEventHandler(uint32 event, void* eventParam);


/* [] END OF FILE */
