/*******************************************************************************
* File Name: wss.c
*
* Version: 1.0
*
* Description:
*  This file contains routines related to Weight Scale Service.
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

#include "common.h"
#include "wss.h"


/***************************************
*        Global Variables
***************************************/
uint8                      isWssIndicationEnabled = NO;
uint8                      isWssIndicationPending = NO;
WSS_MEASUREMENT_VALUE_T    weightMeasurement[MAX_USERS];
WSS_MEASUREMENT_VALUE_T    weightMeasurementDef;
uint8                      wssIndData[WSS_WS_MEASUREMENT_MAX_DATA_SIZE];
uint32                     wssFeature;


/*******************************************************************************
* Function Name: WssCallBack
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the BLE Component
*  specific to Weight Scale Service.
*
* Parameters:
*  event - A WSS event.
*  eventParams - The data structure specific to an event received.
*
*******************************************************************************/
void WssCallBack(uint32 event, void *eventParam)
{
    switch(event)
    {

    /****************************************************
    *              WSS Server events
    ****************************************************/
    /* WSS Server - Indication for Weight Scale Service Characteristic
        was enabled. Parameter of this event is structure 
        of CYBLE_WSS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_WSSS_INDICATION_ENABLED:
        DBG_PRINTF("CYBLE_EVT_WSSS_INDICATION_ENABLED\r\n");
        isWssIndicationEnabled = YES;
        break;

    /* WSS Server - Indication for Weight Scale Service Characteristic
        was disabled. Parameter of this event is structure 
        of CYBLE_WSS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_WSSS_INDICATION_DISABLED:
        DBG_PRINTF("CYBLE_EVT_WSSS_INDICATION_DISABLED\r\n");
        isWssIndicationEnabled = NO;
        break;

    /* WSS Server - Weight Scale Service Characteristic
        Indication was confirmed. Parameter of this event
        is structure of CYBLE_WSS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_WSSS_INDICATION_CONFIRMED:
        DBG_PRINTF("CYBLE_EVT_WSSS_INDICATION_CONFIRMED\r\n");
        break;

    /****************************************************
    *               WSS Client events
    * These events are not active for the Server device
    * operation. They are added as a template for the
    * Client mode operation to ease the user experience
    * with Weight Scale Service.
    ****************************************************/
    /* WSS Client - Weight Scale Service Characteristic
        Indication was received. Parameter of this event
        is structure of CYBLE_WSS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_WSSC_INDICATION:
        
        break;

    /* WSS Client - Read Response for Read Request of Weight Scale 
        Service Characteristic value. Parameter of this event
        is structure of CYBLE_WSS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_WSSC_READ_CHAR_RESPONSE:
        
        break;

    /* WSS Client - Read Response for Read Request of Weight Scale
        Service Characteristic Descriptor Read request. 
        Parameter of this event is structure of
        CYBLE_WSS_DESCR_VALUE_T type.
    */
    case CYBLE_EVT_WSSC_READ_DESCR_RESPONSE:
        
        break;

    /* WSS Client - Write Response for Write Request of Weight Scale
        Service Characteristic Configuration Descriptor value.
        Parameter of this event is structure of 
        CYBLE_WSS_DESCR_VALUE_T type.
    */
    case CYBLE_EVT_WSSC_WRITE_DESCR_RESPONSE:
        
        break;

	default:
        DBG_PRINTF("Unrecognised WSS event.\r\n");
	    break;
    }
}


/*******************************************************************************
* Function Name: WssInit
********************************************************************************
*
* Summary:
*  Initializes the variables related to the Weight Scale Service's 
*  characteristics with values from the BLE component customizer's GUI. 
*
*******************************************************************************/
void WssInit(void)
{
    uint8 count = 0u;
    uint8 i;
    CYBLE_API_RESULT_T apiResult;
    uint8 rdData[WSS_WS_MEASUREMENT_MAX_DATA_SIZE]; 
    
    /* Register event handler for WSS specific events */
    CyBle_WssRegisterAttrCallback(WssCallBack);
    
    /* Read initial values of Weight Feature Characteristic */
    apiResult =
        CyBle_WsssGetCharacteristicValue(CYBLE_WSS_WEIGHT_SCALE_FEATURE, WSS_WEIGHT_FEATURE_CHAR_SIZE, rdData);

    if(apiResult == CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Weight Feature Characteristic was read successfully \r\n");

        /* Get Weight Feature and store it as uint32 */
        wssFeature = (uint32) ( ((uint32) rdData[BYTE_0]) |
                                (((uint32) rdData[BYTE_1]) << BYTE_SHIFT) |
                                (((uint32) rdData[BYTE_2]) << TWO_BYTES_SHIFT) |
                                (((uint32) rdData[BYTE_3]) << THREE_BYTES_SHIFT));
    }
    else
    {
        DBG_PRINTF("Error while reading Weight Feature Characteristic. Error code: %d \r\n", apiResult);
    }

    /* Read initial values of Weight Measurement Characteristic */
    apiResult =
        CyBle_WsssGetCharacteristicValue(CYBLE_WSS_WEIGHT_MEASUREMENT, WSS_WS_MEASUREMENT_MAX_DATA_SIZE, rdData);

    if(apiResult == CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Weight Measurement Characteristic was read successfully \r\n");

        /* Fill data into Weight Measurement Characteristic into the
        * structure for easier access to Characteristic fields.
        */
        weightMeasurementDef.flags = rdData[count++];
        weightMeasurementDef.weightKg = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        weightMeasurementDef.weightLb = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        weightMeasurementDef.year = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        weightMeasurementDef.month = rdData[count++];
        weightMeasurementDef.day = rdData[count++];
        weightMeasurementDef.hour = rdData[count++];
        weightMeasurementDef.minutes = rdData[count++];
        weightMeasurementDef.seconds = rdData[count++];
        weightMeasurementDef.userId = rdData[count++];
        weightMeasurementDef.bmi = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        weightMeasurementDef.heightM = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        weightMeasurementDef.heightIn = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        
        for(i = 0u; i < MAX_USERS; i++)
        {
            memcpy(&weightMeasurement[i], &weightMeasurementDef, sizeof(weightMeasurementDef));
        }
    }
    else
    {
        DBG_PRINTF("Error while reading Weight Measurement Characteristic. Error code: %d \r\n", apiResult);
    }
}


