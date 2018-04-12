/*******************************************************************************
* File Name: ess.c
*
* Version: 1.10
*
* Description:
*  This file contains routines related to Environmental Sensing Service.
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

/***************************************
*        Global Variables
***************************************/
uint8 isIndicationEnabled = NO;
uint8 isIndicationPending = NO;
uint8 isEssInitDone = NO;
uint16 indicationValue;

const uint8 defaultSensorCond[NUMBER_OF_TRIGGERS] =
    {CYBLE_ESS_TRIG_NO_LESS_THEN_TIME_INTERVAL, CYBLE_ESS_TRIG_WHEN_CHANGED,CYBLE_ESS_TRIG_TRIGGER_INACTIVE};

extern CYBLE_ESS_CHARACTERISTIC_DATA_T humidity;
extern CYBLE_ESS_CHARACTERISTIC_DATA_T windSpeed[SIZE_2_BYTES];

/*******************************************************************************
* Function Name: EssInit
********************************************************************************
*
* Summary: This function initialised parameters for Environmental Sensing Service.
*
*******************************************************************************/
void EssInit(void)
{
    windSpeed[0].EssChrIndex = CYBLE_ESS_TRUE_WIND_SPEED;
    windSpeed[0].chrInstance = CHARACTERISTIC_INSTANCE_1;
    EssInitCharacteristic(&windSpeed[0]);
    
    windSpeed[1].EssChrIndex = CYBLE_ESS_TRUE_WIND_SPEED;
    windSpeed[1].chrInstance = CHARACTERISTIC_INSTANCE_2;
    EssInitCharacteristic(&windSpeed[1]);
    
    humidity.EssChrIndex = CYBLE_ESS_HUMIDITY;
    humidity.chrInstance = CHARACTERISTIC_INSTANCE_1;
    EssInitCharacteristic(&humidity);
}

