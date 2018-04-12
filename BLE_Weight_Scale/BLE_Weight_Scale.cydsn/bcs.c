/*******************************************************************************
* File Name: bcs.c
*
* Version: 1.0
*
* Description:
*  This file contains routines related to Body Composition Service.
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
#include "bcs.h"


/***************************************
*        Global Variables
***************************************/
uint8                      isBcsIndicationEnabled = NO;
uint8                      isBcsIndicationPending = NO;
BCS_MEASUREMENT_VALUE_T    bodyMeasurement[MAX_USERS];
uint8                      bcsIndData[BCS_BC_MEASUREMENT_MAX_DATA_SIZE];
uint32                     bcsFeature;


/*******************************************************************************
* Function Name: BcsCallBack
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the BLE Component
*  specific to Body Composition Service.
*
* Parameters:
*  event - A BCS event.
*  eventParams - The data structure specific to an event received.
*
*******************************************************************************/
void BcsCallBack(uint32 event, void *eventParam)
{
    switch(event)
    {
    /****************************************************
    *              BCS Server events
    ****************************************************/
    /* BCS Server - Indication for Body Composition Service Characteristic
        was enabled. Parameter of this event is structure 
        of CYBLE_BCS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_BCSS_INDICATION_ENABLED:
        DBG_PRINTF("CYBLE_EVT_BCSS_INDICATION_ENABLED\r\n");
        isBcsIndicationEnabled = YES;
        break;

    /* BCS Server - Indication for Body Composition Service Characteristic
        was disabled. Parameter of this event is structure 
        of CYBLE_BCS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_BCSS_INDICATION_DISABLED:
        DBG_PRINTF("CYBLE_EVT_BCSS_INDICATION_DISABLED\r\n");
        isBcsIndicationEnabled = NO;
        break;

    /* BCS Server - Body Composition Service Characteristic
        Indication was confirmed. Parameter of this event
        is structure of CYBLE_BCS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_BCSS_INDICATION_CONFIRMED:
        DBG_PRINTF("CYBLE_EVT_BCSS_INDICATION_CONFIRMED\r\n");
        break;

    /****************************************************
    *               BCS Client events
    * These events are not active for the Server device
    * operation. They are added as a template for the
    * Client mode operation to ease the user experience
    * with the Body Composition Service.
    ****************************************************/
    /* BCS Client - Body Composition Service Characteristic
        Indication was received. Parameter of this event
        is structure of CYBLE_BCS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_BCSC_INDICATION:
        
        break;
    
    /* BCS Client - Read Response for Read Request of Body Composition 
        Service Characteristic value. Parameter of this event
        is structure of CYBLE_BCS_CHAR_VALUE_T type.
    */
    case CYBLE_EVT_BCSC_READ_CHAR_RESPONSE:
        
        break;

    /* BCS Client - Read Response for Read Request of Body Composition
        Service Characteristic Descriptor Read request. 
        Parameter of this event is structure of
        CYBLE_BCS_DESCR_VALUE_T type.
    */
    case CYBLE_EVT_BCSC_READ_DESCR_RESPONSE:
        
        break;

    /* BCS Client - Write Response for Write Request of Body Composition
        Service Characteristic Configuration Descriptor value.
        Parameter of this event is structure of 
        CYBLE_BCS_DESCR_VALUE_T type.
    */
    case CYBLE_EVT_BCSC_WRITE_DESCR_RESPONSE:
        
        break;

	default:
        DBG_PRINTF("Unrecognised BCS event.\r\n");
        
	    break;
    }
}


