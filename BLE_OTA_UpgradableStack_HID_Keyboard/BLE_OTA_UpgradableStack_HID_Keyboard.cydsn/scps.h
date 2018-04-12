/*******************************************************************************
* File Name: scps.h
*
* Version 1.20
*
* Description:
*  Contains the function prototypes and constants available to the example
*  project.
*
********************************************************************************
* Copyright 2014-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#ifndef BLE_OTA_EP_SCPS_H_
#define BLE_OTA_EP_SCPS_H_

#include <project.h>


/***************************************
*       Function Prototypes
***************************************/
void ScpsCallBack (uint32 event, void *eventParam);
void ScpsInit(void);


/***************************************
* External data references
***************************************/
extern uint16 requestScanRefresh;
extern uint16 scanInterval;
extern uint16 scanWindow;

#endif /* BLE_OTA_EP_SCPS_H_ */

/* [] END OF FILE */
