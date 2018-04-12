/*******************************************************************************
* File Name: rscs.c
*
* Version: 1.0
*
* Description:
*  This file contains routines related to Running Speed and Cadence Service.
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
#include "rscs.h"


/***************************************
*        Global Variables
***************************************/
uint8                   profile = WALKING;
uint8                   rscNotificationState = DISABLED;
uint8                   rscIndicationState = DISABLED;
uint8                   rscIndicationPending = NO;
uint8                   rcsOpCode = RSC_SC_CP_INVALID_OP_CODE;
uint8                   rcsRespValue = RSC_SC_CP_INVALID_OP_CODE;
uint32                  totalDistanceCm;

/* Sensor locations supported by the device */
uint8                   rscSensors[RSC_SENSORS_NUMBER];

/* This variable contains profile simulation data */
RSC_RSC_MEASUREMENT_T   rscMeasurement;

/* This variable contains the value of RSC Feature Characteristic */
uint16                  rscFeature;

/* Contains the min. and max. cadence ranges for Running and Walking profiles */
CADENCE_MAX_MIN_T       cadenceRanges[NUM_PROFILES] = {
    {WALKING_INST_CADENCE_MAX, WALKING_INST_CADENCE_MIN},
    {RUNNING_INST_CADENCE_MAX, RUNNING_INST_CADENCE_MIN}};

/* Contains the min. and max. stride length ranges for Running and Walking 
* profiles.
*/
STRIDE_LEN_MAX_MIN_T    strideLenRanges[NUM_PROFILES] = {
    {WALKING_INST_STRIDE_LENGTH_MAX, WALKING_INST_STRIDE_LENGTH_MIN},
    {RUNNING_INST_STRIDE_LENGTH_MAX, RUNNING_INST_STRIDE_LENGTH_MIN}};