/*******************************************************************************
* Function Name: EssCallBack
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the BLE Component
*  specific to Environmental Sensing Service.
*
* Parameters:
*  uint32 event: An ESS event.
*  void* eventParams: The data structure specific to an event received.
*
*******************************************************************************/
void EssCallBack(uint32 event, void *eventParam)
{
    CYBLE_ESS_CHAR_VALUE_T * charValPtr;
    CYBLE_ESS_DESCR_VALUE_T * descrValPtr;

    switch(event)
    {
        /****************************************************
        *              ESS Server events
        ****************************************************/
        /* ESS Server - Notifications for Environmental Sensing Service
        * Characteristics were enabled. The parameter of this event is a structure of
        * the CYBLE_ESS_CHAR_VALUE_T type.
        */
        case CYBLE_EVT_ESSS_NOTIFICATION_ENABLED:
            DBG_PRINTF("CYBLE_EVT_ESSS_NOTIFICATION_ENABLED\r\n");
            charValPtr = (CYBLE_ESS_CHAR_VALUE_T *) eventParam;
            DBG_PRINTF("Char instance: %d \r\n.",charValPtr->charInstance + 1u);
            switch(charValPtr->charIndex)
            {   
            case CYBLE_ESS_TRUE_WIND_SPEED :
                windSpeed[charValPtr->charInstance].isNotificationEnabled = YES;
                break;
            
            case CYBLE_ESS_HUMIDITY :
                humidity.isNotificationEnabled = YES;
                break;
                
            default : 
                DBG_PRINTF("Unsupported characteristic. Characteristic index: %d \r\n.",charValPtr->charIndex);
                break;
            }            
            break;
            
        /* ESS Server - Notifications for Environmental Sensing Service
        * Characteristics were disabled. The parameter of this event is a structure of
        * the CYBLE_ESS_CHAR_VALUE_T type.
        */
        case CYBLE_EVT_ESSS_NOTIFICATION_DISABLED:
            DBG_PRINTF("CYBLE_EVT_ESSS_NOTIFICATION_DISABLED\r\n");
            charValPtr = (CYBLE_ESS_CHAR_VALUE_T *) eventParam;
            DBG_PRINTF("Char instance: %d \r\n",charValPtr->charInstance + 1u);
            switch(charValPtr->charIndex)
            {   
            case CYBLE_ESS_TRUE_WIND_SPEED :
                windSpeed[charValPtr->charInstance].isNotificationEnabled = NO;
                break;
            
            case CYBLE_ESS_HUMIDITY :
                humidity.isNotificationEnabled = NO;
                break;
            default : 
                DBG_PRINTF("Characteristic index is filed. Characteristic index: %d \r\n.",charValPtr->charIndex);
                break;
            }            
                
            break;

        /* ESS Server - The indication for Descriptor Value Changed Characteristic
        * was enabled. The parameter of this event is a structure of
        * the CYBLE_ESS_CHAR_VALUE_T type.
        */
        case CYBLE_EVT_ESSS_INDICATION_ENABLED:
            DBG_PRINTF("CYBLE_EVT_ESSS_INDICATION_ENABLED\r\n");
            charValPtr = (CYBLE_ESS_CHAR_VALUE_T *) eventParam;
            isIndicationEnabled = YES;
            DBG_PRINTF("\r\n");
            break;

        /* ESS Server - The indication for Descriptor Value Changed Characteristic
        * was disabled. The parameter of this event is a structure of
        * the CYBLE_ESS_CHAR_VALUE_T type.
        */
        case CYBLE_EVT_ESSS_INDICATION_DISABLED:
            DBG_PRINTF("CYBLE_EVT_ESSS_INDICATION_DISABLED\r\n");
            charValPtr = (CYBLE_ESS_CHAR_VALUE_T *) eventParam;
            isIndicationEnabled = NO;
            DBG_PRINTF("\r\n");
            break;

        /* ESS Server - The indication for Descriptor Value Changed Characteristic
        * was confirmed. The parameter of this event is a structure of 
        * the CYBLE_ESS_CHAR_VALUE_T type.
        */
        case CYBLE_EVT_ESSS_INDICATION_CONFIRMATION:
            DBG_PRINTF("CYBLE_EVT_ESSS_INDICATION_CONFIRMATION\r\n");
            break;

        /* ESS Server - A write request for Environmental Sensing Service
        * Characteristic was received. The parameter of this event is a structure of
        * the CYBLE_ESS_CHAR_VALUE_T type.
        */    
        case CYBLE_EVT_ESSS_CHAR_WRITE:
            DBG_PRINTF("CYBLE_EVT_ESSS_CHAR_WRITE\r\n");
            break;

        /* ESS Server - A write request for Environmental Sensing Service
        * characteristic descriptor was received. The parameter of this event is a structure of
        * the CYBLE_ESS_DESCR_VALUE_T type. This event is generated only when a write for
        * CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR, CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR or
        * CYBLE_ESS_ES_CONFIG_DESCR has occurred.
        */    
        case CYBLE_EVT_ESSS_DESCR_WRITE:
            DBG_PRINTF("CYBLE_EVT_ESSS_DESCR_WRITE\r\n");
            descrValPtr = (CYBLE_ESS_DESCR_VALUE_T *) eventParam;
            DBG_PRINTF(" %s Char instance: %d", CharIndexToText(descrValPtr->charIndex), descrValPtr->charInstance + 1u);
            DBG_PRINTF(" Descriptor: %d. ", descrValPtr->descrIndex);
            HandleDescriptorWriteOp(descrValPtr);
            break;

        /****************************************************
        *               ESS Client events
        * These events are not active for the Server device
        * operation. They are added as a template for the
        * Client mode operation to ease the user experience
        * with the Envirinmental Sensing Service.
        ****************************************************/
        /* ESS Client - Environmental Sensing Service Characteristic
        * Notification was received. The parameter of this event is a structure of
        * CYBLE_ESS_CHAR_VALUE_T type.
        */
        case CYBLE_EVT_ESSC_NOTIFICATION:
            /* Add code to handle notification from the Server */
            break;

        /* ESS Client - Environmental Sensing Service Characteristic
        * Indication was received. The parameter of this event is a structure of 
        * CYBLE_ESS_CHAR_VALUE_T type.
        */
        case CYBLE_EVT_ESSC_INDICATION:
            /* Add code to handle indication from the Server */
            break;

        /* ESS Client - Read Response for Read Request of Environmental Sensing 
        * Service Characteristic value. The parameter of this event is a structure of
        * CYBLE_ESS_CHAR_VALUE_T type.
        */
        case CYBLE_EVT_ESSC_READ_CHAR_RESPONSE:
            /* Add code to handle read characteristic response from the Server */
            break;

        /* ESS Client - Write Response for Write Request of Environmental Sensing 
        * Service Characteristic value. The parameter of this event is a structure of
        * CYBLE_ESS_CHAR_VALUE_T type.
        */
        case CYBLE_EVT_ESSC_WRITE_CHAR_RESPONSE:
            /* Add code to handle write characteristic response from the Server */
            break;

        /* ESS Client - Read Response for Read Request of Environmental Sensing
        * Service Characteristic Descriptor Read request. The parameter of this event
        * is a structure of CYBLE_ESS_DESCR_VALUE_T type.
        */
        case CYBLE_EVT_ESSC_READ_DESCR_RESPONSE:
            /* Add code to handle read characteristic descriptor response from the Server */
            break;

        /* ESS Client - Write Response for Write Request of Environmental Sensing
        * Service Characteristic Configuration Descriptor value. The parameter of
        * this event is a structure of  CYBLE_ESS_DESCR_VALUE_T type.
        */
        case CYBLE_EVT_ESSC_WRITE_DESCR_RESPONSE:
            /* Add code to handle write characteristic descriptor response from the Server
            */
            break;

    	default:
            DBG_PRINTF("Unrecognized ESSS event.\r\n");
            
    	    break;
    }
}


