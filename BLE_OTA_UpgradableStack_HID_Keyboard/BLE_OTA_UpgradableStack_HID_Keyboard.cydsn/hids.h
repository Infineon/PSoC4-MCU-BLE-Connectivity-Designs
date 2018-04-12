/*******************************************************************************
* File Name: hids.h
*
* Version 1.20
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

#ifndef BLE_OTA_EP_HIDS_H_
#define BLE_OTA_EP_HIDS_H_

#include <project.h>


/***************************************
*          Constants
***************************************/

#define KEYBOARD_TIMEOUT            (10u)     /* Сounts in hundreds of ms */

/* Keyboard scan codes for notification defined in section 
*  10 Keyboard/Keypad Page of HID Usage Tables spec ver 1.12 
*/
#define SIM_KEY_MIN                 (4u)        /* Minimum simulated key 'a' */
#define SIM_KEY_MAX                 (40u)       /* Maximum simulated key '0' */
#define KEYBOARD_JITTER_SIZE        (1u)
#define NUM_LOCK                    (0x53u)
#define CAPS_LOCK                   (0x39u)
#define SCROLL_LOCK                 (0x47u)
/* LED codes received from HOST through output report 
*  Defined in section 11 LED Page of HID Usage Tables spec ver 1.12 
*/
#define NUM_LOCK_LED                (0x01u)
#define CAPS_LOCK_LED               (0x02u)
#define SCROLL_LOCK_LED             (0x04u)
#define KEYBOARD_DATA_SIZE          (8u)


/***************************************
*       Function Prototypes
***************************************/
void HidsCallBack(uint32 event, void *eventParam);
void HidsInit(void);
void SimulateKeyboard(void);


/***************************************
* External data references
***************************************/
extern uint16 keyboardSimulation;
extern uint8 protocol;  
extern uint8 suspend;

#endif /* BLE_OTA_EP_HIDS_H_ */

/* [] END OF FILE */