/*******************************************************************************
* Function Name: RscServiceAppEventHandler
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the BLE Component,
*  which are specific to Running Speed and Cadence Service.
*
* Parameters:  
*  uint8 event:       Running Speed and Cadence Service event.
*  void* eventParams: Data structure specific to event received.
*
* Return: 
*  None
*
*******************************************************************************/
void RscServiceAppEventHandler(uint32 event, void *eventParam)
{
    uint8 i;
    CYBLE_RSCS_CHAR_VALUE_T *wrReqParam;
    
    switch(event)
    {
    /***************************************
    *        RSCS Server events
    ***************************************/
    case CYBLE_EVT_RSCSS_NOTIFICATION_ENABLED:
        DBG_PRINTF("Notifications for RSC Measurement Characteristic are enabled\r\n");
        rscNotificationState = ENABLED;
        break;
        
    case CYBLE_EVT_RSCSS_NOTIFICATION_DISABLED:
        DBG_PRINTF("Notifications for RSC Measurement Characteristic are disabled\r\n");
        rscNotificationState = DISABLED;
		break;

    case CYBLE_EVT_RSCSS_INDICATION_ENABLED:
        DBG_PRINTF("Indications for SC Control point Characteristic are enabled\r\n");
        rscIndicationState = ENABLED;
		break;
        
    case CYBLE_EVT_RSCSS_INDICATION_DISABLED:
        DBG_PRINTF("Indications for SC Control point Characteristic are disabled\r\n");
        rscIndicationState = DISABLED;
		break;

    case CYBLE_EVT_RSCSS_INDICATION_CONFIRMATION:
        DBG_PRINTF("Indication Confirmation for SC Control point was received\r\n");
		break;
	
    case CYBLE_EVT_RSCSS_CHAR_WRITE:
        
        wrReqParam = (CYBLE_RSCS_CHAR_VALUE_T *) eventParam;
        DBG_PRINTF("Write to SC Control Point Characteristic occurred\r\n");
        DBG_PRINTF("Data length: %d\r\n", wrReqParam->value->len);
        DBG_PRINTF("Received data:");

        for(i = 0u; i < wrReqParam->value->len; i++)
        {
             DBG_PRINTF(" %x", wrReqParam->value->val[i]);
        }
         DBG_PRINTF("\r\n");
        
        rcsOpCode = wrReqParam->value->val[RSC_SC_CP_OP_CODE_IDX];

        switch(rcsOpCode)
        {
        case CYBLE_RSCS_SET_CUMMULATIVE_VALUE:
            /* Validate command length */
            if(wrReqParam->value->len == RSC_SET_CUMMULATIVE_VALUE_LEN)
            {
                if(0u != (rscFeature && RSC_FEATURE_INST_STRIDE_SUPPORTED))
                {
                    totalDistanceCm = ((wrReqParam->value->val[RSC_SC_CUM_VAL_BYTE3_IDX] << THREE_BYTES_SHIFT) |
                                        (wrReqParam->value->val[RSC_SC_CUM_VAL_BYTE2_IDX] << TWO_BYTES_SHIFT) |
                                        (wrReqParam->value->val[RSC_SC_CUM_VAL_BYTE1_IDX] << ONE_BYTE_SHIFT) |
                                        wrReqParam->value->val[RSC_SC_CUM_VAL_BYTE0_IDX]) * RSCS_CM_TO_DM_VALUE;

                    DBG_PRINTF("Set cumulative value command was received.\r\n");
                    rscMeasurement.totalDistance *= RSCS_CM_TO_DM_VALUE;
                    rcsRespValue = CYBLE_RSCS_ERR_SUCCESS;
                }
                else
                {
                    DBG_PRINTF("The procedure is not supported.\r\n");
                    rcsRespValue = CYBLE_RSCS_ERR_OP_CODE_NOT_SUPPORTED;
                }
            }
            else
            {
                rcsRespValue = CYBLE_RSCS_ERR_OPERATION_FAILED;
            }
            break;

        case CYBLE_RSCS_START_SENSOR_CALIBRATION:
            DBG_PRINTF("Start Sensor calibration command was received.\r\n");
            rcsRespValue = CYBLE_RSCS_ERR_OP_CODE_NOT_SUPPORTED;
            rcsOpCode = CYBLE_RSCS_START_SENSOR_CALIBRATION;
            break;

        case CYBLE_RSCS_UPDATE_SENSOR_LOCATION:
            /* Validate command length */
            if(wrReqParam->value->len == RSC_UPDATE_SENSOR_LOCATION_LEN)
            {
                if(0u != (rscFeature & RSC_FEATURE_MULTIPLE_SENSOR_LOC_PRESENT))
                {
                    DBG_PRINTF("Update sensor location command was received.\r\n");
                    
                    /* Check if the requested sensor location is supported */
                    if(YES == IsSensorLocationSupported(wrReqParam->value->val[RSC_SC_SENSOR_LOC_IDX]))
                    {
                        DBG_PRINTF("New Sensor location was set.\r\n");
                        /* Set requested sensor location */
                        CyBle_RscssSetCharacteristicValue(CYBLE_RSCS_SENSOR_LOCATION, 1u, 
                            &wrReqParam->value->val[RSC_SC_SENSOR_LOC_IDX]);
                        rcsRespValue = CYBLE_RSCS_ERR_SUCCESS;
                    }
                    else
                    {
                        DBG_PRINTF("The requested sensor location is not supported.\r\n");
                        /* Invalid sensor location is received */
                        rcsRespValue = CYBLE_RSCS_ERR_INVALID_PARAMETER;
                    }
                }
                else
                {
                    DBG_PRINTF("The procedure is not supported.\r\n");
                    rcsRespValue = CYBLE_RSCS_ERR_OP_CODE_NOT_SUPPORTED;
                }
            }
            else
            {
                rcsRespValue = CYBLE_RSCS_ERR_OPERATION_FAILED;
            }
            break;

        case CYBLE_RSCS_REQ_SUPPORTED_SENSOR_LOCATION:
            /* Validate command length */
            if(wrReqParam->value->len == RSC_REQ_SUPPORTED_SENSOR_LOCATION_LEN)
            {
                if(0u != (rscFeature & RSC_FEATURE_MULTIPLE_SENSOR_LOC_PRESENT))
                {
                    DBG_PRINTF("Supported sensor location command was received.\r\n");
                    rcsRespValue = CYBLE_RSCS_ERR_SUCCESS;
                }
                else
                {
                    DBG_PRINTF("The procedure is not supported.\r\n");
                    rcsRespValue = CYBLE_RSCS_ERR_OP_CODE_NOT_SUPPORTED;
                }
            }
            else
            {
                rcsRespValue = CYBLE_RSCS_ERR_OPERATION_FAILED;
            }
            break;

        default:
            DBG_PRINTF("Unsupported command.\r\n");
            break;
        }

        /* Set the flag to sent indication from main() */
        rscIndicationPending = YES;
		break;

    /***************************************
    *        RSCS Client events
    ***************************************/
    case CYBLE_EVT_RSCSC_NOTIFICATION:
        break;
    case CYBLE_EVT_RSCSC_INDICATION:
        break;
    case CYBLE_EVT_RSCSC_READ_CHAR_RESPONSE:
        break;
    case CYBLE_EVT_RSCSC_WRITE_CHAR_RESPONSE:
        break;
    case CYBLE_EVT_RSCSC_READ_DESCR_RESPONSE:
        break;
    case CYBLE_EVT_RSCSC_WRITE_DESCR_RESPONSE:
        break;

	default:
        DBG_PRINTF("Unrecognized RSCS event.\r\n");
	    break;
    }
}