/*******************************************************************************
* Function Name: EssInitCharacteristic()
********************************************************************************
*
* Summary:
*   Initializes the Environmental Sensing Service for single characteristic.
*
* Parameters:  
*   *sensorPtr: A pointer to the sensor characteristic structure.
*
*******************************************************************************/
void EssInitCharacteristic(CYBLE_ESS_CHARACTERISTIC_DATA_T *sensorPtr)
{
    uint8 i;
    uint8 buff[SIZE_2_BYTES];
    ESS_MEASUREMENT_VALUE_T esMeasurementDescrVal;
    
     /* A temporary store for trigger settings descriptor values. The trigger settings
    * descriptor value have a variable length, refer to section 3.1.2.2 of ESS spec.
    * For this example the max. length is 4 bytes.
    */
    uint8 esTrigSettingsVal[SIZE_4_BYTES];

    
    switch(sensorPtr->EssChrIndex)
    {
        case CYBLE_ESS_TRUE_WIND_SPEED :
            sensorPtr->value = INIT_WIND_SPEED;

            switch(sensorPtr->chrInstance)
            {   
                case CHARACTERISTIC_INSTANCE_1 :
                    sensorPtr->valueMax = WIND_SPEED_MAX1;
                    sensorPtr->valueMin = WIND_SPEED_MIN1;
                    sensorPtr->valueUpdateStep = WIND_UPDATE_STEP_1;
                    break;
                
                case CHARACTERISTIC_INSTANCE_2 :
                    sensorPtr->valueMax = WIND_SPEED_MAX2;
                    sensorPtr->valueMin = WIND_SPEED_MIN2;
                    sensorPtr->valueUpdateStep = WIND_UPDATE_STEP_2;                    
                    break;
                
                default :   // Message
                    break;
            }
            break;
            
        case CYBLE_ESS_HUMIDITY :
            sensorPtr->value = INIT_HUMIDITY;
            sensorPtr->valueMax = HUMIDITY_MAX;
            sensorPtr->valueMin = HUMIDITY_MIN;
            sensorPtr->valueUpdateStep = HUMIDITY_UPDATE_STEP;
            break;
        
        default :  // Message
            
            break;    

    }
    
    sensorPtr->sensorNewDataReady = NO;
    sensorPtr->isMeasurementPeriodElapsed = NO;
    sensorPtr->isNotificationEnabled = NO;
    sensorPtr->prevValue = sensorPtr->value;
    
   
    /* Register event handler for ESS specific events */
    CyBle_EssRegisterAttrCallback(EssCallBack);

    /* Set initial value for parametr */
    CyBle_Set16ByPtr(buff, sensorPtr->value);
    (void) CyBle_EsssSetCharacteristicValue(sensorPtr->EssChrIndex, 
                                            sensorPtr->chrInstance,
                                            SIZE_2_BYTES,
                                            buff);

    /* Get initial values for ES Configuration Descriptor of parametr*/
    (void) CyBle_EsssGetCharacteristicDescriptor(sensorPtr->EssChrIndex, 
                                                 sensorPtr->chrInstance,
                                                 CYBLE_ESS_ES_CONFIG_DESCR, 
                                                 SIZE_1_BYTE, 
                                                 &sensorPtr->esConfig);
     
    for(i = 0u; i < NUMBER_OF_TRIGGERS; i++)
    {
        if (CYBLE_ERROR_GATT_DB_INVALID_ATTR_HANDLE == CyBle_EsssGetCharacteristicDescriptor(sensorPtr->EssChrIndex,
                                                     sensorPtr->chrInstance,
                                                     (CYBLE_ESS_DESCR_INDEX_T)(i + CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR1),
                                                     SIZE_4_BYTES, 
                                                     esTrigSettingsVal))
        {   /* No settings for this trigger */
            sensorPtr->valueCond[i] = CYBLE_ESS_TRIG_TRIGGER_INACTIVE;
            sensorPtr->cmpValue[i] = 0;
        }
        else 
        {   /* Trigger is active */
            sensorPtr->valueCond[i] = esTrigSettingsVal[TRIG_CONDITION_OFFSET];
            if ( sensorPtr->valueCond[i] > CYBLE_ESS_LAST_TRIG_CONDITION )
            { 
                sensorPtr->valueCond[i] = defaultSensorCond[i];
            }

            /* Pack cmpompare value to buffer to be set to GATT database */
            GetUint24(&sensorPtr->cmpValue[i], &esTrigSettingsVal[TRIG_OPERAND_OFFSET]);
                
            if( ( sensorPtr->valueCond[i] == CYBLE_ESS_TRIG_USE_FIXED_TIME_INTERVAL)
            ||  ( sensorPtr->valueCond[i] == CYBLE_ESS_TRIG_NO_LESS_THEN_TIME_INTERVAL) )
            {    /* Pack notification timeout value to buffer to be set to GATT database */
                GetUint24(&sensorPtr->ntfTimeoutVal, &esTrigSettingsVal[TRIG_OPERAND_OFFSET]);
            }
            else
            {
                sensorPtr->ntfTimeoutVal = NTF_INIT_TIMEOUT_VAL;
            }
        }
    }    

    /* Get the value of ES measurement Descriptor */
    (void) CyBle_EsssGetCharacteristicDescriptor(sensorPtr->EssChrIndex, 
                                                 sensorPtr->chrInstance,
                                                 CYBLE_ESS_ES_MEASUREMENT_DESCR, 
                                                 sizeof(esMeasurementDescrVal), 
                                                 (uint8 *) &esMeasurementDescrVal);
    /* Store the measurement period value into uint32 for easy access to it */
    GetUint24(&sensorPtr->measurementPeriod, &esMeasurementDescrVal.measurementPeriod[0u]);
    /* Store the update interval value into uint32 for easy access to it */
    GetUint24(&sensorPtr->updateIntervalValue, &esMeasurementDescrVal.updateInterval[0u]);
    
    DBG_PRINTF("\r\n* The initialised Charakteristic - %s instance #%d\r\n", CharIndexToText(sensorPtr->EssChrIndex),sensorPtr->chrInstance+1); 
    DBG_PRINTF("* Value of imitated parameter          - %d\r\n", sensorPtr->value); 
    DBG_PRINTF("* Maximum value of imitated parameter  - %d\r\n", sensorPtr->valueMax); 
    DBG_PRINTF("* Minimum value of imitated parameter  - %d\r\n", sensorPtr->valueMin); 
    DBG_PRINTF("* Step of imitated parameter changing  - %d\r\n", sensorPtr->valueUpdateStep); 
    DBG_PRINTF("* Value of ES Configuration descriptor - %s\r\n", (sensorPtr->esConfig == CYBLE_ESS_CONF_BOOLEAN_AND)?"AND":"OR"); 
    DBG_PRINTF("* Notification timeout value           - %ld\r\n", sensorPtr->ntfTimeoutVal); 
    
    for(i = 0u; i < NUMBER_OF_TRIGGERS; i++)
    {    
        DBG_PRINTF("* Value condition, Comparison value #%d - %d, %ld\r\n", i,sensorPtr->valueCond[i],sensorPtr->cmpValue[i]); 
    }
    
    DBG_PRINTF("* Measurement period in seconds        - %ld\r\n", sensorPtr->measurementPeriod); 
    DBG_PRINTF("* Update Interval in seconds           - %ld\r\n\n", sensorPtr->updateIntervalValue); 
}