/*******************************************************************************
* Function Name: BcsInit
********************************************************************************
*
* Summary:
*  Initializes the variables related to Body Composition Service's 
*  characteristics with values from the BLE component customizer's GUI. 
*
*******************************************************************************/
void BcsInit(void)
{
    uint8 count = 0u;
    CYBLE_API_RESULT_T apiResult;
    uint8 rdData[BCS_BC_MEASUREMENT_MAX_DATA_SIZE]; 
    
    /* Register event handler for BCS specific events */
    CyBle_BcsRegisterAttrCallback(BcsCallBack);
    
    /* Read initial values of Body Composition Feature Characteristic */
    apiResult =
        CyBle_BcssGetCharacteristicValue(CYBLE_BCS_BODY_COMPOSITION_FEATURE, BCS_BC_FEATURE_CHAR_SIZE, rdData);

    if(apiResult == CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Body Composition Feature Characteristic was read successfully \r\n");

        /* Get Body Composition Feature and store it as uint32 */
        bcsFeature = (uint32) ( ((uint32) rdData[BYTE_0]) |
                                (((uint32) rdData[BYTE_1]) << BYTE_SHIFT) |
                                (((uint32) rdData[BYTE_2]) << TWO_BYTES_SHIFT) |
                                (((uint32) rdData[BYTE_3]) << THREE_BYTES_SHIFT));
    }
    else
    {
        DBG_PRINTF("Error while reading Body Composition Feature Characteristic. Error code: %d \r\n", apiResult);
    }

    /* Read initial values of Body Composition Measurement Characteristic */
    apiResult =
        CyBle_BcssGetCharacteristicValue(CYBLE_BCS_BODY_COMPOSITION_MEASUREMENT, BCS_BC_MEASUREMENT_MAX_DATA_SIZE, rdData);

    if(apiResult == CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Body Composition Measurement Characteristic was read successfully \r\n");

        /* Fill data into Body Composition Measurement Characteristic into
        * structure for easier access to Characteristic fields.
        */
        bodyMeasurement[userIndex].flags = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        bodyMeasurement[userIndex].bodyFatPercentage = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        bodyMeasurement[userIndex].year = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        bodyMeasurement[userIndex].month = rdData[count++];
        bodyMeasurement[userIndex].day = rdData[count++];
        bodyMeasurement[userIndex].hour = rdData[count++];
        bodyMeasurement[userIndex].minutes = rdData[count++];
        bodyMeasurement[userIndex].seconds = rdData[count++];
        bodyMeasurement[userIndex].userId = rdData[count++];
        bodyMeasurement[userIndex].basalMetabolism = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        bodyMeasurement[userIndex].musclePercentage = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        bodyMeasurement[userIndex].muscleMassKg = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        bodyMeasurement[userIndex].muscleMassLb = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        bodyMeasurement[userIndex].fatFreeMassKg = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        bodyMeasurement[userIndex].fatFreeMassLb = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        bodyMeasurement[userIndex].softLeanMassKg = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        bodyMeasurement[userIndex].softLeanMassLb = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        bodyMeasurement[userIndex].bodyWatherMassKg = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        bodyMeasurement[userIndex].bodyWatherMassLb = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        bodyMeasurement[userIndex].impedance = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        bodyMeasurement[userIndex].weightKg = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        bodyMeasurement[userIndex].weightLb = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        bodyMeasurement[userIndex].heightM = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
        bodyMeasurement[userIndex].heightIn = PACK_U16(rdData[count], rdData[count + 1u]);
        count+=2u;
    }
    else
    {
        DBG_PRINTF("Error while reading Body Composition Measurement Characteristic. Error code: %d \r\n", apiResult);
    }
}