/*******************************************************************************
* Function Name: InitProfile
********************************************************************************
*
* Summary:
*  Performs profile's initialization.
*
* Parameters:  
*  None.
*
* Return: 
*  None
*
*******************************************************************************/
void InitProfile(void)
{
    uint8 buff[RSC_RSC_MEASUREMENT_CHAR_SIZE];
    
    if(CyBle_RscssGetCharacteristicValue(CYBLE_RSCS_RSC_MEASUREMENT,
                                         RSC_RSC_MEASUREMENT_CHAR_SIZE,
                                         (uint8 *) &rscMeasurement) !=
            CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Failed to read the RSC Measurement value.\r\n");
    }

    totalDistanceCm = rscMeasurement.totalDistance * RSCS_CM_TO_DM_VALUE;
    
    /* Get the RSC Feature */
    /* Get the characteristic into the buffer */
    if((CyBle_RscssGetCharacteristicValue(CYBLE_RSCS_RSC_FEATURE, RSC_RSC_FEATURE_SIZE, buff)) == CYBLE_ERROR_OK)
    {
        /* Fill in the RSC_RSC_MEASUREMENT_T structure */
        rscFeature = (uint16) ((((uint16) buff[1u]) << ONE_BYTE_SHIFT) | ((uint16) buff[0u]));
    }
    else
    {
        DBG_PRINTF("Failed to read the RSC Feature value.\r\n");
    }
    
    /* Set supported sensor locations */
    rscSensors[RSC_SENSOR1_IDX] = RSC_SENSOR_LOC_IN_SHOE;
    rscSensors[RSC_SENSOR2_IDX] = RSC_SENSOR_LOC_HIP;
}


/*******************************************************************************
* Function Name: SimulateProfile
********************************************************************************
*
* Summary:
*  Simulates the Running Speed and Cadence profile. When this function is called, 
*  it is assumed that the a complete stride has occurred and it is the time to
*  update the speed and the total distance values.
*
* Parameters:  
*  None.
*
* Return: 
*  None
*
*******************************************************************************/
void SimulateProfile(void)
{
    /* Update total distance */
    totalDistanceCm += rscMeasurement.instStridelen;
    rscMeasurement.totalDistance = totalDistanceCm / RSCS_CM_TO_DM_VALUE;

    /* Calculate speed in m/s with resolution of 1/256 of second */
    rscMeasurement.instSpeed = (((uint16)(2 * rscMeasurement.instCadence * rscMeasurement.instStridelen)) << 8u) /
                    (RSCS_SEC_TO_MIN_VALUE * RSCS_CM_TO_M_VALUE);
                    
    
}


/*******************************************************************************
* Function Name: IsSensorLocationSupported
********************************************************************************
*
* Summary:
*  Verifies if the requested sensor location is supported by the device.
*
* Parameters:  
*  sensorLocation: Sensor location.
*
* Return: 
*  Status:
*   YES - the sensor is supported;
*   NO - sensor is not supported.
*
*******************************************************************************/
uint8 IsSensorLocationSupported(uint8 sensorLocation)
{
    uint8 i;
    uint8 result = NO;
    
    for(i = 0u; i < RSC_SENSORS_NUMBER; i++)
    {
        if(rscSensors[i] == sensorLocation)
        {
            result = YES;
        }
    }
    return(result);
}


/*******************************************************************************
* Function Name: HandleRscNotifications
********************************************************************************
*
* Summary:
*  Handles Notifications to the Client device.
*
* Parameters:  
*  None.
*
* Return: 
*  None
*
*******************************************************************************/
void HandleRscNotifications(void)
{
    CYBLE_API_RESULT_T apiResult;
    uint16 instSpeed;

    /* Send notification to the peer Client */
    apiResult = CyBle_RscssSendNotification(connectionHandle,
                                            CYBLE_RSCS_RSC_MEASUREMENT,
                                            RSC_RSC_MEASUREMENT_CHAR_SIZE,
                                            (uint8 *) &rscMeasurement);

    /* Update the debug info if notification is sent successfully */
    if(CYBLE_ERROR_OK == apiResult)
    {
        DBG_PRINTF("Notification is sent! \r\n");
        DBG_PRINTF("Cadence: %d, ", rscMeasurement.instCadence);

        /* Calculate the instantaneous speed in m/h */
        instSpeed = ((rscMeasurement.instSpeed * RSCS_SEC_TO_HOUR_VALUE) >> 8u) / 10u;
        (void)instSpeed;
        /* Display instantaneous speed in km/h */
        DBG_PRINTF("Speed: %d.%2.2d km/h, ", instSpeed/RSCS_10M_TO_KM_VALUE, instSpeed%RSCS_10M_TO_KM_VALUE);
        DBG_PRINTF("Stride length: %d cm, ", rscMeasurement.instStridelen);
        DBG_PRINTF("Total distance: %ld m, ", (rscMeasurement.totalDistance) / RSCS_DM_TO_M_VALUE);

        if(WALKING == profile)
        {
            DBG_PRINTF("Status: Walking \r\n");
        }
        else
        {
            DBG_PRINTF("Status: Running \r\n");
        }
    }
    else
    {
        /* Print info about occurred errors */
        if(CYBLE_ERROR_INVALID_PARAMETER == apiResult)
        {
            DBG_PRINTF("CyBle_RscssSendNotification() resulted with CYBLE_ERROR_INVALID_PARAMETER\r\n");
        }
        else if(CYBLE_ERROR_NTF_DISABLED == apiResult)
        {
            DBG_PRINTF("CyBle_RscssSendNotification() resulted with CYBLE_ERROR_NTF_DISABLED\r\n");
        }
        else
        {
            DBG_PRINTF("CyBle_RscssSendNotification() resulted with CYBLE_ERROR_INVALID_STATE\r\n");
        }
    }
}


