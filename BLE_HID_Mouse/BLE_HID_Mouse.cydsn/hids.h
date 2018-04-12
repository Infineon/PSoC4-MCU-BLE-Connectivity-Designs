/*******************************************************************************
* File Name: hids.h
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


/***************************************
*          Constants
***************************************/

/* Mouse movement states */
#define MOUSE_DOWN                  (0u)  
#define MOUSE_LEFT                  (1u)
#define MOUSE_UP                    (2u)
#define MOUSE_RIGHT                 (3u)
#define MOUSE_LB                    (1u)

#define MOUSE_DATA_LEN              (3u)
#define POSMASK                     (0x03u)     /* Mouse position state mask */
#define BOX_SIZE                    (128u)      /* Transfers per side of the box */
#define CURSOR_STEP                 (3)         /* Step size */


/***************************************
*       Function Prototypes
***************************************/
void HidsCallBack(uint32 event, void *eventParam);
void HidsInit(void);
void SimulateMouse(void);


/***************************************
* External data references
***************************************/
extern uint16 mouseSimulation;
extern uint8 protocol;  
extern uint8 suspend;
extern uint32 mouseTimer;


/* [] END OF FILE */