/*******************************************************************************
* Function Name: WssPackIndicationData
********************************************************************************
*
* Summary:
*  Packs the Weight Measurement Characteristic value data to be indicated. The
*  size of the buffer should be not less than WSS_WS_MEASUREMENT_MAX_DATA_SIZE.
* 
*
* Parameters:
*  pData -       The pointer to the buffer where indication data will be stored.
*  length -      The length of a buffer. After the function execution this parameter
*                will contain the actual length of bytes written to the buffer.
*  bMeasurement -The pointer to the Weight Measurement Characteristic value.
*
* Return
*  WSS_RET_SUCCESS - Success.
*  WSS_RET_FAILURE - Failure. The buffer allocated for indication data is too
*                    small.
*
*******************************************************************************/
uint8 WssPackIndicationData(uint8 *pData, uint8 *length, WSS_MEASUREMENT_VALUE_T *wMeasurement)
{
    uint8 i;
    uint8 result = WSS_RET_FAILURE;
    uint16 flagMask;
    uint8 currLength = 0u;

    if((*length != 0u) && (*length >= WSS_WS_MEASUREMENT_MAX_DATA_SIZE))
    {
        pData[currLength++] = LO8(wMeasurement->flags);
        
        if((wMeasurement->flags & WSS_MEASUREMENT_UNITS_IMPERIAL) != 0u)
        {
            pData[currLength++] = LO8(wMeasurement->weightLb);
            pData[currLength++] = HI8(wMeasurement->weightLb);
        }
        else
        {
            pData[currLength++] = LO8(wMeasurement->weightKg);
            pData[currLength++] = HI8(wMeasurement->weightKg);
        }

        for(i = 1u; i < WSS_FLAGS_COUNT; i++)
        {
            /* 'flagMask' contains mask for bit in the wMeasurement->flags.
            * Used for parsing flags and adding respective data to indication. 
            */
            flagMask = 1u << i;
            
            switch(wMeasurement->flags & flagMask)
            {
            /* Add time stamp data */
            case WSS_MEASUREMENT_TIME_STAMP_PRESENT:
                pData[currLength++] = LO8(wMeasurement->year);
                pData[currLength++] = HI8(wMeasurement->year);
                pData[currLength++] = wMeasurement->month;
                pData[currLength++] = wMeasurement->day;
                pData[currLength++] = wMeasurement->hour;
                pData[currLength++] = wMeasurement->minutes;
                pData[currLength++] = wMeasurement->seconds;
                break;
            /* Add user ID data */
            case WSS_USER_ID_PRESENT:
                pData[currLength++] = wMeasurement->userId;
                break;
            /* Add BMI and height data */
            case WSS_BMI_AND_HEIGHT_PRESENT:
                pData[currLength++] = LO8(wMeasurement->bmi);
                pData[currLength++] = HI8(wMeasurement->bmi);
                    
                if((wMeasurement->flags & WSS_MEASUREMENT_UNITS_IMPERIAL) != 0u)
                {
                    pData[currLength++] = LO8(wMeasurement->heightIn);
                    pData[currLength++] = HI8(wMeasurement->heightIn);
                }
                else
                {
                    pData[currLength++] = LO8(wMeasurement->heightM);
                    pData[currLength++] = HI8(wMeasurement->heightM);
                }
                break;
            default:
                /* Feature is disabled */
                break;
            }
        }

        if(currLength <= *length)
        {
            *length = currLength;
            result = WSS_RET_SUCCESS;
        }
    }

    return(result);
}


/* [] END OF FILE */