/*******************************************************************************
* Function Name: HandleButtonPress()
********************************************************************************
*
* Summary:
*  Changes the value of ES Configuration Descriptor in the GATT database.
*
* Parameters:  
*   *sensorPtr: A pointer to the sensor characteristic structure.
*
*******************************************************************************/
void HandleButtonPress(CYBLE_ESS_CHARACTERISTIC_DATA_T *sensorPtr)
{
    /* Change value of ES Configuration descriptor ... */
    if(sensorPtr->esConfig == CYBLE_ESS_CONF_BOOLEAN_AND)
    {
        sensorPtr->esConfig = CYBLE_ESS_CONF_BOOLEAN_OR;
    }
    else
    {
        sensorPtr->esConfig= CYBLE_ESS_CONF_BOOLEAN_AND;
    }

    /* ... and set it to GATT database */
    (void) CyBle_EsssSetCharacteristicDescriptor(   sensorPtr->EssChrIndex,
                                                    sensorPtr->chrInstance,
                                                    CYBLE_ESS_ES_CONFIG_DESCR, 
                                                    SIZE_1_BYTE, 
                                                    &sensorPtr->esConfig);
}


/*******************************************************************************
* Function Name: HandleDescriptorWriteOp()
********************************************************************************
*
* Summary:
*  Parses the data received with CYBLE_EVT_ESSS_DESCR_WRITE and appropriately
*  updates the internal variables used for Environmental Profile simulation.
*
* Parameters:  
*  *descrValPtr: A pointer to the data received with CYBLE_EVT_ESSS_DESCR_WRITE
*                event.
*
*******************************************************************************/
void HandleDescriptorWriteOp(CYBLE_ESS_DESCR_VALUE_T *descrValPtr)
{
    uint32 i;
    
    switch (descrValPtr->descrIndex)
    { 
        /* This case is for three following conditions:
        *      CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR1,
        *      CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR2,        
        *      CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR3.
        */
        case CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR1:
        case CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR2:        
        case CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR3:         
        
            switch(descrValPtr->charIndex)
            {   
                case CYBLE_ESS_TRUE_WIND_SPEED :
                    windSpeed[descrValPtr->charInstance].valueCond[descrValPtr->descrIndex-CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR1]
                                            = descrValPtr->value->val[0u];
                    
                    windSpeed[descrValPtr->charInstance].cmpValue[descrValPtr->descrIndex-CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR1]
                                            =   ((uint32) ((descrValPtr->value->val[3u] << SHIFT_16_BITS) |
                                                (descrValPtr->value->val[2u] << SHIFT_8_BITS) | 
                                                descrValPtr->value->val[1u]));
                                                    
                    if( (descrValPtr->value->val[0u] == CYBLE_ESS_TRIG_USE_FIXED_TIME_INTERVAL)
                    ||  (descrValPtr->value->val[0u] == CYBLE_ESS_TRIG_NO_LESS_THEN_TIME_INTERVAL) )
                    {   /* Update notification timer variables with new value */
                        windSpeed[descrValPtr->charInstance].ntfTimeoutVal = 
                                                windSpeed[descrValPtr->charInstance].cmpValue[descrValPtr->descrIndex-CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR1];                   
                    }
                    
                    break;
                
                case CYBLE_ESS_HUMIDITY :
                    humidity.valueCond[descrValPtr->descrIndex-CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR1]
                                            = descrValPtr->value->val[0u];
                    
                    humidity.cmpValue[descrValPtr->descrIndex-CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR1]
                                            =   ((uint32) ((descrValPtr->value->val[3u] << SHIFT_16_BITS) |
                                                (descrValPtr->value->val[2u] << SHIFT_8_BITS) | 
                                                descrValPtr->value->val[1u]));
                                                    
                    if( (descrValPtr->value->val[0u] == CYBLE_ESS_TRIG_USE_FIXED_TIME_INTERVAL)
                    ||  (descrValPtr->value->val[0u] == CYBLE_ESS_TRIG_NO_LESS_THEN_TIME_INTERVAL) )
                    {   /* Update notification timer variables with new value */
                        humidity.ntfTimeoutVal = humidity.cmpValue[descrValPtr->descrIndex-CYBLE_ESS_ES_TRIGGER_SETTINGS_DESCR1];                   
                    }
                    
                    break;
                                                
                default : 
                    DBG_PRINTF("Character index is filed. Character index: %d \r\n.",descrValPtr->charIndex);
                    break;         
        
            }
        
            DBG_PRINTF("Received value is: ");
            for(i = 0u; i < descrValPtr->value->len; i++)
            {
                DBG_PRINTF("0x%2.2x ", descrValPtr->value->val[i]);
            }
            DBG_PRINTF("\r\n");
            isIndicationPending = YES;
            indicationValue = CYBLE_ESS_VALUE_CHANGE_SOURCE_CLIENT | CYBLE_ESS_VALUE_CHANGE_ES_TRIGGER;
            break;
        
        case CYBLE_ESS_ES_CONFIG_DESCR: 
            switch(descrValPtr->charIndex)
            {   
            case CYBLE_ESS_TRUE_WIND_SPEED :
                windSpeed[descrValPtr->charInstance].esConfig = descrValPtr->value->val[0u];
                break;
            
            case CYBLE_ESS_HUMIDITY :
                humidity.esConfig = descrValPtr->value->val[0u];
                break;
            
            default : 
                DBG_PRINTF("Character index is filed. Character index: %d \r\n.",descrValPtr->charIndex);
                break;
            }
                    
            DBG_PRINTF("Received value is: 0x%2.2x\r\n", descrValPtr->value->val[0u]);
            isIndicationPending = YES;
            indicationValue = CYBLE_ESS_VALUE_CHANGE_SOURCE_CLIENT | CYBLE_ESS_VALUE_CHANGE_ES_CONFIG;
            break;
        case CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR:
            DBG_PRINTF("Write to CYBLE_ESS_CHAR_USER_DESCRIPTION_DESCR has occurred. Value is: \"");
            for(i = 0u; i < descrValPtr->value->len; i++)
            {
                DBG_PRINTF("%c", descrValPtr->value->val[i]);
            }
            DBG_PRINTF("\"\r\n");
            isIndicationPending = YES;
            indicationValue = CYBLE_ESS_VALUE_CHANGE_SOURCE_CLIENT | CYBLE_ESS_VALUE_CHANGE_USER_DESCRIPTION;
            break;
        default:
            break;
    }
}