/*******************************************************************************
* Function Name: HandleRscIndications
********************************************************************************
*
* Summary:
*  Handles SC Control Point indications to the Client device. With this 
*  indication the Client receives a response for the previously send SC Control
*  Point Procedure. Refer to the Running Speed and Cadence Profile spec. for 
*  details on the SC Control Point Procedures.
*  
* Parameters:  
*  None.
*
* Return: 
*  None
*
*******************************************************************************/
void HandleRscIndications(void)
{
    uint8 i;
    uint8 size = RSC_SC_CP_SIZE;
    CYBLE_API_RESULT_T apiResult;
    uint8 buff[RSC_SC_CP_SIZE + RSC_SENSORS_NUMBER];

    /* Handle the received SC Control Point Op Code */
    switch(rcsOpCode)
    {
    case CYBLE_RSCS_REQ_SUPPORTED_SENSOR_LOCATION:
        buff[RSC_SC_CP_RESP_VAL_IDX] = rcsRespValue;
        if(rcsRespValue == CYBLE_RSCS_ERR_SUCCESS)
        {
            for(i = 0u; i < RSC_SENSORS_NUMBER; i++)
            {
                buff[RSC_SC_CP_RESP_PARAM_IDX + i] = rscSensors[i];
            }

            /* Update the size with number of supported sensors */
            size += RSC_SENSORS_NUMBER;
        }
    case CYBLE_RSCS_START_SENSOR_CALIBRATION:
    case CYBLE_RSCS_SET_CUMMULATIVE_VALUE:
    case CYBLE_RSCS_UPDATE_SENSOR_LOCATION:
        buff[RSC_SC_CP_RESP_VAL_IDX] = rcsRespValue;
        break;

    default:
        buff[RSC_SC_CP_RESP_VAL_IDX] = CYBLE_RSCS_ERR_OP_CODE_NOT_SUPPORTED;
        break;
    }

    buff[RSC_SC_CP_RESP_OP_CODE_IDX] = CYBLE_RSCS_RESPONSE_CODE;
    buff[RSC_SC_CP_REQ_OP_CODE_IDX] = rcsOpCode;

    apiResult = CyBle_RscssSendIndication(connectionHandle, CYBLE_RSCS_SC_CONTROL_POINT, size, buff);

    if(CYBLE_ERROR_OK == apiResult)
    {
        DBG_PRINTF("CyBle_RscssSendIndication() succeeded\r\n");
    }
    else
    {
        DBG_PRINTF("CyBle_RscssSendIndication() resulted with an error. Error code: %x\r\n", apiResult);
    }
        
    /* Clear the Op Code and the indication pending flag */
    rcsOpCode = RSC_SC_CP_INVALID_OP_CODE;
    rscIndicationPending = NO;
}


/*******************************************************************************
* Function Name: UpdatePace
********************************************************************************
*
* Summary:
*  Simulates "RUNNING" or "WALKING" profile.
*
* Parameters:  
*  None.
*
* Return: 
*  None
*
*******************************************************************************/
void UpdatePace(void)
{
    /* Update stride length */
    if(rscMeasurement.instStridelen <= strideLenRanges[profile].max)
    {
        rscMeasurement.instStridelen++;
    }
    else
    {
        rscMeasurement.instStridelen = strideLenRanges[profile].min;
    }
    
    /* .. and cadence */
    if(rscMeasurement.instCadence <= cadenceRanges[profile].max)
    {
        rscMeasurement.instCadence++;
    }
    else
    {
        rscMeasurement.instCadence = cadenceRanges[profile].min;
    }
}


/* [] END OF FILE */
