/*******************************************************************************
* File Name: ans.h
*
* Version 1.0
*
* Description:
*  Contains the function prototypes and constants useb by Alert Notification
*  Service.
*
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>


/***************************************
*        API Constants
***************************************/
/* Alert Notification Control Point Characteristic indexes */
#define ANCP_COMMAND_ID_INDEX               (0u)
#define ANCP_CATEGORY_ID_INDEX              (1u)

/* Supported Client Categories */
#define ANS_CLIENT_SUPPORTED_CATEGORIES     ((((uint16) 1u) << CYBLE_ANS_CAT_ID_EMAIL) | \
                                            (((uint16) 1u) << CYBLE_ANS_CAT_ID_MISSED_CALL) | \
                                            (((uint16) 1u) << CYBLE_ANS_CAT_ID_SMS_MMS))


/***************************************
*        Function Prototypes
***************************************/

void AnsServiceAppEventHandler(uint32 event, void * eventParam);


/* [] END OF FILE */