/*******************************************************************************
* Function Name: SimulateProfile()
********************************************************************************
*
* Summary:
*  Simulates a wind measurement based on the time periods specified in the ES 
*  Measurement descriptor. 
*
* Parameters:  
*   *sensorPtr: A pointer to the sensor characteristic structure.
*
*******************************************************************************/
void SimulateProfile(CYBLE_ESS_CHARACTERISTIC_DATA_T *sensorPtr)
{
    /* Following two condition checks are required for measurement period simulation */
    if((sensorPtr->isMeasurementPeriodElapsed == NO) && (sensorPtr->measurementPeriod <= mainTimer))
    {
        sensorPtr->isMeasurementPeriodElapsed = YES;
        DBG_PRINTF("Measurement Period for %s sensor#%d (%d s) has elapsed.\r\n",
            CharIndexToText(sensorPtr->EssChrIndex), sensorPtr->chrInstance + 1u, LO16(sensorPtr->measurementPeriod));
    }

    /* The first sensor simulates an increase in the wind speed by 1.2 m/s each
    * 15 seconds until it reaches the maximum of 80 m/s. Then the wind speed
    * falls down to the minimum of 10 m/s, and then again it is
    * increased by 1.2 m/s each 15 seconds.
    * The second sensor simulates an increase in the wind speed by 0.7 m/s each
    * 20 seconds until, it reaches the maximum of ~90 m/s. After that the speed 
    * is not updated any more holding the maximum wind speed.
    */
    if(sensorPtr->isMeasurementPeriodElapsed == YES)
    {
        if(sensorPtr->updateIntervalTimer == 0u)
        {   
            sensorPtr->prevValue = sensorPtr->value;
            
            if(sensorPtr->valueMax > sensorPtr->value)
            {
                sensorPtr->value += sensorPtr->valueUpdateStep;
            }
            else if(sensorPtr->chrInstance == CHARACTERISTIC_INSTANCE_1)
            {
                sensorPtr->value = sensorPtr->valueMin;
            }
            else
            {
                /*  Value of CHARACTERISTIC_INSTANCE_2 is not changed */
            }
            sensorPtr->updateIntervalTimer = sensorPtr->updateIntervalValue;
            sensorPtr->sensorNewDataReady = YES;
            /* Updated Change Index value as new data is available */
            essChangeIndex++;
            CyBle_EsssSetChangeIndex(essChangeIndex);
            
            DBG_PRINTF("Update Interval for %s sensor#%d (%d s) has elapsed.\r\n",
                       CharIndexToText(sensorPtr->EssChrIndex),sensorPtr->chrInstance + 1u, LO16(sensorPtr->updateIntervalTimer));
        }
    }
}


