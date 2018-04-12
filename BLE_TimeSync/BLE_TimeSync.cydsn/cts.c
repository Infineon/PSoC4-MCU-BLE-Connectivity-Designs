/*******************************************************************************
* File Name: cts.c
*
* Version: 1.0
*
* Description:
*  This file contains CTS callback handler function.
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
#include "common.h"
#include "cts.h"


/***************************************
*        Global Variables
***************************************/
/* Initial Time equals to 00:00:00 am of 1.01.1900 */
CYBLE_CTS_CURRENT_TIME_T currentTime = {
            INIT_YEAR_LOW,
            INIT_YEAR_HIGH,
            INIT_MONTH,
            INIT_DAY,
            INIT_HOURS,
            INIT_MINUTES,
            INIT_SECONDS,
};
CYBLE_CTS_LOCAL_TIME_INFO_T localTime;
CYBLE_CTS_REFERENCE_TIME_INFO_T referenceTime;


/*******************************************************************************
* Function Name: CtsAppEventHandler
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the BLE Component,
*  which are specific to Current Time Service.
*
* Parameters:  
*  event:       Event for Current Time Service.
*  eventParams: Event param for Current Time Service.
*
* Return: 
*  None
*
*******************************************************************************/
void CtsAppEventHandler(uint32 event, void *eventParam)
{
    CYBLE_CTS_CHAR_VALUE_T *timeAttribute;
    CYBLE_GATT_VALUE_T *timeValue;
    CYBLE_CTS_CHAR_VALUE_T * ntfValParam;
    
    /* This is a CTS specific event triggered by the BLE component */
    switch(event)
    {
    /* Only operation that is required for RTC is reading the time. We are not enabling notifications
    * right now.
    */
    case CYBLE_EVT_CTSC_READ_CHAR_RESPONSE:
    
        timeAttribute = (CYBLE_CTS_CHAR_VALUE_T *) eventParam;
        
        DBG_PRINTF("CTS characteristic read response received\r\n");
        if(timeAttribute -> charIndex == CYBLE_CTS_CURRENT_TIME)
        {
            /* Copy the current time received from the time server to local data structure for RTC
            * operation.
            */
            timeValue = timeAttribute->value;
            memcpy(&currentTime, timeValue->val, timeValue->len);
            
            PrintCurrentTime();
        }
        else if(timeAttribute -> charIndex == CYBLE_CTS_LOCAL_TIME_INFO)
        {
            /* Copy the current time received from the time server to local data structure for RTC
             * operation */
            timeValue = timeAttribute->value;
            memcpy(&localTime, timeValue->val, timeValue->len);
            
            DBG_PRINTF("Local time:\r\nTime Zone : %d\r\nDST : %d\r\n", localTime.timeZone, localTime.dst);
        }
        else if(timeAttribute -> charIndex == CYBLE_CTS_REFERENCE_TIME_INFO)
        {
            /* Copy the current time received from the time server to local data structure for RTC
            * operation.
            */
            timeValue = timeAttribute->value;
            memcpy(&referenceTime, timeValue->val, timeValue->len);
            
            DBG_PRINTF("Current time:\r\nTime source : %d\r\nTime Accuracy : %d\r\nDays since update : %d\r\nHours since update : %d\r\n",
                    referenceTime.timeSource, referenceTime.timeAccuracy, referenceTime.daysSinceUpdate, 
                    referenceTime.hoursSinseUpdate);
        }
        else
        {
            DBG_PRINTF("Unknown CTS characteristic \r\n");
        }
        DBG_PRINTF("\r\n");
        break;
        
    case CYBLE_EVT_CTSC_NOTIFICATION:
        ntfValParam = (CYBLE_CTS_CHAR_VALUE_T *) eventParam;
        memcpy(&currentTime, ntfValParam->value->val, ntfValParam->value->len);
        DBG_PRINTF("Current Time Characteristic notification received\r\n");
        PrintCurrentTime();
        DBG_PRINTF("\r\n");
        break;
            
    case CYBLE_EVT_CTSC_WRITE_DESCR_RESPONSE:
        DBG_PRINTF("CTS Current Time CCCD was written successfully\r\n");
        DBG_PRINTF("\r\n");
        break;

    /***************************************
    *        NDCS Server events
    ***************************************/
    case CYBLE_EVT_CTSS_NOTIFICATION_ENABLED:
        break;
    case CYBLE_EVT_CTSS_NOTIFICATION_DISABLED:
        break;

    default:
        DBG_PRINTF("Unknown CTS event received\r\n");
        break;
    }
}


/*******************************************************************************
* Function Name: PrintCurrentTime
********************************************************************************
*
* Summary:
*  Displays the current time information on the terminal.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void PrintCurrentTime(void)
{
    DBG_PRINTF("Current time: ");
    
    if(TEN > currentTime.hours)
    {
        DBG_PRINTF("0%d:", currentTime.hours);
    }
    else
    {
        DBG_PRINTF("%d:", currentTime.hours);
    }
    
    if(TEN > currentTime.minutes)
    {
        DBG_PRINTF("0%d:", currentTime.minutes);
    }
    else
    {
        DBG_PRINTF("%d:", currentTime.minutes);
    }
    
    if(TEN > currentTime.seconds)
    {
        DBG_PRINTF("0%d: ", currentTime.seconds);
    }
    else
    {
        DBG_PRINTF("%d: ", currentTime.seconds);
    }
    
    switch(currentTime.dayOfWeek)
    {
    case MONDAY:
        DBG_PRINTF("Mon ");
        break;
    case TUESDAY:
        DBG_PRINTF("Tue ");
        break;
    case WEDNESDAY:
        DBG_PRINTF("Wed ");
        break;
    case THURSDAY:
        DBG_PRINTF("Thu");
        break;
    case FRIDAY:
        DBG_PRINTF("Fri ");
        break;
    case SATURDAY:
        DBG_PRINTF("Sat ");
        break;
     case SUNDAY:
        DBG_PRINTF("Sun ");
        break;
    default:
        DBG_PRINTF("Unknown day of week ");
        break;
    }
    DBG_PRINTF(" %d.%d.%d\r\n", currentTime.month, currentTime.day, 
                            ((uint16)currentTime.yearHigh)<< 8u | currentTime.yearLow);
}

/*******************************************************************************
* Function Name: PrintCurrentTime
********************************************************************************
*
* Summary:
*  Implements the function of a Real Time Clock. Every second it updates the 
*  current time.
*  The example project doesn't update days month and years after time
*  syncronisation. For the simulation only seconds minutes and hour are handled.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void TimeUpdate(void)
{
    /* 'needUpdate' will be used to update hours */
    static uint8 needUpdate = FALSE;
    
    /* Update seconds */
    if(CYBLE_CTS_SECONDS_MAX == currentTime.seconds)
    {
        currentTime.seconds = 0u;
    }
    else
    {
        currentTime.seconds++;
    }
    
    /* Update minutes */
    if(0u == currentTime.seconds)
    {
        if(CYBLE_CTS_MINUTES_MAX == currentTime.minutes)
        {
            currentTime.minutes = 0u;
            needUpdate = TRUE;
        }
        else
        {
            currentTime.minutes++;
        }
    }

    /* Update hours */
    if(TRUE == needUpdate)
    {
        if(CYBLE_CTS_HOURS_MAX == currentTime.hours)
        {
            currentTime.hours = 0u;
        }
        else
        {
            currentTime.hours++;
        }
        needUpdate = FALSE;
    }
}


/* [] END OF FILE */
