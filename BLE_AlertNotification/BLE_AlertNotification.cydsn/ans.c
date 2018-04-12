/*******************************************************************************
* File Name: ans.c
*
* Version: 1.0
*
* Description:
*  This file contains ANS callback handler function.
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
*
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>
#include <stdio.h>
#include "ans.h"
#include "common.h"


/*******************************************************************************
* Function Name: AnsServiceAppEventHandler
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the BLE Component,
*  which are specific to Alert Notification Service.
*
* Parameters:
*  uint8 event:       ANS event.
*  void* eventParams: Data structure specific to event.
*
* Return:
*  None
*
*******************************************************************************/
void AnsServiceAppEventHandler(uint32 event, void *eventParam)
{
    uint8 i;
    uint16 rdCategories;
    CYBLE_ANS_CHAR_VALUE_T *rdCharVal;

    switch(event)
    {
    /***************************************
    *        ANS Client events
    ***************************************/
    case CYBLE_EVT_ANSC_READ_CHAR_RESPONSE:

        DBG_PRINTF("\r\n");
        rdCharVal = (CYBLE_ANS_CHAR_VALUE_T *) eventParam;

        /* Get value of Server's supported categories */
        MakeWordFromBytePtr(rdCharVal->value->val, &rdCategories);

        if(CYBLE_ANS_SUPPORTED_NEW_ALERT_CAT == rdCharVal->charIndex)
        {
            DBG_PRINTF("Supported New Alert Category Characteristic read response\r\n");
        }
        else
        {
            DBG_PRINTF("Unread Alert Category Characteristic read response \r\n");
        }

        /* Current Client supports only "Email", "Missed Call" and "SMS/MMS" New Alert
        * and Unread Alert categories.
        */
        supportedCategories = rdCategories & ANS_CLIENT_SUPPORTED_CATEGORIES;

        DBG_PRINTF("Read Characteristic value is following:\r\n");
        DBG_PRINTF("Category ID Bit Mask 0: %x\r\n", rdCharVal->value->val[0u]);
        DBG_PRINTF("Category ID Bit Mask 1: %x\r\n", rdCharVal->value->val[1u]);
        break;

    case CYBLE_EVT_ANSC_WRITE_CHAR_RESPONSE:
        DBG_PRINTF("\r\n");
        /* Confirmation of successful characteristic write operation */
        DBG_PRINTF("Alert Notification Control Point Characteristic was written successfully.\r\n");
        break;

    case CYBLE_EVT_ANSC_WRITE_DESCR_RESPONSE:
        DBG_PRINTF("\r\n");
        /* Confirmation of successful characteristic descriptor write operation */
        DBG_PRINTF("ANS Characteristic's Descriptor was written successfully.\r\n");
        break;

    case CYBLE_EVT_ANSC_NOTIFICATION:

        rdCharVal = (CYBLE_ANS_CHAR_VALUE_T *) eventParam;

        DBG_PRINTF("\r\n");
        if(CYBLE_ANS_NEW_ALERT == rdCharVal->charIndex)
        {
            DBG_PRINTF("New Alert Characteristic notification received.\r\n");
        }
        else
        {
            DBG_PRINTF("Unread Alert Status Characteristic notification received.\r\n");
        }

        /* Verify notified data if it fits to categories Client supports */
        if((CYBLE_ANS_CAT_ID_INSTANT_MESSAGE >= rdCharVal->value->val[0u]) &&
            ((((uint16) 1u) << rdCharVal->value->val[0u]) & ANS_CLIENT_SUPPORTED_CATEGORIES))
        {
            /* Print information about received value and turn on the corresponding LED.
            * Red LED = Missed call, Green LED = Email, Blue LED = SMS/MMS.
            */
            DBG_PRINTF("Notified value is following:\r\n");

            switch(rdCharVal->value->val[0u])
            {
                case CYBLE_ANS_CAT_ID_EMAIL:
                    DBG_PRINTF("Category ID - Email.\r\n");
                    Adv_Green_LED_Write(LED_ON);
                    break;
                case CYBLE_ANS_CAT_ID_MISSED_CALL:
                    DBG_PRINTF("Category ID - Missed Call.\r\n");
                    Disc_Red_LED_Write(LED_ON);
                    break;
                case CYBLE_ANS_CAT_ID_SMS_MMS:
                    DBG_PRINTF("Category ID - SMS/MMS.\r\n");
                    Sms_Blue_LED_Write(LED_ON);
                    break;
                default:
                    break;
            }

            DBG_PRINTF("Number of alerts: %x.\r\n", rdCharVal->value->val[1u]);

            if(rdCharVal->value->len > 2u)
            {
                DBG_PRINTF("Text: \"");

                for(i = 2u; i < rdCharVal->value->len; i++)
                {
                    DBG_PRINTF("%c", rdCharVal->value->val[i]);
                }
                DBG_PRINTF("\".\r\n");
            }
        }
        else
        {
            DBG_PRINTF("Notified category is not supported.\r\n");
        }
        break;

    case CYBLE_EVT_ANSC_READ_DESCR_RESPONSE:
        /* Not used in this example */
        break;

    /***************************************
    *        ANS Server events
    ***************************************/
    case CYBLE_EVT_ANSS_NOTIFICATION_ENABLED:
        break;
    case CYBLE_EVT_ANSS_NOTIFICATION_DISABLED:
        break;
    case CYBLE_EVT_ANSS_CHAR_WRITE:
        break;

    default:
        DBG_PRINTF("\r\n");
        DBG_PRINTF("Unhandled service event was received\r\n");
        break;
    }
}


/* [] END OF FILE */