/*******************************************************************************
* Function Name: HandleNotificaion()
********************************************************************************
*
* Summary:
*  Sends a notification about a descriptor change to the Client.
*
* Parameters:  
*   *sensorPtr: A pointer to the sensor characteristic structure.
*
*******************************************************************************/
void HandleNotificaion(CYBLE_ESS_CHARACTERISTIC_DATA_T *sensorPtr)
{
    CYBLE_API_RESULT_T apiResult;
    uint8 tmpBuff[CYBLE_ESS_2BYTES_LENGTH];

    /* Pack data to BLE compatible format ... */
    CyBle_Set16ByPtr(tmpBuff, sensorPtr->value);

    /* ... and send it */
    apiResult = CyBle_EsssSendNotification(connectionHandle,
                                           sensorPtr->EssChrIndex,
                                           sensorPtr->chrInstance,
                                           SIZE_2_BYTES,
                                           tmpBuff);

    if(apiResult != CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Send notification is failed: %d \r\n", apiResult);
    }
    else
    {   DBG_PRINTF("Notification for %s #%d was sent successfully. ", CharIndexToText(sensorPtr->EssChrIndex), sensorPtr->chrInstance + 1u);
        DBG_PRINTF("Notified value is: %d.%d m/s.\r\n", sensorPtr->value/100u, sensorPtr->value%100u);
        sensorPtr->sensorNewDataReady = NO;
        sensorPtr->ntfTimer = sensorPtr->ntfTimeoutVal;
    }
}