/*******************************************************************************
* Function Name: BcsPackIndicationData
********************************************************************************
*
* Summary:
*  Packs the Body Composition Measurement Characteristic value data to be 
*  indicated.
*
* Parameters:
*  pData -        The pointer to the buffer where indication data will be
*                 stored.
*  length -       The length of a buffer. After the function execution this
                  parameter.
*                 will contain the actual length of bytes written to the buffer.
*  bMeasurement - The pointer to the Body Composition Measurement Characteristic 
*                 value.
*
* Return
*  WSS_RET_SUCCESS - Success.
*  WSS_RET_FAILURE - Failure. The buffer allocated for indication data is too
*                    small.
*
*******************************************************************************/
uint8 BcsPackIndicationData(uint8 *pData, uint8 *length, BCS_MEASUREMENT_VALUE_T *bMeasurement)
{
    uint8 i;
    uint8 result = 1u;
    uint16 flagMask;
    uint8 currLength = 0u;

    if(*length != 0u)
    {
        pData[currLength++] = LO8(bMeasurement->flags);
        pData[currLength++] = HI8(bMeasurement->flags);
        
        pData[currLength++] = LO8(bMeasurement->bodyFatPercentage);
        pData[currLength++] = HI8(bMeasurement->bodyFatPercentage);
        
        for(i = 1u; i < BCS_FLAGS_COUNT; i++)
        {
            /* 'flagMask' contains mask for bit in bMeasurement->flags.
            * Used for parsing flags and adding respective data to notification. 
            */
            flagMask = (uint16) (((uint16) 1u) << i);
            
            switch(bMeasurement->flags & flagMask)
            {
            /* Add time stamp data */
            case BCS_TIME_STAMP_PRESENT:
                pData[currLength++] = LO8(bMeasurement->year);
                pData[currLength++] = HI8(bMeasurement->year);
                pData[currLength++] = bMeasurement->month;
                pData[currLength++] = bMeasurement->day;
                pData[currLength++] = bMeasurement->hour;
                pData[currLength++] = bMeasurement->minutes;
                pData[currLength++] = bMeasurement->seconds;
                break;
            /* Add user ID data */
            case BCS_USER_ID_PRESENT:
                pData[currLength++] = bMeasurement->userId;
                break;
            /* Add basal metabolism data */
            case BCS_BASAL_METABOLISM_PRESENT:
                pData[currLength++] = LO8(bMeasurement->basalMetabolism);
                pData[currLength++] = HI8(bMeasurement->basalMetabolism);
                break;
            /* Add muscle percentage data */
            case BCS_MUSCLE_PERCENTAGE_PRESENT:
                pData[currLength++] = LO8(bMeasurement->musclePercentage);
                pData[currLength++] = HI8(bMeasurement->musclePercentage);
                break;
            /* Add muscle mass data */
            case BCS_MUSCLE_MASS_PRESENT:
                if((bMeasurement->flags & BCS_MEASUREMENT_UNITS_IMPERIAL) != 0u)
                {
                    pData[currLength++] = LO8(bMeasurement->muscleMassLb);
                    pData[currLength++] = HI8(bMeasurement->muscleMassLb);
                }
                else
                {
                    pData[currLength++] = LO8(bMeasurement->muscleMassKg);
                    pData[currLength++] = HI8(bMeasurement->muscleMassKg);
                }
                break;
            /* Add fat-free mass data */
            case BCS_FAT_FREE_MASS_PRESENT:
                if((bMeasurement->flags & BCS_MEASUREMENT_UNITS_IMPERIAL) != 0u)
                {
                    pData[currLength++] = LO8(bMeasurement->fatFreeMassLb);
                    pData[currLength++] = HI8(bMeasurement->fatFreeMassLb);
                }
                else
                {
                    pData[currLength++] = LO8(bMeasurement->fatFreeMassKg);
                    pData[currLength++] = HI8(bMeasurement->fatFreeMassKg);
                }
                break;
            /* Add soft-lean mass data */
            case BCS_SOFT_LEAN_MASS_PRESENT:
                if((bMeasurement->flags & BCS_MEASUREMENT_UNITS_IMPERIAL) != 0u)
                {
                    pData[currLength++] = LO8(bMeasurement->softLeanMassLb);
                    pData[currLength++] = HI8(bMeasurement->softLeanMassLb);
                }
                else
                {
                    pData[currLength++] = LO8(bMeasurement->softLeanMassKg);
                    pData[currLength++] = HI8(bMeasurement->softLeanMassKg);
                }
                break;
            /* Add body-water mass data */
            case BCS_BODY_WATER_MASS_PRESENT:
                if((bMeasurement->flags & BCS_MEASUREMENT_UNITS_IMPERIAL) != 0u)
                {
                    pData[currLength++] = LO8(bMeasurement->bodyWatherMassLb);
                    pData[currLength++] = HI8(bMeasurement->bodyWatherMassLb);
                }
                else
                {
                    pData[currLength++] = LO8(bMeasurement->bodyWatherMassKg);
                    pData[currLength++] = HI8(bMeasurement->bodyWatherMassKg);
                }
                break;
            /* Add impedance data */
            case BCS_IMPEDANCE_PRESENT:
                pData[currLength++] = LO8(bMeasurement->impedance);
                pData[currLength++] = HI8(bMeasurement->impedance);
                break;
            /* Add weight data */
            case BCS_WEIGHT_PRESENT:
                if((bMeasurement->flags & BCS_MEASUREMENT_UNITS_IMPERIAL) != 0u)
                {
                    pData[currLength++] = LO8(bMeasurement->weightLb);
                    pData[currLength++] = HI8(bMeasurement->weightLb);
                }
                else
                {
                    pData[currLength++] = LO8(bMeasurement->weightKg);
                    pData[currLength++] = HI8(bMeasurement->weightKg);
                }
                break;
            /* Add height data */
            case BCS_HEIGHT_PRESENT:
                if((bMeasurement->flags & BCS_MEASUREMENT_UNITS_IMPERIAL) != 0u)
                {
                    pData[currLength++] = LO8(bMeasurement->heightIn);
                    pData[currLength++] = HI8(bMeasurement->heightIn);
                }
                else
                {
                    pData[currLength++] = LO8(bMeasurement->heightM);
                    pData[currLength++] = HI8(bMeasurement->heightM);
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
            result = 0u;
        }
    }
    return(result);
}


/* [] END OF FILE */
