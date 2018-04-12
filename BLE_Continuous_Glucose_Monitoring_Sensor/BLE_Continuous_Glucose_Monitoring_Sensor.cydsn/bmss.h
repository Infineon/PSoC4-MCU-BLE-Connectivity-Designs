/*******************************************************************************
* File Name: bmss.h
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

#if !defined(BMSS_H)
#define BMSS_H

#include "main.h"
    
#define CYBLE_BMS_BMFT_SIZE        (3u)
#define CYBLE_BMS_BMFT_FIRST_BYTE  (0u)
#define CYBLE_BMS_BMFT_SECOND_BYTE (1u)
#define CYBLE_BMS_BMFT_THIRD_BYTE  (2u)
    
#define CYBLE_BMS_AUTH_CODE_SIZE   (6u)


/***************************************
*      API function prototypes
***************************************/
void BmsInit(void);
void BmsCallBack(uint32 event, void* eventParam);
void BmsProcess(void);


/***************************************
*      External data references
***************************************/
extern CYBLE_GAP_BONDED_DEV_ADDR_LIST_T bdList;
extern CYBLE_GAP_BD_ADDR_T peerBdAddr;
extern uint8 unbond;
extern uint8 bmcp;

#endif /* BMSS_H  */

/* [] END OF FILE */