/*******************************************************************************
* Function Name: HandleNtfConditions()
********************************************************************************
*
* Summary:
*  Verifies if the conditions captured in ES Trigger descriptors and ES 
*  Configuration descriptor were met and returns a final verdict to inform if
*  notifications are allowed to be sent.
*
* Parameters:  
*   *sensorPtr: A pointer to the sensor characteristic structure.
*
* Return: 
*  The status identifying if all conditions for sending a notification for
*  True Speed #1 were met:
*    YES - All conditions were met.
*    NO - Not all conditions were met.
*
*******************************************************************************/
uint8 HandleNtfConditions(CYBLE_ESS_CHARACTERISTIC_DATA_T *sensorPtr)
{
    uint8 result = NO;
    uint8 isCondTrue[NUMBER_OF_TRIGGERS] = {NO, NO, NO};
    uint8 isCondInactive[NUMBER_OF_TRIGGERS] = {NO, NO, NO};
    uint8 i;

    if(sensorPtr->sensorNewDataReady == YES)
    {
        for (i = 0; i < NUMBER_OF_TRIGGERS; i++)
        {   
            switch (sensorPtr->valueCond[i])
            {
                case CYBLE_ESS_TRIG_USE_FIXED_TIME_INTERVAL:  /* FIXED_TIME work same as NO_LESS_THEN_TIME_INTERVAL  */ 
                case CYBLE_ESS_TRIG_NO_LESS_THEN_TIME_INTERVAL:
                    if((sensorPtr->ntfTimer == 0u) && (sensorPtr->sensorNewDataReady == YES))
                    {
                        isCondTrue[i] = YES;
                    }
                    break;
                    
                case CYBLE_ESS_TRIG_WHEN_CHANGED:
                    if(sensorPtr->value != sensorPtr->prevValue)
                    {
                        isCondTrue[i] = YES;
                    }
                    break;
                    
                case CYBLE_ESS_TRIG_WHILE_LESS_THAN:
                    if(sensorPtr->value < sensorPtr->cmpValue[i])
                    {
                        isCondTrue[i] = YES;
                    }
                    break;
                    
                case CYBLE_ESS_TRIG_WHILE_LESS_OR_EQUAL:
                    if(sensorPtr->value <= sensorPtr->cmpValue[i])
                    {
                        isCondTrue[i] = YES;
                    }
                    break;
                    
                case CYBLE_ESS_TRIG_WHILE_GREATER_THAN:
                    if(sensorPtr->value > sensorPtr->cmpValue[i])
                    {
                        isCondTrue[i] = YES;
                    }
                    break;
                    
                case CYBLE_ESS_TRIG_WHILE_GREATER_OR_EQUAL:
                    if(sensorPtr->value >= sensorPtr->cmpValue[i])
                    {
                        isCondTrue[i] = YES;
                    }
                    break;
                    
                case CYBLE_ESS_TRIG_WHILE_EQUAL_TO:
                    if(sensorPtr->value == sensorPtr->cmpValue[i])
                    {
                        isCondTrue[i] = YES;
                    }
                    break;
                    
                case CYBLE_ESS_TRIG_WHILE_EQUAL_NOT_TO:
                    if(sensorPtr->value != sensorPtr->cmpValue[i])
                    {
                        isCondTrue[i] = YES;
                    }
                    break;
                
                default:
                    isCondInactive[i] = YES;
                    break;    
            }
        }
    }

    /* All condition inactive or not present */
    if ( (isCondInactive[0] == YES)
      && (isCondInactive[1] == YES)  
      && (isCondInactive[2] == YES) )
    { 
        result = YES;
    }
    else        /* Condition connected wich boolean AND */
    {   if( sensorPtr->esConfig == CYBLE_ESS_CONF_BOOLEAN_AND )
        {   
           if( ((isCondInactive[0] == YES) || (isCondTrue[0] == YES))
            && ((isCondInactive[1] == YES) || (isCondTrue[1] == YES))
            && ((isCondInactive[2] == YES) || (isCondTrue[2] == YES)) ) 
            {
                result = YES;
            }
        }
        else 
        {   /* Condition connected wich boolean OR */
            if(  (isCondTrue[0] == YES)
              || (isCondTrue[1] == YES)
              || (isCondTrue[2] == YES) ) 
            {
                result = YES;
            }
        }
    }

    return(result);
}

/*******************************************************************************
* Function Name: HandleIndication()
********************************************************************************
*
* Summary:
*  Sends indication about a descriptor change to the Client.
*
* Parameters:  
*  flags: Descriptor value changed characteristic flags as per ESS spec.
*
*******************************************************************************/
void HandleIndication(uint16 flags)
{
    CYBLE_API_RESULT_T apiResult;
    ESS_DESCR_VAL_CHANGE_VALUE_T essValChanged;

    /* Pack data to BLE compatible format ... */
    CyBle_Set16ByPtr(&essValChanged.flags[0u], flags);
    CyBle_Set16ByPtr(&essValChanged.uuid[0u], CYBLE_UUID_CHAR_TRUE_WIND_SPEED);
    
    /* ... and send it */
    apiResult = CyBle_EsssSendIndication(connectionHandle,
                                         CYBLE_ESS_DESCRIPTOR_VALUE_CHANGED,
                                         0u,
                                         SIZE_4_BYTES,
                                         (uint8 *)&essValChanged);

    if(apiResult != CYBLE_ERROR_OK)
    {
        DBG_PRINTF("Send indication is failed: %d \r\n", apiResult);
    }
}


