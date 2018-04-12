/*******************************************************************************
* File Name: passc.h
*
* Version 1.0
*
* Description:
*  Phone Alert Status service related code header.
*
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(PASSC_H)
#define PASSC_H

#include "main.h"

    
#define FLAG_DSCR   (0x01u) /* set descriptor */
#define FLAG_RD     (0x02u) /* read request */
#define FLAG_WR     (0x04u) /* write request */
    
void PassCallBack(uint32 event, void* eventParam);
void PassInit(void);
void PassProcess(void);


/***************************************
*      External data references
***************************************/

extern uint8 passFlag;
extern uint16 dscr;
extern CYBLE_PASS_CHAR_INDEX_T charIndx;
extern uint8 alertStatus;
extern CYBLE_PASS_RS_T ringerSetting;
extern CYBLE_PASS_CP_T controlPoint;


#endif /* PASSC_H  */

/* [] END OF FILE */