/*******************************************************************************
* Function Name: GetUint24()
********************************************************************************
*
* Summary:
*  Extracts three bytes value from the buffer send over the BLE radio and stores 
*  in uint32 variable.
*
* Parameters:  
*  u24Ptr[]: Three byte buffer that holds uint24 value. The buffer holds the 
*            lowest uint24 byte in the first element.
*  *u32:     Output parameter to store uint24 value.
*
*******************************************************************************/
void GetUint24(uint32 *u32, uint8 u24Ptr[])
{
    *u32 = ((uint32) ((uint32) u24Ptr[0u]) | ((uint32) (((uint32) u24Ptr[1u]) << 8u)) |
                                               ((uint32) (((uint32) u24Ptr[2u]) << 16u)));
}
/*******************************************************************************
* Function Name: ChkNtfAndSendData()
********************************************************************************
*
* Summary:
*   Check if there are notifications for parametr value and send them
*
* Parameters:  
*   *sensorPtr: A pointer to the sensor characteristic structure.
*
*******************************************************************************/
void ChkNtfAndSendData(CYBLE_ESS_CHARACTERISTIC_DATA_T *sensorPtr)
{
    if(HandleNtfConditions(sensorPtr) == YES)
    {
        if(sensorPtr->isNotificationEnabled == YES)
        {
            HandleNotificaion(sensorPtr);
            CyBle_ProcessEvents();
        }
    }
}

/*******************************************************************************
* Function Name: CharIndexToText()
********************************************************************************
*
* Summary:
*   Convert a ESS Characteristic Index to the character string
*
* Parameters:  
*   EssChrIndex : ESS Characteristic Index
*
* Return: 
*   A pointer to the character string contained a name of ESS Characteristic
*
*******************************************************************************/
char *CharIndexToText(CYBLE_ESS_CHAR_INDEX_T EssChrIndex)
{   
    char *ptrToMsg;
    
    switch (EssChrIndex)
    {   
        case CYBLE_ESS_DESCRIPTOR_VALUE_CHANGED :   
            ptrToMsg = (char *)"Descriptor Value Changed"; 
            break;
            
        case CYBLE_ESS_APPARENT_WIND_DIR :          
            ptrToMsg = (char *)"Apparent Wind Direction";
            break;
            
        case CYBLE_ESS_APPARENT_WIND_SPEED :        
            ptrToMsg = (char *)"Apparent Wind Speed";
            break;
            
        case CYBLE_ESS_DEW_POINT :                  
            ptrToMsg = (char *)"Dew Point";
            break;
            
        case CYBLE_ESS_ELEVATION :                  
            ptrToMsg = (char *)"Elevation";     
            break;
            
        case CYBLE_ESS_GUST_FACTOR :                
            ptrToMsg = (char *)"Gust Factor";     
            break;
            
        case CYBLE_ESS_HEAT_INDEX :                 
            ptrToMsg = (char *)"Heat Index";                  
            break;
            
        case CYBLE_ESS_HUMIDITY :                   
            ptrToMsg = (char *)"Humidity";                   
            break;
            
        case CYBLE_ESS_IRRADIANCE :                 
            ptrToMsg = (char *)"Irradiance";                     
            break;
            
        case CYBLE_ESS_POLLEN_CONCENTRATION :       
            ptrToMsg = (char *)"Pollen Concentration";
            break;
            
        case CYBLE_ESS_RAINFALL :                  
            ptrToMsg = (char *)"Rainfall";
            break;
            
        case CYBLE_ESS_PRESSURE :                   
            ptrToMsg = (char *)"Pressure";
            break;
            
        case CYBLE_ESS_TEMPERATURE :                
            ptrToMsg = (char *)"Temperature";
            break;
            
        case CYBLE_ESS_TRUE_WIND_DIR :              
            ptrToMsg = (char *)"True Wind Direction";
            break;
            
        case CYBLE_ESS_TRUE_WIND_SPEED :           
            ptrToMsg = (char *)"True Wind Speed";
            break;
            
        case CYBLE_ESS_UV_INDEX :                   
            ptrToMsg = (char *)"UV Index";
            break;
            
        case CYBLE_ESS_WIND_CHILL :                 
            ptrToMsg = (char *)"Wind Chill";
            break;
            
        case CYBLE_ESS_BAROMETRIC_PRESSURE_TREND :  
            ptrToMsg = (char *)"Barometric Pressure";
            break;
            
        case CYBLE_ESS_MAGNETIC_DECLINATION :       
            ptrToMsg = (char *)"Magnetic Declination";
            break;
            
        case CYBLE_ESS_MAGNETIC_FLUX_DENSITY_2D :   
            ptrToMsg = (char *)"Magnetic Flux Density 2D";
            break;
            
        case CYBLE_ESS_MAGNETIC_FLUX_DENSITY_3D :   
            ptrToMsg = (char *)"Magnetic Flux Density 3D";
            break;
            
        case CYBLE_ESS_CHAR_COUNT :                 
            ptrToMsg = (char *)"Total count of ESS";

        default :
            DBG_PRINTF("EssChrIndex is failed: %d \r\n", EssChrIndex);
            ptrToMsg = (char *)"NONE";
            break;
    }
    
    return ptrToMsg;
}
/* [] END OF FILE */
